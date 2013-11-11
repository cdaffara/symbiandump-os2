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

#ifndef _OP_NEW_H_
#define _OP_NEW_H_

#ifdef __WINSCW__
#include <e32cmn.h>
#endif
IMPORT_C void* operator new    (unsigned int) throw();

# if (defined (STDLIB_NONEW) || defined (STDLIB_NEW) || defined (LIB_NONEW_MACRO) || defined (LIB_NONEW_NOMACRO) || \
	defined (LIB_NEW_MACRO) || defined (LIB_NEW_NOMACRO) )
#  define BUILDING_ARCHIVE
# elif (defined (DLL_USING_STDLIB_NONEW) || defined (DLL_USING_STDLIB_NEW) || \
	defined (DLL_USING_LIB_NONEW_NOMACRO) || defined (DLL_USING_LIB_NEW_MACRO) || \
	defined (DLL_USING_LIB_NEW_NOMACRO) || defined (DLL_USING_LIB_NONEW_MACRO) ||\
	defined (STDDLL_USING_STDLIB_NONEW) || defined (STDDLL_USING_STDLIB_NEW) || \
	defined (STDDLL_USING_LIB_NONEW_NOMACRO) || defined (STDDLL_USING_LIB_NEW_MACRO) || \
	defined (STDDLL_USING_LIB_NEW_NOMACRO) || defined (STDDLL_USING_LIB_NONEW_MACRO) )
#  define BUILDING_DLL
# else
#  define BUILDING_EXE
# endif

# if !defined (BUILDING_ARCHIVE)
void *Foo();
# endif // BUILDING_ARCHIVE

#endif //_OP_NEW_H_

