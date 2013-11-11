/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : staticlibinit_gcce.h
* Part of     : libc library
* Used to remove the static library linking problem on gcce platform
* Version     : 1.0
*
*/



#ifndef STATIC_LIB_INIT_GCCE_H
#define STATIC_LIB_INIT_GCCE_H

#include <e32def.h>
#pragma once

/*
 *It'a dummy method used to fix GCCE toolchain problem for static libraries.
 */
#ifdef __GCCE__
extern "C" GLDEF_C int __GccGlueInit();
void _GcceInit()
   {
   __GccGlueInit();
   }
#endif // __GCCE__

#endif //STATIC_LIB_INIT_GCCE_H

// End of file