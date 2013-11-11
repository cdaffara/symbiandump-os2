/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32def.h>
#include <asn1enc.h>
#include <asn1dec.h>

EXPORT_C CASN1EncEncoding::~CASN1EncEncoding()
	{
	delete iContents;
	}

// Set arbitrary initial values for tag type and class because they will be 
// inited in ConstructL().
EXPORT_C CASN1EncEncoding::CASN1EncEncoding() : 
	CASN1EncBase(EASN1EOC, EUniversal)
	{
	}

EXPORT_C CASN1EncEncoding* CASN1EncEncoding::NewLC(const TDesC8& aEncoding)
	{
	CASN1EncEncoding* self = new (ELeave) CASN1EncEncoding;
	CleanupStack::PushL(self);
	self->ConstructL(aEncoding);
	return self;
	}

EXPORT_C CASN1EncEncoding* CASN1EncEncoding::NewL(const TDesC8& aEncoding)
	{
	CASN1EncEncoding* self = NewLC(aEncoding);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncEncoding* CASN1EncEncoding::NewLC(const TDesC8& aEncoding, TTagType aType, TASN1Class aClass)
	{
	CASN1EncEncoding* self = new (ELeave) CASN1EncEncoding;
	CleanupStack::PushL(self);
	self->ConstructL(aEncoding, aType, aClass);
	return self;		
	}

void CASN1EncEncoding::ConstructL(const TDesC8& aEncoding)
	{
	// Use decoder to get information about the outer wrapper of the passed 
	// encoding.
	TASN1DecGeneric decoder(aEncoding);
	decoder.InitL();
	iClass = decoder.Class();
	iTag = decoder.Tag();
	// Set this information so that the base class knows how to write 
	// DER encoding.
	SetTag(iTag, iClass);
	// Copy just the contents of the passed encoding.
	iContents = decoder.GetContentDER().AllocL();
	// Save this for base class writing functions.
	iContentsLengthDER = iContents->Length();
	// This is base class' method which initializes length of length 
	// encoding for proper DER writing.
	CalculateLengthLengthDER();
	}

void CASN1EncEncoding::ConstructL(const TDesC8& aEncoding, TTagType aType, TASN1Class aClass)
	{
	// Use decoder to get information about the outer wrapper of the passed 
	// encoding.
	TASN1DecGeneric decoder(aEncoding);
	decoder.InitL();
	iClass = aClass;
	iTag = aType;
	// Set this information so that the base class knows how to write 
	// DER encoding.
	SetTag(aType, aClass);
	// Copy just the contents of the passed encoding.
	iContents = decoder.GetContentDER().AllocL();
	// Save this for base class writing functions.
	iContentsLengthDER = iContents->Length();
	// This is base class' method which initializes length of length 
	// encoding for proper DER writing.
	CalculateLengthLengthDER();
	}


// This method is not called but is necessary because it overrides a 
// pure virtual function of the base class. The variable is properly 
// initialized in ConstructL().
void CASN1EncEncoding::CalculateContentsLengthDER()
	{
	iContentsLengthDER = iContents->Length();
	}

TBool CASN1EncEncoding::IsConstructed() const
	{
	return ETrue;
	}

// When this method is called by the base class write helper, the tag 
// and length are already written.
void CASN1EncEncoding::WriteContentsDERL(TDes8& aBuf) const
	{
	aBuf.Copy(*iContents);
	}
