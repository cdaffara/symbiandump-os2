/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* software hash base class implementation
* software hash base class implementation
*
*/


/**
 @file
*/

#include "softwarehashbase.h"

#include <cryptospi/hashplugin.h>
#include "pluginconfig.h"
#include <cryptospi/keys.h>
#include "md5impl.h"
#include "hmacimpl.h"

using namespace SoftwareCrypto;

CSoftwareHash* CSoftwareHash::NewL(TUid aAlgorithm, TUid aOperationMode, const CKey* aKey)
	{
	CSoftwareHash* self=NewLC(aAlgorithm, aOperationMode, aKey);
	CleanupStack::Pop();
	return self;
	}
	
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
CSoftwareHash* CSoftwareHash::NewL(TUid aAlgorithm)
	{
	CSoftwareHash* self=NewLC(aAlgorithm, KHashModeUid, NULL);
	CleanupStack::Pop();
	return self;
	}
#endif

CSoftwareHash* CSoftwareHash::NewLC(TUid aAlgorithm, TUid aOperationMode, const CKey* aKey)
	{
	CSoftwareHash* self=new (ELeave) CSoftwareHash();
	CleanupStack::PushL(self);
	self->ConstructL(aAlgorithm, aOperationMode, aKey);
	return self;
	}

CSoftwareHash::CSoftwareHash()
	{		
	}

CSoftwareHash::~CSoftwareHash()
	{
	if (iHashImpl)
		{
		iHashImpl->Close();			
		}

	if (iHmacImpl)
		{
		iHmacImpl->Close();
		}
	delete iKey;
	}

void CSoftwareHash::ConstructL(TUid aAlgorithm, TUid aOperationMode, const CKey* aKey)
	{
	//
	// Only Hash and Hmac mode are supported.
	//
	if (aOperationMode!=KHmacModeUid && aOperationMode!=KHashModeUid)
		{
		User::Leave(KErrNotSupported);			
		}
		
	//Set the key if there is one
	if (aKey)
		{
		SetKeyL(*aKey);
		}
	
	switch (aAlgorithm.iUid)
		{
		/*case KCryptoPluginMd2:
			{
			iHashImpl=CMD2Impl::NewL();
			}
			break;*/
			
		case KTestPlugin02Md5_1:
			{
			iHashImpl=CMD5Impl::NewL(aAlgorithm);
			}
			break;
			
		/*case KTestPlugin02Sha1_1:
			{
			iHashImpl=CSHA1Impl::NewL(aAlgorithm);
			}
			break;*/
		
	default:
		User::Leave(KErrNotSupported);
		}
		
	SetOperationModeL(aOperationMode);
	}

void CSoftwareHash::SetOperationModeL(TUid aOperationMode)
	{
	switch (aOperationMode.iUid)
		{
	case KHmacMode:
		{
		//
		//Only create hmac implementation if there isn't one
		//
		if (!iHmacImpl)
			{
			if (iKey)
				{
				iHmacImpl=CHMacImpl::NewL(*iKey, iHashImpl);	
				}
			else
				{
				iHmacImpl=CHMacImpl::NewL(iHashImpl);
				}							
			}
		}
		break;
		
	case KHashMode:
		{
		Reset();	
		}
		break;
		
	default:
		User::Leave(KErrNotSupported);
		}
		
	//
	// Set the operation mode.
	//
	iOperationMode=aOperationMode;
	}

MSoftwareHash* CSoftwareHash::Impl()
	{
	MSoftwareHash* impl=NULL;
	if (iOperationMode==KHashModeUid)
		{
		impl=iHashImpl;
		}
	else if (iOperationMode==KHmacModeUid && iKey)
			{
			impl=iHmacImpl;
			}
	return impl;
	}
	
void CSoftwareHash::SetKeyL(const CKey& aKey)
	{
	Reset();
	delete iKey;
	iKey=CKey::NewL(aKey);
	if (iHmacImpl)
		{
		iHmacImpl->SetKeyL(aKey);
		}
	}
	
void CSoftwareHash::Reset()
	{
	if (iHashImpl)
		{
		iHashImpl->Reset();			
		}
		
	if (iHmacImpl)
		{
		iHmacImpl->Reset();			
		}
	}
	
void CSoftwareHash::Close()
	{
	delete this;
	}
	
void CSoftwareHash::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	MSoftwareHash* impl=Impl();
	if (impl)
		{
		impl->GetCharacteristicsL(aPluginCharacteristics);			
		}
	else
		{
		User::Leave(KErrNotReady);
		}		
	}
	
const CExtendedCharacteristics* CSoftwareHash::GetExtendedCharacteristicsL()
	{
	MSoftwareHash* impl=Impl();
	if (!impl)
		{
		User::Leave(KErrNotReady);	
		}
	return impl->GetExtendedCharacteristicsL();
	}
	
TAny* CSoftwareHash::GetExtension(TUid aExtensionId)
	{
	MSoftwareHash* impl=Impl();
	if (impl)
		{
		return impl->GetExtension(aExtensionId);			
		}
	else
		{
		return NULL;	
		}
	}

TPtrC8 CSoftwareHash::Hash(const TDesC8& aMessage)
	{
	MSoftwareHash* impl=Impl();
	if (impl)
		{
		return impl->Hash(aMessage);			
		}
	else
		{
		return KNullDesC8();
		}
	}
	
void CSoftwareHash::Update(const TDesC8& aMessage)
	{
	MSoftwareHash* impl=Impl();
	if (impl)
		{
		return impl->Update(aMessage);
		}
	}
	
TPtrC8 CSoftwareHash::Final(const TDesC8& aMessage)
	{
	MSoftwareHash* impl=Impl();
	if (impl)
		{
		return impl->Final(aMessage);
		}
	else
		{
		return KNullDesC8();
		}
	}
	
MHash* CSoftwareHash::ReplicateL()
	{
	CSoftwareHash* that=new(ELeave)CSoftwareHash();
	CleanupStack::PushL(that);
	if (this->iKey)
		{
		that->iKey=CKey::NewL(*this->iKey);			
		}
	that->iOperationMode=this->iOperationMode;
	that->iHashImpl=static_cast<MSoftwareHash*>(this->iHashImpl->ReplicateL());
	if (this->iHmacImpl)
		{
		that->iHmacImpl=static_cast<MSoftwareHash*>(this->iHmacImpl->ReplicateL());			
		}
	CleanupStack::Pop();
	return that;
	}
	
MHash* CSoftwareHash::CopyL()
	{
	CSoftwareHash* that=new(ELeave)CSoftwareHash();
	CleanupStack::PushL(that);
	if (this->iKey)
		{
		that->iKey=CKey::NewL(*this->iKey);			
		}
	that->iOperationMode=this->iOperationMode;
	that->iHashImpl=static_cast<MSoftwareHash*>(this->iHashImpl->CopyL());
	if (this->iHmacImpl)
		{
		that->iHmacImpl=static_cast<MSoftwareHash*>(this->iHmacImpl->CopyL());
		}
	CleanupStack::Pop();
	return that;
	}
		

