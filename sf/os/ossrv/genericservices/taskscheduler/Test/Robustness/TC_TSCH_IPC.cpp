// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 

#include <e32test.h>
#include <e32math.h>
#include <e32debug.h>
#include "CSCHCODE.H"
#include "SCHINFO.H"
#include <bsul/bsul.h>
#include <schinfointernal.h>
using namespace BSUL;


LOCAL_D RTest				Test (_L("TC_TSCH_IPC"));

_LIT(KServerName, "!TaskScheduler");
_LIT(KServerProcess, "SchExe");

struct TTestInfo
	{
	TInt iFunction;
	TInt iType;
	TInt iArgCount;
	};

struct TExitDetails
	{
	TExitCategoryName iCategory;
	TExitType iExitType;
	TInt iReason;
	};

const TInt KAsynchDelay = 500000;

const TInt KFunctionNumbers [] = {	ERegisterClient,
									ECreateTimeSchedule,
									ECreateConditionSchedule,
									EScheduleTask,
									ERemoveTask,
									EDisableSchedule,
									EEnableSchedule,
									EDeleteSchedule,
									EEditTimeSchedule,
									EEditConditionSchedule,
									EGetScheduleInfo,
									EGetTimeScheduleData,
									EGetConditionScheduleData,
									EGetTaskData,
									ECountSchedules,
									ECountTasks,
									EGetTaskRefs,
									EGetTask,
									EGetTaskDataSize,
									EGetScheduleRefs,
									EDeleteTask,
									EGetSchedulerItemRefAndNextDueTime,
									EGetScheduleType};

const TInt KNumFunctions = sizeof(KFunctionNumbers)/sizeof(KFunctionNumbers[0]);

//===============================================================================

TBool IsFunctionAsynchronous(TInt aFunc)
	{
	TBool asynch = EFalse;
	switch(aFunc)
		{			
			
		default:
			break;
		}
	return asynch;
	}

class RIpcFuzzTest : public RSessionBase
{
public: // Constructors and destructor

	/**
		* Constructor for performing 1st stage construction
		*/
	RIpcFuzzTest();

	/**
		* Destructor.
		*/
	~RIpcFuzzTest();

	/**
		* Performs test steps
		*/
	
	void RunTestL(const TDesC& aTargetSrvName, TInt aFunc, 
				TInt aTestType, TInt aArgCount);
	
private:
	TInt Fuzz(TInt aMsg, TInt aArgCount);
	TInt FuzzL(TInt aMsg, TInt aArgCount);
	TInt Fuzz8L(TInt aMsg, TInt aArgCount);
};

RIpcFuzzTest::RIpcFuzzTest()
	{
	// No implementation required
	}


RIpcFuzzTest::~RIpcFuzzTest()
	{
	Close();	
	}

TInt RIpcFuzzTest::Fuzz(TInt aMsg, TInt aArgCount)
	{
	TIpcArgs args;
	
	for(TInt i = 0; i < aArgCount;i++)
		{
		args.Set(i,Math::Random());
		}	
	
	TInt ret;
	
	if(IsFunctionAsynchronous(aMsg))
		{
		ret = Send(aMsg, args);
		User::After(KAsynchDelay);
		}
	else
		{
		ret = SendReceive(aMsg, args);
		}
	return ret;
	}

TInt RIpcFuzzTest::Fuzz8L(TInt aMsg, TInt aArgCount)
	{
	HBufC8* buf = HBufC8::NewLC(255);
	TPtr8 ptr = buf->Des();
	ptr.Fill(Math::Random(),255);
	
	TIpcArgs args;
	
	for(TInt i = 0; i < aArgCount;i++)
		{
		args.Set(i,&ptr);
		}
	
	TInt ret;
	
	if(IsFunctionAsynchronous(aMsg))
		{
		ret = Send(aMsg, args);
		User::After(KAsynchDelay);
		}
	else
		{
		ret = SendReceive(aMsg, args);
		}

	CleanupStack::PopAndDestroy(buf);
	return ret;
	}

TInt RIpcFuzzTest::FuzzL(TInt aMsg, TInt aArgCount)
	{
	HBufC* buf = HBufC::NewLC(255);
	TPtr ptr = buf->Des();
	ptr.Fill(Math::Random(),255);

	TIpcArgs args;
	
	for(TInt i = 0; i < aArgCount;i++)
		{
		args.Set(i,&ptr);
		}
	
	TInt ret;
	
	if(IsFunctionAsynchronous(aMsg))
		{
		ret = Send(aMsg, args);
		User::After(KAsynchDelay);
		}
	else
		{
		ret = SendReceive(aMsg, args);
		}

	CleanupStack::PopAndDestroy(buf);
	return ret;
	}

void RIpcFuzzTest::RunTestL(const TDesC& aTargetSrvName, 
							TInt aFunc, TInt aTestType, TInt aArgCount)
	{
		TVersion version(0,0,0);
	
		TInt err = CreateSession(aTargetSrvName, version);
		
		User::LeaveIfError(err);
	
		switch(aTestType)
			{
			case 0:
				Fuzz(aFunc,aArgCount);
				break;
				
			case 1:
				Fuzz8L(aFunc,aArgCount);
				break;
				
			case 2:
				FuzzL(aFunc,aArgCount);
				break;			
			}
	}

TInt KillProcess(const TDesC& aProcessName)
	{
	TFullName name;

	RDebug::Print(_L("Find and kill \"%S\" process.\n"), &aProcessName);

	TBuf<64> pattern(aProcessName);
	TInt length = pattern.Length();
	pattern += _L("*");
	TFindProcess procFinder(pattern);

	while (procFinder.Next(name) == KErrNone)
		{
		if (name.Length() > length)
			{//If found name is a string containing aProcessName string.
			TChar c(name[length]);
			if (c.IsAlphaDigit() ||
				c == TChar('_') ||
				c == TChar('-'))
				{
				// If the found name is other valid application name
				// starting with aProcessName string.
				RDebug::Print(_L(":: Process name: \"%S\".\n"), &name);
				continue;
				}
			}
		RProcess proc;
		if (proc.Open(name) == KErrNone)
			{
			proc.Kill(0);
			RDebug::Print(_L("\"%S\" process killed.\n"), &name);
			}
		proc.Close();
		}
	return KErrNone;
	}


TInt FuzzServerL(TAny* aTestInfo)
	{
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt err=KErrNoMemory;
   	if (cleanup)
   		{
   		
		TTestInfo* info = (TTestInfo*)aTestInfo;
		RIpcFuzzTest fuzzer;
		
		TRAP(err,fuzzer.RunTestL(KServerName,info->iFunction
				,info->iType, info->iArgCount));
		
		fuzzer.Close();

   		delete cleanup;
   		}
   	return err;
	}


void TestServerApi(TInt aFunctionNumber,
			TInt aTestType,TInt aArgCount, TExitDetails& aExitDetails)
	{
    
    TTestInfo testInfo;
	testInfo.iFunction = aFunctionNumber;
	testInfo.iType = aTestType;
	testInfo.iArgCount = aArgCount;
    
    RThread thread;	
    _LIT(KThreadName,"FuzzerThread" );
	thread.Create(KThreadName,&FuzzServerL, KDefaultStackSize, NULL,&testInfo);
	
	TRequestStatus threadStat;
	thread.Logon(threadStat);
	
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);
	
	thread.Resume();
	
	User::WaitForRequest(threadStat);
	
	User::SetJustInTime(jit);

	aExitDetails.iCategory = thread.ExitCategory();
	aExitDetails.iReason = thread.ExitReason();
	aExitDetails.iExitType = thread.ExitType();
	
	thread.Close();

	}


TInt LaunchServer(RProcess& aServer)
	{
	
	Test.Printf(_L("Launching Task Scheduler...\n"));
	
	const TUid KServerUid3 = {0x10005399};	
	const TUidType serverUid(KNullUid,KNullUid,KServerUid3);
	
	TInt err = aServer.Create(KServerProcess, _L(""),serverUid);
	 
	if(err == KErrNone)
		{   
		aServer.SetPriority(EPriorityForeground);  
		 
		//Start server and wait until it is running
		TRequestStatus serverStat;
		aServer.SetJustInTime(false);   
		aServer.Resume(); 
		    
		aServer.Rendezvous(serverStat);
		User::WaitForRequest(serverStat);
		}
	 
	 return err;
	    
	}

void PrintTestMessage(TInt iFunc, TInt iType, TInt iArgCount)
	{
	switch(iType)
		{
		case 0:
			Test.Printf(_L("\nFuzz Test on function number %d using random Int data. Number of Args = %d"), iFunc, iArgCount);
			break;
			
		case 1:
			Test.Printf(_L("\nFuzz Test on function number %d using random Des8 data. Number of Args = %d"), iFunc, iArgCount);
			break;
			
		case 2:
			Test.Printf(_L("\nFuzz Test on function number %d using random Des data. Number of Args = %d"), iFunc, iArgCount);
			break;
			
		}
	
	}

/**
Invoke the tests
*/
/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-4004
@SYMTestCaseDesc	    Tests SchSvr APIs for IPC Robustness
@SYMTestPriority 	    High
@SYMTestActions  	    The function calls each of the SchSvr APIs through a custom session object
						passing random TInt, Des8 and Des16 data .
@SYMTestExpectedResults The server should be robust to all malformed messages and should not
						hang or panic.
@SYMDEF                	PDEF118811
*/
LOCAL_C void DoFuzzTestsL ()
    {  
    Test.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-4004 "));
    RProcess server;
    
    User::LeaveIfError(LaunchServer(server));

    TExitDetails exitDetails;
      
    for(TInt i = 0;i< KNumFunctions;i++)
    	{
    	
    	//Carry out each type of test
    	for(TInt testType = 0; testType < 3;testType++)
    		{
       		//Carry out each test with number of arguments 1 - 4
    		for(TInt argCount = 1;argCount <= 4;argCount++)
    			{
    			PrintTestMessage(KFunctionNumbers[i], testType, argCount);

    			TestServerApi(KFunctionNumbers[i], testType, argCount, exitDetails);
		    	//Kill the server process and verify that it was still running
		    	//If the server was already dead it would return the reason it exited
	    		if(server.ExitType() != EExitPending)
	    			{
	    			server.Kill(0);
	    			TInt exitReason = server.ExitReason();
	    			server.Close();
	    			Test(exitReason == 0);    	
	    			User::LeaveIfError(LaunchServer(server));
	    			}
    			}
	    	
    		Test.Printf(_L("\nFuzz Test Successful\n"));
    		}
    	}
    }


/*
 * SERVER API TESTING
 */

typedef void (*TestFunction)();

class RClientMessageTestSession : public RSessionBase
	{
public:
	TInt Connect();
	
	TInt TestRegisterClient(const TDesC& aData0, TInt aArg1);
	
	TInt TestCreateTimeSchedule(TDesC8& aData0,TDesC8& aData1,TDes8& aData2);
	TInt TestCreateCondSchedule(TDesC8& aData0,TDesC8& aData1,TDesC8& aData2,TDes8& aData3);
	
	TInt TestScheduleTask(TDesC8& aData0,TInt aArg1,TDes8& aData2,TDesC& aData3);
	TInt TestDeleteTask(TInt aArg0);
	TInt TestDeleteSchedule(TInt aArg0);
	TInt TestDisableSchedule(TInt aArg0);
	TInt TestEnableSchedule(TInt aArg0);
	
	TInt TestEditTimeSchedule(TInt aArg0, TInt aArg1, TDes8& aData2);
	TInt TestEditCondSchedule(TInt aArg0, TInt aArg1, TDes8& aData2, TDes8& aData3);
	
	TInt TestGetScheduleRefs(TInt aArg0, TInt aArg1, TDes8& aData2);
	TInt TestCountSchedules(TDes8& aData0, TInt aArg1);
	TInt TestGetScheduleType(TInt aArg0,TDes8& aData1);
	TInt TestGetScheduleInfo(TInt aArg0, TDes8& aData1, TDes8& aData2);
	TInt TestGetTimeScheduleData(TInt aArg0, TDes8& aData1, TDes8& aData2);
	TInt TestGetCondScheduleData(TInt aArg0, TDes8& aData1, TDes8& aData2, TDes8& aData3);
	TInt TestGetScheduleItemRef(TInt aArg0, TDes8& aData1, TDes8& aData2);
		
	TInt TestGetTaskData(TInt aArg0, TDes8& aData1, TDes8& aData2);
	TInt TestGetTaskRefs(TInt aArg0, TInt aArg1,TInt aArg2, TDes8& aData3);
	TInt TestCountTasks(TDes8& aData0, TInt aArg1, TInt aArg2);	
	TInt TestGetTaskInfo(TInt aArg0,TDesC8& aData1,TInt aArg2,TDesC& aData3);
	TInt TestGetTaskDataSize(TInt aArg0,TDesC8& aData1);


	};

static TInt LaunchLogServerL()
	{

	RProcess process;

	TInt err = process.Create(KServerProcess,_L(""));
	 
	if(err == KErrNone)
		{
		 TRequestStatus serverStat;		
		 
		 process.SetJustInTime(EFalse);
		 process.Resume();
		 process.Rendezvous(serverStat);
		 User::WaitForRequest(serverStat);
		}
	
	return err;
	}


TInt RClientMessageTestSession::Connect()
	{
	TInt retry = 2;
	for(;;)
		{
		TInt r = CreateSession(KServerName,TVersion(1,0,0));
		
		if((r != KErrNotFound)&&(r != KErrServerTerminated))
			{
			return r;
			}
		
		if(--retry == 0)
			{
			return r;
			}
		
		r = LaunchLogServerL();
		if((r != KErrNone)&&(r != KErrAlreadyExists))
			{
			return r;
			}	
		}
	}

TInt RClientMessageTestSession::TestRegisterClient(const TDesC& aData0, TInt aArg1)
	{
	return SendReceive(ERegisterClient,TIpcArgs(&aData0, aArg1));
	}

TInt RClientMessageTestSession::TestCreateTimeSchedule(TDesC8& aData0,TDesC8& aData1,TDes8& aData2)
	{
	return SendReceive(ECreateTimeSchedule,TIpcArgs(&aData0, &aData1,& aData2));
	}

TInt RClientMessageTestSession::TestCreateCondSchedule(TDesC8& aData0,TDesC8& aData1,TDesC8& aData2,TDes8& aData3)
	{
	return SendReceive(ECreateConditionSchedule,TIpcArgs(&aData0,&aData1, &aData2, &aData3));
	}

TInt RClientMessageTestSession::TestScheduleTask(TDesC8& aData0,TInt aArg1,TDes8& aData2,TDesC& aData3)
	{
	return SendReceive(EScheduleTask,TIpcArgs(&aData0, aArg1, &aData2, &aData3));
	}

TInt RClientMessageTestSession::TestDeleteTask(TInt aArg0)
	{
	return SendReceive(EDeleteTask,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestDeleteSchedule(TInt aArg0)
	{
	return SendReceive(EDeleteSchedule,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestDisableSchedule(TInt aArg0)
	{
	return SendReceive(EDisableSchedule,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestEnableSchedule(TInt aArg0)
	{
	return SendReceive(EEnableSchedule,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestEditTimeSchedule(TInt aArg0, TInt aArg1, TDes8& aData2)
	{
	return SendReceive(EEditTimeSchedule,TIpcArgs(aArg0, aArg1, &aData2));
	}

TInt RClientMessageTestSession::TestEditCondSchedule(TInt aArg0, TInt aArg1, TDes8& aData2, TDes8& aData3)
	{
	return SendReceive(EEditConditionSchedule,TIpcArgs(aArg0, aArg1, &aData2,&aData3));
	}


TInt RClientMessageTestSession::TestGetScheduleRefs(TInt aArg0, TInt aArg1, TDes8& aData2)
	{
	return SendReceive(EGetScheduleRefs,TIpcArgs(aArg0, aArg1, &aData2));
	}

TInt RClientMessageTestSession::TestCountSchedules(TDes8& aData0, TInt aArg1)
	{
	return SendReceive(ECountSchedules,TIpcArgs(&aData0, aArg1));
	}

TInt RClientMessageTestSession::TestGetScheduleType(TInt aArg0,TDes8& aData1)
	{
	return SendReceive(EGetScheduleType,TIpcArgs(aArg0, &aData1));
	}

TInt RClientMessageTestSession::TestGetScheduleInfo(TInt aArg0, TDes8& aData1, TDes8& aData2)
	{
	return SendReceive(EGetScheduleInfo,TIpcArgs(aArg0, &aData1, &aData2));
	}

TInt RClientMessageTestSession::TestGetTimeScheduleData(TInt aArg0, TDes8& aData1, TDes8& aData2)
	{
	return SendReceive(EGetTimeScheduleData,TIpcArgs(aArg0, &aData1, &aData2));
	}

TInt RClientMessageTestSession::TestGetCondScheduleData(TInt aArg0, TDes8& aData1, TDes8& aData2, TDes8& aData3)
	{
	return SendReceive(EGetConditionScheduleData,TIpcArgs(aArg0, &aData1, &aData2, &aData3));
	}

TInt RClientMessageTestSession::TestGetScheduleItemRef(TInt aArg0, TDes8& aData1, TDes8& aData2)
	{
	return SendReceive(EGetSchedulerItemRefAndNextDueTime,TIpcArgs(aArg0, &aData1, &aData2));
	}

TInt RClientMessageTestSession::TestGetTaskData(TInt aArg0, TDes8& aData1, TDes8& aData2)
	{
	return SendReceive(EGetTaskData,TIpcArgs(aArg0, &aData1, &aData2));
	}

TInt RClientMessageTestSession::TestGetTaskRefs(TInt aArg0, TInt aArg1,TInt aArg2, TDes8& aData3)
	{
	return SendReceive(EGetTaskRefs,TIpcArgs(aArg0, aArg1, aArg2, &aData3));
	}

TInt RClientMessageTestSession::TestCountTasks(TDes8& aData0, TInt aArg1, TInt aArg2)
	{
	return SendReceive(ECountTasks,TIpcArgs(&aData0, aArg1, aArg2));
	}

TInt RClientMessageTestSession::TestGetTaskInfo(TInt aArg0,TDesC8& aData1,TInt aArg2,TDesC& aData3)
	{
	return SendReceive(EGetTask,TIpcArgs(aArg0, &aData1, aArg2, &aData3));
	}

TInt RClientMessageTestSession::TestGetTaskDataSize(TInt aArg0,TDesC8& aData1)
	{
	return SendReceive(EGetTaskDataSize,TIpcArgs(aArg0, &aData1));
	}





TInt TestFunctionLauncherL(TAny* aTestFunction)
	{
   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TInt r=KErrNoMemory;
   	if (cleanup)
   		{
   		TestFunction function = (TestFunction)aTestFunction;

__UHEAP_MARK;  		
 		TRAP(r,function());
__UHEAP_MARKEND;
 		
   		delete cleanup;
   		}
   	return r;
	}


TExitDetails LaunchTestThreadL(const TDesC& aThreadName, TestFunction aFunction)
	{
	RThread thread;	
	thread.Create(aThreadName, &TestFunctionLauncherL, KDefaultStackSize, NULL, (TAny*)aFunction);
	
	TRequestStatus threadStat;
	thread.Logon(threadStat);
	
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);
	
	thread.Resume();
	User::WaitForRequest(threadStat);
	
	User::SetJustInTime(jit);
	 
	TExitDetails exitDetails;
	exitDetails.iCategory = thread.ExitCategory();
	exitDetails.iReason = thread.ExitReason();
	exitDetails.iExitType = thread.ExitType();
	
	return exitDetails;
	}



/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-4005
@SYMTestCaseDesc	    Tests Create Schedule API robustness
@SYMTestPriority 	    High
@SYMTestActions  	    Calls the Create Schedule APIs passing in varying parameters
						to test the robustness of the API				
@SYMTestExpectedResults The server should be robust to badly formed messages and should
						return KErrBadParameter if messages do not fit the constraints 
						defined in the message schema.  
						The server should panic the client with KErrBadDescriptor if an 
						invalid descriptor argument is passed.
@SYMDEF                	PDEF118811
*/
void TestCreateSchedulesL()
	{
	
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	err = session.TestRegisterClient(_L("MyClient2"),-1);	
	Test(err == KErrBadParameter);
	
	TScheduleSettings2 settings;
	TScheduleEntryInfo2 info;
	TInt id;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TScheduleEntryInfo2> pInfo(info);
	TPckg<TInt> pId(id);
	
	settings.iPersists = -1;
	settings.iEntryCount = -1;	
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId);
	Test(err == KErrBadParameter);
	
	settings.iPersists = 0;
	settings.iEntryCount = 1;	
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId);
	Test(err == KErrNone);
	
	TTsTime time;
	TPckg<TTsTime> pTime(time);
	TTaskSchedulerCondition conditions;
	TPckg<TTaskSchedulerCondition> pConditions(conditions);
	
	err = session.TestCreateCondSchedule(pSettings, pConditions, pTime,pId);
	Test(err == KErrNone);
	
	err = session.TestDisableSchedule(id);
	Test(err == KErrNone);
	
	err = session.TestDisableSchedule(249990000);
	Test(err == KErrNotFound);
	
	err = session.TestDisableSchedule(-1);
	Test(err == KErrBadParameter);
	
	err = session.TestDisableSchedule(250000000);
	Test(err == KErrBadParameter);
	
	CleanupStack::PopAndDestroy(&session);
	}

void TestCreateSchedulesPanic1L()
	{
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TScheduleSettings2 settings;
	TScheduleEntryInfo2 info;
	TInt id;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TScheduleEntryInfo2> pInfo(info);
	TPckg<TInt> pId(id);
	
	settings.iPersists = 0;
	settings.iEntryCount = 0;	
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId);
	Test(err == KErrNone);

	CleanupStack::PopAndDestroy(&session);
	}

void TestCreateSchedulesPanic2L()
	{
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TScheduleSettings2 settings;
	TTaskSchedulerCondition conditions;
	TTsTime time;
	TInt id;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TTaskSchedulerCondition> pConditions(conditions);
	TPckg<TTsTime> pTime(time);
	TPckg<TInt> pId(id);
	
	settings.iEntryCount = 2;	
	settings.iPersists = 0;
	
	err = session.TestCreateCondSchedule(pSettings, pConditions, pTime,pId);
	Test(err == KErrNone);
	
	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-4006
@SYMTestCaseDesc	    Tests Edit Schedule API robustness
@SYMTestPriority 	    High
@SYMTestActions  	    Calls the Edit Schedule APIs passing in varying parameters
						to test the robustness of the API				
@SYMTestExpectedResults The server should be robust to badly formed messages and should
						return KErrBadParameter if messages do not fit the constraints 
						defined in the message schema.  
@SYMDEF                	PDEF118811
*/
void TestEditSchedulesL()
	{
	
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TScheduleSettings2 settings;
	TTaskSchedulerCondition conditions;
	TTsTime time;
	TInt id1;
	TInt id2;
	TScheduleInfo schedInfo;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TTaskSchedulerCondition> pConditions(conditions);
	TPckg<TTsTime> pTime(time);
	TPckg<TInt> pId1(id1);
	TPckg<TInt> pId2(id2);
	TPckg<TScheduleInfo> pSchedInfo(schedInfo);
	
	settings.iEntryCount = 1;	
	settings.iPersists = 0;
	
	TScheduleEntryInfo2 info;
	TPckg<TScheduleEntryInfo2> pInfo(info);
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId1);
	Test(err == KErrNone);
	
	err = session.TestEditTimeSchedule(settings.iEntryCount, id1,pInfo );
	Test(err == KErrNone);
	
	err = session.TestEditTimeSchedule(-1, id1,pInfo );
	Test(err == KErrBadParameter);
	
	err = session.TestEditTimeSchedule(10000, id1,pInfo );
	Test(err == KErrBadParameter);
	
	err = session.TestEditTimeSchedule(settings.iEntryCount, -1,pInfo );
	Test(err == KErrBadParameter);
	
	err = session.TestEditTimeSchedule(settings.iEntryCount,300000000,pInfo );
	Test(err == KErrBadParameter);
	
	err = session.TestCreateCondSchedule(pSettings, pConditions, pTime,pId2);
	Test(err == KErrNone);
	
	err = session.TestEditCondSchedule(settings.iEntryCount, id2,pConditions, pTime );
	Test(err == KErrNone);
	
	err = session.TestEditCondSchedule(-231, id2,pConditions, pTime );
	Test(err == KErrBadParameter);
	
	err = session.TestEditCondSchedule(11000, id2,pConditions, pTime );
	Test(err == KErrBadParameter);
	
	err = session.TestEditCondSchedule(settings.iEntryCount, -1,pConditions, pTime );
	Test(err == KErrBadParameter);
	
	err = session.TestEditCondSchedule(settings.iEntryCount, 250000000,pConditions, pTime );
	Test(err == KErrBadParameter);
		
	CleanupStack::PopAndDestroy(&session);

	}

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-4007
@SYMTestCaseDesc	    Tests  Schedule Data API robustness
@SYMTestPriority 	    High
@SYMTestActions  	    Calls the Get Schedule Data APIs passing in varying parameters
						to test the robustness of the API				
@SYMTestExpectedResults The server should be robust to badly formed messages and should
						return KErrBadParameter if messages do not fit the constraints 
						defined in the message schema.  
						The server should panic the client with KErrBadDescriptor if an 
						invalid descriptor argument is passed.
@SYMDEF                	PDEF118811
*/
void TestGetScheduleDataL()
	{
	RClientMessageTestSession session;
		
		TInt err = session.Connect();
		
		CleanupClosePushL(session);
		
		err = session.TestRegisterClient(_L("MyClient"),0);	
		Test(err == KErrNone);
		
		TScheduleSettings2 settings;
		TTaskSchedulerCondition conditions;
		TTsTime time;
		TInt id1;
		TInt id2;
		TScheduleInfo schedInfo;
		
		TPckg<TScheduleSettings2> pSettings(settings);
		TPckg<TTaskSchedulerCondition> pConditions(conditions);
		TPckg<TTsTime> pTime(time);
		TPckg<TInt> pId1(id1);
		TPckg<TInt> pId2(id2);
		TPckg<TScheduleInfo> pSchedInfo(schedInfo);
		
		settings.iEntryCount = 1;	
		settings.iPersists = 0;
		
		TScheduleEntryInfo2 info;
		TPckg<TScheduleEntryInfo2> pInfo(info);
		
		err = session.TestCreateTimeSchedule(pSettings, pInfo, pId1);
		Test(err == KErrNone);
		
		err = session.TestCreateCondSchedule(pSettings, pConditions, pTime,pId2);
		Test(err == KErrNone);
				
		err = session.TestGetScheduleInfo(id1,pSchedInfo, pTime);
		Test(err == KErrNone);
		
		err = session.TestGetScheduleInfo(-1,pSchedInfo, pTime);
		Test(err == KErrBadParameter);
		
		err = session.TestGetScheduleInfo(250000000,pSchedInfo, pTime);
		Test(err == KErrBadParameter);
		
		TInt count;
		TPckg<TInt> pCount(count);
		
		err = session.TestCountSchedules(pCount, -378);
		Test(err == KErrBadParameter);
		
		err = session.TestCountSchedules(pCount, EAllSchedules);
		Test(err == KErrNone);
		
		err = session.TestCountSchedules(pCount, EPendingSchedules);
		Test(err == KErrNone);
		
		err = session.TestCountSchedules(pCount, 3);
		Test(err == KErrBadParameter);
		
		schedInfo.iEntryCount = 1;
		
		err = session.TestGetTimeScheduleData(id1,pSchedInfo, pInfo);
		Test(err == KErrNone);
		
		schedInfo.iEntryCount = 23;
		
		err = session.TestGetTimeScheduleData(id1,pSchedInfo, pInfo);
		Test(err == KErrArgument);
		
		schedInfo.iEntryCount = 1;
		
		err = session.TestGetTimeScheduleData(250000000,pSchedInfo, pInfo);
		Test(err == KErrBadParameter);
		
		err = session.TestGetCondScheduleData(id2,pSchedInfo, pInfo, pTime);
		Test(err == KErrNone);
		
		schedInfo.iEntryCount = 23;
		
		err = session.TestGetCondScheduleData(id2,pSchedInfo, pInfo, pTime);
		Test(err == KErrArgument);
		
		schedInfo.iEntryCount = 1;
		
		err = session.TestGetCondScheduleData(250000000,pSchedInfo, pInfo, pTime);
		Test(err == KErrBadParameter);
		
		err = session.TestGetCondScheduleData(-3,pSchedInfo, pInfo, pTime);
		Test(err == KErrBadParameter);			
		
		err = session.TestCountSchedules(pCount, EAllSchedules);
		Test(err == KErrNone);
		
		HBufC8* data = HBufC8::NewL(sizeof(TSchedulerItemRef)* count);
		TPtr8 pData = data->Des();
		
		err = session.TestGetScheduleRefs(count,EAllSchedules,pData);
		Test(err == KErrNone);
		
		err = session.TestCountSchedules(pCount, EPendingSchedules);
		Test(err == KErrNone);
		
		err = session.TestGetScheduleRefs(count,EPendingSchedules,pData);
		Test(err == KErrArgument);
		
		err = session.TestGetScheduleRefs(count,3,pData);
		Test(err == KErrBadParameter);
		
		err = session.TestGetScheduleRefs(count,-1,pData);
		Test(err == KErrBadParameter);
		
		err = session.TestGetScheduleRefs(-5,EAllSchedules,pData);
		Test(err == KErrBadParameter);
		
		delete data;
		
		TInt type;
		TPckg<TInt> pType(type);
		
		err = session.TestGetScheduleType(id1,pType);
		Test(err == KErrNone);
		
		err = session.TestGetScheduleType(id2,pType);
		Test(err == KErrNone);
		
		err = session.TestGetScheduleType(-1,pType);
		Test(err == KErrBadParameter);
		
		err = session.TestGetScheduleType(250000002,pType);
		Test(err == KErrBadParameter);
		
		CleanupStack::PopAndDestroy(&session);
	}

void TestGetScheduleDataPanic1L()
	{
	RClientMessageTestSession session;
		
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TScheduleSettings2 settings;
	TTaskSchedulerCondition conditions;
	TTsTime time;
	TInt id1;
	TScheduleInfo schedInfo;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TTsTime> pTime(time);
	TPckg<TInt> pId1(id1);
	TPckg<TScheduleInfo> pSchedInfo(schedInfo);
	
	settings.iEntryCount = 1;	
	settings.iPersists = 0;
	
	TScheduleEntryInfo2 info;
	TPckg<TScheduleEntryInfo2> pInfo(info);
	
	schedInfo.iEntryCount = 1;
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId1);
	Test(err == KErrNone);
	
	err = session.TestGetTimeScheduleData(id1,pSchedInfo, pId1);
	Test(err == KErrOverflow);

	CleanupStack::PopAndDestroy(&session);
	}

void TestGetScheduleDataPanic2L()
	{
	RClientMessageTestSession session;
		
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TScheduleSettings2 settings;
	TInt id1;
	TInt count;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TInt> pId1(id1);
	TPckg<TInt> pCount(count);
	
	settings.iEntryCount = 1;	
	settings.iPersists = 0;
	
	TScheduleEntryInfo2 info;
	TPckg<TScheduleEntryInfo2> pInfo(info);
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId1);
	Test(err == KErrNone);
	
	err = session.TestCountSchedules(pCount, EAllSchedules);
	Test(err == KErrNone);
		
	err = session.TestGetScheduleRefs(count,EAllSchedules,pInfo);
	Test(err == KErrOverflow);
	
	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-4008
@SYMTestCaseDesc	    Tests  Schedule Task API robustness
@SYMTestPriority 	    High
@SYMTestActions  	    Calls the Schedule Tasks APIs passing in varying parameters
						to test the robustness of the API				
@SYMTestExpectedResults The server should be robust to badly formed messages and should
						return KErrBadParameter if messages do not fit the constraints 
						defined in the message schema.  
						The server should panic the client with KErrBadDescriptor if an 
						invalid descriptor argument is passed.
@SYMDEF                	PDEF118811
*/
void TestScheduleTasksL()
	{
	
	RClientMessageTestSession session;
		
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TScheduleSettings2 settings;
	TScheduleEntryInfo2 info;
	TInt id1;
	TInt taskId;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TScheduleEntryInfo2> pInfo(info);
	TPckg<TInt> pId1(id1);
	TPckg<TInt> pTaskId(taskId);

	
	settings.iEntryCount = 1;	
	settings.iPersists = 0;
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId1);
	Test(err == KErrNone);
	
	TTaskInfo taskInfo;
	TPckg<TTaskInfo> pTaskInfo(taskInfo);
	TBuf<32> taskData;
	
	err = session.TestScheduleTask(pTaskInfo, -1, pTaskId,taskData);
	Test(err == KErrBadParameter);
	
	err = session.TestScheduleTask(pTaskInfo, 300000000, pTaskId,taskData);
	Test(err == KErrBadParameter);
	
	err = session.TestDeleteTask(0);
	Test(err == KErrNotFound);
	
	err = session.TestDeleteTask(249999999);
	Test(err == KErrNotFound);
	
	err = session.TestDeleteTask(250000000);
	Test(err == KErrNotFound);
	
	err = session.TestDeleteTask(-1);
	Test(err == KErrNotFound);
		
	CleanupStack::PopAndDestroy(&session);
	}

void TestScheduleTasksPanic1L()
	{
	RClientMessageTestSession session;
		
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TScheduleSettings2 settings;
	TScheduleEntryInfo2 info;
	TInt id1;
	TInt taskId;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TScheduleEntryInfo2> pInfo(info);
	TPckg<TInt> pId1(id1);
	TPckg<TInt> pTaskId(taskId);

	
	settings.iEntryCount = 1;	
	settings.iPersists = 0;
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId1);
	Test(err == KErrNone);
	
	TTaskInfo taskInfo;
	TPckg<TTaskInfo> pTaskInfo(taskInfo);
	TBuf<32> taskData;
	
	//This should cause the server to panic the client as p0 is 
	//the wrong size
	err = session.TestScheduleTask(pInfo, id1, pTaskId,taskData);
		
	CleanupStack::PopAndDestroy(&session);
	}

void TestScheduleTasksPanic2L()
	{
	RClientMessageTestSession session;
		
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TScheduleSettings2 settings;
	TScheduleEntryInfo2 info;
	TInt id1;
	TInt taskId;
	
	TPckg<TScheduleSettings2> pSettings(settings);
	TPckg<TScheduleEntryInfo2> pInfo(info);
	TPckg<TInt> pId1(id1);
	TPckg<TInt> pTaskId(taskId);

	
	settings.iEntryCount = 1;	
	settings.iPersists = 0;
	
	err = session.TestCreateTimeSchedule(pSettings, pInfo, pId1);
	Test(err == KErrNone);
	
	TTaskInfo taskInfo;
	TPckg<TTaskInfo> pTaskInfo(taskInfo);
	TBuf<32> taskData;
	
	//This should cause the server to panic the client as p2 is too large
	err = session.TestScheduleTask(pTaskInfo, id1, pTaskInfo,taskData);
		
	CleanupStack::PopAndDestroy(&session);
	}


/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-4009
@SYMTestCaseDesc	    Tests  Get Task Data API robustness
@SYMTestPriority 	    High
@SYMTestActions  	    Calls the Get Task Data APIs passing in varying parameters
						to test the robustness of the API				
@SYMTestExpectedResults The server should be robust to badly formed messages and should
						return KErrBadParameter if messages do not fit the constraints 
						defined in the message schema.  
@SYMDEF                	PDEF118811
*/
void TestGetTaskDataL()
	{
	
	RClientMessageTestSession session;
		
	TInt err = session.Connect();
	
	CleanupClosePushL(session);
	
	err = session.TestRegisterClient(_L("MyClient"),0);	
	Test(err == KErrNone);
	
	TSchedulerItemRef itemRef;
	TScheduleInfo schedInfo;
	TTaskInfo taskInfo;
	TInt id1;
	TInt count;
	TInt taskId;
	
	TPckg<TScheduleInfo> pSchedInfo(schedInfo);
	TPckg<TSchedulerItemRef> pItemRef(itemRef);
	TPckg<TTaskInfo> pTaskInfo(taskInfo);
	TPckg<TInt> pId1(id1);
	TPckg<TInt> pCount(count);
	TPckg<TInt> pTaskId(taskId);
	
	
	err = session.TestGetTaskData(-1,pSchedInfo,pTaskInfo);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskData(340000000,pSchedInfo,pTaskInfo);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskRefs(0,0,0,pItemRef);
	Test(err == KErrArgument);
	
	err = session.TestGetTaskRefs(-1,EAllSchedules,EAllTasks,pItemRef);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskRefs(340000000, EAllSchedules,EMyTasks,pItemRef);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskRefs(1,-1,EAllTasks,pItemRef);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskRefs(1,3,EAllTasks,pItemRef);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskRefs(1,EPendingSchedules,-1,pItemRef);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskRefs(1,EPendingSchedules,2,pItemRef);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskRefs(1,EAllSchedules,EMyTasks,pItemRef);
	Test(err == KErrArgument);
	
	err = session.TestCountTasks(pCount,EAllSchedules,EMyTasks);
	Test(err == KErrNone);
	
	err = session.TestCountTasks(pCount,-1,EMyTasks);
	Test(err == KErrBadParameter);
	
	err = session.TestCountTasks(pCount,2,EMyTasks);
	Test(err == KErrBadParameter);
	
	err = session.TestCountTasks(pCount,EAllSchedules,-1);
	Test(err == KErrBadParameter);
	
	err = session.TestCountTasks(pCount,EAllSchedules,2);
	Test(err == KErrBadParameter);
	
	err = session.TestGetTaskDataSize(0,pCount);
	Test(err == KErrNotFound);
	
	err = session.TestGetTaskDataSize(249999999,pCount);
	Test(err == KErrNotFound);
	
	err = session.TestGetTaskDataSize(250000000,pCount);
	Test(err == KErrNotFound);
	
	err = session.TestGetTaskDataSize(-1,pCount);
	Test(err == KErrNotFound);
	
	TBuf<10> data;
	
	err = session.TestGetTaskInfo(0,pTaskInfo,10,data);
	Test(err == KErrNotFound);
	
	err = session.TestGetTaskInfo(249999999,pTaskInfo,10,data);
	Test(err == KErrNotFound);
	
	err = session.TestGetTaskInfo(250000000,pTaskInfo,10,data);
	Test(err == KErrNotFound);
	
	err = session.TestGetTaskInfo(-1,pTaskInfo,10,data);
	Test(err == KErrNotFound);
	
	TSchedulerItemRef schedItem;
	TTsTime time;
	
	TPckg<TSchedulerItemRef> pSchedItem(schedItem);
	TPckg<TTsTime> pTime(time);

	err = session.TestGetScheduleItemRef(0,pSchedItem,pTime);
	Test(err == KErrNone);
	
	err = session.TestGetScheduleItemRef(249999999,pSchedItem,pTime);
	Test(err == KErrNotFound);
	
	err = session.TestGetScheduleItemRef(250000000,pSchedItem,pTime);
	Test(err == KErrBadParameter);
	
	err = session.TestGetScheduleItemRef(-1,pSchedItem,pTime);
	Test(err == KErrBadParameter);	
			
	CleanupStack::PopAndDestroy(&session);
	}


static void DoAPITestsL()
	{
	
	TExitDetails exitDetails;
	
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-4005 Test Create Schedule APIs "));
	exitDetails = LaunchTestThreadL(_L("TestCreateSchedulesL"), &TestCreateSchedulesL);
	Test(exitDetails.iExitType == EExitKill);
	
	exitDetails = LaunchTestThreadL(_L("TestCreateSchedulesPanic1L"), &TestCreateSchedulesPanic1L);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);
	
	exitDetails = LaunchTestThreadL(_L("TestCreateSchedulesPanic2L"), &TestCreateSchedulesPanic2L);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);
	
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-4006 Test Edit Schedule APIs "));
	exitDetails = LaunchTestThreadL(_L("TestEditSchedulesL"), &TestEditSchedulesL);
	Test(exitDetails.iExitType == EExitKill);

	Test.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-4007 "));
	exitDetails = LaunchTestThreadL(_L("TestGetScheduleDataL"), &TestGetScheduleDataL);
	Test(exitDetails.iExitType == EExitKill);
	
	exitDetails = LaunchTestThreadL(_L("TestGetScheduleDataPanic1L"), &TestGetScheduleDataPanic1L);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrOverflow);
	
	Test.Next(_L("  Test Get Schedule Data APIs "));
	exitDetails = LaunchTestThreadL(_L("TestGetScheduleDataPanic2L"), &TestGetScheduleDataPanic2L);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrOverflow);
	
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-4008 Test Schedule Task APIs "));
	exitDetails = LaunchTestThreadL(_L("TestScheduleTasksL"), &TestScheduleTasksL);
	Test(exitDetails.iExitType == EExitKill);
	
	exitDetails = LaunchTestThreadL(_L("TestScheduleTasksPanic1L"), &TestScheduleTasksPanic1L);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);
	
	exitDetails = LaunchTestThreadL(_L("TestScheduleTasksPanic2L"), &TestScheduleTasksPanic2L);
	Test(exitDetails.iExitType == EExitPanic);
	Test(exitDetails.iReason == KErrBadDescriptor);
	
	Test.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-4009 Test Get Task Data APIs "));
	exitDetails = LaunchTestThreadL(_L("TestGetTaskDataL"), &TestGetTaskDataL);
	Test(exitDetails.iExitType == EExitKill);
	
	}






GLDEF_C TInt E32Main ()
	{

	Test.Printf (_L ("\n"));
	Test.Title ();
	Test.Start (_L("IPC Fuzz Tests"));

   	CTrapCleanup* cleanup=CTrapCleanup::New();
	
   	TInt err=KErrNoMemory;
   	if (cleanup)
   		{
   		
   		// Construct and install the active scheduler
   		CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
   		CActiveScheduler::Install (scheduler);
   		
   		KillProcess(KServerProcess);
   		User::After(1000000);
   				
   		TRAP (err, DoFuzzTestsL ());
   		Test (err == KErrNone);
   		
   		Test.Next(_L("Task Scheduler API Robustness Tests"));
   		TRAP (err, DoAPITestsL ());
   		Test (err == KErrNone);
  
   		Test.End ();
   		Test.Close ();
   		
   		delete scheduler;
   		delete cleanup;
   		
   		}
	return err;
	}
