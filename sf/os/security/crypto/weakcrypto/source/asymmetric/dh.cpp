/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <asymmetric.h>
#include <asymmetrickeys.h>
#include <bigint.h>

EXPORT_C CDH* CDH::NewL(const CDHPrivateKey& aPrivateKey)
	{
	CDH* self = new(ELeave) CDH(aPrivateKey);
	return self;
	}

EXPORT_C CDH* CDH::NewLC(const CDHPrivateKey& aPrivateKey)
	{
	CDH* self = NewL(aPrivateKey);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C HBufC8* CDH::AgreeL(const CDHPublicKey& aPublicKey) const
	{
	//Calculate X^x mod N
	RInteger result = TInteger::ModularExponentiateL(aPublicKey.X(), 
		iPrivateKey.x(), aPublicKey.N());
	CleanupStack::PushL(result);
	HBufC8* key = result.BufferLC();
	CleanupStack::Pop(key);
	CleanupStack::PopAndDestroy(&result);
	return key;
	}

EXPORT_C CDH::CDH(const CDHPrivateKey& aPrivateKey) : iPrivateKey(aPrivateKey)
	{
	}


