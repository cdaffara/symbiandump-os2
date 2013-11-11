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
* Framework used to launch static suicide threads which are expected
* to be panicked.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


void ConfirmPanicReason(RThread aThread, TInt aExpectedReason)
/**
	Confirm the supplied thread was panicked with category KKmsLddPanicCat
	and the supplied reason.

	@param	aThread			Thread which should have been panicked.
	@param	aExpectedReason	Reason with which the supplied thread should have
							been panicked.
 */
	{
	TExitCategoryName exitCat = aThread.ExitCategory();
	test.Printf(_L("thread exit with type=%d, cat=\"%S\", reason=%d\n"), aThread.ExitType(), &exitCat, aThread.ExitReason());
	test(aThread.ExitType() == EExitPanic);
	test(exitCat == KmsLddImpl::KKmsLddPanicCat);
	test(aThread.ExitReason() == aExpectedReason);
	}

static TInt TestPanicEntrypoint(TAny* aPtr)
/**
	Entrypoint for thread which is expected to be panicked.
	
	@param	aPtr			Standard launch parameter.  This points to
							the function which should be called with an
							open channel.
	@return					Thread entrypoints must return a value, but this
							is not used because the thread is killed.
 */
	{
	RKmsChannel kmsldd;
	TInt r = kmsldd.Open();
	test(r == KErrNone);
	
	TPanicFunc f = TAnyPtrToFuncPtr<TPanicFunc>(aPtr);
	TRequestStatus rs;
	f(kmsldd, rs);
	User::WaitForRequest(rs);
	
	// should not have reached here
	test(EFalse);
	
	return KErrNone;
	}

RThread LaunchThreadAndWaitForRendezvous(TThreadFunction aEntrypoint, TAny* aData)
/**
	Launches a second thread in this process and waits for it
	to complete a rendezvous.
	
	@param	aEntrypoint		The new thread's entrypoint function.
	@param	aData			Cookie data sent to entrypoint function.
	@return					Open handle to newly-created thread.  Ownership
							of the thread is transferred to the caller who
							must close it.
 */
	{
	RThread thd;
	TInt r = thd.Create(
		_L("KmsLddTestPanic"), aEntrypoint,
		KDefaultStackSize, KMinHeapSize, KMinHeapSize, 
		aData);
	test(r == KErrNone);
	
	TRequestStatus rs;
	thd.Rendezvous(rs);
	test(rs == KRequestPending);
	thd.Resume();
	
	User::WaitForRequest(rs);
	return thd;
	}

void TestPanic(TPanicFunc aFunc, TInt aExpectedReason)
/**
	Launch a thread which tries to get panicked by calling a channel
	function and providing an invalid reference to user-side data.

	@param	aFunc			Function to execute in suicide thread context.
	@param	aExpectedReason	The thread is expected to be panicked with
							category KKmsLddPanicCat and this reason.
 */
	{
	TInt jit = User::JustInTime();
	User::SetJustInTime(EFalse);
	RThread thd = LaunchThreadAndWaitForRendezvous(
		TestPanicEntrypoint,  (TAny*) aFunc);	
	User::SetJustInTime(jit);
	
	ConfirmPanicReason(thd, aExpectedReason);
	thd.Close();
	}
