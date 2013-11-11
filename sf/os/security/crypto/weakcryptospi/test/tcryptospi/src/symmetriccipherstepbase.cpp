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
*
*/


/**
 @file
 @internalTechnology
*/
 
#include "symmetriccipherstepbase.h"
#include <bigint.h>


/**
Common setup of a crypto SPI symmetric cipher.
@param aArc4Check Determines whether the check is run on the cipher type for ARC4 and if it matches, sets up the CCryptoParams accordingly.
@param aRc2Check Determines whether the check is run on the cipher type for RC2 and if it matches, sets up the CCryptoParams accordingly.
@param aOperationMode On return is the operation mode read in from the config file and used in the initial setup of the cipher.
@param aCipher On return the symmetric cipher implementation.
@param aKey On return aCipher's key object.  This is should not be deleted until after aCipher is deleted.
@return The error value from the call to the symmetric cipher creation function.
*/
void CSymmetricCipherStepBase::SetupCipherL(TBool aArc4Check, TBool aRc2Check, TVariantPtrC& aOperationMode, CSymmetricCipher*& aCipher, CKey*& aKey)
	{	
	TPtrC keyPath;
	TVariantPtrC algorithm;
	TVariantPtrC paddingMode;
		
	if(	!GetStringFromConfig(ConfigSection(),KConfigEncryptKey, keyPath) ||
		!GetStringFromConfig(ConfigSection(),KConfigAlgorithmUid, algorithm) || 
		!GetStringFromConfig(ConfigSection(),KConfigOperationMode, aOperationMode) ||
		!GetStringFromConfig(ConfigSection(),KConfigPaddingMode, paddingMode ))
		{
		User::Leave(KErrNotFound);
		}
	
	// Create key 
	TKeyProperty keyProperty;
	CCryptoParams* params = CCryptoParams::NewLC(); 
	
	CFileReader* keyData = CFileReader::NewLC(keyPath);
	params->AddL( *keyData, KSymmetricKeyParameterUid);
	
	aKey=CKey::NewL(keyProperty, *params);
	CleanupStack::PushL(aKey);

	CCryptoParams* xparams = NULL;	
	if ((aArc4Check) && (TUid(algorithm) == KArc4Uid))
		{
		// Set the RC4 DiscardBytes to 0
		xparams = CCryptoParams::NewLC();
		xparams->AddL(NULL, KARC4DiscardBytes);
		
		// Create a Symmetric Cipher with the values from the ini file
		CSymmetricCipherFactory::CreateSymmetricCipherL(aCipher, algorithm, *aKey, KCryptoModeEncryptUid, aOperationMode, paddingMode, xparams);
		CleanupStack::PopAndDestroy(xparams);
		CleanupStack::Pop(aKey);
		CleanupStack::PopAndDestroy(2, params);
		}
	else if ((aRc2Check) && (TUid(algorithm) == KRc2Uid))
		{
		TInt keylen = TPtrC8(*keyData).Length() * 8;
		xparams = CCryptoParams::NewLC();
				
		// Set the RC2 EffectiveKeyLen according to the input key size
		xparams->AddL( keylen, KRC2EffectiveKeyLenBits);
		
		// Create a Symmetric Cipher with the values from the ini file
		CSymmetricCipherFactory::CreateSymmetricCipherL(aCipher, algorithm, *aKey, KCryptoModeEncryptUid, aOperationMode, paddingMode, xparams);
		CleanupStack::PopAndDestroy(xparams);
		CleanupStack::Pop(aKey);
		CleanupStack::PopAndDestroy(2, params);
		}
	else
		{
		// Create a Symmetric Cipher with the values from the ini file
		CSymmetricCipherFactory::CreateSymmetricCipherL(aCipher, algorithm, *aKey, KCryptoModeEncryptUid, aOperationMode, paddingMode, xparams);
		CleanupStack::Pop(aKey);
		CleanupStack::PopAndDestroy(2, params);
		}
	}
	

/**
When running in CTR mode call this function to calculate the block size of a cipher.
@param aCipher The cipher whose block size is returned.
@return The block size in bits.
*/	
TInt CSymmetricCipherStepBase::CtrModeCalcBlockSizeL(CSymmetricCipher& aCipher)
	{
  	// aCipher MUST be running in CTR mode
	aCipher.SetOperationModeL(KOperationModeCBCUid);
	TInt blockSize = aCipher.BlockSize();
	aCipher.SetOperationModeL(KOperationModeCTRUid);
	return blockSize;	
	}

	
/**
Read in the plaintext from the course file listed in the configuration file.
@param aPlaintext Descriptor pointing to the plaintext.
@param aReader This CFileReader pointer must be NULL when passed in. It must not be deleted until after the client has finished with the plaintext.
*/	
HBufC8* CSymmetricCipherStepBase::ReadInPlaintextL()
	{
	TPtrC plaintextPath;
	if(!GetStringFromConfig(ConfigSection(),KConfigSourcePath, plaintextPath))
		{
		User::Leave(KErrNotFound);
		}
	
	return ReadFileL(plaintextPath);
	}


/**
Read in the ciphertext from the course file listed in the configuration file.
@param aCiphertext Descriptor pointing to the ciphertext.
@param aReader This CFileReader pointer must be NULL when passed in. It must not be deleted until after the client has finished with the ciphertext.

*/
HBufC8* CSymmetricCipherStepBase::ReadInCiphertextL()
	{
	TPtrC ciphertextPath;
	if(!GetStringFromConfig(ConfigSection(),KConfigEncryptedPath, ciphertextPath))
		{
		User::Leave(KErrNotFound);
		}

	return ReadFileL(ciphertextPath);
	}


/**
Read in the IV from the course file listed in the configuration file.
@param aIV Descriptor pointing to the IV.
@param aReader This CFileReader pointer must be NULL when passed in. It must not be deleted until after the client has finished with the IV.

*/
HBufC8* CSymmetricCipherStepBase::ReadInIvL()
	{
	TPtrC ivPath;
	if(!GetStringFromConfig(ConfigSection(),KConfigIVPath, ivPath))
		{
		User::Leave(KErrNotFound);
		}

	return ReadFileL(ivPath);
	}
	
	
/**
@param aCounter
*/	
HBufC8* CSymmetricCipherStepBase::CtrModeIncrementCounterL(TDesC8& aCounter)
	{
	RInteger bigInt = RInteger::NewL(aCounter);
	CleanupClosePushL(bigInt);
	++bigInt;
	HBufC8* result = bigInt.BufferLC();
	CleanupStack::Pop(result);
	CleanupStack::PopAndDestroy();
	return result;
	}

HBufC8* CSymmetricCipherStepBase::ReadInHexCiphertextL()
	{
	HBufC8* hex = ReadInCiphertextL();
	CleanupStack::PushL(hex);
	HBufC8* result = ConvertFromHexFormatToRawL((*hex));
	CleanupStack::PopAndDestroy(hex);
	return result;
	}
	
HBufC8* CSymmetricCipherStepBase::ReadInHexPlaintextL()
	{
	HBufC8* hex = ReadInPlaintextL();
	CleanupStack::PushL(hex);
	HBufC8* result = ConvertFromHexFormatToRawL((*hex));
	CleanupStack::PopAndDestroy(hex);
	return result;
	}	

HBufC8* CSymmetricCipherStepBase::ReadInHexPlainTextL(TPtrC aFile)
	{
	HBufC8* data = ReadFileL(aFile);
	CleanupStack::PushL(data);
	HBufC8* result = ConvertFromHexFormatToRawL(*data);
	CleanupStack::PopAndDestroy(data);
	return result;
	}	

HBufC8* CSymmetricCipherStepBase::ReadFileL(TPtrC aFile)
	{
	CFileReader* reader = CFileReader::NewL(aFile);
	CleanupStack::PushL(reader);
	TPtrC8 ptr(*reader);
	HBufC8* fileData = ptr.AllocL();
	CleanupStack::PopAndDestroy(reader);
	return fileData;
	}

HBufC8* CSymmetricCipherStepBase::ReadInHexIvL()
	{
	HBufC8* hex = ReadInIvL();
	CleanupStack::PushL(hex);
	HBufC8* result = ConvertFromHexFormatToRawL((*hex));
	CleanupStack::PopAndDestroy(hex);
	return result;
	}

HBufC8* CSymmetricCipherStepBase::ConvertFromHexFormatToRawL(TDesC8& aInput)
	{
	TBuf8<2> hexPair;
	HBufC8* result = HBufC8::NewLC(aInput.Length()/2);
	TUint8 val;
			
	for(TInt i = 0 ; i < aInput.Length()-1 ; i+=2)
		{
		hexPair = aInput.Mid(i,2);
		TLex8 lex(hexPair);
		User::LeaveIfError(lex.Val(val, EHex));
		result->Des().Append(val);			
		}
	CleanupStack::Pop(result);
	return result;	
	}
