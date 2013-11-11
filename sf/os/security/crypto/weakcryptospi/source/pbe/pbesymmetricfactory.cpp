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


#include <symmetric.h>
#include "pbesymmetricfactory.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS

/** OpenSSL PKCS8 Effective Key Length Compatibility.*/
const TUint KPkcs8CompatibilityBits = 128;

/** PKCS12 PBE Effective Key Length Compatibility.*/
const TUint KPkcs12CompatibilityBits = 40;

#endif

TUint PBE::GetBlockBytes(TPBECipher aCipher)
	{
	switch(aCipher)
		{
		case ECipherAES_CBC_128:
		case ECipherAES_CBC_192:
		case ECipherAES_CBC_256:
			return KAESBlockBytes;
		case ECipherDES_CBC:
		case ECipher3DES_CBC:

		case ECipher2Key3DES_CBC: 

			return KDESBlockBytes;
		case ECipherRC2_CBC_128_16:
		case ECipherRC2_CBC_40_16:
		case ECipherRC2_CBC_128:
		case ECipherRC2_CBC_40:

		case ECipherRC2_CBC_40_5:

			return KRC2BlockBytes;

		case ECipherARC4_128:
		case ECipherARC4_40:
			return 1; // 1 byte block for stream cipher

		default:
			User::Panic(_L("Invalid PBE cipher"), 1);
		}
	return (KErrNone); //	For the compiler
	}

TUint PBE::GetKeyBytes(TPBECipher aCipher)
	{
	switch(aCipher)
		{
		case ECipherAES_CBC_128:
			return KAESKeyBytes128;
		case ECipherAES_CBC_192:
			return KAESKeyBytes192;
		case ECipherAES_CBC_256:
			return KAESKeyBytes256;
		case ECipherDES_CBC:
			return KDESKeyBytes;
		case ECipher3DES_CBC:
			return K3DESKeyBytes;

		case ECipher2Key3DES_CBC:
			return K2Key3DESKeyBytes;

 		case ECipherRC2_CBC_128:
 		case ECipherRC2_CBC_128_16:
			return KRC2KeyBytes128;
		case ECipherRC2_CBC_40:
		case ECipherRC2_CBC_40_16:

		case ECipherRC2_CBC_40_5:

			return KRC2KeyBytes40;

		case ECipherARC4_128:
			return KRC4KeyBytes128;
		case ECipherARC4_40:	
			return KRC4KeyBytes40;

		default:
			User::Panic(_L("Invalid PBE cipher"), 1);
		}
	return (KErrNone);	//	For the compiler
	}

CSymmetricCipher* PBE::MakeEncryptorL(TPBECipher aCipher, const TDesC8& aKey, 
	const TDesC8& aIV)
	{
	CSymmetricCipher* cipher = 0;	
	CBlockTransformation* block = 0;
	switch(aCipher)
		{

		// stream cipher
		case ECipherARC4_40:
		case ECipherARC4_128:
			cipher = CARC4::NewL(aKey, 0);
			break;	

		// block cipher	
		case ECipherAES_CBC_128:
		case ECipherAES_CBC_192:
		case ECipherAES_CBC_256:
			block = CAESEncryptor::NewLC(aKey);
			break;

		case ECipherDES_CBC:
			block = CDESEncryptor::NewLC(aKey);
			break;
			
		case ECipher3DES_CBC:		
			block = C3DESEncryptor::NewLC(aKey);
			break;
	
		case ECipher2Key3DES_CBC:
			{
			// Construct 3key from 2 key ( copy first key to 3rd key ) each key 8 bytes
			TBuf8<K3DESKeyBytes>  encryptKey(aKey);			
			encryptKey.Append(aKey.Ptr(),KDESKeyBytes);
			block = C3DESEncryptor::NewLC(encryptKey);			
			break;
			}

		case ECipherRC2_CBC_40:
		case ECipherRC2_CBC_128:		
			block = CRC2Encryptor::NewLC(aKey);
			break;

		case ECipherRC2_CBC_40_16:
		case ECipherRC2_CBC_128_16:
			block = CRC2Encryptor::NewLC(aKey, KPkcs8CompatibilityBits);
			break;

		case ECipherRC2_CBC_40_5:
			block = CRC2Encryptor::NewLC(aKey, KPkcs12CompatibilityBits);
			break;	

		default:
			User::Panic(_L("Invalid PBE encryptor"), 1);
		}

	// if aCipher is not stream cipher, create block cipher object
	if(aCipher != ECipherARC4_40 && aCipher != ECipherARC4_128)
		{			
		block = CModeCBCEncryptor::NewL(block, aIV);
		CleanupStack::Pop(); //1st block owned by 2nd
		CleanupStack::PushL(block);//2nd block
		CPadding* padding = CPaddingSSLv3::NewLC(GetBlockBytes(aCipher));
		cipher = CBufferedEncryptor::NewL(block, padding);
		CleanupStack::Pop(padding); //owned by cipher
		CleanupStack::Pop(block); //owned by cipher
		}

	return cipher;
	}


CSymmetricCipher* PBE::MakeDecryptorL(TPBECipher aCipher, const TDesC8& aKey, 
	const TDesC8& aIV)
	{
	CSymmetricCipher* cipher = 0;
	CBlockTransformation* block = 0;
	switch(aCipher)
		{
		// stream cipher
		case ECipherARC4_40:
		case ECipherARC4_128:
			cipher = CARC4::NewL(aKey, 0);
			break;	

		// block cipher	
		case ECipherAES_CBC_128:
		case ECipherAES_CBC_192:
		case ECipherAES_CBC_256:
			block = CAESDecryptor::NewLC(aKey);
			break;

		case ECipherDES_CBC:
			block = CDESDecryptor::NewLC(aKey);
			break;
			
		case ECipher3DES_CBC:		
			block = C3DESDecryptor::NewLC(aKey);
			break;

		case ECipher2Key3DES_CBC:
			{
			// Construct 3key from 2 key ( copy first key to 3rd key ) each key 8 bytes
			TBuf8<K3DESKeyBytes>  encryptKey(aKey);			
			encryptKey.Append(aKey.Ptr(),KDESKeyBytes);
			block = C3DESDecryptor::NewLC(encryptKey);			
			break;
			}

		case ECipherRC2_CBC_40:
		case ECipherRC2_CBC_128:
		  	block = CRC2Decryptor::NewLC(aKey);
			break;
			
		case ECipherRC2_CBC_40_16:
		case ECipherRC2_CBC_128_16:
		  	block = CRC2Decryptor::NewLC(aKey, KPkcs8CompatibilityBits);
			break;

		case ECipherRC2_CBC_40_5:
			block = CRC2Decryptor::NewLC(aKey, KPkcs12CompatibilityBits);
			break;	

		default:
			User::Panic(_L("Invalid PBE decryptor"), 1);
		}

	// if aCipher is not stream cipher, create block cipher object
	if(aCipher != ECipherARC4_40 && aCipher != ECipherARC4_128)
		{	
		block = CModeCBCDecryptor::NewL(block, aIV);
		CleanupStack::Pop(); //1st block owned by 2nd
		CleanupStack::PushL(block);//2nd block

		CPadding* padding = CPaddingSSLv3::NewLC(GetBlockBytes(aCipher));
		cipher = CBufferedDecryptor::NewL(block, padding);
		CleanupStack::Pop(padding); //owned by cipher
		CleanupStack::Pop(block); //owned by cipher
		}

	return cipher;
	}
