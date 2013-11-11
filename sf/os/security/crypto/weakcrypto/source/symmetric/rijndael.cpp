/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "rijndael.h"
#include "rijndaeltables.h"
#include "../common/inlines.h"
#include <cryptostrength.h>

const TUint KAESKeyBytes128 = 16;
const TUint KAESKeyBytes192 = 24;
const TUint KAESKeyBytes256 = 32;
const TUint KAESBlockBytes = 16;

/* CRijndael */
EXPORT_C CRijndael::CRijndael(void)
	{
	}

void CRijndael::Reset()
	{
	SetKey(*iKey);
	}

TInt CRijndael::KeySize() const
	{
	return (4*(iRounds+1));
	}

CRijndael::~CRijndael()
	{
	delete iKey;
	}

void CRijndael::ConstructL(const TDesC8& aKey)
	{
	TUint keySize = aKey.Size();
	assert((keySize==KAESKeyBytes128)||(keySize==KAESKeyBytes192)||(keySize==KAESKeyBytes256));
	iKey = aKey.AllocL();
	iRounds = keySize/4 + 6;
	SetKey(aKey);
	}

void CRijndael::SetKey(const TDesC8& aKey)
	{
	TUint keySize = aKey.Size();
	TUint32 temp; 
	TUint32* rk = &iK[0];

	TUint i = 0;

	GetUserKeyBigEndian(rk, keySize/4, &aKey[0], keySize);

	switch(keySize)
		{
		case (KAESKeyBytes128):
			{
			FOREVER
				{
				temp  = rk[3];
				rk[4] = rk[0] ^
					(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 2)] & 0xff000000) ^
					(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 1)] & 0x00ff0000) ^
					(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 0)] & 0x0000ff00) ^
					(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 3)] & 0x000000ff) ^
					RIJNDAEL_TABLE::rcon[i];
				rk[5] = rk[1] ^ rk[4];
				rk[6] = rk[2] ^ rk[5];
				rk[7] = rk[3] ^ rk[6];
				if (++i == 10)
					break;
				rk += 4;
				}
			}
		break;

		case (KAESKeyBytes192):
			{
			FOREVER
				{
				temp = rk[ 5];
				rk[ 6] = rk[ 0] ^
					(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 2)] & 0xff000000) ^
					(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 1)] & 0x00ff0000) ^
					(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 0)] & 0x0000ff00) ^
					(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 3)] & 0x000000ff) ^
					RIJNDAEL_TABLE::rcon[i];
				rk[ 7] = rk[ 1] ^ rk[ 6];
				rk[ 8] = rk[ 2] ^ rk[ 7];
				rk[ 9] = rk[ 3] ^ rk[ 8];
				if (++i == 8)
					break;
				rk[10] = rk[ 4] ^ rk[ 9];
				rk[11] = rk[ 5] ^ rk[10];
				rk += 6;
				}
			}
		break;

		case (KAESKeyBytes256):
			{
			FOREVER
				{
        		temp = rk[ 7];
        		rk[ 8] = rk[ 0] ^
        			(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 2)] & 0xff000000) ^
        			(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 1)] & 0x00ff0000) ^
        			(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 0)] & 0x0000ff00) ^
        			(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 3)] & 0x000000ff) ^
        			RIJNDAEL_TABLE::rcon[i];
        		rk[ 9] = rk[ 1] ^ rk[ 8];
        		rk[10] = rk[ 2] ^ rk[ 9];
        		rk[11] = rk[ 3] ^ rk[10];
				if (++i == 7)
					break;
        		temp = rk[11];
        		rk[12] = rk[ 4] ^
        			(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 3)] & 0xff000000) ^
        			(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 2)] & 0x00ff0000) ^
        			(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 1)] & 0x0000ff00) ^
        			(RIJNDAEL_TABLE::Te4[GETBYTE(temp, 0)] & 0x000000ff);
        		rk[13] = rk[ 5] ^ rk[12];
        		rk[14] = rk[ 6] ^ rk[13];
        		rk[15] = rk[ 7] ^ rk[14];

				rk += 8;
				}
			}
		break;

		default:
			assert(0);	//	Shouldn't get here, keeps compiler happy
		}
	}


/* CAESEncryptor */
EXPORT_C CAESEncryptor* CAESEncryptor::NewL(const TDesC8& aKey)
	{
	CAESEncryptor* me = CAESEncryptor::NewLC(aKey);
	CleanupStack::Pop(me);
	return (me);
	}

EXPORT_C CAESEncryptor* CAESEncryptor::NewLC(const TDesC8& aKey)
	{
	CAESEncryptor* me = new (ELeave) CAESEncryptor();
	CleanupStack::PushL(me);
	me->ConstructL(aKey);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()));
	return (me);
	}

TInt CAESEncryptor::BlockSize() const
	{
	return KAESBlockBytes;
	}

void CAESEncryptor::Transform(TDes8& aBlock)
	{
	assert((TUint)aBlock.Size()==KAESBlockBytes);
	
	TUint32 s0, s1, s2, s3, t0, t1, t2, t3;
	const TUint32* rk = &iK[0];

/*
 *	map byte array block to cipher state
 *	and add initial round key:
*/
	GetBlockBigEndian((TUint8*)&aBlock[0], s0, s1, s2, s3);
	s0 ^= rk[0];
	s1 ^= rk[1];
	s2 ^= rk[2];
	s3 ^= rk[3];
/*
 *	Nr - 1 full rounds:
*/
    TUint r = iRounds >> 1;
	FOREVER
		{
        t0 =
            RIJNDAEL_TABLE::Te0[GETBYTE(s0, 3)] ^
            RIJNDAEL_TABLE::Te1[GETBYTE(s1, 2)] ^
            RIJNDAEL_TABLE::Te2[GETBYTE(s2, 1)] ^
            RIJNDAEL_TABLE::Te3[GETBYTE(s3, 0)] ^
            rk[4];
        t1 =
            RIJNDAEL_TABLE::Te0[GETBYTE(s1, 3)] ^
            RIJNDAEL_TABLE::Te1[GETBYTE(s2, 2)] ^
            RIJNDAEL_TABLE::Te2[GETBYTE(s3, 1)] ^
            RIJNDAEL_TABLE::Te3[GETBYTE(s0, 0)] ^
            rk[5];
        t2 =
            RIJNDAEL_TABLE::Te0[GETBYTE(s2, 3)] ^
            RIJNDAEL_TABLE::Te1[GETBYTE(s3, 2)] ^
            RIJNDAEL_TABLE::Te2[GETBYTE(s0, 1)] ^
            RIJNDAEL_TABLE::Te3[GETBYTE(s1, 0)] ^
            rk[6];
        t3 =
            RIJNDAEL_TABLE::Te0[GETBYTE(s3, 3)] ^
            RIJNDAEL_TABLE::Te1[GETBYTE(s0, 2)] ^
            RIJNDAEL_TABLE::Te2[GETBYTE(s1, 1)] ^
            RIJNDAEL_TABLE::Te3[GETBYTE(s2, 0)] ^
            rk[7];

        rk += 8;
        if (--r == 0) 
			break;
        
        s0 =
            RIJNDAEL_TABLE::Te0[GETBYTE(t0, 3)] ^
            RIJNDAEL_TABLE::Te1[GETBYTE(t1, 2)] ^
            RIJNDAEL_TABLE::Te2[GETBYTE(t2, 1)] ^
            RIJNDAEL_TABLE::Te3[GETBYTE(t3, 0)] ^
            rk[0];
        s1 =
            RIJNDAEL_TABLE::Te0[GETBYTE(t1, 3)] ^
            RIJNDAEL_TABLE::Te1[GETBYTE(t2, 2)] ^
            RIJNDAEL_TABLE::Te2[GETBYTE(t3, 1)] ^
            RIJNDAEL_TABLE::Te3[GETBYTE(t0, 0)] ^
            rk[1];
        s2 =
            RIJNDAEL_TABLE::Te0[GETBYTE(t2, 3)] ^
            RIJNDAEL_TABLE::Te1[GETBYTE(t3, 2)] ^
            RIJNDAEL_TABLE::Te2[GETBYTE(t0, 1)] ^
            RIJNDAEL_TABLE::Te3[GETBYTE(t1, 0)] ^
            rk[2];
        s3 =
            RIJNDAEL_TABLE::Te0[GETBYTE(t3, 3)] ^
            RIJNDAEL_TABLE::Te1[GETBYTE(t0, 2)] ^
            RIJNDAEL_TABLE::Te2[GETBYTE(t1, 1)] ^
            RIJNDAEL_TABLE::Te3[GETBYTE(t2, 0)] ^
            rk[3];
		}
/*
 *	apply last round and
 *	map cipher state to byte array block:
*/

	s0 =
		(RIJNDAEL_TABLE::Te4[GETBYTE(t0, 3)] & 0xff000000) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t1, 2)] & 0x00ff0000) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t2, 1)] & 0x0000ff00) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t3, 0)] & 0x000000ff) ^
		rk[0];
	s1 =
		(RIJNDAEL_TABLE::Te4[GETBYTE(t1, 3)] & 0xff000000) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t2, 2)] & 0x00ff0000) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t3, 1)] & 0x0000ff00) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t0, 0)] & 0x000000ff) ^
		rk[1];
	s2 =
		(RIJNDAEL_TABLE::Te4[GETBYTE(t2, 3)] & 0xff000000) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t3, 2)] & 0x00ff0000) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t0, 1)] & 0x0000ff00) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t1, 0)] & 0x000000ff) ^
		rk[2];
	s3 =
		(RIJNDAEL_TABLE::Te4[GETBYTE(t3, 3)] & 0xff000000) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t0, 2)] & 0x00ff0000) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t1, 1)] & 0x0000ff00) ^
		(RIJNDAEL_TABLE::Te4[GETBYTE(t2, 0)] & 0x000000ff) ^
		rk[3];

	PutBlockBigEndian((TUint8*)&aBlock[0], s0, s1, s2, s3);
	}

CAESEncryptor::CAESEncryptor(void)
	{
	}

/* CAESDecryptor */
EXPORT_C CAESDecryptor* CAESDecryptor::NewL(const TDesC8& aKey)
	{
	CAESDecryptor* me = CAESDecryptor::NewLC(aKey);
	CleanupStack::Pop(me);
	return (me);
	}


EXPORT_C CAESDecryptor* CAESDecryptor::NewLC(const TDesC8& aKey)
	{
	CAESDecryptor* me = new (ELeave) CAESDecryptor();
	CleanupStack::PushL(me);
	me->ConstructL(aKey);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(aKey.Size()));
	return (me);
	}

TInt CAESDecryptor::BlockSize() const
	{
	return KAESBlockBytes;
	}

void CAESDecryptor::Transform(TDes8& aBlock)
	{
	TUint32 s0, s1, s2, s3, t0, t1, t2, t3;
    const TUint32* rk = &iK[0];

/*
 *	map byte array block to cipher state
 *	and add initial round key:
*/
	GetBlockBigEndian((TUint8*)&aBlock[0], s0, s1, s2, s3);

	s0 ^= rk[0];
	s1 ^= rk[1];
	s2 ^= rk[2];
	s3 ^= rk[3];
/*
 *	Nr - 1 full rounds:
*/
    TUint r = iRounds >> 1;
    FOREVER
		{
        t0 =
            RIJNDAEL_TABLE::Td0[GETBYTE(s0, 3)] ^
            RIJNDAEL_TABLE::Td1[GETBYTE(s3, 2)] ^
            RIJNDAEL_TABLE::Td2[GETBYTE(s2, 1)] ^
            RIJNDAEL_TABLE::Td3[GETBYTE(s1, 0)] ^
            rk[4];
        t1 =
            RIJNDAEL_TABLE::Td0[GETBYTE(s1, 3)] ^
            RIJNDAEL_TABLE::Td1[GETBYTE(s0, 2)] ^
            RIJNDAEL_TABLE::Td2[GETBYTE(s3, 1)] ^
            RIJNDAEL_TABLE::Td3[GETBYTE(s2, 0)] ^
            rk[5];
        t2 =
            RIJNDAEL_TABLE::Td0[GETBYTE(s2, 3)] ^
            RIJNDAEL_TABLE::Td1[GETBYTE(s1, 2)] ^
            RIJNDAEL_TABLE::Td2[GETBYTE(s0, 1)] ^
            RIJNDAEL_TABLE::Td3[GETBYTE(s3, 0)] ^
            rk[6];
        t3 =
            RIJNDAEL_TABLE::Td0[GETBYTE(s3, 3)] ^
            RIJNDAEL_TABLE::Td1[GETBYTE(s2, 2)] ^
            RIJNDAEL_TABLE::Td2[GETBYTE(s1, 1)] ^
            RIJNDAEL_TABLE::Td3[GETBYTE(s0, 0)] ^
            rk[7];

        rk += 8;
        if (--r == 0)
            break;
        
        s0 =
            RIJNDAEL_TABLE::Td0[GETBYTE(t0, 3)] ^
            RIJNDAEL_TABLE::Td1[GETBYTE(t3, 2)] ^
            RIJNDAEL_TABLE::Td2[GETBYTE(t2, 1)] ^
            RIJNDAEL_TABLE::Td3[GETBYTE(t1, 0)] ^
            rk[0];
        s1 =
            RIJNDAEL_TABLE::Td0[GETBYTE(t1, 3)] ^
            RIJNDAEL_TABLE::Td1[GETBYTE(t0, 2)] ^
            RIJNDAEL_TABLE::Td2[GETBYTE(t3, 1)] ^
            RIJNDAEL_TABLE::Td3[GETBYTE(t2, 0)] ^
            rk[1];
        s2 =
            RIJNDAEL_TABLE::Td0[GETBYTE(t2, 3)] ^
            RIJNDAEL_TABLE::Td1[GETBYTE(t1, 2)] ^
            RIJNDAEL_TABLE::Td2[GETBYTE(t0, 1)] ^
            RIJNDAEL_TABLE::Td3[GETBYTE(t3, 0)] ^
            rk[2];
        s3 =
            RIJNDAEL_TABLE::Td0[GETBYTE(t3, 3)] ^
            RIJNDAEL_TABLE::Td1[GETBYTE(t2, 2)] ^
            RIJNDAEL_TABLE::Td2[GETBYTE(t1, 1)] ^
            RIJNDAEL_TABLE::Td3[GETBYTE(t0, 0)] ^
            rk[3];
		}
/*
 *	apply last round and
 *	map cipher state to byte array block:
*/
   	s0 =
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t0, 3)] & 0xff000000) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t3, 2)] & 0x00ff0000) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t2, 1)] & 0x0000ff00) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t1, 0)] & 0x000000ff) ^
   		rk[0];
   	s1 =
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t1, 3)] & 0xff000000) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t0, 2)] & 0x00ff0000) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t3, 1)] & 0x0000ff00) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t2, 0)] & 0x000000ff) ^
   		rk[1];
   	s2 =
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t2, 3)] & 0xff000000) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t1, 2)] & 0x00ff0000) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t0, 1)] & 0x0000ff00) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t3, 0)] & 0x000000ff) ^
   		rk[2];
   	s3 =
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t3, 3)] & 0xff000000) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t2, 2)] & 0x00ff0000) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t1, 1)] & 0x0000ff00) ^
   		(RIJNDAEL_TABLE::Td4[GETBYTE(t0, 0)] & 0x000000ff) ^
   		rk[3];

	
	PutBlockBigEndian((TUint8*)&aBlock[0], s0, s1, s2, s3);
	}


void CAESDecryptor::SetKey(const TDesC8& aKey)
	{
	CRijndael::SetKey(aKey);

	TUint i, j;
	TUint32* rk = &iK[0];
	TUint32 temp;

//	invert the order of the round keys 
	for (i = 0, j = 4*iRounds; i < j; i += 4, j -= 4)
		{
		temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
		temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
		temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
		temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
		}

//	apply the inverse MixColumn transform to all round keys but the first and the last
	for (i = 1; i < iRounds; i++)
		{
		rk += 4;
		rk[0] =
			RIJNDAEL_TABLE::Td0[RIJNDAEL_TABLE::Te4[GETBYTE(rk[0], 3)] & 0xff] ^
			RIJNDAEL_TABLE::Td1[RIJNDAEL_TABLE::Te4[GETBYTE(rk[0], 2)] & 0xff] ^
			RIJNDAEL_TABLE::Td2[RIJNDAEL_TABLE::Te4[GETBYTE(rk[0], 1)] & 0xff] ^
			RIJNDAEL_TABLE::Td3[RIJNDAEL_TABLE::Te4[GETBYTE(rk[0], 0)] & 0xff];
		rk[1] =
			RIJNDAEL_TABLE::Td0[RIJNDAEL_TABLE::Te4[GETBYTE(rk[1], 3)] & 0xff] ^
			RIJNDAEL_TABLE::Td1[RIJNDAEL_TABLE::Te4[GETBYTE(rk[1], 2)] & 0xff] ^
			RIJNDAEL_TABLE::Td2[RIJNDAEL_TABLE::Te4[GETBYTE(rk[1], 1)] & 0xff] ^
			RIJNDAEL_TABLE::Td3[RIJNDAEL_TABLE::Te4[GETBYTE(rk[1], 0)] & 0xff];
		rk[2] =
			RIJNDAEL_TABLE::Td0[RIJNDAEL_TABLE::Te4[GETBYTE(rk[2], 3)] & 0xff] ^
			RIJNDAEL_TABLE::Td1[RIJNDAEL_TABLE::Te4[GETBYTE(rk[2], 2)] & 0xff] ^
			RIJNDAEL_TABLE::Td2[RIJNDAEL_TABLE::Te4[GETBYTE(rk[2], 1)] & 0xff] ^
			RIJNDAEL_TABLE::Td3[RIJNDAEL_TABLE::Te4[GETBYTE(rk[2], 0)] & 0xff];
		rk[3] =
			RIJNDAEL_TABLE::Td0[RIJNDAEL_TABLE::Te4[GETBYTE(rk[3], 3)] & 0xff] ^
			RIJNDAEL_TABLE::Td1[RIJNDAEL_TABLE::Te4[GETBYTE(rk[3], 2)] & 0xff] ^
			RIJNDAEL_TABLE::Td2[RIJNDAEL_TABLE::Te4[GETBYTE(rk[3], 1)] & 0xff] ^
			RIJNDAEL_TABLE::Td3[RIJNDAEL_TABLE::Te4[GETBYTE(rk[3], 0)] & 0xff];
		}
	}

CAESDecryptor::CAESDecryptor()
	{	
	}
