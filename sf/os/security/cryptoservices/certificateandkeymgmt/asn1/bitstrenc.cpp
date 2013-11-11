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
* This file contains the implementation for class encoding bit strings in ASN1 DER.
*
*/


/**
 @file
*/

#include <asn1enc.h>

EXPORT_C CASN1EncBitString* CASN1EncBitString::NewLC(const TDesC8& aBitStr)
	{
	CASN1EncBitString* self = new (ELeave) CASN1EncBitString();
	CleanupStack::PushL(self);
	self->ConstructL(aBitStr);
	return self;
	}

EXPORT_C CASN1EncBitString* CASN1EncBitString::NewL(const TDesC8& aBitStr)
	{
	CASN1EncBitString* self = NewLC(aBitStr);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncBitString* CASN1EncBitString::NewLC(const TDesC8& aBitStr, 
													 TUint aLengthBits)
	{
	CASN1EncBitString* self = new (ELeave) CASN1EncBitString();
	CleanupStack::PushL(self);
	self->ConstructL(aBitStr, aLengthBits);
	return self;
	}

EXPORT_C CASN1EncBitString* CASN1EncBitString::NewL(const TDesC8& aBitStr, 
													TUint aLengthBits)
	{
	CASN1EncBitString* self = NewLC(aBitStr, aLengthBits);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncBitString* CASN1EncBitString::NewL(const CASN1EncBase& aEncObj)
	{
	CASN1EncBitString* self = NewLC(aEncObj);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CASN1EncBitString* CASN1EncBitString::NewLC(const CASN1EncBase& aEncObj)
	{
	CASN1EncBitString* self = new (ELeave) CASN1EncBitString();
	CleanupStack::PushL(self);
	self->ConstructL(aEncObj);
	return self;
	}

EXPORT_C CASN1EncBitString::~CASN1EncBitString()
	{
	delete iContents;
	}

CASN1EncBitString::CASN1EncBitString()
: CASN1EncPrimitive(EASN1BitString),
  iPadding(0)
	{
	}

void CASN1EncBitString::ConstructL(const TDesC8& aBitStr)
	{
	iContents = aBitStr.AllocL();
	CASN1EncPrimitive::ConstructL();
	}

void CASN1EncBitString::ConstructL(const TDesC8& aBitStr, TUint aLengthBits)
	{
	iContents = aBitStr.AllocL();
	CASN1EncPrimitive::ConstructL();
	TUint totalBits = aBitStr.Length() * 8;
	__ASSERT_ALWAYS(aLengthBits <= totalBits, User::Leave(KErrArgument));
	iPadding = (TUint8)(totalBits - aLengthBits);
	if(iPadding > 7)
		User::Leave(KErrArgument);
	}

/**
 * @internalTechnology
 * Constructs bit string from ASN.1 encoding object.
 * @param aEncObj ASN.1 encoding object to wrap in bit string.
 * @note First produces raw DER encoding from the object, then creates 
 *     a bit string using other construct function.
 */
void CASN1EncBitString::ConstructL(const CASN1EncBase& aEncObj)
	{
	// produce raw DER encoding from the created ASN.1 encoding
	TUint len = aEncObj.LengthDER();
	HBufC8* intDer = HBufC8::NewMaxLC(len);
	TPtr8 ptrDer = intDer->Des();
	TUint pos = 0;
	aEncObj.WriteDERL(ptrDer, pos);
	// wrap the produced DER encoding into a bit string
	ConstructL(*intDer);
	// cleanup
	CleanupStack::PopAndDestroy(intDer);
	}

/** 
 * @internalTechnology 
 * Calculates length of DER-encoded bit string contents. For non empty
 * bit strings this differs from octet string in the extra leading byte 
 * containing the number of unused bits in the last octet.
 */
void CASN1EncBitString::CalculateContentsLengthDER()
	{
	iContentsLengthDER = iContents->Length();
	if (iContentsLengthDER > 0)
		{
		iContentsLengthDER++;
		}
	}

/**
 * @internalTechnology 
 * Writes DER-encoded bit string contents to aBuf. Prepends 
 * the actual bit string octets with extra octet containing 
 * number of unused bits in the last octet. Before writing, 
 * converts contents of the bit string to big-endian form.
 * @param aBuf Buffer to write to; must be long enough; 
 *     see #CalculateContentsLengthDER method.
 */
void CASN1EncBitString::WriteContentsDERL(TDes8& aBuf) const
	{
	if (iContents->Length() > 0)
		{
		aBuf[0] = iPadding;
		TUint len = iContents->Length();
		// We do not need to swap bits, as it is already done.
		aBuf.Replace(1, len, *iContents);		
		}
	else
		{
		// no padding octet for the empty bit string
		aBuf.SetLength(0);
		}
	}
