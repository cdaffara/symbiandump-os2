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
*
*/


#include <e32def.h>
#include <cryptospi/keys.h>
#include "asymmetriccipherimpl.h"
#include <cryptostrength.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>
#include "pluginconfig.h"
#include "../../../source/common/inlines.h"

using namespace SoftwareCrypto;

CAsymmetricCipherImpl::CAsymmetricCipherImpl(
	TUid aCryptoMode,
	TUid aPaddingMode) :
	iCryptoMode(aCryptoMode),
	iPaddingMode(aPaddingMode)
	{
	}

void CAsymmetricCipherImpl::ConstructL(const CKey& aKey)
	{
	DoSetCryptoModeL(iCryptoMode);
	DoSetKeyL(aKey);
	DoSetPaddingModeL(iPaddingMode);
	}

CAsymmetricCipherImpl::~CAsymmetricCipherImpl()
	{
	delete iKey;
	delete iPadding;
	}
		
void CAsymmetricCipherImpl::Close()
	{
	delete this;
	}
	
void CAsymmetricCipherImpl::Reset()
	{
	}
	
TAny* CAsymmetricCipherImpl::GetExtension(TUid /*aExtensionId*/)
	{
	return 0;
	}
	
void CAsymmetricCipherImpl::GetCharacteristicsL(const TAny*& aPluginCharacteristics)
	{
	TInt numCiphers = sizeof(KAsymmetricCipherCharacteristics)/sizeof(TAsymmetricCipherCharacteristics*);
	TInt32 implUid = ImplementationUid().iUid;
	for (TInt i = 0; i < numCiphers; ++i)
		{
		if (KAsymmetricCipherCharacteristics[i]->cmn.iImplementationUID == implUid)
			{
			aPluginCharacteristics = KAsymmetricCipherCharacteristics[i];
			break;
			}
		}
	}

TInt CAsymmetricCipherImpl::GetMaximumOutputLengthL() const
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	return 0;
	}

TInt CAsymmetricCipherImpl::GetMaximumInputLengthL() const
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	return 0;
	}

void CAsymmetricCipherImpl::SetKeyL(const CKey& aKey)
	{
	DoSetKeyL(aKey);
	}

void CAsymmetricCipherImpl::DoSetKeyL(const CKey& aKey)
	{
	delete iKey;								// delete any previous key
	iKey = CKey::NewL(aKey);
	}

void CAsymmetricCipherImpl::SetCryptoModeL(TUid aCryptoMode)
	{
	DoSetCryptoModeL(aCryptoMode);
	Reset();
	}
	
void CAsymmetricCipherImpl::DoSetCryptoModeL(TUid aCryptoMode)
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
	}

void CAsymmetricCipherImpl::SetPaddingModeL(TUid aPaddingMode)
	{
	DoSetPaddingModeL(aPaddingMode);
	Reset();
	}
	
void CAsymmetricCipherImpl::DoSetPaddingModeL(TUid aPaddingMode)
	{
	CPadding* padding(0);
	TInt padlength = 0;
	
	switch (iCryptoMode.iUid)
		{
		case KCryptoModeEncrypt:
			padlength = GetMaximumOutputLengthL();
			break;
		case KCryptoModeDecrypt:
			padlength = GetMaximumInputLengthL();
			break;
		}
		
	switch (aPaddingMode.iUid)
		{
		case KPaddingModeNone:
			padding = CPaddingNone::NewL(padlength);
			break;
		case KPaddingModePkcs1_v1_5_Encryption:
			padding = CPaddingPKCS1Encryption::NewL(padlength);
			break;
		default:
			User::Leave(KErrNotSupported);
		}
		
	delete iPadding;
	iPadding = padding;
	iPaddingMode = aPaddingMode;
	}
