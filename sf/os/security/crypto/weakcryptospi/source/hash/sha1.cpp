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
// 32bit endian-independent SHA-1 implementation
//
CSHA1::CSHA1()
	{		
	}
	
EXPORT_C CSHA1* CSHA1::NewL(void)
	{
	CSHA1* self=CSHA1Shim::NewL();
	self->Reset();
	return self;
	}
	
EXPORT_C CSHA1::~CSHA1(void)
	{
	}
	
// All these methods have been replaced by the shim
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif
EXPORT_C CMessageDigest* CSHA1::ReplicateL(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);	
	return NULL;
	}

EXPORT_C TPtrC8 CSHA1::Hash(const TDesC8& /*aMessage*/)
	{
	// Method replaced by shim
	ASSERT(EFalse);	
	return KNullDesC8();
	}

EXPORT_C CMessageDigest* CSHA1::CopyL(void)
	{
	// Method replaced by shim
	ASSERT(EFalse);	
	return NULL;
	}
	
EXPORT_C TInt CSHA1::BlockSize(void)
	{
	ASSERT(EFalse);	
	return KErrNotSupported;
	}
	
EXPORT_C TInt CSHA1::HashSize(void)
	{
	ASSERT(EFalse);
	return KErrNotSupported;
	}

EXPORT_C void CSHA1::Reset(void)
	{
	ASSERT(EFalse);
	}

EXPORT_C void CSHA1::Update(const TDesC8& /*aMessage*/)
	{
	ASSERT(EFalse);
	}

EXPORT_C TPtrC8 CSHA1::Final(const TDesC8& /*aMessage*/)
	{
	ASSERT(EFalse);	
	return KNullDesC8();
	}

EXPORT_C TPtrC8 CSHA1::Final()
	{
	ASSERT(EFalse);
	return KNullDesC8();
	}

void CSHA1::RestoreState()
	{
	ASSERT(EFalse);
	}

void CSHA1::StoreState()
	{
	ASSERT(EFalse);
	}

///////////////////////////////////////////////////////////////////////////////////////
//	CSHA code is deprecated
///////////////////////////////////////////////////////////////////////////////////////

_LIT(KSHA, "HASH::CSHA");

EXPORT_C CSHA* CSHA::NewL(void)
{
	User::Panic(KSHA, KErrNotSupported);
	return (NULL);	//	Shut compiler up
}

EXPORT_C CSHA::~CSHA(void)
{
	User::Panic(KSHA, KErrNotSupported);
}

EXPORT_C TPtrC8 CSHA::Hash(const TDesC8& /*aMessage*/)
{
	User::Panic(KSHA, KErrNotSupported);
	return (KNullDesC8());	//	Shut compiler up
}

EXPORT_C TInt CSHA::HashSize(void)
{
	User::Panic(KSHA, KErrNotSupported);
	return (-1);	//	Shut compiler up
}

EXPORT_C TInt CSHA::BlockSize(void)
{
	User::Panic(KSHA, KErrNotSupported);
	return (-1);	//	Shut compiler up
}

EXPORT_C CMessageDigest* CSHA::CopyL(void)
{
	User::Panic(KSHA, KErrNotSupported);
	return (NULL);	//	Shut compiler up
}

EXPORT_C CMessageDigest* CSHA::ReplicateL(void)
{
	User::Panic(KSHA, KErrNotSupported);
	return (NULL);	//	Shut compiler up
}

EXPORT_C void CSHA::Reset(void)
{
	User::Panic(KSHA, KErrNotSupported);
}

EXPORT_C void CSHA::Update(const TDesC8& /*aMessage*/)
{
	User::Panic(KSHA, KErrNotSupported);
}

EXPORT_C TPtrC8 CSHA::Final(const TDesC8& /*aMessage*/)
{
	User::Panic(KSHA, KErrNotSupported);
	return (KNullDesC8());	//	Shut compiler up
}

EXPORT_C TPtrC8 CSHA::Final()
{
	User::Panic(KSHA, KErrNotSupported);
	return (KNullDesC8());	//	Shut compiler up
}

void CSHA::RestoreState()
{
	User::Panic(KSHA, KErrNotSupported);
}
void CSHA::StoreState()
{
	User::Panic(KSHA, KErrNotSupported);
}
