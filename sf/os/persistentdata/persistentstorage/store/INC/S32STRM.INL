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

// Class RReadStream
inline RReadStream::RReadStream()
	: iSrc(NULL)
/** Sets the stream buffer source to NULL, which implies that it is safe to close

@see RReadStream::Source() */
	{}
inline RReadStream::RReadStream(MStreamBuf* aSource)
	: iSrc(aSource)
/** Constructs a read stream over the supplied stream buffer.

@param aSource The stream buffer used to read from the read stream.
@see RReadStream::Source() */
	{}
inline MStreamBuf* RReadStream::Source()
/** Gets the underlying stream buffer for the read stream. This provides access 
to the stream-buffer implementation used by the read stream.

Note:

If the stream buffer supports seeking, this allows the RReadStream client 
to have random access to the stream data.

@return The underlying stream buffer for the read stream.
@see RReadStream::Attach()
@see RReadStream::Detach() */
	{return iSrc;}
inline void RReadStream::Close()
/** Frees resources used by the stream.

@see RReadStream::Release() */
	{Release();}
inline void RReadStream::ReadL(RWriteStream& aStream)
/**	Writes the content of this stream to the specified write stream.

@param aStream A reference to the write stream to which this stream is to be written.*/
	{aStream.WriteL(*this);}
inline void RReadStream::ReadL(RWriteStream& aStream,TInt aLength)
/**	Writes the content of this stream to the specified write stream.

@param aStream A reference to the write stream to which this stream is to be written.
@param aLength The length of data from this stream to be written to aStream.*/
	{aStream.WriteL(*this,aLength);}
inline void RReadStream::Pop()
/** Removes the cleanup item for this read stream object from the cleanup stack. */
	{CleanupStack::Pop();}
inline void RReadStream::Attach(MStreamBuf* aSource)
/** Sets the stream buffer source for this read stream. The existing source (if 
there is one) will be discarded.

This is useful for derived classes, enabling them to set the source following 
construction.

Note:

Release() is not called on any discarded stream buffer.

@param aSource The new source stream buffer for this read stream.
@see RReadStream::Source()
@see RReadStream::Detach() */
	{iSrc=aSource;}
inline void RReadStream::Detach()
/** Sets the stream buffer source for this read stream to NULL. The existing source, 
if there is one, will be discarded.

Note that Release() is not called on any discarded stream buffer.

@see RReadStream::Source()
@see RReadStream::Attach() */
	{iSrc=NULL;}

// Class RWriteStream
inline RWriteStream::RWriteStream()
	: iSnk(NULL),iExterL(NULL)
/** The stream buffer sink, as returned by Sink(), is set to NULL, and the stream 
has no associated Store map. */
	{}
inline RWriteStream::RWriteStream(const MExternalizer<TStreamRef>& anExter)
	: iSnk(NULL),iExterL(&anExter)
/** Constructs a write stream with an associated store map. The stream buffer sink 
is set to NULL. 

@param anExter Specifies an externaliser. For example, CStoreMap implements 
MExternalizer<TStreamRef>. */
	{}
inline RWriteStream::RWriteStream(MStreamBuf* aSink)
	: iSnk(aSink),iExterL(NULL)
/** Constructs a write stream over the supplied stream buffer. The stream has no 
associated Store map.

@param aSink The stream buffer used to write to the write stream. */
	{}
inline MStreamBuf* RWriteStream::Sink()
/** Gets the underlying stream buffer for the write stream. This provides access 
to the stream-buffer implementation used by the write stream.

@return The underlying stream buffer for the write stream. */
	{return iSnk;}
inline void RWriteStream::Pop()
/** Removes the cleanup item for this write stream object from the cleanup stack. */
	{CleanupStack::Pop();}
inline void RWriteStream::Attach(MStreamBuf* aSink)
/** Sets the stream buffer sink for this write stream. The existing sink, if there 
is one, will be discarded.

This is useful for derived classes, enabling them to set the sink following 
construction.

Note:

Release() is not called for discarded sinks.

@param aSink The stream buffer sink for this write stream. */
	{iSnk=aSink;}
inline void RWriteStream::Detach()
/** Sets the stream buffer sink for this write stream to NULL. The existing sink, 
if there is one, will be discarded.

Note:

Release() is not called for discarded sinks. */
	{iSnk=NULL;}

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

// Template class TExternalizer
template <class T>
inline void TExternalizer<T>::operator()(const T& anObject,RWriteStream& aStream) const
	{aStream<<anObject;}
template <class T>
inline TExternalizeFunction TExternalizer<T>::Function()
/** Gets a pointer to the function to be used to perform externalisation.

The implementation is the private, static member of this class, ExternalizeAsL().

@return The externalisation function. */
	{return &ExternalizeAsL;}
template <class T>
void TExternalizer<T>::ExternalizeAsL(const TAny* aPtr,RWriteStream& aStream)
	{aStream<<*((const T*)aPtr);}
inline TExternalizer<TAny>::TExternalizer(TExternalizeFunction aFunction)
	: iFunc(aFunction)
	{}
inline void TExternalizer<TAny>::operator()(const TAny* aPtr,RWriteStream& aStream) const
	{(*iFunc)(aPtr,aStream);}
inline TExternalizeFunction TExternalizer<TAny>::Function() const
	{return iFunc;}

// Template class TInternalizer
template <class T>
inline void TInternalizer<T>::operator()(T& anObject,RReadStream& aStream) const
	{aStream>>anObject;}
template <class T>
inline TInternalizeFunction TInternalizer<T>::Function()
/** Gets a pointer to the function to be used to perform internalisation.

The implementation is the private, static member of this class, InternalizeAsL().

@return The internalisation function. */
	{return &InternalizeAsL;}
template <class T>
void TInternalizer<T>::InternalizeAsL(TAny* aPtr,RReadStream& aStream)
	{aStream>>*((T*)aPtr);}
inline TInternalizer<TAny>::TInternalizer(TInternalizeFunction aFunction)
	: iFunc(aFunction)
	{}
inline void TInternalizer<TAny>::operator()(TAny* aPtr,RReadStream& aStream) const
	{(*iFunc)(aPtr,aStream);}
inline TInternalizeFunction TInternalizer<TAny>::Function() const
	{return iFunc;}

// Template class MExternalizer
template <class T>
inline void MExternalizer<T>::operator()(const T& anObject,RWriteStream& aStream) const
	{ExternalizeL(anObject,aStream);}

// Template class MInternalizer
template <class T>
inline void MInternalizer<T>::operator()(T& anObject,RReadStream& aStream) const
	{InternalizeL(anObject,aStream);}

// Class TStreamRef
inline TStreamRef::TStreamRef(const TAny* aPtr,TExternalizeFunction aFunction)
	: iPtr(aPtr),iFunc(aFunction)
/** Constructor.

@param aPtr A pointer to the object for which externalisation is being done.
@param aFunction The externalisation function. */
	{}
inline const TAny* TStreamRef::Ptr() const
/** Gets the pointer to the object to be externalised.

@return The object to be externalised. */
	{return iPtr;}
inline TExternalizeFunction TStreamRef::Function() const
/** Gets a pointer to the externalisation function.

@return The externalisation function. */
	{return iFunc;}
inline void TStreamRef::ExternalizeL(RWriteStream& aStream) const
/** Externalises the object to the specified write stream.

@param aStream The write stream. */
	{aStream.WriteRefL(*this);}

// Externalization and internalization functions
IMPORT_C void ExternalizeL(TInt64 anInt64,RWriteStream& aStream);
IMPORT_C void InternalizeL(TInt64& anInt64,RReadStream& aStream);

IMPORT_C void ExternalizeL(const TDesC8& aDes8,RWriteStream& aStream);
IMPORT_C void ExternalizeL(const TDesC16& aDes16,RWriteStream& aStream);
IMPORT_C void InternalizeL(TDes8& aDes8,RReadStream& aStream);
IMPORT_C void InternalizeL(TDes16& aDes16,RReadStream& aStream);

IMPORT_C void ExternalizeL(const TCheckedUid& aUid,RWriteStream& aStream);
IMPORT_C void InternalizeL(TCheckedUid& aUid,RReadStream& aStream);

IMPORT_C void ExternalizeL(TPoint aPoint,RWriteStream& aStream);
IMPORT_C void ExternalizeL(TSize aSize,RWriteStream& aStream);
IMPORT_C void ExternalizeL(const TRect& aRect,RWriteStream& aStream);
IMPORT_C void InternalizeL(TPoint& aPoint,RReadStream& aStream);
IMPORT_C void InternalizeL(TSize& aSize,RReadStream& aStream);
IMPORT_C void InternalizeL(TRect& aRect,RReadStream& aStream);

IMPORT_C void ExternalizeL(const CBufBase& aBuf,RWriteStream& aStream);
IMPORT_C void InternalizeL(CBufBase& aBuf,RReadStream& aStream);

IMPORT_C void ArrayExternalizeCountL(TInt aCount,RWriteStream& aStream);
IMPORT_C void DoExternalizeAllL(const CArrayFixBase& anArray,RWriteStream& aStream,TExternalizer<TAny> anExter);
IMPORT_C TInt ArrayInternalizeCountL(RReadStream& aStream);
IMPORT_C void DoInternalizeAllL(CArrayFixBase& anArray,RReadStream& aStream,TInternalizer<TAny> anInter);

template <class T>
inline void ExternalizeCountL(const CArrayFix<T>& anArray,RWriteStream& aStream)
	{ArrayExternalizeCountL(anArray.Count(),aStream);}
template <class T>
inline void ExternalizeAllL(const CArrayFix<T>& anArray,RWriteStream& aStream)
	{DoExternalizeAllL(anArray,aStream,TExternalizer<T>::Function());}
template <class T>
void InternalizeCountL(CArrayFix<T>& anArray,RReadStream& aStream)
	{
	TInt n=ArrayInternalizeCountL(aStream);
	anArray.ResizeL(n);
	}
template <class T>
inline void InternalizeAllL(CArrayFix<T>& anArray,RReadStream& aStream)
	{DoInternalizeAllL(anArray,aStream,TInternalizer<T>::Function());}

template <class T>
void ExternalizeL(const CArrayFix<T>& anArray,RWriteStream& aStream)
	{
	ExternalizeCountL(anArray,aStream);
	ExternalizeAllL(anArray,aStream);
	}
template <class T>
void InternalizeL(CArrayFix<T>& anArray,RReadStream& aStream)
	{
	InternalizeCountL(anArray,aStream);
	InternalizeAllL(anArray,aStream);
	}

template <class T>
inline void ExternalizeL(const T* aPtr,RWriteStream& aStream)
	{aStream<<TStreamRef(aPtr,TExternalizer<T>::Function());}

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

inline Externalize::Function Externalization(const CBufBase*)
	{return Externalize::Function();}
inline Externalize::Function Externalization(const CArrayFixBase*)
	{return Externalize::Function();}
inline Internalize::Function Internalization(CBufBase*)
	{return Internalize::Function();}
inline Internalize::Function Internalization(CArrayFixBase*)
	{return Internalize::Function();}

template <class T>
inline Externalize::Function Externalization(T*const*)
	{return Externalize::Function();}

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
inline RWriteStream& operator<<(RWriteStream& aStream,const TReal32& aReal32)
	{aStream.WriteReal32L(aReal32);return aStream;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TReal64& aReal64)
	{aStream.WriteReal64L(aReal64);return aStream;}

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
inline RReadStream& operator>>(RReadStream& aStream,TReal32& aReal32)
	{aReal32=aStream.ReadReal32L();return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TReal64& aReal64)
	{aReal64=aStream.ReadReal64L();return aStream;}

inline RWriteStream& operator<<(RWriteStream& aStream,const TInt64& anInt64)
	{ExternalizeL(anInt64,aStream);return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TInt64& anInt64)
	{InternalizeL(anInt64,aStream);return aStream;}

inline RWriteStream& operator<<(RWriteStream& aStream,const TUid& aUid)
	{return aStream<<aUid.iUid;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TCheckedUid& aUid)
	{ExternalizeL(aUid,aStream);return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TUid& aUid)
	{return aStream>>aUid.iUid;}
inline RReadStream& operator>>(RReadStream& aStream,TCheckedUid& aUid)
	{InternalizeL(aUid,aStream);return aStream;}

inline RWriteStream& operator<<(RWriteStream& aStream,const TPoint& aPoint)
	{ExternalizeL(aPoint,aStream);return aStream;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TSize& aSize)
	{ExternalizeL(aSize,aStream);return aStream;}
inline RWriteStream& operator<<(RWriteStream& aStream,const TRect& aRect)
	{ExternalizeL(aRect,aStream);return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TPoint& aPoint)
	{InternalizeL(aPoint,aStream);return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TSize& aSize)
	{InternalizeL(aSize,aStream);return aStream;}
inline RReadStream& operator>>(RReadStream& aStream,TRect& aRect)
	{InternalizeL(aRect,aStream);return aStream;}

inline RWriteStream& operator<<(RWriteStream& aStream,const TStreamRef& aRef)
	{aRef.ExternalizeL(aStream);return aStream;}

