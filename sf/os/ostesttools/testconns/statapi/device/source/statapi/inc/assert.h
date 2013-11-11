/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __ASSERT_H__
#define __ASSERT_H__

// asserte
// Extend assert to raise a panic that features the source file and 
// line number that caused the assert.
#include <e32std.h>
extern void AssertE( TPtrC8 _file, TInt _line );
#define asserte(x)	if(!(x)) { AssertE( (TPtrC8(reinterpret_cast<const TText8*>(__FILE__))), __LINE__); }

// assert
// Exit the program and show the message "Assert Failed".
#define assert(x)	__ASSERT_ALWAYS((x),User::Panic(_L("Assert Failed"),0))

#endif //__ASSERT_H__

