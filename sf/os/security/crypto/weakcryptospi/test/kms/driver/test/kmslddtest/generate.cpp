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
* Tests generating a key.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


void TestGenerateSingleKey(TInt aLength, RProcess aOwner, TKeyHandle& aHandle, TInt aExpectedResult)
/**
	Generate a key with the global channel object, and test the LDD completes the request
	with the expected result.

	@param	aLength			Key length in bytes.
	@param	aOwner			Process which will own the newly-created key.
	@param	aHandle			On success this will identify the newly-created key.  Its
							value is undefined on failure.
	@param	aExpectedResult	Expected Symbian OS error code with which the LDD will complete
							the request.
 */
	{
	TRequestStatus rs;
	KmsChannel.GenerateKey(aLength, aOwner, aHandle, rs);
	User::WaitForRequest(rs);
	test(rs == aExpectedResult);
	}

void TestGenerateSingleKey(TInt aLength, TKeyHandle& aHandle, TInt aExpectedResult)
/**
	Generate a key with the global channel object, using the current process as the owner.

	@param	aLength			Key length in bytes.
	@param	aHandle			On success this will identify the newly-created key.  Its
							value is undefined on failure.
	@param	aExpectedResult	Expected Symbian OS error code with which the LDD will complete
							the request.
 */
	{
	TestGenerateSingleKey(aLength, CurrentProcess, aHandle, aExpectedResult);
	}

void RKmsChannelTest::TestInvalidGenArgsDescriptor(TRequestStatus& aStatus)
/**
	Supply an invalid TGenKeyArgs descriptor to the driver to ensure the current
	thread is panicked as expected.

	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	DoRequest(KmsLddImpl::EGenerateKey, aStatus, NULL);
	}

static void TestGenerateKeyBadHandleAddr(RKmsChannel& aChannel, TRequestStatus& aStatus)
/**
	Supply an invalid key handle to the LDD when generating the key, to ensure the
	driver panicks the current thread.

	@param	aChannel		Custom channel which is used to send generate request to
							the driver.
	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	TKeyHandle* ph = 0;		// in two stages to avoid RVCT #284-D: NULL reference is not allowed
	aChannel.GenerateKey(2, CurrentProcess, *ph, aStatus);
	}

static void GenKeyOomAlloc(TRequestStatus& aStatus)
/**
	Attempt to generate a key.  This function is called in OOM to test the
	driver cleans up on failure.

	@param	aStatus			On return, the caller will wait on this object.
							This is provided to save the function from having
							to declare an instance, wait on it, and then return
							return result.
	@see GenKeyOomFree
 */
	{
	KmsChannel.GenerateKey(12, CurrentProcess, GenKeyHandle0, aStatus);
	}

static void GenKeyOomFree()
/**
	Clean up resources allocated for GenKeyOomAlloc.  This is called after the
	operation succeeds.

	@see GenKeyOomAlloc.
 */
	{
	TestDeleteSingleKey(GenKeyHandle0, KErrNone);
	}


void TestGenerateKey()
/**
	Test generating keys, successfully and otherwise.  See in-function
	comments for specific test cases.
 */
	{
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-GENERATE-0001 TestGenerateKey "));
	
	TestGenerateSingleKey(1, GenKeyHandle0, KErrNone);
	TestGenerateSingleKey(13, GenKeyHandle1, KErrNone);

	// error case - invalid length
	TestGenerateSingleKey(0, GenKeyHandle0, KErrArgument);

	// error case - invalid arg descriptor
	TestPanicTestChannel(&RKmsChannelTest::TestInvalidGenArgsDescriptor, KmsLddImpl::EGenKeyBadArgDescriptor);
	
	// error case - invalid handle
	TestPanic(TestGenerateKeyBadHandleAddr, KmsLddImpl::EGenKeyInvalidHandleAddr);

	// OOM
	TestDeleteSingleKey(GenKeyHandle0, KErrNone);
	RunOomTest(GenKeyOomAlloc, GenKeyOomFree);

	DeleteAllKeys();
	test.End();
	}

