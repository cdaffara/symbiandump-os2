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
* Framework function for running hardware key store OOM tests.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


void RunOomTest(void (*aAllocFunc)(TRequestStatus&), void (*aFreeFunc)())
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
		__UHEAP_MARK;
		__KHEAP_MARK;
		
		__KHEAP_SETFAIL(RAllocator::EDeterministic, i);
		TRequestStatus rs;
		aAllocFunc(rs);
		User::WaitForRequest(rs);
		r = rs.Int();
		
		test(r == KErrNone || r == KErrNoMemory);
		if (r == KErrNone)
			aFreeFunc();
		
		__KHEAP_MARKEND;
		__UHEAP_MARKEND;
		
		__KHEAP_RESET;
		__UHEAP_RESET;
		}
	}

