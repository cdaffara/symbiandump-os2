// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bautils.h>
#include <e32math.h>
#include "LogServShared.h"
#include "logservcli.h"
#include "t_logutil.h"

///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_logbadclient test"));

const TInt KTestIterCount = 5000;
const TInt KMaxDesArgLen = 1000;
enum TArgType 
	{
	EIntArgType, 
	ETextArgType, 
	EBinArgType, 
	ELastArgType
	};

const TLogServFunction KLogIpcMsgCodes[] = 
	{
	ELogOperationCancel, ELogOperationGetResult, ELogOperationInitiate, ELogNotify, ELogNotifyCancel, ELogViewCreate,
	ELogViewDelete, ELogViewCount, ELogViewOperationInitiate, ELogViewChangeNotificationsRequest,
	ELogViewChangeNotificationsCancel, ELogViewFetchChanges, ELogViewNotifyLockStatusChange, 
	ELogViewNotifyLockStatusChangeCancel, ELogNotifyExtended, ELogNotifyExtendedCancel, ELogNOTUSED   
	};

const TLogOperationType KLogOpTypes[] =
	{
	ELogOperationEventAdd, ELogOperationEventGet, ELogOperationEventChange, ELogOperationEventDelete,
	ELogOperationTypeAdd, ELogOperationTypeGet, ELogOperationTypeChange, ELogOperationTypeDelete,
	ELogOperationClearLog,  ELogOperationClearRecent, ELogOperationConfigGet,  ELogOperationConfigChange,
	ELogOperationMaintain, ELogOperationViewSetup, ELogOperationViewRemoveEvent, ELogOperationViewClearDuplicates,
	ELogOperationViewSetFlags, ELogOperationViewWindowFetch, (TLogOperationType)-100
	};

//////////////////////////////////////////////////////////////////////////////////////

//If the LogEng server crashes and the test receives KErrServerTerminated error, then the 
//next set will contain the last:
// - iteration number;
// - function code;
// - IPC arguments values;
struct TThreadData
	{
	TInt				iIteration;
	TInt 				iFunction;
	TArgType 			iArgType[KMaxMessageArguments];
	TInt 				iIntArg[KMaxMessageArguments];
	TBuf<KMaxDesArgLen> iTextArg[KMaxMessageArguments];
	TBuf8<KMaxDesArgLen> iBinArg[KMaxMessageArguments];
	TInt64 				iSeed;
	};
//////////////////////////////////////////////////////////////////////////////////////

_LIT(KPanicCategory, "SrvTerm");
_LIT(KPanicCategory2, "InvArg");
const TInt KPanicCode = 1111;
const TInt KPanicCode2 = 2222;

static TLogClientServerData TheLogIpcData;
static TPtrC8 TheLogIpcDataPtr((const TUint8*)&TheLogIpcData, sizeof(TheLogIpcData));

///////////////////////////////////////////////////////////////////////////////////////

void PrintIterationCount(TInt aIteration)
	{
	if((aIteration % 100) == 0)
		{
		TTime time;
		time.HomeTime();
		TDateTime dt = time.DateTime();
		TBuf<16> tbuf;
		tbuf.Format(_L("%02d:%02d:%02d.%06d"), dt.Hour(), dt.Minute(), dt.Second(), dt.MicroSecond());
		TheTest.Printf(_L("-----[%S] Test iterations: %d\r\n"), &tbuf, aIteration);
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//Worker thread function.
//It behaves as a malicious client. Connects to the LogEng server. In each test iteration generates some random values
//for the function number, handle, handle type, IPC arguments. Then sends a command to the server using these
//randomly generated values. If the server crashes and the thread function receives KErrServerTerminated error,
//then the thread kills itself and the main thread will get KPanicCategory and KPanicCode as a reason for the
//worker thread's death. The last set of randomly generated values will be stored in the memory, pointed by aData argument.
TInt ThreadFunc1(void* aData)
	{
	__UHEAP_MARK;
	
	CTrapCleanup* tc = CTrapCleanup::New();
	TTEST(tc != NULL);

	TThreadData* p = static_cast <TThreadData*> (aData);
	TTEST(p != NULL);
	TThreadData& data = *p;
	
	RLogSession sess;
	TInt err = sess.Connect();
	TTEST2(err, KErrNone);

	while(++data.iIteration <= KTestIterCount)
		{
		TIpcArgs args;
		const TInt KFnCnt = sizeof(KLogIpcMsgCodes) / sizeof(KLogIpcMsgCodes[0]);
		TInt fnIdx = Math::Rand(data.iSeed) % KFnCnt;
		data.iFunction = KLogIpcMsgCodes[fnIdx];
		PrintIterationCount(data.iIteration);
		for(TInt argIdx=0;argIdx<KMaxMessageArguments;++argIdx)
			{
			//Initialize arguments
			data.iArgType[argIdx] = EBinArgType;
			if(argIdx > 0)
				{
				data.iArgType[argIdx] = static_cast <TArgType> (Math::Rand(data.iSeed) % ELastArgType);
				}
			switch(data.iArgType[argIdx])
				{
				case EIntArgType:
					data.iIntArg[argIdx] = Math::Rand(data.iSeed) % 9711;
					args.Set(argIdx, data.iIntArg[argIdx]);
					break;
				case ETextArgType:
					{
					TInt len = Math::Rand(data.iSeed) % KMaxDesArgLen;	
					data.iTextArg[argIdx].SetLength(len);
					args.Set(argIdx, &data.iTextArg[argIdx]);
					}
					break;
				case EBinArgType:
					{
					if(argIdx == 0)
						{
						//The operations ids are guaranteed to be sequential by logeng.dll implementation.
						TheLogIpcData.iOperationId = data.iIteration;
						//if(Math::Rand(data.iSeed) & 1)
						//	{
						//	TheLogIpcData.iOperationId = 0;
						//	}
						const TInt KTypeCnt = sizeof(KLogOpTypes) / sizeof(KLogOpTypes[0]);
						TInt typeIdx = Math::Rand(data.iSeed) % KTypeCnt;
						TheLogIpcData.iOperationType = KLogOpTypes[typeIdx];
						TheLogIpcData.iDataSlot1 = Math::Rand(data.iSeed);
						TheLogIpcData.iDataSlot2 = Math::Rand(data.iSeed);
						args.Set(argIdx, &TheLogIpcDataPtr);
						}
					else
						{
						TInt len = Math::Rand(data.iSeed) % KMaxDesArgLen;	
						data.iBinArg[argIdx].SetLength(len);
						args.Set(argIdx, &data.iBinArg[argIdx]);
						}
					}
					break;
				default:
					User::Panic(KPanicCategory2, KPanicCode2);
					break;
				}
			}
		//Send arguments
		//RDebug::Print(_L("##data.iFunction=%d\r\n"), data.iFunction);
		TRequestStatus stat;
		sess.Send(data.iFunction, args, stat);
		if(stat.Int() == KErrServerTerminated)
			{
			User::Panic(KPanicCategory, KPanicCode);
			}
		else if(stat.Int() == KRequestPending)
			{
			if(data.iFunction == ELogOperationInitiate)
				{
				//RDebug::Print(_L("##ELogOperationGetResult\r\n"));
				err = sess.Send(ELogOperationGetResult, args);
				if(err == KErrServerTerminated)
					{
					User::Panic(KPanicCategory, KPanicCode);
					}
				}
			else
				{
				//Give some time to the LogEng server to do something with that async request, then cancel it.
				//Otherwise, on a multi-core hardware, the LogEnd server will end up with a long queue of
				//pending requests, not cleared if the client side thread is panic'd. It will be a complete chaos.
				//RDebug::Print(_L("##data.iFunction=%d, wait and cancel async request\r\n"), data.iFunction);
				User::After(100000);
				TRequestStatus* s = &stat;
				User::RequestComplete(s, KErrCancel);
				}
			//RDebug::Print(_L("##---err=%d\r\n"), err);
			}
		}

	sess.Close();

	delete tc;	
	
	__UHEAP_MARKEND;
	
	return KErrNone;		
	}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

//Thread function to detect a crash in the server.
//The server should run for the duration of the test.
// return KErrAbort: If failure to start server
// return KErrServerTerminated: If server process is terminated 

TInt ServerWatcherFunc(TAny* /*aData*/)
	{
	__UHEAP_MARK;

	_LIT(KLogEngServerName,"LogServ*");

	TInt err;

	// Start the server if not already running
	RLogSession sess;
	err = sess.Connect();
	if (err != KErrNone)
		return KErrAbort;
	sess.Close();

	TFindProcess findProcess(KLogEngServerName);
	TFullName result;
	if ( findProcess.Next(result) != KErrNone )
		return KErrAbort;
	
	RProcess server;
	if( server.Open(findProcess, EOwnerProcess) != KErrNone)
		return KErrAbort;

	TRequestStatus status;
	server.Logon(status);	
 	User::WaitForRequest(status);
	
	server.Close();
	
	__UHEAP_MARKEND;

	return KErrServerTerminated;
	}


/**
@SYMTestCaseID			PDS-LOGENG-UT-4045
@SYMTestCaseDesc		In a loop, where the loop iterations are less than KTestIterCount (5000 at the moment), 
						the test creates a worker thread, which will behave as a malicious client. 
						If the worker thread crashes the LogEng server, then the worker thread
						dies notifying the main thread about the LogEng server crash. The main thread prints the 
						values used in the last IPC call and crashes the test.
@SYMTestPriority		High
@SYMTestActions			LogEng, Malicious client simulation test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ12746
*/	
void BadClientTest()
	{
	// Start a thread to watch the server process
	RThread serverWatcher;
	TInt err = serverWatcher.Create(_L("ServerWatcher"), &ServerWatcherFunc, 0x2000, 0x1000, 0x10000, NULL, EOwnerProcess);
	TRequestStatus serverStatus;
	serverWatcher.Logon(serverStatus);
	serverWatcher.Resume();

	TThreadData* p = new TThreadData;
	TEST(p != NULL);
	TThreadData& data = *p;
	data.iFunction = 0;
	TTime now;
	now.UniversalTime();
	data.iSeed = now.Int64();

	for(data.iIteration=0;data.iIteration<KTestIterCount;++data.iIteration)
		{
		PrintIterationCount(data.iIteration);
		
		//Run the malicious client (one test thread which will try to crash the LogEng server)
		User::After(200000);
		_LIT(KTestThreadName, "TLBCThr");
		RThread thread;
		err = thread.Create(KTestThreadName, &ThreadFunc1, 0x2000, 0x1000, 0x10000, &data, EOwnerProcess);
		if(err == KErrAlreadyExists)
			{
			TheTest.Printf(_L("##Iteration %d. Function %d. Thread \"%S\" already exists!\r\n"), data.iIteration, data.iFunction, &KTestThreadName);
			for(TInt i=0;i<KMaxMessageArguments;++i)
				{
				TheTest.Printf(_L("##Arg %d, Type %d\r\n"), i + 1, data.iArgType[i]);
				switch(data.iArgType[i])
					{
					case EIntArgType:
					    TheTest.Printf(_L("Integer, value=%d\r\n"), data.iIntArg[i]);
						break;
					case ETextArgType:
					    TheTest.Printf(_L("Text, length=%d\r\n"), data.iTextArg[i].Length());
						break;
					case EBinArgType:
					    TheTest.Printf(_L("Binary, length=%d\r\n"), data.iBinArg[i].Length());
						break;
					default:
					    TheTest.Printf(_L("Invalid argument type: %d\r\n"), data.iArgType[i]);
						break;
					}
				}
			break;
			}
		TEST2(err, KErrNone);
		User::SetJustInTime(EFalse);		
		TRequestStatus status;
		thread.Logon(status);
		TEST2(status.Int(), KRequestPending);
		thread.Resume();
		User::WaitForRequest(status, serverStatus);

		// If the Server has crashed then we must fail		
		if (serverStatus != KRequestPending) 
			{
			TheTest.Printf(_L("##Iteration=%d, Function=%d, Status=%d, Server Status=%d\r\n"), data.iIteration, data.iFunction, status.Int(), serverStatus.Int());
			}

		TExitType exitType = thread.ExitType();
		TInt exitReason = thread.ExitReason();
		thread.Close();
		User::SetJustInTime(ETrue);
		
		if(exitType == EExitPanic || serverStatus != KRequestPending)
			{
			if(exitReason == KPanicCode || serverStatus != KRequestPending)
				{
				TheTest.Printf(_L("##Server terminated!\r\n"));
				TheTest.Printf(_L("##Iteration=%d, Function=%d, iOperationType=%d, iDataSlot1=%d, iDataSlot2=%d\r\n"), 
						data.iIteration, data.iFunction,
						TheLogIpcData.iOperationType,
						TheLogIpcData.iDataSlot1,
						TheLogIpcData.iDataSlot2);
				for(TInt i=0;i<KMaxMessageArguments;++i)
					{
					TheTest.Printf(_L("##Arg %d, Type %d\r\n"), i + 1, data.iArgType[i]);
					switch(data.iArgType[i])
						{
						case EIntArgType:
						    TheTest.Printf(_L("Integer, value=%d\r\n"), data.iIntArg[i]);
							break;
						case ETextArgType:
						    TheTest.Printf(_L("Text, length=%d\r\n"), data.iTextArg[i].Length());
							break;
						case EBinArgType:
						    TheTest.Printf(_L("Binary, length=%d\r\n"), data.iBinArg[i].Length());
							break;
						default:
						    TheTest.Printf(_L("Invalid argument type: %d\r\n"), data.iArgType[i]);
							break;
						}
					}
				TEST(0);
				}
			}
		}//for
	delete p;

	// Check to see if the server crashed and not detected by client
	TEST(serverStatus.Int() == KRequestPending);
	serverWatcher.Kill(KErrCancel);
	serverWatcher.Close();
		
	}

/**
@SYMTestCaseID			PDS-LOGENG-UT-4044
@SYMTestCaseDesc		LogEng server startup - file I/O error simulation test.
						The test case shuts down the LogEng server in debug mode.
						Then attempts to connect to the server in a file I/O error simulation
						loop.
@SYMTestPriority		High
@SYMTestActions			LogEng server startup - file I/O error simulation test.
@SYMTestExpectedResults Test must not fail
@SYMREQ					REQ12746
*/	
void LogEngSrvStartupFileIoErrTest()
	{
	//Shut down the server if it is running
	RLogSession sess;
	TInt err = sess.Connect();
	TEST2(err, KErrNone);

	err = sess.Send(ELogMakeTransient, TIpcArgs(1));
	TEST2(err, KErrNone);
	sess.Close();
	
	//The shutdown delay is 2 seconds (defined in LogServShutdownTimer.h file). In this csase 5 seconds is more than enough.
	User::After(5000000);
	
	RFs fs;
	err = fs.Connect();
	TEST2(err, KErrNone);

	TBool finished = EFalse;
	TInt failCount = 0;

	while(!finished)
		{
		fs.SetErrorCondition(KErrCorrupt, ++failCount);
		TInt err = sess.Connect();
		fs.SetErrorCondition(KErrNone, 0);
		sess.Close();
		if(err == KErrNone)
			{
			finished = ETrue;
			}
		else
			{
			TEST2(err, KErrCorrupt);
			}
		}

	fs.Close();
	TheTest.Printf(_L("===LogEng Server Startup File I/O error simularion test succeeded at iteration %d\n"), failCount);
	}

void DoTests()
	{
	TheTest.Start(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4045: Bad client test"));
	BadClientTest();
#ifdef _DEBUG	
	TheTest.Next(_L(" @SYMTestCaseID:PDS-LOGENG-UT-4044: LogEng Server Startup - File I/O error simulation test"));
	LogEngSrvStartupFileIoErrTest();
#endif	
	}

TInt E32Main()
	{
	TheTest.Title();
	
	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	DoTests();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;

	User::Heap().Check();
	return KErrNone;
	}
