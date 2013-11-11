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
* Code for the sequence-encoding class
*
*/


#include "panic.h"

#include <asn1enc.h>

const TInt KArrayGranularity = 5;


EXPORT_C CASN1EncSequence* CASN1EncSequence::NewLC()
	{
	CASN1EncSequence* self = new (ELeave) CASN1EncSequence();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CASN1EncSequence* CASN1EncSequence::NewL()
	{
	CASN1EncSequence* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncSequence::~CASN1EncSequence()
	{
	iChildren.ResetAndDestroy();
	}


CASN1EncSequence::CASN1EncSequence()
: CASN1EncContainer(EASN1Sequence), iChildren(KArrayGranularity)
	{
	}


void CASN1EncSequence::AddChildIntL(const CASN1EncBase* aChild)
	{
	User::LeaveIfError(iChildren.Append(aChild));
	}


TUint CASN1EncSequence::NumChildren() const
	{
	return iChildren.Count();
	}


const CASN1EncBase& CASN1EncSequence::Child(const TUint aIndex) const
	{
	return *iChildren[aIndex];
	}

EXPORT_C CASN1EncSet* CASN1EncSet::NewLC()
	{
	CASN1EncSet* self = new (ELeave) CASN1EncSet();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CASN1EncSet* CASN1EncSet::NewL()
	{
	CASN1EncSet* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncSet::~CASN1EncSet()
	{
	iChildren.ResetAndDestroy();
	}


CASN1EncSet::CASN1EncSet()
: CASN1EncContainer(EASN1Set), iChildren(KArrayGranularity)
	{
	}


void CASN1EncSet::AddChildIntL(const CASN1EncBase* aChild)
	{
	User::LeaveIfError(iChildren.Append(aChild));
	}


TUint CASN1EncSet::NumChildren() const
	{
	return iChildren.Count();
	}


const CASN1EncBase& CASN1EncSet::Child(const TUint aIndex) const
	{
	return *iChildren[aIndex];
	}
