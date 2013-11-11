/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation for the explicit tag wrapping class.  See header for usage.
*
*/


#include "panic.h"

#include <asn1enc.h>

EXPORT_C CASN1EncExplicitTag* CASN1EncExplicitTag::NewLC(CASN1EncBase* aEncoder,
														 TTagType aType,
														 TASN1Class aClass)
	{
	__ASSERT_ALWAYS(aEncoder, Panic(KErrArgument));
	CleanupStack::PushL(aEncoder); // We are responsibe for this object
	CASN1EncExplicitTag* self = new (ELeave) CASN1EncExplicitTag(aEncoder, aType, aClass);
	CleanupStack::Pop(aEncoder); // We now have ownership through the 'self' object
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CASN1EncExplicitTag* CASN1EncExplicitTag::NewL(CASN1EncBase* aEncoder,
														 TTagType aType,
														 TASN1Class aClass)
	{
	CASN1EncExplicitTag* self = NewLC(aEncoder, aType, aClass);
	CleanupStack::Pop(self);
	return self;
	}


CASN1EncExplicitTag::CASN1EncExplicitTag(CASN1EncBase* aEncoder, const TTagType aType,
										 const TASN1Class aClass)
: CASN1EncConstructed(aType, aClass), iEncoder(aEncoder)
	{
	iEncoder->SetParent(this);
	}


EXPORT_C CASN1EncExplicitTag::~CASN1EncExplicitTag()
	{
	delete iEncoder;
	}


TUint CASN1EncExplicitTag::NumChildren() const
	{
	return 1;
	}


const CASN1EncBase& CASN1EncExplicitTag::Child(const TUint aIndex) const
	{
	__ASSERT_ALWAYS(aIndex == 0, Panic(KErrArgument));

	return *iEncoder;
	}

