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
* Tests handle to KMS LDD channel can only be be duplicated as
* thread-relative and into the owning thread.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


static TInt DupIntoSecondProcess(TAny* aPtr)
/**
	Test duplicating the KMS channel handle into a second thread.
	
	@param	aPtr			Standard entrypoint argument.  This actually
							points to the calling thread's ID.
	@return					KErrNone.  This value has no meaning, and is
							only used to satisfy the TThreadFunction signature.
 */
	{
	TInt r;
	
	// test thread id
	TThreadId testThreadId = *reinterpret_cast<const TThreadId*>(aPtr);
	RThread testThread;
	r = testThread.Open(testThreadId);
	test(r == KErrNone);
	
	RKmsChannel dupCh;
	TInt h = KmsChannel.Handle();
	
	// duplicate into process-relative handle "in" second thread
	dupCh.SetHandle(h);
	r = dupCh.Duplicate(testThread, EOwnerProcess);
	test(r == KErrAccessDenied);
	
	// duplicate into thread-relative handle "in" second thread
	dupCh.SetHandle(h);
	r = dupCh.Duplicate(testThread, EOwnerThread);
	test(r == KErrAccessDenied);

	testThread.Close();	
	return KErrNone;
	}

void TestDup()
/**
	Test a KMS channel handle can only be duplicated by the thread
	which opened it.
 */
	{
	TInt r;
	
	RKmsChannel dupCh;
	TInt h = KmsChannel.Handle();
	RThread currentThread;
	
	// duplicate into process-relative handle "in" test thread
	dupCh.SetHandle(h);
	r = dupCh.Duplicate(currentThread, EOwnerProcess);
	test(r == KErrAccessDenied);
	
	// duplicate into thread-relative handle in test thread
	dupCh.SetHandle(h);
	r = dupCh.Duplicate(currentThread, EOwnerThread);
	test(r == KErrNone);
	dupCh.Close();
	
	TThreadId id = currentThread.Id();
	RThread secondThread = LaunchThreadAndWaitForRendezvous(DupIntoSecondProcess, &id);
	secondThread.Close();
	}

