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
// This file contains test classes and their implementations
// to test production class CBackupNotifier. Where necessary stubs
// are implemented to help in writing test harness using RTest.
// 
//

#include <e32test.h>
#include <s32file.h>
#include <bautils.h>

#include "BackupNotifier.h"
#include "BackupNotifierObserver.h"

// Used for suppressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

LOCAL_D CTrapCleanup*		TheTrapCleanup		=NULL;

LOCAL_D CActiveScheduler*	TheActiveScheduler	=NULL;

LOCAL_D RTest				test(_L("t_backupnotifier.exe"));

//It is used by some test methods which are called two times:
//from normal test and from OOM test.
static void LeaveIfErrNoMemory(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		User::Leave(aError);
		}
	}

/**
Stub classes are provided to act as replacement members for the class
under test. This ensures the test class is constructed correctly and its
behaviour is also correct within the context of the test. It allows the
class to be tested in isolation. Other tests are available that test the
interoperability of the class
*/
class CBackupNotifierObserverStub : public CBase, public MBackupNotifierObserver
	{
public:

	CBackupNotifierObserverStub();
	~CBackupNotifierObserverStub();
	TInt Suspend();
	TInt Resume();

	/** Flag to check for BackupNotifierObserver directory scanning status*/
	TBool iSuspended;
	};

CBackupNotifierObserverStub::CBackupNotifierObserverStub()
: CBase()
	{
	iSuspended = EFalse;
	}

CBackupNotifierObserverStub::~CBackupNotifierObserverStub()
	{
	// Do nothing here
	}

/**
Overload of the MBackupNotifierObserver method.
Called to stop the directory scanning as the backup session is started.
*/
TInt CBackupNotifierObserverStub::Suspend()
	{
	iSuspended = ETrue;
	return KErrNone;
	}


/**
Overload of the MBackupNotifierObserver method.
Called to resume the directory scanning as the backup session is over.
*/
TInt CBackupNotifierObserverStub::Resume()
	{
	iSuspended = EFalse;
	return KErrNone;
	}

/**
This friend class allows us to access private and protected members of production
code class BackupNotifier.
*/
class TBackupNotifier_StateAccessor
{
public:
	void HandleBackupOperationEventL(CBackupNotifier& , const TBackupOperationAttributes&);
	void RegisterForNotificationsL(CBackupNotifier& aNotifier);
	void DeregisterForNotifications(CBackupNotifier& aNotifier);
	TBool IsRegisteredForNotifications(CBackupNotifier& aNotifier);
};

/**
Handles the Backup operation event.

@param		aNotifier The CBackupNotifier class object under test
@param		aAttributes Attribute specifying the Backup event(Start/Stop)
*/
void TBackupNotifier_StateAccessor::HandleBackupOperationEventL(CBackupNotifier& aNotifier,
											const TBackupOperationAttributes& aAttributes)
	{
	aNotifier.HandleBackupOperationEventL(aAttributes);
	}

/**
Creates the Backup session if already not created and Registers BackupNotifier for
notifications from the backup server.

@param		aNotifier The CBackupNotifier class object under test
*/
void TBackupNotifier_StateAccessor::RegisterForNotificationsL(CBackupNotifier& aNotifier)
	{
	aNotifier.RegisterForNotificationsL();
	}

/**
Deregisters the BackupNotifier for notifications from the backup server and deletes
the Backup session object.

@param		aNotifier The CBackupNotifier class object under test
*/
void TBackupNotifier_StateAccessor::DeregisterForNotifications(CBackupNotifier& aNotifier)
	{
	if(aNotifier.iBackupSession)
		{
		if(aNotifier.iIsRegistered)
			{
			aNotifier.iBackupSession->DeRegisterBackupOperationObserver(aNotifier);
			}
		delete aNotifier.iBackupSession;
		aNotifier.iBackupSession = NULL;
		}
	aNotifier.iIsRegistered = EFalse;
	}

/**
Checks whether the notifier object is registered for backup notifications.

@param		aNotifier The CBackupNotifier class object under test
*/
TBool TBackupNotifier_StateAccessor::IsRegisteredForNotifications(CBackupNotifier& aNotifier)
	{
	return aNotifier.iIsRegistered;
	}


/**
Test class encloses necessary members that aid to test CBackupNotifier
*/
class CBackUpNotifierTest: public CBase
	{
public:
	static CBackUpNotifierTest* NewL();

	~CBackUpNotifierTest();
	void HandleBackupOperationEventTestL();
	void RegisterForNotificationsTestL();

private:
	CBackUpNotifierTest();
	void ConstructL();

public:
	/** The instance of the class under test */
	CBackupNotifier*					iBackupNotifier;

	/** The instance of the stubbed observer of the class under test */
	CBackupNotifierObserverStub*		iObserverStub;

	/** Friend class pointer used for accessing private members */
	TBackupNotifier_StateAccessor*		iStateAccessor;
	};

/**
Create a new CBackUpNotifierTest object
@return			A pointer to the newly created class.
*/
CBackUpNotifierTest* CBackUpNotifierTest::NewL()
	{
	CBackUpNotifierTest* self = new (ELeave) CBackUpNotifierTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

/**
Standardized default constructor
@post		CBackUpNotifierTest is fully constructed
*/

CBackUpNotifierTest::CBackUpNotifierTest()
	{
	// Do nothing here
	}

/**
Standardized 2nd(Initialization) phase of two phase construction.
Completes the safe construction of the CBackUpNotifierTest object
@post		CBackUpNotifierTest is fully constructed.
@leave		KErrNoMemory.
*/
void CBackUpNotifierTest::ConstructL()
	{
	iStateAccessor = new(ELeave) TBackupNotifier_StateAccessor();
	iObserverStub = new(ELeave) CBackupNotifierObserverStub();
	iBackupNotifier = CBackupNotifier::NewL(*iObserverStub);
	}

/**
Standard destructor
*/
CBackUpNotifierTest::~CBackUpNotifierTest()
	{
	delete iBackupNotifier;
	delete iObserverStub;
	delete iStateAccessor;
	}

/**
The test executes by first starting the backup session and then by ending it.
Backup operation event notifications are passed to the observer(i.e CRegistrar) for
notifing CDiscoverer to start/ stop the discoveries process depending on the
event(Backup start/end). The observer stub class is being used for the
verification of the results.

@SYMTestCaseID          SYSLIB-ECOM-CT-0753
@SYMTestCaseDesc	    Tests for CBackupNotifier::HandleBackupOperationEventL function
@SYMTestPriority 	    High
@SYMTestActions  	    Setting the backup operation attributes before starting the backup operation.
                        Suspend BackupNotifier observer.
						Reset backup operation attributes and stop the backup operation.
						Resume BackupNotifier observer.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CBackUpNotifierTest::HandleBackupOperationEventTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0753 "));
	TBackupOperationAttributes attribs;
	// Setting the backup operation attributes before making the event.
	attribs.iFileFlag=MBackupObserver::EReleaseLockNoAccess;
	attribs.iOperation=MBackupOperationObserver::EStart;

	// Start the backup operation
	TRAPD(err, iStateAccessor->HandleBackupOperationEventL(*iBackupNotifier, attribs));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);
	// BackupNotifier observer should get suspended. The directory scanning process(in stub class)
	// should get stopped as backup is started.
	test(iObserverStub->iSuspended);

	// Resetting the backup operation attributes before making another event.
	attribs.iFileFlag=MBackupObserver::EReleaseLockNoAccess;
	attribs.iOperation=MBackupOperationObserver::EEnd;

	// Stop the backup operation
	TRAP(err, iStateAccessor->HandleBackupOperationEventL(*iBackupNotifier, attribs));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);
	// BackupNotifier observer should get resumed. The directory scanning process(in stub class)
	// should get resumed as backup is over.
	test(!iObserverStub->iSuspended);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0754
@SYMTestCaseDesc	    Register for notification test
@SYMTestPriority 	    High
@SYMTestActions  	    Tests by first unregistring(if already registerd) the Backup notifier for notifications
						and then by registering for notifications from the backup server.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CBackUpNotifierTest::RegisterForNotificationsTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0754 "));
	TBool isRegistered = iStateAccessor->IsRegisteredForNotifications(*iBackupNotifier);

	// Idealy it should got registered when the CBackupNotifier object is created.
	if(isRegistered)
		{
		TRAPD(err, iStateAccessor->DeregisterForNotifications(*iBackupNotifier));
		::LeaveIfErrNoMemory(err);
		test(err == KErrNone);
		}

	// Backup notifier should not be registerd for backup operation events.
	isRegistered = iStateAccessor->IsRegisteredForNotifications(*iBackupNotifier);
	test(!isRegistered);

	// Register the Backup Notifier for the backup operation events.
	TRAPD(err, iStateAccessor->RegisterForNotificationsL(*iBackupNotifier));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	// Backup notifier should be registerd for backup operation events.
	isRegistered = iStateAccessor->IsRegisteredForNotifications(*iBackupNotifier);
	test(isRegistered);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0755
@SYMTestCaseDesc	    Create and delete test of CBackUpNotifier
@SYMTestPriority 	    High
@SYMTestActions  	    Check for handle leak after deletion of object.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void CreateDeleteTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0755 CreateDeleteTestL "));
	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...
	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	CBackUpNotifierTest* notifierTest = CBackUpNotifierTest::NewL();

	test(notifierTest!=NULL);

	delete notifierTest;

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
	test(startThreadHandleCount == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0756
@SYMTestCaseDesc	    OOM Test for create and delete of CBackUpNotifier
@SYMTestPriority 	    High
@SYMTestActions  	    Check for handle leak after deletion of object.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void OOMCreateDeleteTest()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0756 OOM CreateDeleteTest "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CBackUpNotifierTest* theTest = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);

		TRAP(err, theTest = CBackUpNotifierTest::NewL());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete theTest;
		theTest = NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		}
	while(err == KErrNoMemory);

	test.Printf(_L("- Succeeded at heap failure rate of %i\n"), failAt);
	test(err == KErrNone);
	}

// Type definition for pointer to member function.
// Used in calling the CBackUpNotifierTest member function for testing.
typedef void (CBackUpNotifierTest::*ClassFuncPtrL) (void);

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0757
@SYMTestCaseDesc	    Function to call all test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up test functions of CBackupNotifier,check that no handles have leaked
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all test functions
@param		testFunc pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0757 "));
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CBackUpNotifierTest* theTest = CBackUpNotifierTest::NewL();
	CleanupStack::PushL(theTest);

	(theTest->*testFuncL)();

	CleanupStack::PopAndDestroy(theTest);

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0758
@SYMTestCaseDesc	    Function to call all OOM test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up all OOM test function related to CBackupNotifier
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0758 "));
	test.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		CBackUpNotifierTest* theTest = CBackUpNotifierTest::NewL();
		CleanupStack::PushL(theTest);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (theTest->*testFuncL)());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		CleanupStack::PopAndDestroy(theTest);
		theTest = NULL;
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

	CreateDeleteTestL();
	DoBasicTestL(&CBackUpNotifierTest::HandleBackupOperationEventTestL, _L("HandleBackupOperationEventL Test"));
	DoBasicTestL(&CBackUpNotifierTest::RegisterForNotificationsTestL,   _L("RegisterForNotificationsL Test"));

	OOMCreateDeleteTest();
	DoOOMTestL(&CBackUpNotifierTest::HandleBackupOperationEventTestL, _L("HandleBackupOperationEventL Test"));
	DoOOMTestL(&CBackUpNotifierTest::RegisterForNotificationsTestL,   _L("RegisterForNotificationsL Test"));

	__UHEAP_MARKEND;
	}

//Initialise the Active Scheduler
LOCAL_C void SetupL()
    {
	// Construct and install the active scheduler
	TheActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("BackUp Notifier Tests"));

	TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, SetupL());
	test(err == KErrNone);

	TRAP(err, DoTestsL());
	test(err == KErrNone);

	delete TheTrapCleanup;
	delete TheActiveScheduler;
	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return(KErrNone);
	}
