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
// Name        : uncaught_exception_test.cpp
// Part of     : standard c++ tests.
// 
//

#include <exception>
#include "test_decls.h"
#include <cstddef>

int glob_val = 0;
class UserClass{
public:
	~UserClass(){
		// This object is destroyed before the exception is caught.
		bool uc = std::uncaught_exception();
		if(uc == true)
			glob_val = 1;
	}
};

void foo() {

	UserClass uc;
	throw int (0);
}
int main()
{
	try{
	foo();
	}catch(...){
	}
	CPP_TESTS_ASSERT_ALLWAYS(glob_val == 1);
	return 0;
}

