/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* General utility functions which are used by all KMS test code.
*
*/




/**
 @file
 @internalComponent
*/

#ifndef KMSTEST_H
#define KMSTEST_H

/** GCCE emits warning if multicharacter constants are used directly. */
#define _MULTICHAR4(_c3,_c2,_c1,_c0)	((_c3 << 24) | (_c2 << 16) | (_c1 << 8) | _c0)
/** GCCE emits warning if multicharacter constants are used directly. */
#define MULTICHAR4(_s)				_MULTICHAR4(_s[0], _s[1], _s[2], _s[3])

template <class T>
inline T TAnyPtrToFuncPtr(TAny* aPtr);


#include "kmstest.inl"


#endif	// #ifndef KMSTEST_H

