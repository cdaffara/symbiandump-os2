/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CSTACKDEPRECATED_INL__
#define __CSTACKDEPRECATED_INL__

// constants
const TInt KStackGranularity=32;

enum TCStackPanic
	{
	ECStackErrStackIsEmpty,
	ECStackDeleteWhenNotOwner,
	ECStackPopsWhenOwner
	};

_LIT(KCStackPanicName, "CStackDeprecated");

inline void Panic(TCStackPanic aPanic)
	{
	User::Panic(KCStackPanicName, aPanic);
	}

//
// CStack
//

template <class T, TBool StackOwnsEntry>
inline CStackDeprecated<T, StackOwnsEntry>::CStackDeprecated()
: CArrayPtrSeg<T>(KStackGranularity)
	{
	this->Reset();
	}

template <class T, TBool StackOwnsEntry>
inline CStackDeprecated<T, StackOwnsEntry>::~CStackDeprecated()
	{ 
	Clear();
	}

template <class T, TBool StackOwnsEntry>
inline void CStackDeprecated<T, StackOwnsEntry>::Clear() 
	{ 
	if (StackOwnsEntry) 
		this->ResetAndDestroy(); 
	else 
		this->Reset(); 
	}

template <class T, TBool StackOwnsEntry>
inline TBool CStackDeprecated<T, StackOwnsEntry>::IsEmpty() const 
	{ 
	return this->Count()==0;
	}

template <class T, TBool StackOwnsEntry>
inline void CStackDeprecated<T, StackOwnsEntry>::PushL(T* aItem) 
	{
	if (StackOwnsEntry)
		CleanupStack::PushL(aItem); 
	this->AppendL(aItem); 
	if (StackOwnsEntry)
		CleanupStack::Pop();
	}

template <class T, TBool StackOwnsEntry>
inline T* CStackDeprecated<T, StackOwnsEntry>::Pop() 
	{
	__ASSERT_DEBUG(!IsEmpty(), Panic(ECStackErrStackIsEmpty));
	T* item=Head(); 
	this->Delete(this->Count()-1);
	return item;
	}

template <class T, TBool StackOwnsEntry>
inline T* CStackDeprecated<T, StackOwnsEntry>::Head() const 
	{
	__ASSERT_DEBUG(!IsEmpty(), Panic(ECStackErrStackIsEmpty));
	return this->At(this->Count()-1);
	}

template <class T, TBool StackOwnsEntry>
inline T* CStackDeprecated<T, StackOwnsEntry>::Last() const 
	{
	__ASSERT_DEBUG(!IsEmpty(), Panic(ECStackErrStackIsEmpty));
	return this->At(0);
	}
	

#endif // __CSTACKDEPRECATED_INL__

// End of File
