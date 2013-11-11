// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "UT_STD.H"

EXPORT_C TStreamId TSwizzleCBase::AsId() const
/** Gets the streamid of the represented object.

This swizzle must currently represent the object as a stream id, otherwise 
the function raises a STORE-Store 3 panic.

@return The stream id of the represented object */
	{
	__ASSERT_ALWAYS(IsId(),Panic(EStoreSwizzleBadId));
	return iPtr==NULL?KNullStreamId:TStreamId((TUint32)iPtr>>1); // implementation dependency
	}

EXPORT_C void TSwizzleCBase::InternalizeL(RReadStream& aStream)
/** Internalises a stream id from the read stream, constructs a swizzle from this 
stream id and copies the swizzle to this swizzle.

The presence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.

@param aStream Stream from which the stream id should be internalised */
	{
	TStreamId id;
	aStream>>id;
	*this=TSwizzleCBase(id);
	}

EXPORT_C TSwizzleCBase::TSwizzleCBase(TStreamId anId)
//
// Construct from a stream id.
//
	: iPtr(anId==KNullStreamId?NULL:(TAny*)((anId.Value()<<1)|0x1))
	{}

EXPORT_C void TSwizzleCBase::DoExternalizeL(RWriteStream& aStream,TExternalizer<TAny> anExter) const
//
// Write this swizzle to aStream as an out-of-stream reference.
//
	{
	aStream<<TStreamRef(iPtr,(IsPtr()?anExter.Function():NULL));
	}

EXPORT_C TBool TSwizzleCBase::IsPtrRep(const TAny* aPtr)
//
// Test whether aPtr is a valid representation for a pointer.
//
	{
	return !((TUint32)aPtr&0x3); // implementation dependency
	}

EXPORT_C TBool TSwizzleCBase::IsIdRep(const TAny* aPtr)
//
// Test whether aPtr is a valid representation for a stream id.
//
	{
	return aPtr==NULL||(TUint32)aPtr&0x1; // implementation dependency
	}

EXPORT_C void TSwizzleCBase::__DbgChkPtr(const TAny* aPtr)
//
// Check for invalid pointers.
//
	{
	__ASSERT_ALWAYS(IsPtrRep(aPtr),Panic(EStoreSwizzleBadPtr));
	}

EXPORT_C void TSwizzleCBase::__DbgChkRef(TStreamRef aRef)
//
// Check for an invalid out-of-stream reference.
//
	{
	__ASSERT_ALWAYS((IsPtrRep(aRef.Ptr())&&aRef.Function()!=NULL)||(IsIdRep(aRef.Ptr())&&aRef.Function()==NULL),Panic(EStoreSwizzleBadReference));
	}

