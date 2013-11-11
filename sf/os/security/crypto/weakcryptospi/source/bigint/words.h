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
 @internalComponent
*/
 
#ifndef __BIGINT_WORDS_H__
#define __BIGINT_WORDS_H__

#include "../common/inlines.h"

inline void ArraySetBit(TUint* aS, TUint aJ)
	{
	aS[aJ/WORD_BITS] |= (1L << (aJ%WORD_BITS));
	}

inline TUint CountWords(const TUint *X, TUint N)
	{
	while (N && X[N-1]==0)
		N--;
	return N;
	}

inline void SetWords(TUint *r, TUint a, TUint n)
	{
	Mem::Fill(r, WordsToBytes(n), a);
	}

inline void CopyWords(TUint *r, const TUint *a, TUint n)
	{
	Mem::Copy(r, a, WordsToBytes(n));
	}

inline TUint ShiftWordsLeftByBits(TUint *r, TUint n, TUint shiftBits)
	{
	assert (shiftBits<WORD_BITS);
	TUint u, carry=0;
	if (shiftBits)
		for (TUint i=0; i<n; i++)
			{
			u = r[i];
			r[i] = (u << shiftBits) | carry;
			carry = u >> (WORD_BITS-shiftBits);
			}
	return carry;
	}

inline TUint ShiftWordsRightByBits(TUint *r, TUint n, TUint shiftBits)
{
	assert (shiftBits<WORD_BITS);
	TUint u, carry=0;
	if (shiftBits)
		for (int i=n-1; i>=0; i--)
			{
			u = r[i];
			r[i] = (u >> shiftBits) | carry;
			carry = u << (WORD_BITS-shiftBits);
			}
	return carry;
	}

inline TUint CryptoMin(TUint aLeft, TUint aRight) 
	{
	return(aLeft<aRight ? aLeft : aRight);
	}

inline TUint CryptoMax(TUint aLeft, TUint aRight)
	{
	return(aLeft<aRight ? aRight : aLeft);
	}

inline void ShiftWordsLeftByWords(TUint *r, TUint n, TUint shiftWords)
	{
	shiftWords = CryptoMin(shiftWords, n);
	if (shiftWords)
		{
		for (TUint i=n-1; i>=shiftWords; i--)
			r[i] = r[i-shiftWords];
		SetWords(r, 0, shiftWords);
		}
	}

inline void ShiftWordsRightByWords(TUint *r, TUint n, TUint shiftWords)
	{
	shiftWords = CryptoMin(shiftWords, n);
	if (shiftWords)
		{
		for (TUint i=0; i+shiftWords<n; i++)
			r[i] = r[i+shiftWords];
		SetWords(r+n-shiftWords, 0, shiftWords);
		}
	}

inline TUint BytePrecision(TUint aValue)
	{
	TUint x=4;
	if(aValue < 0x10000) aValue <<=16, x-=2;
	if(aValue < 0x1000000) x--;
	if(!aValue) x=0;
	return x;
	}

inline TUint BitPrecision(TUint aValue)
	{
	if(!aValue) return 0;
	TUint x=32;
	if(aValue < 0x10000)    aValue<<=16, x-=16;
	if(aValue < 0x1000000)  aValue<<=8, x-=8;
	if(aValue < 0x10000000) aValue<<=4, x-=4;
	if(aValue < 0x40000000) aValue<<=2, x-=2;
	if(aValue < 0x80000000) --x;
	return x;
	}

static const TUint RoundupSizeTable[] = {2, 2, 2, 4, 4, 8, 8, 8, 8};
    
static inline TUint RoundupSize(TUint n)
	{
    if (n<=8)
        return RoundupSizeTable[n];
    else if (n<=16)
        return 16;
    else if (n<=32)
        return 32;
    else if (n<=64)
        return 64;
    else return 1U << BitPrecision(n-1);
	}

#endif
