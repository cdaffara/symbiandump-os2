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
* random shim implementation
* random shim implementation
*
*/


/**
 @file
*/

#include "randomshim.h"
#include <cryptospi/cryptospidef.h>
#include <cryptospi/cryptorandomapi.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/keys.h>
#include <e32debug.h>
#include "securityerr.h"

using namespace CryptoSpi;

_LIT(KRandomFail, "Cannot obtain randomness");

//
// Random shim implementation
//
CRandomShim* CRandomShim::NewL()
	{
	CRandomShim* self = CRandomShim::NewLC();
	CleanupStack::Pop();
	return self;
	}

CRandomShim* CRandomShim::NewLC()
	{
	CRandomShim* self = new(ELeave) CRandomShim();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CRandomShim::GenerateBytesL(TDes8& aDest)
	{	
	iRandomImpl->GenerateRandomBytesL(aDest);
	}

CRandomShim::CRandomShim()
	{
	}

CRandomShim::~CRandomShim()
	{
	delete iRandomImpl;
	}
	
void CRandomShim::ConstructL()
	{
	CRandomFactory::CreateRandomL(iRandomImpl, KRandomUid, NULL);	
	}

/**
 * @deprecated Use RandomL() instead
 * @panic This function can panic under low memory conditions
 * See PDEF097319: TRandom::Random panics during OOM
 * This method is preserved only for BC reasons
 */
void TRandomShim::Random(TDes8& aDest)
	{
	CRandomShim* rand = NULL;
	TRAPD(ret, rand = CRandomShim::NewL());
	if (ret != KErrNone)
		{
		User::Panic(KRandomFail, ret);
		}
	TRAPD(ret2, rand->GenerateBytesL(aDest));
	delete rand;
	if ((ret2 != KErrNone) && (ret2 != KErrNotSecure))
		{
		// this method can't leave so the cleanup stack can't be used (because of PushL()) 
		// so we just delete the randon shim here if GenerateBytesL() leaves
		User::Panic(KRandomFail, ret);
		}
	}

void TRandomShim::RandomL(TDes8& aDest)
	{
	CRandomShim* rand = CRandomShim::NewL();
	CleanupStack::PushL(rand);
	
	TRAPD(error, rand->GenerateBytesL(aDest));
	CleanupStack::PopAndDestroy(rand); // Use a singleton, avoid new overhead?
	
	// This method should leave on low memory conditions.
	if(error == KErrNoMemory)
		{
		User::Leave(error);	
		}
	}	

void TRandomShim::SecureRandomL(TDes8& aDest)
	{
	CRandomShim* rand = CRandomShim::NewLC();	
	
	rand->GenerateBytesL(aDest);
	CleanupStack::PopAndDestroy(rand);	
	}
