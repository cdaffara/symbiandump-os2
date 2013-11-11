// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32panic.h>
#include <f32file.h>
#include <bautils.h>
#include "../EcomTestUtils/EcomTestUtils.h"
#include "LoadManager.h"
#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins

_LIT(KEComExDllOnC, "c:\\sys\\bin\\EComExample5.dll");
_LIT(KEComRscFileOnZ, "z:\\RAMOnly\\EComExample5.rsc");
_LIT(KEComRscFileOnC, "c:\\resource\\plugins\\EComExample5.rsc");

_LIT(KEComExampleDllOnC,		"C:\\sys\\bin\\EComExample.dll");
_LIT(KEComExample2DllOnC,		"C:\\sys\\bin\\EComExample2.dll");
_LIT(KEComExample3DllOnC,		"C:\\sys\\bin\\EComExample3.dll");

_LIT(KEComExampleRscOnC,		"C:\\resource\\plugins\\EComExample.rsc");
_LIT(KEComExample2RscOnC,		"C:\\resource\\plugins\\EComExample2.rsc");
_LIT(KEComExample3RscOnC,		"C:\\resource\\plugins\\EComExample3.rsc");

_LIT(KEComExampleRscOnZ,		"Z:\\RAMOnly\\EComExample.rsc");
_LIT(KEComExample2RscOnZ,		"Z:\\RAMOnly\\EComExample2.rsc");
_LIT(KEComExample3RscOnZ,		"Z:\\RAMOnly\\EComExample3.rsc");


_LIT(KEComExDllOnZ, "z:\\RAMOnly\\EComExample5.dll");
_LIT(KEComExampleDllOnZ,			"Z:\\RAMOnly\\EComExample.dll");
_LIT(KEComExample2DllOnZ,		"Z:\\RAMOnly\\EComExample2.dll");
_LIT(KEComExample3DllOnZ,		"Z:\\RAMOnly\\EComExample3.dll");

/** Test cases covered in this test code:
1.	Thread create imp destroy then not calling FinalClose() will cause a panic inside the thread( capture this panic)(memory leak)

2.	Thread list implementation then not calling FinalClose() will cause a panic inside the thread(capture this panic)(memory leak).

3.	Thread Create Impl1, Create Impl2, Destroy Impl2, Destory Impl1 then FinalClose() no memory leak.

4.	Thread create Impl1, call FinalClose() 2x, Destroy Impl1 then FinalClose() to show that FinalClose() can be called many times with no unknown consequence.

5.	REComSession::public constructor (without OpenL), called FinalClose () 2x, then Close(), no problem and memory leak.

6.	REComSession::OpenL,call Close() then call FinalClose() to show no memory leak problem.

7.  Test calling REComSession::FinalClose() to show no memory leak, errors, crash problem
*/

LOCAL_D RTest test(_L("Final Close Test"));

class RFinalCloseTest
	{
public:
	//Test case 1
	static TInt Create_Destroy_Without_FinalClose(TAny*);
	static TInt Create_Destroy_With_FinalCloseL();
	static TInt Create_Destroy_TestL();

	//Test case 2
	static TInt ListImpl_Without_FinalClose(TAny*);
	static TInt ListImpl_With_FinalCloseL();
	static void ListImplementation_TestL();

	//Test case 3
	static void Create_Destroy2_With_FinalCloseL();

	//Test case 4
	static void Create_Destroy3_With_FinalCloseL();

	//Test case 5
	static void Create_Destroy4_With_FinalCloseL();

	//Test case 6
	static void FinalClose();

	//Test case 7
	static void NonConstructL_Create_Destroy();

	static void ThreadPanicTest(const TDesC& aName,TThreadFunction aFunction);
	};

/**
Test creating an implementation, deleting the implementation
and then call FinalClose(). Check that there is no crash and memory leak

@SYMTestCaseID          SYSLIB-ECOM-CT-0759
@SYMTestCaseDesc	    Tests for creating and destroying an implementation
@SYMTestPriority 	    High
@SYMTestActions  	    Create and delete a new implementation with UID and initialisation parameters.
                        Call up REComSession::FinalClose
						Check for no crash and memory leaks.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
TInt RFinalCloseTest::Create_Destroy_With_FinalCloseL()
	{
	__UHEAP_MARK;

	Create_Destroy_TestL();

	REComSession::FinalClose();

	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
Test creating an implementation, deleting the implementation
Never call FinalClose() to show memory leak panic

@SYMTestCaseID          SYSLIB-ECOM-CT-0760
@SYMTestCaseDesc	    Tests for creating and destroying an implementation
@SYMTestPriority 	    High
@SYMTestActions  	    Create and delete a new implementation with UID and initialisation parameters.
                    	FinalClose is omitted to show memory leak panic
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
TInt RFinalCloseTest::Create_Destroy_Without_FinalClose(TAny*)
	{
	
	__UHEAP_MARK;
	CTrapCleanup* threadcleanup = CTrapCleanup::New();

	TRAPD(err,Create_Destroy_TestL());

	test(err==KErrNone);
	//This FinalClose() is purposely omitted for testing the PANIC
	//REComSession::FinalClose();

	delete threadcleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

//The test code for creating and destroying an implementation
//To be used in the two test cases above
TInt RFinalCloseTest::Create_Destroy_TestL()
	{
	TUid dTorKey;
	TUid implUid={0x101F8478};

	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	CExampleInterface* dllPtr = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid,dTorKey,&initParams));

	//Check the return implementation instantiation has the same implementation uid
	test(dllPtr->ImplId()==implUid);
	//The returned Uid to be used in Destruction should match the implementation uid as well
	CInstanceInfoSimple* instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (dTorKey.iUid);
	test(instanceInfo->ImplementationUid()==implUid);

	REComSession::DestroyedImplementation(dTorKey);

	delete dllPtr;
	dllPtr=0;

	return KErrNone;
	}

//The test code for listing implementation of an interface Uid.
//The test also verify all the implementations in the list by
//checking all the implementation Uid and their version.
void RFinalCloseTest::ListImplementation_TestL()
	{
	/**
	Some Plugin in Z that can be used for testing purposes
	Interface UID	DLL	UID		Implementation UID	Version		DllFile
	-------------------------------------------------------------------
	0x10009DC0		0x10009DB1	0x10009DC3			1		Z:\\..\\EComExample.dll		old_version
					0x10009DB1	0x10009DC4			1		Z:\\..\\EComExample.dll		old_version
					0x10009DB3	0x10009DC3			2		Z:\\..\\EComExample2.dll	=
					0x10009DB3	0x10009DC4			2		Z:\\..\\EComExample2.dll	=
					0x101F8477	0x101F8478			1		Z:\\..\\EComExample3.dll	=
					0x101F8479  0x101F847A			1		Z:\\..\\EComExample4.dll	=
					0x10009DB2	0x10009DC5			1		Z:\\..\\Suidical.dll		=

	*/
	TUid interfaceUid={0x10009DC0};
	RImplInfoPtrArray implArray;

	//Array of all the expected implementation Uid;
	RArray<TInt> expectedUidArray;
	expectedUidArray.Append(0x10009DC3);
	expectedUidArray.Append(0x10009DC4);
	expectedUidArray.Append(0x101F8478);
	expectedUidArray.Append(0x101F847A);
	expectedUidArray.Append(0x10009DC5);

	REComSession::ListImplementationsL(interfaceUid,implArray);

	//Expected number of implementations retured=5
	//This line has been removed as it is not safe to always assume there will be
	//a fixed number of implementations based on an the interface 0x10009DC0.
	//test(implArray.Count()==5);


	//Check that the implementation uid returned matched the specs above
	for (TInt i=0;i<implArray.Count();i++)
	{
		TUid implUid=implArray[i]->ImplementationUid();
		if (expectedUidArray.Find(implUid.iUid)!=KErrNotFound)
			{
			//check for the latest version for 10009DC3 & 10009DC4
			if (implUid.iUid==0x10009DC3 || implUid.iUid==0x10009DC4)
				test(implArray[i]->Version()==2);
			//all the other implementations have version 1
			else
				test(implArray[i]->Version()==1);
			}
	}
	expectedUidArray.Reset();
	expectedUidArray.Close();
	implArray.ResetAndDestroy();
	}

/**
The test code for testing the proper use of ListImplementationsL where
after calling that function we call REComSession::FinalClose()

@SYMTestCaseID          SYSLIB-ECOM-CT-0761
@SYMTestCaseDesc	    Tests for proper listing of implementation of an interface UID
@SYMTestPriority 	    High
@SYMTestActions  	    Call up REComSession::FinalClose after ListImplementation_TestL
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
TInt RFinalCloseTest::ListImpl_With_FinalCloseL()
	{
	__UHEAP_MARK;

	ListImplementation_TestL();

	REComSession::FinalClose();

	__UHEAP_MARKEND;

	return KErrNone;
	}

/**
The test code for testing the improper use of ListImplementationsL where
after the call, we never invoke REComSession::FinalClose().This will
result in some memory leak panic.

@SYMTestCaseID          SYSLIB-ECOM-CT-0762
@SYMTestCaseDesc	    Tests for proper listing of implementation of an interface UID
@SYMTestPriority 	    High
@SYMTestActions  	    Call to REComSession::FinalClose is omitted to show memory leak panic
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
TInt RFinalCloseTest::ListImpl_Without_FinalClose(TAny*)
	{
	
	__UHEAP_MARK;

	CTrapCleanup* threadcleanup = CTrapCleanup::New();
	TRAPD(err,ListImplementation_TestL());
	test(err==KErrNone);

	//This FinalClose() is purposely omitted for testing the PANIC
	//REComSession::FinalClose();

	delete threadcleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
/**
The test code for testing the implementation creation and deletion
where we create two different implementation using two different plugins
destroy them and call FinalClose() to show there is no problem and memory leak

@SYMTestCaseID          SYSLIB-ECOM-CT-0763
@SYMTestCaseDesc	    Tests for proper creation and deletion of implementation using two different plugins
@SYMTestPriority 	    High
@SYMTestActions  	    Create the implementation with a call to REComSession::CreateImplementationL twice with different UIDs
                        Signal the destruction of an interface implementation to ECOM with a call to REComSession::DestroyedImplementation
						Call up REComSession::FinalClose
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RFinalCloseTest::Create_Destroy2_With_FinalCloseL()
	{
	__UHEAP_MARK;

	TUid implUid1={0x10009DC3};
	TUid implUid2={0x101F847A};
	TUid dTorKey1;
	TUid dTorKey2;
	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	//Creating implementation 1
	CExampleInterface* impl1 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid1,dTorKey1,&initParams));
	test(impl1->ImplId()==implUid1);

	//Creating implementation 2
	CExampleInterface* impl2 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid2,dTorKey2,&initParams));
	test(impl2->ImplId()==implUid2);

	//Destroy implementation 2
	REComSession::DestroyedImplementation(dTorKey2);

	delete impl2;
	impl2=0;

	//Now Destroy implementation 1
	REComSession::DestroyedImplementation(dTorKey1);

	delete impl1;
	impl1=0;

	REComSession::FinalClose();

	__UHEAP_MARKEND;

	}

/**
The test code for testing that REComSession::FinalClose() can be called many times
without introducing any strange behaviour, memory leak and crash problem.

@SYMTestCaseID          SYSLIB-ECOM-CT-0764
@SYMTestCaseDesc	    Tests to show that FinalClose() can be called many times with no unknown consequence.
@SYMTestPriority 	    High
@SYMTestActions  	    Create the implementation with some empty initialisation parameters
						Call up REComSession::FinalClose twice before destroying the implementation and once after the deletion
                        Check for any strange behaviour, memory leak and crash problem.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RFinalCloseTest::Create_Destroy3_With_FinalCloseL()
	{
	__UHEAP_MARK;
	TUid implUid1={0x10009DC3};
	TUid dTorKey1;
	// Set up some empty initialisation parameters
	CExampleInterface::TExampleInterfaceInitParams initParams;
	initParams.integer		= 0;
	initParams.descriptor	= NULL;

	//Creating implementation 1
	CExampleInterface* impl1 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid1,dTorKey1,&initParams));
	test(impl1->ImplId()==implUid1);

	//Called FinalClose() twice
	REComSession::FinalClose();
	REComSession::FinalClose();

	REComSession::DestroyedImplementation(dTorKey1);
	delete impl1;

	REComSession::FinalClose();

	__UHEAP_MARKEND;
	}

/**
The test code for testing the functionality of REComSession::FinalClose()
without introducing any strange behaviour,memory leak and crash problem

@SYMTestCaseID          SYSLIB-ECOM-CT-0765
@SYMTestCaseDesc	    Tests for the REComSession::FinalClose() function
@SYMTestPriority 	    High
@SYMTestActions  	    Open and close single connection to the the ECom server.
                        Call FinalClose, to check for any strange behaviour,memory leak and crash problem
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RFinalCloseTest::Create_Destroy4_With_FinalCloseL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0765 "));
	__UHEAP_MARK;

	REComSession& ecomsession = ecomsession.OpenL();

	ecomsession.Close();

	REComSession::FinalClose();

	__UHEAP_MARKEND;
	}
/**
The test code for testing the functionality of REComSession::FinalClose()
without introducing any strange behaviour,memory leak and crash problem

@SYMTestCaseID          SYSLIB-ECOM-CT-0766
@SYMTestCaseDesc	    Tests for the REComSession::FinalClose() function
@SYMTestPriority 	    High
@SYMTestActions  	    Call FinalClose() function,to check for any memory leak and crash problem
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RFinalCloseTest::FinalClose()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0766 "));
	__UHEAP_MARK;

	REComSession::FinalClose();

	__UHEAP_MARKEND;
	}

LOCAL_C void NonConstructL_Create_Destroy_helper()
	{
	REComSession session;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0767
@SYMTestCaseDesc	    Tests for the REComSession::FinalClose() function
@SYMTestPriority 	    High
@SYMTestActions  	    Open and close connection to ECOM server
                        Check for any memory leak and crash problem
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RFinalCloseTest::NonConstructL_Create_Destroy()
	{
	__UHEAP_MARK;

	// Test 1
	::NonConstructL_Create_Destroy_helper();

	// Test 2
	REComSession* ptr = new REComSession;
	delete ptr;

	__UHEAP_MARKEND;
	}

/**
The test code is used for capturing the PANIC that occurs as a result of not
calling REComSession::FinalClose() when using ECOM plugins.

@SYMTestCaseID          SYSLIB-ECOM-CT-0768
@SYMTestCaseDesc	    Tests for PANIC when REComSession::FinalClose() is called using ECOM plugins
@SYMTestPriority 	    High
@SYMTestActions  	    Check for memory leak panic
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void RFinalCloseTest::ThreadPanicTest(const TDesC& aName,TThreadFunction aFunction)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0768 "));
	test.Next(aName);
	TRequestStatus threadStatus;
	RThread thread;
	TBool jit;
	jit=User::JustInTime();
	User::SetJustInTime(EFalse);

	TInt err=thread.Create(aName,aFunction,KDefaultStackSize*8,KMinHeapSize,0x100000,0);
	test(err==KErrNone);
	thread.Logon(threadStatus)	;
	thread.Resume();

	User::WaitForRequest(threadStatus);

	//Now check why the thread Exit

	test(thread.ExitType()==EExitPanic);
	test(thread.ExitReason()==0);
	//A memory leak will have the exit category of form ALLOC:xxxxxx
	//test.Printf(_L("Exit xcat: %S"), thread.ExitCategory().Left(6));
	test(thread.ExitCategory().Left(6).CompareF(_L("ALLOC:"))==0);
	thread.Close();
	User::SetJustInTime(jit);
	}

/**
Copies the Plugins to specific folder for testing purpose
*/
LOCAL_C void CopyPlugins()
    {
	// Copy the dlls and .rsc files on to RAM
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExDllOnZ, KEComExDllOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExampleDllOnZ, KEComExampleDllOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample2DllOnZ, KEComExample2DllOnC));
	test(err==KErrNone);
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample3DllOnZ, KEComExample3DllOnC));
	test(err==KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExampleRscOnZ, KEComExampleRscOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample2RscOnZ, KEComExample2RscOnC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample3RscOnZ, KEComExample3RscOnC));
	test(err==KErrNone);
	}

// Deleting plugin from the RAM for cleanup purpose
inline LOCAL_C void DeleteTestPlugin()
	{
	TRAPD(err, EComTestUtils::FileManDeleteFileL(KEComExDllOnC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRscFileOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExampleDllOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample2DllOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample3DllOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExampleRscOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample2RscOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExample3RscOnC));
	}

LOCAL_C void RunTestL()
	{
	__UHEAP_MARK;
	//Basic ECOM test
	test.Next(_L("FinalClose"));
	RFinalCloseTest::FinalClose();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0759 Create_Destroy_With_FinalCloseL "));
	RFinalCloseTest::Create_Destroy_With_FinalCloseL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0761 ListImpl_With_FinalCloseL "));
	RFinalCloseTest::ListImpl_With_FinalCloseL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0763 Create_Destroy2_With_FinalCloseL "));
	RFinalCloseTest::Create_Destroy2_With_FinalCloseL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0764 Create_Destroy3_With_FinalCloseL "));
	RFinalCloseTest::Create_Destroy3_With_FinalCloseL();
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0767 NonConstructL_Create_Destroy "));
	RFinalCloseTest::NonConstructL_Create_Destroy();

	//ECOM FinalClose() Panic Test
	#if defined(_DEBUG)
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0760 "));
	RFinalCloseTest::ThreadPanicTest(_L("Create Destroy Panic Testing"),RFinalCloseTest::Create_Destroy_Without_FinalClose);
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0762 "));
	RFinalCloseTest::ThreadPanicTest(_L("List Implementation Panic Testing"),RFinalCloseTest::ListImpl_Without_FinalClose);
	#endif

	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Title();
	test.Start(_L("Final Close tests."));

	CopyPlugins();

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,RunTestL());
	test(err==KErrNone);

	// Cleanup files. If the cleanup fails that is no problem,
	// as any subsequent tests will replace them. The only downside
	// would be the disk not being tidied
	DeleteTestPlugin();

	delete scheduler;
	delete cleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(0);
	}
