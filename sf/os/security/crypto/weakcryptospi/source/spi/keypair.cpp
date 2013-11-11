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
* Generic Keypair implementation
* Generic Keypair implementation
*
*/


/**
 @file
*/

#include <cryptospi/keypair.h>
#include <cryptospi/cryptoparams.h>
#include <cryptospi/keys.h>

#include "../common/inlines.h"

using namespace CryptoSpi;


EXPORT_C CKeyPair* CKeyPair::NewL(CKey* aPublicKey, CKey* aPrivateKey)
	{
	CKeyPair* self = new(ELeave) CKeyPair();
	CleanupStack::PushL(self);
	self->ConstructL(aPublicKey, aPrivateKey);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C const CKey& CKeyPair::PublicKey() const
	{
	return *iPublicKey;
	}
	
EXPORT_C const CKey& CKeyPair::PrivateKey() const
	{
	return *iPrivateKey;
	}

CKeyPair::CKeyPair()
	{
	}

CKeyPair::~CKeyPair()
	{
	delete iPublicKey;
	delete iPrivateKey;
	}

void CKeyPair::ConstructL(CKey* aPublicKey, CKey* aPrivateKey)
	{
	iPublicKey = aPublicKey;
	iPrivateKey = aPrivateKey;
	}
