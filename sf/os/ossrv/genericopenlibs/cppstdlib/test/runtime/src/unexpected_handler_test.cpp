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
// Name        : unexpected_handler_test.cpp
// Part of     : standard c++ tests.
// 
//

#include <exception>
#include "test_decls.h"
#include <cstddef>
#include <cstdlib>

void foo()
{
/*
 * The handler is invoked so terminate
 **/
	CPP_TESTS_ASSERT_ALLWAYS(1);
	abort();
}

void bar() throw()
{
	throw int(0);
}

int main()
{
	std::unexpected_handler hu_old,hu_new;
//	std::terminate_handler ht_old,ht_new;
	
	hu_new = (std::unexpected_handler)&foo;
	hu_old = std::set_unexpected(hu_new);
	hu_old = hu_old;//get rid of the warning
	
//	ht_new = (std::terminate_handler)&foo;
//	ht_old = std::set_terminate(ht_new);
	try {
	bar();
	}catch(...){}
	/*
	 * Failure case. If we reach here, the unexpected_handler wasn't 
	 * invoked.
	 **/
	CPP_TESTS_ASSERT_ALLWAYS(0);
	return 0;
}
