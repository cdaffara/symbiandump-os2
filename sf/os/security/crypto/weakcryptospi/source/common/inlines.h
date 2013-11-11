/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalTechnology
*/
 
#ifndef __INLINES_H__
#define __INLINES_H__

#include <e32base.h>

#define assert(x) __ASSERT_DEBUG((x), User::Panic(_L("crypto.dll"), 1))

#if defined(__GCC32__)
typedef long long Int64;
typedef unsigned long long Uint64;
#elif defined(__VC32__)
typedef __int64 Int64;
typedef unsigned __int64 Uint64;
#elif defined(__CW32__)
#pragma longlong on
typedef long long Int64;
typedef unsigned long long Uint64;
#endif

typedef Uint64 dword;
typedef TUint word;
typedef TUint32 word32;

const TUint WORD_SIZE = sizeof(TUint); 
const TUint WORD_BYTES = WORD_SIZE;
const TUint BYTE_BITS = 8;
const TUint WORD_BITS = WORD_SIZE*BYTE_BITS;

//These next two versions of GETBYTE compile to LDR's of words and then shifts
//and ands to get it down to a byte.
//#define GETBYTE(x, y) (TUint)(((x)>>(8*(y)))&255)
//#define GETBYTE(x, y) (TUint)TUint8((x)>>(8*(y)))

//This next version gets the best assembler on gcc and armv4 (it uses LDRB
//rather than shifts and ands
#define GETBYTE(x, y) (((TUint8 *)&(x))[y])

#define MAKE_DWORD(lowWord, highWord) ((dword(highWord)<<WORD_BITS) | (lowWord))
#define LOW_WORD(x) (TUint32)(x)
#define HIGH_WORD(x) (TUint32)((x)>>WORD_BITS)

template <class T> inline void TClassSwap(T& a, T& b)
	{
	T temp(a);
	a = b;
	b = temp;
	}
	
// Returns log2 of aNum where aNum is a power
// of two	
inline TUint8 CryptoLog2(TUint8 aNum)
	{
	switch (aNum)
		{		
		case 1:
			return 0;
		case 1 << 1:
			return 1;
		case 1 << 2:
			return 2;
		case 1 << 3:
			return 3;
		case 1 << 4:
			return 4;
		case 1 << 5:
			return 5;
		case 1 << 6:
			return 6;
		case 1 << 7:
			return 7;
		default:
			ASSERT(EFalse);
		}
	return 0;
	}
	
inline TUint BitsToBytes(TUint bitCount)
	{
	return ((bitCount+7)/(BYTE_BITS));
	}

inline TUint BytesToWords(TUint byteCount)
	{
	return ((byteCount+WORD_SIZE-1)/WORD_SIZE);
	}

inline TUint BitsToWords(TUint bitCount)
	{
	return ((bitCount+WORD_BITS-1)/(WORD_BITS));
	}

inline TUint WordsToBits(TUint wordCount)
	{
	return wordCount * WORD_BITS;
	}

inline TUint BytesToBits(TUint byteCount)
	{	
	return byteCount * BYTE_BITS;
	}

inline TUint WordsToBytes(TUint wordCount)
	{
	return wordCount * WORD_BYTES;
	}

inline void XorWords(TUint32* r, const TUint32* a, TUint n)
	{
	assert(((TUint32)r & 3) == 0); // Catch alignment problems
	
	for (TUint i=0; i<n; i++)
		r[i] ^= a[i];
	}

inline void XorBuf(TUint8* buf, const TUint8* mask, TUint count)
	{
	if (((TUint)buf | (TUint)mask | count) % WORD_SIZE == 0) 
		{
		XorWords((TUint32*)buf, (const TUint32*)mask, count/WORD_SIZE); 
		}
	else
		{
		for (TUint i=0; i<count; i++)
			buf[i] ^= mask[i];
		}
	}

// ************** rotate functions ***************
template <class T> inline T rotlFixed(T x, TUint y)
	{
	assert(y < sizeof(T)*8);
	return ( (T)((x<<y) | (x>>(sizeof(T)*8-y))) );
	}

template <class T> inline T rotrFixed(T x, TUint y)
	{
	assert(y < sizeof(T)*8);
	return ((T)((x>>y) | (x<<(sizeof(T)*8-y))));
	}

inline TUint32 byteReverse(TUint32 value)
	{
	value = ((value & 0xFF00FF00) >> 8) | ((value & 0x00FF00FF) << 8);
	return rotlFixed(value, 16U);
	}

template <class T>
void byteReverse(T* out, const T* in, TUint32 byteCount)
	{
	TUint count = (byteCount+sizeof(T)-1)/sizeof(T);
	for (TUint i=0; i<count; i++)
		out[i] = byteReverse(in[i]);
	}

template <class T>
inline void GetUserKeyLittleEndian(T *out, TUint32 outlen, const TUint8* in, TUint32 inlen)
	{
	const TUint U = sizeof(T);
	assert(inlen <= outlen*U);
	Mem::Copy(out, in, inlen);
	Mem::FillZ((TUint8*)out+inlen, outlen*U-inlen);
	}

template <class T>
inline void GetUserKeyBigEndian(T *out, TUint32 outlen, const TUint8* in, TUint32 inlen)
	{
	const TUint U = sizeof(T);
	assert(inlen <= outlen*U);
	Mem::Copy(out, in, inlen);
	Mem::FillZ((TUint8*)out+inlen, outlen*U-inlen);
	byteReverse(out, out, inlen);
	}

// The following methods have be changed to use byte rather than word accesses,
// as if the input pointer is not be word aligned a fault occurs on arm
// hardware.  This isn't optimal from a performance point of view, but it is
// neccessary because the crypto interfaces (CSymmetricCipher,
// CBlockTransformation) allow clients to pass non-aligned data.

// Fetch 4 words from user's buffer into "a", "b", "c", "d" in LITTLE-endian order
inline void GetBlockLittleEndian(const TUint8* block, TUint16 &a, TUint16 &b, TUint16 &c, TUint16 &d)
	{
	a = (TUint16)(block[0] | block[1] << 8);
	b = (TUint16)(block[2] | block[3] << 8);
	c = (TUint16)(block[4] | block[5] << 8);
	d = (TUint16)(block[6] | block[7] << 8);
	}

// Put 4 words back into user's buffer in LITTLE-endian order
inline void PutBlockLittleEndian(TUint8* block, TUint16 a, TUint16 b, TUint16 c, TUint16 d)
	{
	block[0] = (TUint8)(a & 0xff);
	block[1] = (TUint8)(a >> 8);
	block[2] = (TUint8)(b & 0xff);
	block[3] = (TUint8)(b >> 8);
	block[4] = (TUint8)(c & 0xff);
	block[5] = (TUint8)(c >> 8);
	block[6] = (TUint8)(d & 0xff);
	block[7] = (TUint8)(d >> 8);
	}

// Fetch 1 word from user's buffer in BIG-endian order
inline void GetWordBigEndian(const TUint8* block, TUint32 &a)
	{
	a = block[0] << 24 | block[1] << 16 | block[2] << 8 | block[3];
	}

// Put 1 word back into user's buffer in BIG-endian order
inline void PutWordBigEndian(TUint8* block, TUint32 a)
	{
	block[0] = (TUint8)(a >> 24);
	block[1] = (TUint8)((a >> 16) & 0xff);
	block[2] = (TUint8)((a >> 8) & 0xff);
	block[3] = (TUint8)(a & 0xff);
	}

// Fetch 2 words from user's buffer into "a", "b" in BIG-endian order
inline void GetBlockBigEndian(const TUint8* block, TUint32 &a, TUint32& b)
	{
	GetWordBigEndian(block, a);
	GetWordBigEndian(block + 4, b);
	}

// Put 2 words back into user's buffer in BIG-endian order
inline void PutBlockBigEndian(TUint8* block, TUint32 a, TUint32 b)
	{
	PutWordBigEndian(block, a);
	PutWordBigEndian(block + 4, b);
	}

// Fetch 4 words from user's buffer into "a", "b", "c", "d" in BIG-endian order
inline void GetBlockBigEndian(const TUint8* block, TUint32& a, TUint32& b, TUint32& c, TUint32& d)
	{
	GetWordBigEndian(block, a);
	GetWordBigEndian(block + 4, b);
	GetWordBigEndian(block + 8, c);
	GetWordBigEndian(block + 12, d);
	}

// Put 4 words back into user's buffer in BIG-endian order
inline void PutBlockBigEndian(TUint8* block, TUint32 a, TUint32 b, TUint32 c, TUint32 d)
	{
	PutWordBigEndian(block, a);
	PutWordBigEndian(block + 4, b);
	PutWordBigEndian(block + 8, c);
	PutWordBigEndian(block + 12, d);
	}

#endif // __INLINES_H__
