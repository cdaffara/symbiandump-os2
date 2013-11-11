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
* Test KMS LDD channel enables test code to send invalid arguments
* to the LDD, which may not be available when the client API is used.
* This file contains test functions which are used by all clients.
* Specific test functions, e.g. for generating keys, are stored with
* the relevant test code.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


TInt RKmsChannelTest::Open()
/**
	Open a channel on the KMS LDD.
 */
	{
	// device driver factory name.  This is not the LDD filename.
	const TDesC& name = RKmsChannel::FactoryName();
	
	// The driver must have been built with at least the same version as this channel.
	const TVersion reqVer = KmsLddImpl::Version();

	return DoCreate(
		name,
		reqVer,
		KNullUnit,			// aUnit: units not supported (KDeviceAllowUnit flag not set)
		NULL,				// aDriver: PDD name, not supported (KDeviceAllocPhysicalDevice flag not set)
		NULL,				// anInfo: allow info not supported (KDeviceAllowInfo flag not set)
		EOwnerThread,		// aType: handle is owned by thread, not process
		EFalse);			// aProtected: channel cannot be shared with other processes
	}

static TInt TestPanicTestChannelEntrypoint(TAny* aPtr)
/**
	Entrypoint for suicide thread which uses the custom channel.

	@param	aPtr			Standard entrypoint argument.  This points to
							a member function to invoke on the thread.
	@return					Thread entrypoints must return a value, but this
							is not used because the thread is panicked.
 */
	{
	RKmsChannelTest testLdd;
	TInt r = testLdd.Open();
	test(r == KErrNone);
	
	TTestChannelPanicFunc f = *reinterpret_cast<const TTestChannelPanicFunc*>(aPtr);
	TRequestStatus rs;
	(testLdd.*f)(rs);
	User::WaitForRequest(rs);
	
	// should not have reached here
	test(EFalse);
	
	return KErrNone;
	}

void TestPanicTestChannel(TTestChannelPanicFunc aFunc, TInt aExpectedReason)
/**
	Create a suicide thread and invoke the supplied function on it.  This function
	is used to test that a thread is panicked with the expected category and reason
	when it supplies invalid references to client-side data to the channel.

	@param	aFunc			Function to invoke on custom channel.
	@param	aExpectedReason	The thread is expected to be panicked with
							category KKmsLddPanicCat and this reason.
 */
	{
	test.Printf(_L("TestPanicTestChannel: KMSLDD %d\n"), aExpectedReason);
	
	RThread thd;
	thd.Create(
		_L("KmsLddTestChannel"), TestPanicTestChannelEntrypoint,
		KDefaultStackSize, KMinHeapSize, KMinHeapSize, 
		&aFunc);
	
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

