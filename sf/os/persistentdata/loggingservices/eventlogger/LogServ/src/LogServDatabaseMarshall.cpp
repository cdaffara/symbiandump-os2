// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <logclientchangeobserver.h>
#include "LogServDatabaseMarshall.h"
#include "logservpanic.h"
#include "LogServCacheConfig.h"
#include "LogServCacheStrings.h"
#include "LogServCacheTypes.h"
#include "LogServBackupInterface.h"
#include "LogServResourceInterpreter.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServSqlStrings.h"
#include "LOGREPDEFS.H"


// Constants
const TInt KExpectedNumberOfTables = 4; // Should match the code in CreateTablesL

// Literal constants
_LIT(KLogDatabaseName,"Logdbu.dat");



/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServDatabaseMarshall (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServDatabaseMarshall::CLogServDatabaseMarshall(RFs& aFsSession, 
												   CLogServResourceInterpreter& aResourceInterface, 
												   MLogServBackupInterface& aBackupInterface)
:	iFsSession(aFsSession), iResourceInterface(aResourceInterface), iBackupInterface(aBackupInterface)
	{
	}

CLogServDatabaseMarshall::~CLogServDatabaseMarshall()
	{
	iBackupInterface.BIObserverRemove(*this);
	//
	delete iDatabaseName;
	delete iCacheStrings;
	delete iCacheTypes;
	delete iCacheConfig;
	delete iSecurity;
	delete iEventType;
	//
	iDatabase.Close();
	iStandardTypeUids.Close();
	}

void CLogServDatabaseMarshall::ConstructL()
	{
	iBackupInterface.BIObserverAddL(*this, MLogServBackupInterface::EObjectDatabaseMarshall);
	//
	iSecurity = CLogServSecurity::NewL(iResourceInterface);
	//
	iEventType = CLogEventType::NewL();
	
	DatabaseLocateL();
	DatabaseOpenL();
	RestoreStandardTypesL();
	
	iCacheTypes->CopyStandardTypeUidsL(iStandardTypeUids);
	}

CLogServDatabaseMarshall* CLogServDatabaseMarshall::NewL(RFs& aFsSession, 
														 CLogServResourceInterpreter& aResourceInterface, 
														 MLogServBackupInterface& aBackupInterface)
	{
	CLogServDatabaseMarshall* self = new(ELeave) CLogServDatabaseMarshall(aFsSession, aResourceInterface, aBackupInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

TInt CLogServDatabaseMarshall::DTIBegin()
	{
	const TInt backupError = iBackupInterface.BIErrorValueForCurrentState();
	if	(backupError != KErrNone)
		return backupError;

	__ASSERT_DEBUG(!iDatabase.InTransaction(), Panic(ELogBeginInTransaction));
	return iDatabase.Begin();
	}

TInt CLogServDatabaseMarshall::DTICommitAndEnd()
	{
	__ASSERT_DEBUG(iDatabase.InTransaction(), Panic(ELogCommitNotInTransaction));
	TInt err = iDatabase.Commit();
	if (err == KErrNone && iCacheStrings != NULL)
		{
		iCacheStrings->Commit();
		}
	return err;
	}

void CLogServDatabaseMarshall::DTIRollBack()
	{
	__ASSERT_DEBUG(iDatabase.InTransaction(), Panic(ELogRollbackNotInTransaction));
	iDatabase.Rollback();
	if (iCacheStrings != NULL)
		{
		iCacheStrings->Rollback();
		}
	}

TInt CLogServDatabaseMarshall::DTIExecuteSql(const TDesC& aStatement, TDbTextComparison aComparison)
	{
	return iDatabase.Execute(aStatement, aComparison);
	}

TBool CLogServDatabaseMarshall::DTIInTransaction() const
	{
	return iDatabase.InTransaction();
	}

TBool CLogServDatabaseMarshall::DTIDatabaseIsDamaged() const
	{
	return iDatabase.IsDamaged();
	}

CLogServResourceInterpreter& CLogServDatabaseMarshall::DTIResourceInterface() const
	{
	return iResourceInterface;
	}

MLogServDatabaseChangeInterface& CLogServDatabaseMarshall::DTIChangeInterface() const
	{
	__ASSERT_ALWAYS(iChangeInterface, Panic(ELogNoChangeInterfacePointer));
	return *iChangeInterface;
	}

CLogServCacheStrings& CLogServDatabaseMarshall::DTICacheStrings() const
	{
	__ASSERT_ALWAYS(iCacheStrings, Panic(ELogCacheAccessDuringBackupStrings));
	return *iCacheStrings;
	}

CLogServCacheTypes& CLogServDatabaseMarshall::DTICacheTypes() const
	{
	__ASSERT_ALWAYS(iCacheTypes, Panic(ELogCacheAccessDuringBackupTypes));
	return *iCacheTypes;
	}

CLogServCacheConfig& CLogServDatabaseMarshall::DTICacheConfig() const
	{
	__ASSERT_ALWAYS(iCacheConfig, Panic(ELogCacheAccessDuringBackupConfig));
	return *iCacheConfig;
	}

RDbDatabase& CLogServDatabaseMarshall::DTIDatabase()
	{
	return iDatabase;
	}

TBool CLogServDatabaseMarshall::DTIIsAllowed(TEventOp aEventOp, const RMessage2& aMessage, TUid aEventType, const char* aDiagnostic) const
	{
	return iSecurity->IsAllowed(aMessage, aEventType, aEventOp, aDiagnostic);
	}
	
const RArray<TUid>& CLogServDatabaseMarshall::DTIUidsOfStandardTypes()
	{
	return iStandardTypeUids;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseMarshall::BOHandleEventL(TLogServBackupEvent aEvent)
	{
	switch(aEvent)
		{
		case EBackupStarting:
			{
			//Destroy config, types and strings caches
			delete iCacheConfig;
			iCacheConfig = NULL;
			delete iCacheStrings;
			iCacheStrings = NULL;
			delete iCacheTypes;
			iCacheTypes = NULL;
			//Close the database
			iDatabase.Close();
			}
			break;
			
		case EBackupEnded:
			{
			// Re-open the database and create config, types and strings caches 
			DatabaseOpenL();
			// reset views as a different database is being restored
			DTIChangeInterface().DCISubmitGlobalChangeContextL(KLogClientChangeEventRefreshView);
			}
			break;
		default:
			break;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServDatabaseMarshall::DatabaseLocateL()
	{
	// Get drive for database
	TDriveUnit driveUnit(static_cast<TInt>(RFs::GetSystemDrive()));
	TDriveName name(driveUnit.Name());
	
	TFileName path;
	iFsSession.PrivatePath(path);
	
	// Ensure database path exists
	TParse parse;
	User::LeaveIfError(parse.Set(path, &name, NULL));
	path = parse.FullName();
	
	TInt error = iFsSession.MkDirAll(path);
	if	(error != KErrAlreadyExists)
		User::LeaveIfError(error);
	
	path += KLogDatabaseName;
	iDatabaseName = path.AllocL();
	}

/**
Opens the LogEng database.
@return KErrNone,    If the "database open" operation completes successfully.
                     If the "database open" operation fails the function returns the repported error code. 
        KErrCorrupt, If the database is opened successfully but is damaged, then the function returns KErrCorrupt.
*/
TInt CLogServDatabaseMarshall::DoDbOpen()
    {
    LOGTEXT2("CLogServDatabaseMarshall::DatabaseOpenL() - attempting to open db: %S", iDatabaseName);
#ifdef LOGGING_ENABLED
    TEntry entry;
    if  (iFsSession.Entry(*iDatabaseName, entry) == KErrNone)
        {
        LOGTEXT2("CLogServDatabaseMarshall::DatabaseOpenL() - Database file: %S DOES exist", iDatabaseName);        
        }
    else
        {
        LOGTEXT2("CLogServDatabaseMarshall::DatabaseOpenL() - Database file: %S NOT FOUND", iDatabaseName);     
        }
#endif
    // Open database
    TInt err = iDatabase.Open(iFsSession, DatabaseName());
    LOGTEXT2("CLogServDatabaseMarshall::DatabaseOpenL() - attempting to open DMBS database resulted in error: %d", error);
    // Check if the database is damaged. If it is set the error to KErrCorrupt so that it 
    // will be deleted.
    if ((err == KErrNone) && iDatabase.IsDamaged()) 
        {
        err =  KErrCorrupt;
        }
    return err;
    }

/**
Check if the database table count is the expected one -  KExpectedNumberOfTables.
@return True,   The database tables count is KExpectedNumberOfTables,
        False,  The database tables count is not KExpectedNumberOfTables;
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors or 
                      other system-wide error codes.
*/
TBool CLogServDatabaseMarshall::DbTableCntCheckL()
    {
    CDbTableNames* tables = iDatabase.TableNamesL();
    TInt numberOfTables = tables->Count();
    delete tables;
    LOGTEXT2("CLogServDatabaseMarshall::DatabaseOpenL() - Number of tables: %d", numberOfTables);
    return numberOfTables == KExpectedNumberOfTables;
    }

/**
Alters the "Event" table if the number column length is not KLogMaxNumberLength.
@return KErrNone, The "alter" operation has completed successfully, system wide or database specific error code otherwise.  
*/
TInt CLogServDatabaseMarshall::AlterEventTblIfOldFmt(CDbColSet& aEventTblColSet)
    {
    const TDbCol* numberCol = aEventTblColSet.Col(KLogFieldEventNumberString);
    __ASSERT_DEBUG(numberCol != NULL, User::Invariant());
    TInt err = KErrNone;
    // check the column width is correct
    if(numberCol->iMaxLength != KLogMaxNumberLength)
        {
        //The column width is not correct, so this is an old format database.
        //Modify the database so the number length is KLogMaxNumberLength.
        (const_cast <TDbCol*> (numberCol))->iMaxLength = KLogMaxNumberLength;
        err = iDatabase.AlterTable(KLogNameEventString, aEventTblColSet);
        }
    return err;
    }

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	

/**
Alters the "Event" table if the the table does not have "SimId" column.
@return KErrNone, The "alter" operation has completed successfully, system wide or database specific error code otherwise.  
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Some other failure codes, not related to the "alter" opertaion.
*/
TInt CLogServDatabaseMarshall::AlterEventTblIfNoSimIdL(CDbColSet& aEventTblColSet)
    {//Compiled only when SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM macro is defined
    const TDbCol* simIdCol = aEventTblColSet.Col(KLogFieldEventSimId);
    TInt err = KErrNone;
    if(!simIdCol)
        {
        TDbCol col(KLogFieldEventSimId, EDbColUint32);
        aEventTblColSet.AddL(col);
        err = iDatabase.AlterTable(KLogNameEventString, aEventTblColSet);
        }
    return err;
    }

#endif

/**
Checks the database structure and alters the tables if that's an old format database.
@return KErrNone, The "alter" operation has completed successfully, system wide or database specific error code otherwise.
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Some other failure codes, not related to the "alter" opertaion.
*/
TInt CLogServDatabaseMarshall::AlterDbIfOldFmtL()
    {
    CDbColSet* tableEventCol = iDatabase.ColSetL(KLogNameEventString);
    CleanupStack::PushL(tableEventCol);
    //Check for old format database which had MaxNumberLength =32
    TInt err = AlterEventTblIfOldFmt(*tableEventCol);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
    //Check if the "SimId" column is present
    if(err == KErrNone)
        {
        err = AlterEventTblIfNoSimIdL(*tableEventCol);
        }
#endif    
    CleanupStack::PopAndDestroy(tableEventCol);
    return err;
    }

/**
Closes and deletes the LogEng database. In _DEBUG builds the "delete file" error will be printed out. 
*/
void CLogServDatabaseMarshall::DbDelete()
    {
    iDatabase.Close();
#ifdef _DEBUG       
	//Do not remove the statement bellow. In _DEBUG builds it forms a single "TInt err2 = iFsSession.Delete(DatabaseName());" statement.
    TInt err2 =
#endif      
    iFsSession.Delete(DatabaseName());
#ifdef _DEBUG
    if((err2 != KErrNone) && (err2 != KErrNotFound))
        {
        RDebug::Print(_L("CLogServDatabaseMarshall::DatabaseOpenL() - Failed to delete file. Error = %d"), err2);
        }
#endif
    }

/**
Attempts to create the LogEng database and tables.
@return   KErrNoNone, The database was created successfully, system wide or database specific error otherwise.
*/
TInt CLogServDatabaseMarshall::DbCreate()
    {
    // Try and create the database
    LOGTEXT("CLogServDatabaseMarshall::DatabaseOpenL() - trying to create database");
    TRAPD(err, DatabaseCreateL(DatabaseName()));
    LOGTEXT2("CLogServDatabaseMarshall::DatabaseOpenL() - creation error was: %d", error);
    return err;
    }

/**
Creates config and string LogEng caches. Finishes the initialization of the event types cache. 
@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors or 
                      other system-wide error codes.
*/
void CLogServDatabaseMarshall::CreateCachesL()
    {
    // Create other cache objects (these require the database to be already opened)
    LOGTEXT("CLogServDatabaseMarshall::DatabaseOpenL() - creating config cache");
    iCacheConfig = CLogServCacheConfig::NewL(*this);
    LOGTEXT("CLogServDatabaseMarshall::DatabaseOpenL() - creating string cache");
    iCacheStrings = CLogServCacheStrings::NewL(*this);
    // Finish the type cache initialization
    LOGTEXT("CLogServDatabaseMarshall::DatabaseOpenL() - initializing type cache");
    iCacheTypes->InitializeL();
    }

void CLogServDatabaseMarshall::DatabaseOpenL()
	{
	// Create the cache objects - objects need to be put into the cache as
	// soon as the database is open.
	LOGTEXT("CLogServDatabaseMarshall::DatabaseOpenL() - creating type cache");
	iCacheTypes = CLogServCacheTypes::NewL(*this);
	TInt err = DoDbOpen();
	// Check we have the expected number of tables
	if(err == KErrNone && !DbTableCntCheckL())
		{
	    err = KErrCorrupt;
		}
    // Check a compaction can be performed. If it can't it indicates a serious problem?
    if(err == KErrNone && (err = iDatabase.Compact()) != KErrNone)
       {
       err = KErrCorrupt;
       }
	if(err == KErrNone)
		{
		err = CLogServDatabaseMarshall::AlterDbIfOldFmtL();
		}
	// If the database failed to open, delete and recreate.
	if(err == KErrNotFound || err == KErrCorrupt || err == KErrArgument || err == KErrEof)
		{
		DbDelete();
		// Try and create the database
		err = DbCreate();
	    if(err != KErrNone)
	        {
	        DbDelete();
	        }
		}
	User::LeaveIfError(err);
	CreateCachesL();
    // At this point, its safe to tell the backup interface what file it has to watch
    LOGTEXT("CLogServDatabaseMarshall::DatabaseOpenL() - registering database filename with backup interface");
    iBackupInterface.BISetDatabaseNameL(DatabaseName());
	LOGTEXT("CLogServDatabaseMarshall::DatabaseOpenL() - end");
	}

void CLogServDatabaseMarshall::DatabaseCreateL(const TDesC& aName)
	{
	User::LeaveIfError(iDatabase.Replace(iFsSession, aName));
	CreateTablesL();
	}

// Note: Number of tables HAS to match KExpectedNumberOfTables
void CLogServDatabaseMarshall::CreateTablesL()
	{
	DTIBeginWithRollBackProtectionLC();

	// Create event table
	TheSql.Format(KLogTableEventString, KLogMaxRemotePartyLength, KLogMaxSubjectLength, KLogMaxNumberLength);
	User::LeaveIfError(iDatabase.Execute(TheSql));
	MakeColumnAutoIncremetingL(KLogNameEventString, KLogFieldIdString);

	// Create event type table
	TheSql.Format(KLogTableTypeString, KLogMaxDescriptionLength);
	User::LeaveIfError(iDatabase.Execute(TheSql));
	MakeColumnAutoIncremetingL(KLogNameTypeString, KLogFieldIdString);

	// Create string list table
	TheSql.Format(KLogTableStringString, KLogMaxSharedStringLength);
	User::LeaveIfError(iDatabase.Execute(TheSql));
	MakeColumnAutoIncremetingL(KLogNameStringString, KLogFieldIdString);

	// Create configuration table
	TheSql.Copy(KLogTableConfigString);
	User::LeaveIfError(iDatabase.Execute(TheSql));

	// Create the index
	CreateIndiciesL();

	// Set the initial configuration
	CreateConfigurationL();

	// Load standard event types
	CreateTypesL();

	DTICommitAndCancelRollbackProtectionL();
	}

void CLogServDatabaseMarshall::CreateTypesL(TBool aReadOnly)
	{
	// Get the array size
	TResourceReader reader;
	iResourceInterface.CreateResourceReaderLC(reader, R_LOG_INITIAL_EVENTS);

	// Create them
	DTICacheTypes().CreateStandardTypesL(reader, aReadOnly);

	CleanupStack::PopAndDestroy(); // reader
	}

void CLogServDatabaseMarshall::CreateIndiciesL()
	{
	// Get the array size
	TResourceReader reader;
	iResourceInterface.CreateResourceReaderLC(reader, R_LOG_INDEXES);

	const TInt indexes = reader.ReadInt16();

	// Read in the array
	for(TInt c1 = 0; c1 < indexes; c1++)
		{
		const TPtrC name(reader.ReadTPtrC());
		const TPtrC table(reader.ReadTPtrC());

		// Get the number of keys
		const TInt keys = reader.ReadInt16();

		CDbKey* key = CDbKey::NewLC();

		for(TInt c2 = 0; c2 < keys; c2++)
			{
			TPtrC col = reader.ReadTPtrC();
			TUint order = reader.ReadUint16();
			TInt len = reader.ReadInt16();

			// Add the key
			key->AddL(TDbKeyCol(col, len, (TDbKeyCol::TOrder)order));
			}

		// Make key unique if required
		if (reader.ReadInt8())
			key->MakeUnique();

		// Set comparison
		const TDbTextComparison comparison = static_cast<TDbTextComparison>(reader.ReadInt8());
		key->SetComparison(comparison);

		// Create the index
		User::LeaveIfError(iDatabase.CreateIndex(name, table, *key));

		CleanupStack::PopAndDestroy(key);
		}

	CleanupStack::PopAndDestroy(); // reader
	}

void CLogServDatabaseMarshall::CreateConfigurationL()
	{
	// Load the resource/repository file default configuration
	// The precedence is given to the reading from the repository file.
   	TLogConfig config;
	
	CRepository* repository = NULL;
	TRAPD(res, repository = CRepository::NewL(KUidLogengRepository));		
	if (res == KErrNone)
		{
		CleanupStack::PushL(repository);
		ReadRepositoryFileConfigurationL(config, *repository);
		CleanupStack::PopAndDestroy(repository);
		}
	else if (res == KErrCorrupt)
		{
		User::Leave(res);
		}
	else
		{
		ReadResourceFileConfigurationL(config);
		}	
   	// Insert the column
   	TheSql.Format(KLogSqlInsertConfigString, config.iMaxLogSize, config.iMaxRecentLogSize, config.iMaxEventAge);
	User::LeaveIfError(iDatabase.Execute(TheSql));
	}

void CLogServDatabaseMarshall::ReadRepositoryFileConfigurationL(TLogConfig& aConfig, CRepository& repository) const
	{
	TInt maxLogSize;
	TInt maxRecentLogSize;
	TInt maxEventAge;
	//	
	User::LeaveIfError(repository.Get(KMaxLogSizeRepKey, maxLogSize));
	aConfig.iMaxLogSize = static_cast<TLogSize> (maxLogSize);
	User::LeaveIfError(repository.Get(KMaxRecentLogSizeRepKey, maxRecentLogSize));
	aConfig.iMaxRecentLogSize = static_cast<TLogRecentSize> (maxRecentLogSize);
	User::LeaveIfError(repository.Get(KMaxEventAgeRepKey, maxEventAge));
	aConfig.iMaxEventAge = static_cast<TLogAge> (maxEventAge);	
	}

void CLogServDatabaseMarshall::ReadResourceFileConfigurationL(TLogConfig& aConfig) const
	{	
	TResourceReader reader;
	iResourceInterface.CreateResourceReaderLC(reader, R_LOG_INITIAL_CONFIG);
	//
	aConfig.iMaxLogSize = static_cast<TLogSize>(reader.ReadUint16());
	aConfig.iMaxRecentLogSize = static_cast<TLogRecentSize>(reader.ReadUint8());
	aConfig.iMaxEventAge = static_cast<TLogAge>(reader.ReadUint32());
	//
	CleanupStack::PopAndDestroy(); // reader	
	}

void CLogServDatabaseMarshall::MakeColumnAutoIncremetingL(const TDesC& aTable, const TDesC& aColumn)
	{
	CDbColSet* newTable = iDatabase.ColSetL(aTable);
	CleanupStack::PushL(newTable);

	const TDbCol* oldCol = newTable->Col(aColumn);
	__ASSERT_DEBUG(oldCol != NULL, Panic(ELogNoSuchColumn));

	TDbCol newCol = *oldCol;
	newCol.iAttributes |= TDbCol::EAutoIncrement;

	newTable->Remove(aColumn);
	newTable->AddL(newCol);

	User::LeaveIfError(iDatabase.DropTable(aTable));
	User::LeaveIfError(iDatabase.CreateTable(aTable, *newTable));
		
	CleanupStack::PopAndDestroy(newTable);
	}
	
