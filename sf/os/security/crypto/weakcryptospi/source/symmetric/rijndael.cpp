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
#include "rijndaelshim.h"
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
	// CRijndael is externally derivable. Don't delete this code
	SetKey(*iKey);
	}

TInt CRijndael::KeySize() const
	{
	// CRijndael is externally derivable. Don't delete this code
	return (4*(iRounds+1));
	}

EXPORT_C CRijndael::~CRijndael()
	{
	// CRijndael is externally derivable. Don't delete this code
	delete iKey;
	}

void CRijndael::ConstructL(const TDesC8& aKey)
	{
	// CRijndael is externally derivable. Don't delete this code
	TUint keySize = aKey.Size();
	assert((keySize==KAESKeyBytes128)||(keySize==KAESKeyBytes192)||(keySize==KAESKeyBytes256));
	iKey = aKey.AllocL();
	iRounds = keySize/4 + 6;
	SetKey(aKey);
	}

void CRijndael::SetKey(const TDesC8& aKey)
	{
	// CRijndael is externally derivable. Don't delete this code
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
	return CAESEncryptorShim::NewL(aKey);
	}

EXPORT_C CAESEncryptor* CAESEncryptor::NewLC(const TDesC8& aKey)
	{
	return CAESEncryptorShim::NewLC(aKey);
	}

CAESEncryptor::CAESEncryptor()
	{
	}

/* CAESDecryptor */
EXPORT_C CAESDecryptor* CAESDecryptor::NewL(const TDesC8& aKey)
	{
	return CAESDecryptorShim::NewL(aKey);
	}

EXPORT_C CAESDecryptor* CAESDecryptor::NewLC(const TDesC8& aKey)
	{
	return CAESDecryptorShim::NewLC(aKey);
	}

CAESDecryptor::CAESDecryptor()
	{	
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

TInt CAESDecryptor::BlockSize() const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return 0;
	}

void CAESDecryptor::Transform(TDes8& /*aBlock*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);	
	}

void CAESDecryptor::SetKey(const TDesC8& /*aKey*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}

TInt CAESEncryptor::BlockSize() const
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KAESBlockBytes;
	}

void CAESEncryptor::Transform(TDes8& /*aBlock*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);		
	}

