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
* software Mac implementation
* Software Mac Implementation
*
*/


/**
 @file
*/


// MAC plugin header
#include "macimpl.h"
#include "pluginconfig.h"

// headers from cryptospi framework
#include <cryptospi/cryptospidef.h>

#include "hmacimpl.h"


using namespace SoftwareCrypto;


CMacImpl* CMacImpl::NewL(const CKey& aKey, const TUid aImplementationId, const CCryptoParams* aAlgorithmParams)
	{
	CMacImpl* self = new (ELeave) CMacImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aKey, aImplementationId, aAlgorithmParams);
	CleanupStack::Pop();
	return self;
	}

CMacImpl::CMacImpl() 
	{
	}

void CMacImpl::ConstructL(const CKey& aKey, const TUid aImplementationId, const CCryptoParams* /*aAlgorithmParams*/) 
	{
	iImplementationUid = aImplementationId; 
	iKey = CryptoSpi::CKey::NewL(aKey);
		
	MSoftwareHash* hashImpl = NULL;
	
	switch (aImplementationId.iUid)
		{
		case KTestPlugin02XcbcMac96:
	    	{
			CSymmetricCipher* symmetricCipher = NULL;
			CryptoSpi::CSymmetricCipherFactory::CreateSymmetricCipherL(symmetricCipher,
														CryptoSpi::KAesUid, *iKey,
														CryptoSpi::KCryptoModeEncryptUid,
														CryptoSpi::KOperationModeCBCUid,
														CryptoSpi::KPaddingModeNoneUid,
														NULL);
			
			iCmacImpl= CCMacImpl::NewL(*iKey, symmetricCipher, CryptoSpi::KAlgorithmCipherAesXcbcMac96);
			iBase = ECipherBased;
			}
			break;
			
	    case KTestPlugin02XcbcPrf128:
	    	{
			TBuf8<16> tempKey;
			tempKey.SetLength(16);
			
			CryptoSpi::CCryptoParams* keyParams = CryptoSpi::CCryptoParams::NewLC();
			keyParams->AddL(tempKey, CryptoSpi::KSymmetricKeyParameterUid);
			CryptoSpi::CKey* key = CryptoSpi::CKey::NewLC(aKey.KeyProperty(),*keyParams);
				
			CSymmetricCipher* symmetricCipher = NULL;
			CryptoSpi::CSymmetricCipherFactory::CreateSymmetricCipherL(symmetricCipher,
														CryptoSpi::KAesUid, *key,
														CryptoSpi::KCryptoModeEncryptUid,
														CryptoSpi::KOperationModeCBCUid,
														CryptoSpi::KPaddingModeNoneUid,
														NULL);
			CleanupStack::PopAndDestroy(2, keyParams); //key and keyParams
			
			iCmacImpl= CCMacImpl::NewL(*iKey, symmetricCipher, CryptoSpi::KAlgorithmCipherAesXcbcPrf128);
			iBase = ECipherBased;
			}
			break;
			
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}

	if(iBase == EHashBased)
		{
		CleanupStack::PopAndDestroy(hashImpl);
		}
	}

CMacImpl::~CMacImpl()
	{
	delete iKey;
	if(iHmacImpl)
		{
		iHmacImpl->Close();
		}
	delete iCmacImpl;
	}
	
void CMacImpl::Reset()
	{
	if (iBase == EHashBased)
		{
		iHmacImpl->Reset();
		}
	else if (iBase == ECipherBased)
		{
		iCmacImpl->Reset();
		}
	}
	
void CMacImpl::Close()
	{
	delete this;	
	}
	
void CMacImpl::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	aPluginCharacteristics=NULL;
	TInt macNum=sizeof(KMacCharacteristics)/sizeof(TMacCharacteristics*);
	for (TInt i=0;i<macNum;++i)
		{
		if (KMacCharacteristics[i]->iMacChar.iImplementationUID==ImplementationUid().iUid)
			{
			aPluginCharacteristics = KMacCharacteristics[i];
			break;
			}
		}	
	}

const CExtendedCharacteristics* CMacImpl::GetExtendedCharacteristicsL()
	{
	return (iBase == EHashBased) ? iHmacImpl->GetExtendedCharacteristicsL(): iCmacImpl->GetExtendedCharacteristicsL();
	}

TAny* CMacImpl::GetExtension(TUid aExtensionId)
	{
	return (iBase == EHashBased) ? iHmacImpl->GetExtension(aExtensionId): NULL;
	}

TUid CMacImpl::ImplementationUid() const
	{
	return iImplementationUid;
	}

TPtrC8 CMacImpl::MacL(const TDesC8& aMessage)
	{
	return (iBase == EHashBased) ? iHmacImpl->Hash(aMessage):iCmacImpl->MacL(aMessage);
	}		
	
void CMacImpl::UpdateL(const TDesC8& aMessage)
	{
	(iBase == EHashBased) ? iHmacImpl->Update(aMessage):iCmacImpl->UpdateL(aMessage);
	}

TPtrC8 CMacImpl::FinalL(const TDesC8& aMessage)
	{
	return (iBase == EHashBased) ? iHmacImpl->Final(aMessage):iCmacImpl->FinalL(aMessage);
	}

void CMacImpl::ReInitialiseAndSetKeyL(const CKey& aKey)
	{
	delete iKey;
	iKey = NULL;
	iKey = CryptoSpi::CKey::NewL(aKey);
	
	if (iBase == EHashBased)
		{
		iHmacImpl->SetKeyL(aKey);
		iHmacImpl->Reset();
		}
	else if (iBase == ECipherBased)
		{
		iCmacImpl->ReInitialiseAndSetKeyL(aKey);
		}
	}

MMac* CMacImpl::ReplicateL()
	{
	CMacImpl* that= new(ELeave) CMacImpl();
	CleanupStack::PushL(that);
	that->iImplementationUid = iImplementationUid;
	that->iBase = iBase;
	that->iKey=CKey::NewL(*iKey);
	
	if(iBase == EHashBased)
		{
		that->iHmacImpl=static_cast<CHMacImpl*>(iHmacImpl->ReplicateL());
		}
	else if (iBase == ECipherBased)
		{
		that->iCmacImpl= iCmacImpl->ReplicateL();
		}
	CleanupStack::Pop(that);
	return that;
	}
	
MMac* CMacImpl::CopyL()
	{
	CMacImpl* that= new(ELeave) CMacImpl();
	CleanupStack::PushL(that);
	that->iImplementationUid = iImplementationUid;
	that->iBase = iBase;
	that->iKey=CKey::NewL(*iKey);

	if(iBase == EHashBased)
		{
		that->iHmacImpl=static_cast<CHMacImpl*>(iHmacImpl->CopyL());
		}
	else if (iBase == ECipherBased)
		{
		that->iCmacImpl= iCmacImpl->CopyL();
		}
	CleanupStack::Pop(that);
	return that;
	}
