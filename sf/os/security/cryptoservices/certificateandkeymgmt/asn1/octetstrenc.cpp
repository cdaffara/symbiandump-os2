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
* Implementation for class encoding octet strings in ASN1 DER
*
*/


#include <asn1enc.h>

EXPORT_C CASN1EncOctetString* CASN1EncOctetString::NewLC(const TDesC8& aStr)
	{
	CASN1EncOctetString* self = new (ELeave) CASN1EncOctetString();
	CleanupStack::PushL(self);
	self->ConstructL(aStr);
	return self;
	}

EXPORT_C CASN1EncOctetString* CASN1EncOctetString::NewL(const TDesC8& aStr)
	{
	CASN1EncOctetString* self = NewLC(aStr);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncOctetString::~CASN1EncOctetString()
	{
	delete iContents;
	}


CASN1EncOctetString::CASN1EncOctetString()
: CASN1EncPrimitive(EASN1OctetString)
	{
	}


void CASN1EncOctetString::ConstructL(const TDesC8& aStr)
	{
	iContents = aStr.AllocL();
	CASN1EncPrimitive::ConstructL();
	}


void CASN1EncOctetString::CalculateContentsLengthDER()
	{
	iContentsLengthDER = iContents->Length();
	}


void CASN1EncOctetString::WriteContentsDERL(TDes8& aBuf) const
	{
	aBuf.Copy(*iContents);
	}
