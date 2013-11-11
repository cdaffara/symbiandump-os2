/*
* Copyright (c) 2000-2007 Nokia Corporation and/or its subsidiary(-ies).
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

// CStackDeprecated class was part of Symbian enviroment(as a CStack.h), but now it is deprecated. 

#ifndef __CSTACKDEPRECATED_H__
#define __CSTACKDEPRECATED_H__

#include <e32base.h>

#pragma warning (disable : 4127) // conditional expression is constant


/**	
	@file
	@publishedAll
	@released
*/


template <class T, TBool StackOwnsEntry>
class CStackDeprecated : /*protected CArrayPtrSeg<T>*/ public CArrayPtrSeg<T>
	{
public:
	inline CStackDeprecated();
	inline ~CStackDeprecated();

	inline void Clear();		
	inline TBool IsEmpty() const;
	inline void PushL(T* aItem);
	inline T* Pop();
	inline T* Head() const;
	inline T* Last() const;		
	};

#include "cstackdeprecated.inl"

#pragma warning ( default : 4127 ) // conditional expression is constant

#endif // __CSTACK_HDEPRECATED__

// End of File
