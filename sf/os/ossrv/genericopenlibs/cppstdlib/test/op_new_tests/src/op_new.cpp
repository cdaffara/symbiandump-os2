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

#include "op_new.h"

#if defined (BUILDING_ARCHIVE)
void* Foo()
{
# if (defined (STDLIB_NONEW) || defined (LIB_NONEW_MACRO) || defined (LIB_NONEW_NOMACRO) )
	
	return 0;
# else
	int *ptr = new int;
	return ptr;
# endif

}

#elif defined (BUILDING_DLL)
  #if defined (STDDLL_USING_STDLIB_NONEW) || defined (STDDLL_USING_STDLIB_NEW) \
    || defined (DLL_USING_STDLIB_NEW) || defined (DLL_USING_STDLIB_NONEW)
    IMPORT_C void* Foo();
  #endif
EXPORT_C void* FooBar()
{
	return Foo();
}
#else

# if defined (STDEXE)
int main()
# else
int E32Main()
# endif
{
	int *p = (int*)Foo();
	if(p)
		return 0;

	return 1;
}

#endif
