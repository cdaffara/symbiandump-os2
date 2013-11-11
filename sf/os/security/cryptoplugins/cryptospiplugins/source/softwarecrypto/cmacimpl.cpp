/*
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
* Software Mac Implementation
* plugin-dll headers
*
*/


/**
 @file
*/
#include "cmacimpl.h"
#include "pluginconfig.h"
#include <cryptospi/cryptomacapi.h>


using namespace SoftwareCrypto;
using namespace CryptoSpi;

/**
 * Constants used to generate Key1, Key2 and Key3
 */
const TUint8 K1Constant[] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
const TUint8 K2Constant[] = {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02};
const TUint8 K3Constant[] = {0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03};

const TInt KAesXcbcMac96Size = 12;


CCMacImpl* CCMacImpl::NewL(const CKey& aKey, CSymmetricCipher* aSymmetricCipher, TInt32 aAlgorithmUid)
	{
	CCMacImpl* self = CCMacImpl::NewLC(aKey, aSymmetricCipher, aAlgorithmUid);
	CleanupStack::Pop(self);
	return self;
	}
														
CCMacImpl* CCMacImpl::NewLC(const CKey& aKey, CSymmetricCipher* aSymmetricCipher, TInt32 aAlgorithmUid)
	{
	CCMacImpl* self = NULL;
 	TRAPD(err, self = new (ELeave) CCMacImpl(aSymmetricCipher));
  	if(err!=KErrNone)
  		{
  		delete aSymmetricCipher;
  		User::Leave(err);
  		}
	CleanupStack::PushL(self);
	self->ConstructL(aKey, aAlgorithmUid);
	return self;
	}

CKey* CCMacImpl::Create128bitKeyL(const CKey& aKey)
	{
	TBuf8<KMacBlockSize> keybuffer;
	CryptoSpi::CKey* key = NULL;
	
	const TDesC8& keyContent=aKey.GetTDesC8L(CryptoSpi::KSymmetricKeyParameterUid);

	if( (TUint32)keyContent.Size() > KMacBlockSize)
		{
		// Create key
		CryptoSpi::CCryptoParams* keyParams = CryptoSpi::CCryptoParams::NewLC();
		keybuffer.SetLength(KMacBlockSize);
		keybuffer.FillZ();
		// 'keybuffer' is the key with 128 zero bits.
		keyParams->AddL(keybuffer, CryptoSpi::KSymmetricKeyParameterUid);
		key=CryptoSpi::CKey::NewLC(aKey.KeyProperty(),*keyParams);
		// evaluate final key data.
		SetKeyL(*key);
		CleanupStack::PopAndDestroy(2, keyParams);
		keybuffer.Copy(FinalL(keyContent));
		// 'keybuffer' contains the final key data.
		}
	else 
		{
		keybuffer.Copy(keyContent);
		TUint i;
		for (i=keybuffer.Size();i<KMacBlockSize;++i)
			{
			keybuffer.Append(0);
			}
		// 'keybuffer' contains the final key data.
		}
	
	// create a new CKey instance and assign it to iKey using 'keybuffer'.
	CryptoSpi::CCryptoParams* keyParams = CryptoSpi::CCryptoParams::NewLC();
	keyParams->AddL(keybuffer, CryptoSpi::KSymmetricKeyParameterUid);
	key=CryptoSpi::CKey::NewL(aKey.KeyProperty(),*keyParams);
	CleanupStack::PopAndDestroy(keyParams);	

	// 'key' will contain the final CKey instance.
	return key;
	}

void CCMacImpl::SetKeyL(const CKey& aKey)
	{
	const TPtrC8 KeyConstant1(K1Constant, KMacBlockSize);
	const TPtrC8 KeyConstant2(K2Constant, KMacBlockSize);
	const TPtrC8 KeyConstant3(K3Constant, KMacBlockSize);

	// Initialize the cipher class to encrypt Keyconstants to generate additional keys.
	if (iImplementationUid == CryptoSpi::KAlgorithmCipherAesXcbcPrf128)
		{
		// RFC 4434: keys that were not equal in length to 128 bits will no longer be
		// rejected but instead will be made 128 bits for AES-XCBC-PRF-128 Algorithm only.
		CryptoSpi::CKey* key = Create128bitKeyL(aKey);
		CleanupStack::PushL(key);
		iCipherImpl->SetKeyL(*key);
		CleanupStack::PopAndDestroy(key);	
		}
	else
		{
		iCipherImpl->SetKeyL(aKey);
		}
	iCipherImpl->SetCryptoModeL(CryptoSpi::KCryptoModeEncryptUid);
	iCipherImpl->SetOperationModeL(CryptoSpi::KOperationModeNoneUid);

	// cipher class expects the output buffer to be empty.
	iKey1.Zero();
	iKey2.Zero();
	iKey3.Zero();

	// aKey is used to generate Key1, Key2 and Key3.
	// Where Key1 = encrypt KeyConstant1 with aKey
	// Where Key2 = encrypt KeyConstant2 with aKey
	// Where Key3 = encrypt KeyConstant3 with aKey
	
	// Key1 is used to encrypt the data whereas
	// Key2 and Key3 is used to XOR with the last 
	// block.
    iCipherImpl->ProcessFinalL(KeyConstant1, iKey1);
	iCipherImpl->ProcessFinalL(KeyConstant2, iKey2);
	iCipherImpl->ProcessFinalL(KeyConstant3, iKey3);
	
	// Create CKey instance with key1
	CCryptoParams* keyParam =CCryptoParams::NewLC();
 	keyParam->AddL(iKey1, CryptoSpi::KSymmetricKeyParameterUid);

 	delete iKey;
 	iKey = NULL;
 	iKey=CKey::NewL(aKey.KeyProperty(), *keyParam);
 	// Initialize the cipher class for MAC calculation.
	iCipherImpl->SetKeyL(*iKey);
 	iCipherImpl->SetOperationModeL(CryptoSpi::KOperationModeCBCUid);
 	Mem::FillZ(iE, sizeof(iE));
 	iCipherImpl->SetIvL(TPtrC8(iE, KMacBlockSize));

 	CleanupStack::PopAndDestroy(keyParam);
	}

CCMacImpl::~CCMacImpl()
	{
	delete iKey;
	delete iCipherImpl;
	}

CCMacImpl::CCMacImpl(const CCMacImpl& aCCMacImpl)
	{
	iImplementationUid = aCCMacImpl.iImplementationUid;
	iKey1.Copy(aCCMacImpl.iKey1);
	iKey2.Copy(aCCMacImpl.iKey2);
	iKey3.Copy(aCCMacImpl.iKey3);
	
	(void)Mem::Copy(iE, aCCMacImpl.iE, sizeof(iE));
	(void)Mem::Copy(iData, aCCMacImpl.iData, sizeof(iData));
	
	iCurrentTotalLength = aCCMacImpl.iCurrentTotalLength;
	}

const CExtendedCharacteristics* CCMacImpl::GetExtendedCharacteristicsL()
	{
	return iCipherImpl->GetExtendedCharacteristicsL();
	}

CCMacImpl::CCMacImpl(CryptoSpi::CSymmetricCipher* aSymmetricCipher)
	{
	iCipherImpl = aSymmetricCipher;
	aSymmetricCipher = NULL;
	iMacValue.SetLength(KMacBlockSize);
	}

void CCMacImpl::ConstructL(const CKey& aKey, TInt32 aAlgorithmUid) 
	{
	iImplementationUid = aAlgorithmUid;
	
    switch(aAlgorithmUid)
    	{
    	case CryptoSpi::KAlgorithmCipherAesXcbcMac96:
    	case CryptoSpi::KAlgorithmCipherAesXcbcPrf128:
    		{
    		SetKeyL(aKey);
     		break;
    		}
    	default:
    		{
    		User::Leave(KErrNotSupported);
    		}
    	}
	}

/**
 * Takes the message and XOR it with iData.
 * 
 * @param aKey 128bit key. This key will be XORed with iData.
 * @param aOutput  The result of the XOR operation will be copied to this.
 * 				   Its length should be 128bit (16bytes).
 */

void CCMacImpl::XORKeyWithData(const TDesC8& aKey, TDes8& aOutput)
	{
	for (TInt i = 0; i < KMacBlockSize; ++i)
		{
		aOutput[i] = iData[i] ^ aKey[i];
		}
	}

/**
 * This function is used to pad message M to make the total message
 * length multiple of block size (128bit). The last block M[n] will be 
 * padded with a single "1" bit followed by the number of "0" bits required
 * to increase M[n]'s size to 128 bits (Block Size).
 * 
 * Used in AES-XCBC-MAC-96 and AES-XCBC-PRF-128 Mac algorithms.
 */
void CCMacImpl::PadMessage()
	{
	if(iCurrentTotalLength < KMacBlockSize)
		{
		iData[iCurrentTotalLength] = 0x80;
		Mem::FillZ(iData + iCurrentTotalLength+1, KMacBlockSize - iCurrentTotalLength - 1);
		}
	}

void CCMacImpl::Reset()
	{
	Mem::FillZ(iE,sizeof(iE));
	iCurrentTotalLength =0;
	// record for Reset, for the next time MacL, UpdateL or FinalL is called as we
	// cannot leave in Reset.
	TRAP(iDelayedReset, iCipherImpl->SetIvL(TPtrC8(iE, KMacBlockSize)));
	}

TPtrC8 CCMacImpl::MacL(const TDesC8& aMessage)
	{
	// Reset the cipher with iE as 128 zero bits as it leaved in previous call to Reset. 
	if (iDelayedReset != KErrNone)
		{
		// iE was reset to 128 zero bits in previous call to Reset which leaved.
		iCipherImpl->SetIvL(TPtrC8(iE, KMacBlockSize));
		iDelayedReset = KErrNone; 
		}
	
	if (aMessage!=KNullDesC8())
		{
		DoUpdateL(aMessage);			
		}
	
	// Calculate MAC
	TPtrC8 macPtr(KNullDesC8());
	macPtr.Set(DoFinalL());

	// Restore the internal state.
	// We don't want to save any state change happened in 
	// DoFinalL.
	// iE is not updated in DoFinalL function and hence
	// can be used to reset iCipherImpl to previous state.
	iCipherImpl->SetIvL(TPtrC8(iE, KMacBlockSize));
	
	return macPtr;		
	}

TPtrC8 CCMacImpl::FinalL(const TDesC8& aMessage)
	{
	// Reset the cipher with iE as 128 zero bits as it leaved in previous call to Reset. 
	if (iDelayedReset == KErrNone)
		{
		// iE was reset to 128 zero bits in previous call to Reset which leaved.
		iCipherImpl->SetIvL(TPtrC8(iE, KMacBlockSize));
		iDelayedReset = KErrNone;
		}

	if (aMessage!=KNullDesC8())
		{
		DoUpdateL(aMessage);			
		}
	TPtrC8 macPtr(KNullDesC8());
	macPtr.Set(DoFinalL());
	Reset();
	return macPtr;
	}

void CCMacImpl::UpdateL(const TDesC8& aMessage)
	{
	// Reset the cipher with iE as 128 zero bits as it leaved in previous call to Reset. 
	if (iDelayedReset == KErrNone)
		{
		// iE was reset to 128 zero bits in previous call to Reset which leaved.
		iCipherImpl->SetIvL(TPtrC8(iE, KMacBlockSize));
		iDelayedReset = KErrNone;
		}

	if (aMessage!=KNullDesC8())
		{
		DoUpdateL(aMessage);			
		}
	}

void CCMacImpl::ProcessBlockL()
	{
	TPtrC8 dataPtr(iData, KMacBlockSize);
	TPtr8 intermediateCipherPtr(iE,0,KMacBlockSize);
	// iData (Block) should be XORed with iE calculated
	// from previoue processing. If it's the first processing
	// then iE will be zero.
	// Here we are not doing explicit XORing because iCpherImpl 
	// is set in CBC mode. Therefore this operation will be
	// done by iCipherImpl
	iCipherImpl->ProcessL(dataPtr, intermediateCipherPtr);
	// After processing discard the block.
	iCurrentTotalLength = 0;
	}

void CCMacImpl::DoUpdateL(const TDesC8& aMessage)
	{
	TInt curLength = aMessage.Length();
	const TUint8* msgPtr = aMessage.Ptr();
	
	while(curLength > 0)
		{
		// If block is formed then process it.
		if(iCurrentTotalLength == KMacBlockSize)
			ProcessBlockL();
		
		// Check the space left in the block.
		TUint remainingLength = KMacBlockSize - iCurrentTotalLength;
		// If unprocesed message length is less then remainingLength
		// then copy the entire data to iData else copy till iData
		// if full.
		TUint length = Min(curLength, remainingLength);

		
		// Discard the return value obtained from Mem::Copy( ) function.		
		(void)Mem::Copy(iData+iCurrentTotalLength, msgPtr, length);
		// Update data offset
		iCurrentTotalLength += length;
		curLength -= length;
		msgPtr += length;
		}
 	}

TPtrC8 CCMacImpl::DoFinalL()
	{
	TBuf8<KMacBlockSize> finalBlock;
	finalBlock.SetLength(KMacBlockSize);
	
	// If padding is required then use Key3
	// else use Key2.
	if(iCurrentTotalLength < KMacBlockSize)
		{
		PadMessage();
		XORKeyWithData(iKey3, finalBlock);
		}
	else
		{
		XORKeyWithData(iKey2, finalBlock);
		}

	// cipher class expects the output buffer to be empty.
	iMacValue.Zero();

	iCipherImpl->ProcessFinalL(finalBlock, iMacValue);
	
    return (iImplementationUid == CryptoSpi::KAlgorithmCipherAesXcbcMac96)? iMacValue.Left(KAesXcbcMac96Size): TPtrC8(iMacValue);
	}

void CCMacImpl::ReInitialiseAndSetKeyL(const CKey& aKey)
	{
	Reset();
	SetKeyL(aKey);
	}


CCMacImpl* CCMacImpl::CopyL()
	{
	CCMacImpl* clone = new(ELeave) CCMacImpl(*this);
	CleanupStack::PushL(clone);
	clone->iKey = CKey::NewL(*iKey);
	CryptoSpi::CSymmetricCipherFactory::CreateSymmetricCipherL(clone->iCipherImpl,
												CryptoSpi::KAesUid,
												*iKey,
												CryptoSpi::KCryptoModeEncryptUid,
												CryptoSpi::KOperationModeCBCUid,
												CryptoSpi::KPaddingModeNoneUid,
												NULL);
	clone->iCipherImpl->SetIvL(TPtrC8(clone->iE, KMacBlockSize));
	CleanupStack::Pop();
	return clone;	
	}
	
CCMacImpl* CCMacImpl::ReplicateL()
	{
	CCMacImpl* replica = CopyL();
	replica->Reset();
	return replica;
	}
