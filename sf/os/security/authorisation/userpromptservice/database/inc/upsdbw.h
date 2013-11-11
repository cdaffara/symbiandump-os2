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
* Defines a writable interface for UPS Decision Database
*
*/


/**
 @file
 @internalTechnology
 @released
*/
 
#ifndef UPSDBW_H
#define UPSDBW_H

#include <ups/upsdb.h>
#include <ups/upserr.h>
#include <ups/upsconst.h>
#include "upslog.h"

namespace UserPromptService
{

//Database, table and index names
_LIT(KDecisionTable,  "UPSDecisions");
_LIT(KPrimaryIndex,   "UPSPrimaryIndex");
_LIT(KRecordIdIndex,  "UPSRecordIdIndex");

//Column names of the decision table
_LIT(KColServerSid,    		"ServerSid");
_LIT(KColServiceId,    		"ServiceId");
_LIT(KColClientSid,    		"ClientSid");
_LIT(KColEvaluatorId,  		"EvaluatorId");
_LIT(KColFingerprint,  		"Fingerprint");
_LIT(KColClientEntity, 		"ClientEntity");
_LIT(KColDescription,  		"Description");
_LIT(KColResult, 	   		"Result");
_LIT(KColEvaluatorInfo,		"EvaluatorInfo");
_LIT(KColMajorPolicyVersion,"MajorPolicyVersion");
_LIT(KColRecordId,			"RecordId");

//Constant lengths
static const TInt KBaseSQLLength   = 33;
static const TInt KMaxIntCondition = 26;    //columnname_length(11) + equal_sign(1) + max_decimal_digit_length(9) + and_length(5)
static const TInt KMaxStringCondition = 52; //columnname_length(12) + equal_sign(1) + max_field_length(32) + two_singlequote(2) + and_length(5)
static const TInt KCompactionTreshold = 50; //a percentage which indicates at least how much of the size of the database must be live data
static const TInt KIndexColumnNumber  = 7;  //The number of columns constructing the decision index

//Basic SQL statement parts
_LIT(KSQLQueryBase, 			 "SELECT * FROM %S");
_LIT(KSQLQueryString,	 		 "%S");
_LIT(KSQLQueryConditionInt,		 "%u");
_LIT(KSQLQueryConditionString,	 "'%S'");
_LIT(KSQLQueryWhere, 			 " WHERE ");
_LIT(KSQLQueryAnd, 				 " AND ");
_LIT(KSQLQueryNull, 			 " IS NULL");
_LIT(KSQLQueryEqual,			 "=");
_LIT(KSQLQueryNotEqual,			 "<>");
_LIT(KSQLQueryLessThan,			 "<");
_LIT(KSQLQueryGreaterThan,		 ">");
_LIT(KSQLQueryLessThanOrEqual,	 "<=");
_LIT(KSQLQueryGreaterThanOrEqual,">=");
	
//Other classes used in UPS database
class CDecisionFilter;
class CDecisionRecord;
class CDecisionDb;
	
NONSHARABLE_CLASS(CDecisionDbCompactor):public CActive
/**
	Provides an interface to compact the decision database asynchronously. While the
    compaction operation is in progress, the database cannot be used for any other
	operations such as searching records or preparing views.
 */
	{
	friend class CDecisionDbW;
	
public:	
 	IMPORT_C void Compact(TRequestStatus& aStatus);	
 	~CDecisionDbCompactor();

protected:	
	static CDecisionDbCompactor *NewLC();
 	CDecisionDbCompactor();	
 	
	/**
		From CActive. Handles incremental database compaction.
	 */
	void RunL();	
	/**
		From CActive. Cancels current database compaction.
	 */
	void DoCancel();	
	/**
		From CActive. Handles errors.
	 */
	TInt RunError(TInt aError);

private:
	/**
		Handle to the interface used for performing database compaction in incremental steps
	 */
	RDbIncremental iDbIncremental;
	/**
	 	Contains the current step count for the incremental compaction operation	
	 */
	TPckgBuf<TInt> iStep;
	/**
	   Copy of the status variable of the client which requested the compaction service
	 */
	TRequestStatus* iClientStatus;
	};


NONSHARABLE_CLASS(CDecisionDbW):public CDecisionDb
/**
   A writable interface for UPS decision database. Connects to the database during construction.
   Creates the decision database and table, if not exist. Performs querying, insertion and deletion
   operations on the decision database.
 */
	{
public:
	IMPORT_C static CDecisionDbW *NewL(const TDesC& aDbName, RFs& aFs);	
	IMPORT_C static CDecisionDbW *NewLC(const TDesC& aDbName, RFs& aFs);
	
	~CDecisionDbW();

	IMPORT_C void CreateDecisionL(CDecisionRecord& aRecord);	
	IMPORT_C CDecisionRecord *GetDecisionL(CDecisionFilter& aFilter);
	IMPORT_C TBool UpdateDecisionL(CDecisionFilter& aFilter, CDecisionRecord& aNewRecord);
	IMPORT_C void RemoveDecisionsL(CDecisionFilter& aFilter);
	IMPORT_C void DeleteDatabaseL(RFs& aFs);
	IMPORT_C CDecisionDbCompactor *PrepareCompactionLC();

private:
	CDecisionDbW();
	
	void ConstructL(const TDesC& aDbName, RFs& aFs);	
	void OpenDatabaseL(RFs& aFs);
	void CreateDatabaseL(RFs& aFs);
	void CreateTableL();	
	void CreateIndexL();	
	void DoRemoveDecisionL(CDecisionFilter& aFilter);
	void UpdateCurrentRowL(RDbTable& aTable, CDecisionRecord& aRecord);
	void PrepareForSearchL(RDbTable& aTable, CDecisionFilter& aFilter,TDbSeekMultiKey<KIndexColumnNumber>& aSeekKey);
	
private:
	/**
	 * The fully qualified path of the database
	 */
	HBufC* iDbName;	
	};		
	
}//namespace Ups

#endif //UPSDBW_H
