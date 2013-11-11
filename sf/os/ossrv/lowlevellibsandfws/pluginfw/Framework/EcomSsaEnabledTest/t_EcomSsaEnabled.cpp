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
static RTest TheTest(_L("T_EcomSsaEnabled"));

_LIT(KEComExampleDllOnZ,	"Z:\\RAMOnly\\EComExample3.dll");
_LIT(KEComExampleDllOnC,	"C:\\sys\\bin\\EComExample3.dll");
_LIT(KEComPluginRscOnZ,		"Z:\\RAMOnly\\EComExample3.rsc");
_LIT(KEComPluginRscOnC,		"C:\\resource\\plugins\\EComExample3.rsc");

#ifdef __ECOM_SERVER_TESTABILITY__

_LIT8(KImplementationDataOnZ,	"RomOnly");
_LIT8(KImplementationDataOnC,	"RamOnly");

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

#ifdef __ECOM_SERVER_TESTABILITY__
/**
* Test if the requested plugin is discovered
*@param			aInterfaceUid A UID specifying the required interface.
*@param			aResolutionParameters A descriptor specifying any additional
*				implementation characteristics to be fulfilled.
*@return		return TRUE if plugin exists
*/
static TBool TestForPlugin(TUid aInterfaceUid,
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
#endif

//
//
//Test functions
//
//
#ifdef __ECOM_SERVER_TESTABILITY__
/**
Wrapper function to call change and process startup state test functions
and check the current startup state is matched with the expected startup state.
@param		aState A startup state to be set in ECOM
@param		aExpectedState An expected startup state in ECOM
*/
LOCAL_C void ChangeAndProcessStartupState(TInt aState, TInt aExpectedState)
{

	TInt state = EStartupStateUndefined;
	TRAPD(err, ChangeStartupStateL(aState));
	TEST2(err, KErrNone);
	TRAP(err, ProcessCurrentStartupStateL());
	TEST2(err, KErrNone);
	TRAP(err, state = GetCurrentStartupStateL());
	TEST2(err, KErrNone);
	TEST2(state, aExpectedState);
}

/**
@SYMTestCaseID		SYSLIB-ECOM-CIT-0188
@SYMTestCaseDesc 	The SSA is enabled, the ECOM will do a staged discovery with the
simulated Domain Manager.
@SYMTestPriority 	High
@SYMTestActions  	Check the plugins and the state of ECOM are correct at each state
 using client API GetCurrentStartupStateL and ListImplementationsL.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ967
*/
LOCAL_C void TestEcomSsaEnabledL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CIT-0188 "));
	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	// Test Starts...

	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	TUid ifUid = {0x10009DC0};

	EnableSsa(TheTest, TheFs);

	//Change the state to a user defined state then
	//check that current state is still EStartupStateUndefined
	ChangeAndProcessStartupState(EStartupStateUndefined+1,EStartupStateUndefined);

	//Change the state to a user defined state then
	//check that current state is still EStartupStateUndefined
	ChangeAndProcessStartupState(EStartupStateCriticalStatic-1,EStartupStateUndefined );

	//Change the state to EStartupStateCriticalStatic then
	//check that current state is EStartupStateCriticalStatic
	//check that a plugin from RO drive is discovered
	ChangeAndProcessStartupState(EStartupStateCriticalStatic,EStartupStateCriticalStatic );
	TEST2(ETrue, TestForPlugin(ifUid, KImplementationDataOnZ()));

	//Change the state to a user defined state then
	//check that current state is still EStartupStateCriticalStatic
	ChangeAndProcessStartupState(EStartupStateCriticalStatic+1,EStartupStateCriticalStatic);

	//Change the state to a user defined state then
	//check that current state is still EStartupStateCriticalStatic
	ChangeAndProcessStartupState(EStartupStateCriticalDynamic-1,EStartupStateCriticalStatic);

	//Change the state to EStartupStateCriticalDynamic then
	//check that current state is EStartupStateCriticalDynamic
	ChangeAndProcessStartupState(EStartupStateCriticalDynamic, EStartupStateCriticalDynamic);

	//Change the state to a user defined state then
	//check that current state is still EStartupStateCriticalDynamic
	ChangeAndProcessStartupState(EStartupStateCriticalDynamic+1, EStartupStateCriticalDynamic);

	//Change the state to a user defined state then
	//check that current state is still EStartupStateCriticalDynamic
	ChangeAndProcessStartupState(EStartupStateNonCritical-1,EStartupStateCriticalDynamic);

	//Change the state to EStartupStateNonCritical then
	//check that current state is EStartupStateNonCritical
	//check that a plugin from RO drive is discovered
	ChangeAndProcessStartupState(EStartupStateNonCritical,EStartupStateNonCritical );
	TEST2(ETrue, TestForPlugin(ifUid, KImplementationDataOnC()));

	//Change the state to a user defined state then
	//check that current state is still EStartupStateNonCritical
	ChangeAndProcessStartupState(EStartupStateNonCritical+1, EStartupStateNonCritical);

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
// Used in calling T_EcomSsaEnabled test functions.
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
	DoBasicTestL(&TestEcomSsaEnabledL, _L("TestEcomSsaEnabledL"));
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
	TheTest.Start(_L("Ecom Ssa Tests"));

	TEST2(TheFs.Connect(), KErrNone);

	// get clean-up stack
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err, ::KillEComServerL());
	TEST2(err, KErrNone);

	EnableEcomTestBehaviour(TheTest, TheFs);

	SetupFiles(); //Add plugins to C: drive

	TRAP(err,DoTestsL());
	TEST2(err, KErrNone);

	CleanupFiles(); //Cleanup after test. Remove the plugins from C: drive

	DisableEcomTestBehaviour(TheTest, TheFs);

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

