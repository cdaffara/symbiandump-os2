// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LogCliServShared.h"
#include "logservpanic.h"
#include "logpackage.h"
#include "t_logutil.h"

RTest	TheTest(_L("t_logservipc"));

_LIT(KServerName, "!LogServ");
_LIT(KServerProcess, "LogServ");

///////////////////////////////////////////////////////////////////////////////////////
//

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

const TInt KFunctionNumbers [] = {ELogOperationCancel,
								ELogOperationGetResult,
								ELogOperationInitiate,
								ELogNotify,
								ELogNotifyCancel,
								ELogViewCreate,
								ELogViewDelete,
								ELogViewCount,
								ELogViewOperationInitiate,
								ELogViewChangeNotificationsRequest,
								ELogViewChangeNotificationsCancel,
								ELogViewFetchChanges,
								ELogViewNotifyLockStatusChange,
								ELogViewNotifyLockStatusChangeCancel,
								ELogNotifyExtended,
								ELogNotifyExtendedCancel};

const TInt KNumFunctions = sizeof(KFunctionNumbers)/sizeof(KFunctionNumbers[0]);

//===============================================================================

TBool IsFunctionAsynchronous(TInt aFunc)
	{
	TBool asynch = EFalse;
	switch(aFunc)
		{			
		case ELogOperationInitiate:
		case ELogNotify:
		case ELogViewOperationInitiate:
		case ELogViewChangeNotificationsRequest:
		case ELogViewNotifyLockStatusChange:
		case ELogNotifyExtended:
			asynch = ETrue;
			break;
			
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
		
		LEAVE_IF_ERROR(err);
	
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
	TInt err = thread.Create(KThreadName,&FuzzServerL, KDefaultStackSize, NULL,&testInfo);
	TEST2(err, KErrNone);
	
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
	
	TheTest.Printf(_L("Launching LogServer...\n"));
	
	const TUid KServerUid3 = {0x0101f401d};	
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
			TheTest.Printf(_L("\nFuzz Test on function number %d using random Int data. Number of Args = %d"), iFunc, iArgCount);
			break;
			
		case 1:
			TheTest.Printf(_L("\nFuzz Test on function number %d using random Des8 data. Number of Args = %d"), iFunc, iArgCount);
			break;
			
		case 2:
			TheTest.Printf(_L("\nFuzz Test on function number %d using random Des data. Number of Args = %d"), iFunc, iArgCount);
			break;
			
		}
	
	}

/**
Invoke the tests
*/
/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-4002
@SYMTestCaseDesc	    Tests LogEng APIs for IPC Robustness
@SYMTestPriority 	    High
@SYMTestActions  	    The function calls each of the Logeng APIs through a custom session object
						passing random TInt, Des8 and Des16 data .
@SYMTestExpectedResults The server should be robust to all malformed messages and should not
						hang or panic.
@SYMDEF                	INC114113
*/
LOCAL_C void DoFuzzTestsL ()
    {  
    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4002 "));
    RProcess server;
    
    LEAVE_IF_ERROR(LaunchServer(server));

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
	    			TEST2(exitReason, 0);    	
	    			LEAVE_IF_ERROR(LaunchServer(server));
	    			}
    			}
	    	
    		TheTest.Printf(_L("\nFuzz Test Successful\n"));
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
	
	TInt TestMakeTransient(TInt aArg0);
	TInt TestMakeTransient(const TDesC8& aArg0);
	
	TInt TestCliServDataParam(TInt aFunc, TDes8& aArg0);
	TInt TestCliServDataParam(TInt aFunc, TDes8& aArg0, TUint32 aArg1);
	TInt TestCliServDataParam(TInt aFunc, TDes8& aArg0, TDes8& aArg1, TInt aArg2);
	TInt TestCliServDataParam(TInt aFunc, TDes& aArg0, TDes8& aArg1, TInt aArg2);
	
	TInt TestLogNotify(TInt aArg0);
	TInt TestLogNotifyCancel();
	
	TInt TestLogViewCreate(TInt aArg0, TInt aArg1);	
	
	TInt TestLogViewIdParam(TInt aFunc, TInt aArg0);
	TInt TestLogViewIdParam(TInt aFunc, TDes8& aArg1);
	
	TInt TestLogViewFetchChanges(TInt aArg0, TInt aArg1, TDes8& aArg2);
	TInt TestLogViewFetchChanges(TInt aArg0, TInt aArg1, const TDesC8& aArg2);
	
	TInt TestLogNotifyExtended(TDes8& aArg0, TDes8& aArg1, TDes8& aArg2,TDes8& aArg3);
	TInt TestLogNotifyExtended(TDes& aArg0, TDes& aArg1, TDes& aArg2,TDes& aArg3);
	TInt TestLogNotifyExtended(TDes8& aArg0, TInt aArg1, TInt aArg2,const TDesC8& aArg3);
	
	TInt TestFunction45(TAny* aData);
	

	};

static TInt LaunchLogServer()
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
		
		r = LaunchLogServer();
		if((r != KErrNone)&&(r != KErrAlreadyExists))
			{
			return r;
			}	
		}
	}

TInt RClientMessageTestSession::TestMakeTransient(TInt aArg0)
	{
	return SendReceive(ELogMakeTransient,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestMakeTransient(const TDesC8& aArg0)
	{
	return SendReceive(ELogMakeTransient,TIpcArgs(&aArg0));
	}

TInt RClientMessageTestSession::TestCliServDataParam(TInt aFunc, TDes8& aArg0)
	{
	return SendReceive(aFunc,TIpcArgs(&aArg0));
	}

TInt RClientMessageTestSession::TestCliServDataParam(TInt aFunc, TDes8& aArg0, TUint32 aArg1)
	{
	return SendReceive(aFunc,TIpcArgs(&aArg0, aArg1));
	}

TInt RClientMessageTestSession::TestCliServDataParam(TInt aFunc, TDes8& aArg0, TDes8& aArg1, TInt aArg2)
	{
	return SendReceive(aFunc,TIpcArgs(&aArg0, &aArg1, aArg2));
	}

TInt RClientMessageTestSession::TestLogNotify(TInt aArg0)
	{
	return Send(ELogNotify,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestLogNotifyCancel()
	{
	return Send(ELogNotifyCancel);
	}
	
TInt RClientMessageTestSession::TestLogViewCreate(TInt aArg0, TInt aArg1)
	{
	return SendReceive(ELogViewCreate,TIpcArgs(aArg0,aArg1));
	}


TInt RClientMessageTestSession::TestLogViewIdParam(TInt aFunc, TInt aArg0)
	{
	return SendReceive(aFunc,TIpcArgs(aArg0));
	}

TInt RClientMessageTestSession::TestLogViewIdParam(TInt aFunc, TDes8& aArg0)
	{
	return SendReceive(aFunc,TIpcArgs(&aArg0));
	}

TInt RClientMessageTestSession::TestLogViewFetchChanges(TInt aArg0, TInt aArg1, TDes8& aArg2)
	{
	return SendReceive(ELogViewFetchChanges,TIpcArgs(aArg0,aArg1, &aArg2));
	}

TInt RClientMessageTestSession::TestLogViewFetchChanges(TInt aArg0, TInt aArg1, const TDesC8& aArg2)
	{
	return SendReceive(ELogViewFetchChanges,TIpcArgs(aArg0,aArg1, &aArg2));
	}

TInt RClientMessageTestSession::TestLogNotifyExtended(TDes8& aArg0, 
								TDes8& aArg1, TDes8& aArg2,TDes8& aArg3)
	{
	return Send(ELogNotifyExtended,TIpcArgs(&aArg0,&aArg1,&aArg2,&aArg3));
	}

TInt RClientMessageTestSession::TestLogNotifyExtended(TDes& aArg0, 
								TDes& aArg1, TDes& aArg2,TDes& aArg3)
	{
	return Send(ELogNotifyExtended,TIpcArgs(&aArg0,&aArg1,&aArg2,&aArg3));
	}

TInt RClientMessageTestSession::TestLogNotifyExtended(TDes8& aArg0, 
								TInt aArg1, TInt aArg2,const TDesC8& aArg3)
	{
	return Send(ELogNotifyExtended,TIpcArgs(&aArg0,&aArg1,&aArg2,&aArg3));
	}

TInt RClientMessageTestSession::TestFunction45(TAny* aData)
	{
	return SendReceive(45,TIpcArgs(aData));
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
	TInt err = thread.Create(aThreadName, &TestFunctionLauncherL, KDefaultStackSize, NULL, (TAny*)aFunction);
	TEST2(err, KErrNone);
	
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
@SYMTestCaseID          SYSLIB-LOGENG-CT-4003
@SYMTestCaseDesc	    Tests Message schema validation for the ELogMakeTransient message.
@SYMTestPriority 	    High
@SYMTestActions  	    Sends messages to the test server to test the validation of messages
                        against the message schema.
@SYMTestExpectedResults The server should validate the message and handle bad messages appropriately
@SYMDEF                	INC114113
*/
void TestMakeTransientL()
	{
	
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
	TEST2(err, KErrNone);
	
	CleanupClosePushL(session);
	
	err = session.TestMakeTransient(0);	
	TEST2(err, KErrNone);
	
	//anything different from 0 should be considered as ETrue
	err = session.TestMakeTransient(1);	
	TEST2(err, KErrNone);
	
	err = session.TestMakeTransient(-5);	
	TEST2(err, KErrNone);
	
	err = session.TestMakeTransient(3);	
	TEST2(err, KErrNone);
	
	err = session.TestMakeTransient( _L8("Des8"));	
	TEST2(err, KErrNone);
	
	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-4004
@SYMTestCaseDesc	    Tests Message schema validation for the messages accepting
 						TLogClientServerData parameters.
@SYMTestPriority 	    High
@SYMTestActions  	    Sends messages to the test server to test the validation of messages
						against the message schema.  The messages contain either valid or invalid 
						parameters. 
@SYMTestExpectedResults The server should validate the message and handle bad messages appropriately
@SYMDEF                	INC114113
*/
void TestCliServDataParamL()
	{
	
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
    TEST2(err, KErrNone);
	
	CleanupClosePushL(session);
	
	TLogClientServerData data;
	TPckg<TLogClientServerData> pData(data);
	
	TBuf8<sizeof(TLogClientServerData)> buf;
	
	buf.SetLength(sizeof(TLogClientServerData));
	
	buf.Fill(0xFF);
	
	data.iOperationType = 	ELogOperationEventAdd; 
	data.iOperationId = 1;
	err = session.TestCliServDataParam(ELogOperationInitiate,pData);	
	TEST2(err, KErrBadDescriptor);
	
	data.iOperationType = 	ELogOperationViewWindowFetch; 
	data.iOperationId = 5;
	err = session.TestCliServDataParam(ELogOperationInitiate,pData);	
	TEST2(err, KErrArgument);
	
	data.iOperationType = 	ELogOperationEventAdd; 
	data.iOperationId = 0x8FFFFFFF;
	err = session.TestCliServDataParam(ELogOperationInitiate,pData);	
	TEST2(err, KErrBadDescriptor);

	data.iOperationType = 	ELogOperationEventAdd; 
	data.iOperationId = 0x8FFFFFFF;
	err = session.TestCliServDataParam(ELogOperationInitiate,buf);	
	TEST2(err, KErrArgument);
	
	data.iOperationType = 	ELogOperationEventAdd; 
	data.iOperationId = 1;
	err = session.TestCliServDataParam(ELogOperationCancel,pData);	
	TEST2(err, KErrCancel);
	
	data.iOperationType = 	ELogOperationEventAdd; 
	data.iOperationId = 1;
	err = session.TestCliServDataParam(ELogOperationCancel,buf);	
	TEST2(err, KErrArgument);
	
	CLogPackage* package = CLogPackage::NewL();
	TPtr8 ptr(package->Ptr());
	
	data.iOperationType = 	ELogOperationEventAdd; 
	data.iOperationId = 1;
	err = session.TestCliServDataParam(ELogOperationGetResult,buf, ptr, 0);	
	TEST2(err, KErrArgument);
	
	data.iOperationType = 	ELogOperationEventAdd; 
	data.iOperationId = 1;
	err = session.TestCliServDataParam(ELogOperationGetResult,pData, ptr, -1 );	
	TEST2(err, KErrNone);
	
	delete package;
	
	TBuf8<8> smallBuf;
	data.iOperationType = 	ELogOperationEventAdd; 
	data.iOperationId = 1;
	err = session.TestCliServDataParam(ELogOperationGetResult,buf, smallBuf, 0);	
	TEST2(err, KErrArgument);
	
	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-LEGACY-T_LOGSERVIPC-0001
@SYMTestCaseDesc	    Tests Message schema validation for the messages accepting
 						TLogClientServerData parameters.
@SYMTestPriority 	    High
@SYMTestActions  	    Sends messages to the test server to test the validation of messages
						against the message schema.  The messages contain either valid or invalid 
						parameters. 
@SYMTestExpectedResults The server should validate the message and handle bad messages appropriately
@SYMDEF                	INC114113
*/
void TestCliServDataParam2L()
	{
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
    TEST2(err, KErrNone);
	
	CleanupClosePushL(session);
	
	TBuf8<sizeof(TLogClientServerData) - 1> buf;
	
	buf.FillZ();
	
	//This should panic with Logserv 63
	err = session.TestCliServDataParam(ELogOperationInitiate,buf);	
    TEST2(err, KErrNone);
	
	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-LEGACY-T_LOGSERVIPC-0002
@SYMTestCaseDesc	    Tests Message schema validation for the messages accepting
 						TLogClientServerData parameters.
@SYMTestPriority 	    High
@SYMTestActions  	    Sends messages to the test server to test the validation of messages
						against the message schema.  The messages contain either valid or invalid 
						parameters. 
@SYMTestExpectedResults The server should validate the message and handle bad messages appropriately
@SYMDEF                	INC114113
*/
void TestCliServDataParam3L()
	{
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
    TEST2(err, KErrNone);
	
	CleanupClosePushL(session);
	
	TBuf8<sizeof(TLogClientServerData) + 1> buf;
	
	buf.FillZ();
	
	err = session.TestCliServDataParam(ELogOperationInitiate,buf);	
	TEST2(err, KErrArgument);
	
	CleanupStack::PopAndDestroy(&session);
	}


/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-4005
@SYMTestCaseDesc	    Tests Message schema validation for the ELogNotify message.
@SYMTestPriority 	    High
@SYMTestActions  	    Sends messages to the test server to test the validation of messages
						against the message schema.  The messages contain either valid or invalid 
						parameters. 
@SYMTestExpectedResults The server should validate the message and handle bad messages appropriately
@SYMDEF                	INC114113
*/
void TestLogNotifyL()
	{
	
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
    TEST2(err, KErrNone);

    CleanupClosePushL(session);
	
	err = session.TestLogNotify(0);
	TEST2(err, KErrNone);	
	
	//Cancel the pending notification
	err = session.TestLogNotifyCancel();
	TEST2(err, KErrNone);
	
	err = session.TestLogNotify(1000000);
	TEST2(err, KErrNone);
	
	//Cancel the pending notification
	err = session.TestLogNotifyCancel();
	TEST2(err, KErrNone);
	
	err = session.TestLogNotify(-1);
	TEST2(err, KErrNone);
	
	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-4006
@SYMTestCaseDesc	    Tests Message schema validation for the ELogViewCreate message.
@SYMTestPriority 	    High
@SYMTestActions  	    Sends messages to the test server to test the validation of messages
						against the message schema.  The messages contain either valid or invalid 
						parameters. 
@SYMTestExpectedResults The server should validate the message and handle bad messages appropriately
@SYMDEF                	INC114113
*/
void TestLogViewCreateL()
	{
	
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
    TEST2(err, KErrNone);
	
	CleanupClosePushL(session);
	
	err = session.TestLogViewCreate(0,ELogViewTypeEvent);	
	TEST2(err, KErrNone);
	
	err = session.TestLogViewCreate(100000,ELogViewTypeDuplicate);	
	TEST2(err, KErrNone);
	
	err = session.TestLogViewCreate(0,ELogViewTypeDuplicate + 1);	
	TEST2(err, KErrArgument);
	
	err = session.TestLogViewCreate(-1,ELogViewTypeRecent);	
	TEST2(err, KErrNone);
	
	err = session.TestLogViewCreate(20,-1);	
	TEST2(err, KErrArgument);

	CleanupStack::PopAndDestroy(&session);
	}


/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-4012
@SYMTestCaseDesc	    Tests Message schema validation for the ELogNotifyExtended message.
@SYMTestPriority 	    High
@SYMTestActions  	    Sends a message to the test server to test the validation of messages
						against the message schema.  The message contains an invalid descriptor. 
@SYMTestExpectedResults The server should panic the client with KErrBadDescriptor
@SYMDEF                	INC114113
*/
void TestLogNotifyExtended2L()
	{
	
	RClientMessageTestSession session;

	TInt err = session.Connect();
    TEST2(err, KErrNone);
	
	CleanupClosePushL(session);

	_LIT8(KDes8,"Des8");
	TPckgBuf<TInt> int0(0);
	
	err = session.TestLogNotifyExtended(int0, 0, -1, KDes8);	
	TEST2(err, KErrNone);

	CleanupStack::PopAndDestroy(&session);
	}

/**
@SYMTestCaseID          SYSLIB-LOGENG-CT-4013
@SYMTestCaseDesc	    Tests Message schema validation for an invalid message
@SYMTestPriority 	    High
@SYMTestActions  	    Sends a message to the test server with a message number not 
						defined in the schema.
@SYMTestExpectedResults The server should return KErrInvalidFunction
@SYMDEF                	INC114113
*/
void InvalidMessageTestL()
	{
	
	RClientMessageTestSession session;
	
	TInt err = session.Connect();
    TEST2(err, KErrNone);
	
	CleanupClosePushL(session);
	
	//This should cause the server to panic the client
	err = session.TestFunction45(0);	
	
	CleanupStack::PopAndDestroy(&session);
	}



static void DoAPITestsL()
	{
	
	TExitDetails exitDetails;
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4004 "));
	exitDetails = LaunchTestThreadL(_L("TestCliServDataParamL"), &TestCliServDataParamL);
	TEST2(exitDetails.iExitType, EExitKill);
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-LEGACY-T_LOGSERVIPC-0001 "));
	exitDetails = LaunchTestThreadL(_L("TestCliServDataParam2L"), &TestCliServDataParam2L);
	TEST2(exitDetails.iExitType, EExitPanic);
	TEST2(exitDetails.iReason, ELogBadDescriptor);
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-LEGACY-T_LOGSERVIPC-0002 "));
	exitDetails = LaunchTestThreadL(_L("TestCliServDataParam3L"), &TestCliServDataParam3L);
	TEST2(exitDetails.iExitType, EExitPanic);
	TEST2(exitDetails.iReason, ELogBadDescriptor);
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4005 "));
	exitDetails = LaunchTestThreadL(_L("TestLogNotifyL"), &TestLogNotifyL);
	TEST2(exitDetails.iExitType, EExitKill);
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4006 "));
	exitDetails = LaunchTestThreadL(_L("TestLogViewCreateL"), &TestLogViewCreateL);
	TEST2(exitDetails.iExitType, EExitKill);
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4012 "));
	exitDetails = LaunchTestThreadL(_L("TestLogNotifyExtended2L"), &TestLogNotifyExtended2L);
	TEST2(exitDetails.iExitType, EExitKill);
	
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4013 "));
	exitDetails = LaunchTestThreadL(_L("InvalidMessageTestL"), &InvalidMessageTestL);
	TEST2(exitDetails.iExitType, EExitPanic);
	TEST2(exitDetails.iReason, ELogIllegalFunction);

 	//test for debug-only API
#ifdef _DEBUG
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-LOGENG-CT-4003 "));
 	exitDetails = LaunchTestThreadL(_L("TestMakeTransientL"), &TestMakeTransientL);
 	TEST2(exitDetails.iExitType, EExitKill);
#endif//_DEBUG	
	}




GLDEF_C TInt E32Main ()
	{

	TheTest.Printf (_L ("\n"));
	TheTest.Title ();
	TheTest.Start (_L("IPC Fuzz Tests"));

   	CTrapCleanup* cleanup=CTrapCleanup::New();
   	TEST(cleanup != NULL);
	
   	TInt err=KErrNoMemory;
   	if (cleanup)
   		{
   		
   		// Construct and install the active scheduler
   		CActiveScheduler* scheduler = new CActiveScheduler;
   	    TEST(scheduler != NULL);
   		CActiveScheduler::Install (scheduler);
   		
   		KillProcess(KServerProcess);
   		User::After(1000000);
   				
   		TRAP (err, DoFuzzTestsL ());
   		TEST2(err, KErrNone);
   		
   		TheTest.Next(_L("LogServ API Robustness Tests"));
   		TRAP (err, DoAPITestsL ());
   		TEST2(err, KErrNone);
   		
   		TheTest.End ();
   		TheTest.Close ();
   		
   		delete scheduler;
   		delete cleanup;
   		
   		}
	return err;
	}
