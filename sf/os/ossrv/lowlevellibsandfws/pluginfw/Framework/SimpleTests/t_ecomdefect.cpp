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
#include "LoadManager.h"
#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "Interface.h" // interface to Plugins
//Test utils for copying the resolver to C
#include "../EcomTestUtils/EcomTestUtils.h"

LOCAL_D RTest TEST(_L("Ecom Defect Test"));

_LIT(KEComExDllOnZ,		"Z:\\RAMOnly\\T_PlatSecResolverC.dll");

_LIT(KEComExDllOnC,		"C:\\sys\\bin\\T_PlatSecResolverC.dll");
_LIT(KEComRscFileOnC,	"C:\\resource\\plugins\\T_PlatSecResolverC.rsc");
_LIT(KEComRscFileOnZ,	"Z:\\RAMOnly\\T_PlatSecResolverC.rsc");

#define UNUSED_VAR(a) a = a
inline LOCAL_C void DeleteTestPlugin()
	{
	TRAPD(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExDllOnC));
	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComRscFileOnC));
	}

class REcomDefectTest
	{
public:
	static void DEF049285_TestCaseL();
	static void DEF049979_TestCaseL();
	static void INC057514_TestCaseL();
	static void DEF065025_TestCase();
	};

/**
Test case for Defect DEF048053 LoadManager Leaks Memory even when FinalClose is called.

@SYMTestCaseID          SYSLIB-ECOM-CT-0770
@SYMTestCaseDesc	    Test case for defect number DEF048053 LoadManager Leaks Memory even when FinalClose is called
@SYMTestPriority 	    High
@SYMTestActions  	    Create two simple implementation with different UID and check for memory leak when FinalClose is called.
                        Create two complex implementations in Two different DLL check for memory leak when FinalClose is called.
                        Create two simple implementation with same UID and check for memory leak when FinalClose is called.
						Create two complex implementations in different DLL check for memory leak when FinalClose is called.
						Test for invalid implementationUid to ensure no leak and proper cleanup
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void REcomDefectTest::DEF049285_TestCaseL()
	{
	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0770 "));
	_LIT(KDummyText,"Dummy params");

	TInt err=KErrNone;
	TInt failAt = 1;
	//TO clear warnings in urel armv5 as failAt++ is only used in __UHEAP_SETFAIL in udeb
	failAt+=0;
	//Dummy instantiation parameters
	CExampleInterface::TExampleInterfaceInitParams iInitParams;
	iInitParams.integer		= 5;
	iInitParams.descriptor	= &KDummyText;

	/**
	-------------Part 1: Two Simple Implementations in Two different DLL----------------
	Plugins used: T_PlatSecEcom1.dll with implUid1=0x102026AA
				  T_PlatSecEcom2.dll with implUid2=0x102026AC
	*/
	TEST.Next(_L("DEF048053 Part 1\n"));
	__UHEAP_MARK;
	TUid implUid1={0x102026AA};
	TUid implUid2={0x102026AC};
	TUid returnedUid1;
	TUid returnedUid2;

	//Create the first implementation
	TAny* imp1=REComSession::CreateImplementationL(implUid1,returnedUid1);
	CInstanceInfoSimple* instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (returnedUid1.iUid);
	TEST(implUid1==instanceInfo->ImplementationUid(), __LINE__);
	//Now start the OOM test when creating the second implementation
	__UHEAP_MARK;
	do
		{
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		TAny* imp2=NULL;
		//Create the second implementation
		TRAP(err,imp2=REComSession::CreateImplementationL(implUid2,returnedUid2));
		if (err==KErrNone)
			{
			instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (returnedUid2.iUid);
			TEST(implUid2==instanceInfo->ImplementationUid(), __LINE__);
			REComSession::DestroyedImplementation(returnedUid2);
			delete imp2;
			imp2=NULL;
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);
  		}
	while (err == KErrNoMemory);
	//CALL FinalClose() HERE, do not want to leak memory
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	REComSession::DestroyedImplementation(returnedUid1);
	delete imp1;
	imp1=NULL;
	//call FinalClose() here, do not want to leak memory
	REComSession::FinalClose();
	__UHEAP_MARKEND;


	/**
	---------------Part 2: Two Complex Implementations in Two different DLL-----------------------
	Plugins used: EComExample2.dll with implUid1=0x10009DC4
				  EComExample3.dll with implUid2=0x101F8478
	*/
	TEST.Next(_L("DEF048053 Part 2\n"));
	__UHEAP_MARK;
	implUid1=TUid::Uid(0x10009DC4);
	implUid2=TUid::Uid(0x101F8478);
	failAt=1;
	//Set up initialisation parameters
	//This initialisation parameters are required for testing the
	//failure point in a more complex plugin where it is possible
	//to fail in the ConstructL stage of the plugin NewL
	//Create the first plugin
	CExampleInterface* impl1 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid1,
															   returnedUid1,
															   &iInitParams
															   ));
	//Now start the OOM test when creating the second implementation
	do
		{
		__UHEAP_MARK;
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		CExampleInterface* impl2=NULL;
		//Create the second implementation
		TRAP(err,impl2 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid2,
												   			returnedUid2,
															&iInitParams
														   )));
		if (err==KErrNone)
			{
			instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (returnedUid2.iUid);
			TEST(implUid2==instanceInfo->ImplementationUid(), __LINE__);
			REComSession::DestroyedImplementation(returnedUid2);
			delete impl2;
			impl2=NULL;
			}
		//CALL FinalClose() HERE!, do not want to leak memory
		REComSession::FinalClose();
		__UHEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone, 0);
  		}
	while (err == KErrNoMemory);

	REComSession::DestroyedImplementation(returnedUid1);
	delete impl1;
	impl1=NULL;
	//call FinalClose() here, do not want to leak memory
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	/*
	-----------Part 3, Two Simple Implementations in the Same DLL-------
	Plugins used: EComExample2.dll with implUid1=0x10009DC3
				  				   with implUid2=0x10009DC4
	*/
	TEST.Next(_L("DEF048053 Part 3\n"));
	__UHEAP_MARK;
	implUid1=TUid::Uid(0x10009DC3);
	implUid2=TUid::Uid(0x10009DC4);
	failAt=1;
	//Set up initialisation parameters
	//This initialisation parameters are required for testing the
	//failure point in a more complex plugin where it is possible
	//to fail in the ConstructL stage of the plugin NewL
	//Create the first plugin
	impl1 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid1,
															   returnedUid1,
															   &iInitParams
															   ));
	//Now start the OOM test when creating the second implementation
	do
		{
		__UHEAP_MARK;
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		CExampleInterface* impl2=NULL;
		//Create the second implementation
		TRAP(err,impl2 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid2,
												   			returnedUid2,
												   			&iInitParams
															)));
		if (err==KErrNone)
			{
			instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (returnedUid2.iUid);
			TEST(implUid2==instanceInfo->ImplementationUid(), __LINE__);
			REComSession::DestroyedImplementation(returnedUid2);
			delete impl2;
			impl2=NULL;
			}
		//CALL FinalClose() HERE!, do not want to leak memory
		REComSession::FinalClose();
		__UHEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone, 0);
  		}
	while (err == KErrNoMemory);

	REComSession::DestroyedImplementation(returnedUid1);
	delete impl1;
	impl1=NULL;
	//call FinalClose() here, do not want to leak memory
	REComSession::FinalClose();
	__UHEAP_MARKEND;


	/*
	------------Part 4. Two complex implementations in different DLL--------
	Plugins used: EComExample2.dll 				with implUid1=0x10009DC4
		  		  EcomRomRslvrExampleOnZ.dll	with implUid2=0x10009DC7
	//Special case
	//Implementation with uid 10009DC7 is registered as the implementation in
	//plugin EComRomRslvrExampleOnZ.DLL however there is no mapping in the
	//implementaton proxy table that matches this implementation to its NewL
	//with KErrNotFound(-1)
	*/
	TEST.Next(_L("DEF048053 Part 4\n"));
	__UHEAP_MARK;
	implUid1=TUid::Uid(0x10009DC4);
	implUid2=TUid::Uid(0x10009DC7);
	failAt=1;
	//Set up initialisation parameters
	//This initialisation parameters are required for testing the
	//failure point in a more complex plugin where it is possible
	//to fail in the ConstructL stage of the plugin NewL
	//Create the first plugin
	impl1 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid1,
															   returnedUid1
															   ));
	//Now start the OOM test when creating the second implementation
	do
		{
		__UHEAP_MARK;
		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);
		CExampleInterface* impl2=NULL;
		//Create the second implementation
		TRAP(err,impl2 = REINTERPRET_CAST(CExampleInterface*,
							REComSession::CreateImplementationL(implUid2,
												   			returnedUid2,
															&iInitParams
														   )));
		if (err==KErrNone)
			{
			instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (returnedUid2.iUid);
			TEST(implUid2==instanceInfo->ImplementationUid(), __LINE__);
			REComSession::DestroyedImplementation(returnedUid2);
			delete impl2;
			impl2=NULL;
			}
		//CALL FinalClose() HERE!, do not want to leak memory
		REComSession::FinalClose();
		__UHEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone, 0);
  		}
	while (err == KErrNoMemory);

	REComSession::DestroyedImplementation(returnedUid1);
	delete impl1;
	impl1=NULL;
	//call FinalClose() here, do not want to leak memory
	REComSession::FinalClose();
	__UHEAP_MARKEND;

	/*
	------------Part 5. Invalid argument testing in CreateImplementation
	Test for invalid implementationUid to ensure no leak and proper cleanup
	*/
	__UHEAP_MARK;

	TUid invalidImplUid={0x1111111};
	TUid returnedUid;
	TAny* invalidimpl=NULL;
	TRAP(err,invalidimpl=REComSession::CreateImplementationL(invalidImplUid,returnedUid));
	TEST(err==KErrNotFound, __LINE__);
	TEST(invalidimpl==NULL, __LINE__);
	TEST(returnedUid==KNullUid, __LINE__);
	REComSession::FinalClose();

	__UHEAP_MARKEND;

	}

/**
Test case for Defect ECom Server only loads Resolvers from Z: Drive

@SYMTestCaseID          SYSLIB-ECOM-CT-0769
@SYMTestCaseDesc	    Test case for defect number DEF049979 LoadManager Leaks Memory even when FinalClose is called
@SYMTestPriority 	    High
@SYMTestActions  	    List all the implemetations once using a UID which reside on C: drive and another on Z: drive
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void REcomDefectTest::DEF049979_TestCaseL()
	{
	RImplInfoPtrArray ifArray;
	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0769 DEF049979_TestCaseL\n "));

	_LIT8(KImplementationTest,"text/wml");
	// Set up the interface find for the default resolver.
	TEComResolverParams ResolverParams;
	ResolverParams.SetDataType(KImplementationTest());
	ResolverParams.SetGenericMatch(ETrue);	// Allow wildcard matching
	TUid ifUid = {0x10009DC0};

	/*
	-----Test case 1 ListImplementation using a C resolver----------
	*/
	__UHEAP_MARK;

	//A resolver uid that only resides in C T_PlatSecResolverC.dll
	TUid resolverUidC={0x10244444};


	REComSession::ListImplementationsL(
			ifUid,
			ResolverParams,
			resolverUidC,
			ifArray);

	// There should be 6 implementations found but only 2 returned.
	// These 2, are the only two that match the datatype supplied.
	// These 2, are also 2 of a posible 4, i.e. version 2.
	// The version 1 implementations are not part of the reported 6
	// they are superseeded.
	// So the 2 that match are implementation uids 0x10009DC3 & 0x10009DC4
	TInt availCount = ifArray.Count();
	TEST(availCount == 2, __LINE__);

	ifArray.ResetAndDestroy();

	/*
	-----Test case 2 List Implementation using a Z resolver---------
	*/
	//A resolver uid that resides in Z T_PlatSecResolverZ.dll
	TUid resolverUidZ={0x10999999};

	REComSession::ListImplementationsL(
			ifUid,
			ResolverParams,
			resolverUidZ,
			ifArray);

	// There should be 6 implementations found but only 2 returned.
	// These 2, are the only two that match the datatype supplied.
	// These 2, are also 2 of a posible 4, i.e. version 2.
	// The version 1 implementations are not part of the reported 6
	// they are superseeded.
	// So the 2 that match are implementation uids 0x10009DC3 & 0x10009DC4
	availCount = ifArray.Count();
	TEST(availCount == 2, __LINE__);

	ifArray.ResetAndDestroy();

	REComSession::FinalClose();
	__UHEAP_MARKEND;

	}


// This class is used for INC057514_TestCaseL.
// Checks the reference count when constructing and destructing REComSessions.
//
class CStuff : public CBase
	{
public:
	static CStuff* NewL() {
		CStuff* self = new (ELeave) CStuff;
		CleanupStack::PushL (self);
		self->ConstructL();
		CleanupStack::Pop (self);
		return self;
		}
	void ConstructL ();
	~CStuff();

	REComSession iEcomSession;

private:
	CStuff() {/*do nothing*/};
	};

void CStuff::ConstructL ()
	{
	iEcomSession = REComSession::OpenL();
	}

CStuff::~CStuff()
	{
	iEcomSession.Close();
	}

/**
Test case for Defect ECOM can't (reference) count

@SYMTestCaseID				SYSLIB-ECOM-CT-01364
@SYMTestCaseDesc	    		Test case for defect number INC057514 ECOM can't (reference) count
@SYMTestPriority				High
@SYMTestActions				Create 2 implementations
							Open session with REComSession
							Close session with REComSession
							When out of scope destructor for REComSession is called.
@SYMTestExpectedResults		The test must not fail.
@SYMDEF					INC057514
*/
void REcomDefectTest::INC057514_TestCaseL()
	{
	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1364 INC057514_TestCaseL "));

	// Set up for heap leak checking
	__UHEAP_MARK;

	//Check Thread handles leak
	TInt startProcessHandleCount = 0;
	TInt startThreadHandleCount = 0;
	TInt endProcessHandleCount = 0;
	TInt endThreadHandleCount = 0;

	RThread rThread;
	rThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// START TEST //


	__UHEAP_MARK;

	CStuff* stuff1 = CStuff::NewL();
	CleanupStack::PushL(stuff1);

	TUid implUid1={0x102026AA};
	TUid returnedUid1;
	TUid returnedUid2;

	//Create the first implementation
	TAny* imp1=stuff1->iEcomSession.CreateImplementationL(implUid1,returnedUid1);
	CInstanceInfoSimple* instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (returnedUid1.iUid);
	CleanupStack::PushL(imp1);
	TEST(implUid1==instanceInfo->ImplementationUid(), __LINE__);

	CStuff* stuff2 = CStuff::NewL();
	CleanupStack::PushL(stuff2);

	//Create the first implementation
	TAny* imp2=stuff2->iEcomSession.CreateImplementationL(implUid1,returnedUid2);
	instanceInfo = reinterpret_cast <CInstanceInfoSimple*> (returnedUid2.iUid);
	CleanupStack::PushL(imp2);
	TEST(implUid1==instanceInfo->ImplementationUid(), __LINE__);

	{
		REComSession session = stuff1->iEcomSession.OpenL();
		session.Close();
		// When we go out of scope we cause the destructor
		// to be called for REComSession.
	}

	REComSession::DestroyedImplementation(returnedUid1);

	CleanupStack::PopAndDestroy(imp2);
	CleanupStack::PopAndDestroy(stuff2);

	REComSession::FinalClose();

	REComSession::DestroyedImplementation(returnedUid2);

	CleanupStack::PopAndDestroy(imp1);
	CleanupStack::PopAndDestroy(stuff1);

	REComSession::FinalClose();


	__UHEAP_MARKEND;



	// END TEST //

	// Check for open handles
	rThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
	TEST(startThreadHandleCount == endThreadHandleCount, __LINE__);

	//Test ends
	__UHEAP_MARKEND;
	}

static RSemaphore TheLoadEcomServerSemaphore;

static TInt LoadEcomServer(void*)
	{
	RThread currThread;
	const TName& threadName = currThread.Name();
	RDebug::Print(_L("Thread %S running\n"), &threadName);

	//Wait until get a notification from the creating thread that the ECOM server can be loaded.
	TheLoadEcomServerSemaphore.Wait();

	CTrapCleanup* cleanup = CTrapCleanup::New();
	TEST(cleanup != NULL);

	//Create ECOM session. This call will try to load the ECOM server.
	REComSession ecomSession;
	TRAPD(err, ecomSession.OpenL());
	TEST(err==KErrNone);

	//Wait some time. During that time the ECOM server will try to process the ECOM registry.
	User::After(3000000);
	ecomSession.Close();

	delete cleanup;
	RDebug::Print(_L("Thread %S exits\n"), &threadName);
	return KErrNone;
	}

/**
Test case for Defect Multi-threaded client start-up of ECOM server can causeKErrInUse errors

@SYMTestCaseID				SYSLIB-ECOM-CT-01365
@SYMTestCaseDesc	    		Test case for defect number DEF065025  Multi-threaded client
							start-up of ECOM server can causeKErrInUse errors
@SYMTestPriority				High
@SYMTestActions				Create 16 threads and block.
							Unblock each thread causing them all to run simultaneously.
							Each thread opens a session to ECOM Server.
							Close each session to ECOM Server.
							Close each thread.
@SYMTestExpectedResults		The test must not fail.
@SYMDEF					DEF065025
*/
void REcomDefectTest::DEF065025_TestCase()
	{
	TEST.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1365 "));
	TInt err=KErrNone;
   	_LIT(KEComServerProcessName,"ecomserver");
   	TRAP(err, EComTestUtils::KillProcessL(KEComServerProcessName));
   	UNUSED_VAR(err);

	const TInt KThreadCnt = 16;
	err = TheLoadEcomServerSemaphore.CreateLocal(0);
	TEST(err==KErrNone);

	RThread loadEcomThread[KThreadCnt];
	TRequestStatus threadStatus[KThreadCnt];
	TInt i;

	//Create KThreadCnt threads. They will be blocked on TheLoadEcomServerSemaphore after
	//their creation.
	for(i=0;i<KThreadCnt;++i)
		{
		TBuf<32> threadName;
		threadName.Format(_L("Th-%02d"), i + 1);
		TInt err = loadEcomThread[i].Create(threadName, (TThreadFunction)LoadEcomServer,
		KDefaultStackSize, KMinHeapSize, 0x00100000, NULL);
		TEST(err==KErrNone);
		loadEcomThread[i].Logon(threadStatus[i]);
		loadEcomThread[i].Resume();
		}
	User::After(3000000);

	//Unblock the threads. The threads will run simultaneously and will try to load multiple
	//instances of the ECOM server, which will try to open and process Registry files at the
	//same time.
	TheLoadEcomServerSemaphore.Signal(KThreadCnt);

	//Wait until all threads die.
	for(i=0;i<KThreadCnt;++i)
		{
		User::WaitForRequest(threadStatus[i]);
		}

	//Close all threads.
	for(i=0;i<KThreadCnt;++i)
		{
		loadEcomThread[i].Close();
		}

	TheLoadEcomServerSemaphore.Close();
	//Put a break point there and kill the test
	//Check EPOCWIND.OUT file.
	}
/**
Copies the Resolver Plugins to C:\ drive
*/
LOCAL_C void CopyPluginsL()
    {
	// Copy the dlls and .rsc files on to RAM
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExDllOnZ, KEComExDllOnC));
 	TEST(err==KErrNone, __LINE__);
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileOnC));
 	TEST(err==KErrNone, __LINE__);
	// Wait, so that ECom server looks for plugins copied from Z: to C drive
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that CDiscoverer::CIdleScanningTimer
	// which is an active object. So the discovering service is asynchronous. We have to
	// wait some time until it finishes. Otherwise ListImplementationsL could fail to find
	// requested implementations.
	User::After(5000000);
	}


LOCAL_C void RunTestL()
	{
	__UHEAP_MARK;

	CopyPluginsL();

	REcomDefectTest::DEF049285_TestCaseL();

	REcomDefectTest::DEF049979_TestCaseL();

	REcomDefectTest::INC057514_TestCaseL();

	REcomDefectTest::DEF065025_TestCase();
	DeleteTestPlugin();

	__UHEAP_MARKEND;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TEST.Title();
	TEST.Start(_L("Ecom Defect tests."));

	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	TRAPD(err,RunTestL());
	TEST(err==KErrNone, __LINE__);

	delete scheduler;
	delete cleanup;

	TEST.End();
	TEST.Close();

	__UHEAP_MARKEND;
	return(0);
	}
