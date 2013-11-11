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

#if !defined(__S32STD_H__)
#define __S32STD_H__
#if !defined(__S32STRM_H__)
#include <s32strm.h>
#endif

/** The value of the null stream ID. */
const TUint32 KNullStreamIdValue=0;
const TUint32 KMaxStreamIdValue=0xfffffff;
const TUint32 KMaskStreamIdValue=0xfffffff;
const TInt KShiftStreamIdValue=28;

/**
 * @publishedAll 
 * @released
 * Provides unique identification for stream within a store.

A store always assigns a new id and constructs and returns an associated TStreamId 
object when a new stream is created.

@see RStoreWriteStream::CreateL()
@see RStoreWriteStream::CreateLC()  
*/
class TStreamId
	{
public:
	/** Constructs an uninitialised object. It is necessary because there are also 
	non-default constructors in this class. */
	TStreamId() {}
	inline TStreamId(TUint32 aValue);
//
	inline TBool operator==(TStreamId anId) const;
	inline TBool operator!=(TStreamId anId) const;
//
	inline void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
//
	inline TUint32 Value() const;
private:
	TUint32 iVal;
private:
	IMPORT_C static void __DbgChkRange(TUint32 aValue);
	};
#if defined(__NO_CLASS_CONSTS__)
#define KNullStreamId TStreamId(KNullStreamIdValue)
#else
/** The null stream ID; this is a stream ID which is guaranteed not to exist. */
const TStreamId KNullStreamId=TStreamId(KNullStreamIdValue);
#endif
//
class CStreamStore;

/**
 * @publishedAll 
 * @released
 * Supports the opening and manipulation of an existing stream in a store.  
*/
class RStoreReadStream : public RReadStream
	{
public:
	IMPORT_C void OpenL(const CStreamStore& aStore,TStreamId anId);
	IMPORT_C void OpenLC(const CStreamStore& aStore,TStreamId anId);
	};

/**
 * @publishedAll 
 * @released
 * Supports the writing of a stream to a store.

The class allows:

creation of a new stream

overwriting of an existing stream

replacement of an existing stream

appending to an existing stream 
*/
class RStoreWriteStream : public RWriteStream
	{
public:
	/** Constructs an uninitialised object. It is necessary because there are also 
	non-default constructors in this class. */
	RStoreWriteStream() {}
	inline RStoreWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C TStreamId CreateL(CStreamStore& aStore);
	IMPORT_C TStreamId CreateLC(CStreamStore& aStore);
	IMPORT_C void OpenL(CStreamStore& aStore,TStreamId anId);
	IMPORT_C void OpenLC(CStreamStore& aStore,TStreamId anId);
	IMPORT_C void ReplaceL(CStreamStore& aStore,TStreamId anId);
	IMPORT_C void ReplaceLC(CStreamStore& aStore,TStreamId anId);
	IMPORT_C void AppendL(CStreamStore& aStore,TStreamId anId);
	IMPORT_C void AppendLC(CStreamStore& aStore,TStreamId anId);
	};

/**
 * @publishedAll 
 * @released
 * Base class for swizzles.

A swizzle maintains a dual representation for an object:

by stream id, if the object is not in memory (the stream contains the external 
representation of that object).

by pointer, if the object is in memory.

The class is not intended for instantiation. 
*/
class TSwizzleCBase
	{
public:
	inline TBool operator==(const TSwizzleCBase& aSwizzle) const;
	inline TBool operator==(const TAny* aPtr) const;
	inline TBool operator!=(const TSwizzleCBase& aSwizzle) const;
	inline TBool operator!=(const TAny* aPtr) const;
//
	inline TBool IsPtr() const;
	inline TBool IsId() const;
	IMPORT_C TStreamId AsId() const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
protected:
	TSwizzleCBase() {}
	inline TSwizzleCBase(const TAny* aPtr);
	IMPORT_C TSwizzleCBase(TStreamId anId);
	inline TSwizzleCBase(TStreamRef aRef);
	inline const TAny* Ptr() const;
	IMPORT_C void DoExternalizeL(RWriteStream& aStream,TExternalizer<TAny> anExter) const;
private:
	IMPORT_C static TBool IsPtrRep(const TAny* aPtr);
	IMPORT_C static TBool IsIdRep(const TAny* aPtr);
private:
	const TAny* iPtr;
private:
	IMPORT_C static void __DbgChkPtr(const TAny* aPtr);
	IMPORT_C static void __DbgChkRef(TStreamRef aRef);
	};
inline TBool operator==(const TAny* aPtr,const TSwizzleCBase& aSwizzle);
inline TBool operator!=(const TAny* aPtr,const TSwizzleCBase& aSwizzle);

/**
 * @publishedAll 
 * @released
 * Implementation class for swizzles.

Although part of the class hierarchy, no function or data members in this 
class form part of the public application programming interface.

The class is not intended for instantiation.

@see TSwizzle  
*/
class TSwizzleBase : public TSwizzleCBase
	{
protected:
	TSwizzleBase() {}
	inline TSwizzleBase(TAny* aPtr);
	inline TSwizzleBase(TStreamId anId);
	inline TAny* Ptr() const;
	};

/**
 * @publishedAll 
 * @released
 * Maintains a dual representation for an object. The representation is:

by stream id, if the object is not in memory (the stream contains the external 
representation of that object).

by pointer, if the object is in memory.

The template class defines the type of object for which the swizzle is a representation. 
Full access to the represented object is available through the swizzle.

Maintaining a dual representation for an object allows the loading of objects 
into memory from a store to be deferred; this is particularly important in 
complex applications. 
*/
template <class T>
class TSwizzle : public TSwizzleBase
	{
public:
	/** Constructs an uninitialised swizzle. It is necessary because there are also 
	non-default constructors in this class. */
	TSwizzle() {}
	inline TSwizzle(T* aPtr);
	inline TSwizzle(TStreamId anId);
	inline TSwizzle<T>& operator=(T* aPtr);
	inline T* AsPtr() const;
	inline operator T*() const;
	inline T& operator*() const;
	inline T* operator->() const;
	inline void ExternalizeL(RWriteStream& aStream) const;
	};

/**
 * @publishedAll 
 * @released
 * A specific instantiation of the family of TSwizzle<class T> classes that maintains 
the representation of an untyped object as a non-const pointer or as a stream 
id.  
*/
TEMPLATE_SPECIALIZATION class TSwizzle<TAny> : public TSwizzleBase
	{
public:
	/** Default constructor.
	
	Constructs an uninitialised swizzle. */
	TSwizzle() {}
	inline TSwizzle(TAny* aPtr);
	inline TSwizzle(TStreamId anId);
	inline TSwizzle(const TSwizzleBase& aSwizzle);
	inline TSwizzle<TAny>& operator=(TAny* aPtr);
	inline TSwizzle<TAny>& operator=(const TSwizzleBase& aSwizzle);
	inline TAny* AsPtr() const;
	inline operator TAny*() const;
	};

/**
 * @publishedAll 
 * @released
 * Maintains a dual representation for a constant object. The representation is:

by stream id, if the object is not in memory (the stream contains the external 
representation of that object).

by pointer, if the object is in memory.

The template class defines the type of object for which the swizzle is a representation. 
Access to the to the represented object is available through the swizzle, 
but is limited. The represented object cannot be changed.

Maintaining a dual representation for an object allows the loading of objects 
into memory from a store to be deferred; this is particularly important in 
complex applications.  
*/
template <class T>
class TSwizzleC : public TSwizzleCBase
	{
public:
	/** Constructs an uninitialised swizzle. It is necessary because there are also 
	non-default constructors in this class. */
	TSwizzleC() {}
	inline TSwizzleC(const T* aPtr);
	inline TSwizzleC(TStreamId anId);
	inline TSwizzleC(TSwizzle<T> aSwizzle);
	inline TSwizzleC<T>& operator=(const T* aPtr);
	inline const T* AsPtr() const;
	inline operator const T*() const;
	inline const T& operator*() const;
	inline const T* operator->() const;
	inline void ExternalizeL(RWriteStream& aStream) const;
	};

/**
 * @publishedAll 
 * @released
 * A specific instantiation of the family of TSwizzleC<class T> classes that maintains 
the representation of an untyped object as a const pointer or as a stream id. 
*/
TEMPLATE_SPECIALIZATION class TSwizzleC<TAny> : public TSwizzleCBase
	{
public:
	/** Default constructor.
	
	Constructs an uninitialised swizzle. */
	TSwizzleC() {}
	inline TSwizzleC(const TAny* aPtr);
	inline TSwizzleC(TStreamId anId);
	inline TSwizzleC(const TSwizzleCBase& aSwizzle);
	inline TSwizzleC(TStreamRef aRef);
	inline TSwizzleC<TAny>& operator=(const TAny* aPtr);
	inline TSwizzleC<TAny>& operator=(const TSwizzleCBase& aSwizzle);
	inline const TAny* AsPtr() const;
	inline operator const TAny*() const;
	};

/**
 * @publishedAll 
 * @released
 * Store map used when externalising swizzled in-memory objects. 

It has three main characteristics:

it acts as an in-memory repository of stream ids and associated swizzles 

it acts as an externaliser for swizzles 

it offers cleanup support  
*/
class CStoreMap : public CBase,public MExternalizer<TStreamRef>
	{
public:
	/**
	 * @publishedAll 
	 * @released
	 */
	struct TEntry {TSwizzleC<TAny> swizzle;TStreamId id;};
	typedef const TEntry* TIterator;
public:
	IMPORT_C static CStoreMap* NewL(CStreamStore& aStore);
	IMPORT_C static CStoreMap* NewLC(CStreamStore& aStore);
	IMPORT_C CStoreMap(CStreamStore& aStore);
	IMPORT_C ~CStoreMap();
//
	IMPORT_C void BindL(TSwizzleC<TAny> aSwizzle,TStreamId anId);
	IMPORT_C void Unbind(TSwizzleC<TAny> aSwizzle);
	IMPORT_C void Forget(TStreamId anId);
	IMPORT_C void Reset();
	IMPORT_C void ResetAndDestroy();
//
	IMPORT_C TStreamId At(TSwizzleC<TAny> aSwizzle) const;
	IMPORT_C TSwizzleC<TAny> Label(TStreamId anId) const;
	IMPORT_C TIterator Begin() const;
	IMPORT_C TIterator End() const;
private:
	void ExternalizeL(const TStreamRef& aRef,RWriteStream& aStream) const;
private:
	CArrayFixFlat<TEntry> iArray;
	TStreamId iFree;
	CStreamStore* iStore;
	};

/**
 * @publishedAll 
 * @released
 * Maintains two way associations between a UID (a TUid type) and a stream id 
(a TStreamId type) .

Each UID and stream id pair forms an entry in an array. The class provides 
member functions to manage these entries, for example, to add a new entry 
or to change the stream id associated with a UID.

@see TUid
@see TStreamId
@see CDictionaryStore 
 */
class CStreamDictionary : public CBase
	{
public:
	IMPORT_C static CStreamDictionary* NewL();
	IMPORT_C static CStreamDictionary* NewLC();
	IMPORT_C CStreamDictionary();
	IMPORT_C ~CStreamDictionary();
//
	IMPORT_C void AssignL(TUid aUid,TStreamId anId);
	IMPORT_C void Remove(TUid aUid);
	IMPORT_C TStreamId At(TUid aUid) const;
	IMPORT_C TBool IsNull() const;
	//
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
private:
	
	class TEntry
		{
	public:
		TEntry() {}
		inline TEntry(TUid aUid,TStreamId anId);
		void ExternalizeL(RWriteStream& aStream) const;
		void InternalizeL(RReadStream& aStream);
	public:
		TUid iUid;
		TStreamId iId;
		};
private:
	CArrayFixSeg<TEntry> iArray;
	};

#include <s32std.inl>
#endif
