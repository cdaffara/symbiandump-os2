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
// Name        : bad_typeid_cast.cpp
// Part of     : standard c++ tests.
// 
//

#include <exception>
#include <typeinfo>
#include "test_decls.h"
#include <cstddef>
#include <iostream>

#ifdef __WINSCW__
#include <libstdcppwsd.h> 
#endif

class myclass{
virtual ~myclass(){}
};

int main()
{
	myclass *ptr = NULL;
	const char *name=NULL;

	try {
		name = (typeid(*ptr)).name();
	}catch(std::bad_typeid bt) {
		CPP_TESTS_ASSERT_ALLWAYS(1);
		return 0;
	}
	catch(...){
	CPP_TESTS_ASSERT_ALLWAYS(0);
	}

	CPP_TESTS_ASSERT_ALLWAYS(0);

	return 0;
}
