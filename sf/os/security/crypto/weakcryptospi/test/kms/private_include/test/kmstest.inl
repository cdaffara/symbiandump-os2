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
* Inline function definitions for general utility functions which are used by all KMS test code.
*
*/




/**
 @file
*/

#ifndef KMSTEST_INL
#define KMSTEST_INL

#include "kmstest.h"

template <class T>
inline T TAnyPtrToFuncPtr(TAny* aPtr)
/**
	ISO C++ doesn't allow converting directly between object and
	function pointers, so this function goes via an integer.

	@param	aPtr			Object pointer to convert to a (static) function pointer.
	@return					Function pointer, where the exact type of the function is	
							described by the T template parameter.
 */
	{
	TUint32 funcAsInt = reinterpret_cast<TUint32>(aPtr);
	return reinterpret_cast<T>(funcAsInt);
	}


#endif

