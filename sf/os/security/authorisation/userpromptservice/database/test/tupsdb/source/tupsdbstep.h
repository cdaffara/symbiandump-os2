/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
 @internalComponent
*/

#ifndef TUPSDBSTEP_H
#define TUPSDBSTEP_H

#include "tupsdb.h"
#include "activewaiter.h"
#include "upsdbw.h"

#include <test/testexecutelogger.h>
#include <test/tefunit.h>
#include <hash.h>
#include "upscommon.h"
#include <scs/cleanuputils.h>

using namespace UserPromptService;

_LIT8(KFingerprint, "FingerprintABCDEFGHIJKLMNOPR%d");
_LIT8(KClientEntity,"ClientEntityABCDEFGHIJKLMNOP%d");
_LIT(KDescription,  "DestinationABCDEFGHIJKLMNOPRSVWXYZ0123456789%d");
_LIT(KDbName,       "tupsdecisions.db");
_LIT(KLogName,       "tperfomance.log");

//Constants for performance test
_LIT(KServerNum,    "server_num");
_LIT(KServiceNum,   "service_num");
_LIT(KClientNum,    "client_num");
_LIT(KEvaluatorNum, "evaluator_num");
_LIT(KRecordNum,    "records_num");
_LIT(KRetrieveNum,  "retrieve_num");

//Constants for unit tests
_LIT(KOpType,        "OperationType");
_LIT(KCreateDatabase,"CreateDatabase");
_LIT(KDeleteDatabase,"DeleteDatabase");
_LIT(KCreateFilterSingleCall,  "CreateFilterSingleCall");
_LIT(KCreateFilterMultipleCalls,  "CreateFilterMultipleCalls");
_LIT(KCreateDecision, "CreateDecision");
_LIT(KInsertRecord,   "Insert");
_LIT(KDeleteRecord,   "Delete");
_LIT(KSingleLookup,   "SingleLookup");
_LIT(KMultipleLookup, "MultipleLookup");
_LIT(KVerifyRecord,   "Verify");
_LIT(KFillTable,      "FillTable");
_LIT(KAllFingerprints,"AllFingerprints");
_LIT(KCompactDatabase,"Compact");
_LIT(KUpdateRecord,   "Update");


_LIT(KDbLocation,     "DatabaseLocation");
_LIT(KExpectedResult, "ExpectedResult");
_LIT(KExpectedRecords,"ExpectedRecords");
_LIT(KOOMCondition,   "OOMCondition");
_LIT(KComparison,	  "ComparisonOp");
_LIT(KCancel,	      "Cancel");

_LIT(KStepPerformance, "MeasurePerformance");
_LIT(KStepUnitTest,    "RunUpsDbUnitTest");
_LIT(KDecisionFileName,"decisions.dat");

//Errors
_LIT(KStrErrNone,    				 "KErrNone");
_LIT(KStrErrUpsBadFingerprintLength, "KErrUpsBadFingerprintLength");
_LIT(KStrErrUpsMissingArgument,		 "KErrUpsMissingArgument");
_LIT(KStrErrUpsBadClientEntityLength,"KErrUpsBadClientEntityLength");
_LIT(KStrErrUpsBadDescriptionLength, "KErrUpsBadDescriptionLength");

const TInt KMd5Length = 0x10;

static const TInt KUpsDescriptionLength = 255;

class CUpsDbBaseStep : public CTestStep
/**
	Implements handy functions used for testing decision database.
 */
	{	
protected:
	CUpsDbBaseStep();
	~CUpsDbBaseStep();
	//Start a timer
	void StartTimer(TTime &aTime);

	//Stops the timer and returns the elapsed time
	void StopTimer(TTime &aStartTime, TInt64 &aElapsedTime);

	//Fill the decision table with a specified number of sample decision records
	void FillTableL(CDecisionDbW &aUpsDb, TInt aRecordNum, TInt aClientNum, TInt aServiceNum, TInt aServerNum, TInt aEvaluatorNum);

	//Create a writable decision database object
	CDecisionDbW* CreateDatabaseLC(TDesC &aDbLocation, RFs& aFs);

	//Get a decision record from the database
	CDecisionRecord *GetDecisionLC(CDecisionDbW &aUpsDb, CDecisionFilter &aFilter);

	//Get a set of decision records from the database
	void GetDecisionsL(CDecisionDbW &aUpsDb, CDecisionFilter &aFilter, TBool aCancel, RPointerArray<CDecisionRecord>& aRecordList);

	//Delete a set of records
	void RemoveDecisionsL(CDecisionDbW &aUpsDb, CDecisionFilter &aFilter);	

	//Delete all records
	void RemoveAllDecisionsL(CDecisionDbW &aUpsDb, RFs& aFs);

	//Insert a record into the decision database
	void InsertRecordL(CDecisionDbW &aUpsDb, CDecisionRecord &aRecord);
	
	//Creates a filter
	CDecisionFilter *CreateFilterL(TInt aFlag, TInt aClientSid, TInt aEvaluatorId, TInt aServiceId, TInt aServerSid, TDesC8 &aFingerprint, TDesC8& aClientEntity,TInt aMajorPolicyVersion, TInt aRecordId, TDesC& aDescription, TInt aResult, TInt aEvaluatorInfo, TComparisonOp aComparisonOp);
	CDecisionFilter *CreateFilterLC(TInt aFlag, TInt aClientId, TInt aEvaluatorId, TInt aServiceId, TInt aServerId, TDesC8 &aFingerPrint, TDesC8& aClientEntity,TInt aMajorPolicyVersion, TInt aRecordId, TDesC& aDescription, TInt aResult, TInt aEvaluatorInfo, TComparisonOp aComparisonOp);
	
	//Create a decision
	CDecisionRecord *CreateRecordL(TInt aClientId, TInt aEvaluatorId, TInt aServiceId, TInt aServerId, TDesC8 &aFingerPrint, TDesC8 &aClientEntity, 
								   TDesC &aDecription, TInt aResult, TInt aMajorPolicyVersion, TInt aEvaluatorInfo, TInt aRecordId);
	
	//Updates a decision record
	void UpdateRecordL(CDecisionDbW& aUpsDb, CDecisionFilter& aFilter, CDecisionRecord& aRecord);
	
	//Creates a number of decision records by using all characters between 0x01 and 0xFF in Fingerprint value
	void TestFingerprintValuesL(CDecisionDbW& aUpsDb);
	
	//Compacts decision database
	void CompactDatabaseL(CDecisionDbW& aUpsDb, TBool aCancel);
	
	//Assigns member variables from configuration file
	void ReadConfigurationFile(TDes &aKeyNotFound);
	
	//Converts error string  to number
	TInt ConvertNumber(TDesC& aError);
	
private:
	void SetFilterFiledsL(CDecisionFilter &aFilter, TInt aFlag, TInt aClientId, TInt aEvaluatorId, TInt aServiceId, TInt aServerId, TDesC8 &aFingerPrint, TDesC8& aClientEntity,TInt aMajorPolicyVersion, TInt aRecordId, TDesC& aDescription, TInt aResult, TInt aEvaluatorInfo, TComparisonOp aComparisonOp);

protected:
	TInt iRecordNum;
	TInt iServerNum;
	TInt iServiceNum;
	TInt iClientNum;
	TInt iEvaluatorNum;
	TInt iRetrieveNum;
	RFs  iFs;
	RFile iFile;
	CActiveScheduler *iScheduler;
	CActiveWaiter	 *iWaiter;
	TBool iLog;
	TBuf8<256> iBuffer;
	};

class CUpsDbStepPerformance : public CUpsDbBaseStep
//Performs perfomance tests
	{
public:
	CUpsDbStepPerformance();
	~CUpsDbStepPerformance();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	void PrintHeader();
		
private:
	TFileName iDbLocation;
	TFileName iPerformanceLog;
	};
	
class CUpsDbStepUnitTest : public CUpsDbBaseStep
//Performs all unit tests tests
	{
public:
	enum TUpsDbUnitTest
		{
		ENotSupported,	  		    //Requested operation is not supported
		ECreateDatabase,  		    //Create a new database
		EDeleteDatabase,  		    //Delete all records and the database
		ECreateFilterSingleCall,    //Creates a filter object with single call
		ECreateFilterMultipleCalls, //Creates a filter object with multiple call
		ECreateDecision,  			//Creates a decision record object
		EInsertRecord, 	  			//Insert a number of decision
		EDeleteRecord, 	  			//Delete a record from the decision database
		ESingleLookup, 	  			//Lookup for single decision record
		EMultipleLookup,  			//Lookup for multipe decision records
		EVerifyRecord,	 		    //Verify the record read in previous operation
		EFillTable,		  			//Fill anumber of records into the decision table
		EAllFingerprints, 			//Tries all binary values between 0-255 to create a fingerprint
		ECompact,		  			//Compact the decision database
		EUpdate			  			//Update the last retrieved record
		};	
public:
	CUpsDbStepUnitTest();
	~CUpsDbStepUnitTest();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	TVerdict doTestL();
	TVerdict doOOMTestL();

protected:
	void ReadDecisionFieldsL(TBool aHash);
	void OpenDecisionFileL(TFileMode aMode);
	void WriteDecisionL(CDecisionRecord &aRecord);
	CDecisionRecord *ReadDecisionL();
	void CloseDecisionFile();
	
protected:
	TUpsDbUnitTest iOpType;
	TPtrC iDbLocation;
	
	TInt  iClientSid;
	TInt  iEvaluatorId;
	TInt  iServiceId;
	TInt  iServerSid;
	RBuf8 iFingerprint;
	RBuf8 iClientEntity;
	TPtrC iDescription;
	TInt  iResult;
	TInt  iEvaluatorInfo;
	TInt  iMajorPolicyVersion;
	TInt  iRecordId;
	TComparisonOp  iComparison;
	TInt  iSetFlag;
	TBool iOOMCondition;
	TBool iCancel;
	
	TFileName iDecisionFileName;
	RFileWriteStream iFileWriteStream;
	RFileReadStream iFileReadStream;
	};
	
#endif //TUPSDBSTEP_H
