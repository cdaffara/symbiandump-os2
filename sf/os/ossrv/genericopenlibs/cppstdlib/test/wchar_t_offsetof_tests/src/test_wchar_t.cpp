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
//

#include <stddef.h>
#include "test_decls.h"

template<class T>
class Example {
public:
	int Foo();
};

template<>
int Example<unsigned short>::Foo()
{
	return sizeof(unsigned short);
}

template<>
int Example<wchar_t>::Foo()
{
	return sizeof(wchar_t);
}

int main()
{
	Example<wchar_t> e1;
	Example<unsigned short> e2;
	CPP_TESTS_ASSERT_ALLWAYS(e1.Foo() == e2.Foo());
	return 0;
}
