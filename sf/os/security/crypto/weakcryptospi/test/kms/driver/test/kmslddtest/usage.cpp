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
* Tests add, deleting, and testing key usages.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


// To simplify the test code, most of the add usage tests use GenKeyHandle0 and KOperationX.


void AddUsage(RProcess aSetter, TKeyHandle aHandle, TInt aOperation, const TSecurityPolicy& aSecPol, TInt aExpectedResult)
/**
	Attempt to add a usage for a key (which may be) in the store, and test the driver
	completes the request with the expected result.

	@param	aSetter			Process which claims to own the key, and can therefore
							define a usage policy for it.
	@param	aHandle			Key for which to define the usage.
	@param	aOperation		Numeric identifier for operation.
	@param	aSecPol			Security policy to apply for access to this operation.
	@param	aExpectedResult	Result with which the driver is expected to complete the request.
 */
	{
	TRequestStatus rs;
	KmsChannel.AddUsage(aSetter, aHandle, aOperation, aSecPol, rs);
	User::WaitForRequest(rs);
	test(rs == aExpectedResult);
	}

static void AddGKZeroUsage(RProcess aSetter, TInt aOperation, const TSecurityPolicy& aSecPol, TInt aExpectedResult)
/**
	Add as usage policy for key GenKeyHandle0.  This function is defined for
	convenience to improve the readability of calling code.

	@param	aSetter			Process which claims to own the key, and can therefore
							define a usage policy for it.
	@param	aOperation		Numeric identifier for operation.
	@param	aSecPol			Security policy to apply for access to this operation.
	@param	aExpectedResult	Result with which the driver is expected to complete the request.
	@see AddGKOneUsage
 */
	{
	AddUsage(aSetter, GenKeyHandle0, aOperation, aSecPol, aExpectedResult);
	}

static void AddGKOneUsage(RProcess aSetter, TInt aOperation, const TSecurityPolicy& aSecPol, TInt aExpectedResult)
/**
	Add as usage policy for key GenKeyHandle1.  This function is defined for
	convenience to improve the readability of calling code.

	@param	aSetter			Process which claims to own the key, and can therefore
							define a usage policy for it.
	@param	aOperation		Numeric identifier for operation.
	@param	aSecPol			Security policy to apply for access to this operation.
	@param	aExpectedResult	Result with which the driver is expected to complete the request.
	@see AddGKZeroUsage
 */
	{
	AddUsage(aSetter, GenKeyHandle1, aOperation, aSecPol, aExpectedResult);
	}

enum TOpXPermission
/**
	These elements are aliases for boolean values.  They are defined to improve
	the readability of calls to ConfirmGKZeroUsages and ConfirmGKOneUsages.  They also
	ensure expected permission values for oeprations X and Y do not get mixed up.

	@see ConfirmGKZeroUsages
	@see ConfirmGKOneUsages
 */
	{
	EOpXAllowed = ETrue,
	EOpXDenied = EFalse
	};

enum TOpYPermission
/**
	These elements are aliases for boolean values.  They are defined to improve
	the readability of calls to ConfirmGKZeroUsages and ConfirmGKOneUsages.  They also
	ensure expected permission values for oeprations X and Y do not get mixed up.

	@see ConfirmGKZeroUsages
	@see ConfirmGKOneUsages
 */
	{
	EOpYAllowed = ETrue,
	EOpYDenied = EFalse
	};


static void ConfirmUsage(RProcess aUsingProc, TKeyHandle aHandle, TInt aOperation, TBool aExpected)
/**
	Confirm whether the process can use the supplied operation on the supplied key.

	@param	aUsingProc		Process to test for permission.
	@param	aHandle			Key with which operation is to be performed.
	@param	aOperation		Numeric operation identifier.
	@param	aExpected		Whether the operation is expected to be allowed.
 */
	{
	TBool allowed;
	TRequestStatus rs;
	KmsChannel.UsageAllowed(aUsingProc, aHandle, aOperation, allowed, rs);
	User::WaitForRequest(rs);
	
	test(rs == KErrNone);
	test(allowed == aExpected);
	}

static void ConfirmGKZeroUsages(RProcess aUsingProc, TOpXPermission aOpXAllowed, TOpYPermission aOpYAllowed)
/**
	Confirms whether the supplied process can use operations X and Y on GenKeyHandle0.

	@param	aUsingProc		Process to test for permission.
	@param	aOpXAllowed		Whether operation X is expected to be allowed.
	@param	aOpYAllowed		Whether operation Y is expected to be allowed.
 */
	{
	ConfirmUsage(aUsingProc, GenKeyHandle0, KOperationX, aOpXAllowed);
	ConfirmUsage(aUsingProc, GenKeyHandle0, KOperationY, aOpYAllowed);
	}

static void ConfirmGKOneUsages(RProcess aUsingProc, TOpXPermission aOpXAllowed, TOpYPermission aOpYAllowed)
/**
	Confirms whether the supplied process can use operations X and Y on GenKeyHandle1.

	@param	aUsingProc		Process to test for permission.
	@param	aOpXAllowed		Whether operation X is expected to be allowed.
	@param	aOpYAllowed		Whether operation Y is expected to be allowed.
 */
	{
	ConfirmUsage(aUsingProc, GenKeyHandle1, KOperationX, aOpXAllowed);
	ConfirmUsage(aUsingProc, GenKeyHandle1, KOperationY, aOpYAllowed);
	}

static void DeleteUsage(RProcess aDeletor, TKeyHandle aHandle, TInt aOperation, TInt aExpectedResult)
/**
	Delete the supplied usage from the hardware store.

	@param	aDeletor		Process which is claiming to own the key, and can
							therefore delete the usage.
	@param	aHandle			Key for which the usage is to be deleted.
	@param	aOperation		Numeric operation identifier.
	@param	aExpectedResult	Result with which driver is expected to complete the request.
 */
	{
	TRequestStatus rs;
	KmsChannel.DeleteUsage(aDeletor, aHandle, aOperation, rs);
	User::WaitForRequest(rs);
	test(rs == aExpectedResult);
	}

static void DeleteGKZeroUsage(RProcess aDeletor, TInt aOperation, TInt aExpectedResult)
/**
	Delete the supplied usage for GenKeyHandle0.  This function is defined so calling
	code is more readable than callilng DeleteUsage directly.

	@param	aDeletor		Process which is claiming to own the key, and can
							therefore delete the usage.
	@param	aOperation		Numeric operation identifier.
	@param	aExpectedResult	Result with which driver is expected to complete the request.
	@see DeleteGKOneUsage
 */
	{
	DeleteUsage(aDeletor, GenKeyHandle0, aOperation, aExpectedResult);
	}

static void DeleteGKOneUsage(RProcess aDeletor, TInt aOperation, TInt aExpectedResult)
/**
	Delete the supplied usage for GenKeyHandle0.  This function is defined so calling
	code is more readable than callilng DeleteUsage directly.

	@param	aDeletor		Process which is claiming to own the key, and can
							therefore delete the usage.
	@param	aOperation		Numeric operation identifier.
	@param	aExpectedResult	Result with which driver is expected to complete the request.
	@see DeleteGKZeroUsage
 */
	{
	DeleteUsage(aDeletor, GenKeyHandle1, aOperation, aExpectedResult);
	}

static void ConfirmDeleteKey(RProcess aClaimedOwner, TKeyHandle aHandle, TInt aExpectedResult)
/**
	Delete a single key from the hardware store, and confirm the driver completes
	the request with the expected result.

	@param	aClaimedOwner	Process which claims to own, and can therefore
							delete, the key.
	@param	aHandle			Key to delete.
	@param	aExpectedResult	Result with which driver is expected to complete the request.
 */
	{
	TRequestStatus rs;
	KmsChannel.DeleteKey(aClaimedOwner, aHandle, rs);
	User::WaitForRequest(rs);
	test(rs == aExpectedResult);
	}

void RKmsChannelTest::TestAddUsageBadArgDescriptor(TRequestStatus& aStatus)
/**
	Pass a usage identifier args descriptor to the driver to ensure the
	current thread is panicked as expected.

	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	TSecurityPolicyBuf secPolBuf;	
	DoRequest(KmsLddImpl::EAddUsage, aStatus, (TAny*) 0, &secPolBuf);
	}

static void TestAddUsageBadSecPolAddr(RKmsChannel& aChannel, TRequestStatus& aStatus)
/**
	Pass a bad security policy address (which the client API wraps with a descriptor) 
	to the driver.  This function is defined to ensure the driver panicks the current
	thread as expected.

	@param	aChannel		Channel object on which to call AddUsage.
	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	TSecurityPolicy* secPol = 0;
	aChannel.AddUsage(CurrentProcess, GenKeyHandle0, KOperationX, *secPol, aStatus);
	}

void RKmsChannelTest::TestDeleteUsageBadArgDescriptor(TRequestStatus& aStatus)
/**
	Pass a usage identifier args descriptor to the driver to ensure the
	current thread is panicked as expected.

	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	DoRequest(KmsLddImpl::EDeleteUsage, aStatus, (TAny*) 0);
	}

void RKmsChannelTest::TestUsageAllowedBadArgDescriptor(TRequestStatus& aStatus)
/**
	Pass a usage allowed args descriptor to the driver to ensure the
	current thread is panicked as expected.

	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	TBool allowed;
	DoRequest(KmsLddImpl::EUsageAllowed, aStatus, (TAny*) 0, (TAny*) &allowed);
	}

static void TestUsageAllowedBadAllowedAddr(RKmsChannel& aChannel, TRequestStatus& aStatus)
/**
	Pass a bad allowed TBool pointer to the driver to ensure the current thread is
	panicked as expected.

	@param	aChannel		Channel object on which to call UsageAllowed.
	@param	aStatus			This object is passed to the driver, but it should not
							be used because the thread should be panicked.
							It is supplied to this function as a convenience so this
							function does not have to declare its own instance.
 */
	{
	TBool* allowed = 0;
	aChannel.UsageAllowed(CurrentProcess, GenKeyHandle0, KOperationX, *allowed, aStatus);
	}

static TInt TestUsageWithSecondProcHandle(RProcess aSecondProc)
/**
	This helper function is defined to be called with a second process handle, so
	it can supply a variety of process handles to the channel functions.

	@param	aSecondProc		Handle to process with SID KSidSecondProc.
	@return					KErrNone.  This value is only returned to satisfy the
							TestUsageWithSecondProcHandle argument signature.
							
 */
	{
	RProcess testProc;
	
	GenKeyHandle0.iValue = 0;
	GenKeyHandle1.iValue = 0;
	
	// key does not exist (no keys so value of GK0 doesn't matter here)
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKZeroUsages(aSecondProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);

	// add usage when key does not exist
	AddGKZeroUsage(aSecondProc, KOperationX, KSecPolSidSecondProc, KErrNotFound);
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKZeroUsages(aSecondProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	// add usage when key owned by different process from setter
	TestGenerateSingleKey(3, testProc, GenKeyHandle0, KErrNone);
	AddGKZeroUsage(aSecondProc, KOperationX, KSecPolSidCurrentProc, KErrPermissionDenied);
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKZeroUsages(aSecondProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	// add usage when key exists and is owned by setter
	AddGKZeroUsage(testProc, KOperationX, KSecPolSidCurrentProc, KErrNone);
	ConfirmGKZeroUsages(testProc, EOpXAllowed, EOpYDenied);
	ConfirmGKZeroUsages(aSecondProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	// overwrite usage when key exists and is owned by setter
	AddGKZeroUsage(testProc, KOperationX, KSecPolSidSecondProc, KErrNone);
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKZeroUsages(aSecondProc, EOpXAllowed, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	// test single key with multiple usages
	AddGKZeroUsage(testProc, KOperationY, KSecPolSidCurrentProc, KErrNone);
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYAllowed);
	ConfirmGKZeroUsages(aSecondProc, EOpXAllowed, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	// test multiple keys with the same operation
	TestGenerateSingleKey(3, aSecondProc, GenKeyHandle1, KErrNone);
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYAllowed);
	ConfirmGKZeroUsages(aSecondProc, EOpXAllowed, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	AddGKOneUsage(aSecondProc, KOperationX, KSecPolSidCurrentProc, KErrNone);
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYAllowed);
	ConfirmGKZeroUsages(aSecondProc, EOpXAllowed, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXAllowed, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	// delete usage where usage does not exist
	DeleteGKOneUsage(aSecondProc, KOperationY, KErrNotFound);
	
	// delete usage where the deletor is not the owner
	DeleteGKZeroUsage(aSecondProc, KOperationX, KErrPermissionDenied);
	
	// delete normal usage
	DeleteGKOneUsage(aSecondProc, KOperationX, KErrNone);
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYAllowed);
	ConfirmGKZeroUsages(aSecondProc, EOpXAllowed, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	// delete key, test usages deleted
	TestDeleteSingleKey(GenKeyHandle0, KErrNone);
	ConfirmGKZeroUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKZeroUsages(aSecondProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(testProc, EOpXDenied, EOpYDenied);
	ConfirmGKOneUsages(aSecondProc, EOpXDenied, EOpYDenied);
	
	// cleanup
	ConfirmDeleteKey(aSecondProc, GenKeyHandle1, KErrNone);

	// add usage error case - bad argument descriptor
	TestPanicTestChannel(&RKmsChannelTest::TestAddUsageBadArgDescriptor, KmsLddImpl::EAddUsageBadArgDescriptor);
	
	// add usage error case - bad sec pol reference
	TestPanic(TestAddUsageBadSecPolAddr, KmsLddImpl::EAddUsageBadSecPolDescriptor);
	
	// delete usage error case - bad argument descriptor.
	// Same panic reason as for AddUsage because same code used to copy data from user side.
	TestPanicTestChannel(&RKmsChannelTest::TestDeleteUsageBadArgDescriptor, KmsLddImpl::EAddUsageBadArgDescriptor);
	
	// usage allowed error case - bad argument descriptor
	TestPanicTestChannel(&RKmsChannelTest::TestUsageAllowedBadArgDescriptor, KmsLddImpl::EUsageAllowedBadArgDescriptor);
	
	// add usage error case - bad sec pol reference
	TestPanic(TestUsageAllowedBadAllowedAddr, KmsLddImpl::EUsageAllowedBadAllowedPtr);
	
	// add usage error case - bad proc handle
	RProcess pr;
	pr.Close();
	TBool allowed;
	TRequestStatus rs;
	KmsChannel.UsageAllowed(pr, GenKeyHandle0, KOperationX, allowed, rs);
	User::WaitForRequest(rs);
	test(rs == KErrArgument);
	
	return KErrNone;
	}

static void AddUsageOomAlloc(TRequestStatus& aStatus)
/**
	Attempt to add a usage.  This function is called in OOM to test the
	driver cleans up on failure.

	@param	aStatus			On return, the caller will wait on this object.
							This is provided to save the function from having
							to declare an instance, wait on it, and then return
							return result.
	@see AddUsageOomFree
 */
	{
	KmsChannel.AddUsage(CurrentProcess, GenKeyHandle0, KOperationX, KSecPolSidCurrentProc, aStatus);
	}

static void AddUsageOomFree()
/**
	Clean up operation for StoreKeyOomAlloc.  This is called once the operation
	has succeeded.

	@see AddUsageOomAlloc.
 */
	{
	DeleteGKZeroUsage(CurrentProcess, KOperationX, KErrNone);
	}

void TestUsage()
/**
	Test adding, deleting, and checking usages.  See TestUsageWithSecondProcHandle
	for specific test cases.

	@see TestUsageWithSecondProcHandle
 */
	{
	test.Start(_L("TestAddUsage"));
	
	TestSecondProcess(/* aCapabilityMask */ 0, KSidSecondProc, TestUsageWithSecondProcHandle, KErrNone);
	
	// OOM
	TestGenerateSingleKey(3, CurrentProcess, GenKeyHandle0, KErrNone);
	RunOomTest(AddUsageOomAlloc, AddUsageOomFree);

	DeleteAllKeys();
	
	test.End();
	}
