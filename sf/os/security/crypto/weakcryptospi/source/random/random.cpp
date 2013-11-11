/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/

#include <e32std.h>
#include <e32math.h>
#include <random.h>
#include <hash.h>
#include "randomshim.h"

extern "C" {
EXPORT_C void RAND_bytes(unsigned char* buf,int bytes)
	{
	TPtr8 ptr(buf,bytes,bytes);
	buf[0]++;
	TRandom::Random(ptr);
	}
}

EXPORT_C CRandom::CRandom(void)
	{
	}

EXPORT_C CSystemRandom* CSystemRandom::NewL(void)
	{
	CSystemRandom* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSystemRandom* CSystemRandom::NewLC(void)
	{
	CSystemRandom* self = new(ELeave)CSystemRandom();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSystemRandom::GenerateBytesL(TDes8& aDest)
	{
	TRAPD(error, iShim->GenerateBytesL(aDest));
    // This method must leave on low memory conditions.
    if(error == KErrNoMemory)
        {
        User::Leave(error); 
        }
    }

CSystemRandom::CSystemRandom(void)
	{
	}

CSystemRandom::~CSystemRandom()
	{
	delete iShim;
	}

void CSystemRandom::ConstructL()
	{
	iShim = CRandomShim::NewL();
	}

// Methods replace by shim are excluded from coverage.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
EXPORT_C void TRandom::Random(TDes8& aDestination)
	{
	// Method replaced by shim
	TRandomShim::Random(aDestination);
	}

EXPORT_C void TRandom::RandomL(TDes8& aDestination)
	{
	// Method replaced by shim
	TRandomShim::RandomL(aDestination);
	}
	
EXPORT_C void TRandom::SecureRandomL(TDes8& aDestination)
	{
	// Method replaced by shim
	TRandomShim::SecureRandomL(aDestination);
	}

EXPORT_C RRandomSession::RRandomSession(void)
	{
	}

EXPORT_C void RRandomSession::ConnectL(void)
	{
	// No action required
	}

EXPORT_C TInt RRandomSession::GetRandom(TDes8& aDestination)
	{
	// Method replaced by shim
	TRandomShim::Random(aDestination);
	return KErrNone;
	}
