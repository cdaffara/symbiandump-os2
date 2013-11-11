// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <f32file.h>
#include <ecom/ecom.h>
#include <startup.hrh>
#include "EComSessionAux.h"
#include "EcomTestUtils.h"
#include "EcomTestIniFileUtils.h"
#include "EcomTestCompTestabilityUtils.h"

static RFs TheFs;
static RTest TheTest(_L("T_EcomSsaDisabled"));

_LIT(KEComExampleDllOnZ,	"Z:\\RAMOnly\\EComExample3.dll");
_LIT(KEComExampleDllOnC,	"C:\\sys\\bin\\EComExample3.dll");
_LIT(KEComPluginRscOnZ,		"Z:\\RAMOnly\\EComExample3.rsc");
_LIT(KEComPluginRscOnC,		"C:\\resource\\plugins\\EComExample3.rsc");

#ifdef __ECOM_SERVER_TESTABILITY__

_LIT8(KImplementationDataOnZ,	"RomOnly");
_LIT8(KImplementationDataOnC,	"RamOnly");

const TInt KOneSecond = 1000000;

#endif

//
//
//Test macroes and functions
//
//
#ifdef __ECOM_SERVER_TESTABILITY__
static void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		TheTest(EFalse, aLine);
		}
	}
#define TEST(arg) ::Check((arg), __LINE__)
#endif


static  void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		TheTest(EFalse, aLine);
		}
	}
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)

//
//
//Helper functions
//
//

/**
* Add plugins to C: drive so they can be discovered during NonStatic discovery
* state.
*/
static void SetupFiles()
	{
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExampleDllOnZ, KEComExampleDllOnC));
	TEST2(err, KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComPluginRscOnZ, KEComPluginRscOnC));
	TEST2(err, KErrNone);
	}

/**
* Remove the plugins from C: drive so we can leave a clean environment for
* the next test.
*/
static void CleanupFiles()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComExampleDllOnC));
	TEST2(err, KErrNone);

	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComPluginRscOnC));
	TEST2(err, KErrNone);
	}

static void KillEComServerL()
	{
	//Need to ensure that the EComServer process is killed before even starting this test by using
   	//the EComTestUtils library
   	_LIT(KEComServerProcessName,"ecomserver");
   	TRAPD(error, EComTestUtils::KillProcessL(KEComServerProcessName));
   	error=error;
	}

/**
* Test if the requested plugin is discovered
*@param			aInterfaceUid A UID specifying the required interface.
*@param			aResolutionParameters A descriptor specifying any additional
*				implementation characteristics to be fulfilled.
*@return		return TRUE if plugin exists
*/
TBool TestForPlugin(TUid aInterfaceUid,
					const TDesC8& aDataType)
	{
	TBool result = EFalse;
	RImplInfoPtrArray aImplInfoArray;
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aDataType);
	resolverParams.SetGenericMatch(ETrue);	// Allow wildcard matching

	REComSession::ListImplementationsL(
			aInterfaceUid,
			resolverParams,
			aImplInfoArray);

	if(aImplInfoArray.Count() > 0)
		{
		result = ETrue;
		}

	aImplInfoArray.ResetAndDestroy();

	return result;
	}

//
//
//Test functions
//
//
#ifdef __ECOM_SERVER_TESTABILITY__
/**
@SYMTestCaseID		SYSLIB-ECOM-CIT-0187
@SYMTestCaseDesc 	The SSA is disabled, the ECOM will do a full discovery with the
simulated Domain Manager.
@SYMTestPriority 	High
@SYMTestActions  	Check the plugins and the state of ECOM using client API
GetCurrentStartupStateL and ListImplementationsL.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ967
*/
LOCAL_C void TestEcomSsaDisabledL()
	{
	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	TInt state = EStartupStateUndefined;

	// Test Starts...

	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	TUid ifUid = {0x10009DC0};

	DisableSsa(TheTest, TheFs);

	//Open the ecom session which in turn should start the ecom server.
	//Next, wait to make sure that all the plugins are discovered.
	//Finally, get rid of the ecom session.
	REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);
	User::After(KOneSecond*10); //delay for 10 sec to ensure all plugins are discovered
	CleanupStack::PopAndDestroy(&ecomSession);

	//check that current state is EStartupStateUndefined
	TRAPD(err, state = GetCurrentStartupStateL());
	TEST2(err, KErrNone);
	TEST2(state, EStartupStateNonCritical);
	TEST2(ETrue, TestForPlugin(ifUid, KImplementationDataOnZ()));
	TEST2(ETrue, TestForPlugin(ifUid, KImplementationDataOnC()));

	REComSession::FinalClose();

	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST(startProcessHandleCount == endProcessHandleCount);
	TEST(startThreadHandleCount  == endThreadHandleCount);

	// Test Ends...

	__UHEAP_MARKEND;
	}
#endif //__ECOM_SERVER_TESTABILITY__

#ifdef __ECOM_SERVER_TESTABILITY__
// Type definition for pointer to member function.
// Used in calling t_ServerStartupMgr test functions.
typedef void (*ClassFuncPtrL) (void);

/**
Wrapper function to call all test functions
@param		testFunc pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	TheTest.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	testFuncL();

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST(startProcessHandleCount == endProcessHandleCount);
	TEST(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}
#endif //__ECOM_SERVER_TESTABILITY__

LOCAL_C void DoTestsL()
	{
#ifdef __ECOM_SERVER_TESTABILITY__
	DoBasicTestL(&TestEcomSsaDisabledL, _L("TestEcomSsaDisabledL"));
#endif //__ECOM_SERVER_TESTABILITY__

	//We are not going to run OOM tests for several reasons:
	//1- These tests are already done as part of the CEcomServer and
	//CServerStartupMgr OOM tests
	//2- EcomServer is running on another process
	//3- We need to kill the EcomServer and start it again.

	//For similar reasons to why we do not run OOM tests we should also
	//not test Ecom startup behaviour in this component test executable.
	//We will need to implement a new test executable for each test.
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	TheTest.Printf(_L("\n"));
	TheTest.Title();
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIB-ECOM-CIT-0187 Ecom Ssa Tests "));

	TEST2(TheFs.Connect(), KErrNone);

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err, ::KillEComServerL());
	TEST2(err, KErrNone);

	DisableEcomTestBehaviour(TheTest, TheFs);

	SetupFiles(); //Add plugins to C: drive

	TRAP(err,DoTestsL());
	TEST2(err, KErrNone);

	CleanupFiles(); //Cleanup after test. Remove the plugins from C: drive

	ResetSsa(TheTest, TheFs);

	//Make sure that following tests start a fresh version of EComServer
	TRAP(err, ::KillEComServerL());

	TheTest.End();
	TheTest.Close();

	//delete scheduler;
	delete cleanup;

	TheFs.Close();

	__UHEAP_MARKEND;

	User::Heap().Check();

	return KErrNone;
	}


