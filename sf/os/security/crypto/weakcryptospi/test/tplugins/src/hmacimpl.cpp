/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* software hmac implementation
* software hmac implementation
*
*/


/**
 @file
*/

#include "hmacimpl.h"
#include "pluginconfig.h"
#include "md2impl.h"
#include "md5impl.h"
#include "sha1impl.h"
#include <cryptospi/keys.h>

using namespace SoftwareCrypto;

CHMacImpl* CHMacImpl::NewL(const CKey& aKey, MSoftwareHash* aHash)
	{
	CHMacImpl* self=NewLC(aKey, aHash);
	CleanupStack::Pop();
	return self;
	}
	
CHMacImpl* CHMacImpl::NewLC(const CKey& aKey, MSoftwareHash* aHash)
	{
	CHMacImpl* self=new(ELeave) CHMacImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aKey, aHash);
	return self;
	}

CHMacImpl* CHMacImpl::NewL(MSoftwareHash* aHash)
	{
	CHMacImpl* self=NewLC(aHash);
	CleanupStack::Pop();
	return self;
	}
	
CHMacImpl* CHMacImpl::NewLC(MSoftwareHash* aHash)
	{
	CHMacImpl* self=new(ELeave) CHMacImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aHash);
	return self;
	}


CHMacImpl::CHMacImpl()
	: iInnerPad(KHMacPad), iOuterPad(KHMacPad)
	{
	}

CHMacImpl::CHMacImpl(const CHMacImpl& aMD)
	: iDigest(NULL), iInnerPad(aMD.iInnerPad), iOuterPad(aMD.iOuterPad)
	{
	}

CHMacImpl::~CHMacImpl()
	{
	if (iDigest)
		{
		iDigest->Close();	
		}
	}

void CHMacImpl::ConstructL(const CKey& aKey, MSoftwareHash* aHash)
	{
	//Clone the hash implementation
	iDigest=static_cast<MSoftwareHash*>(aHash->ReplicateL());
	const TDesC8& keyContent=aKey.GetTDesC8L(KHmacKeyParameterUid);
	Initialise(keyContent);	
	}

void CHMacImpl::ConstructL(MSoftwareHash* aHash)
	{
	//Clone the hash implementation
	iDigest=static_cast<MSoftwareHash*>(aHash->ReplicateL());
	}

	
void CHMacImpl::Initialise(const TDesC8& aKey)
	{
	// initialisation
	if (iDigest)
		{
		iDigest->Reset();
		if( (TUint32)aKey.Size() > KHMacPad)
			{
			iInnerPad = iDigest->Final(aKey);
			}
		else 
			{
			iInnerPad = aKey;
			}
			
		TUint i;
		for (i=iInnerPad.Size();i<KHMacPad;i++)
			iInnerPad.Append(0);

		iOuterPad=iInnerPad;

		const TUint8 Magic1=0x36, Magic2=0x5c;
		for (i=0;i<KHMacPad;i++)
			{
			iInnerPad[i]^=Magic1;
			iOuterPad[i]^=Magic2;
			}
		//start inner hash
		iDigest->Hash(iInnerPad);
		}
	}
	
MHash* CHMacImpl::CopyL()
	{
	CHMacImpl* that=new(ELeave) CHMacImpl(*this);
	CleanupStack::PushL(that);
	that->iDigest=iDigest ? static_cast<MSoftwareHash*>(iDigest->CopyL()) : NULL;
	CleanupStack::Pop();
	return that;
	}
	
MHash* CHMacImpl::ReplicateL()
	{
	CHMacImpl* that=new(ELeave) CHMacImpl(*this);
	CleanupStack::PushL(that);
	that->iDigest=iDigest ? static_cast<MSoftwareHash*>(iDigest->ReplicateL()) : NULL;
	that->Reset();
	CleanupStack::Pop();
	return that;
	}
	
void CHMacImpl::Reset()
	{
	if (iDigest)
		{
		iDigest->Reset();
		iDigest->Update(iInnerPad);
		}
	}

TPtrC8 CHMacImpl::Hash(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	TPtrC8 finalPtr(KNullDesC8());
	StoreState();
	if (iDigest)
		{
		ptr.Set(iDigest->Final(aMessage));
		iDigest->Update(iOuterPad);
		finalPtr.Set(iDigest->Final(ptr));
		}

	RestoreState();
	iDigest->Update(aMessage);

	return (finalPtr);
	}

void CHMacImpl::Update(const TDesC8& aMessage)
	{
	if(iDigest)
		{
		iDigest->Update(aMessage);
		}
	}

TPtrC8 CHMacImpl::Final(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	if(iDigest)
		{
		ptr.Set(iDigest->Final(aMessage));
		iDigest->Update(iOuterPad);
		iDigest->Final(ptr);
		Reset();
		}
	return (ptr);
	}
	
void CHMacImpl::RestoreState()
	{
	iOuterPad.Copy(iOuterPadCopy);
	iInnerPad.Copy(iInnerPadCopy);
	if (iDigest)
		{
		iDigest->RestoreState();
		}
	}

void CHMacImpl::StoreState()
	{
	iOuterPadCopy.Copy(iOuterPad);
	iInnerPadCopy.Copy(iInnerPad);
	if (iDigest)
		{
		iDigest->StoreState();	
		}
	}

void CHMacImpl::SetKeyL(const CKey& aKey)
	{
	const TDesC8& keyContent=aKey.GetTDesC8L(KHmacKeyParameterUid);
	Initialise(keyContent);	
	}

void CHMacImpl::Close()
	{
	delete this;	
	}
	
void CHMacImpl::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	iDigest->GetCharacteristicsL(aPluginCharacteristics);	
	}
	
const CExtendedCharacteristics* CHMacImpl::GetExtendedCharacteristicsL()
	{
	return iDigest->GetExtendedCharacteristicsL();
	}
	
TAny* CHMacImpl::GetExtension(TUid /*aExtensionId*/)
	{
	return NULL;	
	}
	
void CHMacImpl::SetOperationModeL(TUid /*aOperationMode*/)
	{
	User::Leave(KErrNotSupported);
	}

