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
// This file contains tests for testing resolver related
// functionality linking against ecom.lib, i.e. using public API
// Where necessary stubs are implemented to help in writing test
// harness using RTest.
// This test assumes the following setup:
// DLL			/Dll UID
// EComExample /10009DB1 on Z:
// EComExample2/10009DB3 on Z:
// EComExample3/101F8477 on Z:
// EComExample4/101F8479 on Z:
// EComExample5/101F847B on C:
// Suicidal    /10009DB2 on Z:
// EComExample6/0x101F847D on Z:
// The tests work by checking that the correct implementations are chosen
// for interface 0x10009DC0 depending on parameters passed
// Implementations:
// Impl ID		On C:		On Z: e.g:Version/Default_data/dll_uid
// 10009DC3	-			Ver 1/"text/ wml"/10009DB1, Ver 2/"text/ wml"/10009DB3
// 10009DC4	-			Ver 1/"text/ *"/10009DB1,   Ver 2/"text/ *"/10009DB3
// 101F8478	-			Ver 1/""/101F8477
// 101F847A	-			Ver 1/""/101F8479
// 101F847C	Ver 1/""/101F847B	-
// 10009DC5	-			Ver 1/"Suicide"/10009DB2
// 101F847E	on Z		Ver 1/""/101F847D
// 
//

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
#include "../EcomTestUtils/EcomTestUtils.h"

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>

LOCAL_D RTest test(_L("t_suicide.exe"));

LOCAL_D CTrapCleanup* 	  TheTrapCleanup 	 = NULL;

LOCAL_D CActiveScheduler* TheActiveScheduler = NULL;

// Used for supressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

const TInt KOneSecond		= 1000000;

// Interface ID used for testing
const TUid KUidInterface	= {0x10009DC0};

// Resolver ID used for testing
const TUid KUidResolver		= {0x10009DD0};

// Implementaion ID used for testing
const TUid KUidTestImplementation = {0x10009DC5};

// Interface Implementation Uids used for testing
const TInt KUidImplementation1 = 0x10009DC3;
const TInt KUidImplementation2 = 0x10009DC4;
const TInt KUidImplementation3 = 0x10009DC5;
const TInt KUidImplementation4 = 0x101F8478;
const TInt KUidImplementation5 = 0x101F847A;
const TInt KUidImplementation6 = 0x101F847C;
const TInt KUidImplementation7 = 0x101F847E;

// Match string used for testing
_LIT8(KInterfaceResolveMatchStr,"suicidal");

_LIT(KSysBinDirectoryWildcard, "c:\\sys\\bin\\*.dll");
_LIT(KResourceDirectoryWildcard, "c:\\resource\\plugins\\*.rsc");

// Plugins used for this test program
_LIT(KEComExample5OnZ,		"z:\\RAMOnly\\EComExample5.dll");

_LIT(KEComExample5OnC,		"c:\\sys\\bin\\EComExample5.dll");
_LIT(KEComExample5RscOnC,	"c:\\resource\\plugins\\EComExample5.rsc");
_LIT(KEComExample4OnC,		"c:\\sys\\bin\\EComExample4.dll");
_LIT(KEComExample4RscOnC,	"c:\\resource\\plugins\\EComExample4.rsc");
// ROM file copied to the RAM for test purposes
_LIT(KEComExample5RscOnZ,	"z:\\RAMOnly\\EComExample5.rsc");


// Utility clean up function
LOCAL_C void CleanupEComArray(TAny* aArray);

//It is used by some test methods which are called two times:
//from normal test and from OOM test.
static void LeaveIfErrNoMemory(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		REComSession::FinalClose();
		User::Leave(aError);
		}
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0709
@SYMTestCaseDesc	    Tests for creation of plugin with resolver match string
@SYMTestPriority 	    High
@SYMTestActions  	    Checks for no error condition
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestCreateImplSuicideL()
	{
	// Tests creation of plugin with resolver match string
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0709 "));

	/* This should pick
	10009DC5	Ver 1/"Suicide"/10009DB2
	*/
	CExampleInterface* interfaceimpl = NULL;
	TRAPD(err, interfaceimpl = CExampleInterface::NewL(KInterfaceResolveMatchStr()));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);
	CleanupStack::PushL(interfaceimpl);

	test(KUidTestImplementation == interfaceimpl->ImplId());

	TRAP(err, interfaceimpl->DoMethodL());
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	CleanupStack::PopAndDestroy(interfaceimpl);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0710
@SYMTestCaseDesc	    Tests for Listing all Implementations for the Interface KUidInterface
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up REComSession::ListImplementationsL with interface UID.Picks up 6 implementations.
                        Checks for no memory exceptions.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestListAllImplL()
	{
	// Test for Listing all Implementations for the Interface KUidInterface
	//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0710 "));

	RImplInfoPtrArray ifArray;
	CleanupStack::PushL(TCleanupItem(CleanupEComArray, &ifArray));

	TRAPD(err, REComSession::ListImplementationsL(KUidInterface, ifArray));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	/* This should pick 6 implementations given below
	10009DC3	Ver 2/"text/ wml"/10009DB3
	10009DC4	Ver 2/"text/ *"/10009DB3
	101F8478	Ver 1/""/101F8477
	101F847A	Ver 1/""/101F8479
	101F847C	Ver 1/""/101F847B
	10009DC5	Ver 1/"Suicide"/10009DB2
	101F847E	Ver 1/""/101F847D
	*/

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 7);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];

		TDriveName driveName = info->Drive().Name();
		test.Printf(_L("%d. uid={%x} version=%d on drive %S\n"), count+1, info->ImplementationUid(), info->Version(), &driveName);

		switch(info->ImplementationUid().iUid)
			{
			case KUidImplementation1:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementation2:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementation3:
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementation4:
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementation5:
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementation6:
				test(info->Version()==1);
				test(info->Drive()==EDriveC);
				break;

			case KUidImplementation7:
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			default:
				test.Printf(_L("Unknown implementation Uid\n"));
				test(EFalse);
			}
		}
	// Empty the array of implementations
	CleanupStack::PopAndDestroy();//ifArray, results in a call to CleanupEComArray
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0711
@SYMTestCaseDesc	    Tests to list all implementations for an interface with specified
						match string and wildcard match
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up REComSession::ListImplementationsL with interface UID and resolver matching characteristics
                        Checks for no memory exceptions.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestListImplMatchStrWildcardL()
	{
	// Test to list all implementations for an interface with match string
	// and wildcard match
	//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0711 "));

	TEComResolverParams resolverParams;
	_LIT8(KImplementationTest,"text/wml");
	resolverParams.SetDataType(KImplementationTest());//Match string
	resolverParams.SetGenericMatch(ETrue);	// Allow wildcard matching

	RImplInfoPtrArray ifArray;
	CleanupStack::PushL(TCleanupItem(CleanupEComArray, &ifArray));

	TRAPD(err, REComSession::ListImplementationsL(KUidInterface, resolverParams, ifArray));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	/* This should pick 2 implementations given below
	10009DC3	Ver 2/"text/ wml"/10009DB3
	10009DC4	Ver 2/"text/ *"/10009DB3
	*/

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 2);

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];

		TDriveName driveName = info->Drive().Name();
		test.Printf(_L("%d. uid={%x} version=%d on drive %S\n"), count+1, info->ImplementationUid(), info->Version(), &driveName);

		switch(info->ImplementationUid().iUid)
			{
			case KUidImplementation1:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			case KUidImplementation2:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			default:
				test.Printf(_L("Unknown implementation UID\n"));
				test(EFalse);
			}
		}

	// Empty the array of implementations
	CleanupStack::PopAndDestroy();//ifArray, results in a call to CleanupEComArray
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0712
@SYMTestCaseDesc	    Tests to list all implementations for an interface with specified
						match string and no wildcard match
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up REComSession::ListImplementationsL with interface UID and resolver matching characteristics
                        Checks for no memory exceptions.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestListImplMatchStrL()
	{
	// Test to list all implementations for an interface with specified
	// match string and no wildcard match
	//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0712 "));

	TEComResolverParams resolverParams;
	_LIT8(KImplementationTest,"text/wml");
	resolverParams.SetDataType(KImplementationTest());//Match string
	resolverParams.SetGenericMatch(EFalse);	// Don't allow wildcard matching

	RImplInfoPtrArray ifArray;
	CleanupStack::PushL(TCleanupItem(CleanupEComArray, &ifArray));

	TRAPD(err, REComSession::ListImplementationsL(KUidInterface, resolverParams, ifArray));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"),availCount);
	test(availCount == 1);

	/* This should pick
	10009DC3	Ver 2/"text/wml"/10009DB3
	*/

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];

		TDriveName driveName = info->Drive().Name();
		test.Printf(_L("%d. uid={%x} version=%d on drive %S\n"),
					count+1, info->ImplementationUid(), info->Version(), &driveName);

		switch(info->ImplementationUid().iUid)
			{
			case KUidImplementation1:
				test(info->Version()==2);
				test(info->Drive()==EDriveZ);
				break;

			default:
				test.Printf(_L("Unknown implementation Uid\n"));
				test(EFalse);
			}
		}
	// Empty the array of implementations
	CleanupStack::PopAndDestroy();//ifArray, results in a call to CleanupEComArray
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0713
@SYMTestCaseDesc	    Tests to list all Implementations for an Interface with specified
						Resolver ID, Match String and Wildcard match
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up REComSession::ListImplementationsL with interface UID Resolver ID, matching characteristics
                        Checks for no memory exceptions.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void TestListAllImplResolverIDL()
	{
	// Test to list all Implementations for an Interface with specified
	// Resolver ID, Match String and Wildcard match
	//
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0713 "));

	TEComResolverParams resolverParams;
	resolverParams.SetDataType(KInterfaceResolveMatchStr());
	resolverParams.SetGenericMatch(ETrue);	// Allow wildcard matching

	RImplInfoPtrArray ifArray;
	CleanupStack::PushL(TCleanupItem(CleanupEComArray, &ifArray));

	TRAPD(err, REComSession::ListImplementationsL(KUidInterface,
												  resolverParams,
												  KUidResolver,
												  ifArray));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	const TInt availCount = ifArray.Count();
	test.Printf(_L("Found %d implementations.\n"), availCount);
	test(availCount == 1);

	/* This should pick
	10009DC5	Ver 1/"Suicide"/10009DB2
	*/

	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = ifArray[count];

		TDriveName driveName = info->Drive().Name();
		test.Printf(_L("%d. uid={%x} version=%d on drive %S\n"), count+1, info->ImplementationUid(), info->Version(), &driveName);

		switch(info->ImplementationUid().iUid)
			{
			case KUidImplementation3:
				test(info->Version()==1);
				test(info->Drive()==EDriveZ);
				break;

			default:
				test.Printf(_L("Unknown implementation UID\n"));
				test(EFalse);
			}
		}

	// Empty the array of implementations
	CleanupStack::PopAndDestroy();//ifArray, results in a call to CleanupEComArray
	}

typedef void (*ClassFuncPtrL) (void);

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0714
@SYMTestCaseDesc	    Function to call all test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up test function and checks for handle counts.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all test functions

@param		testFuncL pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0714 "));
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	//Call the test function
	(*testFuncL)();

	REComSession::FinalClose();

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0715
@SYMTestCaseDesc	    Wrapper function to call all OOM test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up test function and checks for handle counts.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all OOM test functions

@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTest(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0715 "));
	test.Next(aTestDesc);

	TInt err, tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (*testFuncL)());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		// release handles
		if(err == KErrNone)
			{
			REComSession::FinalClose();
			}

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

	test(err == KErrNone);
	test.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

LOCAL_C void DoTestsL()
	{
	__UHEAP_MARK;

	// Basic tests
	DoBasicTestL(TestCreateImplSuicideL, _L("TestCreateImplSuicideL"));
	DoBasicTestL(TestListImplMatchStrL, _L("TestListImplMatchStrL"));
	DoBasicTestL(TestListImplMatchStrWildcardL, _L("TestListImplMatchStrWildcardL"));
	DoBasicTestL(TestListAllImplL, _L("TestListAllImplL"));
	DoBasicTestL(TestListAllImplResolverIDL, _L("TestListAllImplResolverIDL"));

	// OOM tests
	DoOOMTest(TestCreateImplSuicideL, _L("OOM TestCreateImplSuicide"));
	DoOOMTest(TestListImplMatchStrL, _L("OOM TestListImplMatchStr"));
	DoOOMTest(TestListImplMatchStrWildcardL, _L("OOM TestListImplMatchStrWildcard"));
	DoOOMTest(TestListAllImplL, _L("OOM TestListAllImpl"));
	DoOOMTest(TestListAllImplResolverIDL, _L("OOM TestListAllImplResolverID"));

	__UHEAP_MARKEND;
	}

// This function is used for cleanup support of locally declared arrays
LOCAL_C void CleanupEComArray(TAny* aArray)
	{
	(static_cast<RImplInfoPtrArray*>(aArray))->ResetAndDestroy();
	}

// Deleting plugin from the RAM for cleanup purpose
inline LOCAL_C void DeleteTestPlugins()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KSysBinDirectoryWildcard));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KResourceDirectoryWildcard));
	}

// Copies the Plugins to specific folder for testing purpose
LOCAL_C void CopyPlugins()
	{
	DeleteTestPlugins();

	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExample5OnZ, KEComExample5OnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample5RscOnZ, KEComExample5RscOnC));
	test(err==KErrNone);

	// KEComExample4OnC & KEComExample4RscOnC are copied to C: Drive for
	// testing in t_listimplementation test program. These files should
	// be present in Z:. Please make sure these are present in Z only.
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample4OnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample4RscOnC));
	}


//Initialise the Active Scheduler
//
LOCAL_C void SetupL()
	{
	// Construct and install the Active Scheduler. The Active Schedular is needed
	// by components used by this test as they are ActiveObjects.
	TheActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("Suicide Tests"));

	TheTrapCleanup = CTrapCleanup::New();
	TRAPD(err, SetupL());
	test(err == KErrNone);

	CopyPlugins();
	// The reason for the folowing delay is:
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that an active object,
	// which scans plugin directories. So the discovering service is asynchronous.
	// We have to wait some time until it finishes.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);

	TRAP(err,DoTestsL());
	test(err==KErrNone);

	// Cleanup files. If the cleanup fails that is no problem,
	// as any subsequent tests will replace them. The only downside
	// would be the disk not being tidied
	DeleteTestPlugins();

	delete TheActiveScheduler;
	delete TheTrapCleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
	}
