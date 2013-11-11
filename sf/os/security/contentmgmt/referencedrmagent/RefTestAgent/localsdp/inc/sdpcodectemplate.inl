/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SdpCodecTemplate.inl
* Part of       : SDP Codec
* Interface     : -
* Version       : 1.0
*
*/



#ifndef SDPCODECTEMPLATE_INL
#define SDPCODECTEMPLATE_INL

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>

#include "_sdpdefs.h"
#include "e32std.h"

class RReadStream;
class RWriteStream;

// ---------------------------------------------------------------------------
// SdpCodecTemplate<T>::ExternalizeL 
// ---------------------------------------------------------------------------
//
template<class T> void SdpCodecTemplate<T>::ExternalizeL(const T* aElement,
										RWriteStream& aWriteStream)
	{
	if(aElement)
		{
		aWriteStream.WriteUint8L (1);
		aElement->ExternalizeL(aWriteStream);
		}
	else
		{
		aWriteStream.WriteUint8L (0);
		}
	}

// ---------------------------------------------------------------------------
// SdpCodecTemplate<T>::ExternalizeArrayL
// ---------------------------------------------------------------------------
//
template<class T> void 
SdpCodecTemplate<T>::ExternalizeArrayL(RPointerArray<T> aArray,
						   RWriteStream& aStream)
	{
	aStream.WriteUint32L (aArray.Count());
	for (TInt i = 0;i < aArray.Count();i++)
		{
		aArray[i]->ExternalizeL(aStream);
		}
	}

// ---------------------------------------------------------------------------
// SdpCodecTemplate<T>::EncodeArrayL
// ---------------------------------------------------------------------------
//
template<class T> void 
SdpCodecTemplate<T>::EncodeArrayL(RPointerArray<T> aArray, RWriteStream& aStream)
	{
	for (TInt i = 0;i < aArray.Count();i++)
		{
		aArray[i]->EncodeL(aStream);
		}
	}

// ---------------------------------------------------------------------------
// SdpCodecTemplate<T>::EncodeL
// ---------------------------------------------------------------------------
//
template<class T> void SdpCodecTemplate<T>::EncodeL(const T* aElement,
										RWriteStream& aWriteStream)
	{
	if(aElement)
		{
		aElement->EncodeL(aWriteStream);
		}
	}

// ---------------------------------------------------------------------------
// SdpCodecTemplate<T>::InternalizeArrayL
// ---------------------------------------------------------------------------
//
template<class T> void 
SdpCodecTemplate<T>::InternalizeArrayL(RPointerArray<T>& aArray, 
								RReadStream& aStream)
	{
	TInt count = aStream.ReadUint32L();
	for (TInt i = 0;i < count;i++)
		{
		T* obj = T::InternalizeL(aStream);
		CleanupStack::PushL(obj);
		User::LeaveIfError(aArray.Append(obj));
		CleanupStack::Pop();//obj
		}
	}

// ---------------------------------------------------------------------------
// SdpCodecTemplate<T>::CloneArrayL
// ---------------------------------------------------------------------------
//
template<class T> void 
SdpCodecTemplate<T>::CloneArrayL(RPointerArray<T>& aArray,
						  RPointerArray<T> aSourceArray)
	{
	for (TInt i=0; i < (aSourceArray).Count(); i++)
		{
		T* obj = aSourceArray[i]->CloneL();
		CleanupStack::PushL(obj);
		User::LeaveIfError(aArray.Append(obj));
		CleanupStack::Pop();//obj
		}
	}


#endif
