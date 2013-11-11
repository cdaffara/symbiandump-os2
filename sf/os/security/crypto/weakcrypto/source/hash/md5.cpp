/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* (c) 2003 Symbian Ltd.  All rights reserved.
*
*/


/**
 @file
*/

#include <e32std.h>
#include <hash.h>
#include "hashinc.h"
#define EXPANDLOOP

//
// 32bit endianness independent MD5 implementation
//
CMD5::CMD5(void)
: CMessageDigest(),iHash(MD5_HASH)
	{
	}
CMD5::CMD5(const CMD5& aMD)
: CMessageDigest(aMD),
  iHash(aMD.iHash),iA(aMD.iA),iB(aMD.iB),iC(aMD.iC),iD(aMD.iD),
  iNl(aMD.iNl),iNh(aMD.iNh)
	{
	(void)Mem::Copy(iData,aMD.iData,sizeof(iData));
	}
EXPORT_C CMD5* CMD5::NewL(void)
	{
	CMD5* self = new (ELeave) CMD5();
	self->Reset();
	return (self);
	}
EXPORT_C CMessageDigest* CMD5::ReplicateL(void)
	{
	CMD5* newMD5 = CMD5::NewL();
	return (newMD5);
	}
EXPORT_C CMD5::~CMD5(void)
	{
	}

EXPORT_C TPtrC8 CMD5::Hash(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	DoUpdate(aMessage.Ptr(),aMessage.Size());
	StoreState();
	DoFinal();
	ptr.Set(iHash);
	RestoreState();
	return ptr;
	}

EXPORT_C CMessageDigest* CMD5::CopyL(void)
	{
	return new(ELeave) CMD5(*this);
	}
EXPORT_C TInt CMD5::BlockSize(void)
	{
	return sizeof(iData);
	}
EXPORT_C TInt CMD5::HashSize(void)
	{
	return MD5_HASH;
	}
EXPORT_C void CMD5::Reset(void)
	{
	iA=0x67452301;
	iB=0xefcdab89;
	iC=0x98badcfe;
	iD=0x10325476;
	iNh=0;
	iNl=0;
	}

EXPORT_C void CMD5::Update(const TDesC8& aMessage)
	{
	DoUpdate(aMessage.Ptr(),aMessage.Size());
	}
EXPORT_C TPtrC8 CMD5::Final(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	DoUpdate(aMessage.Ptr(),aMessage.Size());
	DoFinal();
	ptr.Set(iHash);
	Reset();
	return ptr;
	}

EXPORT_C TPtrC8 CMD5::Final()
	{
	TPtrC8 ptr(KNullDesC8());
	DoFinal();
	ptr.Set(iHash);
	Reset();
	return ptr;
	}

void CMD5::DoUpdate(const TUint8* aData,TUint aLength)
	{
	const TUint8* pend=aData+aLength;	
	for (const TUint8* paData=aData;paData<pend;paData++) 
		{
		const TUint8 byte=*paData;
		switch (iNl&3) 
			{
			case 0:
				iData[iNl>>2]=byte;
				break;
			case 1:
				iData[iNl>>2]|=byte<<8;
				break;
			case 2:
				iData[iNl>>2]|=byte<<16;
				break;
			case 3:
				iData[iNl>>2]|=byte<<24;
				break;
			default:
				break;
			};
		if(++iNl==64) 
			{
			Block();
			iNh+=64;
			iNl=0;
			}
		}
	}

static inline TUint CMD5_F(TUint x,TUint y,TUint z)
	{
	return (x&y) | (~x&z);
	}
static inline TUint CMD5_G(TUint x,TUint y,TUint z)
	{
	return (x&z) | (y&~z);
	}
static inline TUint CMD5_H(TUint x,TUint y,TUint z)
	{
	return x^y^z;
	}
static inline TUint CMD5_I(TUint x,TUint y,TUint z)
	{
	return y^(x|~z);
	}

	
#ifdef NOREFS
static inline TUint CMD5_FF(TUint a,const TUint b,const TUint c,const TUint d,const TUint m,const TUint s,const TUint t)
	{
	a+=CMD5_F(b,c,d) + m + t; 
	a=b + CMD_R(a,s);
	return a;
	}
static inline TUint CMD5_GG(TUint a,const TUint b,const TUint c,const TUint d,const TUint m,const TUint s,const TUint t)
	{
	a+=CMD5_G(b,c,d) + m + t; 
	a=b + CMD_R(a,s);
	return a;
	}
static inline TUint CMD5_HH(TUint a,const TUint b,const TUint c,const TUint d,const TUint m,const TUint s,const TUint t)
	{
	a+=CMD5_H(b,c,d) + m + t; 
	a=b + CMD_R(a,s);
	return a;
	}
static inline TUint CMD5_II(TUint a,const TUint b,const TUint c,const TUint d,const TUint m,const TUint s,const TUint t)
	{
	a+=CMD5_I(b,c,d) + m + t; 
	a=b + CMD_R(a,s);
	return a;
	}
void CMD5::Block()
	{
	register TUint tempA=iA;
	register TUint tempB=iB;
	register TUint tempC=iC;
	register TUint tempD=iD;

	tempA = CMD5_FF(tempA,tempB,tempC,tempD,iData[ 0], 7, 0xd76aa478);
	tempD = CMD5_FF(tempD,tempA,tempB,tempC,iData[ 1],12, 0xe8c7b756);
	tempC = CMD5_FF(tempC,tempD,tempA,tempB,iData[ 2],17, 0x242070db);
	tempB = CMD5_FF(tempB,tempC,tempD,tempA,iData[ 3],22, 0xc1bdceee);
	tempA = CMD5_FF(tempA,tempB,tempC,tempD,iData[ 4], 7, 0xf57c0faf);
	tempD = CMD5_FF(tempD,tempA,tempB,tempC,iData[ 5],12, 0x4787c62a);
	tempC = CMD5_FF(tempC,tempD,tempA,tempB,iData[ 6],17, 0xa8304613);
	tempB = CMD5_FF(tempB,tempC,tempD,tempA,iData[ 7],22, 0xfd469501);
	tempA = CMD5_FF(tempA,tempB,tempC,tempD,iData[ 8], 7, 0x698098d8);
	tempD = CMD5_FF(tempD,tempA,tempB,tempC,iData[ 9],12, 0x8b44f7af);
	tempC = CMD5_FF(tempC,tempD,tempA,tempB,iData[10],17, 0xffff5bb1);
	tempB = CMD5_FF(tempB,tempC,tempD,tempA,iData[11],22, 0x895cd7be);
	tempA = CMD5_FF(tempA,tempB,tempC,tempD,iData[12], 7, 0x6b901122);
	tempD = CMD5_FF(tempD,tempA,tempB,tempC,iData[13],12, 0xfd987193);
	tempC = CMD5_FF(tempC,tempD,tempA,tempB,iData[14],17, 0xa679438e);
	tempB = CMD5_FF(tempB,tempC,tempD,tempA,iData[15],22, 0x49b40821);

	tempA = CMD5_GG(tempA,tempB,tempC,tempD,iData[ 1], 5, 0xf61e2562);
	tempD = CMD5_GG(tempD,tempA,tempB,tempC,iData[ 6], 9, 0xc040b340);
	tempC = CMD5_GG(tempC,tempD,tempA,tempB,iData[11],14, 0x265e5a51);
	tempB = CMD5_GG(tempB,tempC,tempD,tempA,iData[ 0],20, 0xe9b6c7aa);
	tempA = CMD5_GG(tempA,tempB,tempC,tempD,iData[ 5], 5, 0xd62f105d);
	tempD = CMD5_GG(tempD,tempA,tempB,tempC,iData[10], 9, 0x02441453);
	tempC = CMD5_GG(tempC,tempD,tempA,tempB,iData[15],14, 0xd8a1e681);
	tempB = CMD5_GG(tempB,tempC,tempD,tempA,iData[ 4],20, 0xe7d3fbc8);
	tempA = CMD5_GG(tempA,tempB,tempC,tempD,iData[ 9], 5, 0x21e1cde6);
	tempD = CMD5_GG(tempD,tempA,tempB,tempC,iData[14], 9, 0xc33707d6);
	tempC = CMD5_GG(tempC,tempD,tempA,tempB,iData[ 3],14, 0xf4d50d87);
	tempB = CMD5_GG(tempB,tempC,tempD,tempA,iData[ 8],20, 0x455a14ed);
	tempA = CMD5_GG(tempA,tempB,tempC,tempD,iData[13], 5, 0xa9e3e905);
	tempD = CMD5_GG(tempD,tempA,tempB,tempC,iData[ 2], 9, 0xfcefa3f8);
	tempC = CMD5_GG(tempC,tempD,tempA,tempB,iData[ 7],14, 0x676f02d9);
	tempB = CMD5_GG(tempB,tempC,tempD,tempA,iData[12],20, 0x8d2a4c8a);

	tempA = CMD5_HH(tempA,tempB,tempC,tempD,iData[ 5], 4, 0xfffa3942);
	tempD = CMD5_HH(tempD,tempA,tempB,tempC,iData[ 8],11, 0x8771f681);
	tempC = CMD5_HH(tempC,tempD,tempA,tempB,iData[11],16, 0x6d9d6122);
	tempB = CMD5_HH(tempB,tempC,tempD,tempA,iData[14],23, 0xfde5380c);
	tempA = CMD5_HH(tempA,tempB,tempC,tempD,iData[ 1], 4, 0xa4beea44);
	tempD = CMD5_HH(tempD,tempA,tempB,tempC,iData[ 4],11, 0x4bdecfa9);
	tempC = CMD5_HH(tempC,tempD,tempA,tempB,iData[ 7],16, 0xf6bb4b60);
	tempB = CMD5_HH(tempB,tempC,tempD,tempA,iData[10],23, 0xbebfbc70);
	tempA = CMD5_HH(tempA,tempB,tempC,tempD,iData[13], 4, 0x289b7ec6);
	tempD = CMD5_HH(tempD,tempA,tempB,tempC,iData[ 0],11, 0xeaa127fa);
	tempC = CMD5_HH(tempC,tempD,tempA,tempB,iData[ 3],16, 0xd4ef3085);
	tempB = CMD5_HH(tempB,tempC,tempD,tempA,iData[ 6],23, 0x04881d05);
	tempA = CMD5_HH(tempA,tempB,tempC,tempD,iData[ 9], 4, 0xd9d4d039);
	tempD = CMD5_HH(tempD,tempA,tempB,tempC,iData[12],11, 0xe6db99e5);
	tempC = CMD5_HH(tempC,tempD,tempA,tempB,iData[15],16, 0x1fa27cf8);
	tempB = CMD5_HH(tempB,tempC,tempD,tempA,iData[ 2],23, 0xc4ac5665);

	tempA = CMD5_II(tempA,tempB,tempC,tempD,iData[ 0], 6, 0xf4292244);
	tempD = CMD5_II(tempD,tempA,tempB,tempC,iData[ 7],10, 0x432aff97);
	tempC = CMD5_II(tempC,tempD,tempA,tempB,iData[14],15, 0xab9423a7);
	tempB = CMD5_II(tempB,tempC,tempD,tempA,iData[ 5],21, 0xfc93a039);
	tempA = CMD5_II(tempA,tempB,tempC,tempD,iData[12], 6, 0x655b59c3);
	tempD = CMD5_II(tempD,tempA,tempB,tempC,iData[ 3],10, 0x8f0ccc92);
	tempC = CMD5_II(tempC,tempD,tempA,tempB,iData[10],15, 0xffeff47d);
	tempB = CMD5_II(tempB,tempC,tempD,tempA,iData[ 1],21, 0x85845dd1);
	tempA = CMD5_II(tempA,tempB,tempC,tempD,iData[ 8], 6, 0x6fa87e4f);
	tempD = CMD5_II(tempD,tempA,tempB,tempC,iData[15],10, 0xfe2ce6e0);
	tempC = CMD5_II(tempC,tempD,tempA,tempB,iData[ 6],15, 0xa3014314);
	tempB = CMD5_II(tempB,tempC,tempD,tempA,iData[13],21, 0x4e0811a1);
	tempA = CMD5_II(tempA,tempB,tempC,tempD,iData[ 4], 6, 0xf7537e82);
	tempD = CMD5_II(tempD,tempA,tempB,tempC,iData[11],10, 0xbd3af235);
	tempC = CMD5_II(tempC,tempD,tempA,tempB,iData[ 2],15, 0x2ad7d2bb);
	tempB = CMD5_II(tempB,tempC,tempD,tempA,iData[ 9],21, 0xeb86d391);

	iA+=tempA;
	iB+=tempB;
	iC+=tempC;
	iD+=tempD;
	}
#else
#ifdef MACRO
#define CMD5_FF(a, b, c, d, m, s, t) (b + CMD_R(a += CMD5_F(b,c,d) + m + t, s))
#define CMD5_GG(a, b, c, d, m, s, t) (b + CMD_R(a += CMD5_G(b,c,d) + m + t, s))
#define CMD5_HH(a, b, c, d, m, s, t) (b + CMD_R(a += CMD5_H(b,c,d) + m + t, s))
#define CMD5_II(a, b, c, d, m, s, t) (b + CMD_R(a += CMD5_I(b,c,d) + m + t, s))
void CMD5::Block()
	{
	register TUint tempA=iA;
	register TUint tempB=iB;
	register TUint tempC=iC;
	register TUint tempD=iD;

	tempA = CMD5_FF(tempA,tempB,tempC,tempD,iData[ 0], 7, 0xd76aa478);
	tempD = CMD5_FF(tempD,tempA,tempB,tempC,iData[ 1],12, 0xe8c7b756);
	tempC = CMD5_FF(tempC,tempD,tempA,tempB,iData[ 2],17, 0x242070db);
	tempB = CMD5_FF(tempB,tempC,tempD,tempA,iData[ 3],22, 0xc1bdceee);
	tempA = CMD5_FF(tempA,tempB,tempC,tempD,iData[ 4], 7, 0xf57c0faf);
	tempD = CMD5_FF(tempD,tempA,tempB,tempC,iData[ 5],12, 0x4787c62a);
	tempC = CMD5_FF(tempC,tempD,tempA,tempB,iData[ 6],17, 0xa8304613);
	tempB = CMD5_FF(tempB,tempC,tempD,tempA,iData[ 7],22, 0xfd469501);
	tempA = CMD5_FF(tempA,tempB,tempC,tempD,iData[ 8], 7, 0x698098d8);
	tempD = CMD5_FF(tempD,tempA,tempB,tempC,iData[ 9],12, 0x8b44f7af);
	tempC = CMD5_FF(tempC,tempD,tempA,tempB,iData[10],17, 0xffff5bb1);
	tempB = CMD5_FF(tempB,tempC,tempD,tempA,iData[11],22, 0x895cd7be);
	tempA = CMD5_FF(tempA,tempB,tempC,tempD,iData[12], 7, 0x6b901122);
	tempD = CMD5_FF(tempD,tempA,tempB,tempC,iData[13],12, 0xfd987193);
	tempC = CMD5_FF(tempC,tempD,tempA,tempB,iData[14],17, 0xa679438e);
	tempB = CMD5_FF(tempB,tempC,tempD,tempA,iData[15],22, 0x49b40821);

	tempA = CMD5_GG(tempA,tempB,tempC,tempD,iData[ 1], 5, 0xf61e2562);
	tempD = CMD5_GG(tempD,tempA,tempB,tempC,iData[ 6], 9, 0xc040b340);
	tempC = CMD5_GG(tempC,tempD,tempA,tempB,iData[11],14, 0x265e5a51);
	tempB = CMD5_GG(tempB,tempC,tempD,tempA,iData[ 0],20, 0xe9b6c7aa);
	tempA = CMD5_GG(tempA,tempB,tempC,tempD,iData[ 5], 5, 0xd62f105d);
	tempD = CMD5_GG(tempD,tempA,tempB,tempC,iData[10], 9, 0x02441453);
	tempC = CMD5_GG(tempC,tempD,tempA,tempB,iData[15],14, 0xd8a1e681);
	tempB = CMD5_GG(tempB,tempC,tempD,tempA,iData[ 4],20, 0xe7d3fbc8);
	tempA = CMD5_GG(tempA,tempB,tempC,tempD,iData[ 9], 5, 0x21e1cde6);
	tempD = CMD5_GG(tempD,tempA,tempB,tempC,iData[14], 9, 0xc33707d6);
	tempC = CMD5_GG(tempC,tempD,tempA,tempB,iData[ 3],14, 0xf4d50d87);
	tempB = CMD5_GG(tempB,tempC,tempD,tempA,iData[ 8],20, 0x455a14ed);
	tempA = CMD5_GG(tempA,tempB,tempC,tempD,iData[13], 5, 0xa9e3e905);
	tempD = CMD5_GG(tempD,tempA,tempB,tempC,iData[ 2], 9, 0xfcefa3f8);
	tempC = CMD5_GG(tempC,tempD,tempA,tempB,iData[ 7],14, 0x676f02d9);
	tempB = CMD5_GG(tempB,tempC,tempD,tempA,iData[12],20, 0x8d2a4c8a);

	tempA = CMD5_HH(tempA,tempB,tempC,tempD,iData[ 5], 4, 0xfffa3942);
	tempD = CMD5_HH(tempD,tempA,tempB,tempC,iData[ 8],11, 0x8771f681);
	tempC = CMD5_HH(tempC,tempD,tempA,tempB,iData[11],16, 0x6d9d6122);
	tempB = CMD5_HH(tempB,tempC,tempD,tempA,iData[14],23, 0xfde5380c);
	tempA = CMD5_HH(tempA,tempB,tempC,tempD,iData[ 1], 4, 0xa4beea44);
	tempD = CMD5_HH(tempD,tempA,tempB,tempC,iData[ 4],11, 0x4bdecfa9);
	tempC = CMD5_HH(tempC,tempD,tempA,tempB,iData[ 7],16, 0xf6bb4b60);
	tempB = CMD5_HH(tempB,tempC,tempD,tempA,iData[10],23, 0xbebfbc70);
	tempA = CMD5_HH(tempA,tempB,tempC,tempD,iData[13], 4, 0x289b7ec6);
	tempD = CMD5_HH(tempD,tempA,tempB,tempC,iData[ 0],11, 0xeaa127fa);
	tempC = CMD5_HH(tempC,tempD,tempA,tempB,iData[ 3],16, 0xd4ef3085);
	tempB = CMD5_HH(tempB,tempC,tempD,tempA,iData[ 6],23, 0x04881d05);
	tempA = CMD5_HH(tempA,tempB,tempC,tempD,iData[ 9], 4, 0xd9d4d039);
	tempD = CMD5_HH(tempD,tempA,tempB,tempC,iData[12],11, 0xe6db99e5);
	tempC = CMD5_HH(tempC,tempD,tempA,tempB,iData[15],16, 0x1fa27cf8);
	tempB = CMD5_HH(tempB,tempC,tempD,tempA,iData[ 2],23, 0xc4ac5665);

	tempA = CMD5_II(tempA,tempB,tempC,tempD,iData[ 0], 6, 0xf4292244);
	tempD = CMD5_II(tempD,tempA,tempB,tempC,iData[ 7],10, 0x432aff97);
	tempC = CMD5_II(tempC,tempD,tempA,tempB,iData[14],15, 0xab9423a7);
	tempB = CMD5_II(tempB,tempC,tempD,tempA,iData[ 5],21, 0xfc93a039);
	tempA = CMD5_II(tempA,tempB,tempC,tempD,iData[12], 6, 0x655b59c3);
	tempD = CMD5_II(tempD,tempA,tempB,tempC,iData[ 3],10, 0x8f0ccc92);
	tempC = CMD5_II(tempC,tempD,tempA,tempB,iData[10],15, 0xffeff47d);
	tempB = CMD5_II(tempB,tempC,tempD,tempA,iData[ 1],21, 0x85845dd1);
	tempA = CMD5_II(tempA,tempB,tempC,tempD,iData[ 8], 6, 0x6fa87e4f);
	tempD = CMD5_II(tempD,tempA,tempB,tempC,iData[15],10, 0xfe2ce6e0);
	tempC = CMD5_II(tempC,tempD,tempA,tempB,iData[ 6],15, 0xa3014314);
	tempB = CMD5_II(tempB,tempC,tempD,tempA,iData[13],21, 0x4e0811a1);
	tempA = CMD5_II(tempA,tempB,tempC,tempD,iData[ 4], 6, 0xf7537e82);
	tempD = CMD5_II(tempD,tempA,tempB,tempC,iData[11],10, 0xbd3af235);
	tempC = CMD5_II(tempC,tempD,tempA,tempB,iData[ 2],15, 0x2ad7d2bb);
	tempB = CMD5_II(tempB,tempC,tempD,tempA,iData[ 9],21, 0xeb86d391);

	iA+=tempA;
	iB+=tempB;
	iC+=tempC;
	iD+=tempD;
	}
#else
static inline void CMD5_FF(TUint& a,const TUint b,const TUint c,const TUint d,const TUint m,const TUint s,const TUint t)
	{
	a+=CMD5_F(b,c,d) + m + t; 
	a=b + CMD_R(a,s);
	}
static inline void CMD5_GG(TUint& a,const TUint b,const TUint c,const TUint d,const TUint m,const TUint s,const TUint t)
	{
	a+=CMD5_G(b,c,d) + m + t; 
	a=b + CMD_R(a,s);
	}
static inline void CMD5_HH(TUint& a,const TUint b,const TUint c,const TUint d,const TUint m,const TUint s,const TUint t)
	{
	a+=CMD5_H(b,c,d) + m + t; 
	a=b + CMD_R(a,s);
	}
static inline void CMD5_II(TUint& a,const TUint b,const TUint c,const TUint d,const TUint m,const TUint s,const TUint t)
	{
	a+=CMD5_I(b,c,d) + m + t; 
	a=b + CMD_R(a,s);
	}
void CMD5::Block()
	{
	register TUint tempA=iA;
	register TUint tempB=iB;
	register TUint tempC=iC;
	register TUint tempD=iD;

	CMD5_FF(tempA,tempB,tempC,tempD,iData[ 0], 7, 0xd76aa478);
	CMD5_FF(tempD,tempA,tempB,tempC,iData[ 1],12, 0xe8c7b756);
	CMD5_FF(tempC,tempD,tempA,tempB,iData[ 2],17, 0x242070db);
	CMD5_FF(tempB,tempC,tempD,tempA,iData[ 3],22, 0xc1bdceee);
	CMD5_FF(tempA,tempB,tempC,tempD,iData[ 4], 7, 0xf57c0faf);
	CMD5_FF(tempD,tempA,tempB,tempC,iData[ 5],12, 0x4787c62a);
	CMD5_FF(tempC,tempD,tempA,tempB,iData[ 6],17, 0xa8304613);
	CMD5_FF(tempB,tempC,tempD,tempA,iData[ 7],22, 0xfd469501);
	CMD5_FF(tempA,tempB,tempC,tempD,iData[ 8], 7, 0x698098d8);
	CMD5_FF(tempD,tempA,tempB,tempC,iData[ 9],12, 0x8b44f7af);
	CMD5_FF(tempC,tempD,tempA,tempB,iData[10],17, 0xffff5bb1);
	CMD5_FF(tempB,tempC,tempD,tempA,iData[11],22, 0x895cd7be);
	CMD5_FF(tempA,tempB,tempC,tempD,iData[12], 7, 0x6b901122);
	CMD5_FF(tempD,tempA,tempB,tempC,iData[13],12, 0xfd987193);
	CMD5_FF(tempC,tempD,tempA,tempB,iData[14],17, 0xa679438e);
	CMD5_FF(tempB,tempC,tempD,tempA,iData[15],22, 0x49b40821);

	CMD5_GG(tempA,tempB,tempC,tempD,iData[ 1], 5, 0xf61e2562);
	CMD5_GG(tempD,tempA,tempB,tempC,iData[ 6], 9, 0xc040b340);
	CMD5_GG(tempC,tempD,tempA,tempB,iData[11],14, 0x265e5a51);
	CMD5_GG(tempB,tempC,tempD,tempA,iData[ 0],20, 0xe9b6c7aa);
	CMD5_GG(tempA,tempB,tempC,tempD,iData[ 5], 5, 0xd62f105d);
	CMD5_GG(tempD,tempA,tempB,tempC,iData[10], 9, 0x02441453);
	CMD5_GG(tempC,tempD,tempA,tempB,iData[15],14, 0xd8a1e681);
	CMD5_GG(tempB,tempC,tempD,tempA,iData[ 4],20, 0xe7d3fbc8);
	CMD5_GG(tempA,tempB,tempC,tempD,iData[ 9], 5, 0x21e1cde6);
	CMD5_GG(tempD,tempA,tempB,tempC,iData[14], 9, 0xc33707d6);
	CMD5_GG(tempC,tempD,tempA,tempB,iData[ 3],14, 0xf4d50d87);
	CMD5_GG(tempB,tempC,tempD,tempA,iData[ 8],20, 0x455a14ed);
	CMD5_GG(tempA,tempB,tempC,tempD,iData[13], 5, 0xa9e3e905);
	CMD5_GG(tempD,tempA,tempB,tempC,iData[ 2], 9, 0xfcefa3f8);
	CMD5_GG(tempC,tempD,tempA,tempB,iData[ 7],14, 0x676f02d9);
	CMD5_GG(tempB,tempC,tempD,tempA,iData[12],20, 0x8d2a4c8a);

	CMD5_HH(tempA,tempB,tempC,tempD,iData[ 5], 4, 0xfffa3942);
	CMD5_HH(tempD,tempA,tempB,tempC,iData[ 8],11, 0x8771f681);
	CMD5_HH(tempC,tempD,tempA,tempB,iData[11],16, 0x6d9d6122);
	CMD5_HH(tempB,tempC,tempD,tempA,iData[14],23, 0xfde5380c);
	CMD5_HH(tempA,tempB,tempC,tempD,iData[ 1], 4, 0xa4beea44);
	CMD5_HH(tempD,tempA,tempB,tempC,iData[ 4],11, 0x4bdecfa9);
	CMD5_HH(tempC,tempD,tempA,tempB,iData[ 7],16, 0xf6bb4b60);
	CMD5_HH(tempB,tempC,tempD,tempA,iData[10],23, 0xbebfbc70);
	CMD5_HH(tempA,tempB,tempC,tempD,iData[13], 4, 0x289b7ec6);
	CMD5_HH(tempD,tempA,tempB,tempC,iData[ 0],11, 0xeaa127fa);
	CMD5_HH(tempC,tempD,tempA,tempB,iData[ 3],16, 0xd4ef3085);
	CMD5_HH(tempB,tempC,tempD,tempA,iData[ 6],23, 0x04881d05);
	CMD5_HH(tempA,tempB,tempC,tempD,iData[ 9], 4, 0xd9d4d039);
	CMD5_HH(tempD,tempA,tempB,tempC,iData[12],11, 0xe6db99e5);
	CMD5_HH(tempC,tempD,tempA,tempB,iData[15],16, 0x1fa27cf8);
	CMD5_HH(tempB,tempC,tempD,tempA,iData[ 2],23, 0xc4ac5665);

	CMD5_II(tempA,tempB,tempC,tempD,iData[ 0], 6, 0xf4292244);
	CMD5_II(tempD,tempA,tempB,tempC,iData[ 7],10, 0x432aff97);
	CMD5_II(tempC,tempD,tempA,tempB,iData[14],15, 0xab9423a7);
	CMD5_II(tempB,tempC,tempD,tempA,iData[ 5],21, 0xfc93a039);
	CMD5_II(tempA,tempB,tempC,tempD,iData[12], 6, 0x655b59c3);
	CMD5_II(tempD,tempA,tempB,tempC,iData[ 3],10, 0x8f0ccc92);
	CMD5_II(tempC,tempD,tempA,tempB,iData[10],15, 0xffeff47d);
	CMD5_II(tempB,tempC,tempD,tempA,iData[ 1],21, 0x85845dd1);
	CMD5_II(tempA,tempB,tempC,tempD,iData[ 8], 6, 0x6fa87e4f);
	CMD5_II(tempD,tempA,tempB,tempC,iData[15],10, 0xfe2ce6e0);
	CMD5_II(tempC,tempD,tempA,tempB,iData[ 6],15, 0xa3014314);
	CMD5_II(tempB,tempC,tempD,tempA,iData[13],21, 0x4e0811a1);
	CMD5_II(tempA,tempB,tempC,tempD,iData[ 4], 6, 0xf7537e82);
	CMD5_II(tempD,tempA,tempB,tempC,iData[11],10, 0xbd3af235);
	CMD5_II(tempC,tempD,tempA,tempB,iData[ 2],15, 0x2ad7d2bb);
	CMD5_II(tempB,tempC,tempD,tempA,iData[ 9],21, 0xeb86d391);

	iA+=tempA;
	iB+=tempB;
	iC+=tempC;
	iD+=tempD;
	}
#endif
#endif

void CMD5::DoFinal(void)
	{
	iNh += iNl;
	const TUint ul128=128;
	switch (iNl&3) 
		{
		case 0:
			iData[iNl>>2] = ul128;
			break;
		case 1:
			iData[iNl>>2] += ul128<<8;
			break;
		case 2:
			iData[iNl>>2] += ul128<<16;
			break;
		case 3:
			iData[iNl>>2] += ul128<<24;
			break;
		default:
			break;
		};
	if (iNl>=56) 
		{
		if (iNl<60)
			iData[15]=0;		
		Block();
		Mem::FillZ(iData,14*sizeof(TUint));
		} 
	else
		{
		const TUint offset=(iNl+4)>>2;
		Mem::FillZ(iData+offset,(14-offset)*sizeof(TUint));
		}

	iData[14]=iNh<<3;//number in bits
	// this will fail if the total input length is longer than 2^32 in bits
	//(2^31 in bytes) which is roughly half a gig.
	iData[15]=0;
	Block();
	//
	// Generate hash value into iHash
	//
	TUint tmp=iA;
	iHash[0]=(TUint8)(tmp & 255);
	iHash[1]=(TUint8)((tmp >>= 8) & 255);
	iHash[2]=(TUint8)((tmp >>= 8) & 255);
	iHash[3]=(TUint8)((tmp >>= 8) & 255);

	tmp=iB;
	iHash[4]=(TUint8)(tmp & 255);
	iHash[5]=(TUint8)((tmp >>= 8) & 255);
	iHash[6]=(TUint8)((tmp >>= 8) & 255);
	iHash[7]=(TUint8)((tmp >>= 8) & 255);

	tmp=iC;
	iHash[8]=(TUint8)(tmp & 255);
	iHash[9]=(TUint8)((tmp >>= 8) & 255);
	iHash[10]=(TUint8)((tmp >>= 8) & 255);
	iHash[11]=(TUint8)((tmp >>= 8) & 255);

	tmp=iD;
	iHash[12]=(TUint8)(tmp & 255);
	iHash[13]=(TUint8)((tmp >>= 8) & 255);
	iHash[14]=(TUint8)((tmp >>= 8) & 255);
	iHash[15]=(TUint8)((tmp >>= 8) & 255);
	}

void CMD5::RestoreState()
{
	iA = iACopy;
	iB = iBCopy;
	iC = iCCopy;
	iD = iDCopy;
	iNl = iNlCopy;
	iNh = iNhCopy;	
	Mem::Copy(&iData[0], &iDataCopy[0], MD5_LBLOCK*sizeof(TUint)); 
}

void CMD5::StoreState()
{
	iACopy = iA;
	iBCopy = iB;
	iCCopy = iC;
	iDCopy = iD;
	iNlCopy = iNl;
	iNhCopy = iNh;	
	Mem::Copy(&iDataCopy[0], &iData[0], MD5_LBLOCK*sizeof(TUint));
}
