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
// 32bit endianness independent MD5 implementation
//
CMD5::CMD5()
	{
	}
	
EXPORT_C CMD5* CMD5::NewL(void)
	{
	CMD5Shim* self=CMD5Shim::NewL();
	self->Reset();
	return self;		
	}
		
EXPORT_C CMD5::~CMD5(void)
	{
	}

// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
EXPORT_C CMessageDigest* CMD5::ReplicateL(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return NULL;	
	}
	
EXPORT_C TPtrC8 CMD5::Hash(const TDesC8& /*aMessage*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KNullDesC8();
	}

EXPORT_C CMessageDigest* CMD5::CopyL(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return NULL;	
	}
	
EXPORT_C TInt CMD5::BlockSize(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KErrNotSupported;
	}
	
EXPORT_C TInt CMD5::HashSize(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KErrNotSupported;
	}
	
EXPORT_C void CMD5::Reset(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}

EXPORT_C void CMD5::Update(const TDesC8& /*aMessage*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}
	
EXPORT_C TPtrC8 CMD5::Final(const TDesC8& /*aMessage*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KNullDesC8();
	}

EXPORT_C TPtrC8 CMD5::Final()
	{
	// Method replaced by shim
	ASSERT(EFalse);
	return KNullDesC8();
	}

void CMD5::RestoreState()
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}

void CMD5::StoreState()
	{
	// Method replaced by shim
	ASSERT(EFalse);
	}
