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
* Implements a read-only interface for UPS Database.
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


//
//CDecisionView
//

CDecisionView::CDecisionView():CActive(EPriorityStandard)
/**
	Constructor for decision view object.
 */
	{
	CActiveScheduler::Add(this);	
	}



CDecisionView::~CDecisionView()
/**
	Destructor for the decision view object.
 */
	{
	iDbView.Close();
	delete iColSet;
	Deque();
	}


EXPORT_C CDecisionView* CDecisionView::NewLC()
/**
	Creates a decision view object. The function leaves, if creation of the view object fails.
	
	@return A pointer to the newly allocated view object, if creation is successful.
	        The pointer is also put onto the cleanup stack.
 */
	{
	CDecisionView* self = new(ELeave) CDecisionView();
	CleanupStack::PushL(self);
	return self;
	}


void CDecisionView::DoCancel()
//From CActive
	{
	iDbView.Cancel();
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}	
	}


TInt CDecisionView::RunError(TInt aError)
//From CActive
	{
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	return KErrNone;
	}


void CDecisionView::RunL() 
//From CActive
	{
	TInt status = iStatus.Int();
	User::LeaveIfError(status);
	
	if(status > 0)
		{
		iDbView.Evaluate(iStatus);
		SetActive();
		}
	else
		{
		User::RequestComplete(iClientStatus,status);
		}
	}
	
		
EXPORT_C void CDecisionView::EvaluateView(TRequestStatus& aStatus)
/**
	Performs all steps of the view evaluation, returning immediately and 
	signalling when all steps are complete. 
	
	@param aStatus The request status used to contain completion information for the function.
	               On completion, the status value should be interpreted as follows: 
	               0, evaluation is complete.< 0, an error code.
 */
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDecisionViewPanic,KErrInUse));	
	
	aStatus = KRequestPending;
	iClientStatus = &aStatus;	
	
	iDbView.Evaluate(iStatus);
	
	SetActive();
	
	}


CDecisionRecord* CDecisionView::GenerateRecordL(RDbRowSet& aRowSet, CDbColSet* aColSet)
/**
	Retrieves all decision record fileds from the given rowset and generates
	a decision record from these information.
	
	@param aRowSet A database rowset object.
	@param aColSet A database column set object.
	
	@return The newly,created decision record if it is successful; otherwise, leaves.
 */
	{
	TSecureId clientSid(aRowSet.ColUint32(aColSet->ColNo(KColClientSid))); 			 //Client Secure Id
	TSecureId serverId = TSecureId(aRowSet.ColUint32(aColSet->ColNo(KColServerSid))); //Service Secure Id
	TUid evaluatorId = TUid::Uid(aRowSet.ColUint32(aColSet->ColNo(KColEvaluatorId))); //Evaluator Uid
	TUid serviceId = TUid::Uid(aRowSet.ColUint32(aColSet->ColNo(KColServiceId)));	 //Service Uid
	TPtrC8 fingerprint = aRowSet.ColDes8(aColSet->ColNo(KColFingerprint));			 //Fingerprint
	TPtrC8 clientEntity = aRowSet.ColDes8(aColSet->ColNo(KColClientEntity));			 //Client Entity
	TInt8 result = aRowSet.ColInt8(aColSet->ColNo(KColResult));						 //Result
	TUint16 ver = aRowSet.ColUint16(aColSet->ColNo(KColMajorPolicyVersion));			 //Major Policy Version
	TUint32 recordId =  aRowSet.ColUint32(aColSet->ColNo(KColRecordId));				 //Unique Record ID
	TUint32 evaluatorInfo = aRowSet.ColUint32(aColSet->ColNo(KColEvaluatorInfo));     //EvaluatorInfo
		
	//a stream is used to read long columns
	TDbColNo col = aColSet->ColNo(KColDescription);
	TInt len = aRowSet.ColLength(col);
		
	RBuf description;
	CleanupClosePushL(description);
	description.CreateL(len);
		
	RDbColReadStream str;
	str.OpenLC(aRowSet,col);
	str.ReadL(description,len);
	str.Close();
	CleanupStack::PopAndDestroy(&str);
		
	CDecisionRecord *retval = CDecisionRecord::NewL(clientSid,evaluatorId,serviceId,serverId,fingerprint,
								 clientEntity,description,result,ver,evaluatorInfo,recordId);
	
	CleanupStack::PopAndDestroy(&description);
	return retval;
	}
	
		
EXPORT_C CDecisionRecord* CDecisionView::NextDecisionL()
/** 
	Moves the cursor to the next row in the view object and returns the current decision row.
 	If there are no more rows, the cursor is positioned to the end of the view object.
 	
 	@return 0, If the cursor is positioned at the end of the view. Otherwise, current record. 
 */
	{
	CDecisionRecord *record(0);
	
	if(iDbView.NextL())
		{
		iDbView.GetL();
		record = GenerateRecordL(iDbView,iColSet);
		}	
	else
		{
		iDbView.Close();
		}
		
	return record;
	}


//
//CDecisionDb
//


CDecisionDb::CDecisionDb()
/**
	Constructor for read-only decision database object
 */
	{
	
	}


CDecisionDb::~CDecisionDb()
/**
	Destructor for read-only decision database object
 */
	{
	
	}

const TDesC* CDecisionDb::GetComparisonOperator(const TUint32& aFlag) const
/**
	Finds and returns the correspondent comparision operator string.
	
	@param aFlag 
 */
	{
	UserPromptService::TComparisonOp op = static_cast<UserPromptService::TComparisonOp>(aFlag & 0xFFFF0000);
	switch (op)
		{
		case UserPromptService::EEqual:
			{
			return &KSQLQueryEqual;
			}
		case UserPromptService::ENotEqual:
			{
			return &KSQLQueryNotEqual;
			}
		case UserPromptService::ELessThan:
			{
			return &KSQLQueryLessThan;
			}
		case UserPromptService::EGreaterThan:
			{
			return &KSQLQueryGreaterThan;
			}
		case UserPromptService::ELessThanOrEqual:
			{
			return &KSQLQueryLessThanOrEqual;
			}
		case UserPromptService::EGreaterThanOrEqual:
			{
			return &KSQLQueryGreaterThanOrEqual;
			}
		}
	return &KSQLQueryEqual;
	}


void CDecisionDb::CreateSqlStatementLC(CDecisionFilter& aFilter, RBuf& aSql)
/**
	Creates an sql statement from a given filter.

    @param aFilter A filter object used to create the SQL statement.
    @param aSql    The newly created SQL statement.
 */
	{
	//Estimate the maximum required buffer (maxLen) to save the sql statement.
	//KSQLQueryBase must be in the statement at least.
	TInt maxLen = KBaseSQLLength;
	
	//Get the bits belonging to ID values (ClientSid,EvaluatorId,ServiceId,ServerSid)
	TUint32 combinedFlags = (aFilter.iSetFlag[KPosClientSid] | aFilter.iSetFlag[KPosEvaluatorId]   |
						 	 aFilter.iSetFlag[KPosServiceId] | aFilter.iSetFlag[KPosServerSid]     |
						 	 aFilter.iSetFlag[KPosFingerprint] | aFilter.iSetFlag[KPosClientEntity]|
						 	 aFilter.iSetFlag[KPosMajorPolicyVersion] | aFilter.iSetFlag[KPosRecordId] |
						 	 aFilter.iSetFlag[KPosDescription] | aFilter.iSetFlag[KPosResult] | 
						 	 aFilter.iSetFlag[KPosEvaluatorInfo]);
						 	
	TUint16 flag = combinedFlags & 0x000006CF;
	//Count how many bits are set, and add KMaxIntCondition for each set bit
	while(flag != 0)
		{
		flag &= (flag-1);
		maxLen += KMaxIntCondition;
		}
	//Get the bits belonging to string values (Fingerprint and ClientEntity)
	flag = combinedFlags & 0x00000130;
	//Count how many bits are set, and add KMaxStringCondition length for each set bit
	while(flag != 0)
		{
		flag &= (flag-1);
		maxLen += KMaxStringCondition;
		}
	
	//Allocate the estimated number of bytes for the sql statement
	aSql.CreateL(maxLen);
	aSql.CleanupClosePushL();
	//First copy the basic SQL statement
	aSql.AppendFormat(KSQLQueryBase,&KDecisionTable);
	
	//If the filter is empty, there is no condition to add to the sql statement.
	//So, simply return the base sql statement. In this case, all rows of
	//the decision table are listed.
	if(maxLen == KBaseSQLLength)
		{
		return;	
		}
	
	//Filter is not empty, add 'WHERE' key word into the statement	
	aSql.Append(KSQLQueryWhere);
		
	TBool multiple = EFalse;
	//Append server SID if exists in the filter
	AppendQueryInteger(aFilter.iServerSid.iId, aSql, multiple, aFilter.iSetFlag[KPosServerSid], KSetServerSid, KColServerSid);
	//Append service UID if exists in the filter
	AppendQueryInteger(aFilter.iServiceId.iUid, aSql, multiple, aFilter.iSetFlag[KPosServiceId], KSetServiceId, KColServiceId);
	//Append client SID if exists in the filter
	AppendQueryInteger(aFilter.iClientSid.iId, aSql, multiple, aFilter.iSetFlag[KPosClientSid], KSetClientSid, KColClientSid);		
	//Append evaluator UID if exists in the filter
	AppendQueryInteger(aFilter.iEvaluatorId.iUid, aSql, multiple, aFilter.iSetFlag[KPosEvaluatorId], KSetEvaluatorId, KColEvaluatorId);				
	//Append major policy version if exists in the filter
	AppendQueryInteger(aFilter.iMajorPolicyVersion, aSql, multiple, aFilter.iSetFlag[KPosMajorPolicyVersion], KSetMajorPolicyVersion, KColMajorPolicyVersion);
	//Append record id if exists in the filter
	AppendQueryInteger(aFilter.iRecordId, aSql, multiple, aFilter.iSetFlag[KPosRecordId], KSetRecordId, KColRecordId);		
	//Append result if exists in the filter
	AppendQueryInteger(aFilter.iResult, aSql, multiple, aFilter.iSetFlag[KPosResult], KSetResult, KColResult);		
	//Append evaluator info if exists in the filter
	AppendQueryInteger(aFilter.iEvaluatorInfo, aSql, multiple, aFilter.iSetFlag[KPosEvaluatorInfo], KSetEvaluatorInfo, KColEvaluatorInfo);		
	//Append description if exists in the filter
	AppendQueryStringL(*aFilter.iDescription, aSql, multiple, aFilter.iSetFlag[KPosDescription], KSetDescription, KColDescription);
	
	//Append fingerprint if exists in the filter	
	//Convert 8-bit value to 16-bit
	if(aFilter.iFingerprint)
		{
		HBufC* fp = HBufC::NewLC(aFilter.iFingerprint->Length());
		TPtr ptrFp(fp->Des());
		ptrFp.Copy(*aFilter.iFingerprint);
	
		AppendQueryStringL(*fp, aSql, multiple, aFilter.iSetFlag[KPosFingerprint], KSetFingerprint, KColFingerprint);
		CleanupStack::PopAndDestroy(fp);
		}
	
	//Append client entity if exists in the filter
	//Convert 8-bit value to 16-bit 
	if(aFilter.iClientEntity)
		{
		HBufC* ce = HBufC::NewLC(aFilter.iClientEntity->Length());
		TPtr ptrCe(ce->Des());
		ptrCe.Copy(*aFilter.iClientEntity);
		
		AppendQueryStringL(*ce, aSql, multiple, aFilter.iSetFlag[KPosClientEntity], KSetClientEntity, KColClientEntity);		
		CleanupStack::PopAndDestroy(ce);
		}
	}


void CDecisionDb::AppendQueryInteger(const TUint32& aValue, TDes& aSql, TBool& aMultiple, const TUint32& aFlag, const TUint32& aSetCol, const TDesC& aColName)
/**
	Appends an integer-based query key field to an SQL query string.
	
	@param aValue 	 The integer value which will be added to the query.
	@param aSql   	 The SQL query string.
	@param aMultiple EFalse, if there is only one query key in the query string. Otherwise, ETrue.
	@param aFlag 	 The flag value corresponding to the query key.
	@param aSetCol 	 A constant value indicating whether or not a specific column is set. 
	@param aColName  The column name of the query key.
 */
	{
	if(aFlag & aSetCol)
		{
		if(aMultiple)
			{
			aSql.Append(KSQLQueryAnd);
			}
		else
			{
			aMultiple=ETrue;
			}
			
		aSql.Append(aColName);
		aSql.AppendFormat(KSQLQueryString,GetComparisonOperator(aFlag));	
		aSql.AppendFormat(KSQLQueryConditionInt,aValue);
		}
	}
	

void CDecisionDb::AppendQueryStringL(const TDesC& aValue, TDes& aSql, TBool& aMultiple, const TUint32& aFlag, const TUint32& aSetCol, const TDesC& aColName)
/**
	Appends an string query key to an SQL query string.
	
	@param aValue 	 The string value which will be added to the query.
	@param aSql   	 The SQL query string.
	@param aMultiple EFalse, if there is only one query key in the query string. Otherwise, ETrue.
	@param aFlag 	 The flag value corresponding to the query key.
	@param aSetCol 	 A constant value indicating whether or not a specific column is set. 
	@param aColName  The column name of the query key.
 */
	{
	if(aFlag & aSetCol)
		{
		if(aMultiple)
			{
			aSql.Append(KSQLQueryAnd);
			}
		else
			{
			aMultiple=ETrue;
			}
		
		aSql.Append(aColName);
		
		if(aValue.Length() != 0)
			{
			//Scan for single quote
			RBuf dbValue;
			DoubleSingleQuotesL(aValue, dbValue);
			dbValue.CleanupClosePushL();
						
			aSql.AppendFormat(KSQLQueryString,GetComparisonOperator(aFlag));	
			aSql.AppendFormat(KSQLQueryConditionString,&dbValue);
			
			CleanupStack::PopAndDestroy(&dbValue);
			}
		else
			{
			aSql.Append(KSQLQueryNull);
			}
		}
	}

	
void CDecisionDb::DoubleSingleQuotesL(const TDesC& aSource, RBuf& aDestination)
/**
	Copies a source descriptor into a destination descriptor. If a single quote
	character is found an extra single quote is copied to the destination.
	Because, it is not possible to search for a string in SQL with single quote (').
	
	@param aSource      A source descriptor.
	@param aDestination A destination descriptor.
 */
	{
	_LIT(KSingleQuote,"'");

	aDestination.Close();
	
	if(aSource.FindF(KSingleQuote) > KErrNotFound)
		{
		const TUint16 KSingleQuoteHex = 0x27;
		TInt len = aSource.Length();
		
		//Define a temporary buffer and set its maximum size to twice of the original (if it contained all quotes!)
		RBuf temp;
		temp.CreateL(len*2);
		CleanupClosePushL(temp);
		
		const TUint16 *ptrSrc = aSource.Ptr();
		TUint16 *ptrDes = (TUint16 *)temp.Ptr();
		TInt srcIndex=0;
		TInt desIndex=0;
		
		for(srcIndex=0; srcIndex<len; ++srcIndex,++desIndex)
			{
			ptrDes[desIndex]	= ptrSrc[srcIndex];
			if(ptrSrc[srcIndex] == KSingleQuoteHex) //is single quote?
				{
				ptrDes[++desIndex] = ptrSrc[srcIndex];//add extra single quote
				}
			}
			
		//Set the actual length	
		temp.SetLength(desIndex);
		//Copy temp into destination. destination's maximum length is the same as the length of temp
		aDestination.CreateL(temp);
		CleanupStack::PopAndDestroy(&temp);
		}
	else
		{
		//there is no single quote, just copy it
		aDestination.CreateL(aSource);
		}
	}
		
		
EXPORT_C CDecisionView *CDecisionDb::CreateViewL(CDecisionFilter& aFilter)
/**
	Generates an SQL view rowset to the decision table by using a filter.
	The view rowset contains a set of rows which depend on the filter.
	  
	@param aFilter 		 A filter object to query the decision database
	@param aDecisionView A decision view object containing handle to the newly created SQL view rowset.
 */		
	{
	RBuf sqlStatement;
	CreateSqlStatementLC(aFilter, sqlStatement);
	
	CDecisionView *dbView = CDecisionView::NewLC();
		
	User::LeaveIfError(dbView->iDbView.Prepare(iDatabase,TDbQuery(sqlStatement),TDbWindow::EUnlimited));
		
	dbView->iColSet = dbView->iDbView.ColSetL();	
		
	CleanupStack::Pop(dbView);
	CleanupStack::PopAndDestroy(&sqlStatement); //sqlStatement
	
	return dbView;
	}

