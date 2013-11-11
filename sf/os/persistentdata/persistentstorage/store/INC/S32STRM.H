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

#if !defined(__S32STRM_H__)
#define __S32STRM_H__
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

/** The largest integer value which can be stored as a TCardinality type. */
const TInt KMaxCardinality=0x1fffffff;
//
class MStreamBuf;
class RWriteStream;

/**
 * @publishedAll 
 * @released
 * The read stream interface. The process of internalising from a stream is 
achieved through a reference to an object of this type.

A store aware class defines an implements an InternalizeL() member function 
which is used to internalise that object and takes a reference to an 
RReadStream as the interface to the read stream. 
*/
class RReadStream
	{
public:
	inline RReadStream();
	inline RReadStream(MStreamBuf* aSource);
	inline MStreamBuf* Source();
	inline void Close();
	IMPORT_C void Release();
//
	IMPORT_C void PushL();
	inline void Pop();
//
	IMPORT_C void ReadL(TDes8& aDes);
	IMPORT_C void ReadL(TDes8& aDes,TInt aLength);
	IMPORT_C void ReadL(TDes8& aDes,TChar aDelim);
	IMPORT_C void ReadL(TUint8* aPtr,TInt aLength);
	IMPORT_C void ReadL(TInt aLength);
	inline void ReadL(RWriteStream& aStream);
	inline void ReadL(RWriteStream& aStream,TInt aLength);
//
	IMPORT_C void ReadL(TDes16& aDes);
	IMPORT_C void ReadL(TDes16& aDes,TInt aLength);
	IMPORT_C void ReadL(TDes16& aDes,TChar aDelim);
	IMPORT_C void ReadL(TUint16* aPtr,TInt aLength);
//
	IMPORT_C TInt8 ReadInt8L();
	IMPORT_C TInt16 ReadInt16L();
	IMPORT_C TInt32 ReadInt32L();
	IMPORT_C TUint8 ReadUint8L();
	IMPORT_C TUint16 ReadUint16L();
	IMPORT_C TUint32 ReadUint32L();
	IMPORT_C TReal32 ReadReal32L() __SOFTFP;
	IMPORT_C TReal64 ReadReal64L() __SOFTFP;
protected:
	inline void Attach(MStreamBuf* aSource);
	inline void Detach();
private:
	MStreamBuf* iSrc;
private:
	friend class RWriteStream;
	};
template <class T>
class MExternalizer;
class TStreamRef;

/**
 * @publishedAll 
 * @released
 * The write stream interface. The process of externalising to a stream is 
achieved through a reference to an object of this type.

A store aware class defines and implements an ExternalizeL() member function 
which is used to externalise that object and takes a reference to an 
RWriteStream as the interface to the write stream. 
*/
class RWriteStream
	{
public:
	inline RWriteStream();
	inline RWriteStream(const MExternalizer<TStreamRef>& anExter);
	inline RWriteStream(MStreamBuf* aSink);
	inline MStreamBuf* Sink();
	IMPORT_C void Close();
	IMPORT_C void Release();
	IMPORT_C void CommitL();
//
	IMPORT_C void PushL();
	inline void Pop();
//
	IMPORT_C void WriteL(const TDesC8& aDes);
	IMPORT_C void WriteL(const TDesC8& aDes,TInt aLength);
	IMPORT_C void WriteL(const TUint8* aPtr,TInt aLength);
	IMPORT_C void WriteL(RReadStream& aStream);
	IMPORT_C void WriteL(RReadStream& aStream,TInt aLength);
//
	IMPORT_C void WriteL(const TDesC16& aDes);
	IMPORT_C void WriteL(const TDesC16& aDes,TInt aLength);
	IMPORT_C void WriteL(const TUint16* aPtr,TInt aLength);
//
	IMPORT_C void WriteInt8L(TInt aValue);
	IMPORT_C void WriteInt16L(TInt aValue);
	IMPORT_C void WriteInt32L(TInt32 aValue);
	IMPORT_C void WriteUint8L(TUint aValue);
	IMPORT_C void WriteUint16L(TUint aValue);
	IMPORT_C void WriteUint32L(TUint32 aValue);
	IMPORT_C void WriteReal32L(TReal aValue) __SOFTFP;
	IMPORT_C void WriteReal64L(TReal64 aValue) __SOFTFP;
protected:
	inline void Attach(MStreamBuf* aSink);
	inline void Detach();
private:
	IMPORT_C void WriteRefL(TStreamRef aRef);
private:
	MStreamBuf* iSnk;
	const MExternalizer<TStreamRef>* iExterL;
private:
	friend class TStreamRef;
	};
//
template <class T>
inline RWriteStream& operator<<(RWriteStream& aStream,const T& anObject);
template <class T>
inline RReadStream& operator>>(RReadStream& aStream,T& anObject);

/**
 * @publishedAll 
 * @released
 * Compact stream format for positive integer values in the range 0 to 
536,870,911 ((2^29)-1). Values in the range 0-127 are stored in a single byte, 
128-16383 in two bytes and other values in 4 bytes.

The class provides conversion to and from TInt, and both externalization and 
internalization functions. It is used to significant effect within Symbian 
code. 
*/
class TCardinality
	{
public:
	TCardinality() {}
	inline TCardinality(TInt aCount);
	inline operator TInt() const;
//
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
private:
	TInt iCount;
private:
	IMPORT_C static void __DbgChkRange(TInt aCount);
	};
//
/** Defines a function type for a function returned by TExternalizer::Function() 
and TStreamRef. */
typedef void (*TExternalizeFunction)(const TAny* aPtr,RWriteStream& aStream);
/** Defines a function type for a function returned by TInternalizer::Function(). */
typedef void (*TInternalizeFunction)(TAny* aPtr,RReadStream& aStream);

/**
 * @publishedAll 
 * @released
 * A family of classes whose instances can be used to perform externalisation 
on behalf of other objects.

@see TStreamRef  
*/
template <class T>
class TExternalizer
	{
public:
	inline void operator()(const T& anObject,RWriteStream& aStream) const;
	inline static TExternalizeFunction Function();
private:
	static void ExternalizeAsL(const TAny* aPtr,RWriteStream& aStream);
	};

/**
 * @publishedAll 
 * @released
 * A specific instantiation of the family of TExternalizer<class T> classes whose 
instances can be used to perform externalisation on behalf of other untyped 
objects. 
*/
TEMPLATE_SPECIALIZATION class TExternalizer<TAny>
	{
public:
	/** Default constructor. */
	TExternalizer() {}
	
	/** Constructs the externaliser with the specified externalisation function.
	
	@param aFunction The externalisation function. */
	inline TExternalizer(TExternalizeFunction aFunction);
	inline void operator()(const TAny* aPtr,RWriteStream& aStream) const;
	
	/** Gets a pointer to the function to be used to perform externalisation.
	
	@return The externalisation function. */
	inline TExternalizeFunction Function() const;
private:
	TExternalizeFunction iFunc;
	};

/**
 * @publishedAll 
 * @released
 * A family of classes whose instances can be used to perform internalisation 
on behalf of other objects. 
*/
template <class T>
class TInternalizer
	{
public:
	inline void operator()(T& anObject,RReadStream& aStream) const;
	inline static TInternalizeFunction Function();
private:
	static void InternalizeAsL(TAny* aPtr,RReadStream& aStream);
	};

/**
 * @publishedAll 
 * @released
 * A specific instantiation of the family of TInternalizer<class T> classes whose 
instances can be used to perform internalisation on behalf of other objects. 
*/
TEMPLATE_SPECIALIZATION class TInternalizer<TAny>
	{
public:
	/** Default constructor. */
	TInternalizer() {}
	
	/** Constructs the internaliser with the specified internalisation function.
	
	@param aFunction The internalisation function. */
	inline TInternalizer(TInternalizeFunction aFunction);
	inline void operator()(TAny* aPtr,RReadStream& aStream) const;
	
	/** Gets a pointer to the function to be used to perform internalisation.
	
	@return The internalisation function. */
	inline TInternalizeFunction Function() const;
private:
	TInternalizeFunction iFunc;
	};

/**
 * @publishedAll 
 * @released
 * A family of classes defining an interface that can be implemented by classes 
that need to perform externalisation on behalf of others.

@see CStoreMap 
*/
template <class T>
class MExternalizer
	{
public:
	inline void operator()(const T& anObject,RWriteStream& aStream) const;
private:
	/** Performs externalisation.
	
	The function is called by operator().
	
	@param anObject The object to be externalised.
	@param aStream The write stream. */
	virtual void ExternalizeL(const T& anObject,RWriteStream& aStream) const=0;
	};

/**
 * @publishedAll 
 * @released
 * A family of classes defining an interface that can be implemented by classes 
that need to perform internalisation on behalf of others.

@see CStoreMap 
*/
template <class T>
class MInternalizer
	{
public:
	inline void operator()(T& anObject,RReadStream& aStream) const;
private:
	/** Performs internalisation.
	
	The function is called by operator().
	
	@param anObject The object to be the target of the internalisation process.
	@param aStream The read stream. */
	virtual void InternalizeL(T& anObject,RReadStream& aStream) const=0;
	};

/**
 * @publishedAll 
 * @released
 * A proxy for perfoming externalisation for classes that do not have an 
externalisation member. 
*/
class TStreamRef
	{
public:
	inline TStreamRef(const TAny* aPtr,TExternalizeFunction aFunction);
	inline const TAny* Ptr() const;
	inline TExternalizeFunction Function() const;
//
	inline void ExternalizeL(RWriteStream& aStream) const;
private:
	const TAny* iPtr;
	TExternalizeFunction iFunc;
	};

/**
 * @publishedAll 
 * @released
 * A class whose members are used to distinguish between the two variants of 
the Symbian OS internal function DoExternalizeL(). 
*/
class Externalize
	{
public:
	/** Indicates that an object will be externalized by calling its 
	ExternalizeL() member. */
	class Member {};

	/** Indicates that an object will be externalized by calling a global 
	ExternalizeL() function. */
	class Function {};
	};

/**
 * @publishedAll 
 * @released
 * A class whose members are used to distinguish between the two variants of 
the Symbian OS internal function DoInternalizeL(). 
*/
class Internalize
	{
public:
	/** Indicates that an object will be internalized by calling its 
	InternalizeL() member. */
	class Member {};

	/** Indicates that an object will be internalized by calling a global 
	InternalizeL() function. */
	class Function {};
	};

#include <s32strm.inl>
#endif
