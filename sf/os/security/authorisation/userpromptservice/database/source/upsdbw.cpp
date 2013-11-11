/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Implements a writable interface for UPS Database.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#include "upsdbw.h"
#include "upscommon.h"

using namespace UserPromptService;

CDecisionDbW::CDecisionDbW()
/**
	Constructor for writable decision database object
 */
	{
	
	}

	
CDecisionDbW::~CDecisionDbW()
/**
	Destructor for writable decision database object
 */
	{
	iDatabase.Close();
	delete iDbName;
	delete iStore;
	}


EXPORT_C CDecisionDbW* CDecisionDbW::NewL(const TDesC& aDbName, RFs& aFs)
/**
	Creates a writable decision database object and connects to the database.
	If the database does not exist or is corrupted, a new decision database is created.
	The function leaves, if creation of the object or connection to the database fail.
		
	@param	 aDbName The path of the decision database
	@param   aFs     Handle to the file server session
	
	@return  A pointer to the newly allocated database object, if creation and connection are successful. 
 */
	{
	CDecisionDbW* self = CDecisionDbW::NewLC(aDbName, aFs);
	CleanupStack::Pop(self);
	return self;
	}

	
EXPORT_C CDecisionDbW* CDecisionDbW::NewLC(const TDesC& aDbName, RFs& aFs)
/**
	Creates a writable decision database object and connects to the database.
	If the database does not exist or is corrupted, a new decision database is created.
	The function leaves, if creation of the object or connection to the database fail.
		
	@param	aDbName The path of the decision database
	param   aFs		Handle to the file server session
	
	@return A pointer to the newly allocated database object, if creation and connection 
			are successful. The pointer is also put onto the cleanup stack.
 */
	{
	CDecisionDbW* self = new (ELeave) CDecisionDbW();
	CleanupStack::PushL(self);
	self->ConstructL(aDbName, aFs);
	return self;
	}

	
void CDecisionDbW::ConstructL(const TDesC& aDbName, RFs& aFs)
/**
	Second phase constructor for the decision database object. Connects to the decision database.
	If the database does not existed or is corrupted, creates a new one. The function leaves,
	if both database connection and creation fail.
	
	@param aDbName The path of the decision database
	@param aFs		Handle to the file server session
 */
	{
	//Construct the name of UPS decision database
	iDbName = aDbName.AllocL();
	
	//First try to open the decision database
	TRAPD(error,OpenDatabaseL(aFs));
	
	if((error == KErrNoMemory) || (error == KErrDiskFull))
		{
		User::Leave(error);
		}
	
	// Makes sure that DB file is writable. (DEF122590)
	aFs.SetAtt(iDbName->Des(),0, KEntryAttReadOnly);
		
	if(error != KErrNone)
		{
		DEBUG_PRINTF2(_L("%S database file does not exist or is corrupted."), iDbName);
		//The decision database does not exist or is corrupted, create a new one
		CreateDatabaseL(aFs);
		
		//Create a decision table in the decision database
		CreateTableL();
		
		//Create an index on the decision table
		CreateIndexL();
		}
	}

	
void CDecisionDbW::OpenDatabaseL(RFs& aFs)
/**
	Opens the decision database.
	@param aFs	Handle to the file server session
 */
	{
	DEBUG_PRINTF2(_L("%S database file is being opened."), iDbName);
	
	iStore = CPermanentFileStore::OpenL(aFs, *iDbName, EFileRead|EFileWrite);
	iDatabase.OpenL(iStore, iStore->Root());
	
	//Database does exist. However, make sure that the decision table also exists
	CDbTableNames *tables = iDatabase.TableNamesL();
	CleanupStack::PushL(tables);
	if(1 != tables->Count())
		{
		DEBUG_PRINTF(_L("The decision table could not be found in the database file!"));
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(tables);
	
	//OK. The decision table does exist. What about the decision index?
	CDbIndexNames *indexes = iDatabase.IndexNamesL(KDecisionTable);
	CleanupStack::PushL(indexes);
	if(2 != indexes->Count())
		{
		DEBUG_PRINTF(_L("The index on the decision table is missing!"));
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(indexes);

	if(iDatabase.IsDamaged())
		{
		User::LeaveIfError(iDatabase.Recover());
		}
	
	DEBUG_PRINTF2(_L("%S database file has been opened successfully."), iDbName);
	}


void CDecisionDbW::CreateDatabaseL(RFs& aFs)
/**
	Creates a new, empty store database.
	@param aFs	Handle to the file server session
 */
	{
	DEBUG_PRINTF2(_L("%S database is being created."), iDbName);
	
	if(iStore)
		{//if database file exists but the table does not, iStore has already been allocated. 
		delete iStore;
		iStore = 0;
		iDatabase.Close();
		}

	//Create a file store
	iStore = CPermanentFileStore::ReplaceL(aFs, *iDbName, EFileRead|EFileWrite);
	iStore->SetTypeL(iStore->Layout());
	
	//Create UPS Decision Database in the file store
	TStreamId sId = iDatabase.CreateL(iStore);
	iStore->SetRootL(sId);
	
	//Commit the database creation
	iStore->CommitL();
	
	DEBUG_PRINTF2(_L("%S database has been created successfully."), iDbName);
	}


void CDecisionDbW::CreateTableL()
/**
	Creates the decision table in the decision database.
 */
	{
	DEBUG_PRINTF(_L("The Ups decision table is being created."));
	
	// Create a table definition
	CDbColSet* columns=CDbColSet::NewLC();
	
	// add the columns to the definition
	TDbCol clientSid(KColClientSid,EDbColUint32);
	clientSid.iAttributes = TDbCol::ENotNull;				
	columns->AddL(clientSid);
	
	TDbCol evaluatorId(KColEvaluatorId,EDbColUint32);
	evaluatorId.iAttributes = TDbCol::ENotNull;				
	columns->AddL(evaluatorId);
	
	TDbCol serviceId(KColServiceId,EDbColUint32);
	serviceId.iAttributes=TDbCol::ENotNull;
	columns->AddL(serviceId);
	
	TDbCol serverSid(KColServerSid,EDbColUint32);
	serverSid.iAttributes=TDbCol::ENotNull;
	columns->AddL(serverSid);
	
	TDbCol fingerprint(KColFingerprint,EDbColText8,KUpsMaxFingerprintLength );
	columns->AddL(fingerprint);
	
	TDbCol clientEntity(KColClientEntity,EDbColText8,KUpsMaxClientEntityLength);
	columns->AddL(clientEntity);
	
	TDbCol description(KColDescription,EDbColLongText);
	columns->AddL(description);
	
	TDbCol result(KColResult,EDbColInt8);
	result.iAttributes=TDbCol::ENotNull;
	columns->AddL(result);
	
	TDbCol evaluatorInfo(KColEvaluatorInfo,EDbColUint32);
	columns->AddL(evaluatorInfo);
	
	TDbCol policyVersion(KColMajorPolicyVersion,EDbColUint16);
	policyVersion.iAttributes=TDbCol::ENotNull;
	columns->AddL(policyVersion);
	
	TDbCol recordId(KColRecordId,EDbColUint32);
	recordId.iAttributes=TDbCol::ENotNull|TDbCol::EAutoIncrement;
	columns->AddL(recordId);
	
	// Create a table with the table definition
	User::LeaveIfError(iDatabase.CreateTable(KDecisionTable,*columns));
				
	// cleanup the column set
	CleanupStack::PopAndDestroy(columns);
	
	DEBUG_PRINTF(_L("The Ups decision table has been created successfully."));
	}


void CDecisionDbW::CreateIndexL()
/**
	Creates an index on the decision table and makes it unique.
 */
	{
	DEBUG_PRINTF(_L("The Ups decision index is being created on the decision table."));
	
	// create the index key
	CDbKey* key=CDbKey::NewLC();

	// add the key columns
	key->AddL(TDbKeyCol(KColClientSid));
	key->AddL(TDbKeyCol(KColEvaluatorId));
	key->AddL(TDbKeyCol(KColServiceId));
	key->AddL(TDbKeyCol(KColServerSid));
	key->AddL(TDbKeyCol(KColFingerprint));
	key->AddL(TDbKeyCol(KColClientEntity));
	key->AddL(TDbKeyCol(KColMajorPolicyVersion));
	
	//Make the index key unique
	key->MakeUnique();
	
	// create the primary index
	User::LeaveIfError(iDatabase.CreateIndex(KPrimaryIndex,KDecisionTable,*key));
	
	//Now create the second index on the record id
	key->Clear();
	// add the record id column
	key->AddL(TDbKeyCol(KColRecordId));
	key->MakeUnique();
	
	// create the record id index
	User::LeaveIfError(iDatabase.CreateIndex(KRecordIdIndex,KDecisionTable,*key));
	
	// cleanup the key
	CleanupStack::PopAndDestroy(key);
	
	DEBUG_PRINTF(_L("The Ups decision index has been created successfully."));
	}


EXPORT_C void CDecisionDbW::DeleteDatabaseL(RFs& aFs)
/**
	Delete the decision database completely.
	@param aFs	Handle to the file server session
 */
	{
	DEBUG_PRINTF2(_L("%S database is being deleted."),iDbName);
	
	iDatabase.Close();
	delete iStore;
	iStore = 0;
	User::LeaveIfError(aFs.Delete(*iDbName));
	
	DEBUG_PRINTF2(_L("%S database has been deleted successfully."),iDbName);
	}


void CDecisionDbW::UpdateCurrentRowL(RDbTable& aTable, CDecisionRecord& aRecord)
/**
	Updates the current row of the rowset by using the values the provided decision record.
	
	@param aTable  A table object which provides access to table data as a rowset.
	@param aRecord A decision record object used to update the current row.
 */
	{
	//Use CDbColSet to set fields
	CDbColSet* colSet = aTable.ColSetL();
	CleanupStack::PushL(colSet);
	
	//Set the fields of the empty row
	aTable.SetColL(colSet->ColNo(KColClientSid)	 ,(TUint32)aRecord.iClientSid.iId);
	aTable.SetColL(colSet->ColNo(KColEvaluatorId),(TUint32)aRecord.iEvaluatorId.iUid);
	aTable.SetColL(colSet->ColNo(KColServiceId)  ,(TUint32)aRecord.iServiceId.iUid);
	aTable.SetColL(colSet->ColNo(KColServerSid)	 ,(TUint32)aRecord.iServerSid.iId);
	
	//Fingerprint may be null
	if(aRecord.iFingerprint.Length() != 0)
		{
		aTable.SetColL(colSet->ColNo(KColFingerprint),aRecord.iFingerprint);
		}
	
	//ClientEntity may be null
	if(aRecord.iClientEntity.Length() != 0)
		{
		aTable.SetColL(colSet->ColNo(KColClientEntity),aRecord.iClientEntity);
		}
	
	//Description may be null	
	if(aRecord.iDescription.Length() != 0)
		{
		//A long column is written by using an RDbColStream
		RDbColWriteStream str;
		TDbColNo col = colSet->ColNo(KColDescription);
		
		str.OpenLC(aTable,col);
		str.WriteL(aRecord.iDescription);
		str.Close();
		
		CleanupStack::PopAndDestroy(&str);
		}
		
	aTable.SetColL(colSet->ColNo(KColResult),aRecord.iResult);
	aTable.SetColL(colSet->ColNo(KColEvaluatorInfo),(TUint32)aRecord.iEvaluatorInfo);
	aTable.SetColL(colSet->ColNo(KColMajorPolicyVersion),aRecord.iMajorPolicyVersion);
	
	CleanupStack::PopAndDestroy(colSet);
	}


EXPORT_C void CDecisionDbW::CreateDecisionL(CDecisionRecord& aRecord)
/**
	Inserts a new decision record into the decision database.
	
	@param aRecord A new decision record
 */
	{
	//Create a database table object
	RDbTable dbTable;
	User::LeaveIfError(dbTable.Open(iDatabase,KDecisionTable,dbTable.EInsertOnly));
	CleanupClosePushL(dbTable);
	
	//Set the rowset cursor to the beginning position
	dbTable.Reset();
	
	//Insert an empty row into the table
	dbTable.InsertL();
	
	//Set the fields of newly inserted row
	UpdateCurrentRowL(dbTable, aRecord);
	
	//Complete insertion
	dbTable.PutL();

	CleanupStack::Pop(&dbTable);
	//Close the rowset
	dbTable.Close();	 
	}


void CDecisionDbW::PrepareForSearchL(RDbTable& aTable, CDecisionFilter& aFilter, TDbSeekMultiKey<KIndexColumnNumber>& aSeekKey)
/**
	Opens the provided table object on the decision database and sets the decision index 
	as the active index for this table. If successful, the rowset is reset to the beginning.

	@param aTable  A table object which provides access to table data as a rowset.
	@param aFilter A filter object which is used to set the decision index.
	@return 	   The key value to lookup in the index.
 */
	{
	User::LeaveIfError(aTable.Open(iDatabase, KDecisionTable, aTable.EUpdatable));
	
	TUint16 flags = KSetClientSid|KSetEvaluatorId|KSetServiceId|KSetServerSid|KSetFingerprint|KSetClientEntity|KSetMajorPolicyVersion;
	
	TUint16 combinedFlags = (0x00FF) & (aFilter.iSetFlag[KPosClientSid]   | aFilter.iSetFlag[KPosEvaluatorId] |
						     			aFilter.iSetFlag[KPosServiceId]   | aFilter.iSetFlag[KPosServerSid]   |
						     			aFilter.iSetFlag[KPosFingerprint] | aFilter.iSetFlag[KPosClientEntity]|
						     			aFilter.iSetFlag[KPosMajorPolicyVersion]);

	//If any of these flags is not set, do not continue
	if ((combinedFlags & flags) != flags)
		{
		DEBUG_PRINTF(_L("The provided filter does not have all required keys to look up a decision."));
		User::Leave(KErrUpsMissingArgument);
		}
	
	aSeekKey.Add((TUint)aFilter.iClientSid.iId);
	aSeekKey.Add((TUint)aFilter.iEvaluatorId.iUid);
	aSeekKey.Add((TUint)aFilter.iServiceId.iUid);
	aSeekKey.Add((TUint)aFilter.iServerSid.iId);
	aSeekKey.Add(*aFilter.iFingerprint);
	aSeekKey.Add(*aFilter.iClientEntity);
	aSeekKey.Add(aFilter.iMajorPolicyVersion);
			
	//Set the primary index
	User::LeaveIfError(aTable.SetIndex(KPrimaryIndex));

	}


EXPORT_C CDecisionRecord* CDecisionDbW::GetDecisionL(CDecisionFilter& aFilter)
/**
	Gets a decision record from the decision database. The functions returns the first matched
	record. All the methods of the filter object except the client entity should be supplied to 
	get the intended decision record. If any other attribute of the filter object is missing, 
	either no decision might be found or a wrong decision may be retrieved
	
	@param aFilter A filter object whose client id, policy evaluator id, service id,
	 			   server id, fingerprint (and client entity) attributes are set.
 */
	{
	//Define a database table object handle
	RDbTable dbTable;
	CleanupClosePushL(dbTable);
	
	//Open the database table object and prepares it for searching
	TDbSeekMultiKey<KIndexColumnNumber> seekKey;
	PrepareForSearchL(dbTable,aFilter,seekKey);
	
	//Define the decision record that will be returned
	CDecisionRecord* retRecord(0);
	
	if(dbTable.SeekL(seekKey))
		{
		//Result found
		CDbColSet* colSet = dbTable.ColSetL();
		CleanupStack::PushL(colSet);
		
		dbTable.GetL();
		
		retRecord = CDecisionView::GenerateRecordL(dbTable,colSet);
		
		CleanupStack::PopAndDestroy(colSet);		
		}

	CleanupStack::PopAndDestroy(&dbTable);
	
	return retRecord;
	}


EXPORT_C TBool CDecisionDbW::UpdateDecisionL(CDecisionFilter& aFilter, CDecisionRecord& aNewRecord)
	{
	//Define a database table object handle
	RDbTable dbTable;
	CleanupClosePushL(dbTable);
	
	//Open the database table object and prepares it for searching
	TDbSeekMultiKey<KIndexColumnNumber> seekKey;
	PrepareForSearchL(dbTable,aFilter,seekKey);
	
	TBool retValue = ETrue; //return value
	
	if(dbTable.SeekL(seekKey))
		{
		//Get the current row
		dbTable.GetL();
		//Prepare it for update
		dbTable.UpdateL();
		//Set the new values
		UpdateCurrentRowL(dbTable, aNewRecord);
		//Commit the update
		dbTable.PutL();
		}
	else
		{
		retValue = EFalse;
		}
	
	CleanupStack::PopAndDestroy(&dbTable);
	
	return retValue;
	}
	

void CDecisionDbW::DoRemoveDecisionL(CDecisionFilter& aFilter)
/**
	Deletes a set of records from the decision database. This function is called 
	by RemoveDecisionsL in a loop to be able to catch and repair index corruptions.
	
	@param aFilter A filter object
 */
	{
	//Create the SQL statement
	RBuf sqlStatement;
	CreateSqlStatementLC(aFilter, sqlStatement);
						
	RDbView dbView;
	CleanupClosePushL(dbView);
	
	User::LeaveIfError(dbView.Prepare(iDatabase,TDbQuery(sqlStatement),TDbWindow::EUnlimited));
	User::LeaveIfError(dbView.EvaluateAll());

	//Begin the delete transaction and lock the database
	User::LeaveIfError(iDatabase.Begin());	
		
	while(dbView.NextL())
		{
		dbView.DeleteL();
		}
			
	CleanupStack::PopAndDestroy(&dbView);		
	CleanupStack::PopAndDestroy(&sqlStatement);
	
	//Release the lock
	TInt result = iDatabase.Commit();
	if(KErrNone != result)
		{
		DEBUG_PRINTF2(_L("Removing a decision has returned with %d. Rollback is now in proggress."),result);
		iDatabase.Rollback();
		User::Leave(result);
		}
	}

EXPORT_C void CDecisionDbW::RemoveDecisionsL(CDecisionFilter& aFilter)
/**
	Deletes a set of records from the decision database. It is possible to delete a specific
	decision record or a set of records based on a given filter. While this function is in
	progress, the database is locked not to lead to an inconsistent data retrieving. If there
	is any index corruption in the decision able, it is tried to be repaired automatically.
	
	@param aFilter A filter object
 */
	{
	TInt loopCount=1;
	TInt error;
	do
		{
		++loopCount;
		TRAP(error,DoRemoveDecisionL(aFilter));
		if(error == KErrCorrupt)
			{
			DEBUG_PRINTF2(_L("The database is corrupted and being recovered. Try %d"),loopCount);
			error = iDatabase.Recover(); //Recover() may fail, so update error
			}
		}while(error==KErrCorrupt && loopCount<2);
		
	User::LeaveIfError(error);
	}


EXPORT_C CDecisionDbCompactor *CDecisionDbW::PrepareCompactionLC()
/**
	Creates a database compaction object to perform asynchronous compaction on the database.
	
	@return A pointer to the newly created database compaction object
			The pointer is also put onto the cleanup stack.
 */
	{
	DEBUG_PRINTF(_L("The Ups database is being compacted."));
	CDecisionDbCompactor *compactor = CDecisionDbCompactor::NewLC();
	User::LeaveIfError(compactor->iDbIncremental.Compact(iDatabase,compactor->iStep()));
	return compactor;
	}

