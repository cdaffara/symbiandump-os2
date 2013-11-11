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
// Name        : terminate_handler_test.cpp
// Part of     : standard c++ tests.
// 
//

#include <exception>
#include "test_decls.h"
#include <cstddef>
#include <cstdlib>

int glob_val = 0;

class UserClass{
public:
	~UserClass() {
		throw int (0); //  This causes std::terminate to be called since the 
						// destructor is throwing while someone is unwinding the stack.
						// Since we ahve the terminate_handler set, my_handler must get called.
	}
};


void foo() {
	UserClass uc;
	throw int (1); // causes stack unwinding - hence calls ~UserClass()
}


void bar() {
	try {
	foo();
	}catch(...){}
}

void my_handler()
{
	glob_val = 1;
	CPP_TESTS_ASSERT_ALLWAYS(1);
	abort();
}

int main()
{
	std::terminate_handler h_old, h_new;
	h_new = (std::terminate_handler)&my_handler;
	h_old = std::set_terminate(h_new);
	h_old = h_old;
	bar();

	//Failed to call terminate handler
	CPP_TESTS_ASSERT_ALLWAYS(0);

	return 0;
}
