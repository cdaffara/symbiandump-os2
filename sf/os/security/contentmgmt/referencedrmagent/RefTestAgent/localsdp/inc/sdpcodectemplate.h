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
* Name          : SdpCodecTemplate.h
* Part of       : Local SDP Codec
* Interface     : -
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef SDPCODECTEMPLATE_H
#define SDPCODECTEMPLATE_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>

#include "_sdpdefs.h"
#include "e32std.h"
#include "stringpool.h"

class RReadStream;
class RWriteStream;

template<class T> class SdpCodecTemplate
	{
    public: 

		static void ExternalizeL(const T* aElement, RWriteStream& aWriteStream);
		static void ExternalizeArrayL(RPointerArray<T> aArray,RWriteStream& aStream);
		static void EncodeArrayL(RPointerArray<T> aArray, RWriteStream& aStream);
		static void EncodeL(const T* aElement, RWriteStream& aWriteStream);
		static void InternalizeArrayL(RPointerArray<T>& aArray, RReadStream& aStream);
		static void 
		CloneArrayL(RPointerArray<T>& aArray, RPointerArray<T> aSourceArray);

	};

#include "sdpcodectemplate.inl"

#endif
