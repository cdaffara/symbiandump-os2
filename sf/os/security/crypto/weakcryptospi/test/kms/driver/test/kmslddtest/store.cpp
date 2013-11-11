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
* Tests storing a key in the hardware store.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


/** Literal key data, declared at file level for convenience. */
_LIT8(KStoreKeyData, "0123456789abcdef");

static void TestStoreSingleKey(const TDesC8& aData, RProcess aOwner, TKeyHandle& aHandle, TInt aExpectedResult)
/**
	Attempt to store a key in the hardware store and check the operation is completed
	with the expected error code.

	@param	aData			Key data to store in hardware.
	@param	aOwner			Process which will own the newly-created key.
	@param	aHandle			On success this will identify the newly-created key.  Its
							value is undefined on failure.
	@param	aExpectedResult	Expected Symbian OS error code with which the LDD will complete
							the request.
 */
	{
	TRequestStatus rs;
	KmsChannel.StoreKey(aData, aOwner, aHandle, rs);
	User::WaitForRequest(rs);
	test(rs == aExpectedResult);
	}

void TestStoreSingleKey(const TDesC8& aData, TKeyHandle& aHandle, TInt aExpectedResult)
/**
	Attempt to store a key in the hardware store, using the current process as its owner.

	@param	aData			Key data to store in hardware.
	@param	aHandle			On success this will identify the newly-created key.  Its
							value is undefined on failure.
	@param	aExpectedResult	Expected Symbian OS error code with which the LDD will complete
							the request.
 */
	{
	TestStoreSingleKey(aData, CurrentProcess, aHandle, aExpectedResult);
	}

void RKmsChannelTest::TestInvalidStoreArgsDescriptor(TRequestStatus& aStatus)
/**
	Supply an invalid TStoreKeyArgs descriptor to the driver to ensure the current
	thread is panicked as expected.

	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	DoRequest(KmsLddImpl::EStoreKey, aStatus, (TAny*) &KStoreKeyData);
	}

static void TestStoreKeyBadDataDesc(RKmsChannel& aChannel, TRequestStatus& aStatus)
/**
	Supply an invalid key data desciptor to the driver, to ensure the current thread
	is panicked as expected.

	
	@param	aChannel		Custom channel which is used to send store request to
							the driver.
	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	TDesC8* data = 0;
	TKeyHandle kh;
	aChannel.StoreKey(*data, CurrentProcess, kh, aStatus);	
	}

static void TestStoreKeyBadHandleAddr(RKmsChannel& aChannel, TRequestStatus& aStatus)
/**
	Supply an invalid handle pointer to the driver, to ensure the current thread is
	panicked as expected.

	@param	aChannel		Custom channel which is used to send store request to
							the driver.
	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	TKeyHandle* ph = 0;		// in two stages to avoid RVCT #284-D: NULL reference is not allowed
	aChannel.StoreKey(KStoreKeyData, CurrentProcess, *ph, aStatus);
	}

static void StoreKeyOomAlloc(TRequestStatus& aStatus)
/**
	Attempt to store a key.  This function is called in OOM to test the
	driver cleans up on failure.

	@param	aStatus			On return, the caller will wait on this object.
							This is provided to save the function from having
							to declare an instance, wait on it, and then return
							return result.
	@see StoreKeyOomFree
 */
	{
	KmsChannel.StoreKey(KStoreKeyData, CurrentProcess, StoreKeyHandle0, aStatus);
	}

static void StoreKeyOomFree()
/**
	Clean up resources allocated for StoreKeyOomAlloc.  This is called after the
	operation succeeds.

	@see StoreKeyOomAlloc.
 */
	{
	TestDeleteSingleKey(StoreKeyHandle0, KErrNone);
	}

void TestStoreKey()
/**
	Test storing user-defined keys in the hardware store.
	See in-function comments for specific test cases.
 */
	{
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-STORE-0001 TestStoreKey "));
	
	TestStoreSingleKey(KStoreKeyData, StoreKeyHandle0, KErrNone);
	TestStoreSingleKey(KStoreKeyData, StoreKeyHandle1, KErrNone);

	// error case - invalid arg desc
	TestPanicTestChannel(&RKmsChannelTest::TestInvalidStoreArgsDescriptor, KmsLddImpl::EStoreKeyBadArgDescriptor);
	
	// error case - invalid length
	TestStoreSingleKey(KNullDesC8, StoreKeyHandle1, KErrArgument);
	
	// error case - invalid data
	TestPanic(TestStoreKeyBadDataDesc, KmsLddImpl::EStoreKeyInvalidDataDesc);
	
	// error case - invalid handle
	TestPanic(TestStoreKeyBadHandleAddr, KmsLddImpl::EStoreKeyInvalidHandleAddr);
	
	// OOM
	RunOomTest(StoreKeyOomAlloc, StoreKeyOomFree);

	DeleteAllKeys();
	
	test.End();
	}

