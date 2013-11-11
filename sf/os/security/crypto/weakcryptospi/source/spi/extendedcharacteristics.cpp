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
* crypto plugin extended characteristic implementation
*
*/


#include <cryptospi/extendedcharacteristics.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/cryptospidef.h>

using namespace CryptoSpi;

//
// Implementation of Extended Characteristics
//

EXPORT_C CExtendedCharacteristics* CExtendedCharacteristics::NewL(TInt aAvailableConcurrency,
																	TBool aExclusiveUse)
	{
	CExtendedCharacteristics* self = new(ELeave) CExtendedCharacteristics();
	CleanupStack::PushL(self);
	self->ConstructL(aAvailableConcurrency, aExclusiveUse);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CExtendedCharacteristics::~CExtendedCharacteristics()
	{
	delete iExtCharacteristics;
	}

EXPORT_C void CExtendedCharacteristics::AddCharacteristicL(const TInt aValue, TUid aUid)
	{
	iExtCharacteristics->AddL(aValue, aUid);
	}

EXPORT_C void CExtendedCharacteristics::AddCharacteristicL(const TDesC8& aValue, TUid aUid)
	{
	iExtCharacteristics->AddL(aValue, aUid);
	}

EXPORT_C TInt CExtendedCharacteristics::GetTIntCharacteristicL(TUid aUid) const
	{
	return iExtCharacteristics->GetTIntL(aUid);
	}

EXPORT_C const TDesC8& CExtendedCharacteristics::GetTDesC8CharacteristicL(TUid aUid) const
	{
	return iExtCharacteristics->GetTDesC8L(aUid);
	}

EXPORT_C const CCryptoParams* CExtendedCharacteristics::ListExtendedCharacteristics() const
	{
	return iExtCharacteristics;
	}

void CExtendedCharacteristics::ConstructL(TInt aAvailableConcurrency, TBool aExclusiveUse)
	{
	iExtCharacteristics = CCryptoParams::NewL();
	iExtCharacteristics->AddL(aAvailableConcurrency, KConcurrencyTypeUid);
	iExtCharacteristics->AddL(aExclusiveUse, KExclusiveUseTypeUid);
	}

CExtendedCharacteristics::CExtendedCharacteristics()
	{
	}
