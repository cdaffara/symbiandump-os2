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

#include <e32cmn.h>

class Aclass
{
};

#ifdef __SYMBIAN_STDCPP_SUPPORT__
int foo(int i)
{
	int *ptr = new int(i); //void* operator new    (unsigned int) throw(std::bad_alloc);
	if(!ptr)
		return i;

	Aclass *a_ptr1 = new(ptr) Aclass(); //inline void* operator new(unsigned int, void* p) throw()

	delete a_ptr1;//inline void operator delete(void*, void*) throw()

	delete ptr;		//void  operator delete (void*)       throw();

	int *a_ptr = new int[5]; //void* operator new[]    (unsigned int) throw(std::bad_alloc);
	if(!a_ptr)
		return i;

	delete[] a_ptr;		//void  operator delete[] (void*)       throw();

	return 0;

}

#else

int foo(int /*i*/)
{
	return 0;
}
#endif
