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
// Name        : unexpected_bad_exc_test.cpp
// Part of     : standard c++ tests.
// 
//

#include <exception>
#include "test_decls.h"
#include <cstddef>

class AnExc{};

int global_val = 0;

void foo()
{
/*
 * The custom handler rethrowing std::bad_exception
 */
	throw;
}

void bar() throw(std::bad_exception,AnExc)
{
/*
 * Throwing an exception other than those in the exception-specification 
 * of this function.
 */
	throw int(0);
}


int main()
{
	std::unexpected_handler h_old,h_new;
	
	h_new = (std::unexpected_handler)&foo;
	h_old = std::set_unexpected(h_new);

	try {
		bar();
	}
	catch(std::bad_exception b) {
		global_val = 1;
	}
	catch(...){
	}
	h_new = std::set_unexpected(h_old);
	
	CPP_TESTS_ASSERT_ALLWAYS(global_val == 1);
	return 0;
}
