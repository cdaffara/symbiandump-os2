/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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


#include "NumberConversion.h"
#include "NumberConversionImp.h"

enum TNumConvPanic
	{
	ENumConvPanicInvalidDigitType = 1,
	};

#ifdef _DEBUG
LOCAL_D void Panic(TNumConvPanic aPanicCode)
//
// Panic the thread with NumberConversion as the category
//
	{
	_LIT(KPanicNumConv, "NumConv");
	User::Panic(KPanicNumConv, aPanicCode);
	}
#endif //_DEBUG

// TStandardDigitMatch

/**
Length required to format KMinTInt in the longest number format
@internalComponent
*/
const TInt KMaxLengthOfFormattedNumber = 11;

_LIT(KDefaultDigit,"0");
_LIT(KFormatIdentifier,"%");

TInt StandardDigitMatch::Match(const TDesC& aText, TInt& aLength,
	TDigitType& aDigitType, NumberConversion::TDigitMatchType aDigitMatchType)
	{
	TInt total = 0;

	aDigitType = EDigitTypeUnknown;
	TDigitType currentDigitType = EDigitTypeUnknown;
	aLength = 0;
	TInt textLength = aText.Length();
	while (aLength < textLength)
		{
		TChar currentChar = aText[aLength];
		currentDigitType = DigitType(currentChar);
		if (currentDigitType == EDigitTypeUnknown)
			{
			return total;
			}
		TInt digit = 0;
		TUint charValue = currentChar;
		digit = charValue - currentDigitType;
		if (aDigitType == EDigitTypeUnknown)
			{
			aDigitType = currentDigitType;
			}
		else
			{
			if (aDigitType != currentDigitType)
				{
				if (aDigitMatchType == NumberConversion::EMatchMultipleTypes)
					{
					aDigitType = EDigitTypeAllTypes;
					}
				else
					{
					return total;
					}
				}
			}

		total = (total * 10) + digit;
		aLength++;
		}
	return total;
	}

TInt StandardDigitMatch::LeadingZeros(const TDesC& aText)
	{
	//Function to find the number of leading zeros
	TInt textLength = aText.Length();
	TInt numOfLeadingZeros = 0;
	TInt currentLength = 0;
	TDigitType currentDigitType = EDigitTypeUnknown;
	TBool leadingZeros = ETrue;

	if (textLength == 1)
		return numOfLeadingZeros; // No leading number, if only one number.
	
	while(currentLength < textLength)
		{
		TChar currentChar = aText[currentLength];
		currentDigitType = DigitType(currentChar);
		if (currentDigitType == EDigitTypeUnknown)
			{
			return numOfLeadingZeros;
			}
		TInt digit = 0;
		TUint charValue = currentChar;
		digit = charValue - currentDigitType;

		if (digit!=0 && leadingZeros)
			leadingZeros=EFalse;
		if(leadingZeros)
			numOfLeadingZeros++;
		currentLength++;
		}
	return numOfLeadingZeros;

	}

TDigitType StandardDigitMatch::DigitType(TChar aChar)
	{
	if (aChar >= EDigitTypeWestern && aChar < EDigitTypeWestern+10)
		{
		return EDigitTypeWestern;
		}
	if (aChar >= EDigitTypeArabicIndic && aChar < EDigitTypeArabicIndic+10)
		{
		return EDigitTypeArabicIndic;
		}
	if (aChar >= EDigitTypeEasternArabicIndic
		&& aChar < EDigitTypeEasternArabicIndic+10)
		{
		return EDigitTypeEasternArabicIndic;
		}
	if (aChar >= EDigitTypeDevanagari && aChar < EDigitTypeDevanagari+10)
		{
		return EDigitTypeDevanagari;
		}
	if (aChar >= EDigitTypeBengali && aChar < EDigitTypeBengali+10)
		{
		return EDigitTypeBengali;
		}
	if (aChar >= EDigitTypeGurmukhi && aChar < EDigitTypeGurmukhi+10)
		{
		return EDigitTypeGurmukhi;
		}	
	if (aChar >= EDigitTypeGujarati && aChar < EDigitTypeGujarati+10)
		{
		return EDigitTypeGujarati;
		}
	if (aChar >= EDigitTypeOriya && aChar < EDigitTypeOriya+10)
		{
		return EDigitTypeOriya;
		}
	if (aChar >= EDigitTypeTamil && aChar < EDigitTypeTamil+10)
		{
		return EDigitTypeTamil;
		}
	if (aChar >= EDigitTypeTelugu && aChar < EDigitTypeTelugu+10)
		{
		return EDigitTypeTelugu;
		}
	if (aChar >= EDigitTypeKannada && aChar < EDigitTypeKannada+10)
		{
		return EDigitTypeKannada;
		}
	if (aChar >= EDigitTypeMalayalam && aChar < EDigitTypeMalayalam+10)
		{
		return EDigitTypeMalayalam;
		}
	if (aChar >= EDigitTypeThai && aChar < EDigitTypeThai+10)
		{
		return EDigitTypeThai;
		}
	if (aChar >= EDigitTypeLao && aChar < EDigitTypeLao+10)
		{
		return EDigitTypeLao;
		}
	if (aChar >= EDigitTypeTibetan && aChar < EDigitTypeTibetan+10)
		{
		return EDigitTypeTibetan;
		}
	if (aChar >= EDigitTypeTibetan && aChar < EDigitTypeTibetan+20)
		{
		return EDigitTypeTibetan;
		}
	if (aChar >= EDigitTypeMayanmar && aChar < EDigitTypeMayanmar+10)
		{
		return EDigitTypeMayanmar;
		}
	if (aChar >= EDigitTypeKhmer && aChar < EDigitTypeKhmer+10)
		{
		return EDigitTypeKhmer;
		}
	return EDigitTypeUnknown;
	}

void StandardDigitMatch::AppendFormat(TDes& aText, TInt aNumber,
	TDigitType aDigitType)
	{
	TInt base = aDigitType;

	if (base != EDigitTypeUnknown)
		{
		TInt length = aText.Length();
		TInt number = aNumber;
		TBuf<1> digitText(KDefaultDigit);
		do
			{
			digitText[0] = (TUint16)((number % 10) + base);
			aText.Insert(length, digitText);
			number /= 10;
			} while (number > 0);
		}
	}

TInt StandardDigitMatch::LengthOfFormattedNumber(TInt aNumber)
	{
	TInt length = 0;
	do
		{
		length++;
		aNumber /= 10;
		} while (aNumber > 0);
	return length;
	}




// NumberConversion

EXPORT_C TInt NumberConversion::ConvertFirstNumber(const TDesC& aText,
	TInt& aLength, TDigitType& aDigitType, TDigitMatchType aDigitMatchType)
/**
Converts the descriptor aText into an integer and returns it. Ignores any minus
signs: only non-negative numbers are returned.

@param aText Input text containing the integer to be converted.
@param aLength On exit aLength is set to the number of characters converted 
from the descriptor.
@param aDigitType Returns the digit type of the number converted. If no 
characters are matched, aDigitType will be set to EDigitTypeUnknown.
@param aDigitMatchType If aDigitMatchType is set to EMatchMultipleTypes, 
different number types in the descriptor are matched and returned as a single 
number. In this case, aDigitType will be set to EDigitTypeAllTypes.
@return The (non-negative) number found.
*/
	{
	return StandardDigitMatch::Match(aText, aLength, aDigitType, aDigitMatchType);
	}

EXPORT_C TInt NumberConversion::PositionAndTypeOfNextNumber(const TDesC& aText,
	TDigitType& aDigitType, TInt aStartFrom)
/**
Finds the position and type of the next number in the descriptor. If the number
has a preceeding minus sign, it will be ignored and the position of the first
digit will be returned.

@param aText Text to be searched.
@param aStartFrom First position within aText to be searched.
@param aDigitType aDigitType is set to the digit type matched. If the 
descriptor doesn't contain a recognisable digit, aDigitType is set to 
EDigitTypeUnknown.
@return The index of the first character.
*/
	{
	TInt index = aStartFrom;
	TInt length = aText.Length();
	while (index < length)
		{
		TChar currentChar(aText[index]);
		aDigitType = StandardDigitMatch::DigitType(currentChar);
		if (aDigitType != EDigitTypeUnknown)
			{
			return index;
			}
		index++;
		}
	return KErrNotFound;
	}

EXPORT_C void NumberConversion::FormatNumber(TDes& aText, TInt aNumber,
	TDigitType aDigitType)
/**
Converts a non-negative integer into localised text.
@param aText Output for the converted number. aText must be long enough to 
accommodate	the text or the descriptor will panic. Negative numbers are not 
supported.
@param aNumber The number to be converted.
@param aDigitType The type of digit to render the number in.
@pre NumberConversion::LengthOfFormattedNumber(aNumber, aDigitType) <= aText.MaxLength() && 0 <= aNumber
*/
	{
	aText.Zero();
	AppendFormatNumber(aText, aNumber, aDigitType);
	}

EXPORT_C void NumberConversion::FormatDigit(TDes& aText, TInt aNumber, TInt aLeadingZero,
	TDigitType aDigitType)
/**
Converts a non-negative integer into localised text.

@param aText Output for the converted number. aText must be long enough to 
accommodate	the text or the descriptor will panic. Negative numbers are not 
supported.
@param aNumber The number to be converted.
@param aDigitType The type of digit to render the number in.
@param aLeadingZero The number of zeros that appear before the number to be 
converted to localised text.
@pre NumberConversion::LengthOfFormattedNumber(aNumber, aDigitType) <= aText.MaxLength() && 0 <= aNumber
*/
	{
	aText.Zero();
	AppendFormatNumber(aText, aNumber, aDigitType);
	
	// Insert the zeros 
	if(aLeadingZero)
		{
		TChar zero = aDigitType;
		TBuf<1> zeroBuf;
		zeroBuf.Append(zero);
		for (TInt i=0; i<aLeadingZero; i++)
			{
			aText.Insert(0,zeroBuf);
			}
		}
	}

EXPORT_C void NumberConversion::AppendFormatNumber(TDes& aText, TInt aNumber,
	TDigitType aDigitType)
/**
Converts a non-negative integer into localised text, appending the result to a
descriptor.

@param aText Output for the converted number. aText must have enough free space 
after its current length to accommodate the text or the descriptor will panic.
Negative numbers are not supported.
@param aNumber The number to be converted.
@param aDigitType The type of digit to render the number in.
@pre NumberConversion::LengthOfFormattedNumber(aNumber, aDigitType) <= aText.MaxLength() - aText.Length() && 0 <= aNumber
*/
	{
	StandardDigitMatch::AppendFormat(aText, aNumber, aDigitType);
	}

EXPORT_C void NumberConversion::ConvertDigits(TDes& aText,
	TDigitType aDigitType)
/**
Converts all of the digits in the descriptor aText into the format specified in
aDigitType.

@param aText The text that is to have its digits converted.
@param aDigitType The digit type to be converted to.
@pre NumberConversion::LengthOfConvertedText(aText, aDigitType) <= aText.MaxLength()
@post All digits in the string will either conform to one of the constants
defined in enum TDigitType or will match the digit type
supplied in aDigitType.
*/
	{	
	TInt charValue;
	for (TInt i=0; i < aText.Length(); i++)
		{		
		if ( (charValue = static_cast<TChar>(aText[i]).GetNumericValue()) >= 0)	
			{					
			TBuf<1> convertedNumber;
			FormatNumber(convertedNumber, charValue, aDigitType);
			aText.Delete(i, 1);
			aText.Insert(i, convertedNumber);			
			}
		}
  }

EXPORT_C TInt NumberConversion::LengthOfFormattedNumber(TInt aNumber,
	TDigitType /*aDigitType*/)
/**
Returns the number of characters required to format aNumber into text.
@param aNumber The number to be converted.
@param aDigitType The format for the number.
@return The length of descriptor required to render the number as text.
*/
	{
	return StandardDigitMatch::LengthOfFormattedNumber(aNumber);
	}

EXPORT_C TInt NumberConversion::LengthOfConvertedText(const TDesC& aText,
	TDigitType aDigitType)
/**
Returns the length of the descriptor required to hold text with its digits
converted.

@param aText Input text for the conversion.
@param aDigitType The type of digit that will be used for the conversion.
@return	The length of descriptor that would be required to convert the digits 
in aText into the type specified by aDigitType.
*/
	{
	TDigitType digitType = EDigitTypeUnknown;
	TInt position = PositionAndTypeOfNextNumber(aText, digitType);
	TInt total = aText.Length();
	while (digitType != EDigitTypeUnknown)
		{
		// Convert this number into a different format
		TInt length = 0;
		TPtrC matchText = aText.Mid(position);
		TInt number = ConvertFirstNumber(matchText, length, digitType);
		total -= length;
		total += LengthOfFormattedNumber(number, aDigitType);
		position = PositionAndTypeOfNextNumber(aText, digitType,
			position + length);
		}
	return total;
	}

EXPORT_C void NumberConversion::Format(TDigitType aDigitType,
	TRefByValue<TDes> aFmt,...)
/**
Formats the descriptor. Format specifiers are converted to values passed in the
variable argument list. The following format specifiers are supported:

%d - Interprets the argument as a TInt and formats it using the aDigitType
format. Negative numbers are not supported.

%S - Interprets the argument as a pointer to a TDesC and inserts it into the
descriptor.

@param aDigitType The digit type for all %d directives.
*/
	{
	VA_LIST argument_list;
	VA_START(argument_list, aFmt);

	TDes& format = aFmt;
	TInt match = KErrNotFound;
	while ((match = format.Find(KFormatIdentifier)) != KErrNotFound)
		{
		TChar formatIdentifier = format[match+1];
		switch (formatIdentifier)
			{
			case 'd':
				{
				format.Delete(match,2);
				TInt number = VA_ARG(argument_list, int);
				TBuf<KMaxLengthOfFormattedNumber> convertedNumber;
				FormatNumber(convertedNumber, number, aDigitType);
				format.Insert(match, convertedNumber);
				}
				break;
			case 'S':
				{
				format.Delete(match,2);
				TDesC* des = VA_ARG(argument_list, TDesC*);
				format.Insert(match, *des);
				}
				break;
			default:
				// Remove format identifier
				format.Delete(match,1);
			};
		}

	VA_END(argument_list);
	}

EXPORT_C TChar NumberConversion::ConvertDigit(TChar& aDigit, TDigitType aDigitType)
/**
converts aDigit (which could be arabic, western digit etc) into the form 
denoted  by aDigitType.

@param TChar& aDigit - contains the digit to be converted.
@param TDigitType aDigitType - aDigit type: western, arabic, thai, ...
@return Digit into the form, denoted by aDigitType.
*/
	{
	__ASSERT_DEBUG(EDigitTypeUnknown != aDigitType && EDigitTypeAllTypes != aDigitType, 
					Panic(ENumConvPanicInvalidDigitType));
	TBuf<1> buf;
	buf.Append(aDigit);
	NumberConversion::ConvertDigits(buf, aDigitType);
	return buf[0];	
	}

