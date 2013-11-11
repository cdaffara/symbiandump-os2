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

#if !defined(__S32HUF_H__)
#define __S32HUF_H__
#if !defined(__S32BUF_H__)
#include <s32buf.h>
#endif
#if !defined(__S32STRM_H__)
#include <s32strm.h>
#endif

class Huffman // Huffman primitives: sell to base
	{
public:
	IMPORT_C static const TUint* DefaultBits(); // long code needs to be sorted out
	IMPORT_C static const TUint8* DefaultTree(); // is there a real case for default tables?
//
	IMPORT_C void Encode(...); // incremental encoding and decoding primitives
	IMPORT_C void Decode(...); // suitable for sharing between descriptors and filters
	};
//
class THufEncodeFilter : public TStreamFilter
	{
public:
	IMPORT_C THufEncodeFilter();
	IMPORT_C void Set(MStreamBuf* aHost,const TUint* aHufBits,TInt aMode=EWrite);
protected:
	IMPORT_C TInt Capacity(TInt aMaxLength);
	IMPORT_C TInt FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* anEnd);
	IMPORT_C void DoSynchL();
private:
	const TUint* iTable;
	TInt iMaxBits;
	TUint iOut;
	TInt iBit;
	};
class THufDecodeFilter : public TStreamFilter
	{
public:
	IMPORT_C THufDecodeFilter();
	IMPORT_C void Set(MStreamBuf* aHost,const TUint8* aHufTree,TInt aMode=ERead);
protected:
	IMPORT_C TInt Capacity(TInt aMaxLength);
	IMPORT_C TInt FilterL(TAny* aPtr,TInt aMaxLength,const TUint8*& aFrom,const TUint8* anEnd);
	IMPORT_C void DoSynchL();
private:
	const TUint8* iRoot;
	const TUint8* iNode;
	TUint iBits;
	};
//
class RHufDecodeReadStream : public RReadStream
	{
public:
	RHufDecodeReadStream() {}
	IMPORT_C RHufDecodeReadStream(RReadStream& aHost,const TUint8* aHufTree=Huffman::DefaultTree());
	IMPORT_C void Open(RReadStream& aHost,const TUint8* aHufTree=Huffman::DefaultTree());
private:
	THufDecodeFilter iFilter;
	};
class RHufEncodeWriteStream : public RWriteStream
	{
public:
	RHufEncodeWriteStream() {}
	inline RHufEncodeWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C RHufEncodeWriteStream(RWriteStream& aHost,const TUint* aHufBits=Huffman::DefaultBits());
	IMPORT_C void Open(RWriteStream& aHost,const TUint* aHufBits=Huffman::DefaultBits());
private:
	THufEncodeFilter iFilter;
	};
//
class RHufEncodeReadStream : public RReadStream
	{
public:
	RHufEncodeReadStream() {}
	IMPORT_C RHufEncodeReadStream(RReadStream& aHost,const TUint* aHufBits=Huffman::DefaultBits());
	IMPORT_C void Open(RReadStream& aHost,const TUint* aHufBits=Huffman::DefaultBits());
private:
	THufEncodeFilter iFilter;
	};
class RHufDecodeWriteStream : public RWriteStream
	{
public:
	RHufDecodeWriteStream() {}
	inline RHufDecodeWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C RHufDecodeWriteStream(RWriteStream& aHost,const TUint8* aHufTree=Huffman::DefaultTree());
	IMPORT_C void Open(RWriteStream& aHost,const TUint8* aHufTree=Huffman::DefaultTree());
private:
	THufDecodeFilter iFilter;
	};

#include "S32HUF.INL"
#endif
