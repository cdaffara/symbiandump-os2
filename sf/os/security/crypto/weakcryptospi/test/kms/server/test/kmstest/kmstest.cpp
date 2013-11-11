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
* Key Management Server test program.
*
*/


/**
 @file
*/


#include <e32test.h>
#include "kmsclient.h"
#include "kmsservercommon.h"
#include <e32def_private.h>

#include "kmstest.h"

#include <e32ldr.h>
#include <e32ldr_private.h>

RTest test(_L("KmsTest"));						///< Test object asserts conditions and prints status.

static RKeyMgmtSession kms;						///< Session object declared at file level for convenience.

static TKeyHandle GenHandle0;					///< When used, refers to generated key.
static TKeyHandle GenHandle1;					///< When used, refers to generated key.
static TKeyHandle StoreHandle0;					///< When used, refers to stored key.
static TKeyHandle StoreHandle1;					///< When used, refers to stored key.

const TInt KOperationX = MULTICHAR4("OP_X");	///< Arbitrary operation identifier to supply as argument to server.


// ---- panic testing ----

/**
	Test cases implement a function with this signature.  On entry
	the session has already been opened, and running in context of
	the thread which should be panicked.  The implementation should
	call the relevant session function, passing in the invalid parameter.
 */
typedef void (*TPanicFunc)(RKeyMgmtSession&);

static void ConfirmPanicReason(RThread aThread, TInt aExpectedReason)
/**
	Confirm that the supplied thread was panicked with KKmsClientPanicCat
	category and the supplied reason.

	@param	aThread			Thread whcih should have been panicked with the supplied reason.
	@param	aExpectedReason	Reason with which thread should have been panicked.
 */
	{
	TExitCategoryName exitCat = aThread.ExitCategory();
	test.Printf(_L("thread exit with type=%d, cat=\"%S\", reason=%d\n"), aThread.ExitType(), &exitCat, aThread.ExitReason());
	test(aThread.ExitType() == EExitPanic);
	test(exitCat == KKmsClientPanicCat);
	test(aThread.ExitReason() == aExpectedReason);
	}

static TInt TestPanicEntrypoint(TAny* aPtr)
/**
	Entrypoint for thread which is expected to be panicked.
	
	@param	aPtr			Standard launch parameter.  This is actuall the
							function which should be called with an open channel.
 */
	{
	RKeyMgmtSession kms;
	TInt r = kms.Connect();
	test(r == KErrNone);
	
	TPanicFunc f = TAnyPtrToFuncPtr<TPanicFunc>(aPtr);
	f(kms);
	
	// should not have reached here
	test(EFalse);
	
	return KErrNone;
	}

void TestPanic(TPanicFunc aFunc, TInt aExpectedReason)
/**
	Creates a new thread, runs the supplied function in its context,
	and confirms that it is panicked with category KKmsClientPanicCat
	and the supplied reason.

	@param	aFunc			Function to call in context of new thread.
	@param	aExpectedReason	The newly-created thread is expected to be panicked
							with this reason.
 */
	{
	test.Printf(_L("TestPanic: KmsLdd %d\n"), aExpectedReason);
	
	RThread thd;
	TInt r = thd.Create(
		_L("KmsTestTestPanic"), TestPanicEntrypoint,
		KDefaultStackSize, KMinHeapSize, KMinHeapSize, 
		(TAny*) aFunc);
	test(r == KErrNone);
	
	TRequestStatus rs;
	thd.Rendezvous(rs);
	test(rs == KRequestPending);
	TInt jit = User::JustInTime();
	User::SetJustInTime(EFalse);
	thd.Resume();
	
	User::WaitForRequest(rs);
	User::SetJustInTime(jit);
	ConfirmPanicReason(thd, aExpectedReason);
	thd.Close();
	}

// ---- custom channel ----

class RTestKeyMgmtSession : public RKeyMgmtSession
	{
public:
	TInt SendFunction(TInt aFunction);
	};

// ---- OOM testing ----

void RunOomTest(TInt (*aAllocFunc)(), void (*aFreeFunc)())
/**
	Test harness calls an allocating function in OOM, ensuring that no resources are leaked.

	@param	aAllocFunc		This allocating function is called in OOM.  It is expected
							to fail with KErrNoMemory, in which case no memory is leaked,
							or to succeed with KErrNone.
	@param	aFreeFunc		Cleans up resources allocated with aAllocFunc after aAllocFunc
							succeeds.
 */
	{
	TInt r = KErrNoMemory;
	for (TInt i = 1; r == KErrNoMemory; ++i)
		{
		__KHEAP_MARK;				// also ensure no resources leaked by key store
		
		kms.SetHeapFail(i);
		r = aAllocFunc();
		kms.EndHeapFail();
		
		test(r == KErrNone || r == KErrNoMemory);
		if (r == KErrNone)
			aFreeFunc();
		
		__KHEAP_MARKEND;
		__KHEAP_RESET;
		}
	}

// ---- simple open / close ----

static void TestOpenClose()
/**
	Open and close a connection to the key management server, but do not
	do anything with it.
 */
	{
	test.Start(_L("TestOpenClose"));
	
	RKeyMgmtSession s;
	TInt r = s.Connect();
	test(r == KErrNone);
	s.Close();

	// open with a lower-than-supported version
	TVersion v = KeyMgmtSvrImpl::Version();
	--v.iMajor;
	r = s.Connect(v);
	test(r == KErrNone);
	s.Close();
	
	// open with the supported version
	++v.iMajor;
	r = s.Connect(v);
	test(r == KErrNone);
	s.Close();
	
	// open with greater-than-supported version
	++v.iMajor;
	r = s.Connect(v);
	test(r == KErrNotSupported);

	test.End();
	}

// ---- generate key ----

static void TestGenerateKeyInvalidHandlePointer(RKeyMgmtSession& aSession)
/**
	Ask the server to generate a key, passing an invalid handle pointer.
	This function is defined to test that the current thread is panicked
	as expected.

	@param	aSession		Open session which is used to send the request.
 */
	 {
	 TKeyHandle* pkh = 0;
	 aSession.GenerateKey(4, *pkh);
	 }

static TInt TestGenKeyInOom()
/**
	This function is called by the OOM testing framework to ensure that
	key allocation is handled appropriately in OOM.
	
	@see CleanupKeyGenInOom
 */
	{
	return kms.GenerateKey(15, GenHandle1);
	}

static void CleanUpKeyGenInOom()
/**
	Cleans up resources allocated by TestGenKeyInOom.
	
	@see TestGenKeyInOom
 */
	{
	TInt r = kms.DeleteKey(GenHandle1);
	test(r == KErrNone);
	}

static void TestGenerateKey()
/**
	Test generating keys with the session.  See in-function
	comments for specific test cases.
 */
	{
	test.Start(_L("TestGenerateKey"));
	
	// IMPORT_C TInt GenerateKey(TInt aLength, TKeyHandle& aHandle);

	TInt r;
	
	// success case - key should be created
	r = kms.GenerateKey(3, GenHandle0);
	test(r == KErrNone);

	// faiure case bad length, reported by LDD
	TKeyHandle kh;
	r = kms.GenerateKey(-3, kh);
	test(r == KErrArgument);
	
	// failure case - bad handle pointer sent to server
	TestPanic(TestGenerateKeyInvalidHandlePointer, EBadClientDescriptor);
	
	// OOM not really required because server does not allocate any memory
	// to generate a key.
	RunOomTest(TestGenKeyInOom, CleanUpKeyGenInOom);
	test.End();
	}

// ---- store key ----

/** Sample store key data, defined at file level for convenience. */
_LIT8(KStoreKeyData, "0123456789abcdef");

static void TestStoreKeyInvalidDataDescriptor(RKeyMgmtSession& aSession)
/**
	Ask the server to store a key, passing in an invalid data descriptor.
	This function is defiend to test that the current thread is panicked
	as expected.

	@param	aSession		Open session which is used to send the request.
 */
	 {
	 const TDesC8* pDesc = 0;
	 TKeyHandle kh;
	 aSession.StoreKey(*pDesc, kh);
	 }

static void TestStoreKeyInvalidHandlePointer(RKeyMgmtSession& aSession)
/**
	Ask the server to store a key, passing in an invalid handle descriptor.
	This function is defiend to test that the current thread is panicked
	as expected.

	@param	aSession		Open session which is used to send the request.
 */
	{
	TKeyHandle* pkh = 0;
	aSession.StoreKey(KStoreKeyData, *pkh);
	}

static TInt TestStoreKeyInOom()
/**
	This function is called by the OOM testing framework to ensure that
	key allocation is handled appropriately in OOM.
	
	@see CleanupKeyGenInOom
 */
	{
	return kms.StoreKey(KStoreKeyData, StoreHandle1);
	}

static void CleanUpStoreGenInOom()
/**
	Cleans up resources allocated by TestGenKeyInOom.
	
	@see TestGenKeyInOom
 */
	{
	TInt r = kms.DeleteKey(StoreHandle1);
	test(r == KErrNone);
	}

static void TestStoreKey()
/**
	Test storing keys with the session.  See in-function comments for
	specific test cases.
 */
	{
	test.Start(_L("TestStoreKey"));
	
	// IMPORT_C TInt StoreKey(const TDesC8& aData, TKeyHandle& aHandle);
	
	TInt r;
	
	// success case - key is stored
	r = kms.StoreKey(KStoreKeyData, StoreHandle0);
	test(r == KErrNone);
	
	// failure case - bad length reported by LDD
	TKeyHandle kh;
	r = kms.StoreKey(KNullDesC8, kh);
	test(r == KErrArgument);
	
	// failure case - bad descriptor sent to server
	TestPanic(TestStoreKeyInvalidDataDescriptor, EBadClientDescriptor);
	
	// failure case - bad handle pointer
	TestPanic(TestStoreKeyInvalidHandlePointer, EBadClientDescriptor);
	
	// OOM
	RunOomTest(TestStoreKeyInOom, CleanUpStoreGenInOom);
	
	test.End();
	}

// ---- delete key ----

static void TestDeleteKey()
/**
	Test deleting keys with the session.  See in-function comments
	for specific test cases.
 */
	{
	test.Start(_L("TestDeleteKey"));
	
	// IMPORT_C TInt DeleteKey(TKeyHandle aHandle);
	
	TInt r;
	
	// success case - known key
	r = kms.DeleteKey(GenHandle0);
	test(r == KErrNone);
	
	// failure case - key not known
	r = kms.DeleteKey(GenHandle0);
	test(r == KErrNotFound);
	
	test.End();
	}

// ---- add usage ----

static void TestAddUsageInvalidSecPolPointer(RKeyMgmtSession& aSession)
/**
	Ask the server to add a usage, passing in an invalid security policy descriptor.
	This function is defiend to test that the current thread is panicked
	as expected.

	@param	aSession		Open session which is used to send the request.
 */
	{
	TSecurityPolicy* psecpol = 0;
	aSession.AddUsage(GenHandle0, KOperationX, *psecpol);
	}

static void TestAddUsage()
/**
	Test adding usages with the session.  See in-function comments
	for specific test cases.
 */
	{
	test.Start(_L("TestAddUsage"));
	
	// IMPORT_C TInt AddUsage(TKeyHandle aHandle, TInt aOperation, const TSecurityPolicy& aSecPol);
	
	TInt r;
	
	// success case 
	_LIT_SECURITY_POLICY_S0(KSecPolSidTestProc, 0xE04E4147);
	r = kms.AddUsage(StoreHandle0, KOperationX, KSecPolSidTestProc);
	test(r == KErrNone);
	
	// failure case - key not found
	r = kms.AddUsage(GenHandle0, KOperationX, KSecPolSidTestProc);
	test(r == KErrNotFound);
	
	// failure case - bad sec pol handle
	TestPanic(TestAddUsageInvalidSecPolPointer, EBadClientDescriptor);
	
	test.End();
	}

// ---- delete usage ----

static void TestDeleteUsage()
/**
	Test deleting usages with the session. See in-functino comments
	for specific test cases.
 */
	{
	test.Start(_L("TestDeleteUsage"));
	
	// IMPORT_C TInt DeleteUsage(TKeyHandle aHandle, TInt aOperation);

	TInt r;
	
	// success case
	r = kms.DeleteUsage(StoreHandle0, KOperationX);
	test(r == KErrNone);
	
	// failure case - key not found
	r = kms.DeleteUsage(GenHandle0, KOperationX);
	test(r == KErrNotFound);
	
	// failure case - usage not found
	r = kms.DeleteUsage(StoreHandle0, KOperationX);
	test(r == KErrNotFound);
	
	test.End();
	}

// ---- invalid function ----

TInt RTestKeyMgmtSession::SendFunction(TInt aFunction)
/**
	Send the supplied function identifier to the server, with
	no IPC args.  This function is defined to test that the
	server handles invalid function identifiers correctly.

	@param	aFunction		Function identifier.
	@return					Return value from SendReceive.  This is
							the value with which the server completed
							the request.
 */
	{
	TIpcArgs ipc;
	return SendReceive(aFunction, ipc);
	}

static void TestInvalidFunction()
/**
	Pass an invalid function code to the server and test that
	it completes it with KErrNotSupported.
 */
	{
	test.Start(_L("TestInvalidFunction"));
	
	RTestKeyMgmtSession tkms;
	TInt r = tkms.Connect();
	test(r == KErrNone);
	r = tkms.SendFunction(10);
	test(r == KErrNotSupported);
	tkms.Close();
	
	test.End();
	}

// ---- cleanup ----

static void CleanUp()
/**
	Clean up resources which were used during the test.
 */
	{
	test.Start(_L("CleanUp"));
	
	kms.DeleteKey(StoreHandle0);
	
	test.End();
	}

// ---- entrypoint ----

TInt E32Main()
/**
	Entrypoint establishes a connection with the KMS and
	then calls individual tests.

	@return					KErrNone.  If an error occurs then the
							process is panicked with RTest.
 */
	{
	TInt r;

	test.Title();
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-KMSTEST-0001 E32Main "));

	RLoader l;
	test(l.Connect()==KErrNone);
	test(l.CancelLazyDllUnload()==KErrNone);
	l.Close();
	
	__KHEAP_MARK;				// also ensure no resources leaked by key store
	__UHEAP_MARK;

	TestOpenClose();
	
	test.Next(_L("Connecting to server"));
	r = kms.Connect();
	test(r == KErrNone);

	TestGenerateKey();
	TestStoreKey();
	TestDeleteKey();
	TestAddUsage();
	TestDeleteUsage();
	TestInvalidFunction();
	CleanUp();

	kms.Close();
	
	// pause to allow LDD to be unloaded.
	User::After(5 * 1000 * 1000);
	
	__UHEAP_MARKEND;
	__KHEAP_MARKEND;

	test.End();
	return KErrNone;
	}
