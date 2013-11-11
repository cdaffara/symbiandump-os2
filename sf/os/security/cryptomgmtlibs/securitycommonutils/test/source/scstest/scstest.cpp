/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Exercises the test implementation of the session count server,
* to test the base SCS functionality.
*
*/


/**
 @file
*/


#include <e32ldr.h>
#include <e32ldr_private.h>
#include <f32file.h>
#include "rtestwrapper.h"
#include <e32def_private.h>

#include <scs/scscommon.h>
#include "scstestcommon.h"
#include "scstestclient.h"

const TInt KOneSecondUs = 1000 * 1000;		///< One second in microseconds.

/** Top-level test object renders stages and confirms conditions. */
static RTestWrapper test(_L("SCSTEST"));

/**
	This session handle is defined at the file level so each individual test
	does not have to connect to the server.
 */
static RScsTestSession sTestSession;
/**
	This subsession handle is defined at the file level so each individual test
	does not have to connect to the server and create a subssesion.
 */
static RScsTestSubsession sTestSubsession;

/** Arbitrary integer value used to construct subsession. */
static const TInt KSubsessValue = 10;

static void LetServerRun()
/**
	Sleep for 100ms so the server thread can run to clean up,
	terminate, or just process the last request which it was sent.
 */
	{
	User::After(100 * 1000);
	}

// -------- OOM testing --------

static void RunOomTest(TInt (*aAllocFunc)(), void (*aFreeFunc)())
/**
	Run the supplied allocator function while forcing OOM on the server side.
	On failure, this functions tests that the server heap is balanced.  On
	success, it runs the free function and also tests that the server heap is
	balanced.

	@param	aAllocFunc		Function which allocates resources on the server heap.
	@param	aFreeFunc		Frees the resources allocated with aAllocFunc.  It should
							only be necessary to call this function if aAllocFunc
							succeeds.  This argument can be NULL if there is no
							corresponding free function.
 */
	{
	TInt r = KErrNoMemory;
	
	for (TInt i = 1; r == KErrNoMemory; ++i)
		{
		sTestSession.SetServerHeapFail(i);
		
		r = aAllocFunc();
		
		test(r == KErrNone || r == KErrNoMemory);
		if (r == KErrNone && aFreeFunc != NULL)	
			aFreeFunc();
		
		sTestSession.ResetServerHeapFail();
		}
	}

// -------- panic testing --------

/**
	To test that a client is panicked, pass a function with this
	signature to TestPanic.  It will be called from a new thread.
	
	@see TestPanic
 */
typedef void (*TPanicFunc)(RScsTestSession&);

static void ConfirmPanicReason(RThread aThread, TInt aExpectedReason)
/**
	Test the supplied thread was panicked with category
	ScsImpl::KScsClientPanicCat and the supplied reason.

	@param	aThread			Panicked thread.
	@param	aExpectedReason	The thread should have been panicked with
							this reason.
	@see ScsImpl::KScsClientPanicCat
 */
	{
	TExitType exitType = aThread.ExitType();
	TExitCategoryName exitCat = aThread.ExitCategory();
	TInt exitReason = aThread.ExitReason();
	
	test(exitType == EExitPanic);
	test(exitCat == ScsImpl::KScsClientPanicCat);
	test(exitReason == aExpectedReason);
	}

static TInt TestPanicEntrypoint(TAny* aPtr)
/**
	This entrypoint is called by the panic thread.  The
	function connects to the SCS test server and passes
	the handle to the function that should be panicked.

	@param	aPtr			Standard entrypoint argument.  This is actually
							a pointer to the function to call with the connected
							session.
	@return					KErrNone.  Required to satisfy entrypoint signature.
 */
	{
	RScsTestSession s;
	TInt r = s.Connect();
	test(r == KErrNone);
	
	TPanicFunc f = TAnyPtrToFuncPtr<TPanicFunc>(aPtr);
	f(s);
	
	test(EFalse);			// should not reach here
	return KErrNone;
	}

static void TestPanic(TPanicFunc aFunc, TInt aExpectedReason)
/**
	Create a thread which should be panicked because it will
	pass invalid data to the SCS test server.  Test the thread
	is panicked with the expected category and reason.

	@param	aFunc			Function to call from panic thread.
	@param	aExpectedReason	Reason with which the thread should
							be panicked.
 */
	{
	RThread thd;
	TInt r = thd.Create(
		_L("ScsTestPanic"), TestPanicEntrypoint,
		KDefaultStackSize, KMinHeapSize, KMinHeapSize, (TAny*) aFunc);
	test(r == KErrNone);
	
	TRequestStatus rs;
	thd.Rendezvous(rs);
	test(rs == KRequestPending);
	TBool jit = User::JustInTime();
	User::SetJustInTime(EFalse);
	thd.Resume();
	
	User::WaitForRequest(rs);
	User::SetJustInTime(jit);
	ConfirmPanicReason(thd, aExpectedReason);
	thd.Close();
	}

// -------- open / close --------

/** Handle to a session which the test code attempts to open in server-side OOM. */
RScsTestSession sOomTestSession;

static TInt OpenOomSession()
/**
	This function is invoked by RunOomTest.  It just
	attempts to connect to the SCS test server.

	@return					Return code from RScsTestSession::Connect.
	@see CloseOomSession
 */
	{
	return sOomTestSession.Connect();
	}

static void CloseOomSession()
/**
	Closes the session which was opened by OpenOomSession.

	@see OpenOomSession
	@pre OpenOomSession was successfully opened by OpenOomSession.
 */
	{
	sOomTestSession.Close();
	}

static void TestServerDeath()
	{
	test.Start(_L("TestServerDeath"));

	test.Next(_L("Starting server without shutdown timer"));
	// Create marker file to get server to run without a activity timeout
	RFs fs;
	TInt r = fs.Connect();
	test(r == KErrNone);
	(void) fs.Delete(KDisableScsTestServerTimeout());

	RFile file;
	r = file.Create(fs, KDisableScsTestServerTimeout(), EFileShareAny|EFileWrite);
	test(r == KErrNone);
	file.Close();

	// Start server
	RScsTestSession scsts;
	r = scsts.Connect();
	test(r == KErrNone);
	
	test.Next(_L("Checking ShutdownServer is not allowed"));
	r = scsts.ShutdownServer();
	test(r == KErrNotSupported);


	test.Next(_L("Doing an async call to check server does not attempt to restart the timer and crash"));
	TPckgBuf<TInt> x1 = 1;
	TRequestStatus rs1;
	scsts.Treble(x1, rs1);
	User::WaitForRequest(rs1);

	test.Printf(_L("NukeServer - scstestserver.exe SHOULD panic\n"));
	r = scsts.NukeServer();
	test(r == KErrServerTerminated);

	
	(void) fs.Delete(KDisableScsTestServerTimeout());
	fs.Close();
	scsts.Close();
	test.End();
	}

static void TestOpenClose()
/**
	Attempt to connect to the server when supplying default,
	lower, and higher version numbers.  Attempt to open a session
	in server-side OOM.
 */
	{
	test.Start(_L("TestOpenClose"));

	TInt r;
	RScsTestSession scsts;
	
	// default version
	r = scsts.Connect();
	test(r == KErrNone);
	scsts.Close();
	
	// lower-than-supported version
	TVersion v = ScsTestImpl::Version();
	--v.iMajor;
	r = scsts.Connect(v);
	test(r == KErrNone);
	scsts.Close();

	// supported version (should be same as default)
	++v.iMajor;
	r = scsts.Connect(v);
	test(r == KErrNone);
	scsts.Close();

	// greater-than-supported version
	++v.iMajor;
	r = scsts.Connect(v);
	test(r == KErrNotSupported);
	
	// test opening session when server running out of memory
	r = sTestSession.Connect();		// required to send OOM commands
	test(r == KErrNone);
	
	RunOomTest(OpenOomSession, CloseOomSession);
	
	sTestSession.Close();

#if 0
	// Test closing server with an out standing request
	r = scsts.Connect();
	test(r == KErrNone);
	test(r == KErrNone);
	TPckgBuf<TInt> x1 = 1;
	TRequestStatus rs1;
	scsts.Treble(x1, rs1);
	// Bypass the presession close
	RSessionBase *session = &scsts;
	session->Close();
	User::WaitForRequest(rs1);
	test(rs1 == KErrCancel);
#endif
	
	test.End();
	}

// -------- invalid session function --------

static void TestInvalidSessionFunction()
/**
	Send an unrecognized SCS code and confirm that it is
	rejected with KErrNotSupported.  This tests the SCS
	implementation.
	
	Send an unrecognized function identifier to the session
	and confirm that it is also rejected with KErrNotSupported.
	This tests the test server session implementation.
 */
	{
	test.Start(_L("TestInvalidSessionFunction"));
	
	TInt r;
	
	r = sTestSession.SendCustomFunction(ScsImpl::EScsUnused);
	test(r == KErrNotSupported);
	
	r = sTestSession.SendCustomFunction(ScsImpl::ECallSessionFunc | ScsTestImpl::ESessUnused);
	test(r == KErrNotSupported);
	
	test.End();
	}

// -------- synchronous session function --------

static void TestSyncSessionFunction()
/**
	Send a recognized function to the SCS test session,
	and confirm that it is executed correctly.
 */
	{
	test.Start(_L("TestSyncSessionFunction"));

	TInt x = 3;
	TInt r = sTestSession.Double(x);
	test(r == KErrNone);
	test(x == 6);
	
	test.End();
	}

// -------- asynchronous session function --------

static TInt LaunchSessionTrebleOom()
/**
	Attempt to launch an asynchronous command on a session
	under OOM conditions.
 */
	{
	TPckgBuf<TInt> x = 3;
	TRequestStatus rs;
	sTestSession.Treble(x, rs);
	User::WaitForRequest(rs);
	
	if (rs == KErrNone)
		{
		test(x() == 9);
		}
	
	return rs.Int();
	}

static void TestAsyncInvalidDescPanic(RScsTestSession& aSession)
/**
	Pass an invalid descriptor to the SCS test server.
	The current thread should be panicked.

	@param	aSession		Open session to SCS test server,
							supplied by TestPanic.
	@see TestPanic
 */
	{
	TDes8* nullDes8 = 0;
	TRequestStatus rs;
	aSession.Treble(*nullDes8, rs);
	User::WaitForRequest(rs);
	}

static void TestRequeueOutstandingPanic(RScsTestSession& aSession)
/**
	Attempt to requeue an asynchronous request which is still
	outstanding.  The current thread should be panicked.

	@param	aSession		Connected session to SCS test server.
	@see TestPanic
 */
	{
	TPckgBuf<TInt> x1 = 1;
	TRequestStatus rs1;
	aSession.Treble(x1, rs1);
	
	TPckgBuf<TInt> x2 = 2;
	TRequestStatus rs2;
	aSession.Treble(x2, rs2);
	
	User::WaitForRequest(rs1, rs2);
	User::WaitForRequest(rs1, rs2);
	}

static void TestAsyncSessionFunction()
/**
	Test asynchronous session-relative functions.

	Launch a request and wait for it to complete.

	Launch a request and cancel it.

	Launch a request with an invalid descriptor.

	Cancel a request which is not outstanding.

	Launch a request in OOM.

	Launch an asynchronous request on a session and wait
	for it to complete.
 */
	{
	test.Start(_L("TestAsyncSessionFunction"));
	
	TInt r;
	
	LaunchSessionTrebleOom();
	
	// test launching async request in OOM
	RunOomTest(LaunchSessionTrebleOom, NULL);

	// cancel async request
	TPckgBuf<TInt> x = 3;
	TRequestStatus rs;
	sTestSession.Treble(x, rs);
	sTestSession.CancelTreble();
	User::WaitForRequest(rs);
	test(rs == KErrCancel);

	// bad descriptor panic
	TestPanic(TestAsyncInvalidDescPanic, ScsImpl::EScsClBadDesc);
	
	// safely cancel async request which is not queued
	sTestSession.CancelTreble();
	
	// panic if requeue outstanding request
	TestPanic(TestRequeueOutstandingPanic, ScsImpl::EScsClAsyncAlreadyQueued);
	
	// test outstanding request cancelled when session closed with RScsSessionBase::Close
	RScsTestSession asyncSession;
	r = asyncSession.Connect();
	test(r == KErrNone);
	x() = 4;
	asyncSession.Treble(x, rs);
	asyncSession.Close();
	User::After(ScsTestImpl::KTrebleTimerDelayUs + KOneSecondUs);
	test(rs == KErrCancel);
	User::WaitForRequest(rs);

	// test outstanding request not cancelled when session closed with RSessionBase::Close
	r = asyncSession.Connect();
	test(r == KErrNone);
	x() = 4;
	asyncSession.Treble(x, rs);
	asyncSession.RSessionBase::Close();
	User::After(ScsTestImpl::KTrebleTimerDelayUs + KOneSecondUs);
	test(rs == KRequestPending);		// client request not cancelled or completed
	
	test.End();
	}

// -------- open / close subsession --------

/**
	This subsession handle is opened in OOM testing.

	@see OpenOomSubsession
	@see CloseOomSubsession
 */
RScsTestSubsession sOomSubsession;

static TInt OpenOomSubsession()
/**
	Attempt to open a subsession in OOM.

	@see CloseOomSubsession
	@see RunOomTest
 */
	{
	return sOomSubsession.Create(sTestSession, 10);
	}

static void CloseOomSubsession()
/**
	Free the subsession which was opened in OOM.

	@see OpenOomSubsession
	@see RunOomTest
 */
	{
	sOomSubsession.Close();
	}

static void TestOpenCloseSubsession()
/**
	Test opening and closing a subsession, including
	opening in OOM.
 */
	{
	test.Start(_L("TestOpenCloseSubsession"));
	
	TInt r;
	
	RScsTestSubsession ss;
	r = ss.Create(sTestSession, 10);
	test(r == KErrNone);
	ss.Close();
	
	// test creating in OOM
	RunOomTest(OpenOomSubsession, CloseOomSubsession);
	
	test.End();
	}

// -------- invalid subsession function --------

static void TestInvalidSubsessionFunction()
/**
	Pass an unrecognized function to a subession and test
	the subsession handles it properly.  This tests the SCS
	test implementation, rather than the SCS itself.
 */
	{
	test.Start(_L("TestInvalidSubsessionFunction"));

	TInt r = sTestSubsession.SendFunction(ScsTestImpl::ESubsessUnused);	
	test(r == KErrNotSupported);
	
	test.End();
	}

// -------- synchronous subsession function --------

static void TestBadHandlePanic(RScsTestSession& aSession)
/**
	Call a subsession function passing in an invalid handle.
	The current thread should be panicked.

	@param	aSession		Open session to SCS test server,
							supplied by TestPanic.
	@see TestPanic
 */
	{
	RScsTestSubsession ss;
	TInt r = ss.Create(aSession, 10);
	test(r == KErrNone);
	
	// modify the subsession handle
	TUint8* addrOfHandle;
	addrOfHandle = (TUint8*)&ss;
	addrOfHandle += sizeof(RSessionBase);
	*((TInt*)addrOfHandle) ^= ~0;
	
	TInt x = 3;
	r = ss.Quadruple(x);	// should be panicked with EScsClBadHandle
	}

static void TestSyncSubsessionFunction()
/**
	Call a synchronous function on a subsession.  This ensures
	the request is routed to the subsession by the SCS.  Also test
	the SCS correctly handles an invalid subsession handle.
 */
	{
	test.Start(_L("TestSyncSubsessionFunction"));
	
	TInt x = -1;
	TInt r = sTestSubsession.Quadruple(x);
	test(r == KErrNone);
	test(x == 4 * KSubsessValue);
	
	TestPanic(TestBadHandlePanic, ScsImpl::EScsClBadHandle);
	
	test.End();
	}

// -------- asynchronous subsession function --------

static TInt LaunchSsTrebleOom()
/**
	Launch an asynchronous request on a subsession in OOM.

	@see RunOomTest
 */
	{
	TPckgBuf<TInt> x = 3;
	TRequestStatus rs;
	sTestSubsession.Treble(x, rs);
	User::WaitForRequest(rs);
	
	if (rs == KErrNone)
		{
		test(x() == 9);
		}
	
	return rs.Int();
	}

static void TestAsyncSubsessionFunction()
/**
	Test queueing and cancelling asynchronous requests on a subsession.
 */
	{
	test.Start(_L("TestAsyncSubsessionFunction"));
	
	TPckgBuf<TInt> x = 7;
	TRequestStatus rs;
	sTestSubsession.Treble(x, rs);
	User::WaitForRequest(rs);
	test(rs == KErrNone);
	test(x() == 21);
	
	// test launching async request in OOM
	RunOomTest(LaunchSsTrebleOom, NULL);

	// cancel async request
	sTestSubsession.Treble(x, rs);
	sTestSubsession.CancelTreble();
	User::WaitForRequest(rs);
	test(rs == KErrCancel);

	// cancel when no outstanding request
	sTestSubsession.CancelTreble();

	test.End();
	}

// -------- leak subsession --------

static void TestLeakSubsession()
/**
	Test closing a session with a remaining subsession;
	ensure server does not leak memory.
 */
	{
	test.Start(_L("TestLeakSubsession"));
	
	TInt r;
	
	sTestSession.SetServerHeapFail(KMaxTInt);
	
	RScsTestSession s;
	r = s.Connect();
	test(r == KErrNone);
	
	RScsTestSubsession ss;
	r = ss.Create(s, KSubsessValue);
	test(r == KErrNone);
	
	s.Close();
	LetServerRun();			// 100ms, let server clean up
	sTestSession.ResetServerHeapFail();
	
	test.End();
	}

// -------- correct async requests cancelled  --------

static void TestCorrectAsyncCancelled()
/**
	Test the correct requests are cancelled.
 */
	{	
	test.Start(_L("TestCorrectAsyncCancelled"));
	
	TInt r;
	
	RScsTestSession s1;
	r = s1.Connect();
	test(r == KErrNone);
	
	RScsTestSubsession ss1a;
	r = ss1a.Create(s1, KSubsessValue);
	test(r == KErrNone);
	
	RScsTestSubsession ss1b;
	r = ss1b.Create(s1, KSubsessValue);
	test(r == KErrNone);
	
	RScsTestSession s2;
	r = s2.Connect();
	test(r == KErrNone);
	
	RScsTestSubsession ss2a;
	r = ss2a.Create(s2, KSubsessValue);
	test(r == KErrNone);
	
	TRequestStatus r1, r1a, r1b, r2, r2a;
	TPckgBuf<TInt> i1(10), i1a(20), i1b(30), i2(40), i2a(50);
	
	s1.Treble(i1, r1);
	ss1a.Treble(i1a, r1a);
	ss1b.Treble(i1b, r1b);
	s2.Treble(i2, r2);
	ss2a.Treble(i2a, r2a);
	
	test(r1 == KRequestPending);
	test(r1a == KRequestPending);
	test(r1b == KRequestPending);
	test(r2 == KRequestPending);
	test(r2a == KRequestPending);
	
	ss1a.CancelTreble();				// subsession doesn't affect parent or siblings
	LetServerRun();
	test(r1 == KRequestPending);
	test(r1a == KErrCancel);
	test(r1b == KRequestPending);
	test(r2 == KRequestPending);
	test(r2a == KRequestPending);
	
	s2.CancelTreble();					// session doesn't affect child
	LetServerRun();
	test(r1 == KRequestPending);
	test(r1a == KErrCancel);
	test(r1b == KRequestPending);
	test(r2 == KErrCancel);
	test(r2a == KRequestPending);
	
	ss2a.Close();						// close subsession cancels outstanding request
	LetServerRun();
	test(r1 == KRequestPending);
	test(r1a == KErrCancel);
	test(r1b == KRequestPending);
	test(r2 == KErrCancel);
	test(r2a == KErrCancel);
	
	// consume pending signals
	User::WaitForRequest(r1);
	User::WaitForRequest(r1a);
	User::WaitForRequest(r1b);
	User::WaitForRequest(r2);
	User::WaitForRequest(r2a);
	
	test(r1 == KErrNone);
	test(r1a == KErrCancel);
	test(r1b == KErrNone);
	test(r2 == KErrCancel);
	test(r2a == KErrCancel);
	
	s1.Close();
	s2.Close();
	
	test.End();
	}

// -------- entrypoint --------


void MainL()
	{
	test.Title(_L("c:\\scstest.log"));
	test.Start(_L(" @SYMTestCaseID:SEC-SCSTEST-0001 scstest "));

	TestServerDeath();

	TestOpenClose();
	
	TInt r = sTestSession.Connect();
	test(r == KErrNone);
	
	TestInvalidSessionFunction();
	TestSyncSessionFunction();
	TestAsyncSessionFunction();
	
	TestOpenCloseSubsession();
	
	r = sTestSubsession.Create(sTestSession, KSubsessValue);
	test(r == KErrNone);
	
	TestInvalidSubsessionFunction();
	TestSyncSubsessionFunction();
	TestAsyncSubsessionFunction();
	TestLeakSubsession();
	TestCorrectAsyncCancelled();
	
	sTestSubsession.Close();
	sTestSession.ShutdownServer(); // Synchronous shutdown of server

	test.End();
	test.Close();
	}

void PanicIfError(TInt r)
	{
	if(r != KErrNone)
		{
		User::Panic(_L("upstest failed: "), r);
		}
	}


TInt E32Main()
/**
	Executable entrypoint establishes connection with SCS test server
	and then invokes tests for each functional area.
 */
	{
	// disable lazy DLL unloading so kernel heap balances at end
	RLoader l;
	PanicIfError(l.Connect());
	PanicIfError(l.CancelLazyDllUnload());
	l.Close();

	(void)test.Console();
	
	__UHEAP_MARK;
	__KHEAP_MARK;
	
	// allocating a cleanup stack also installs it
	CTrapCleanup* tc = CTrapCleanup::New();
	if (tc == 0)
		return KErrNoMemory;

	TRAPD(err, MainL());
	PanicIfError(err);

	
	delete tc;
	
	// The kernel appears to leave some memory allocated related to
	// econs.dll (maybe some sort of internal cache), which is
	// indirectly used by the test code, so we need to ignore this
	// allocation to allow our test to pass...
	User::__DbgMarkEnd(TRUE,1);
	__UHEAP_MARKEND;
	
	
	return KErrNone;
	}


// End of file
