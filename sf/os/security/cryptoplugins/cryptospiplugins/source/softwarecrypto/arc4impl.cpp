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


#include "arc4impl.h"
#include "keys.h"
#include "pluginconfig.h"
#include "symmetriccipherimpl.h"
#include <cryptostrength.h>
#include "common/inlines.h"


using namespace SoftwareCrypto;

CArc4Impl* CArc4Impl::NewL(const CKey& aKey, TInt aDiscardBytes)
	{
	CArc4Impl* self = CArc4Impl::NewLC(aKey, aDiscardBytes);
	CleanupStack::Pop(self);
	return self;		
	}

CArc4Impl* CArc4Impl::NewLC(const CKey& aKey, TInt aDiscardBytes)
	{
	CArc4Impl* self = new(ELeave) CArc4Impl(aDiscardBytes);
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	
	const TDesC8& keyContent = aKey.GetTDesC8L(KSymmetricKeyParameterUid);
	TCrypto::IsSymmetricWeakEnoughL(BytesToBits(keyContent.Size()) - keyContent.Size());

	return self;		
	}

CArc4Impl::CArc4Impl(TInt aDiscardBytes)
:ix(1), iy(0), iDiscardBytes(aDiscardBytes)
	{
	}

CArc4Impl::~CArc4Impl()
	{
	}

void CArc4Impl::ConstructL(const CKey& aKey)
	{
	CSymmetricStreamCipherImpl::ConstructL(aKey);
	GenerateSBox();
	}

CExtendedCharacteristics* CArc4Impl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}
	
const CExtendedCharacteristics* CArc4Impl::GetExtendedCharacteristicsL()
	{
	return CArc4Impl::CreateExtendedCharacteristicsL();
	}

void CArc4Impl::DoProcess(TDes8& aData)
	{
	TInt blockLen = aData.Size();

	if (blockLen > 0)
		{
		TUint8* blockPtr = (TUint8*)&aData[0];	
		do
			{
			*blockPtr++ ^= GenerateByte();
			} 
		while (--blockLen);
		}
	}
	
TBool CArc4Impl::IsValidKeyLength(TInt aKeyBytes) const
	{
	return ((aKeyBytes > 0 && aKeyBytes <= KMaxARC4KeyBytes) ? ETrue : EFalse);
	}

void CArc4Impl::Reset()
	{
	ix = 1;
	iy = 0;
	GenerateSBox();
	}

TUid CArc4Impl::ImplementationUid() const
	{
	return KCryptoPluginArc4Uid;
	}
	
TUint8 CArc4Impl::GenerateByte()
	{
	TUint8 a = iState[ix];
	iy = (TUint8)((iy + a) & 0xff);
	TUint8 b = iState[iy];

	iState[ix] = b;
	iState[iy] = a;
	ix = (TUint8)((ix + 1) & 0xff);
	return (iState[(a + b) & 0xff]);
	}

void CArc4Impl::DiscardBytes(TInt aDiscardBytes)
	{	
	if (aDiscardBytes > 0)
		{
		do
			{
			GenerateByte();
			}
		while(--aDiscardBytes);
		}
	}

void CArc4Impl::GenerateSBox(void)
	{
	TUint keyBytes = iKey->Size();
		
	TInt i = 0;
	for (; i < KSBoxSize; i++)
		iState[i] = (TUint8)i;
	
	TUint keyIndex = 0, stateIndex = 0;
	i = 0;
	for (; i < KSBoxSize; i++)
		{
		TUint a = iState[i];
		stateIndex += (*iKey)[keyIndex] + a;
		stateIndex &= 0xff;
		iState[i] = iState[stateIndex];
		iState[stateIndex] = (TUint8)a;
		if (++keyIndex >= (TUint)keyBytes)
			keyIndex = 0;
		}

	DiscardBytes(iDiscardBytes);
	}
