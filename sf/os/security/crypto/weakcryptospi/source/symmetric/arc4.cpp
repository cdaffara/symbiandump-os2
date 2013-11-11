/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "arc4.h"
#include "arc4shim.h"


EXPORT_C CARC4* CARC4::NewL(const TDesC8& aKey, TUint aDiscardBytes)
	{
	return CArc4Shim::NewL(aKey, aDiscardBytes);
	}

EXPORT_C CARC4* CARC4::NewLC(const TDesC8& aKey, TUint aDiscardBytes)
	{
	return CArc4Shim::NewLC(aKey, aDiscardBytes);
	}

CARC4::CARC4()
	{	
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

void CARC4::Reset()
	{
	// Method replaced by shim 
	ASSERT(EFalse);		
	}
	
TInt CARC4::KeySize() const
	{
	// Method replaced by shim 
	ASSERT(EFalse);	
	return 0;
	}

void CARC4::DoProcess(TDes8& /*aData*/)
	{
	// Method replaced by shim 
	ASSERT(EFalse);	
	}

