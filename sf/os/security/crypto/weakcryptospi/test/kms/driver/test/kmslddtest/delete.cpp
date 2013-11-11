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
* Tests deleting a key from the hardware store.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


TInt DeleteSingleKeySync(TKeyHandle aHandle)
/**
	Delete a single key using the global channel object.  This function
	is provided for convenience, so the caller does not have to set up
	their own TRequestStatus object.

	@param	aHandle			Key to delete.
	@return					Symbian OS error code with which the driver completed
							the request.
 */
	{
	TRequestStatus rs;
	KmsChannel.DeleteKey(CurrentProcess, aHandle, rs);
	User::WaitForRequest(rs);
	return rs.Int();
	}

void TestDeleteSingleKey(TKeyHandle aHandle, TInt aExpectedResult)
/**
	Attempt to delete a single key and test the request was completed
	with the expected error code.

	@param	aHandle			Key to delete.
	@param	aExpectedResult	Expected Symbian OS error code with which the
							driver is expected to complete the request.
 */
	{
	TInt r = DeleteSingleKeySync(aHandle);
	test(r == aExpectedResult);
	}

static TInt TestDeleteKeyNotCreator(RProcess aProcess)
/**
	Attempt to delete a key with the supplied process as the claimed
	owner.  This function is defined to test the driver the correctly
	polices key deletion according to the claimed owner.

	@param	aProcess		Process which claims to own, and can therefore
							delete, the key.
	@return					Error code with which the driver completed the
							request.
 */
	{
	TRequestStatus rs;
	KmsChannel.DeleteKey(aProcess, GenKeyHandle0, rs);
	return rs.Int();
	}

void TestDeleteKey()
/**
	Test deleting keys from the hardware store.  See in-function
	comments for specific test cases.
 */
	{
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-DELETE-0001 TestDeleteKey "));
	
	TestGenerateSingleKey(1, CurrentProcess, GenKeyHandle0, KErrNone);
	TestGenerateSingleKey(1, CurrentProcess, GenKeyHandle1, KErrNone);
	TestGenerateSingleKey(1, CurrentProcess, StoreKeyHandle0, KErrNone);
	TestGenerateSingleKey(1, CurrentProcess, StoreKeyHandle1, KErrNone);
	
	TestDeleteSingleKey(GenKeyHandle0, KErrNone);
	TestDeleteSingleKey(GenKeyHandle1, KErrNone);
	TestDeleteSingleKey(StoreKeyHandle0, KErrNone);
	TestDeleteSingleKey(StoreKeyHandle1, KErrNone);
	
	// error case - non-existent handle
	TestDeleteSingleKey(GenKeyHandle0, KErrNotFound);
	TestDeleteSingleKey(GenKeyHandle1, KErrNotFound);
	TestDeleteSingleKey(StoreKeyHandle0, KErrNotFound);
	TestDeleteSingleKey(StoreKeyHandle1, KErrNotFound);

	// error case - not deleted by creator
	TestGenerateSingleKey(3, GenKeyHandle0, KErrNone);
	TestSecondProcess(/* aCapabilityMask */ 0, /* aSecureId */ 0, TestDeleteKeyNotCreator, KErrPermissionDenied);
	TestDeleteSingleKey(GenKeyHandle0, KErrNone);

	DeleteAllKeys();
	
	test.End();
	}

