// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CEComServer and CEComSession OOM tests
// 
//

#include <e32test.h>
#include <e32math.h>
#include <bautils.h>
#include "EComServerStart.h"
#include "EComServer.h"
#include "EComServerSession.h"
#include "EcomTestUtils.h"
#include "EcomTestIniFileUtils.h"
#include "ServerStartupManager.h"
#include "Discoverer.h"
#include "DiscovererObserver.h"

static RTest TheTest(_L("ECOM Server - OOM test"));
const TUid KCExampleInterfaceUid = {0x10009DC0};
static RFs TheFs;

static void KillEComServerL()
	{
	//Need to ensure that the EComServer process is killed before even starting this test by using
   	//the EComTestUtils library
   	_LIT(KEComServerProcessName,"ecomserver");
   	TRAPD(error, EComTestUtils::KillProcessL(KEComServerProcessName));
   	error=error;
	}

//
//
//Test macroes and functions
//
//

static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//
//

void NewLC_OOMTestL()
	{
	TInt processHandlesS = 0;
	TInt threadHandlesS = 0;
	TInt processHandlesE = 0;
	TInt threadHandlesE = 0;
	RThread().HandleCount(processHandlesS, threadHandlesS);
	for(TInt count=1;;count +=100)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		CEComServer* ecomServer = NULL;
		TRAPD(err, ecomServer = CEComServer::NewLC();
				   CleanupStack::PopAndDestroy(ecomServer));
		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			__UHEAP_MARKEND;
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST2(err, KErrNone);
			}
		__UHEAP_RESET;
		}
	__UHEAP_RESET;
	RThread().HandleCount(processHandlesE, threadHandlesE);
	TEST(processHandlesS == processHandlesE);
	TEST(threadHandlesS == threadHandlesE);
	}

void ListImplementations_OOMTestL()
	{
	TInt processHandlesS = 0;
	TInt threadHandlesS = 0;
	TInt processHandlesE = 0;
	TInt threadHandlesE = 0;
	RThread().HandleCount(processHandlesS, threadHandlesS);


	for(TInt count=1;;++count)
		{
		CEComServer* ecomServer = CEComServer::NewLC();
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		TClientRequest clientReq;
		RArray<TUid> extendedInterfaces;
		CleanupClosePushL(extendedInterfaces);
		RImplInfoArray* ifArray=NULL;
		TRAPD(err, ifArray = ecomServer->ListImplementationsL(KCExampleInterfaceUid,extendedInterfaces,clientReq));
		CleanupStack::PopAndDestroy(&extendedInterfaces);
		if (ifArray!=NULL)
			{
			ifArray->Close();
			delete ifArray;
			}
		if(err == KErrNoMemory)
			{
			CleanupStack::PopAndDestroy(ecomServer);
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			CleanupStack::PopAndDestroy(ecomServer);
			__UHEAP_MARKEND;
			//implInfoArray should not be deleted! The caller does not take the ownership.
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else
			{
			CleanupStack::PopAndDestroy(ecomServer);
			__UHEAP_MARKEND;
			TEST2(err, KErrNone);
			}
		__UHEAP_RESET;
		}
	__UHEAP_RESET;
	//CleanupStack::PopAndDestroy(ecomServer);
	RThread().HandleCount(processHandlesE, threadHandlesE);
	TEST(processHandlesS == processHandlesE);
	TEST(threadHandlesS == threadHandlesE);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-3714
@SYMTestCaseDesc 		Tests CEComServer::ListImplementationsL	with customer resolver.
@SYMTestPriority 		High
@SYMTestActions  		Calls ListImplementationsL(TUid,TUid,const RExtendedInterfacesArray&,const TClientRequest&)
						with customer resolver.
@SYMTestExpectedResults No OOM errors.
@SYMDEF 				DEF111196
*/
void ListImplementations_OOMTest1L()
	{
	TInt processHandlesS = 0;
	TInt threadHandlesS = 0;
	TInt processHandlesE = 0;
	TInt threadHandlesE = 0;
	RThread().HandleCount(processHandlesS, threadHandlesS);


	for(TInt count=1;;++count)
		{
		CEComServer* ecomServer = CEComServer::NewLC();
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		TUid resolverUid = {0x10009DD0};

		TClientRequest clientReq;
		RArray<TUid> extendedInterfaces;
		CleanupClosePushL(extendedInterfaces);
		RImplInfoArray* ifArray=NULL;

		TRAPD(err, ifArray = ecomServer->ListImplementationsL(KCExampleInterfaceUid,resolverUid,extendedInterfaces,clientReq));
		CleanupStack::PopAndDestroy(&extendedInterfaces);
		if (ifArray!=NULL)
			{
			ifArray->Close();
			delete ifArray;
			}
		if(err == KErrNoMemory)
			{
			CleanupStack::PopAndDestroy(ecomServer);
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			CleanupStack::PopAndDestroy(ecomServer);
			__UHEAP_MARKEND;
			//implInfoArray should not be deleted! The caller does not take the ownership.
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else
			{
			CleanupStack::PopAndDestroy(ecomServer);
			__UHEAP_MARKEND;
			TEST2(err, KErrNone);
			}
		__UHEAP_RESET;
		}
	__UHEAP_RESET;
	//CleanupStack::PopAndDestroy(ecomServer);
	RThread().HandleCount(processHandlesE, threadHandlesE);
	TEST(processHandlesS == processHandlesE);
	TEST(threadHandlesS == threadHandlesE);
	}

void GetResolvedDllInfo_OOMTestL()
	{
	TInt processHandlesS = 0;
	TInt threadHandlesS = 0;
	TInt processHandlesE = 0;
	TInt threadHandlesE = 0;
	RThread().HandleCount(processHandlesS, threadHandlesS);

	CEComServer* ecomServer = CEComServer::NewLC();
	TClientRequest clientReq;
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	RImplInfoArray* implInfoArray = ecomServer->ListImplementationsL(KCExampleInterfaceUid,extendedInterfaces,clientReq);
	CleanupStack::PopAndDestroy(&extendedInterfaces);

	TEST(implInfoArray->Count() > 0);

	for(TInt count=1;;++count)
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, count);
		__UHEAP_MARK;

		TEntry dllInfo;
		TUid dtorIdKey;
		TClientRequest clntRq;
		CImplementationInformation* info = (*implInfoArray)[0];
		TRAPD(err, ecomServer->GetResolvedDllInfoL(info->ImplementationUid(),
												   dllInfo, dtorIdKey, clntRq));
		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			__UHEAP_MARKEND;
			RDebug::Print(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else
			{
			__UHEAP_MARKEND;
			TEST2(err, KErrNone);
			}
		__UHEAP_RESET;
		}
	__UHEAP_RESET;
	//implInfoArray should be deleted! The caller takes the ownership.
	if (implInfoArray!=NULL)
		{
		implInfoArray->Close();
		delete implInfoArray;
		}
	CleanupStack::PopAndDestroy(ecomServer);
	RThread().HandleCount(processHandlesE, threadHandlesE);
	TEST(processHandlesS == processHandlesE);
	TEST(threadHandlesS == threadHandlesE);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0177
@SYMTestCaseDesc 	Check that IsSSA works when ecomsrvr has different values.
@SYMTestPriority 	High
@SYMTestActions  	Check that IsSSA returns ETrue when ecomsrvr ini does not exist.
                    Check that IsSSA returns the value in the ecomsrvr ini file when
                    the file exists.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ967
*/
void IsSsa_TestL()
	{
	TInt processHandlesS = 0;
	TInt threadHandlesS = 0;
	TInt processHandlesE = 0;
	TInt threadHandlesE = 0;
	RThread().HandleCount(processHandlesS, threadHandlesS);

	TBool res;

	TestEnableDisableSsaL(TheTest, TheFs);

    /*****************************************************************/
	//test that IsSSA() returns ETrue when ecomsrvr.ini file does
	//not exist.

	//enable ssa
	EnableSsa(TheTest, TheFs);

	//test that IsSSA() returns ETrue
	CEComServer* server=CEComServer::NewLC();
	res = server->IsSSA(TheFs);
	CleanupStack::PopAndDestroy();
	ResetSsa(TheTest, TheFs);
	TEST(res);

    /*****************************************************************/
	//test that IsSSA() returns EFalse when ecomsrvr.ini file exists.

	//disable ssa
	DisableSsa(TheTest, TheFs);

	//test that IsSSA() returns EFalse
	server=CEComServer::NewLC();
	res = server->IsSSA(TheFs);
	CleanupStack::PopAndDestroy();
	ResetSsa(TheTest, TheFs);
	TEST(!res);

	RThread().HandleCount(processHandlesE, threadHandlesE);
	TEST(processHandlesS == processHandlesE);
	TEST(threadHandlesS == threadHandlesE);
	}


/**
Thread entry point for test thread.
Kills the existing ECOM server and launches a new instance

*/
TInt ServerStartThreadEntryL(TAny* /*a*/)
	{
	__UHEAP_MARK;

	//Threshold set so that a OOM failure occur whilst constructing CEComServer::iRegistryData
	const TInt KFailThreshold = 500;

	TInt dummy;
	TAny* ptr  = NULL;

	::KillEComServerL();

	//Invoking OOM failure by allocating most of the heap before running the server
	RHeap heap = User::Heap();
	TInt avail = heap.Available(dummy);
    ptr = User::Alloc(avail-KFailThreshold);

	TEST(ptr != NULL); //Check that the allocation was sucessful

	TInt result = ServerStart();
	delete ptr;

	__UHEAP_MARKEND;

	return result;
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-3163
@SYMTestCaseDesc 	Verify the startup behaviour of the ECOM server under OOM conditions
@SYMTestPriority 	High
@SYMTestActions  	Generate an OOM condition.
					Create a new thread which will launch the ECOM server.
					Wait for the thread to exit and check the thread exit type
					and reason to verify behaviour.
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF094675
*/
void StartServer_OOMTest()
	{

	_LIT(KStartThreadName,"Server Start Thread");

	//Create a new thread to launch the ECOM server
	RThread testThread;
	testThread.Create(KStartThreadName, ServerStartThreadEntryL,
				KDefaultStackSize,KMinHeapSize,KMinHeapSize,NULL);
	TRequestStatus status;
	testThread.Logon(status);
	testThread.Resume();

	//Wait for the thread to exit
	User::WaitForRequest(status);

	//Obtain exit type and reason for test thread
	TExitType exitType = testThread.ExitType();
	TInt exitReason = testThread.ExitReason();

	//close the thread handle
	testThread.Close();

	//Verify the exit reason and exit code
	//Exit type is TExitType::EExitKill when E32Main() exit normally
	TEST(exitType == EExitKill);
	TEST(exitReason == KErrNoMemory);

	}

void DoTestL()
	{
	TheTest.Start(_L("CEComServer::NewLC - OOM test"));
	::NewLC_OOMTestL();

	TheTest.Next(_L("CEComServer::ListImplementationsL - OOM test"));
	::ListImplementations_OOMTestL();

	TheTest.Next(_L("CEComServer::GetResolvedDllInfoL - OOM test"));
	::GetResolvedDllInfo_OOMTestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0177 CEComServer::IsSsa test "));
	::IsSsa_TestL();

	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3163 Server Startup Panic test "));
    ::StartServer_OOMTest();

    TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3714 CEComServer::ListImplementationsL with customer resolver - OOM test "));
	::ListImplementations_OOMTest1L();

	}

TInt E32Main()
	{
	__UHEAP_MARK;

    ::KillEComServerL();

	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);

	TEST2(TheFs.Connect(), KErrNone);

	CActiveScheduler* activeSched = new CActiveScheduler;
	TEST(activeSched != NULL);
	CActiveScheduler::Install(activeSched);

	TheTest.Title();
	TRAPD(err, ::DoTestL());
	TEST2(err, KErrNone);

	ResetSsa(TheTest, TheFs);

	TheTest.End();
	TheTest.Close();

	delete activeSched;

	TheFs.Close();

	delete tc;

	__UHEAP_MARKEND;

	User::Heap().Check();
	return KErrNone;
	}
