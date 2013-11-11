/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <x520ava.h>
#include "X500dec.h"
#include <asn1enc.h>
#include <asn1dec.h>


EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewL(const CX520AttributeTypeAndValue& aPair)
	{
	CX520AttributeTypeAndValue* self = CX520AttributeTypeAndValue::NewLC(aPair);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewLC(const CX520AttributeTypeAndValue& aPair)
	{
	CX520AttributeTypeAndValue* self = new(ELeave) CX520AttributeTypeAndValue;
	CleanupStack::PushL(self);
	self->ConstructL(aPair);
	return self;
	}

EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX520AttributeTypeAndValue::NewL(aBinaryData, pos);
	}

EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX520AttributeTypeAndValue::NewLC(aBinaryData, pos);
	}

EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX520AttributeTypeAndValue* self = CX520AttributeTypeAndValue::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX520AttributeTypeAndValue* self = new(ELeave) CX520AttributeTypeAndValue;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewL(RReadStream& aStream)
	{
	CX520AttributeTypeAndValue* self = CX520AttributeTypeAndValue::NewLC(aStream);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewLC(RReadStream& aStream)
	{
	CX520AttributeTypeAndValue* self = new(ELeave) CX520AttributeTypeAndValue;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}

/** Allocates and initializes a new attribute-value pair object from 
	type and value.
	@param aType Type of the value (see TAttributeType enum).
	@param aValue String value of the attribute.
	@return Pointer to a newly allocated and initialized attribute-value pair.
	@see CX520AttributeTypeAndValue::NewLC */
EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewL(TAttributeType aType, const TDesC8& aValue)
	{
	CX520AttributeTypeAndValue* self = CX520AttributeTypeAndValue::NewLC(aType, aValue);
	CleanupStack::Pop(); // self
	return self;
	}

/** Allocates and initializes a new attribute-value pair object from 
	type and value. Pushes the newly allocated object onto the 
	cleanup stack.
	@param aType Type of the value (see TAttributeType enum).
	@param aValue String value of the attribute.
	@return Pointer to a newly allocated and initialized attribute-value pair.
	@see CX520AttributeTypeAndValue::NewL */
EXPORT_C CX520AttributeTypeAndValue* CX520AttributeTypeAndValue::NewLC(
											TAttributeType aType, 
											const TDesC8& aValue)
	{
	CX520AttributeTypeAndValue* self = new(ELeave) CX520AttributeTypeAndValue;
	CleanupStack::PushL(self);
	self->ConstructL(aType, aValue);
	return self;
	}

CX520AttributeTypeAndValue::CX520AttributeTypeAndValue()
	:iType(NULL), iValue(NULL)
	{
	}

void CX520AttributeTypeAndValue::ConstructL(const CX520AttributeTypeAndValue& aPair)
	{
	iType = aPair.iType->AllocL();
	iValue = aPair.iValue->AllocL();
	}

void CX520AttributeTypeAndValue::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric dec(aBinaryData.Right(aBinaryData.Length() - aPos));
	dec.InitL();
	TInt end = aPos + dec.LengthDER();
	aPos += dec.LengthDERHeader();

	//first element must be the id
	TASN1DecObjectIdentifier encOID;
	iType = encOID.DecodeDERL(aBinaryData, aPos);
	//second is the data
	TASN1DecGeneric second(aBinaryData.Right(aBinaryData.Length() - aPos));
	second.InitL();

	iValue = second.Encoding().AllocL();;
	aPos += second.LengthDER();

	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}
	}

void CX520AttributeTypeAndValue::ConstructL(TAttributeType aType, const TDesC8& aValue)
	{
	// iType is string representation of OID corresponding to the aType.
	TPtrC ptr;
	TInt maxlen = 64; // a reasonable default
	TTagType type = EASN1PrintableString; // the default for all except email, unstructured name and domain component
	switch(aType)
		{
		case ECommonName: 
			ptr.Set(KX520CommonName); 
			maxlen = KX520MaxCNLength;
			break;
		case ELocalityName:
			ptr.Set(KX520LocalityName);
			maxlen = KX520MaxLLength;
			break;
		case EStateOrProvinceName:
			ptr.Set(KX520StateOrProvinceName);
			maxlen = KX520MaxSOPLength;
			break;
		case EPostalCode:
			ptr.Set(KX520PostalCode);
			maxlen = KX520MaxPostalCodeLength;
			break;
		case EOrganizationName:
			ptr.Set(KX520OrganizationName);
			maxlen = KX520MaxOLength;
			break;
		case EOrganizationalUnitName:
			ptr.Set(KX520OrganizationalUnitName);
			maxlen = KX520MaxOULength;
			break;
		case ETitle:
			ptr.Set(KX520Title);
			maxlen = KX520MaxTLength;
			break;
		case EDNQualifier:
			ptr.Set(KX520DNQualifier);
			maxlen = 64; // no information was found on this one, so set to a safe minimum
			break;
		case ECountryName:
			ptr.Set(KX520CountryName);
			maxlen = KX520MaxCNLength;
			break;
		case EGivenName:
			ptr.Set(KX520GivenName);
			maxlen = KX520MaxGNLength;
			break;
		case ESurname:
			ptr.Set(KX520Surname);
			maxlen = KX520MaxSLength;
			break;
		case EInitials:
			ptr.Set(KX520Initials);
			maxlen = KX520MaxILength;
			break;
		case EGenerationQualifier:
			ptr.Set(KX520GenerationQualifier);
			maxlen = KX520MaxGQLength;
			break;
		case EPKCS9EmailAddress:
			ptr.Set(KPKCS9EmailAddress);
			maxlen = KPKCS9MaxEmailAddressLength;
			type = EASN1IA5String;
			break;
		case ESerialNumber:
			ptr.Set(KX520SerialNumber);
			maxlen = KX520MaxSNLength;
			break;
		case ERFC2247DomainComponent:
			ptr.Set(KRFC2247DomainComponent);
			maxlen = KRFC2247MaxDomainComponentLength;
			type = EASN1IA5String;
			break;
		case ERFC2256Street:
			ptr.Set(KRFC2256Street);
			maxlen = KRFC2256StreetLength;
			break;
		case EPKCS9UnstructuredName:
			{
			ptr.Set(KPKCS9UnstructuredName);
			maxlen = KPKCS9MaxUnstructuredNameLength;
			// Determine the encoded value. It could be a IA5String or a UTF8String
			TASN1DecGeneric decoderGeneric(aValue);
			decoderGeneric.InitL();
			type = decoderGeneric.Tag();	
			break;	
			}
		case EX520Description:
			{
			ptr.Set(KX520Description);
			maxlen = KX520MaxDescriptionLength;
			break;
			}
		default:
			User::Leave(KErrArgument);
		}
	// Verify if the passed length is within limits
	if(aValue.Length() > maxlen)
		User::Leave(KErrArgument);

	// Allocate OID string for iType
	iType = ptr.AllocL();

	// iValue must be stored in ASN.1-encoded form
	CASN1EncOctetString* enc = CASN1EncOctetString::NewLC(aValue);
	enc->SetTag(type, EUniversal);
	TUint len = enc->LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(len);
	TUint pos = 0;
	TPtr8 bufptr(buf->Des());
	enc->WriteDERL(bufptr, pos);
	iValue = bufptr.AllocL();
	CleanupStack::PopAndDestroy(2);
	}

EXPORT_C const TPtrC CX520AttributeTypeAndValue::Type() const
	{
	return iType->Des();
	}

EXPORT_C const TPtrC8 CX520AttributeTypeAndValue::EncodedValue() const
	{
	return iValue->Des();
	}

EXPORT_C HBufC* CX520AttributeTypeAndValue::ValueL() const
	{
	if (iType->Des() == KX520CountryName)
		{
		TASN1DecPrintableString encPString;
		TInt pos = 0;
		HBufC* res = encPString.DecodeDERL(iValue->Des(), pos);
		CleanupStack::PushL(res);
		if (res->Length() > KX520MaxCLength)
			{
			User::Leave(KErrArgument);
			}
		CleanupStack::Pop();
		return res;
		}
	if (iType->Des() == KX520DNQualifier)
		{
		TInt pos = 0;
		TASN1DecPrintableString encPString;
		HBufC* res = encPString.DecodeDERL(iValue->Des(), pos);
		return res;
		}
	if (iType->Des() == KPKCS9EmailAddress)
		{
		TASN1DecIA5String encIA5String;
		TInt pos = 0;
		HBufC* res = encIA5String.DecodeDERL(iValue->Des(), pos);
		CleanupStack::PushL(res);
		if (res->Length() > KPKCS9MaxEmailAddressLength)
			{
			User::Leave(KErrArgument);
			}
		CleanupStack::Pop();
		return res;
		}
	if (iType->Des() == KRFC2247DomainComponent)
		{
		TASN1DecIA5String encIA5String;
		TInt pos = 0;
		HBufC* res = encIA5String.DecodeDERL(iValue->Des(), pos);
		CleanupStack::PushL(res);
		if (res->Length() > KRFC2247MaxDomainComponentLength)
			{
			User::Leave(KErrArgument);
			}
		CleanupStack::Pop();
		return res;
		}
	if (iType->Des() == KX520SerialNumber)
		{
		TASN1DecPrintableString encPString;
		TInt pos = 0;
		HBufC* res = encPString.DecodeDERL(iValue->Des(), pos);
		CleanupStack::PushL(res);
		if (res->Length() > KX520MaxSNLength)
			{
			User::Leave(KErrArgument);
			}
		CleanupStack::Pop();
		return res;
		}
	TInt maxLength = 0;
	if (iType->Des() == KPKCS9UnstructuredName)
		{
		TASN1DecGeneric decoderGeneric(iValue->Des());
		decoderGeneric.InitL();
		// The encoded value should be a IA5String
		if (decoderGeneric.Tag() == EASN1IA5String)
			{
			TASN1DecIA5String encIA5String;
			TInt pos = 0;
			HBufC* res = encIA5String.DecodeDERL(iValue->Des(), pos);
			CleanupStack::PushL(res);
			if (res->Length() > KPKCS9MaxUnstructuredNameLength)
				{
				User::Leave(KErrArgument);
				}
			CleanupStack::Pop();
			return res;					
			}
		// But it could also be a UTF8String to support internationalization issues
		else
			{
			maxLength = KPKCS9MaxUnstructuredNameLength;	
			}
		}	
	if (iType->Des() == KX520OrganizationName)
		{
		maxLength = KX520MaxOLength;
		}
	if (iType->Des() == KX520OrganizationalUnitName)
		{
		maxLength = KX520MaxOULength;
		}
	if (iType->Des() == KX520LocalityName)
		{
		maxLength = KX520MaxLLength;
		}
	if (iType->Des() == KX520StateOrProvinceName)
		{
		maxLength = KX520MaxSOPLength;
		}
	if (iType->Des() == KX520Title)
		{
		maxLength = KX520MaxTLength;
		}
	if (iType->Des() == KX520CommonName)
		{
		maxLength = KX520MaxCNLength;
		}
	if (iType->Des() == KX520Surname)
		{
		maxLength = KX520MaxSLength;
		}
	if (iType->Des() == KX520GivenName)
		{
		maxLength = KX520MaxGNLength;
		}
	if (iType->Des() == KX520Initials)
		{
		maxLength = KX520MaxILength;
		}
	if (iType->Des() == KX520GenerationQualifier)
		{
		maxLength = KX520MaxGQLength;
		}
	if (iType->Des() == KX520PostalCode)
		{
		maxLength = KX520MaxPostalCodeLength;
		}
	if (iType->Des() == KRFC2256Street)
		{
		maxLength = KRFC2256StreetLength;
		}
	if (iType->Des() == KX520Description)
		{
		maxLength = KX520MaxDescriptionLength;
		}
	if (maxLength == 0)
		{
		User::Leave(KErrNotSupported);
		}
	TASN1DecX500DirectoryString encDString;
	TInt pos = 0;
	HBufC* res = encDString.DecodeDERL(iValue->Des(), pos, maxLength);
	return res;
	}

TBool CX520AttributeTypeAndValue::IsCaseInSensitiveL(const TDesC8& aSource) const
	{
	TPtr attribute = iType->Des();
	TBool caseInsensitiveAttr = (attribute == KPKCS9EmailAddress || attribute == KPKCS9UnstructuredName || attribute == KX520Description);
	TASN1DecGeneric gen(aSource);
	gen.InitL();
	return ((gen.Tag() == EASN1PrintableString) || caseInsensitiveAttr);
	}


EXPORT_C CASN1EncSequence* CX520AttributeTypeAndValue::EncodeASN1LC() const
	{
	CASN1EncSequence *seq = CASN1EncSequence::NewLC();
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(Type());
	seq->AddAndPopChildL(oid);

	// The current ASN.1 base encoding class assumes that ASN.1 type, 
	// length, and contents are stored and can be written to a buffer
	// separately. Therefore it is difficult, if not impossible, to 
	// store raw ASN.1 encoding data in a tree of ASN.1 encoding 
	// objects. That is why we are forced first to decode the raw value, 
	// and then re-encode it so that we know what type and length it 
	// has.
	TASN1DecGeneric decoderGeneric(EncodedValue());
	decoderGeneric.InitL();
	TASN1DecOctetString decoderOctetString;
	HBufC8* valBuf = decoderOctetString.DecodeDERL(decoderGeneric);
	CleanupStack::PushL(valBuf);
	CASN1EncOctetString* val = CASN1EncOctetString::NewLC(*valBuf);
	val->SetTag(decoderGeneric.Tag(), decoderGeneric.Class());
	seq->AddAndPopChildL(val);
	CleanupStack::PopAndDestroy(valBuf);
	return seq;
	}

EXPORT_C CASN1EncSequence* CX520AttributeTypeAndValue::EncodeASN1L() const
	{
	CASN1EncSequence *seq = EncodeASN1LC();
	CleanupStack::Pop(seq);
	return seq;
	}

EXPORT_C CX520AttributeTypeAndValue::~CX520AttributeTypeAndValue()
	{
	delete iType;
	delete iValue;
	}

EXPORT_C TBool CX520AttributeTypeAndValue::ExactMatchL(const CX520AttributeTypeAndValue& aElement) const
	{
	TBool res = EFalse;
	if (*(iType) != *(aElement.iType))
		{
		return res;
		}
	HBufC* lhs = this->ValueL();
	CleanupStack::PushL(lhs);
	HBufC* rhs = aElement.ValueL();

	TPtr plhs = lhs->Des();
	TPtr prhs = rhs->Des();
	plhs.TrimAll();
	prhs.TrimAll();


	// DEF124902: Certificate name matching done in accordance to RFC3280
	// RFC3280: Printable String and Email address(of value type 'IA5String') will 
 	// be compared case-insensitively.  
 	
    if (IsCaseInSensitiveL(iValue->Des()))
 	    {
 	     //case insensitive comparison for Printable String and IA5String (EmailAdress only).
 	     res = (plhs.CompareF(prhs) == 0);
 	    }
    else
	    {
	     // case-sensitive comparison for strings other than printable string 
	     // Exception: This may include IA5Stings other than 'EmailAddress' attiribute types.
 	     res = (plhs.Compare(prhs) == 0);
	    }
	CleanupStack::PopAndDestroy();
	delete rhs;
	return res; 
	}

void CX520AttributeTypeAndValue::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iType->Des().Length());
	aStream << *iType;
	aStream.WriteInt32L(iValue->Des().Length());
	aStream << *iValue;
	}

void CX520AttributeTypeAndValue::InternalizeL(RReadStream& aStream)
	{
	TInt maxlen;
	maxlen = aStream.ReadInt32L();
	iType = HBufC::NewL(aStream,maxlen);
	maxlen = aStream.ReadInt32L();
	iValue = HBufC8::NewL(aStream,maxlen);
	}
