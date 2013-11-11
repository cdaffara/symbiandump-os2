/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "keyagreementimpl.h"
#include <cryptospi/plugincharacteristics.h>
#include "pluginconfig.h"

using namespace SoftwareCrypto;

CKeyAgreementImpl::CKeyAgreementImpl()
	{
	}

void CKeyAgreementImpl::ConstructL(const CKey& aPrivateKey, const CCryptoParams* aParams)
	{
	SetKeyL(aPrivateKey, aParams);
	}
	
CKeyAgreementImpl::~CKeyAgreementImpl()
	{
	delete iSharedParams;
	delete iPrivateKey;
	}

void CKeyAgreementImpl::Close()
	{
	delete this;
	}
	
void CKeyAgreementImpl::Reset()
	{
	}
	
TAny* CKeyAgreementImpl::GetExtension(TUid /*aExtensionId*/)
	{
	return 0;
	}

void CKeyAgreementImpl::GetCharacteristicsL(const TAny*& aPluginCharacteristics)
	{
	TInt numCiphers = sizeof(KKeyAgreementCharacteristics)/sizeof(TKeyAgreementCharacteristics*);
	TInt32 implUid = ImplementationUid().iUid;
	for (TInt i = 0; i < numCiphers; ++i)
		{
		if (KKeyAgreementCharacteristics[i]->cmn.iImplementationUID == implUid)
			{
			aPluginCharacteristics = KKeyAgreementCharacteristics[i];
			break;
			}
		}
	}

void CKeyAgreementImpl::SetKeyL(const CKey& aSelfPrivateKey, const CCryptoParams* aSharedParams)
	{
	delete iPrivateKey;
	iPrivateKey = CKey::NewL(aSelfPrivateKey);
	delete iSharedParams;
	iSharedParams = CCryptoParams::NewL();
	iSharedParams->CopyL(*aSharedParams);			// copy contents of the object
	}
