// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : oper_new_test.cpp
// Part of     : standard c++ tests.
// 
//

#include <e32std.h>

#include <new>
#include "test_decls.h"

#define ONE_HUGE_VALUE	0xA00000

int glob_val = 0;

int* bar()
{
	User::__DbgSetBurstAllocFail(EFalse, RAllocator::EBurstFailNext, 1, KMaxTUint16);
	int *ptr = new int[ONE_HUGE_VALUE];
	return ptr;
}

int main()
{
		int *ptr;
	try {
		while( (ptr = bar()) != NULL) {
			ptr = bar();
		}
	}catch(std::bad_alloc ba) {
		glob_val = 1;
	}
	catch(...){
	}

	ptr = ptr;
	CPP_TESTS_ASSERT_ALLWAYS( glob_val == 1);

	return 0;
}
