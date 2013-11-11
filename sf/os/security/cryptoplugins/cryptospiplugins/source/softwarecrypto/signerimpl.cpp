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
#include <cryptospi/cryptospidef.h>

#include "signerimpl.h"
#include "pluginconfig.h"

using namespace SoftwareCrypto;

CSignerImpl::CSignerImpl()
	{
	}

CSignerImpl::~CSignerImpl()
	{
	delete iKey;
	}

void CSignerImpl::Close()
	{
	delete this;
	}
	
void CSignerImpl::Reset()
	{
	}
	
TAny* CSignerImpl::GetExtension(TUid /*aExtensionId*/)
	{
	return 0;
	}
	
void CSignerImpl::GetCharacteristicsL(const TAny*& aPluginCharacteristics)
	{
	TInt numCiphers = sizeof(KSignerCharacteristics)/sizeof(TAsymmetricSignatureCharacteristics*);
	TInt32 implUid = ImplementationUid().iUid;
	for (TInt i = 0; i < numCiphers; ++i)
		{
		if (KSignerCharacteristics[i]->cmn.iImplementationUID == implUid)
			{
			aPluginCharacteristics = KSignerCharacteristics[i];
			break;
			}
		}
	}

void CSignerImpl::DoSetKeyL(const CKey& aPrivateKey)
	{
	delete iKey;
	iKey = CKey::NewL(aPrivateKey);
	}

void CSignerImpl::ConstructL(const CKey& aPrivateKey)
	{
	SetKeyL(aPrivateKey);
	}


// Methods implemented in subclass. No coverage here.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
void CSignerImpl::SetPaddingModeL(TUid /*aPaddingMode*/)
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	}
	
void CSignerImpl::SetKeyL(const CKey& /*aPrivateKey*/) 
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	}

TInt CSignerImpl::GetMaximumInputLengthL() const 
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	return 0;
	}

TInt CSignerImpl::GetMaximumOutputLengthL() const
	{
	// Override in subclass
	User::Leave(KErrNotSupported);
	return 0;
	}
