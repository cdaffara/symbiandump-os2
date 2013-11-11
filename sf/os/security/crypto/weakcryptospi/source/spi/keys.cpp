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
* Generic Key implementation
* Generic Key implementation
*
*/


/**
 @file
*/

#include <cryptospi/keys.h>
#include <cryptospi/cryptoparams.h>
#include <cryptospi/cryptospidef.h>
#include <cryptopanic.h>

using namespace CryptoSpi;

EXPORT_C CKey* CKey::NewL(const CKey& aKey)
	{
	CKey* self=NewLC(aKey);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CKey* CKey::NewLC(const CKey& aKey)
	{
	CKey* self=new(ELeave) CKey(aKey.iKeyProperty);
	CleanupStack::PushL(self);
	self->ConstructL(*aKey.iKeyParameters);
	return self;
	}

EXPORT_C CKey* CKey::NewL(const TKeyProperty& aKeyProperty, const CCryptoParams& aKeyParameters)
	{
	CKey* self=NewLC(aKeyProperty, aKeyParameters);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CKey* CKey::NewLC(const TKeyProperty& aKeyProperty, const CCryptoParams& aKeyParameters)
	{
	CKey* self=new(ELeave) CKey(aKeyProperty);
	CleanupStack::PushL(self);
	self->ConstructL(aKeyParameters);
	return self;
	}
	
EXPORT_C const TKeyProperty& CKey::KeyProperty() const
	{
	return iKeyProperty;
	}
	
CKey::CKey(const TKeyProperty& aKeyProperty) : iKeyProperty(aKeyProperty)
	{
	}

EXPORT_C CKey::~CKey()
	{
	// The destructors for each CCryptoParam in this array should securely
	// delete their contents, e.g. RSA/DSA BigInt exponent and modulus.
	delete iKeyParameters;
	}

EXPORT_C const TInteger& CKey::GetBigIntL(TUid aUid) const
	{
	return iKeyParameters->GetBigIntL(aUid);
	}

EXPORT_C TInt CKey::GetTIntL(TUid aUid) const
	{
	return iKeyParameters->GetTIntL(aUid);
	}

EXPORT_C const TDesC8& CKey::GetTDesC8L(TUid aUid) const
	{
	return iKeyParameters->GetTDesC8L(aUid);
	}

EXPORT_C TBool CKey::IsPresent(TUid aUid) const
	{
	return iKeyParameters->IsPresent(aUid);
	}

void CKey::ConstructL(const CCryptoParams& aKeyParameters)
	{
	iKeyParameters = CCryptoParams::NewL();
	iKeyParameters->CopyL(aKeyParameters);
	}

EXPORT_C const CCryptoParams& CKey::KeyParameters() const
	{
	return *iKeyParameters;
	}


