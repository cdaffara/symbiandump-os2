/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


template <class T>
inline void DoExternalizeL(const T& anObject,RWriteStream& aStream,Externalize::Member)
	{anObject.ExternalizeL(aStream);}
template <class T>
inline void DoInternalizeL(T& anObject,RReadStream& aStream,Internalize::Member)
	{anObject.InternalizeL(aStream);}

template <class T>
inline void DoExternalizeL(const T& anObject,RWriteStream& aStream,Externalize::Function)
	{ExternalizeL(anObject,aStream);}
template <class T>
inline void DoInternalizeL(T& anObject,RReadStream& aStream,Internalize::Function)
	{InternalizeL(anObject,aStream);}

// Externalization and internalization selectors
inline Externalize::Member Externalization(const TAny*)
	{return Externalize::Member();}
inline Internalize::Member Internalization(TAny*)
	{return Internalize::Member();}

inline Externalize::Function Externalization(const TDesC8*)
	{return Externalize::Function();}
inline Externalize::Function Externalization(const TDesC16*)
	{return Externalize::Function();}
inline Internalize::Function Internalization(TDes8*)
	{return Internalize::Function();}
inline Internalize::Function Internalization(TDes16*)
	{return Internalize::Function();}

#if 0
inline Externalize::Function Externalization(const CBufBase*)
	{return Externalize::Function();}
inline Externalize::Function Externalization(const CArrayFixBase*)
	{return Externalize::Function();}
inline Internalize::Function Internalization(CBufBase*)
	{return Internalize::Function();}
inline Internalize::Function Internalization(CArrayFixBase*)
	{return Internalize::Function();}
#endif

template <class T>
inline Externalize::Function Externalization(T*const*)
	{return Externalize::Function();}

// Externalization and internalization functions
IMPORT_C void ExternalizeL(TInt64 anInt64,RWriteStream& aStream);
IMPORT_C void InternalizeL(TInt64& anInt64,RReadStream& aStream);

IMPORT_C void ExternalizeL(const TDesC8& aDes8,RWriteStream& aStream);
IMPORT_C void ExternalizeL(const TDesC16& aDes16,RWriteStream& aStream);
IMPORT_C void InternalizeL(TDes8& aDes8,RReadStream& aStream);
IMPORT_C void InternalizeL(TDes16& aDes16,RReadStream& aStream);

IMPORT_C void ExternalizeL(const TCheckedUid& aUid,RWriteStream& aStream);
IMPORT_C void InternalizeL(TCheckedUid& aUid,RReadStream& aStream);

//IMPORT_C void ExternalizeL(TPoint aPoint,RWriteStream& aStream);
IMPORT_C void ExternalizeL(TSize aSize,RWriteStream& aStream);
//IMPORT_C void ExternalizeL(const TRect& aRect,RWriteStream& aStream);
//IMPORT_C void InternalizeL(TPoint& aPoint,RReadStream& aStream);
IMPORT_C void InternalizeL(TSize& aSize,RReadStream& aStream);
//IMPORT_C void InternalizeL(TRect& aRect,RReadStream& aStream);

//IMPORT_C void ExternalizeL(const CBufBase& aBuf,RWriteStream& aStream);
//IMPORT_C void InternalizeL(CBufBase& aBuf,RReadStream& aStream);

IMPORT_C void ArrayExternalizeCountL(TInt aCount,RWriteStream& aStream);
//IMPORT_C void DoExternalizeAllL(const CArrayFixBase& anArray,RWriteStream& aStream,TExternalizer<TAny> anExter);
IMPORT_C TInt ArrayInternalizeCountL(RReadStream& aStream);
//IMPORT_C void DoInternalizeAllL(CArrayFixBase& anArray,RReadStream& aStream,TInternalizer<TAny> anInter);

// Input and output operators
template <class T>
inline RWriteStream& operator<<(RWriteStream& aStream,const T& anObject)
	{DoExternalizeL(anObject,aStream,Externalization(&anObject));return aStream;}
template <class T>
inline RReadStream& operator>>(RReadStream& aStream,T& anObject)
	{DoInternalizeL(anObject,aStream,Internalization(&anObject));return aStream;}

inline RWriteStream& operator<<(RWriteStream& aStream,const TInt8& anInt8)
	{aStream.WriteInt8L(anInt8);return aStream;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TInt16& anInt16)
	{aStream.WriteInt16L(anInt16);return aStream;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TInt32& anInt32)
	{aStream.WriteInt32L(anInt32);return aStream;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TUint8& aUint8)
	{aStream.WriteUint8L(aUint8);return aStream;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TUint16& aUint16)
	{aStream.WriteUint16L(aUint16);return aStream;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TUint32& aUint32)
	{aStream.WriteUint32L(aUint32);return aStream;}
//inline RWriteStream& operator<<(RWriteStream& aStream,const TReal32& aReal32)
//	{aStream.WriteReal32L(aReal32);return aStream;}
//inline RWriteStream& operator<<(RWriteStream& aStream,const TReal64& aReal64)
//	{aStream.WriteReal64L(aReal64);return aStream;}

inline RReadStream& operator>>(RReadStream& aStream,TInt8& anInt8)
	{anInt8=aStream.ReadInt8L();return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TInt16& anInt16)
	{anInt16=aStream.ReadInt16L();return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TInt32& anInt32)
	{anInt32=aStream.ReadInt32L();return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TUint8& aUint8)
	{aUint8=aStream.ReadUint8L();return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TUint16& aUint16)
	{aUint16=aStream.ReadUint16L();return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TUint32& aUint32)
	{aUint32=aStream.ReadUint32L();return aStream;}
//inline RReadStream& operator>>(RReadStream& aStream,TReal32& aReal32)
//	{aReal32=aStream.ReadReal32L();return aStream;}
//inline RReadStream& operator>>(RReadStream& aStream,TReal64& aReal64)
//	{aReal64=aStream.ReadReal64L();return aStream;}


//inline RWriteStream& operator<<(RWriteStream& aStream,const TInt64& anInt64)
//	{ExternalizeL(anInt64,aStream);return aStream;}
//inline RReadStream& operator>>(RReadStream& aStream,TInt64& anInt64)
//	{InternalizeL(anInt64,aStream);return aStream;}

inline RWriteStream& operator<<(RWriteStream& aStream,const TUid& aUid)
	{return aStream<<aUid.iUid;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TCheckedUid& aUid)
	{ExternalizeL(aUid,aStream);return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TUid& aUid)
	{return aStream>>aUid.iUid;}
inline RReadStream& operator>>(RReadStream& aStream,TCheckedUid& aUid)
	{InternalizeL(aUid,aStream);return aStream;}

//inline RWriteStream& operator<<(RWriteStream& aStream,const TPoint& aPoint)
//	{ExternalizeL(aPoint,aStream);return aStream;}
//inline RWriteStream& operator<<(RWriteStream& aStream,const TSize& aSize)
//	{ExternalizeL(aSize,aStream);return aStream;}
//inline RWriteStream& operator<<(RWriteStream& aStream,const TRect& aRect)
//	{ExternalizeL(aRect,aStream);return aStream;}
//inline RReadStream& operator>>(RReadStream& aStream,TPoint& aPoint)
//	{InternalizeL(aPoint,aStream);return aStream;}
//inline RReadStream& operator>>(RReadStream& aStream,TSize& aSize)
//	{InternalizeL(aSize,aStream);return aStream;}
//inline RReadStream& operator>>(RReadStream& aStream,TRect& aRect)
//	{InternalizeL(aRect,aStream);return aStream;}

//inline RWriteStream& operator<<(RWriteStream& aStream,const TStreamRef& aRef)
//	{aRef.ExternalizeL(aStream);return aStream;}

// Class TCardinality
inline TCardinality::TCardinality(TInt aCount)
	:iCount(aCount)
/** Constructs the object with the specified value.

@param aCount The value for this object. */
	{
#if defined (_DEBUG)
	__DbgChkRange(aCount);
#endif
	}
inline TCardinality::operator TInt() const
	{
#if defined (_DEBUG)
	__DbgChkRange(iCount);
#endif
	return iCount;
	}
// Class TDesInternalizer
inline const TDesHeader& TDesInternalizer::Header() const
	{return iHeader;}
inline TDesHeader& TDesInternalizer::Header()
	{return iHeader;}
