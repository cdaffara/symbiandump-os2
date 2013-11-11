#ifndef __S32STRM_H__
#define __S32STRM_H__/*
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


#include <e32base.h>
#include <s32buf.h>

/**
 * @file
 * @internalComponent
 */

namespace store_private 
{
class MemStreamBuf;
class FileStreamBuf;
};


class RWriteStream;
class RReadStream
	{
public:
	RReadStream();
	void Close();
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

protected:
	friend class CPermanentFileStore;
	MStreamBuf *iSrc; // Owned by store not this object
	};


class RWriteStream
	{
public:
	RWriteStream();
	IMPORT_C void Close();
	IMPORT_C void Release();
	IMPORT_C void CommitL();
//
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
protected:
	friend class CPermanentFileStore;
	MStreamBuf *iSnk;  // Owned by store not this object
	};

class TCardinality
	{
public:
	TCardinality() : iCount(0) {}
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

class TDesHeader
	{
public:
	TDesHeader() {}
	inline TDesHeader(const TDesC8& aDes8);
	inline TDesHeader(const TDesC16& aDes16);
	inline TDesHeader& operator=(const TDesC8& aDes8);
	inline TDesHeader& operator=(const TDesC16& aDes16);
//
	inline TBool IsWidth8() const;
	inline TBool IsWidth16() const;
	inline TInt Length() const;
//
	inline void ExternalizeL(RWriteStream& aStream) const;
	inline void InternalizeL(RReadStream& aStream);
private:
	TCardinality iVal;
	};

class TDesInternalizer
	{
public:
	inline const TDesHeader& Header() const;
	inline TDesHeader& Header();
//
	void operator()(TDes8& aDes8,RReadStream& aStream) const;
	void operator()(TDes16& aDes16,RReadStream& aStream) const;
	void ReadDesc8L (TDes8& aDes8,RReadStream& aStream) const;
	void ReadDesc16L (TDes16& aDes16,RReadStream& aStream) const;
private:
	TDesHeader iHeader;
	};


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


template <class T>
inline RWriteStream& operator<<(RWriteStream& aStream,const T& anObject);
template <class T>
inline RReadStream& operator>>(RReadStream& aStream,T& anObject);

#include <s32strm.inl>

#endif
