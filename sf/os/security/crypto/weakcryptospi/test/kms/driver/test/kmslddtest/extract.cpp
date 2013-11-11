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
* Tests extracting keys from the hardware store.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


static void ConfirmSingleExtractionSucceeds(TKeyHandle aHandle, TInt aOperation, const TDesC8& aExpectedData)
/**
	Attempt to extract the identified key from the hardware key store,
	and test it matches the supplied data.

	@param	aHandle			Identifies key to extract from hardware store.
	@param	aOperation		Operation for which this key is to be extracted.  This does not
							affect the key data, but determines whether this process is allowed
							to use (and therefore extract) the key from the store.
	@param	aExpectedData	Expected key data.
 */
	{
	HBufC8* data;
	TInt r = ExtrChannel.Extract(aHandle, aOperation, data);
	test(r == KErrNone);
	test(*data == aExpectedData);
	delete data;
	}

static void ConfirmSingleExtractionFails(TKeyHandle aHandle, TInt aOperation)
/**
	Attempt to extract the identified key from the hardware key store,
	and confirm that the extraction operation fails (because this process
	cannot use the key for the requested operation.)

	@param	aHandle			Identifies key to extract from hardware store.
	@param	aOperation		Operation for which this key is to be extracted.  This does not
							affect the key data, but determines whether this process is allowed
							to use (and therefore extract) the key from the store.
 */
	{
	HBufC8* data;
	TInt r = ExtrChannel.Extract(aHandle, aOperation, data);
	test(r == KErrPermissionDenied);
	}

static void ExtractKeyOomAlloc(TRequestStatus& aStatus)
/**
	Test extract functionality fails sensibly in OOM.
	
	@param	aStatus			Not used.  the extract LDD accesses the
							store synchronously.
	@see ExtractKeyOomFree
 */
	{
	HBufC8* data;
	TInt r = ExtrChannel.Extract(StoreKeyHandle0, KOperationX, data);
	if (r == KErrNone)
		delete data;
	TRequestStatus* prs = &aStatus;
	User::RequestComplete(prs, r);
	}

static void ExtractKeyOomFree()
/**
	Free kernel-side resources allocated by ExtractKeyOomAlloc.
	Does nothing because ExtractKeyOomAlloc does not allocate any
	kernel-side resources.
	
	@see ExtractKeyOomAlloc
 */
	{
	// empty.
	}

/** Test data which will be stored as a key and then extracted. */
_LIT8(KStoreKey0Data, "00000000");
/** Test data which will be stored as a key and then extracted. */
_LIT8(KStoreKey1Data, "11111111");

void TestExtraction()
/**
	Ensure the extraction functions correctly police access to the keys.
	See in-function comments for specific test cases.
 */
	{
	// SK0, X -> current; SK0, Y -> second
	TestStoreSingleKey(KStoreKey0Data, StoreKeyHandle0, KErrNone);
	AddUsage(CurrentProcess, StoreKeyHandle0, KOperationX, KSecPolSidCurrentProc, KErrNone);
	AddUsage(CurrentProcess, StoreKeyHandle0, KOperationY, KSecPolSidSecondProc, KErrNone);
	
	// SK1, X -> second; SK0, Y -> current
	TestStoreSingleKey(KStoreKey1Data, StoreKeyHandle1, KErrNone);
	AddUsage(CurrentProcess, StoreKeyHandle1, KOperationX, KSecPolSidSecondProc, KErrNone);
	AddUsage(CurrentProcess, StoreKeyHandle1, KOperationY, KSecPolSidCurrentProc, KErrNone);
	
	ConfirmSingleExtractionSucceeds(StoreKeyHandle0, KOperationX, KStoreKey0Data);
	ConfirmSingleExtractionFails(StoreKeyHandle0, KOperationY);
	ConfirmSingleExtractionFails(StoreKeyHandle1, KOperationX);
	ConfirmSingleExtractionSucceeds(StoreKeyHandle1, KOperationY, KStoreKey1Data);
	
	// OOM - test here to save regenerating keys and usages
	RunOomTest(ExtractKeyOomAlloc, ExtractKeyOomFree);
	
	TestDeleteSingleKey(StoreKeyHandle0, KErrNone);
	TestDeleteSingleKey(StoreKeyHandle1, KErrNone);
	
	DeleteAllKeys();
	}

