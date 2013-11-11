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
* Performs UPS Database Performance Tests
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
// The rest of the code 
	
	
CUpsDbStepPerformance::CUpsDbStepPerformance()
	{
     // Destructor Code
    delete iWaiter;
	delete iScheduler;
	}
	
CUpsDbStepPerformance::~CUpsDbStepPerformance()
	{
	//Constructor Code
	} 
	
TVerdict CUpsDbStepPerformance::doTestStepPreambleL()
	{
	TBuf<KUpsDescriptionLength> keyNotFound;
	ReadConfigurationFile(keyNotFound);
	
	if(keyNotFound.Length())
    	{
    	ERR_PRINTF2(_L("Failed to read '%S' key of configuration"),&keyNotFound);
    	SetTestStepResult(EFail);
    	User::Leave(KErrNotFound);	
    	}
           		
      //Create a database name
     User::LeaveIfError(iFs.Connect());
	 User::LeaveIfError(iFs.CreatePrivatePath(EDriveC));
	 User::LeaveIfError(iFs.PrivatePath(iDbLocation));
	 iPerformanceLog.Copy(iDbLocation);
	 iDbLocation.Append(KDbName);
	 iPerformanceLog.Append(KLogName);
	 
	 //Delete the database if left from previous execution
	iFs.Delete(iDbLocation);
	
	//Open the log file where the performance results are written in CSV format
	TInt err = iFile.Open(iFs,iPerformanceLog,EFileWrite);
	if(KErrNotFound == err)
		{
		User::LeaveIfError(iFile.Replace(iFs,iPerformanceLog,EFileWrite));
		//Write the column headers to the log file
		PrintHeader();
		}
	else if(KErrNone != err)
		{
		User::Leave(err);
		}
		
	TInt pos = 0;
	iFile.Seek(ESeekEnd, pos);
	iLog = ETrue;
	
	//Create and install active scheduler 
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	iWaiter = new(ELeave)CActiveWaiter(Logger());
		
     return TestStepResult();
	}
	
TVerdict CUpsDbStepPerformance::doTestStepL()
	{
    INFO_PRINTF1(_L("Running Step: Measuring Database Performance"));
	
	//Create a decision database
	CDecisionDbW* decisionDb = CreateDatabaseLC(iDbLocation, iFs);
	
	//Fill the table with decision records	
	FillTableL(*decisionDb,iRecordNum,iClientNum,iServiceNum,iServerNum,iEvaluatorNum);
	
	//Get a set of decision records with ClientSid
	TUid clientId = TUid::Uid(iRetrieveNum%iClientNum + iRecordNum);
	CDecisionFilter* filter = CDecisionFilter::NewLC();
	filter->SetClientSid(clientId,EEqual);
	
	INFO_PRINTF2(_L("Lookup for ClientSid=%d"),clientId.iUid);
	RPointerArray<CDecisionRecord> recordList;
	CleanupResetAndDestroyPushL(recordList);

	GetDecisionsL(*decisionDb, *filter, EFalse, recordList);	
	CleanupStack::PopAndDestroy(2, filter);
	
	//Get a set of decision records with EvaluatorId
	TUid evaluatorId = TUid::Uid(iRetrieveNum%iEvaluatorNum + 1000000 + iRecordNum);
	filter = CDecisionFilter::NewLC();
	filter->SetEvaluatorId(evaluatorId,EEqual);
	CleanupResetAndDestroyPushL(recordList);
	INFO_PRINTF2(_L("Lookup for EvaluatorId=%d"),evaluatorId.iUid);
	
	GetDecisionsL(*decisionDb, *filter, EFalse, recordList);
	CleanupStack::PopAndDestroy(2, filter);
	
	//Get a set of decision records with ServiceId
	TUid serviceId = TUid::Uid(iRetrieveNum%iServiceNum + 10000 + iRecordNum);
	filter = CDecisionFilter::NewLC();
	filter->SetServiceId(serviceId,EEqual);
	CleanupResetAndDestroyPushL(recordList);
	INFO_PRINTF2(_L("Lookup for ServiceId=%d"),serviceId.iUid);
	
	GetDecisionsL(*decisionDb, *filter, EFalse, recordList);
	CleanupStack::PopAndDestroy(2, filter);
	
	//Get a set of decision records with ServerSid
	TUid serverId = TUid::Uid(iRetrieveNum%iServerNum + 100000 + iRecordNum);
	filter = CDecisionFilter::NewLC();
	filter->SetServerSid(serverId,EEqual);
	CleanupResetAndDestroyPushL(recordList);
	INFO_PRINTF2(_L("Lookup for ServerSid=%d"),serverId.iUid);
	
	GetDecisionsL(*decisionDb, *filter, EFalse, recordList);
	CleanupStack::PopAndDestroy(2, filter);
	
	//Lookup for Fingerprint
	TBuf8<KDbMaxName>bufFP;
	CMD5* md5FP = CMD5::NewL();
	CleanupStack::PushL(md5FP);
	
	bufFP.AppendFormat(KFingerprint,iRetrieveNum);
	filter = CDecisionFilter::NewLC();
		
	filter->SetFingerprintL(md5FP->Hash(bufFP),EEqual);
	CleanupResetAndDestroyPushL(recordList);
	INFO_PRINTF1(_L("Lookup for a Fingerprint"));
	
	GetDecisionsL(*decisionDb, *filter, EFalse, recordList);
	CleanupStack::PopAndDestroy(2, filter);
	
	//Lookup for ClientEntity
	TBuf8<KDbMaxName>bufCE;
	CMD5* md5CE = CMD5::NewL();
	CleanupStack::PushL(md5CE);
	
	bufCE.AppendFormat(KClientEntity,iRetrieveNum);
	filter = CDecisionFilter::NewLC();
	
	filter->SetClientEntityL(md5CE->Hash(bufCE),EEqual);
	CleanupResetAndDestroyPushL(recordList);
	INFO_PRINTF1(_L("Lookup for a ClientEntity"));
	
	GetDecisionsL(*decisionDb, *filter, EFalse, recordList);
	CleanupStack::PopAndDestroy(2, filter);
	
	//Get a single record
	md5FP->Reset();
	md5CE->Reset();
	filter = CDecisionFilter::NewLC(clientId,evaluatorId,serviceId,serverId,md5FP->Hash(bufFP),md5CE->Hash(bufCE),2);
	
	INFO_PRINTF1(_L("Lookup for a single,unique record"));
	
	CDecisionRecord *retRecord (0); 
	retRecord = GetDecisionLC(*decisionDb,*filter);
	CleanupStack::PopAndDestroy(retRecord);
	CleanupStack::PopAndDestroy(filter);
	
	//Delete a set of records with ClientSid
	filter = CDecisionFilter::NewLC();
	filter->SetClientSid(clientId,EEqual);
	
	INFO_PRINTF2(_L("Delete a set of records where ClientSid=%d"),clientId.iUid);
	
	RemoveDecisionsL(*decisionDb,*filter);
	CleanupStack::PopAndDestroy(filter);
	
	//Delete a set of records with ServiceId
	filter = CDecisionFilter::NewLC();
	filter->SetServiceId(serviceId,EEqual);
	
	INFO_PRINTF2(_L("Delete a set of records where ServiceId=%d"),serviceId.iUid);
	
	RemoveDecisionsL(*decisionDb,*filter);
	CleanupStack::PopAndDestroy(filter);
	
	//Delete a set of records with ServerSid
	filter = CDecisionFilter::NewLC();
	filter->SetServerSid(serverId,EEqual);
	
	INFO_PRINTF2(_L("Delete a set of records where ServerSid=%d"),serverId.iUid);
	
	RemoveDecisionsL(*decisionDb,*filter);
	CleanupStack::PopAndDestroy(filter);
	
	//Insert a record
	TBuf<KDbMaxName>bufDes;
	bufDes.AppendFormat(KDescription,iRetrieveNum);
	CDecisionRecord* record = CDecisionRecord::NewLC(clientId,evaluatorId,serviceId,serverId,md5FP->Hash(bufFP),md5CE->Hash(bufCE),bufDes,1,2,0);
	
	INFO_PRINTF1(_L("Insert a decision record"));
	
	InsertRecordL(*decisionDb,*record);
	CleanupStack::PopAndDestroy(record);
	
	//Delete All Records
	INFO_PRINTF1(_L("Delete all decision records"));
	RemoveAllDecisionsL(*decisionDb, iFs);
	
	CleanupStack::PopAndDestroy(3,decisionDb);//md5FP, MD5CE & decisionDb
	
	iFile.Write(_L8("\n"));
	
    return TestStepResult();
    }
      
TVerdict CUpsDbStepPerformance::doTestStepPostambleL()
	{
	iFile.Close();
	iFs.Close();
      // Override base class virtual function
	return TestStepResult();
	}

void CUpsDbStepPerformance::PrintHeader()
	{
	iFile.Write(_L8("\"Create Database\";"));
	iFile.Write(_L8("\"Lookup with ClientSid\";"));
	iFile.Write(_L8("\"Lookup with EvaluatorId\";"));
	iFile.Write(_L8("\"Lookup with ServiceId\";"));
	iFile.Write(_L8("\"Lookup with ServerSid\";"));
	iFile.Write(_L8("\"Lookup with Fingerprint\";"));
	iFile.Write(_L8("\"Lookup with ClientEntity\";"));
	iFile.Write(_L8("\"Lookup Single Record\";"));
	iFile.Write(_L8("\"Delete with ClientSid\";"));
	iFile.Write(_L8("\"Delete with ServiceId\";"));
	iFile.Write(_L8("\"Delete with ServerSid\";"));
	iFile.Write(_L8("\"Insert A Record\";"));
	iFile.Write(_L8("\"Delete All Records\";"));
	iFile.Write(_L8("\n"));
	}

