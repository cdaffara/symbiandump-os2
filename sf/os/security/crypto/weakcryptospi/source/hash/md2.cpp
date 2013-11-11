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


#include <e32std.h>
#include <hash.h>
#include "hashshim.h"

#define EXPANDLOOP

//
// 32bit endianness independent MD2 implementation
//
CMD2::CMD2()
	{		
	}
	
EXPORT_C CMD2* CMD2::NewL(void)
	{
	return CMD2Shim::NewL();
	}
	
EXPORT_C CMD2::~CMD2(void)
	{
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
EXPORT_C CMessageDigest* CMD2::ReplicateL(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);	
	return NULL;	
	}
	
EXPORT_C TPtrC8 CMD2::Hash(const TDesC8& /*aMessage*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);	
	return KNullDesC8();
	}

EXPORT_C CMessageDigest* CMD2::CopyL(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);	
	return NULL;	
	}

EXPORT_C TInt CMD2::BlockSize(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KErrNotSupported;
	}
	
EXPORT_C TInt CMD2::HashSize(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KErrNotSupported;
	}
	
EXPORT_C void CMD2::Reset(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}

EXPORT_C void CMD2::Update(const TDesC8& /*aMessage*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}

EXPORT_C TPtrC8 CMD2::Final(const TDesC8& /*aMessage*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KNullDesC8();
	}

EXPORT_C TPtrC8 CMD2::Final()
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KNullDesC8();
	}

void CMD2::RestoreState()
	{
	// Method replaced by shim
	ASSERT(EFalse);		
	}

void CMD2::StoreState()
	{
	// Method replaced by shim
	ASSERT(EFalse);		
	}
