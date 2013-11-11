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
* Tests KMS LDD API by generating, storing, and deleting keys;
* by adding and deleting usages; and by extracting keys from the
* hardware store.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


// ---- cleanup ----

void DeleteAllKeys()
/**
	The individual unit tests call this when they have finished to
	ensure all keys have been removed before the next test is run.
 */
	{	
	DeleteSingleKeySync(GenKeyHandle0);
	DeleteSingleKeySync(GenKeyHandle1);
	DeleteSingleKeySync(StoreKeyHandle0);
	DeleteSingleKeySync(StoreKeyHandle1);
	}

static void RunInMemoryMark(void (*aTestFunc)())
/**
	Call the supplied function, ensuring no memory is leaked from
	either the user side or kernel heaps.

	@param	aTestFunc		Function to call.
 */
	{
	__UHEAP_MARK;
	__KHEAP_MARK;
	aTestFunc();
	__KHEAP_MARKEND;
	__UHEAP_MARKEND;
	}

// ---- entrypoint ----

TInt E32Main()
/**
	Main entrypoint loads LDD and calls individual tests for opening and closing
	the channel; generating a key; storing a key; adding, deleting, and asking about
	a usage; and allocating resources in OOM.
 */
	{
	TInt r;

	test.Title();
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-MAIN-0001 E32Main "));

	RLoader l;
	test(l.Connect()==KErrNone);
	test(l.CancelLazyDllUnload()==KErrNone);
	l.Close();
	
	__UHEAP_MARK;
	__KHEAP_MARK;

	test.Next(_L("Loading KMS LDD"));
	r = User::LoadLogicalDevice(KKmsLddFileName);
	test(r == KErrNone);

	test.Next(_L("Loading KMS extract LDD"));
	r = User::LoadLogicalDevice(KKmsExtrLddFileName);
	test(r == KErrNone);

	RunInMemoryMark(TestOpenClose);
	
	r = KmsChannel.Open();
	test(r == KErrNone);
	r = ExtrChannel.Open();
	test(r == KErrNone);
	
	RunInMemoryMark(TestDup);
	RunInMemoryMark(TestGenerateKey);
	RunInMemoryMark(TestStoreKey);
	RunInMemoryMark(TestDeleteKey);
	RunInMemoryMark(TestUsage);
	RunInMemoryMark(TestExtraction);
	
	ExtrChannel.Close();
	KmsChannel.Close();
	
	test.Next(_L("closing KMS LDD"));
	r = User::FreeLogicalDevice(RKmsChannel::FactoryName());
	test(r == KErrNone);

	test.Next(_L("closing KMS extract LDD"));
	r = User::FreeLogicalDevice(RKmsExtrChannel::FactoryName());
	test(r == KErrNone);

	// device driver codeseg is not unloaded until the system goes idle
	// (See ~DLogicalDevice call to DCodeSeg::ScheduleKernelCleanup(TBool aImmed).)
	User::After(2 * 1000 * 1000);

	__KHEAP_MARKEND;
	__UHEAP_MARKEND;

	test.End();
	return KErrNone;
	}

