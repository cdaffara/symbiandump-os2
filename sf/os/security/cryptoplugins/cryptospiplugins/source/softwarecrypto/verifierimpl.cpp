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


#include <e32def.h>
#include <e32cmn.h>
#include "keys.h"

#include "verifierimpl.h"

#include <cryptospi/cryptospidef.h>
#include "pluginconfig.h"

using namespace SoftwareCrypto;

CVerifierImpl::CVerifierImpl()
	{
	}

CVerifierImpl::~CVerifierImpl()
	{
	delete iKey;
	}

void CVerifierImpl::Close()
	{
	delete this;
	}
	
void CVerifierImpl::Reset()
	{
	}
	
TAny* CVerifierImpl::GetExtension(TUid /*aExtensionId*/)
	{
	return 0;
	}
	
void CVerifierImpl::GetCharacteristicsL(const TAny*& aPluginCharacteristics)
	{
	TInt numCiphers = sizeof(KVerifierCharacteristics)/sizeof(TAsymmetricSignatureCharacteristics*);
	TInt32 implUid = ImplementationUid().iUid;
	for (TInt i = 0; i < numCiphers; ++i)
		{
		if (KVerifierCharacteristics[i]->cmn.iImplementationUID == implUid)
			{
			aPluginCharacteristics = KVerifierCharacteristics[i];
			break;
			}
		}
	}

void CVerifierImpl::DoSetKeyL(const CKey& aKey)
	{
	delete iKey;
	iKey = CKey::NewL(aKey);
	}

void CVerifierImpl::ConstructL(const CKey& aPrivateKey)
	{
	SetKeyL(aPrivateKey);	
	}

// Methods implemented in subclass. No coverage here.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
void CVerifierImpl::SetPaddingModeL(TUid /*aPaddingMode*/) 
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	}

void CVerifierImpl::SetKeyL(const CKey& /*aPublicKey*/)
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	}

TInt CVerifierImpl::GetMaximumInputLengthL() const
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	return 0;
	}

TInt CVerifierImpl::GetMaximumOutputLengthL() const
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	return 0;
	}
