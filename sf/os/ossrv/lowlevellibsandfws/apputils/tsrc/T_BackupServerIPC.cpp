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

#include "backup_std.h"

LOCAL_D RTest				TheTest (_L("T_BackupServerIPC"));
LOCAL_D CActiveScheduler*	TheScheduler;
_LIT(KServerLauncherProcess, "T_BackupServerLauncher");
_LIT(KServerStartProcess, "baksrvs");

_LIT(KServerName, "!BackupServer");

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

//===============================================================================

TBool IsFunctionAsynchronous(TInt aFunc)
	{
	TBool asynch = EFalse;
	switch(aFunc)
		{
		case EBakOpCodeEventReady:
		case EBakOpCodeCloseAllFiles:
		case EBakOpCodeCloseServer:
		case EBakOpCodeBackupOperationEventReady:
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

		User::LeaveIfError(CreateSession(aTargetSrvName, version, 200));

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

	TheTest.Printf(_L("Launching BackupServer...\n"));

	TInt err = aServer.Create(KServerLauncherProcess, _L(""));

	 if(err == KErrNone)
		 {
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
			TheTest.Printf(_L("\nFuzz Test on function number %d using random Int data. Number of Args = %d\n"), iFunc, iArgCount);
			break;

		case 1:
			TheTest.Printf(_L("\nFuzz Test on function number %d using random Des8 data. Number of Args = %d\n"), iFunc, iArgCount);
			break;

		case 2:
			TheTest.Printf(_L("\nFuzz Test on function number %d using random Des data. Number of Args = %d\n"), iFunc, iArgCount);
			break;

		}

	}

/**
Invoke the tests
*/
/**
@SYMTestCaseID          SYSLIB-BAFL-CT-4039
@SYMTestCaseDesc	    Tests Bafl Backup Server APIs for IPC Robustness
@SYMTestPriority 	    High
@SYMTestActions  	    The function calls each of the Bafl Backup Server APIs through a
						custom session object passing random TInt, Des8 and Des16 data .
@SYMTestExpectedResults The server should be robust to all malformed messages and should not
						hang or panic.
@SYMDEF                	INC113760
*/
LOCAL_C void DoTestsL ()
    {

    RProcess server;
    const TInt KMinFuncNumber = 20;
    const TInt KMaxFuncNumber = 35;

    TInt err = LaunchServer(server);
    TheTest(err == 0);

    TExitDetails exitDetails;

    for(TInt i = KMinFuncNumber;i<= KMaxFuncNumber;i++)
    	{

    	//Carry out each type of test
    	for(TInt testType = 0; testType < 3;testType++)
    		{
       		//Carry out each test with number of arguments 1 - 4
    		for(TInt argCount = 1;argCount <= 4;argCount++)
    			{
    			PrintTestMessage(i, testType, argCount);

    			TestServerApi(i, testType, argCount, exitDetails);
		    	//Kill the server process and verify that it was still running
		    	//If the server was already dead it would return the reason it exited
	    		if(server.ExitType() != EExitPending)
	    			{
	    			server.Kill(0);
	    			TInt exitReason = server.ExitReason();
	    			server.Close();
	    			TheTest(exitReason == 0);
	    			User::LeaveIfError(LaunchServer(server));
	    			}
    			}

	    	TheTest.Printf(_L("\nFuzz Test Successful\n"));
    		}
    	}
    }

void SetupTestL()
	{
	TheScheduler = new (ELeave) CActiveScheduler;
   	CActiveScheduler::Install (TheScheduler);

	}


GLDEF_C TInt E32Main ()
	{

	TheTest.Printf (_L ("\n"));
	TheTest.Title ();
	TheTest.Start (_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-4039 IPC Fuzz Tests "));

   	CTrapCleanup* cleanup=CTrapCleanup::New();

   	TInt err=KErrNoMemory;
   	if (cleanup)
   		{
   		// Construct and install the active scheduler
   		TRAP(err, SetupTestL());

   		//Kill baksrvs which may have already been launched by another test
   		KillProcess(KServerStartProcess);
   		KillProcess(KServerLauncherProcess);

   		TRAP (err, DoTestsL ());
   		TheTest (err == KErrNone);

   		TheTest.End ();
   		TheTest.Close ();

   		delete TheScheduler;
   		delete cleanup;

   		}
	return err;
	}
