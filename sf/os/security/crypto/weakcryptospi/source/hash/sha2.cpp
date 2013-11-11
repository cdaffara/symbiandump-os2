/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <hash.h>
#include "hashshim.h"

//
// SHA-2 implementation
//
CSHA2::CSHA2()
	{		
	}
	
EXPORT_C CSHA2* CSHA2::NewL(TSH2Algo aAlgorithmId)
	{
	CSHA2* self = CSHA2::NewLC(aAlgorithmId);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSHA2* CSHA2::NewLC(TSH2Algo aAlgorithmId)
	{
	CSHA2* self =CSHA2Shim::NewLC(aAlgorithmId);
	self->Reset();
	return self;						
	}	
	
EXPORT_C CSHA2::~CSHA2(void)
	{
	}
	
// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

void CSHA2::RestoreState()
	{
	ASSERT(EFalse);
	}

void CSHA2::StoreState()
	{
	ASSERT(EFalse);
	}
