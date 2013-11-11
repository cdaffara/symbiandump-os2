// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "inetprottextutils.h"

#include "inetprottextutilslocal.h"
#include <uriutilscommon.h>

// Panic category
//
#ifdef _DEBUG
	_LIT(KTextUtilsPanicCategory, "IPTXTUTL"); 
#endif

const TUint KZeroChar		= '0';
const TUint KAChar			= 'A';
const TUint	KQuote			= '"';
const TUint KMinusSign		= '-';
const TInt KMaxTIntDecimalDigits = 11; // KMaxTInt  => "-2147483648" 
const TInt KMaxTIntDividedByTen = KMaxTInt/10;

#define KHorizTab 0x09
#define KSpace 0x20
#define KZero 0x30
#define KNine 0x39
#define KDecimal 0x2E


const TInt KMaxNumberSize	= 32;	// includes '-' sign

/**
	Removes any contiguous whitespace at the extremes of the data, as specified
	by aMode. Whitespace is defined by the functions TChar::IsSpace() - white
	space includes spaces, tabs, and separators (e.g. new line).
	
	@param			aData	A descriptor pointer with the data.
	@param			aMode	The specified removal mode.
	@return			The number of consumed whitespace characters. If the data is
	empty or the data is all whitespace the error code KUriUtilsErrEmptyData is
	returned.
*/
EXPORT_C TInt InetProtTextUtils::RemoveWhiteSpace(TPtrC8& aData, TRemoveMode aMode)
	{
	TInt consumed = DoRemoveWhiteSpace(aData, aMode);
	if( consumed == KUriUtilsErrEmptyData )
		{
		// Either empty data or all whitespace
		aData.Set(KNullDesC8());
		}
	return consumed;
	}

/**
	Removes any contiguous whitespace at the extremes of the data, as specified
	by aMode. Whitespace is defined by the functions TChar::IsSpace() - white
	space includes spaces, tabs, and separators (e.g. new line).
	
	@param			aData	A descriptor pointer with the data.
	@param			aMode	The specified removal mode.
	@return			The number of consumed whitespace characters. If the data is
	empty or the data is all whitespace the error code KUriUtilsErrEmptyData is
	returned.
*/
EXPORT_C TInt InetProtTextUtils::RemoveWhiteSpace(TPtrC16& aData, TRemoveMode aMode)
	{
	TInt consumed = DoRemoveWhiteSpace(aData, aMode);
	if( consumed == KUriUtilsErrEmptyData )
		{
		// Either empty data or all whitespace
		aData.Set(KNullDesC16());
		}
	return consumed;
	}

/**
	Converts an integer value into its decimal character representation.
	
	@param			aInt		An integer value.	
	@param			aBuffer		An output argument of a pointer to a data buffer
	that will contain the character representation of the integer value.
*/
EXPORT_C void InetProtTextUtils::ConvertIntToDescriptorL(TInt aInt, HBufC8*& aBuffer)
	{
	DoConvertIntToDescriptorL(aInt, aBuffer, EDecimal);
	}

/**
	Converts an integer value into its decimal character representation.
	
	@param			aInt		An integer value.	
	@param			aBuffer		An output argument of a pointer to a data buffer
	that will contain the character representation of the integer value.
*/
EXPORT_C void InetProtTextUtils::ConvertIntToDescriptorL(TInt aInt, HBufC16*& aBuffer)
	{
	DoConvertIntToDescriptorL(aInt, aBuffer, EDecimal);
	}

/**
	Converts the character representation of an integer into its numeric value.
	Preceeding whitespace is ignored and the integer is delimited by either the
	end of the data, whitespace or any other character other than 0 to 9.
	
	@param			aData	A descriptor pointer with the data.
	@param			aInt	An output argument in which the numeric value of the
	data is placed.
	@return			The number of consumed characters that contributed to the 
					integer value, including any whitespace before the integer. 
					If the data is empty the error code KUriUtilsErrEmptyData 
					is returned. If there are no digits at the beginning of the
					data the error code KUriUtilsErrNoIntergerInData is returned.
*/
EXPORT_C TInt InetProtTextUtils::ConvertDescriptorToInt(const TDesC8& aData, TInt& aInt)
	{
	// Do conversion and return number of consumed characters
	return DoConvertDescriptorToInt(TPtrC8(aData), aInt, EDecimal);
	}

/**
	Converts the character representation of an integer into its numeric value.
	Preceeding whitespace is ignored and the integer is delimited by either the
	end of the data, whitespace or any other character other than 0 to 9.
	
	@param			aData	A descriptor pointer with the data.
	@param			aInt	An output argument in which the numeric value of the
	data is placed.
	@return			The number of consumed characters that contributed to the 
					integer value, including any whitespace before the integer.
					If the data is empty the error code KUriUtilsErrEmptyData is
					returned. If there are no digits at the beginning of the data
					the error code KUriUtilsErrNoIntergerInData is returned.
*/
EXPORT_C TInt InetProtTextUtils::ConvertDescriptorToInt(const TDesC16& aData, TInt& aInt)
	{
	// Do conversion and return number of consumed characters
	return DoConvertDescriptorToInt(TPtrC16(aData), aInt, EDecimal);
	}

/**
	@internalComponent
	
	Converts an integer to its hex representation.
	
	@param		aHex	The integer value to convert.
	@param		aBuffer	An output argument that is set to the buffer containing 
						the hex representation of aValue.
	@panic		EInvariantFalse	The integer value was too big.
*/
EXPORT_C void InetProtTextUtils::ConvertHexToDescriptorL(TInt aHex, HBufC8*& aBuffer)
	{
	DoConvertIntToDescriptorL(aHex, aBuffer, EHex);
	}

/**
	@internalComponent
	
	Converts an integer to its hex representation.
	
	@param		aHex	The integer value to convert.
	@param		aBuffer	An output argument that is set to the buffer containing 
						the hex representation of aValue.
	@panic		EInvariantFalse	The integer value was too big.
*/
EXPORT_C void InetProtTextUtils::ConvertHexToDescriptorL(TInt aHex, HBufC16*& aBuffer)
	{
	DoConvertIntToDescriptorL(aHex, aBuffer, EHex);
	}

/**
	@internalComponent
	
	Descriptor to hex convertor. Searches the descriptor buffer for a hex number
	representation at the start of the buffer. The hex number is deemed to have 
	ended when the first non-hex character is found. The numeric value of the 
	hex number is returned along with the number characters consumed in obtaining
	the number. The amount of leading whitespace is included in the number of 
	characters consumed.
	
	@param		aData	The descriptor buffer to be parsed.
	@param		aHex	An output argument set to the numeric value of the 
						parsed hex number.
	@return		The number of characters consumed in parsing the hex number. If 
				the descriptor was empty then KUriUtilsErrEmptyData is returned.
				If no hex number was parsed then KUriUtilsErrNoIntergerInData is
				returned.
*/
EXPORT_C TInt InetProtTextUtils::ConvertDescriptorToHex(const TDesC8& aData, TInt& aHex)
	{
	// Do conversion, returning the number of consumed characters
	return DoConvertDescriptorToInt(TPtrC8(aData), aHex, EHex);
	}

/**
	@internalComponent
	
	Descriptor to hex convertor. Searches the descriptor buffer for a hex number
	representation at the start of the buffer. The hex number is deemed to have 
	ended when the first non-hex character is found. The numeric value of the 
	hex number is returned along with the number characters consumed in obtaining
	the number. The amount of leading whitespace is included in the number of 
	characters consumed.
	
	@param		aData	The descriptor buffer to be parsed.
	@param		aHex	An output argument set to the numeric value of the 
						parsed hex number.
	@return		The number of characters consumed in parsing the hex number. If 
				the descriptor was empty then KUriUtilsErrEmptyData is returned.
				If no hex number was parsed then KUriUtilsErrNoIntergerInData is
				returned.
*/
EXPORT_C TInt InetProtTextUtils::ConvertDescriptorToHex(const TDesC16& aData, TInt& aHex)
	{
	// Do conversion, returning the number of consumed characters
	return DoConvertDescriptorToInt(TPtrC16(aData), aHex, EHex);
	}

/** 
	Extract a token from the head of the supplied buffer, which is assumed to be
	a token-list. The tokens are separated by the specified character. Any white
	space surrounding the token is stripped out.  The number of characters 
	consumed from the buffer are returned. The buffer is updated to not include
	the extracted token including the separator.
	
	@param			aBuffer		The buffer containing the token-list.
	@param			aToken		An output argument containing the extracted token.
	@param			aSeparator	The separator character.
	@return			The number of characters consumed from the buffer.
*/
EXPORT_C TInt InetProtTextUtils::ExtractNextTokenFromList(TPtrC8& aBuffer, TPtrC8& aToken, TChar aSeparator)
	{
	return DoExtractNextTokenFromList(aBuffer, aToken, aSeparator);
	}

/** 
	Extract a token from the head of the supplied buffer, which is assumed to be
	a token-list. The tokens are separated by the specified character. Any white
	space surrounding the token is stripped out.  The number of characters 
	consumed from the buffer are returned. The buffer is updated to not include
	the extracted token including the separator.
	
	@param			aBuffer		The buffer containing the token-list.
	@param			aToken		An output argument containing the extracted token.
	@param			aSeparator	The separator character.
	@return			The number of characters consumed from the buffer.
*/
EXPORT_C TInt InetProtTextUtils::ExtractNextTokenFromList(TPtrC16& aBuffer, TPtrC16& aToken, TChar aSeparator)
	{
	return DoExtractNextTokenFromList(aBuffer, aToken, aSeparator);
	}

/** 
	Extract a token from the head of the supplied buffer, which is assumed to be
	a token-list. The tokens are separated by one of the specified characters. 
	Any white space surrounding the token is stripped out.  The number of 
	characters consumed from the buffer are returned. The buffer is updated to 
	not include the extracted token including the separator.
	
	@param			aBuffer		The buffer containing the token-list.
	@param			aToken		An output argument containing the extracted token.
	@param			aSeparators	The list of separator characters.
	@return			The number of characters consumed from the buffer.
*/
EXPORT_C TInt InetProtTextUtils::ExtractNextTokenFromList(TPtrC8& aBuffer, TPtrC8& aToken, const TDesC8& aSeparators)
	{
	TPtrC8 separators = aSeparators;
	return DoExtractNextTokenFromList(aBuffer, aToken, separators);
	}

/** 
	Extract a token from the head of the supplied buffer, which is assumed to be
	a token-list. The tokens are separated by one of the specified characters. 
	Any white space surrounding the token is stripped out.  The number of 
	characters consumed from the buffer are returned. The buffer is updated to 
	not include the extracted token including the separator.
	
	@param			aBuffer		The buffer containing the token-list.
	@param			aToken		An output argument containing the extracted token.
	@param			aSeparators	The list of separator characters.
	@return			The number of characters consumed from the buffer.
*/
EXPORT_C TInt InetProtTextUtils::ExtractNextTokenFromList(TPtrC16& aBuffer, TPtrC16& aToken, const TDesC16& aSeparators)
	{
	TPtrC16 separators = aSeparators;
	return DoExtractNextTokenFromList(aBuffer, aToken, separators);
	}

/**	
	Extract a quoted string value from the head of the supplied buffer. Anything
	outside the quotes is discarded and the quotes themselves are not included 
	in the returned string.
	
	@param			aBuffer			The buffer containing the quoted.
	@param			aQuotedString	An output argument containing the extracted
									string.
	@return			The number of characters consumed from the buffer.
*/
EXPORT_C TInt InetProtTextUtils::ExtractQuotedStringL(TPtrC8& aBuffer, TPtrC8& aQuotedString)
	{
	return DoExtractQuotedStringL(aBuffer, aQuotedString);
	}

/**	
	Extract a quoted string value from the head of the supplied buffer. Anything
	outside the quotes is discarded and the quotes themselves are not included 
	in the returned string.
	
	@param			aBuffer			The buffer containing the quoted.
	@param			aQuotedString	An output argument containing the extracted
									string.
	@return			The number of characters consumed from the buffer.
*/
EXPORT_C TInt InetProtTextUtils::ExtractQuotedStringL(TPtrC16& aBuffer, TPtrC16& aQuotedString)

	{
	return DoExtractQuotedStringL(aBuffer, aQuotedString);
	}

/**	
	@internalTechnology
	
	Extract an integer value from the head of the supplied buffer.
	
	@param			aBuffer	The buffer containing the integer value.
	@param			aIntVal	An output argument in which extracted integer value
							is placed.
	@param			aAllowNonWsTerminator	If set to true whitespace is considered as the 
									terminator, if set to false a non-decimal charecter
									is considered as terminator.
	@return			The number of characters in aBuffer after the integer.
	@leave			KUriUtilsErrMalformedInteger if the value in aBuffer is not an integer 
					(ie, contains non-decimal characters, or is too big to fit in a TInt)
*/
EXPORT_C TInt InetProtTextUtils::ExtractIntegerValueL(TPtrC8& aBuffer, TInt& aIntVal, TBool aAllowNonWsTerminator)
	{
	// Anticipate an empty token by default
	TPtrC8 data(aBuffer);

	// Trim out any whitespace to the left the integer and check there's something left!
	InetProtTextUtils::RemoveWhiteSpace(data, InetProtTextUtils::ERemoveLeft);
	const TInt len = data.Length();
	TInt retVal = aBuffer.Length() -  len; // the amount of WS consumed
	if (len == 0)
		{
		// No integer was found - the supplied buffer was all WS!
		aIntVal = 0;
		retVal = aBuffer.Length();
		aBuffer.Set(data);
		}
	else
		{
		// Got some data to work with... find out how many digits are present. Assume that the integer
		// required is a contiguous set of decimal digits followed by WS or the end of the buffer
		TInt numDigits = 0;
		TChar ch = KZero; // default most significant digit to zero without affecting the result
		TBool done = EFalse;
		while (!done)
			{
			// check for the end of the integer. This depends on what is allowed to terminate it.
			if (aAllowNonWsTerminator)
				{
				done = (numDigits == data.Length());
				if (!done)
					{
					ch = data[numDigits];
					done = ((ch < KZero) || (ch > KNine));
					}
				}
			else
				{
				done = (numDigits == data.Length());
				if (!done)
					{
					ch = data[numDigits];
					done = ((ch == KSpace) || (ch == KHorizTab) || (ch == KDecimal));
					}
				}

			// Check that we're getting digits
			if (!done)
				{
				if ((ch < KZero) || (ch > KNine)) // digits 0-9
					User::Leave(KUriUtilsErrMalformedInteger);
				++numDigits;
				}
			}
		// Make sure there's no overflow (trivial check)
		if(numDigits>KMaxTIntDecimalDigits)
			User::Leave(KUriUtilsErrMalformedInteger);
		
		// Now work back, building up the integer
		aIntVal = 0;
		for (TInt pos = 0; pos < numDigits; pos++)
			{
			// double check no overflow (
			if(aIntVal>KMaxTIntDividedByTen   ||  //works except for 2147483648 and ...49)
				( (aIntVal == KMaxTIntDividedByTen)  && (data[pos] >= (KNine-1)) ) // covers those two cases
				)  
				User::Leave(KUriUtilsErrMalformedInteger);
			aIntVal = (data[pos] - KZero) + (aIntVal * 10); // decimal, innit?
			}

		// work out how much this has consumed
		retVal += numDigits;

		// Finally ensure the buffer has had the correct amount of data consumed
		if (len == aBuffer.Length())
			aBuffer.Set(KNullDesC8()); // there was nothing to the right of the integer
		else
			aBuffer.Set(aBuffer.Mid(retVal));
		}
	return retVal;
	}

/*
 *	Local methods
 */

/**
	@internalComponent
	
	Does the whitespace removal. Checks the mode and then calls the appropriate
	functions.

	@warning		Will panic will KUriUtilsErrBadTExtRemoveMode if aMode not valid.
	@param			aData	A descriptor pointer with the data.
	@param			aMode	The specified removal mode.
	@return			The number of consumed whitespace characters. If the data is
					empty or the data is all whitespace the error code 
					KUriUtilsErrEmptyData is returned.
*/
template<class TPtrCType>
TInt DoRemoveWhiteSpace(TPtrCType& aData, InetProtTextUtils::TRemoveMode aMode)
	{
	TInt consumed = 0;
	switch( aMode )
		{
	case InetProtTextUtils::ERemoveLeft:
		{
		consumed = FindWhiteSpaceLeft(aData);
		if( consumed > 0 )
			{
			// Found some whitespace
			aData.Set(aData.Mid(consumed));
			}
		} break;
	case InetProtTextUtils::ERemoveRight:
		{
		consumed = FindWhiteSpaceRight(aData);
		if( consumed > 0 )
			{
			// Found some whitespace
			aData.Set(aData.Left(aData.Length() - consumed));
			}
		} break;
	case InetProtTextUtils::ERemoveBoth:
		{
		// Remove from left first...
		consumed = FindWhiteSpaceLeft(aData);
 		if( consumed != KUriUtilsErrEmptyData )
			{
			// Found some whitespace - set the pointer then remove from right
			aData.Set(aData.Mid(consumed));
			TInt consumedRight = FindWhiteSpaceRight(aData);

			// To get this far descriptor is non-empty and not all whitespace - no check
			aData.Set(aData.Left(aData.Length() - consumedRight));
			consumed += consumedRight;
			}
		} break;
	default:
		__ASSERT_DEBUG(EFalse, User::Panic(KTextUtilsPanicCategory, KUriUtilsErrBadTextRemoveMode));
		}
	return consumed;
	}

/**
	@internalComponent
	
	Finds the number of contiguous whitespace characters at the 
	beginning of the data.

	@param			aData	A descriptor pointer with the data.
	@return			The number of consumed whitespace characters. If the data is
					empty or the data is all whitespace the error code 
					KTextUtilsErrEmptyData is returned.
 */
template<class TPtrCType>
TInt FindWhiteSpaceLeft(const TPtrCType& aData)
	{
	const TInt length = aData.Length();
	if( length == 0 )
		return KUriUtilsErrEmptyData;
	TInt i = 0;
	TBool done = EFalse;
	while( !done && i < length )
		{
		TChar current = aData[i];
		done  = !current.IsSpace();
		if( !done )
			++i;
		}
	if( i == length )
		return KUriUtilsErrEmptyData;
	return i;
	}

/**
	@internalComponent
	
	Finds the number of contiguous whitespace characters at the end of the data.

	@param			aData	A descriptor pointer with the data.
	@return			The number of consumed whitespace characters. If the data is
					empty or the data is all whitespace the error code 
					KTextUtilsErrEmptyData is returned.
*/
template<class TPtrCType>
TInt FindWhiteSpaceRight(const TPtrCType& aData)
	{
	const TInt length = aData.Length();
	if( length == 0 )
		return KUriUtilsErrEmptyData;
	TInt i = 0;
	TBool done = EFalse;
	while( !done && i < length )
		{
		TChar current = aData[(length-1) - i];
		done  = !current.IsSpace();
		if( !done )
			++i;
		}
	if( i == length )
		return KUriUtilsErrEmptyData;
	return i;
	}

/** 
	Extracts next token from the buffer.
	
	@param			aBuffer		The buffer containing the token-list.
	@param			aToken		An output argument containing the extracted token.
	@param			aSeparator	The separator character.
	@return			The number of characters consumed from the buffer.
*/
template<class TPtrCType>
TInt DoExtractNextTokenFromList(TPtrCType& aBuffer, TPtrCType& aToken, TChar aSeparator)
	{
	// Anticipate an empty token by default.
	TInt consumed = 0;

	// Search for the separator
	const TInt pos = aBuffer.Locate(aSeparator);

	// If the separator was found, take chars upto it. Otherwise use the whole
	// remaining buffer - implies the last token in the list.
	if( pos == KErrNotFound )
		{
		// Last token - take the whole lot
		aToken.Set(aBuffer);
		consumed = aBuffer.Length();
		}
	else
		{
		// Take upto the separator position for the token, and move the buffer 
		// past the token.
		aToken.Set(aBuffer.Left(pos));
		consumed = pos + 1;	// include the separator
		}
	// Set the buffer to move past the extracted token.
	aBuffer.Set(aBuffer.Mid(consumed));

	// Trim out any whitespace surrounding the token, and return how many charas
	// were consumed.
	InetProtTextUtils::RemoveWhiteSpace(aToken, InetProtTextUtils::ERemoveBoth);
	return consumed;
	}

/** 
	Extracts next token from the buffer.
	
	@param			aBuffer		The buffer containing the token-list.
	@param			aToken		An output argument containing the extracted token.
	@param			aSeparator	The separator.
	@return			The number of characters consumed from the buffer.
*/
template<class TPtrCType>
TInt DoExtractNextTokenFromList(TPtrCType& aBuffer, TPtrCType& aToken, const TPtrCType& aSeparator)
	{
	// Finds the next token in the list, where all the characters in
	// the separator array are considered to be separators.

	// Anticipate an empty token by default.
	TInt consumed = 0;

	// Search for separator...
	const TInt length = aBuffer.Length();
	TInt pos = 0;
	TBool found = EFalse;
	while( !found && pos < length )
		{
		// If we find a quoted string, skip over it (and any token
		// separators within it)
		if( aBuffer[pos] == KQuote )
			{
			for( ++pos; pos < length && aBuffer[pos] != KQuote; ++pos )
				{}
			if (pos == length)
				break; // end of string reached
			}
		if( aSeparator.Locate(aBuffer[pos]) != KErrNotFound )
			found = ETrue;
		else
			++pos;
		}

	// If the separator was found, take chars upto it. Otherwise use the whole
	// remaining buffer - implies the last token in the list.
	if( found )
		{
		// Take upto the separator position for the token, and move the buffer 
		// past the token.
		aToken.Set(aBuffer.Left(pos));
		consumed = pos + 1;
		}
	else
		{
		// Last token - take the whole lot
		aToken.Set(aBuffer);
		consumed = aBuffer.Length();
		}
	// Set the buffer to move past the extracted token.
	aBuffer.Set(aBuffer.Mid(consumed));

	// Trim out any whitespace surrounding the token, and return how
	// many characters were extracted.
	InetProtTextUtils::RemoveWhiteSpace(aToken, InetProtTextUtils::ERemoveBoth);
	return consumed;
	}

/**	
	Extract a quoted string value from the buffer. 
	
	@param			aBuffer			The buffer containing the quoted.
	@param			aQuotedString	An output argument containing the extracted
									string.
	@return			The number of characters consumed from the buffer.
*/
template<class TPtrCType>
TInt DoExtractQuotedStringL(TPtrCType& aBuffer, TPtrCType& aQuotedString)
	{
	// Find the opening "
	TInt quotePos1 = aBuffer.Locate(KQuote);
	if( quotePos1 == KErrNotFound )
		User::Leave(KUriUtilsErrDecodeMalformedQuotedString);

	// Find the closing "
	TPtrCType temp = aBuffer.Mid(quotePos1 + 1);	// ignore the "
	TInt quotePos2 = temp.LocateReverse(KQuote);
	if( quotePos2 == KErrNotFound )
		User::Leave(KUriUtilsErrDecodeMalformedQuotedString);
	
	// return what's between them and consume the right number of characters from the buffer
	aQuotedString.Set(temp.Left(quotePos2));
	TInt consumed = quotePos1 + quotePos2 + 2; // +2 is to consume the quotes themselves
	aBuffer.Set(aBuffer.Mid(consumed));
	return consumed;
	}

/**
	Converts an integer value into its decimal character representation.
	
	@param			aInt		An integer value.	
	@param			aBuffer		An output argument of a pointer to a data buffer
	that will contain the character representation of the integer value.
	@param			aRadix		enum value.	
*/
template<class HBufCType>
void DoConvertIntToDescriptorL(TInt aInt, HBufCType*& aBuffer, TRadix aRadix)
	{
	// Create the buffer - max possible size for binary number is 32
	TBuf8<KMaxNumberSize> buf;
	buf.SetMax();

	// Is the number -ve?
	TUint value = aInt;
	TBool negative = EFalse;
	if( aInt < 0 )
		{
		negative = ETrue;
		value = -aInt;
		}

	// Now form the number...
	TInt index = KMaxNumberSize;
	do	
		{
		// Update the index
		--index;
		__ASSERT_DEBUG( index > 0, User::Invariant() );

		// Find the value of the least significant digit
		TUint q = value/aRadix;
		TUint c = value - (q*aRadix);

		// Convert digit to character and insert into the buffer.
		( c > 9 ) ? c += (KAChar-10) : c += KZeroChar;
		buf[index] = STATIC_CAST(TUint8, c);

		// Update the value left to convert.
		value = q;
		} while( value > 0 );

	// Add the '-' sign if number was negative
	if( negative )
		buf[--index] = KMinusSign;

	// Create the output buffer...
	TPtrC8 number = buf.Mid(index);
	aBuffer = HBufCType::NewL(number.Length());
	aBuffer->Des().Copy(number);
	}

/**
	Converts the character representation of an integer into its numeric value.
	Preceeding whitespace is ignored and the integer is delimited by either the
	end of the data, whitespace or any other character other than 0 to 9.
	
	@param			aData	A descriptor pointer with the data.
	@param			aInt	An output argument in which the numeric value of the
	data is placed.
	@param			aRadix		enum value.	
	@return			The number of consumed characters that contributed to the 
					integer value, including any whitespace before the integer. 
					If the data is empty the error code KUriUtilsErrEmptyData 
					is returned. If there are no digits at the beginning of the
					data the error code KUriUtilsErrNoIntergerInData is returned.
*/
template<class TPtrCType>
TInt DoConvertDescriptorToInt(const TPtrCType& aData, TInt& aInt, TRadix aRadix)
	{
	// Remove any whitespace before the digits
	TPtrCType data = aData;
	TInt consumed = InetProtTextUtils::RemoveWhiteSpace(data, InetProtTextUtils::ERemoveLeft);

	// Ensure that there are some digits to convert
	TInt length = data.Length();
	if( length == 0 )
		{
		return KUriUtilsErrEmptyData;
		}

	// Search for digits and calculate value - stop when non-digit found
	TInt value = 0;
	TBool done = EFalse;
	
	TBool negative=EFalse;

	if(data[0]==KMinusSign)
	   {
		negative=ETrue;
	   }	
	
	TInt i=0;
	
	// Length is verifying as user input may be just only '-' sign.
	if( negative && length > 1 )
		{
		i = 1;	
		}
	while( !done )
		{
		// Get the current char and see if it is a digit
		TChar c = data[i];

		if( c.IsHexDigit() )
			{
			// Need to convert char to its numeric value
			TInt digit = c - KZeroChar;

			// Was the char in the A-F range?
			if( digit > 9 )
				digit = c.GetUpperCase() - (KAChar - 10);
			
			if( digit > aRadix )
				{
				// The character is too large for the radix - end of data.
				done = ETrue;
				}
			else
				{
				// Update running total
				value *= aRadix;
				value += digit;
				}
			}
		else
			{
			// Non-digit found - we're done!
			done = ETrue;
			}
		// Check for end of data
		done = done || !(++i < length);
		}
	// Set up outputs
	if( i == 0 )
		{
		// No digits found in data - do not set aInt
		return KUriUtilsErrNoIntegerInData;
		}
	// Set the value of the interger
	if( negative )
		aInt = -value;
	else
		aInt = value;

	// Return consumed characters
	return i + consumed;
	}
