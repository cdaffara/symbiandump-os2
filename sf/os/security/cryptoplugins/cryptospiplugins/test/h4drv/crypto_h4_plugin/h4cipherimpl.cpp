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
 @internalComponent
 @released
*/
#include "h4cipherimpl.h"

#include <e32def.h>
#include <cryptostrength.h>
#include <cryptospi/cryptospidef.h>
#include "keys.h"
#include <cryptospi/plugincharacteristics.h>
#include "pluginconfig.h"
#include <cryptopanic.h>
#include <securityerr.h>
#include "keyhandle.h"

using namespace HwCrypto;

#define BytesToBits(byteCount) (byteCount*8)

//
// Implementation of Symmetric Cipher class
//
CH4CipherImpl::CH4CipherImpl(TUint8 aBlockBytes,
							 TUid aCryptoMode,
							 TUid aOperationMode,
							 TUid aPaddingMode) 
	: iBlockBytes(aBlockBytes),
	  iCryptoMode(aCryptoMode),
	  iOperationMode(aOperationMode),
	  iPaddingMode(aPaddingMode)
	{
	}

void CH4CipherImpl::ConstructL(const CKey& aKey) 
	{
	SetKeyL(aKey);		
	SetOperationModeL(iOperationMode);
	SetCryptoModeL(iCryptoMode);	
	SetPaddingModeL(iPaddingMode);
	
	iPartialBlock.ReAllocL(iBlockBytes);
	iPaddingBlock.ReAllocL(iBlockBytes);
	
	iIv.ReAllocL(iBlockBytes);
	//	iIv.SetLength(iBlockBytes);
	//	TRandom::RandomL(iIv);
	}
	
CExtendedCharacteristics* CH4CipherImpl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.		

	//	return CExtendedCharacteristics::NewL(KMaxTInt, iStandardsConformance, EFalse);	
	return 0;	
	}

CH4CipherImpl::~CH4CipherImpl()
	{			
	delete iPadding;
	iIv.Close();
	iPartialBlock.Close();
	iPaddingBlock.Close();	

	delete iKey;	
	iStandardsConformance.Close();	
	}
		
void CH4CipherImpl::Close()
	{
	delete this;
	}
	
TAny* CH4CipherImpl::GetExtension(TUid /*aExtensionId*/) 
	{
	return 0;
	}
	
void CH4CipherImpl::GetCharacteristicsL(const TAny*& aPluginCharacteristics)
	{
	TInt numCiphers = sizeof(KSymmetricCipherCharacteristics)/sizeof(TSymmetricCipherCharacteristics*);
	TInt32 implUid = ImplementationUid().iUid;
	for (TInt i = 0; i < numCiphers; ++i)
		{
		if (KSymmetricCipherCharacteristics[i]->cmn.iImplementationUID == implUid)
			{
			aPluginCharacteristics = KSymmetricCipherCharacteristics[i];
			break;
			}
		}	
	}


void CH4CipherImpl::Reset()
	{
	iPartialBlock.Zero();
	iPaddingBlock.Zero();
	
	// Reconfigure h/w based on iCryptoMode/iOperationMode/iKey/iIv
	TRAP_IGNORE(DoSetupL());
	iNeedToSetupHw = EFalse;
	}	


TInt CH4CipherImpl::GetKeyStrength() const
	{
	return BytesToBits(iKey->Length());
	}
	
HBufC8* CH4CipherImpl::ExtractKeyDataLC(const CKey& aKey) const
	{
	const TDesC8& keyContent = aKey.GetTDesC8L(KSymmetricKeyParameterUid);
	return keyContent.AllocLC();
	}

TInt CH4CipherImpl::KeySize() const
	{
	// return key size in BITS
	return BytesToBits(iKeyBytes);
	}

void CH4CipherImpl::SetKeyL(const CKey& aKey)
	{
	HBufC8* key = ExtractKeyDataLC(aKey);
	TInt keyLength(key->Length());
	
	const TKeyProperty &keyProps = aKey.KeyProperty();

	if(keyProps.iKeyType != KSymmetricKeyUid)
		{
		User::Leave(KErrArgument);
		}

	if(keyProps.iKeyAttribute == KNonEmbeddedKeyUid)
		{
		// Not an embedded key, so key is the actual key data and we
		// can therefore check its strength...
		TCrypto::IsSymmetricWeakEnoughL(BytesToBits(keyLength));
		if (! IsValidKeyLength(keyLength))
			{
			User::Leave(KErrNotSupported);
			}
		}
		
	delete iKey;	
	CleanupStack::Pop(key);
	iKey = key;
	iKeyBytes = keyLength;

	// H/W needs reconfiguring
	iNeedToSetupHw = ETrue;
	}	

TInt CH4CipherImpl::BlockSize() const
	{
	// return block size in BITS
	return BytesToBits(iBlockBytes);
	}

void CH4CipherImpl::SetCryptoModeL(TUid aCryptoMode)
	{
	switch (aCryptoMode.iUid)
		{
		case KCryptoModeEncrypt:
		case KCryptoModeDecrypt:
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	iCryptoMode = aCryptoMode;		
	// H/W needs reconfiguring
	iNeedToSetupHw = ETrue;
	}

void CH4CipherImpl::SetOperationModeL(TUid aOperationMode)
	{
	switch (aOperationMode.iUid)
		{
		case KOperationModeNone:
		case KOperationModeECB:
		case KOperationModeCBC:
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	iOperationMode = aOperationMode;		
	// H/W needs reconfiguring
	iNeedToSetupHw = ETrue;
	}

void CH4CipherImpl::SetPaddingModeL(TUid aPaddingMode)
	{
	if(!iPadding || (aPaddingMode != iPaddingMode))
		{
		CPadding* padding(0);
		switch (aPaddingMode.iUid)
			{
			case KPaddingModeNone:
				padding = CPaddingNone::NewL(iBlockBytes);
				break;
			case KPaddingModeSSLv3:
			padding = CPaddingSSLv3::NewL(iBlockBytes);
			break;
			case KPaddingModePKCS7:
				padding = CPaddingPKCS7::NewL(iBlockBytes);
				break;
			default:
				User::Leave(KErrNotSupported);
			}
		delete iPadding;
		iPadding = padding;
		iPaddingMode = aPaddingMode;
		}
	
	// H/W needs reconfiguring
	iNeedToSetupHw = ETrue;
	}
	
void CH4CipherImpl::SetIvL(const TDesC8& aIv)
	{
	if (iOperationMode.iUid != KOperationModeCBC)
		{
		User::Leave(KErrNotSupported);
		}

	if (aIv.Length() != iBlockBytes) 
		{
		User::Leave(KErrArgument);
		}
	iIv = aIv;	

	// H/W needs reconfiguring
	iNeedToSetupHw = ETrue;
	}



	
	




TInt CH4CipherImpl::MaxOutputLength(TInt aInputLength) const
	{	
	// The maximum output length required for Process is equal to the
	// size of the number of whole input blocks available.
	//
	// The block bytes is a power of two so we can use this to avoid
	// doing a real mod operation
	TUint partialBlockLength(iPartialBlock.Length());
	return (partialBlockLength + aInputLength) & ~TUint32(iBlockBytes - 1);
	}	

TInt CH4CipherImpl::MaxFinalOutputLength(TInt aInputLength) const
	{
	if (iCryptoMode.iUid == KCryptoModeEncrypt)
		{
		return iPadding->MaxPaddedLength(iPartialBlock.Length() + aInputLength);
		}
	else
		{
		return iPadding->MaxUnPaddedLength(aInputLength + iPartialBlock.Length());
		}
	}



void CH4CipherImpl::ProcessL(const TDesC8& aInput, TDes8& aOutput)
	{
	if(iNeedToSetupHw)
		{
		// Reconfigure h/w based on iCryptoMode/iOperationMode/iKey/iIv
		DoSetupL();
		iNeedToSetupHw = EFalse;
		}

	TInt inputLength(aInput.Length());	
	if (MaxOutputLength(inputLength) > aOutput.MaxLength())
		{
		User::Leave(KErrOverflow);
		}	

	TInt partialBlockLength(iPartialBlock.Length()); // partial data written to h/w last time
	TUint32 totalInput = partialBlockLength + inputLength;
	
	// Pass new data to h/w driver
	DoWriteL(aInput.Ptr(), inputLength);

	if(totalInput < iBlockBytes)
		{
		// Keep a copy of the partial block which is inside the h/w
		// driver in case we need to calculate pad for it later...
		if(inputLength) iPartialBlock.Append(aInput);
		// Not enough written yet for more data to be available yet.
		return;
		}
	else
		{
		// We have completed the previous partial block so we no
		// longer need to keep a copy of it
		iPartialBlock.Zero();
		// Work out length of partial block at end of current data
		TUint32 trailing = TUint32(partialBlockLength + inputLength) & TUint32(iBlockBytes - 1);
		// Keep a copy of the partial block data
		if(trailing) iPartialBlock.Append(aInput.Right(trailing));
		}

	//
	// Work out how much data is available for reading.
	//
	// The h/w processes data a block at a time, and we only ever read
	// a multiple of the block size so the available data will always
	// be a multiple of the blocksize.

	TUint32 availableData = totalInput & ~TUint32(iBlockBytes - 1);
	if (availableData)
		{
		// Read available data
		DoReadL(aOutput, availableData);
		}
	}

void CH4CipherImpl::ProcessFinalL(const TDesC8& aInput, TDes8& aOutput)
	{
	if(iNeedToSetupHw)
		{
		// Reconfigure h/w based on iCryptoMode/iOperationMode/iKey/iIv
		DoSetupL();
		iNeedToSetupHw = EFalse;
		}

	if(MaxFinalOutputLength(aInput.Length()) > aOutput.MaxLength() - aOutput.Length())
		{
		User::Leave(KErrOverflow);
		}


	if(iCryptoMode.iUid == KCryptoModeEncrypt)
		{
		// process everything up to the last (possibly empty block)
		ProcessL(aInput, aOutput);

		// pad the plaintext
		iPaddingBlock.Zero();
		iPadding->PadL(iPartialBlock, iPaddingBlock);

		TUint32 padLength = iPaddingBlock.Length();
		// Make sure pad worked
		if(padLength & TUint32(iBlockBytes-1))
			{
			User::Leave(KErrInvalidPadding);
			}
	
		if(padLength > 0)
			{
			// Padding created

			// We have already written iPartialBlock data to the h/w
			// so skip those bytes which are in the pad.
			TUint32 partialLength = iPartialBlock.Length();
			if(partialLength)
				{
				// Make sure the pad algorithm did not change the bytes at
				// the start of the block....
				if(iPaddingBlock.Left(partialLength) != iPartialBlock)
					{
					User::Leave(KErrInvalidPadding);
					}
				}
			// Pass new data to h/w driver
			DoWriteL(iPaddingBlock.Ptr() + partialLength, padLength - partialLength);

			// Have now written an exact multiple of blocks to h/w so clear partial
			iPartialBlock.Zero();

			// Read data
			DoReadL(aOutput, padLength);
			}
		}
	else
		{
		// Decrypt

		// Input length (including inputstore) must be a multiple of the 
		// block size in length
		if ((aInput.Length() + iPartialBlock.Length()) & (iBlockBytes - 1)) 
			{
			User::Leave(KErrArgument);
			}
		
		// process everything up to the last (possibly empty block)
		ProcessL(aInput, aOutput);
		ASSERT(iPartialBlock.Length()==0); // all the blocks should have been decrypted

		// Retrieve last decrypted block.
		iPaddingBlock = aOutput.Right(iBlockBytes);
		aOutput.SetLength(aOutput.Length() - iBlockBytes);

		// Unpad the last block and (re)append to output
		iPadding->UnPadL(iPaddingBlock, aOutput);
	
		iPaddingBlock.Zero();
		iPartialBlock.Zero();
		}
	
	}



// End of file


