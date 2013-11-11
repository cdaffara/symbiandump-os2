/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <wtlsnames.h>
#include "wtlsdec.h"
#include <x500dn.h>
#include <utf.h>

_LIT(KWTLSStructuredTextFieldSeparator, ";");
_LIT(KWTLSStructuredTextAssignmentCharacter, "=");
const TInt KWTLSTextHeaderLength = 3;

EXPORT_C CWTLSName* CWTLSName::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSName::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSName* CWTLSName::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSName::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSName* CWTLSName::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSName* self = CWTLSName::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSName* CWTLSName::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSName* self = new(ELeave) CWTLSName;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CWTLSName* CWTLSName::NewL(const CWTLSName& aName)
	{
	CWTLSName* self = CWTLSName::NewLC(aName);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSName* CWTLSName::NewLC(const CWTLSName& aName)	
	{
	CWTLSName* self = new(ELeave) CWTLSName;
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}

CWTLSName::CWTLSName() 	
	{
	}

void CWTLSName::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	if ((aBinaryData.Length() - aPos) < 1)
		{
		User::Leave(KErrArgument);
		}
	iNameType = aBinaryData[aPos];
	aPos++;
	switch(iNameType)
		{
		case EWTLSText:
			{
			AllocTextDataL(aBinaryData, aPos);
			break;
			}
		case EWTLSX500DN:
			{
			AllocNameDataL(aBinaryData, aPos);
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

void CWTLSName::ConstructL(const CWTLSName& aName)
	{
	iNameType = aName.iNameType;
	iNameData = aName.iNameData->AllocL();
	}

EXPORT_C CWTLSName::~CWTLSName() 	
	{
	delete iNameData;
	}

void CWTLSName::AllocTextDataL(const TDesC8& aBinaryData, TInt& aPos)
	{
	if ((aBinaryData.Length() - aPos) < KWTLSTextHeaderLength)
		{
		User::Leave(KErrArgument);
		}
	TInt nameLength = (aBinaryData[aPos+2] + KWTLSTextHeaderLength);
	if ((aBinaryData.Length() ) < (aPos+nameLength))
		{
		User::Leave(KErrArgument);
		}
	iNameData = aBinaryData.Mid(aPos, nameLength).AllocL();
	aPos += nameLength;
	}

void CWTLSName::AllocNameDataL(const TDesC8& aBinaryData, TInt& aPos)
	{
	if ((aBinaryData.Length() - aPos) < 1)
		{
		User::Leave(KErrArgument);
		}
	TUint8 nameLength = aBinaryData[aPos];
	aPos++;
	if (aBinaryData.Length() - (aPos + nameLength) < 0)
		{
		User::Leave(KErrArgument);
		}
	iNameData = aBinaryData.Mid(aPos, nameLength).AllocL();
	aPos += nameLength;
	}

EXPORT_C TBool CWTLSName::ExactMatchL(const CWTLSName& aName) const	
	{		
//hmmm, in the absence of any matching rules, just match the bytes...
 	return (*(iNameData) == *(aName.iNameData));
	}

EXPORT_C TWTLSNameType CWTLSName::NameType() const
	{
	return iNameType;
	}

EXPORT_C TPtrC8 CWTLSName::NameData() const
	{
	return *iNameData;
	}

EXPORT_C HBufC* CWTLSName::DisplayNameL() const
	{
	switch (iNameType)
		{
		case EWTLSText:
			{
			CWTLSStructuredText* sText = NULL;
			TRAPD(err, sText = CWTLSStructuredText::NewL(*iNameData));
			if (err != KErrNone)
				{
				if (err != KErrArgument)
					{
					User::Leave(err);
					}
				else
					{
					CWTLSText* text = CWTLSText::NewLC(*iNameData);
					HBufC* res = text->Name().AllocL();
					CleanupStack::PopAndDestroy();//text
					return res;
					}
				}
			else
				{
				CleanupStack::PushL(sText);
				HBufC* res = sText->DisplayNameL();
				CleanupStack::PopAndDestroy();//sText
				return res;
				}
			}
		case EWTLSX500DN:
			{
			CX500DistinguishedName* dN = CX500DistinguishedName::NewLC(*iNameData);
			HBufC* res = dN->DisplayNameL();
			CleanupStack::PopAndDestroy();//dN
			return res;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			return NULL;//never gets to here...
			}
		}
	}

//************************************************************************//
//text name
EXPORT_C CWTLSText* CWTLSText::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSText::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSText* CWTLSText::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSText::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSText* CWTLSText::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSText* self = CWTLSText::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSText* CWTLSText::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSText* self = new(ELeave) CWTLSText;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CWTLSText::~CWTLSText() 	
	{
	delete iName;
	}

EXPORT_C TBool CWTLSText::ExactMatchL(const CWTLSText& aName) const
	{
//subtle difference between this byte-match and 
//CWTLSName::ExactMatchL(...) is that this should successfully match 2 names that 
//are the same which were encoded using different character sets...
	return (*iName == *(aName.iName));
	}

EXPORT_C TPtrC CWTLSText::Name() const
	{
	return *iName;
	}

EXPORT_C TWTLSCharSet CWTLSText::CharacterSet() const
	{
	return iCharacterSet;
	}

CWTLSText::CWTLSText()
	{
	}

void CWTLSText::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	if ((aBinaryData.Length() - aPos) < 3)
		{
		User::Leave(KErrArgument);
		}
	TWTLSDecUnsignedInteger decInt;
	iCharacterSet = decInt.DecodeShortL(aBinaryData, aPos, 2);
	TUint8 nameLength = aBinaryData[aPos];
	aPos++;
	if (aBinaryData.Length() < (aPos + nameLength))
		{
		User::Leave(KErrArgument);
		}
	TPtrC8 ptr = aBinaryData.Mid(aPos, nameLength);
	//only works for latin-1 here...
	iName = HBufC::NewL(ptr.Length());
	switch (iCharacterSet)
		{
		case KWTLSLatin1CharSet://=latin-1
			{
			TPtr pRes = iName->Des();
			pRes.Copy(ptr);
			aPos+=nameLength;
			break;
			}
		case KWTLSUTF8CharSet:
			{
			TPtr pRes = iName->Des();
			User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(pRes, ptr));	
			break;
			}
		default:
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

TWTLSStructuredTextField::TWTLSStructuredTextField(const TDesC& aType, const TDesC& aValue)
	:iType(aType), iValue(aValue)
	{
	}

EXPORT_C TPtrC TWTLSStructuredTextField::Type() const
	{
	return iType;
	}

EXPORT_C TPtrC TWTLSStructuredTextField::Value() const
	{
	return iValue;
	}

//Structured text class
EXPORT_C CWTLSStructuredText* CWTLSStructuredText::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSStructuredText::NewL(aBinaryData, pos);
	}

EXPORT_C CWTLSStructuredText* CWTLSStructuredText::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CWTLSStructuredText::NewLC(aBinaryData, pos);
	}

EXPORT_C CWTLSStructuredText* CWTLSStructuredText::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSStructuredText* self = CWTLSStructuredText::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CWTLSStructuredText* CWTLSStructuredText::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSStructuredText* self = new(ELeave) CWTLSStructuredText;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CWTLSStructuredText::~CWTLSStructuredText()
	{
	delete iFields;
	}

EXPORT_C HBufC* CWTLSStructuredText::DisplayNameL() const
	{
	if (iFields->Count() > 3)
		{
		return iFields->At(3).Value().AllocL();
		}
	else
		{
		return iFields->At(1).Value().AllocL();
		}
	}

	//accessors for defined fields
EXPORT_C TPtrC CWTLSStructuredText::ServiceName() const
	{
	//construction ensures iFields must have at least 3 elements
	return iFields->At(0).Value();
	}

EXPORT_C TPtrC CWTLSStructuredText::Organization() const
	{
	//construction ensures iFields must have at least 3 elements
	return iFields->At(1).Value();
	}

EXPORT_C TPtrC CWTLSStructuredText::Country() const
	{
	//construction ensures iFields must have at least 3 elements
	return iFields->At(2).Value();
	}

EXPORT_C TInt CWTLSStructuredText::Count() const
	{
	return iFields->Count();
	}

EXPORT_C const TWTLSStructuredTextField* CWTLSStructuredText::FieldByName(const TDesC& aType) const
	{
	TInt count = iFields->Count();
	for (TInt i = 0; i < count; i++)
		{
		TWTLSStructuredTextField* field = &(iFields->At(i));
		if (field->Type() == aType)
			{
			return field;
			}
		}
	return NULL;
	}

EXPORT_C const TWTLSStructuredTextField& CWTLSStructuredText::FieldByIndex(TInt aIndex) const
	{
	return iFields->At(aIndex);
	}

CWTLSStructuredText::CWTLSStructuredText()
	{
	}

void CWTLSStructuredText::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CWTLSText::ConstructL(aBinaryData, aPos);
	iFields = new(ELeave) CArrayFixFlat<TWTLSStructuredTextField> (4);
	TInt pos = 0;
	TInt end = iName->Length();
	AddFieldValueL(KWTLSServiceName, pos);
	AddFieldValueL(KWTLSOrganizationName, pos);
	AddFieldValueL(KWTLSCountryName, pos);
	if (pos < end)
		{
		AddFieldValueL(KWTLSCommonName, pos);
		}
	while (pos < end)
		{
		AddFieldL(pos);
		}
	}

void CWTLSStructuredText::AddFieldValueL(const TDesC& aFieldName, TInt& aPos)
	{
	TPtrC startOfData = iName->Right(iName->Length() - aPos);
	TPtrC value = GetFieldL(startOfData, aPos);
	TWTLSStructuredTextField field(aFieldName, value);
	iFields->AppendL(field);
	}

void CWTLSStructuredText::AddFieldL(TInt& aPos)
	{
	TPtrC startOfData = iName->Right(iName->Length() - aPos);
	TPtrC fieldData = GetFieldL(startOfData, aPos);
	TInt fieldLength = fieldData.Length();
	TInt endType = fieldData.FindF(KWTLSStructuredTextAssignmentCharacter);
	if ((endType == KErrNotFound) || ((endType+1) == fieldLength))
		{
		User::Leave(KErrArgument);
		}
	TPtrC type = fieldData.Left(endType);
	TPtrC value = fieldData.Right(	fieldLength - (endType+1)	);
	TWTLSStructuredTextField field(type, value);
	iFields->AppendL(field);
	}

TPtrC CWTLSStructuredText::GetFieldL(TDesC& aString, TInt& aPos)
	{
//aString = where we've got up to
//aPos = start of this chunk of name data
	TInt length = 0;
	TBool done = GetSubFieldL(aString, length);
	while (!done)
		{
		TPtrC remainder = aString.Right(aString.Length() - (length));
		TInt increment = 0;
		done = GetSubFieldL(remainder, increment);
		length += increment;
		}
	if (length == 0)
		{
		//disallow zero-length field values
		User::Leave(KErrArgument);
		}
	aPos += length;
	if (length < aString.Length())
		{
		//if we're not on the last field, skip the ';' character
		aPos++;
		}
	return aString.Left(length);
	}

TBool CWTLSStructuredText::GetSubFieldL(TDesC& aString, TInt& aPos)
	{
//fields in structured text are separated by ';'. If this character appears 
//in the field text it's represented by ';;'.  so, the string 'symbian;systems' would
//be encoded as 'symbian;;systems'. 

//starting at the start of aString, this function sets 'aPos' to the position after the 
//first occurrence of ';' in aString. If the character following ';' is also ';' it returns 
//EFalse (meaning, this sub-field isn't a whole field), otherwise it returns ETrue.

	aPos = aString.FindF(KWTLSStructuredTextFieldSeparator);	
	if (aPos == KErrNotFound)
		{
		aPos = aString.Length();
		}
	else
		{		
		//separator char mustn't be last char
		if (aPos == (aString.Length() -1))
			{
			User::Leave(KErrArgument);
			}
		else
			{
			//if the next char is ';', skip it and return EFalse
			TPtrC next = aString.Mid(aPos + 1, 1);
			if (next == KWTLSStructuredTextFieldSeparator)
				{
				aPos++;
				return EFalse;
				}
			//if it isn't, that's the end of the field
			}
		}
	return ETrue;
	}
