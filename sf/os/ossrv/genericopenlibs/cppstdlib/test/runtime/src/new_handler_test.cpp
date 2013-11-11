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
// Name        : new_handler_test.cpp
// Part of     : standard c++ tests.
// 
//

#include <new>
#include "test_decls.h"

int one_huge_chunk = 0xa000;
int *last_huge_chunk=NULL;

int glob_val = 0;
void foo()
{
	/*
	 * This is the new handler and it frees the last successful alloc so
	 * that the allocation in operator new has some free memory.
	 */
	glob_val++;
	delete [] last_huge_chunk;

}

void bar()
{
	last_huge_chunk	= new int[one_huge_chunk];
}

int main()
{
	std::new_handler h_old, h_new;

	try{
		while(1){
			// Keep allocating until we reach OOM condition. At OOM
			// the default new handler throws std::bad_alloc
			bar();
		}
	}catch(std::bad_alloc ba)
		{
		}
	
	/*
	 * Once the handler is set, failure of 'new' will call the handler to
	 * get some free memory... 
	 */
	h_new = (std::new_handler)&foo;

	h_old = std::set_new_handler(h_new);

	try {
	/*
	 * Try once more to see if our handler actually freed up  some memory
	 */
		bar();
	}catch(...){
		glob_val = 0;
	}
	
	CPP_TESTS_ASSERT_ALLWAYS(glob_val != 0);

	h_new = std::set_new_handler(h_old);
	return 0;
}
