/*
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
*
*/

#ifndef __ACMUTILS_H__
#define __ACMUTILS_H__

#include <e32std.h>

// Macros to save a bit of ROM space in release builds- the checking form of 
// Pop only really needs to be run once anyway.
#ifdef _DEBUG
#define CLEANUPSTACK_POP(aaa)	CleanupStack::Pop(aaa)
#else
#define CLEANUPSTACK_POP(aaa)	CleanupStack::Pop()
#endif

#endif // __ACMUTILS_H__
