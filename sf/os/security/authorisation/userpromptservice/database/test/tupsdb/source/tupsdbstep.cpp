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
* Performs decision filter, record and database tests
*
*/


/**
 @file
 @test
*/

#include "tupsdbstep.h"
#include <test/testexecutelog.h>

using namespace UserPromptService;


CUpsDbStepUnitTest::CUpsDbStepUnitTest()
/**
	Constructer
 */
	{
	iComparison = EEqual;
	iCancel = EFalse;	
	}
	
CUpsDbStepUnitTest::~CUpsDbStepUnitTest()
/**
	Destructor
 */
	{
	iFs.Close();
	iFileWriteStream.Close();
	iFileReadStream.Close();
	delete iWaiter;
	delete iScheduler;
	}

	
TVerdict CUpsDbStepUnitTest::doTestStepPreambleL()
/**
	This function is called at the beginning of the test step. First reads the operation type
	and converts it to a proper enumaration value (TUpsDbUnitTest). Then gets the specified 
	database location. The test fails if the database location can not be found, except
	the unit tests of the decision record and filter objects.
 */
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	iWaiter = new(ELeave)CActiveWaiter(Logger());
	
	iLog = EFalse;
	
	TPtrC opType;

	if(EFalse == GetStringFromConfig(ConfigSection(),KOpType,opType))
		{
		ERR_PRINTF2(_L("Failed to read '%S' key of configuration"),&KOpType);
		SetTestStepResult(EFail);
		}
	
	if(!opType.CompareF(KInsertRecord))
		{
		iOpType = EInsertRecord;
		}
	else if(!opType.CompareF(KDeleteRecord))
		{
		iOpType = EDeleteRecord;
		}
	else if(!opType.CompareF(KSingleLookup))
		{
		iOpType = ESingleLookup;
		}
	else if(!opType.CompareF(KMultipleLookup))
		{
		iOpType = EMultipleLookup;
		}
	else if(!opType.CompareF(KVerifyRecord))
		{
		iOpType = EVerifyRecord;
		}
	else if(!opType.CompareF(KCreateDatabase))
		{
		iOpType = ECreateDatabase;
		}
	else if(!opType.CompareF(KDeleteDatabase))
		{
		iOpType = EDeleteDatabase;
		}
	else if(!opType.CompareF(KCreateFilterSingleCall))
		{
		iOpType = ECreateFilterSingleCall;
		}
	else if(!opType.CompareF(KCreateFilterMultipleCalls))
		{
		iOpType = ECreateFilterMultipleCalls;
		}
	else if(!opType.CompareF(KCreateDecision))
		{
		iOpType = ECreateDecision;
		}
	else if(!opType.CompareF(KFillTable))
		{
		iOpType = EFillTable;
		}
	else if(!opType.CompareF(KAllFingerprints))
		{
		iOpType = EAllFingerprints;
		}
	else if(!opType.CompareF(KCompactDatabase))
		{
		iOpType = ECompact;
		}
	else if(!opType.CompareF(KUpdateRecord))
		{
		iOpType = EUpdate;
		}
	else
		{
		iOpType = ENotSupported;
		}
	
	if(EFalse == GetStringFromConfig(ConfigSection(),KDbLocation,iDbLocation))
		{
		if((iOpType != ECreateFilterSingleCall) && (iOpType != ECreateDecision) && 
		   (iOpType != ECreateFilterMultipleCalls) && (iOpType != ENotSupported))
			{
			ERR_PRINTF2(_L("Failed to read '%S' key of configuration"),&KDbLocation);
			SetTestStepResult(EFail);
			User::Leave(KErrNotFound);
			}
		}
	
	GetBoolFromConfig(ConfigSection(), KOOMCondition,iOOMCondition);
	
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.CreatePrivatePath(EDriveC));			
	return TestStepResult();
	}


TVerdict CUpsDbStepUnitTest::doTestStepL()
/**
	Calls actual test step or OOM test
 */
	{
	if (!iOOMCondition)
		{
		 doTestL(); 
		}
	else
		{
 		return doOOMTestL();
	    }	

   	 return TestStepResult();
	}


TVerdict CUpsDbStepUnitTest::doTestL()
/**
	Perform the actual test steps. Unit tests for all provided database operations
	and creation of both filter and decision record objects.
 */
	{
	
	//Define required objects
	CDecisionDbW    *upsDb  = NULL;
	CDecisionRecord *record = NULL;
	CDecisionFilter *filter = NULL;
	CleanupClosePushL(iFingerprint);
	CleanupClosePushL(iClientEntity);
		
	switch(iOpType)
		{
		case ECreateDatabase:
			{
			//Make sure the database file does not exist
			iFs.Delete(iDbLocation);
			
			//Create the database
			upsDb = CreateDatabaseLC(iDbLocation, iFs);
			
			//Disconnect
			CleanupStack::PopAndDestroy(upsDb);
			break;
			}
		
		case EDeleteDatabase:
			{
			//Connect to the database
			upsDb = CDecisionDbW::NewL(iDbLocation, iFs);
			CleanupStack::PushL(upsDb);
			
			//Delete all records and database
			RemoveAllDecisionsL(*upsDb, iFs);
			
			//Disconnect
			CleanupStack::PopAndDestroy(upsDb);
			break;
			}
		
		case ECreateFilterSingleCall:
			{
			//Read the decision values from the config file
			ReadDecisionFieldsL(EFalse);
			
			TPtrC strExpectedResult;
			if(EFalse == GetStringFromConfig(ConfigSection(),KExpectedResult,strExpectedResult))
				{
				ERR_PRINTF1(_L("Expected result is missing!"));
				SetTestStepResult(EFail);
				break;
				}
			TInt expectedResult = ConvertNumber(strExpectedResult);
				
			//First method is to create a filter with single function call	
			TRAPD(error,filter = CDecisionFilter::NewL(TUid::Uid(iClientSid),TUid::Uid(iEvaluatorId),TUid::Uid(iServiceId),TUid::Uid(iServerSid),iFingerprint,iClientEntity,iMajorPolicyVersion));	
			
			if(error != expectedResult)
				{
				ERR_PRINTF3(_L("In the single call method, the result (%d) is different from the expected result (%d)"), error, expectedResult);
				SetTestStepResult(EFail);
				}
				
			if(error == KErrNone)
				{
				INFO_PRINTF1(_L("Successful to create a decision filter using the single call method"));
				delete filter;
				}
			else
				{
				INFO_PRINTF1(_L("Failed to create a decision filter using the single call method"));
				}
				
			break;
			}
		
		case ECreateFilterMultipleCalls:
			{
			//Read the decision values from the config file
			ReadDecisionFieldsL(EFalse);
			
			TPtrC strExpectedResult;
			if(EFalse == GetStringFromConfig(ConfigSection(),KExpectedResult,strExpectedResult))
				{
				ERR_PRINTF1(_L("Expected result is missing!"));
				SetTestStepResult(EFail);
				break;
				}
			TInt expectedResult = ConvertNumber(strExpectedResult);
				
			//Second method is to create an empty filter and then set provided fields
			TRAPD(error,filter = CreateFilterL(iSetFlag,iClientSid,iEvaluatorId,iServiceId,iServerSid,iFingerprint,iClientEntity,iMajorPolicyVersion,iRecordId,iDescription,iResult,iEvaluatorInfo,iComparison));		
			
				if(error != expectedResult)
				{
				ERR_PRINTF3(_L("In the multiple calls method, the result (%d) is different from the expected result (%d)"), error, expectedResult);
				SetTestStepResult(EFail);
				}
				
			if(error == KErrNone)
				{
				INFO_PRINTF1(_L("Successful to create a decision filter using the multiple calls method"));
				delete filter;
				}
			else
				{
				INFO_PRINTF1(_L("Failed to create a decision filter using the multiple calls method"));
				}
				
			break;
			}
			
		case ECreateDecision:
			{
			//Read the decision values from the config file
			ReadDecisionFieldsL(EFalse);
			
			TPtrC strExpectedResult;
			if(EFalse == GetStringFromConfig(ConfigSection(),KExpectedResult,strExpectedResult))
				{
				ERR_PRINTF1(_L("Expected result is missing!"));
				SetTestStepResult(EFail);
				break;
				}
			TInt expectedResult = ConvertNumber(strExpectedResult);
			
			TRAPD(error,record = CreateRecordL(iClientSid,iEvaluatorId,iServiceId,iServerSid,iFingerprint,iClientEntity,iDescription,iResult,iMajorPolicyVersion,iEvaluatorInfo,iRecordId));
			
			if(error != expectedResult)
				{
				ERR_PRINTF3(_L("The result (%d) is different from the expected result (%d)"), error, expectedResult);
				SetTestStepResult(EFail);
				}
				
				if(error == KErrNone)
				{
				INFO_PRINTF1(_L("Successful to create a decision record"));
				delete record;
				}
			else
				{
				INFO_PRINTF1(_L("Failed to create a decision record"));
				}
				
			break;
			}
			
		case EInsertRecord:
			{
			//Read the decision values from the config file
			ReadDecisionFieldsL(ETrue);
			
			//ClientSid,EvaluatorId,ServiceId,ServerSid and Fingerprint must be provided
			if((iSetFlag & (KSetClientSid|KSetEvaluatorId|KSetServiceId|KSetServerSid|KSetFingerprint))!=
			   (KSetClientSid|KSetEvaluatorId|KSetServiceId|KSetServerSid|KSetFingerprint))
				{
				SetTestStepResult(EFail);
				break;
				}
				
			//Create decision record
			record = CreateRecordL(iClientSid,iEvaluatorId,iServiceId,iServerSid,iFingerprint,iClientEntity,iDescription,iResult,iMajorPolicyVersion,iEvaluatorInfo,iRecordId);
			CleanupStack::PushL(record);
			
			//Connect to the database
			upsDb = CDecisionDbW::NewL(iDbLocation,iFs);
			CleanupStack::PushL(upsDb);
			
			//Insert the decision
			InsertRecordL(*upsDb, *record);
			
			//Disconnect and destroy objects
			CleanupStack::PopAndDestroy(2,record);
			
			break;
			}
			
		case EDeleteRecord:
			{
			//Read the decision values from the config file
			ReadDecisionFieldsL(ETrue);
		
			if(!iSetFlag)
				{
				ERR_PRINTF1(_L("A filter key must be supplied at least!"));
				SetTestStepResult(EFail);
				break;
				}
								
			//Create filter
			filter = CreateFilterLC(iSetFlag,iClientSid,iEvaluatorId,iServiceId,iServerSid,iFingerprint,iClientEntity,iMajorPolicyVersion,iRecordId,iDescription,iResult,iEvaluatorInfo,iComparison);
			
			//Connect to the database
			upsDb = CDecisionDbW::NewLC(iDbLocation,iFs);	
			
			//Delete the decision
			RemoveDecisionsL(*upsDb, *filter);
		
			CleanupStack::PopAndDestroy(2,filter);
			break;
			}
		
		case ESingleLookup:
			{
			//Read the decision values from the config file
			ReadDecisionFieldsL(ETrue);
			
			TInt expectedRecords;
			if(EFalse == GetIntFromConfig(ConfigSection(),KExpectedRecords,expectedRecords))
				{
				ERR_PRINTF1(_L("Expected result is missing!"));
				SetTestStepResult(EFail);
				break;
				}
			
			TPtrC strExpectedResult;
			TInt expectedResult=KErrNone;
			if(GetStringFromConfig(ConfigSection(),KExpectedResult,strExpectedResult))
				{
				expectedResult = ConvertNumber(strExpectedResult);
				}
				
			//Create filter
			filter = CreateFilterL(iSetFlag,iClientSid,iEvaluatorId,iServiceId,iServerSid,iFingerprint,iClientEntity,iMajorPolicyVersion,iRecordId,iDescription,iResult,iEvaluatorInfo,iComparison);
			CleanupStack::PushL(filter);
			
			//Connect to the database
			upsDb = CDecisionDbW::NewLC(iDbLocation,iFs);
			
			//Retrieve single record
			TRAPD(err,record = GetDecisionLC(*upsDb, *filter);if(record)CleanupStack::Pop(record););
			if((err==KErrNone)&&(record))
				{
				CleanupStack::PushL(record);
				}
			  
			if(err!=expectedResult)
				{
				SetTestStepResult(EFail);
				}
				
			if(record)
				{
				OpenDecisionFileL(EFileWrite);
				WriteDecisionL(*record);
				CloseDecisionFile();
				CleanupStack::PopAndDestroy();
				}
			else
				{
				if(expectedRecords)
					{
					ERR_PRINTF1(_L("Failed to retrieve a single record"));
					SetTestStepResult(EFail);
					}
				}
				
	
			CleanupStack::PopAndDestroy(upsDb);
			CleanupStack::PopAndDestroy(filter);
			break;			
			}
		
		case EMultipleLookup:
			{
			//Read the decision values from the config file
			ReadDecisionFieldsL(ETrue);
			
			//Create filter
			filter = CreateFilterL(iSetFlag,iClientSid,iEvaluatorId,iServiceId,iServerSid,iFingerprint,iClientEntity,iMajorPolicyVersion,iRecordId,iDescription,iResult,iEvaluatorInfo,iComparison);
			CleanupStack::PushL(filter);
			
			//Connect to the database
			upsDb = CDecisionDbW::NewL(iDbLocation,iFs);
			CleanupStack::PushL(upsDb);
			
			RPointerArray<CDecisionRecord> recordList;
			CleanupResetAndDestroyPushL(recordList);
			GetDecisionsL(*upsDb, *filter, iCancel, recordList);
			
			TInt recNum;
			recNum = recordList.Count();
			if(recNum)
				{				
				OpenDecisionFileL(EFileWrite);
			
				for(TInt i=0; i<recNum; ++i)
					{
					WriteDecisionL(*recordList[i]);
					}
				CloseDecisionFile();				
				}
			CleanupStack::PopAndDestroy(&recordList);			
				
			TInt expectedRecords;
			if(EFalse == GetIntFromConfig(ConfigSection(),KExpectedRecords,expectedRecords))
				{
				expectedRecords=-1;
				}
			
			if(!iCancel && (((expectedRecords>=0) && (recNum != expectedRecords))||((expectedRecords<0) && (recNum<=0))))
				{
				SetTestStepResult(EFail);
				}
		
			CleanupStack::PopAndDestroy(2,filter);
			break;	
			}
			
		case EVerifyRecord:
			{
			//Read the decision values from the config file
			ReadDecisionFieldsL(ETrue);
			TBool verified = ETrue;
			
			OpenDecisionFileL(EFileRead);
				
			while((record = ReadDecisionL()) != NULL)
				{
				CleanupStack::PushL(record);
				
				if(iSetFlag & KSetClientSid)
					{
					if(record->iClientSid != TSecureId(iClientSid))
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetEvaluatorId)
					{
					if(record->iEvaluatorId != TUid::Uid(iEvaluatorId))
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetServiceId)
					{
					if(record->iServiceId != TUid::Uid(iServiceId))
						{
						verified = EFalse;
						break;
						}
					}
					
				if(iSetFlag & KSetServerSid)
					{
					if(record->iServerSid != TSecureId(iServerSid))
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetFingerprint)
					{
					if(record->iFingerprint.Compare(iFingerprint))
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetClientEntity)
					{
					if(record->iClientEntity.Compare(iClientEntity))
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetDescription)
					{
					if(record->iDescription.Compare(iDescription))
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetResult)
					{
					if(record->iResult != iResult)
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetEvaluatorInfo)
					{
					if(record->iEvaluatorInfo != iEvaluatorInfo)
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetMajorPolicyVersion)
					{
					if(record->iMajorPolicyVersion != iMajorPolicyVersion)
						{
						verified = EFalse;
						break;
						}
					}
				
				if(iSetFlag & KSetRecordId)
					{
					if(record->iRecordId != iRecordId)
						{
						verified = EFalse;
						break;
						}
					}
						
				CleanupStack::PopAndDestroy(record);						
				}//while
			
			
			if(verified == EFalse)
				{
				ERR_PRINTF1(_L("Retrieved record(s) is(are) different from the supplied one"));
				SetTestStepResult(EFail);
				CleanupStack::PopAndDestroy(record);
				}
			else
				{
				INFO_PRINTF1(_L("Retrieved record(s) have been verified successfully"));
				}
				
			CloseDecisionFile();
			
			break;
			}
		
		case EFillTable:
			{
			TBuf<KUpsDescriptionLength> keyNotFound;
			ReadConfigurationFile(keyNotFound);
			TInt len = keyNotFound.Length();
			if(len > 0)
    			{
    			//if more than one keys could not read or retrieve_num is not in the list
    			if((len > 13) || (keyNotFound.Find(KRetrieveNum) == KErrNotFound))
    				{
    				ERR_PRINTF2(_L("Failed to read '%S' key of configuration"),&keyNotFound);
    				SetTestStepResult(EFail);
    				User::Leave(KErrNotFound);
    				}	
    			}
    		//Connect to the decision database
			CDecisionDbW* upsDb = CreateDatabaseLC(iDbLocation,iFs);
	
			//Fill the table with a specified number of decision records	
			FillTableL(*upsDb,iRecordNum,iClientNum,iServiceNum,iServerNum,iEvaluatorNum);
			
			//Disconnect
			CleanupStack::PopAndDestroy(upsDb);
			break;
			}
		
		case EAllFingerprints:
			{
			//Connect to the decision database
			CDecisionDbW* upsDb = CreateDatabaseLC(iDbLocation,iFs);
			TestFingerprintValuesL(*upsDb);
			//Disconnect
			CleanupStack::PopAndDestroy(upsDb);
			break;
			}
		
		case ECompact:
			{
			ReadDecisionFieldsL(ETrue);
			//Connect to the decision database
			CDecisionDbW* upsDb = CreateDatabaseLC(iDbLocation,iFs);
			CompactDatabaseL(*upsDb,iCancel);
			//Disconnect
			CleanupStack::PopAndDestroy(upsDb);
			break;
			}
		
		case EUpdate:
			{
			//Open the decision file containing the record which will be updated
			OpenDecisionFileL(EFileRead);
			//Read the record	
			record = ReadDecisionL();
			//Close the decision file
			CloseDecisionFile();
			//If we could not read the record, the test step fails
			if(!record)
				{
				SetTestStepResult(EFail);
				break;
				}
			CleanupStack::PushL(record);
			
			//Create filter from the record which has been read from the decision file
			TInt flag = (KSetClientSid|KSetEvaluatorId|KSetServiceId|KSetServerSid|KSetFingerprint|KSetClientEntity|KSetMajorPolicyVersion);
			filter = CreateFilterL(flag,record->iClientSid.iId,record->iEvaluatorId.iUid,record->iServiceId.iUid,record->iServerSid.iId,record->iFingerprint,record->iClientEntity,record->iMajorPolicyVersion,record->iRecordId,iDescription,iResult,iEvaluatorInfo,iComparison);
			CleanupStack::PushL(filter);
			
			//Create a new file
			ReadDecisionFieldsL(ETrue);
			//Create a new record from the values in the configuration file
			CDecisionRecord *newRecord = CreateRecordL(iClientSid,iEvaluatorId,iServiceId,iServerSid,iFingerprint,iClientEntity,iDescription,iResult,iMajorPolicyVersion,iEvaluatorInfo,iRecordId);
			CleanupStack::PushL(newRecord);
										
			//Connect to the decision database
			CDecisionDbW* upsDb = CreateDatabaseLC(iDbLocation,iFs);
			//Update the record
			UpdateRecordL(*upsDb, *filter, *newRecord);
			
			CleanupStack::PopAndDestroy(4, record);
			
			break;
			}
			
		case ENotSupported:
			{
			ERR_PRINTF1(_L("The provided key is not supported!"));
			SetTestStepResult(EFail);
			break;
			}
		}
		
	CleanupStack::PopAndDestroy(2); //Pop RBufs	
	return TestStepResult();
	}


TVerdict CUpsDbStepUnitTest::doOOMTestL()
/**
	Performs OOM test
 */
	{
	TVerdict verdict = EFail;
 	TInt countAfter = 0;
	TInt countBefore = 0;
 	for (TInt oomCount = 0; ; oomCount++)
 		{
 		INFO_PRINTF2(_L("\n ==== Number of memory allocations %d ===="), oomCount);
 		verdict = EFail;
 		__UHEAP_RESET;
 		__UHEAP_SETFAIL(RHeap::EDeterministic, oomCount);
 		countBefore = User::CountAllocCells();
 		TRAPD(error, doTestL());// ----> This is the actual test that runs under OOM conditions.
 		countAfter = User::CountAllocCells();
 		__UHEAP_RESET;
 		
 		TESTL(error == KErrNone || error == KErrNoMemory);
 		
 		if (error == KErrNone)
 			{
 			verdict = EPass;
 			INFO_PRINTF2(_L("OOM Status %d"),error);
			INFO_PRINTF1(_L("Test outcome : Passed"));
  			break;
 			}
 		else
 			{
 			if (countBefore != countAfter)
 				{
 				INFO_PRINTF2(_L("OOM Status %d"),error);
 				INFO_PRINTF2(_L("OOM Failed at %d"), oomCount);
 				SetTestStepResult(verdict);  
 				break;
 				}
 			}
 		INFO_PRINTF2(_L("OOM Failed Point status %d"), error);
		}
	INFO_PRINTF3(_L("Heap alloc count ok: %d final vs %d initial"), countAfter,countBefore);
 	SetTestStepResult(verdict);
 	if (verdict==EFail)
	 	{
 		User::Leave(KErrGeneral);	 		
	 	}	 	
 	return verdict;
	}
	
	
TVerdict CUpsDbStepUnitTest::doTestStepPostambleL()
/**
	Runs at the end of the step. Do nothing.
 */
	{
	iFs.Close();
	return TestStepResult();
	}


void CUpsDbStepUnitTest::ReadDecisionFieldsL(TBool aHash)
/**
	Reads the fileds required to create a decision record or filter.
	
	@param aHash Hashes fingerprint and client entity values if ETrue.
 */
	{
	if(GetHexFromConfig(ConfigSection(),KColClientSid,iClientSid))
		{
		iSetFlag |= KSetClientSid;
		}
		
	if(GetHexFromConfig(ConfigSection(),KColEvaluatorId,iEvaluatorId))
		{
		iSetFlag |= KSetEvaluatorId;
		}
		
	if(GetHexFromConfig(ConfigSection(),KColServiceId,iServiceId))
		{
		iSetFlag |= KSetServiceId;
		}
		
	if(GetHexFromConfig(ConfigSection(),KColServerSid,iServerSid))
		{
		iSetFlag |= KSetServerSid;
		}
		
	TPtrC string; //16-bit temporary pointer
	CMD5 *md5 = CMD5::NewL();
	CleanupStack::PushL(md5);
		
	if(GetStringFromConfig(ConfigSection(),KColFingerprint,string))
		{
		if(string.Length() > 0)
			{
			//Copy to convert 16-bit to 8-bit
			iFingerprint.CreateL(string.Length()*2);
			iFingerprint.Copy(string);
			if(aHash)
				{
				TPtrC8 hashed = md5->Hash(iFingerprint);
				if(string.Length()*2 < hashed.Length())
					{
					iFingerprint.ReAlloc(hashed.Length());
					}
				iFingerprint.Copy(hashed);
				md5->Reset();
				}
			}
		iSetFlag |= KSetFingerprint;
		}
			
	if(GetStringFromConfig(ConfigSection(),KColClientEntity,string))
		{
		if(string.Length() > 0)
			{
			//Copy to convert 16-bit to 8-bit
			iClientEntity.CreateL(string.Length()*2);
			iClientEntity.Copy(string);
			if(aHash)
				{
				TPtrC8 hashed = md5->Hash(iClientEntity);
				if(string.Length()*2 < hashed.Length())
					{
					iClientEntity.ReAlloc(hashed.Length());
					}
				iClientEntity.Copy(hashed);
				}
			}
		iSetFlag |= KSetClientEntity;
		}
	
	CleanupStack::PopAndDestroy(md5);
	
	if(GetStringFromConfig(ConfigSection(),KColDescription,iDescription))
		{
		iSetFlag |= KSetDescription;	
		}
	
	if(GetIntFromConfig(ConfigSection(),KColResult,iResult))
		{
		iSetFlag |= KSetResult;	
		}
					
	if(GetIntFromConfig(ConfigSection(),KColEvaluatorInfo,iEvaluatorInfo))
		{
		iSetFlag |= KSetEvaluatorInfo;	
		}
	
	if(GetIntFromConfig(ConfigSection(),KColMajorPolicyVersion,iMajorPolicyVersion))
		{
		iSetFlag |= KSetMajorPolicyVersion;
		}
	
	if(GetIntFromConfig(ConfigSection(),KColRecordId,iRecordId))
		{
		iSetFlag |= KSetRecordId;
		}
	
	TInt compOp;
	if(GetHexFromConfig(ConfigSection(),KComparison,compOp))
		{
		iComparison = static_cast<TComparisonOp>(compOp);
		}		
	
	GetIntFromConfig(ConfigSection(),KCancel,iCancel);
	}


void CUpsDbStepUnitTest::OpenDecisionFileL(TFileMode aMode)
/**
	Opens a log file which is used to write/read decision records.
	@param aMode File open mode. If EFileWrite, the existed file is replaced with new one.
 */
	{
	//The file server connection will be closed in the destructor
	User::LeaveIfError(iFs.CreatePrivatePath(EDriveC));
	
	User::LeaveIfError(iFs.PrivatePath(iDecisionFileName));
	iDecisionFileName.Append(KDecisionFileName);
	
	if((aMode & EFileWrite)== EFileWrite)
		{
		User::LeaveIfError(iFileWriteStream.Replace(iFs,iDecisionFileName,aMode));
		}
	else if((aMode & EFileRead)== EFileRead)
		{
		User::LeaveIfError(iFileReadStream.Open(iFs,iDecisionFileName,aMode));
		}
	//Note that the streams will be closed in the destructor
	}

void CUpsDbStepUnitTest::WriteDecisionL(CDecisionRecord &aRecord)
/**
	Writes a provided decision into the decision log file. This file is then used
	to verify the written records.
 */
	{
	iFileWriteStream.WriteInt32L(aRecord.iClientSid.iId);
	iFileWriteStream.WriteInt32L(aRecord.iEvaluatorId.iUid);
	iFileWriteStream.WriteInt32L(aRecord.iServiceId.iUid);
	iFileWriteStream.WriteInt32L(aRecord.iServerSid.iId);
	iFileWriteStream.WriteUint8L(aRecord.iResult);
	iFileWriteStream.WriteUint32L(aRecord.iEvaluatorInfo);
	iFileWriteStream.WriteUint16L(aRecord.iMajorPolicyVersion);
	iFileWriteStream.WriteUint32L(aRecord.iRecordId);
	
	iFileWriteStream.WriteInt16L(aRecord.iFingerprint.Length());
	iFileWriteStream.WriteL(aRecord.iFingerprint);
	
	if(aRecord.iClientEntity.Length())
		{
		iFileWriteStream.WriteInt16L(aRecord.iClientEntity.Length());
		iFileWriteStream.WriteL(aRecord.iClientEntity);
		}
	else
		{
		iFileWriteStream.WriteInt16L(4);
		iFileWriteStream.WriteL(_L8("NULL"));	
		}
	
	if(aRecord.iDescription.Length())
		{
		iFileWriteStream.WriteInt16L(aRecord.iDescription.Length());
		iFileWriteStream.WriteL(aRecord.iDescription);
		}
	else
		{
		iFileWriteStream.WriteInt16L(4);
		iFileWriteStream.WriteL(_L("NULL"));
		}
	
	iFileWriteStream.CommitL();
	}
	
	
CDecisionRecord *CUpsDbStepUnitTest::ReadDecisionL()
/**
	Reads and returns a decision record from the log file. If reaches the file end, returns NULL.
	The records retrieved from the log file are compared with the records defined in the configuration file.
 */
	{	
	
	TBuf8<KDbMaxName> fingerPrint;
	TBuf8<KDbMaxName> clientEntity;
	TBuf<KDbMaxName> description;
	TInt len;
	CDecisionRecord *record = NULL;
	
	TStreamPos currentPosition = iFileReadStream.Source()->TellL(MStreamBuf::ERead);
	TStreamPos endPosition = iFileReadStream.Source()->SeekL(MStreamBuf::ERead,EStreamEnd);
	
	if(currentPosition == endPosition)
		{
		INFO_PRINTF1(_L("Reached to the end of the decision file"));
		return record;
		}
			
	iFileReadStream.Source()->SeekL(MStreamBuf::ERead,currentPosition);
	CleanupClosePushL(iFileReadStream);//In first read a byte is allocated inside the stream, so push it
	TInt32  clientId 	  		= iFileReadStream.ReadInt32L();
	TInt32  evaluatorId   		= iFileReadStream.ReadInt32L();
	TInt32  serviceId 	  		= iFileReadStream.ReadInt32L();
	TInt32  serverId	  		= iFileReadStream.ReadInt32L();
	TUint8  result		  		= iFileReadStream.ReadUint8L();
	TUint32 evaluatorInfo 		= iFileReadStream.ReadUint32L();
	TUint16 policyMajorVersion 	= iFileReadStream.ReadUint16L();
	TUint32 recordId 	  		= iFileReadStream.ReadUint32L();
	
	len = iFileReadStream.ReadInt16L();
	iFileReadStream.ReadL(fingerPrint,len);
	len = iFileReadStream.ReadInt16L();
	iFileReadStream.ReadL(clientEntity,len);
	len = iFileReadStream.ReadInt16L();
	iFileReadStream.ReadL(description,len);
	
	if(!clientEntity.Compare(_L8("NULL")))
		{
		clientEntity.SetLength(0);
		}
		
	if(!description.Compare(_L("NULL")))
		{
		description.SetLength(0);
		}
	
	//Create a decision record from readings
	record = CreateRecordL((TInt)clientId,(TInt)evaluatorId,(TInt)serviceId,(TInt)serverId,fingerPrint,clientEntity,description,(TInt)result,(TInt)policyMajorVersion,(TInt)evaluatorInfo,(TInt)recordId);
	CleanupStack::Pop(&iFileReadStream);	
	return record;
	}
	
void CUpsDbStepUnitTest::CloseDecisionFile()
/**
	Closes the file server session and read/write stream objects
 */
	{
	iFileReadStream.Close();
	iFileWriteStream.Close();
	}

