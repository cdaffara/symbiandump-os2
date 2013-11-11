/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* include this file to make ASSERT macros more useful
*
*/


#ifndef __ASSERTFILEANDLINE_H_
#define __ASSERTFILEANDLINE_H_

#ifdef _DEBUG
#ifdef ASSERT
#undef ASSERT
#endif
/**
@internalComponent
*/
#define __ASSERT_PANIC__(l) User::Panic(KPanicFileName().Right(12),l)
#define __ASSERT_FILE__(s) _LIT(KPanicFileName,s)
#define ASSERT(x) { __ASSERT_FILE__(__FILE__); __ASSERT_DEBUG(x, __ASSERT_PANIC__(__LINE__) ); }
#endif

#endif
