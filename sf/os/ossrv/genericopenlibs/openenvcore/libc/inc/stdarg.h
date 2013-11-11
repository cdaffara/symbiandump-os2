/* 
*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This is all somewhat tortured because ANSI C says that stdio.h is not supposed 
* to define va_list. stdarg_e.h defines something suitable for stdio.h, and
* this file sorts out va_list.
* 
*
*/



#ifndef _STDARG_H
#define _STDARG_H

#include <stdarg_e.h>

#ifndef __DOXYGEN__
#define va_list __e32_va_list
#endif //__DOXYGEN__

#endif
