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

// Template class TCapture
template <class T>
inline TCapture<T>::TCapture(T& aRef)
	: iPtr(&aRef)
	{}
template <class T>
inline T& TCapture<T>::Object() const
	{return *iPtr;}

// Class TDesHeader
inline TDesHeader::TDesHeader(const TDesC8& aDes8)
	: iVal((aDes8.Length()<<1)+1)
	{}
inline TDesHeader::TDesHeader(const TDesC16& aDes16)
	: iVal((aDes16.Length()<<1))
	{}
inline TDesHeader& TDesHeader::operator=(const TDesC8& aDes8)
	{return *this=TDesHeader(aDes8);}
inline TDesHeader& TDesHeader::operator=(const TDesC16& aDes16)
	{return *this=TDesHeader(aDes16);}
inline TBool TDesHeader::IsWidth8() const
	{return TInt(iVal)&0x1;}
inline TBool TDesHeader::IsWidth16() const
	{return !IsWidth8();}
inline TInt TDesHeader::Length() const
	{return TInt(iVal)>>1;}
inline void TDesHeader::ExternalizeL(RWriteStream& aStream) const
	{aStream<<iVal;}
inline void TDesHeader::InternalizeL(RReadStream& aStream)
	{aStream>>iVal;}

// Class TDesInternalizer
inline const TDesHeader& TDesInternalizer::Header() const
	{return iHeader;}
inline TDesHeader& TDesInternalizer::Header()
	{return iHeader;}

// Class TPtrInput
inline TPtrInput::TPtrInput(TAny* aPtr)
	: iPtr((TUint8*)aPtr)
	{}

// Class TPtrOutput
inline TPtrOutput::TPtrOutput(const TAny* aPtr)
	: iPtr((TUint8*)aPtr)
	{}

// Class HDirectStoreBuf
inline HDirectStoreBuf::~HDirectStoreBuf()
	{RShareBuf::DoRelease();}
inline HDirectStoreBuf::HDirectStoreBuf(TInt anOffset)
	: iOff(anOffset)
	{}

