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
* Implements basic database test functions.
*
*/


/**
 @file
 @test
*/

#include "tupsdbstep.h"
#include <test/testexecutelog.h>

using namespace UserPromptService;


//
//CUpsDbBaseStep
//

CUpsDbBaseStep::CUpsDbBaseStep()
/**
	onstructor Code
 */
	{
   
	}
	
CUpsDbBaseStep::~CUpsDbBaseStep()
/**
	Destructor Code
 */
	{

	} 


CDecisionFilter *CUpsDbBaseStep::CreateFilterL(TInt aFlag, TInt aClientSid, TInt aEvaluatorId, TInt aServiceId, TInt aServerSid, TDesC8 &aFingerprint, TDesC8& aClientEntity,TInt aMajorPolicyVersion, TInt aRecordId, TDesC& aDescription, TInt aResult, TInt aEvaluatorInfo, TComparisonOp aComparisonOp)
 /**
	Creates an empty filter object and then passes it to SetFilterFiledsL to get assigned all filter keys separately. 
	This function is used to test NewL method of CDecisionFilter
	
	@param aFlag		 Shows which filter keys have been set 
	@param aClientSid	 Secure Id of client application
	@param aEvaluatorId	 UID for the policy evaluator DLL
	@param aServiceId	 UID for service 
	@param aServerSid	 UID for the system server
	@param aFingerprint	 Hash of the destination and/or opaque data.
	@param aClientEntity The name of the entity within the client that requested the service. 
	@param aMajorPolicyVersion The major version of the policy file.
	@param aRecordId	 An auto-incrementing record number.
	@param aDescription A copy of description and/or opaque
	@param aResult Whether the request should be approved
	@param aEvaluatorInfo Policy evaluator specific data
	@param aComparisonOp Comparison Operator (=,!=,<,> etc.)
	
	@return				 A pointer to the newly allocated filter object.
 */	
	{
	//Create an empty filter
	CDecisionFilter *filter = CDecisionFilter::NewL();
	CleanupStack::PushL(filter);
	
	SetFilterFiledsL(*filter, aFlag, aClientSid, aEvaluatorId, aServiceId, aServerSid,aFingerprint, aClientEntity, aMajorPolicyVersion, aRecordId, aDescription, aResult, aEvaluatorInfo, aComparisonOp);
	
	CleanupStack::Pop(filter);
	return filter;
	}


CDecisionFilter *CUpsDbBaseStep::CreateFilterLC(TInt aFlag, TInt aClientSid, TInt aEvaluatorId, TInt aServiceId, TInt aServerSid, TDesC8 &aFingerprint, TDesC8& aClientEntity,TInt aMajorPolicyVersion, TInt aRecordId, TDesC& aDescription, TInt aResult, TInt aEvaluatorInfo, TComparisonOp aComparisonOp)
/**
	Creates an empty filter object and then calls SetFilterFiledsL to get assigned all filter keys separately. 
	This function is used to test NewLC method of CDecisionFilter
	
	@see CUpsDbBaseStep::CreateFilterL
	
	@return A pointer to the newly allocated filter object. The pointer is also put onto the cleanup stack.
 */	
	{
	//Create an empty filter
	CDecisionFilter *filter = CDecisionFilter::NewLC();
	
	SetFilterFiledsL(*filter, aFlag, aClientSid, aEvaluatorId, aServiceId, aServerSid,aFingerprint, aClientEntity, aMajorPolicyVersion, aRecordId, aDescription, aResult, aEvaluatorInfo, aComparisonOp);
	
	return filter;
	}


void CUpsDbBaseStep::SetFilterFiledsL(CDecisionFilter &aFilter, TInt aFlag, TInt aClientSid, TInt aEvaluatorId, TInt aServiceId, TInt aServerSid, TDesC8 &aFingerprint, TDesC8& aClientEntity, TInt aMajorPolicyVersion, TInt aRecordId, TDesC& aDescription, TInt aResult, TInt aEvaluatorInfo, TComparisonOp aComparisonOp)
/**
	This method assigns all the fields of a provided filter object with the supplied parameters.
	The method is called by CreateFilterL and CreateFilterLC methods.
	
	@see CUpsDbBaseStep::CreateFilterL
	@param aFilter Empty decision filter object
 */
	{
	//Sets the variables using aFlag
	if(aFlag & KSetClientSid)
		{
		aFilter.SetClientSid(TSecureId(aClientSid), aComparisonOp);
		}
	
	if(aFlag & KSetEvaluatorId)
		{
		aFilter.SetEvaluatorId(TUid::Uid(aEvaluatorId),aComparisonOp);
		}
	
	if(aFlag & KSetServiceId)
		{
		aFilter.SetServiceId(TUid::Uid(aServiceId),aComparisonOp);
		}
	
	if(aFlag & KSetServerSid)
		{
		aFilter.SetServerSid(TSecureId(aServerSid),aComparisonOp);
		}
		
	if(aFlag & KSetFingerprint)
		{
		aFilter.SetFingerprintL(aFingerprint,aComparisonOp);
		}
	
	if(aFlag & KSetClientEntity)
		{
		aFilter.SetClientEntityL(aClientEntity,aComparisonOp);
		}
	
	if(aFlag & KSetMajorPolicyVersion)
		{
		aFilter.SetMajorPolicyVersion(aMajorPolicyVersion,aComparisonOp);
		}
	
	if(aFlag & KSetRecordId)
		{
		aFilter.SetRecordId(aRecordId,aComparisonOp);
		}
	
	if(aFlag & KSetDescription)
		{
		aFilter.SetDescriptionL(aDescription,aComparisonOp);
		}
		
	if(aFlag & KSetResult)
		{
		aFilter.SetResult(aResult,aComparisonOp);
		}
		
	if(aFlag & KSetEvaluatorInfo)
		{
		aFilter.SetEvaluatorInfo(aEvaluatorInfo,aComparisonOp);
		}
	}

	
CDecisionRecord *CUpsDbBaseStep::CreateRecordL(TInt aClientSid, TInt aEvaluatorId, TInt aServiceId, TInt aServerSid, TDesC8 &aFingerprint, TDesC8 &aClientEntity,
											   TDesC &aDecription, TInt aResult, TInt aMajorPolicyVersion, TInt aEvaluatorInfo, TInt aRecordId)
/**
	Creates a decision record from provided parameters.
	
	@param aClientSid	  Secure Id of client application
	@param aEvaluatorId	  UID for the policy evaluator DLL
	@param aServiceId	  UID for service e.g. sms, mms, telephony, gprs
	@param aServerSid	  UID for the system server that the decision applies to
	@param aFingerprint	  Hash of the destination and/or opaque data. Maximum length is 32 bytes.
	@param aClientEntity  The name of the entity within the client that requested the service. Maximum length is 32 bytes. 
	@param aDescription	  A copy of description and/or opaque. 
	@param aResult		  Whether the request should be approved (=1) or denied (=0)
	@param aMajorPolicyVersion The major version of the policy file.
	@param aEvaluatorInfo Policy evaluator specific data
	@param aRecordId	  An auto-incrementing record number.
	
	@return				  A pointer to the newly allocated decision record object, if creation is successful.

 */
	{
	CDecisionRecord *newRecord;
	
	newRecord = CDecisionRecord::NewL(TSecureId(aClientSid),TUid::Uid(aEvaluatorId),TUid::Uid(aServiceId),TSecureId(aServerSid),aFingerprint,aClientEntity,aDecription,aResult,aMajorPolicyVersion,aEvaluatorInfo, aRecordId);
	
	return newRecord;
	}


void CUpsDbBaseStep::StartTimer(TTime &aTime)
/**
	Assigns current time to the provided parameter
	@param aTime Keeps the current time
 */
	{
	aTime.HomeTime();
	}
		

void CUpsDbBaseStep::StopTimer(TTime &aStartTime, TInt64 &aElapsedTime)
/**
	Substracts the provided time from current time and returns the elapsed time.
	@param aStartTime First measured time
	@param aElapsedTime The elapsed time from the first measurement
 */
	{
	TTime endTime;
	endTime.HomeTime();
	
	TTimeIntervalMicroSeconds timeDiff = endTime.MicroSecondsFrom(aStartTime);
	aElapsedTime = timeDiff.Int64();
	
	if(iLog)
		{
		_LIT8(KMessage,"%Ld");
		iBuffer.AppendFormat(KMessage,aElapsedTime);
		iFile.Write(iBuffer);
		iFile.Write(_L8(";"));
		iBuffer.Zero();
		}
	}
	

void CUpsDbBaseStep::FillTableL(CDecisionDbW &aUpsDb, TInt aRecordNum, TInt aClientNum, TInt aServiceNum, TInt aServerNum, TInt aEvaluatorNum)
/**
	Fill the decision table with a specified number of decision records.
	The parameteres are used as seeds to create sample decision records. 
	
	@param aUpsDb		 Handle to the database
	@param aRecordNum	 Number of records which will be inserted into the database
	@param aClientNum	 Number of different clients 
	@param aServiceNum	 Number of different services provided by each server
	@param aServerNum	 Number of different servers
	@param aEvaluatorNum Number of different policy evaluator DLLs
 */
	{
	TSecureId clientId = TSecureId(0);
	TSecureId serverId = TSecureId(0);
	TUid serviceId	 =TUid::Null();
	TUid evaluatorId =TUid::Null();
	
	TUint8 result = 0;
	TUint16 policyMajorVersion = 2;
	
	TBuf8<KDbMaxName> bufFP;
	TBuf8<KDbMaxName> bufCE;
	TBuf<KDbMaxName> bufD;
	
	CDecisionRecord* newRecord = NULL;
	
	for(TInt i=0; i<aRecordNum; i++)
		{
		bufFP.AppendFormat(KFingerprint,i);
		bufCE.AppendFormat(KClientEntity,i);
		bufD.AppendFormat(KDescription,i);
		
		CMD5* md5FP = CMD5::NewL();
		CleanupStack::PushL(md5FP);
		
		CMD5* md5CE = CMD5::NewL();
		CleanupStack::PushL(md5CE);
		
		clientId=TSecureId(i%aClientNum + aRecordNum);
		serviceId=TUid::Uid(i%aServiceNum + 10000 + aRecordNum);
		serverId =TSecureId(i%aServerNum + 100000 + aRecordNum);
		evaluatorId = TUid::Uid(i%aEvaluatorNum + 1000000 + aRecordNum);
		
		result = i%2;
		
		newRecord = CDecisionRecord::NewLC(clientId,evaluatorId,serviceId,serverId,md5FP->Hash(bufFP),md5CE->Hash(bufCE),bufD, result,policyMajorVersion, 0);
		
		aUpsDb.CreateDecisionL(*newRecord);
		
		CleanupStack::PopAndDestroy(newRecord);
		CleanupStack::PopAndDestroy(2); //md5FP and md5CE
		
		bufFP.Zero();
		bufCE.Zero();
		bufD.Zero();
		}
	}
	

CDecisionDbW *CUpsDbBaseStep::CreateDatabaseLC(TDesC& aDbLocation, RFs& aFs)
/**
	Create a writable decision database object. Prints the consumed time to perform this operation.
	
	@param aDbLocation Where the database will be created
 */
	{
	TTime startTime;
	//Timer is started
	StartTimer(startTime);
	
	//Database is created
	CDecisionDbW *upsDb = CDecisionDbW::NewLC(aDbLocation, aFs);
	
	//Timer is stopped
	TInt64 elapsedTime = 0;
	StopTimer(startTime,elapsedTime);
	
	INFO_PRINTF2(_L("Decision database created in %Ld microseconds."), elapsedTime);
	
	return upsDb;
	}
	

CDecisionRecord *CUpsDbBaseStep::GetDecisionLC(CDecisionDbW &aUpsDb, CDecisionFilter &aFilter)
/**
	Get a decision record from the database. This function uses single record lookup method (RDbTable).
	Prints the consumed time to perform this operation.
	
	@param aUpsDb Handle to the database
	@param aFilter Filter
 */
	{
	TTime startTime;
	//Timer is started
	StartTimer(startTime);
	
	CDecisionRecord *retRecord = NULL;
	retRecord = aUpsDb.GetDecisionL(aFilter);
	if(retRecord)
		{
		CleanupStack::PushL(retRecord);
		}
	
	//Timer is stopped
	TInt64 elapsedTime = 0;
	StopTimer(startTime,elapsedTime);
	
	if(retRecord)
		{
		INFO_PRINTF2(_L("A single record retrieved in %Ld microseconds."), elapsedTime);
		}
	
	return retRecord;
	}
	

void CUpsDbBaseStep::GetDecisionsL(CDecisionDbW &aUpsDb, CDecisionFilter &aFilter, TBool aCancel, RPointerArray<CDecisionRecord>& aRecordList)
/**
   Get a set of decision records from the database. Uses multiple records lookup method (RDbView).
   Prints the consumed time to perform this operation.
 */
	{
	TTime startTime;
	//Timer is started
	StartTimer(startTime);
	
	CDecisionView *dbView = aUpsDb.CreateViewL(aFilter);
	CleanupStack::PushL(dbView);

	dbView->EvaluateView(iWaiter->iStatus);
	TInt num = 0;
	if(aCancel)
		{
		dbView->Cancel();
		iWaiter->WaitActiveL(KErrCancel);
		}
	else
		{
		iWaiter->WaitActiveL(KErrNone);
		CDecisionRecord *record = NULL;		
		
		while((record = dbView->NextDecisionL()) != NULL)
			{
			CleanupStack::PushL(record);
			num++;
			aRecordList.AppendL(record);
			CleanupStack::Pop(record);
			}
		}
	
	//Timer is stopped
	TInt64 elapsedTime = 0;
	StopTimer(startTime,elapsedTime);
	
	INFO_PRINTF3(_L("%d records retrieved in %Ld microseconds."), num, elapsedTime);
	CleanupStack::PopAndDestroy(dbView);
	}
	

void CUpsDbBaseStep::RemoveDecisionsL(CDecisionDbW &aUpsDb, CDecisionFilter &aFilter)
/**
	Delete a set of records. Prints the consumed time to perform this operation.
 */
	{
	TTime startTime;
	//Timer is started
	StartTimer(startTime);
	
	aUpsDb.RemoveDecisionsL(aFilter);
	
	//Timer is stopped
	TInt64 elapsedTime = 0;
	StopTimer(startTime,elapsedTime);
	
	INFO_PRINTF2(_L("A set of records deleted in %Ld microseconds."), elapsedTime);
	}
	
	
void CUpsDbBaseStep::RemoveAllDecisionsL(CDecisionDbW &aUpsDb, RFs& aFs)
/**
	Deletes all records in the database. Actually delete all database.
	Prints the consumed time to perform this operation.
 */
	{
	TTime startTime;
	//Timer is started
	StartTimer(startTime);
	
	aUpsDb.DeleteDatabaseL(aFs);
	
	//Timer is stopped
	TInt64 elapsedTime = 0;
	StopTimer(startTime,elapsedTime);
	
	INFO_PRINTF2(_L("All records deleted in %Ld microseconds."), elapsedTime);
	}
	

void CUpsDbBaseStep::InsertRecordL(CDecisionDbW &aUpsDb, CDecisionRecord &aRecord)
/**
	Inserts the provided record into the decision database. Prints the consumed time to perform this operation.
 */
	{
	TTime startTime;
	//Timer is started
	StartTimer(startTime);
	
	aUpsDb.CreateDecisionL(aRecord);
	
	//Timer is stopped
	TInt64 elapsedTime = 0;
	StopTimer(startTime,elapsedTime);
	
	INFO_PRINTF2(_L("A record inserted in %Ld microseconds."), elapsedTime);
	}
	

void CUpsDbBaseStep::ReadConfigurationFile(TDes &aKeyNotFound)
/**
	Reads the key fileds from the ini file which are used to fill table with a specified number of
	sample records.
	@param aKeyNotFound The appended list of the key names which have not been found
 */
	{
	
	if(EFalse == GetIntFromConfig(ConfigSection(),KRecordNum, iRecordNum))
		{
		aKeyNotFound.Append(KRecordNum);
		aKeyNotFound.Append(_L(" "));
		}
	else if(EFalse == GetIntFromConfig(ConfigSection(),KServerNum, iServerNum))
		{
		aKeyNotFound.Append(KServerNum);
		aKeyNotFound.Append(_L(" "));
		}
	else if(EFalse == GetIntFromConfig(ConfigSection(),KServiceNum, iServiceNum))
		{
		aKeyNotFound.Append(KServiceNum);
		aKeyNotFound.Append(_L(" "));
		}
	else if(EFalse == GetIntFromConfig(ConfigSection(),KClientNum, iClientNum))
		{
		aKeyNotFound.Append(KClientNum);
		aKeyNotFound.Append(_L(" "));
		}
	else if(EFalse == GetIntFromConfig(ConfigSection(),KEvaluatorNum, iEvaluatorNum))
		{
		aKeyNotFound.Append(KEvaluatorNum);
		aKeyNotFound.Append(_L(" "));
		}
	else if(EFalse == GetIntFromConfig(ConfigSection(),KRetrieveNum, iRetrieveNum))
		{
		aKeyNotFound.Append(KRetrieveNum);
		aKeyNotFound.Append(_L(" "));
		}
	}


TInt CUpsDbBaseStep::ConvertNumber(TDesC& aError)
/**
	Converts error string  to number
	@param aError Error string
*/	
		{
		TInt error = KErrNotFound;
		
		if(!aError.CompareF(KStrErrNone))
			{
			error = KErrNone;	
			}
		else if(!aError.CompareF(KStrErrUpsBadFingerprintLength))
			{
			error = KErrUpsBadFingerprintLength;
			}
		else if(!aError.CompareF(KStrErrUpsMissingArgument))
			{
			error = KErrUpsMissingArgument;
			}
		else if(!aError.CompareF(KStrErrUpsBadClientEntityLength))
			{
			error = KErrUpsBadClientEntityLength;
			}
			
		return error;
		}
		
static void PopulateFingerPrint(TDes8& aBuf, TInt aStart)
	{
	TUint8 *ptr = (TUint8 *)aBuf.Ptr();
	
	TInt pos;
	TInt value;
	
	for(pos = 0, value = aStart; pos < KUpsMaxFingerprintLength; ++pos, ++value )
		{
		ptr[pos] = value;
		}
	aBuf.SetLength(pos);	
	}

void CUpsDbBaseStep::TestFingerprintValuesL(CDecisionDbW& aUpsDb)
/**
	Creates a number of decision records by using all characters 
	between 0x01 and 0xFF in Fingerprint value.
 */

	{
	TInt clientId		= 1;
	TInt serverId		= 2;
	TInt serviceId		= 3;
	TInt evaluatorId	= 4;
	TInt evaluatorInfo	= 5;
	TInt result			= 0;
	TInt policyVersion	= 9;
	TInt recordId		= 0;
	
	CDecisionRecord *record = NULL;
	CDecisionFilter *filter = NULL;
	RPointerArray<CDecisionRecord> recordList;	
	TBuf8<KUpsMaxFingerprintLength> bufFp;
	TBuf8<KUpsMaxClientEntityLength> bufCe;
	TBuf<KUpsDescriptionLength> bufDes;
	bufFp.SetLength(KUpsMaxFingerprintLength);
	bufCe.SetLength(KUpsMaxClientEntityLength);
	TInt flag = KSetClientSid|KSetEvaluatorId|KSetServiceId|KSetServerSid|KSetFingerprint|KSetClientEntity;
	TInt pos = 0;
	
	for(pos=0; pos <= KUpsDescriptionLength+1; pos+= KUpsMaxFingerprintLength)
		{
		PopulateFingerPrint(bufFp, pos);
		record = CreateRecordL(++clientId,++evaluatorId,++serviceId,++serverId, bufFp, bufCe, bufDes, result, policyVersion, evaluatorInfo, recordId);
		CleanupStack::PushL(record);
		InsertRecordL(aUpsDb,*record);
		
		filter = CreateFilterLC(flag,clientId,evaluatorId,serviceId,serverId,bufFp,bufCe,policyVersion,recordId,bufDes,result,evaluatorInfo,EEqual);
		CleanupResetAndDestroyPushL(recordList);
		GetDecisionsL(aUpsDb, *filter, EFalse, recordList);
		if(recordList.Count() != 1)
			{
			SetTestStepResult(EFail);
			CleanupStack::PopAndDestroy(3, record);
			break;
			}		
		CleanupStack::PopAndDestroy(3, record);
		}	
	}


void CUpsDbBaseStep::CompactDatabaseL(CDecisionDbW& aUpsDb, TBool aCancel)
/**
	Compacts the decision database.
 */
	{
	TTime startTime;
	//Timer is started
	StartTimer(startTime);
	
	CDecisionDbCompactor *dbCompact = aUpsDb.PrepareCompactionLC();
	dbCompact->Compact(iWaiter->iStatus);
	
	if(aCancel)
		{
		dbCompact->Cancel();
		iWaiter->WaitActiveL(KErrCancel);
		}
	else
		{
		iWaiter->WaitActiveL(KErrNone);		
		}
		
	//Timer is stopped
	TInt64 elapsedTime = 0;
	StopTimer(startTime,elapsedTime);
			
	if(iWaiter->iStatus == 0)
		{
		INFO_PRINTF2(_L("Compaction has been achieved in %Ld microseconds."), elapsedTime);
		}
	else
		{
		if(!aCancel && iWaiter->iStatus!=KErrCancel)
			{
			ERR_PRINTF1(_L("Failed to compact database!"));
			SetTestStepResult(EFail);		
			}
		}
		
	CleanupStack::PopAndDestroy(dbCompact);
	}


void CUpsDbBaseStep::UpdateRecordL(CDecisionDbW& aUpsDb, CDecisionFilter& aFilter, CDecisionRecord& aRecord)
/**
	Updates a decision record from the decision database.
	
	@param aUpsDb A pointer to the decision database object
	@param aFilter Filter used to find the record which will be updated
	@param aRecord New record containing the values which will be updated
 */
	{
	TTime startTime;
	//Timer is started
	StartTimer(startTime);
	
	if(EFalse == aUpsDb.UpdateDecisionL(aFilter,aRecord))
		{
		SetTestStepResult(EFail);
		User::Leave(KErrNotFound);
		}
	else
		{
		//Timer is stopped
		TInt64 elapsedTime = 0;
		StopTimer(startTime,elapsedTime);
		INFO_PRINTF2(_L("The record was updated in %Ld microseconds."), elapsedTime);	
		}
		
	}
