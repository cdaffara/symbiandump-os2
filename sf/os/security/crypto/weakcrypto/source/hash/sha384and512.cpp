/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Common implementation of SHA384 and SHA512
* RFC 4634 (US Secure Hash Algorithms (SHA and HMAC-SHA))
*
*/


/**
 @file
*/

#include "sha384and512.h"

/**
 * SHA512 Constants
 * 
 * SHA-512 uses a sequence of eighty constant 64-bit words. 
 * These words represent the first sixty-four bits of the fractional 
 * parts of the cube roots of the first eighty prime numbers.
 * 
 * FIPS 180-2 Section 4.2.3
 */
const TUint64 K[80] = 
	{
	UI64LIT(0x428a2f98d728ae22), UI64LIT(0x7137449123ef65cd), UI64LIT(0xb5c0fbcfec4d3b2f), UI64LIT(0xe9b5dba58189dbbc),
	UI64LIT(0x3956c25bf348b538), UI64LIT(0x59f111f1b605d019), UI64LIT(0x923f82a4af194f9b), UI64LIT(0xab1c5ed5da6d8118),
	UI64LIT(0xd807aa98a3030242), UI64LIT(0x12835b0145706fbe), UI64LIT(0x243185be4ee4b28c), UI64LIT(0x550c7dc3d5ffb4e2),
	UI64LIT(0x72be5d74f27b896f), UI64LIT(0x80deb1fe3b1696b1), UI64LIT(0x9bdc06a725c71235), UI64LIT(0xc19bf174cf692694),
	UI64LIT(0xe49b69c19ef14ad2), UI64LIT(0xefbe4786384f25e3), UI64LIT(0x0fc19dc68b8cd5b5), UI64LIT(0x240ca1cc77ac9c65),
	UI64LIT(0x2de92c6f592b0275), UI64LIT(0x4a7484aa6ea6e483), UI64LIT(0x5cb0a9dcbd41fbd4), UI64LIT(0x76f988da831153b5),
	UI64LIT(0x983e5152ee66dfab), UI64LIT(0xa831c66d2db43210), UI64LIT(0xb00327c898fb213f), UI64LIT(0xbf597fc7beef0ee4),
	UI64LIT(0xc6e00bf33da88fc2), UI64LIT(0xd5a79147930aa725), UI64LIT(0x06ca6351e003826f), UI64LIT(0x142929670a0e6e70),
	UI64LIT(0x27b70a8546d22ffc), UI64LIT(0x2e1b21385c26c926), UI64LIT(0x4d2c6dfc5ac42aed), UI64LIT(0x53380d139d95b3df),
	UI64LIT(0x650a73548baf63de), UI64LIT(0x766a0abb3c77b2a8), UI64LIT(0x81c2c92e47edaee6), UI64LIT(0x92722c851482353b),
	UI64LIT(0xa2bfe8a14cf10364), UI64LIT(0xa81a664bbc423001), UI64LIT(0xc24b8b70d0f89791), UI64LIT(0xc76c51a30654be30),
	UI64LIT(0xd192e819d6ef5218), UI64LIT(0xd69906245565a910), UI64LIT(0xf40e35855771202a), UI64LIT(0x106aa07032bbd1b8),
	UI64LIT(0x19a4c116b8d2d0c8), UI64LIT(0x1e376c085141ab53), UI64LIT(0x2748774cdf8eeb99), UI64LIT(0x34b0bcb5e19b48a8),
	UI64LIT(0x391c0cb3c5c95a63), UI64LIT(0x4ed8aa4ae3418acb), UI64LIT(0x5b9cca4f7763e373), UI64LIT(0x682e6ff3d6b2b8a3),
	UI64LIT(0x748f82ee5defb2fc), UI64LIT(0x78a5636f43172f60), UI64LIT(0x84c87814a1f0ab72), UI64LIT(0x8cc702081a6439ec),
	UI64LIT(0x90befffa23631e28), UI64LIT(0xa4506cebde82bde9), UI64LIT(0xbef9a3f7b2c67915), UI64LIT(0xc67178f2e372532b),
	UI64LIT(0xca273eceea26619c), UI64LIT(0xd186b8c721c0c207), UI64LIT(0xeada7dd6cde0eb1e), UI64LIT(0xf57d4f7fee6ed178),
	UI64LIT(0x06f067aa72176fba), UI64LIT(0x0a637dc5a2c898a6), UI64LIT(0x113f9804bef90dae), UI64LIT(0x1b710b35131c471b),
	UI64LIT(0x28db77f523047d84), UI64LIT(0x32caab7b40c72493), UI64LIT(0x3c9ebe0a15c9bebc), UI64LIT(0x431d67c49c100d4c),
	UI64LIT(0x4cc5d4becb3e42b6), UI64LIT(0x597f299cfc657e2a), UI64LIT(0x5fcb6fab3ad6faec), UI64LIT(0x6c44198c4a475817)
	};

/**
 * Define the SHA SIGMA and sigma macros 
 * 
 * FIPS 180-2 section 4.1.3
 */
// Equation 4.10
inline TUint64 SHA512_SIGMA0(TUint64 aWord)
	{
	return (SHA_ROTR<TUint64>(28,aWord) ^ SHA_ROTR<TUint64>(34,aWord) ^ SHA_ROTR<TUint64>(39,aWord));
	}
// Equation 4.11
inline TUint64 SHA512_SIGMA1(TUint64 aWord)
	{
	return (SHA_ROTR<TUint64>(14,aWord) ^ SHA_ROTR<TUint64>(18,aWord) ^ SHA_ROTR<TUint64>(41,aWord));
	}
// Equation 4.12
inline TUint64 SHA512_sigma0(TUint64 aWord)
	{
	return (SHA_ROTR<TUint64>(1, aWord) ^ SHA_ROTR<TUint64>(8, aWord) ^ SHA_SHR<TUint64>(7, aWord));
	}
// Equation 4.13
inline TUint64 SHA512_sigma1(TUint64 aWord)
	{
	return (SHA_ROTR<TUint64>(19,aWord) ^ SHA_ROTR<TUint64>(61,aWord) ^ SHA_SHR<TUint64>(6,aWord));
	}

// Macros
inline TUint64 Make64BitWord(const TUint8* aData)
	{
	TUint64 result = (TUint64)aData[0] << 56 | (TUint64)aData[1] << 48 | (TUint64)aData[2] << 40 | 
					 (TUint64)aData[3] << 32 | (TUint64)aData[4] << 24 | (TUint64)aData[5] << 16 |
					 (TUint64)aData[6] << 8 | (TUint64)aData[7];
	return result;
	}


CSHA384And512* CSHA384And512::NewL()
	{
	CSHA384And512* self=new (ELeave) CSHA384And512();
	return self;						
	}
														
CSHA384And512::CSHA384And512() : iHash(KSHA512HashSize)
	{		
	}
	
CSHA384And512::CSHA384And512(const CSHA384And512& aSHAImpl)
				: 	iHash(aSHAImpl.iHash),
					iA(aSHAImpl.iA),
					iB(aSHAImpl.iB),
					iC(aSHAImpl.iC),
					iD(aSHAImpl.iD),
					iE(aSHAImpl.iE),
					iF(aSHAImpl.iF),
					iG(aSHAImpl.iG),
					iH(aSHAImpl.iH),
					iNl(aSHAImpl.iNl),
					iNh(aSHAImpl.iNh),
					iBlockIndex(aSHAImpl.iBlockIndex)
	{
	Mem::Copy(iData, aSHAImpl.iData, KSHA512BlockSize*sizeof(TUint64));
	}
	
void CSHA384And512::Reset(const TAny* aValueArr)
	{
	const TUint64* values = static_cast<const TUint64*>(aValueArr);

	iA = values[0];
	iB = values[1];
	iC = values[2];
	iD = values[3];
	iE = values[4];
	iF = values[5];
	iG = values[6];
	iH = values[7];
	iNh = 0;
	iNl = 0;
	iBlockIndex = 0;
	}

//This function will panic if the total input length is longer than 2^128 in bits
_LIT(KPanicString, "Message length exceeds supported length");
inline void CSHA384And512::AddLength(const TUint64 aLength)
	{
	TUint64 temp = iNl;
	iNl += aLength << 3;
	__ASSERT_ALWAYS(((iNh != KMaxTUint64) || (temp <= iNl)), User::Panic(KPanicString, KErrOverflow));
	iNh += (temp > iNl);
	}

// This assumes a big-endian architecture
void CSHA384And512::Update(const TUint8* aData,TUint aLength)
	{
	while((aLength / 8) > 0 && (iBlockIndex % 8 == 0))
		{
		iData[iBlockIndex>>3] = Make64BitWord(aData);
		iBlockIndex+=8;
		aData+=8;
		aLength-=8;
		if(iBlockIndex==KSHA512BlockSize) 
			{
			Block();
			AddLength(KSHA512BlockSize);
			}
		}

	while(aLength--)
		{
		if(!(iBlockIndex&0x07))
			{
			iData[iBlockIndex >> 3] = 0;
			}
		iData[iBlockIndex >> 3] |= static_cast<TUint64>(*aData) << ((7 - iBlockIndex&0x07) << 3) ;
		++aData;
		++iBlockIndex;
		if(iBlockIndex==KSHA512BlockSize) 
			{
			Block();
			AddLength(KSHA512BlockSize);
			}
		}
	}

static inline void CSHA512_16(	const TUint64 aA, 
								const TUint64 aB, 
								const TUint64 aC,
								TUint64& aD, 
								const TUint64 aE, 
								const TUint64 aF,
								const TUint64 aG, 
								TUint64& aH,
								TUint64 aTemp1,
								TUint64 aTemp2,
								const TUint64 aK,
								const TUint64 aWord)
	{
	aTemp1 = aH + SHA512_SIGMA1(aE) + SHA_Ch(aE,aF,aG) + aK + aWord;
	aTemp2 = SHA512_SIGMA0(aA) + SHA_Maj(aA,aB,aC);
	aD = aD + aTemp1;
	aH = aTemp1 + aTemp2;
	}

static inline void CSHA512_64(	const TUint64 aA, 
								const TUint64 aB, 
								const TUint64 aC,
								TUint64& aD, 
								const TUint64 aE, 
								const TUint64 aF,
								const TUint64 aG, 
								TUint64& aH,
								TUint64 aTemp1,
								TUint64 aTemp2,
								const TUint64 aK,
								TUint64& aWord0,
								const TUint64 aWord2,
								const TUint64 aWord7,
								const TUint64 aWord15,
								const TUint64 aWord16)
	{
	aWord0 = SHA512_sigma1(aWord2) + aWord7 + SHA512_sigma0(aWord15) + aWord16;
	CSHA512_16(aA, aB, aC, aD, aE, aF, aG, aH, aTemp1, aTemp2, aK, aWord0);
	}

/**
 * This function actually calculates the hash.
 * Function is defined in FIPS 180-2 section 6.3.2
 * 
 * This function is the expanded version of the following loop.
 *	for(TUint i = 0; i < 80; ++i)
 *		{
 *		if(i >= 16)
 *			{
 *			iData[i] = SHA512_sigma1(iData[i-2]) + iData[i-7] + SHA512_sigma0(iData[i-15]) + iData[i-16];
 *			}
 *
 *		temp1 = tempH + SHA512_SIGMA1(tempE) + SHA_Ch(tempE,tempF,tempG) + K[i] + iData[i];
 *	    temp2 = SHA512_SIGMA0(tempA) + SHA_Maj(tempA,tempB,tempC);
 *	    tempH = tempG;
 *	    tempG = tempF;
 *	    tempF = tempE;
 *	    tempE = tempD + temp1;
 *	    tempD = tempC;
 *	    tempC = tempB;
 *	    tempB = tempA;
 *	    tempA = temp1 + temp2;		
 *		}
 */
void CSHA384And512::Block()
	{
	TUint64 tempA=iA;
	TUint64 tempB=iB;
	TUint64 tempC=iC;
	TUint64 tempD=iD;
	TUint64 tempE=iE;
	TUint64 tempF=iF;
	TUint64 tempG=iG;
	TUint64 tempH=iH;
	TUint64 temp1=0;
	TUint64 temp2=0;
	
	CSHA512_16(tempA,tempB,tempC,tempD,tempE,tempF,tempG,tempH,temp1,temp2,K[0],iData[0]);
	CSHA512_16(tempH,tempA,tempB,tempC,tempD,tempE,tempF,tempG,temp1,temp2,K[1],iData[1]);
	CSHA512_16(tempG,tempH,tempA,tempB,tempC,tempD,tempE,tempF,temp1,temp2,K[2],iData[2]);
	CSHA512_16(tempF,tempG,tempH,tempA,tempB,tempC,tempD,tempE,temp1,temp2,K[3],iData[3]);
	CSHA512_16(tempE,tempF,tempG,tempH,tempA,tempB,tempC,tempD,temp1,temp2,K[4],iData[4]);
	CSHA512_16(tempD,tempE,tempF,tempG,tempH,tempA,tempB,tempC,temp1,temp2,K[5],iData[5]);
	CSHA512_16(tempC,tempD,tempE,tempF,tempG,tempH,tempA,tempB,temp1,temp2,K[6],iData[6]);
	CSHA512_16(tempB,tempC,tempD,tempE,tempF,tempG,tempH,tempA,temp1,temp2,K[7],iData[7]);

	CSHA512_16(tempA,tempB,tempC,tempD,tempE,tempF,tempG,tempH,temp1,temp2,K[8],iData[8]);
	CSHA512_16(tempH,tempA,tempB,tempC,tempD,tempE,tempF,tempG,temp1,temp2,K[9],iData[9]);
	CSHA512_16(tempG,tempH,tempA,tempB,tempC,tempD,tempE,tempF,temp1,temp2,K[10],iData[10]);
	CSHA512_16(tempF,tempG,tempH,tempA,tempB,tempC,tempD,tempE,temp1,temp2,K[11],iData[11]);
	CSHA512_16(tempE,tempF,tempG,tempH,tempA,tempB,tempC,tempD,temp1,temp2,K[12],iData[12]);
	CSHA512_16(tempD,tempE,tempF,tempG,tempH,tempA,tempB,tempC,temp1,temp2,K[13],iData[13]);
	CSHA512_16(tempC,tempD,tempE,tempF,tempG,tempH,tempA,tempB,temp1,temp2,K[14],iData[14]);
	CSHA512_16(tempB,tempC,tempD,tempE,tempF,tempG,tempH,tempA,temp1,temp2,K[15],iData[15]);

	CSHA512_64(	tempA, tempB, tempC, tempD, tempE, tempF, tempG, tempH, temp1, temp2,
				K[16], iData[16], iData[14], iData[9], iData[1], iData[0]);
	CSHA512_64(	tempH, tempA, tempB, tempC, tempD, tempE, tempF, tempG, temp1, temp2,
				K[17], iData[17], iData[15], iData[10], iData[2], iData[1]);
	CSHA512_64(	tempG, tempH, tempA, tempB, tempC, tempD, tempE, tempF, temp1, temp2,
				K[18], iData[18], iData[16], iData[11], iData[3], iData[2]);
	CSHA512_64(	tempF, tempG, tempH, tempA, tempB, tempC, tempD, tempE, temp1, temp2,
				K[19], iData[19], iData[17], iData[12], iData[4], iData[3]);
	CSHA512_64(	tempE, tempF, tempG, tempH, tempA, tempB, tempC, tempD, temp1, temp2,
				K[20], iData[20], iData[18], iData[13], iData[5], iData[4]);
	CSHA512_64(	tempD, tempE, tempF, tempG, tempH, tempA, tempB, tempC, temp1, temp2,
				K[21], iData[21], iData[19], iData[14], iData[6], iData[5]);
	CSHA512_64(	tempC, tempD, tempE, tempF, tempG, tempH, tempA, tempB, temp1, temp2,
				K[22], iData[22], iData[20], iData[15], iData[7], iData[6]);
	CSHA512_64(	tempB, tempC, tempD, tempE, tempF, tempG, tempH, tempA, temp1, temp2,
				K[23], iData[23], iData[21], iData[16], iData[8], iData[7]);

	CSHA512_64(	tempA, tempB, tempC, tempD, tempE, tempF, tempG, tempH, temp1, temp2,
				K[24], iData[24], iData[22], iData[17], iData[9], iData[8]);
	CSHA512_64(	tempH, tempA, tempB, tempC, tempD, tempE, tempF, tempG, temp1, temp2,
				K[25], iData[25], iData[23], iData[18], iData[10], iData[9]);
	CSHA512_64(	tempG, tempH, tempA, tempB, tempC, tempD, tempE, tempF, temp1, temp2,
				K[26], iData[26], iData[24], iData[19], iData[11], iData[10]);
	CSHA512_64(	tempF, tempG, tempH, tempA, tempB, tempC, tempD, tempE, temp1, temp2,
				K[27], iData[27], iData[25], iData[20], iData[12], iData[11]);
	CSHA512_64(	tempE, tempF, tempG, tempH, tempA, tempB, tempC, tempD, temp1, temp2,
				K[28], iData[28], iData[26], iData[21], iData[13], iData[12]);
	CSHA512_64(	tempD, tempE, tempF, tempG, tempH, tempA, tempB, tempC, temp1, temp2,
				K[29], iData[29], iData[27], iData[22], iData[14], iData[13]);
	CSHA512_64(	tempC, tempD, tempE, tempF, tempG, tempH, tempA, tempB, temp1, temp2,
				K[30], iData[30], iData[28], iData[23], iData[15], iData[14]);
	CSHA512_64(	tempB, tempC, tempD, tempE, tempF, tempG, tempH, tempA, temp1, temp2,
				K[31], iData[31], iData[29], iData[24], iData[16], iData[15]);

	CSHA512_64(	tempA, tempB, tempC, tempD, tempE, tempF, tempG, tempH, temp1, temp2,
				K[32], iData[32], iData[30], iData[25], iData[17], iData[16]);
	CSHA512_64(	tempH, tempA, tempB, tempC, tempD, tempE, tempF, tempG, temp1, temp2,
				K[33], iData[33], iData[31], iData[26], iData[18], iData[17]);
	CSHA512_64(	tempG, tempH, tempA, tempB, tempC, tempD, tempE, tempF, temp1, temp2,
				K[34], iData[34], iData[32], iData[27], iData[19], iData[18]);
	CSHA512_64(	tempF, tempG, tempH, tempA, tempB, tempC, tempD, tempE, temp1, temp2,
				K[35], iData[35], iData[33], iData[28], iData[20], iData[19]);
	CSHA512_64(	tempE, tempF, tempG, tempH, tempA, tempB, tempC, tempD, temp1, temp2,
				K[36], iData[36], iData[34], iData[29], iData[21], iData[20]);
	CSHA512_64(	tempD, tempE, tempF, tempG, tempH, tempA, tempB, tempC, temp1, temp2,
				K[37], iData[37], iData[35], iData[30], iData[22], iData[21]);
	CSHA512_64(	tempC, tempD, tempE, tempF, tempG, tempH, tempA, tempB, temp1, temp2,
				K[38], iData[38], iData[36], iData[31], iData[23], iData[22]);
	CSHA512_64(	tempB, tempC, tempD, tempE, tempF, tempG, tempH, tempA, temp1, temp2,
				K[39], iData[39], iData[37], iData[32], iData[24], iData[23]);

	CSHA512_64(	tempA, tempB, tempC, tempD, tempE, tempF, tempG, tempH, temp1, temp2,
				K[40], iData[40], iData[38], iData[33], iData[25], iData[24]);
	CSHA512_64(	tempH, tempA, tempB, tempC, tempD, tempE, tempF, tempG, temp1, temp2,
				K[41], iData[41], iData[39], iData[34], iData[26], iData[25]);
	CSHA512_64(	tempG, tempH, tempA, tempB, tempC, tempD, tempE, tempF, temp1, temp2,
				K[42], iData[42], iData[40], iData[35], iData[27], iData[26]);
	CSHA512_64(	tempF, tempG, tempH, tempA, tempB, tempC, tempD, tempE, temp1, temp2,
				K[43], iData[43], iData[41], iData[36], iData[28], iData[27]);
	CSHA512_64(	tempE, tempF, tempG, tempH, tempA, tempB, tempC, tempD, temp1, temp2,
				K[44], iData[44], iData[42], iData[37], iData[29], iData[28]);
	CSHA512_64(	tempD, tempE, tempF, tempG, tempH, tempA, tempB, tempC, temp1, temp2,
				K[45], iData[45], iData[43], iData[38], iData[30], iData[29]);
	CSHA512_64(	tempC, tempD, tempE, tempF, tempG, tempH, tempA, tempB, temp1, temp2,
				K[46], iData[46], iData[44], iData[39], iData[31], iData[30]);
	CSHA512_64(	tempB, tempC, tempD, tempE, tempF, tempG, tempH, tempA, temp1, temp2,
				K[47], iData[47], iData[45], iData[40], iData[32], iData[31]);

	CSHA512_64(	tempA, tempB, tempC, tempD, tempE, tempF, tempG, tempH, temp1, temp2,
				K[48], iData[48], iData[46], iData[41], iData[33], iData[32]);
	CSHA512_64(	tempH, tempA, tempB, tempC, tempD, tempE, tempF, tempG, temp1, temp2,
				K[49], iData[49], iData[47], iData[42], iData[34], iData[33]);
	CSHA512_64(	tempG, tempH, tempA, tempB, tempC, tempD, tempE, tempF, temp1, temp2,
				K[50], iData[50], iData[48], iData[43], iData[35], iData[34]);
	CSHA512_64(	tempF, tempG, tempH, tempA, tempB, tempC, tempD, tempE, temp1, temp2,
				K[51], iData[51], iData[49], iData[44], iData[36], iData[35]);
	CSHA512_64(	tempE, tempF, tempG, tempH, tempA, tempB, tempC, tempD, temp1, temp2,
				K[52], iData[52], iData[50], iData[45], iData[37], iData[36]);
	CSHA512_64(	tempD, tempE, tempF, tempG, tempH, tempA, tempB, tempC, temp1, temp2,
				K[53], iData[53], iData[51], iData[46], iData[38], iData[37]);
	CSHA512_64(	tempC, tempD, tempE, tempF, tempG, tempH, tempA, tempB, temp1, temp2,
				K[54], iData[54], iData[52], iData[47], iData[39], iData[38]);
	CSHA512_64(	tempB, tempC, tempD, tempE, tempF, tempG, tempH, tempA, temp1, temp2,
				K[55], iData[55], iData[53], iData[48], iData[40], iData[39]);

	CSHA512_64(	tempA, tempB, tempC, tempD, tempE, tempF, tempG, tempH, temp1, temp2,
				K[56], iData[56], iData[54], iData[49], iData[41], iData[40]);
	CSHA512_64(	tempH, tempA, tempB, tempC, tempD, tempE, tempF, tempG, temp1, temp2,
				K[57], iData[57], iData[55], iData[50], iData[42], iData[41]);
	CSHA512_64(	tempG, tempH, tempA, tempB, tempC, tempD, tempE, tempF, temp1, temp2,
				K[58], iData[58], iData[56], iData[51], iData[43], iData[42]);
	CSHA512_64(	tempF, tempG, tempH, tempA, tempB, tempC, tempD, tempE, temp1, temp2,
				K[59], iData[59], iData[57], iData[52], iData[44], iData[43]);
	CSHA512_64(	tempE, tempF, tempG, tempH, tempA, tempB, tempC, tempD, temp1, temp2,
				K[60], iData[60], iData[58], iData[53], iData[45], iData[44]);
	CSHA512_64(	tempD, tempE, tempF, tempG, tempH, tempA, tempB, tempC, temp1, temp2,
				K[61], iData[61], iData[59], iData[54], iData[46], iData[45]);
	CSHA512_64(	tempC, tempD, tempE, tempF, tempG, tempH, tempA, tempB, temp1, temp2,
				K[62], iData[62], iData[60], iData[55], iData[47], iData[46]);
	CSHA512_64(	tempB, tempC, tempD, tempE, tempF, tempG, tempH, tempA, temp1, temp2,
				K[63], iData[63], iData[61], iData[56], iData[48], iData[47]);

	CSHA512_64(	tempA, tempB, tempC, tempD, tempE, tempF, tempG, tempH, temp1, temp2,
				K[64], iData[64], iData[62], iData[57], iData[49], iData[48]);
	CSHA512_64(	tempH, tempA, tempB, tempC, tempD, tempE, tempF, tempG, temp1, temp2,
				K[65], iData[65], iData[63], iData[58], iData[50], iData[49]);
	CSHA512_64(	tempG, tempH, tempA, tempB, tempC, tempD, tempE, tempF, temp1, temp2,
				K[66], iData[66], iData[64], iData[59], iData[51], iData[50]);
	CSHA512_64(	tempF, tempG, tempH, tempA, tempB, tempC, tempD, tempE, temp1, temp2,
				K[67], iData[67], iData[65], iData[60], iData[52], iData[51]);
	CSHA512_64(	tempE, tempF, tempG, tempH, tempA, tempB, tempC, tempD, temp1, temp2,
				K[68], iData[68], iData[66], iData[61], iData[53], iData[52]);
	CSHA512_64(	tempD, tempE, tempF, tempG, tempH, tempA, tempB, tempC, temp1, temp2,
				K[69], iData[69], iData[67], iData[62], iData[54], iData[53]);
	CSHA512_64(	tempC, tempD, tempE, tempF, tempG, tempH, tempA, tempB, temp1, temp2,
				K[70], iData[70], iData[68], iData[63], iData[55], iData[54]);
	CSHA512_64(	tempB, tempC, tempD, tempE, tempF, tempG, tempH, tempA, temp1, temp2,
				K[71], iData[71], iData[69], iData[64], iData[56], iData[55]);

	CSHA512_64(	tempA, tempB, tempC, tempD, tempE, tempF, tempG, tempH, temp1, temp2,
				K[72], iData[72], iData[70], iData[65], iData[57], iData[56]);
	CSHA512_64(	tempH, tempA, tempB, tempC, tempD, tempE, tempF, tempG, temp1, temp2,
				K[73], iData[73], iData[71], iData[66], iData[58], iData[57]);
	CSHA512_64(	tempG, tempH, tempA, tempB, tempC, tempD, tempE, tempF, temp1, temp2,
				K[74], iData[74], iData[72], iData[67], iData[59], iData[58]);
	CSHA512_64(	tempF, tempG, tempH, tempA, tempB, tempC, tempD, tempE, temp1, temp2,
				K[75], iData[75], iData[73], iData[68], iData[60], iData[59]);
	CSHA512_64(	tempE, tempF, tempG, tempH, tempA, tempB, tempC, tempD, temp1, temp2,
				K[76], iData[76], iData[74], iData[69], iData[61], iData[60]);
	CSHA512_64(	tempD, tempE, tempF, tempG, tempH, tempA, tempB, tempC, temp1, temp2,
				K[77], iData[77], iData[75], iData[70], iData[62], iData[61]);
	CSHA512_64(	tempC, tempD, tempE, tempF, tempG, tempH, tempA, tempB, temp1, temp2,
				K[78], iData[78], iData[76], iData[71], iData[63], iData[62]);
	CSHA512_64(	tempB, tempC, tempD, tempE, tempF, tempG, tempH, tempA, temp1, temp2,
				K[79], iData[79], iData[77], iData[72], iData[64], iData[63]);

	iA+=tempA;
	iB+=tempB;
	iC+=tempC;
	iD+=tempD;
	iE+=tempE;
	iF+=tempF;
	iG+=tempG;
	iH+=tempH;

	iBlockIndex=0;
	}

/**
 * According to the standard, the message must be padded to an
 * even 512 bits. The first padding bit must be a '1'. The last
 * 64 bits represent the length of the original message. All bits 
 * in between should be 0. This helper function will pad the 
 * message according to those rules by filling the iData array 
 * accordingly. 
 */ 
void CSHA384And512::PadMessage()
	{
	const TUint64 padByte = 0x80;
	
	if(!(iBlockIndex&0x07))
		{
		iData[iBlockIndex >> 3] = 0;
		}
	iData[iBlockIndex >> 3] |= padByte << ((7 - iBlockIndex&0x07) << 3) ;
	
	if (iBlockIndex >= (KSHA512BlockSize - 2*sizeof(TUint64))) 
		{
		if (iBlockIndex < (KSHA512BlockSize - sizeof(TUint64)))
			iData[(KSHA512BlockSize>>3)-1]=0;		
		Block();
		Mem::FillZ(iData,KSHA512BlockSize);
		} 
	else
		{
		const TUint offset=(iBlockIndex+8)>>3; //+8 to account for the word added in the
		//switch statement above
		Mem::FillZ(iData+offset, (KSHA512BlockSize - offset*sizeof(TUint64)));
		}

	iData[(KSHA512BlockSize >> 3) - 2] = iNh;
	iData[(KSHA512BlockSize >> 3) - 1] = iNl;	
	}

inline void CSHA384And512::CopyWordToHash(TUint64 aVal, TUint aIndex)
	{
	TUint64 value = Make64BitWord(reinterpret_cast<TUint8*>(&aVal));
	Mem::Copy(const_cast<TUint8*>(iHash.Ptr())+ (8*aIndex), &value, sizeof(aVal));
	}

const TDesC8& CSHA384And512::Final()
	{
	AddLength(iBlockIndex);
	PadMessage();
	Block();
	//
	// Generate hash value into iHash
	//
	CopyWordToHash(iA, 0);
	CopyWordToHash(iB, 1);
	CopyWordToHash(iC, 2);
	CopyWordToHash(iD, 3);
	CopyWordToHash(iE, 4);
	CopyWordToHash(iF, 5);
	CopyWordToHash(iG, 6);
	CopyWordToHash(iH, 7);
	
	return iHash;
	}

void CSHA384And512::RestoreState()
	{
	iA = iACopy;
	iB = iBCopy;
	iC = iCCopy;
	iD = iDCopy;
	iE = iECopy;
	iF = iFCopy;
	iG = iGCopy;
	iH = iHCopy;
	iNl = iNlCopy;
	iNh = iNhCopy;	
	iBlockIndex = iBlockIndexCopy;
	Mem::Copy((TAny*)iData, (TAny*)iDataCopy, KSHA512BlockSize*sizeof(TUint64)); 
	}

void CSHA384And512::StoreState()
	{
	iACopy = iA;
	iBCopy = iB;
	iCCopy = iC;
	iDCopy = iD;
	iECopy = iE;
	iFCopy = iF;
	iGCopy = iG;
	iHCopy = iH;
	iNlCopy = iNl;
	iNhCopy = iNh;	
	iBlockIndexCopy = iBlockIndex;
	Mem::Copy((TAny*)iDataCopy, (TAny*)iData, KSHA512BlockSize*sizeof(TUint64));
	}

// Implemented in hmacimpl.cpp or softwarehashbase.cpp
// but required as derived from MHash. No coverage here.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

