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
// Name        : typeinfo_test.cpp
// Part of     : standard c++ tests.
// 
//

#include <e32cmn.h>
#include <typeinfo>
#include <new>
#include "test_decls.h"

class ClassBase
{
public:
	ClassBase() {}
};

class ClassA : public ClassBase
{
public:
	ClassA() {}
};

class ClassB : public ClassBase
{
	public:
	ClassB() {}
};
int numErrors = 0;

class ClassC{};
void foo(ClassA* aPtr1, ClassC* aPtr2)
	{
	if(typeid(*aPtr1) == typeid(*aPtr2))
		numErrors++;
	}
int E32Main()
{
	ClassBase *aPtr1, *aPtr2;
	aPtr1 = new ClassA();
	aPtr2 = new ClassB();

	if(typeid(*aPtr1) != typeid(*aPtr2))
		numErrors++;

	ClassA a;
	ClassC c;
	foo(&a, &c);

	CPP_TESTS_ASSERT_ALLWAYS(numErrors == 0);
	return 0;
}
