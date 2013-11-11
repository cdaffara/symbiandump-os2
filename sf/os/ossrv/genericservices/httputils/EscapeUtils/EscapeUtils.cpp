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

#include <escapeutils.h>

#include <utf.h>

#include "EscapeUtilsInternal.h"

// Constants
//
_LIT(KHexDigit, "0123456789ABCDEF");
_LIT(KExcludedData, "{}|\\^`<>#%\"");
_LIT8(KQueryData8, ";/?:@&=+$,[]");
_LIT8(KPathData8, "/;=?[]");
_LIT8(KAuthData8, ";:@?/[]");
_LIT8(KUrlEncoded8, ";/?:@&=+$[]!\'()~*");
_LIT16(KQueryData16, ";/?:@&=+$,[]");
_LIT16(KPathData16, "/;=?[]");
_LIT16(KAuthData16, ";:@?/[]");
_LIT16(KUrlEncoded16, ";/?:@&=+$[]!\'()~");
const TInt KEscapeUtilsConversionBufferSize	= 50;
const TInt KEscapeIndicator					= '%';
const TInt KEscapeTripleLength				= 3;
const TInt KEscDelimiterPos					= 0;
const TInt KMostSignificantNibblePos		= 1;
const TInt KLeastSignificantNibblePos		= 2;

// Panic category
//
#ifdef _DEBUG
	_LIT(KEscapeUtilsPanicCategory, "ESC-UTILS"); 
#endif

//
//
// Implementation of EscapeUtils
//
//

/**
	Escape encodes excluded and reserved characters in the data as escape triples. 
	The reserved characters are defined by the escape mode. These characters and the 
	set of excluded characters specified by RFC2396 form the entire set of excluded data.
						
	@since			6.0
	@leave		    KUriUtilsErr16BitChar. A 16-Bit character was found in the data to be escape encoded.
	@param			aData	A descriptor with the data to encode.
	@param			aEscapeMode	An enum specifying the escape mode.
	@return			A pointer to a descriptor buffer which contains the escape encoded data.
*/
EXPORT_C HBufC8* EscapeUtils::EscapeEncodeL(const TDesC8& aData, TEscapeMode aEscapeMode)
	{
	// Need descriptor pointer to reserved characters...
	TPtrC8 reserved;

	switch (aEscapeMode)
		{
	case EEscapeNormal:
		{
		// This is normal operation - no reserved chars
		reserved.Set(KNullDesC8);
		} break;
	case EEscapeQuery:
		{
		// Reserved data in a URI query - ; / ? : @ & = + $ ,
		reserved.Set(KQueryData8());
		} break;
	case EEscapePath:
		{
		// Reserved data in a URI path segment - / ; = ?
		reserved.Set(KPathData8());
		} break;
	case EEscapeAuth:
		{
		// Reserved data in a URI authority - ; : @ ? /
		reserved.Set(KAuthData8());
		} break;
	case EEscapeUrlEncoded:
		{
		// Reserved data in  Url Encoded data - ; / ? : @ & = + $ [ ] ! ' ( ) ~ *
		reserved.Set(KUrlEncoded8());
		} break;
	default:
		// Not supported return NULL
		__ASSERT_DEBUG(EFalse, User::Panic(KEscapeUtilsPanicCategory, KUriUtilsErrBadEscapeMode));
		return NULL;
		}
	return EscapeEncodeL(aData, reserved);
	}

/**
	Escape encodes excluded and reserved characters in the data as escape triples. The 
	reserved characters are defined by the escape mode. These characters and the set of 
	excluded characters specified by RFC2396 form the entire set of excluded data.
						
	@since			6.0
	@leave			KUriUtilsErr16BitChar. A 16-Bit character was found in the data to be escape encoded.
	@param			aData	A descriptor with the data to encode.
	@param			aEscapeMode	An enum specifying the escape mode.
	@return			A pointer to a descriptor buffer which contains the escape encoded data.
*/
EXPORT_C HBufC16* EscapeUtils::EscapeEncodeL(const TDesC16& aData, TEscapeMode aEscapeMode)
	{
	// Need to descriptor pointer to reserved chars
	TPtrC16 reserved;

	switch (aEscapeMode)
		{
	case EEscapeNormal:
		{
		// This is normal operation - no reserved chars
		reserved.Set(KNullDesC16);
		} break;
	case EEscapeQuery:
		{
		// Reserved data in a URI query - ; / ? : @ & = + $ [],
		reserved.Set(KQueryData16());
		} break;
	case EEscapePath:
		{
		// Reserved data in a URI path segment - / ; = ? []
		reserved.Set(KPathData16());
		} break;
	case EEscapeAuth:
		{
		// Reserved data in a URI authority - ; : @ ? / []
		reserved.Set(KAuthData16());
		} break;
	case EEscapeUrlEncoded:
		{
		// Reserved data in  Url Encoded data - ; / ? : @ & = + $ [ ] ! ' ( ) ~
		reserved.Set(KUrlEncoded16());
		} break;
	default:
		// Not supported return NULL
		__ASSERT_DEBUG(EFalse, User::Panic(KEscapeUtilsPanicCategory, KUriUtilsErrBadEscapeMode));
		return NULL;
		}
	return EscapeEncodeL(aData, reserved);
	}

/**
	Escape encodes excluded and reserved characters in the data as escape triples. These 
	characters and the set of excluded characters specified by RFC2396 form the entire set
	of excluded data.
						
	@since			6.0
	@leave			KUriUtilsErr16BitChar. A 16-Bit character was found in the data to be escape encoded.
	@param			aData			A descriptor with the data to encode.
	@param			aReservedChars	A descriptor with the reserved characters.
	@return			A pointer to a descriptor buffer which contains the escape encoded data.
*/
EXPORT_C HBufC8* EscapeUtils::EscapeEncodeL(const TDesC8& aData, const TDesC8& aReservedChars)
	{
	// Allocate space to build escaped url - consider worse case; all characters are excluded => length x 3
	HBufC8* buf = HBufC8::NewLC(aData.Length()*3);
	TPtr8 escaped = buf->Des();

	User::LeaveIfError(EscapeEncodeData(aData, aReservedChars, escaped));
	HBufC8* encoded = escaped.AllocL();

	CleanupStack::PopAndDestroy(buf);
	return encoded;
	}

/**
	Escape encodes excluded and reserved characters in the data as escape triples. These characters 
	and the set of excluded characters specified by RFC2396 form the entire set of excluded data.
						
	@since			6.0
	@leave			KUriUtilsErr16BitChar. A 16-Bit character was found in the data to be escape encoded.
	@param			aData			A descriptor with the data to encode.
	@param			aReservedChars	A descriptor with the reserved characters.
	@return			A pointer to a descriptor buffer which contains the escape encoded data.
*/
EXPORT_C HBufC16* EscapeUtils::EscapeEncodeL(const TDesC16& aData, const TDesC16& aReservedChars)
	{
	// Allocate space to build escaped url - consider worse case; all characters are excluded => length x 3
	HBufC16* buf = HBufC16::NewLC(aData.Length()*3);
	TPtr16 escaped = buf->Des();

	User::LeaveIfError(EscapeEncodeData(aData, aReservedChars, escaped));
	HBufC16* encoded = escaped.AllocL();

	CleanupStack::PopAndDestroy(buf);
	return encoded;
	}

/**
	Escape decodes the data.
	
	@since			6.0
	@param			aData	A descriptor with the data to decode.
	@return			A pointer to a descriptor buffer which contains the escape decoded data.
*/
EXPORT_C HBufC8* EscapeUtils::EscapeDecodeL(const TDesC8& aData)
	{
	// Allocate space to build unescaped data
	HBufC8* buf = HBufC8::NewLC(aData.Length());
	TPtr8 unescaped = buf->Des();

	User::LeaveIfError(EscapeDecodeData(aData, unescaped));
	HBufC8* decoded = unescaped.AllocL();

	CleanupStack::PopAndDestroy(buf);
	return decoded;
	}

/**
	Escape decodes the data.
	
	@since			6.0
	@param			aData	A descriptor with the data to decode.
	@return			A pointer to a descriptor buffer which contains the escape decoded data.
*/
EXPORT_C HBufC16* EscapeUtils::EscapeDecodeL(const TDesC16& aData)
	{
	// Allocate space to build unescaped data
	HBufC16* buf = HBufC16::NewLC(aData.Length());
	TPtr16 unescaped = buf->Des();

	User::LeaveIfError(EscapeDecodeData(aData, unescaped));
	HBufC16* decoded = unescaped.AllocL();

	CleanupStack::PopAndDestroy(buf);
	return decoded;
	}

/**
	@internalComponent
	
	escape encode only those characters that cannot be in a URI. assume all %hh are %encoded already.
	
	@param		aData	The descriptor buffer to be escape encoded.
	@return		A pointer to a descriptor buffer which contains the escape encoded data.
*/
HBufC8* EscapeUtils::ReEscapeEncodeL(const TDesC8& aData)
	{
	// Descriptor to hex digits and excluded chars
	const TDesC& KHexChars = KHexDigit;

	const TInt length = aData.Length();
	
	// find out how many characters need escape encoding
	TInt count = 0;
	for( TInt i=0; i<length; ++i )
		{
		TChar current( aData[i] );
		if( EscapeUtils::IsExcludedChar(current) && current != KFragmentDelimiter && 
			!(current == KEscapeIndicator &&  i+2<length && TChar(aData[i+1]).IsHexDigit() && TChar(aData[i+2]).IsHexDigit() ) )
			{
			count++;
			}
		}
	if( count == 0)	// no encoding needed, just allocate and return the whole string
		{
		return aData.AllocL();
		}
	// pre-allocate space for the descriptor
	HBufC8* buf = HBufC8::NewLC( aData.Length() + count*2 ); // two extra chars for each escaped
	TPtr8 escaped = buf->Des();

	for( TInt i=0; i<length; ++i )
		{
		// Check if current character must be escaped
		TChar current ( aData[i] );
		// Check if current character is excluded, but not  if it appears to be escape encoded
		TBool excluded = EscapeUtils::IsExcludedChar(current) && current != KFragmentDelimiter &&
			!(current == KEscapeIndicator &&  i+2<length && TChar(aData[i+1]).IsHexDigit() && TChar(aData[i+2]).IsHexDigit() );

		if( excluded )
			{
			// Excluded char - escape encode
			escaped.Append(KEscapeIndicator);
			const TInt mostSignificantNibble = (current & 0xf0) >> 4;	// Get msNibble by masking against 11110000 and dividing by 16 (>>4)
			escaped.Append(KHexChars[mostSignificantNibble]);
			const TInt leastSignificantNibble = (current & 0x0f);		// Get lsNibble by masking against 00001111
			escaped.Append(KHexChars[leastSignificantNibble]);
			}
		else
			{
			// Not an excluded char - just append			  
			escaped.Append(current);
			}
		}
	CleanupStack::Pop(buf);
	return buf;
	}


/**

	Converts UNICODE data into UTF8 format.
	
	@since			6.0
	@leave			KUriUtilsCannotConvert. When the input data cannot be converted.
	@param			aString			A descriptor with the data to convert.
	@return			A pointer to an 8-bit descriptor buffer which contains UTF8 data.
*/
EXPORT_C HBufC8* EscapeUtils::ConvertFromUnicodeToUtf8L(const TDesC& aString)
	{
	// Return an empty buffer straight-away
	if( aString.Compare(KNullDesC) == 0 )
		return KNullDesC8().AllocL();

	// Convert from Unicode to UTF8
	TPtrC unicode = aString;
	TBuf8<KEscapeUtilsConversionBufferSize> buf;
	HBufC8* utf8Buffer = HBufC8::NewLC(unicode.Length());
	TPtr8 utf8 = utf8Buffer->Des();

	// Loop until all of the filename is converted
	FOREVER
		{
		const TInt returnValue = CnvUtfConverter::ConvertFromUnicodeToUtf8(buf, unicode);
		if( returnValue == CnvUtfConverter::EErrorIllFormedInput || returnValue < 0)
			User::Leave(KUriUtilsCannotConvert);

		// Is escapedFullPath too small?
		if( utf8.Length() + buf.Length() > utf8.MaxLength() )
			{
			utf8Buffer = utf8Buffer->ReAllocL(utf8.Length() + buf.Length());
			CleanupStack::Pop();	// utf8Buffer (old version)
			CleanupStack::PushL(utf8Buffer);	// new version
			utf8.Set(utf8Buffer->Des());
			}
		// Copy converted characters
		utf8.Append(buf);

		if( returnValue == KErrNone )
			break; // All of aUnicodeText has been converted and handled

		// Set input descriptor to remaining characters
		unicode.Set(unicode.Right(returnValue));
		}
	CleanupStack::Pop(utf8Buffer);
	return utf8Buffer;		// Ownership transfered to caller
	}

/**
	Converts UTF8 format into UNICODE data.
	
	@since			6.0
	@leave			KUriUtilsCannotConvert. When the input data cannot be converted.
	@param			aString			A descriptor with the data to convert.
	@return			A pointer to a 16-bit descriptor buffer which contains UNICODE data.
*/
EXPORT_C HBufC* EscapeUtils::ConvertToUnicodeFromUtf8L(const TDesC8& aString)
	{
	// Return an empty buffer straight-away
	if( aString.Compare(KNullDesC8) == 0 )
		return KNullDesC().AllocL();

	// Convert from Unicode to UTF8
	TPtrC8 utf8 = aString;
	TBuf<KEscapeUtilsConversionBufferSize> buf;
	HBufC* unicodeBuffer = HBufC::NewLC(utf8.Length());
	TPtr unicode = unicodeBuffer->Des();

	// Loop until all of the filename is converted
	FOREVER
		{
		const TInt returnValue = CnvUtfConverter::ConvertToUnicodeFromUtf8(buf, utf8);
		if( returnValue == CnvUtfConverter::EErrorIllFormedInput || returnValue < 0)
			User::Leave(KUriUtilsCannotConvert);

		// Is escapedFullPath too small?
		if( unicode.Length() + buf.Length() > unicode.MaxLength() )
			{
			unicodeBuffer = unicodeBuffer->ReAllocL(unicode.Length() + buf.Length());
			CleanupStack::Pop();	// unicodeBuffer (old version)
			CleanupStack::PushL(unicodeBuffer);	// new version
			unicode.Set(unicodeBuffer->Des());
			}
		// Copy converted characters
		unicode.Append(buf);

		if( returnValue==0 )
			break; // All of utf8 has been converted and handled

		// Set input descriptor to remaining characters
		utf8.Set(utf8.Right(returnValue));
		}
	CleanupStack::Pop(unicodeBuffer);
	return unicodeBuffer;	// Ownership transfered to caller
	}

/**
	Checks to see if the input argument is excluded.
	
	@since			6.0
	@param			aChar	The character to be checked.
	@return			A boolean value of ETrue if the character is an excluded one, or
					EFalse if it is not.
 */
EXPORT_C TBool EscapeUtils::IsExcludedChar(TChar aChar)
	{
	const TDesC& KExcludedChars = KExcludedData;
	TBool excluded = KExcludedChars.Locate(aChar) != KErrNotFound || aChar <= 0x1F || aChar == ' ' || aChar > 0x7E;
	return excluded;
	}

/**
	Checks for an escape triple at the start of the input descriptor. If there is a triple 
	its value is calculated and returned through the output argument aHexValue. If there is 
	no escape triple then this argument is left unchanged.
						
	@since			6.0
	@param			aData		The descriptor to be checked for an escape triple.
	@param			aHexValue	The output argument with the value of the escape triple 
	if it exists.
	@return			A boolean value of ETrue if there is an escape triple at the start of 
	the input descriptor, EFalse otherwise.
*/
EXPORT_C TBool EscapeUtils::IsEscapeTriple(const TDesC8& aData, TInt& aHexValue)
	{
	return CheckAndConvertEscapeTriple(aData, aHexValue);
	}

/**
	Checks for an escape triple at the start of the input descriptor. If there is a triple 
	its value is calculated and returned through the output argument aHexValue. If there is 
	no escape triple then this argument is left unchanged.
						
	@since			6.0
	@param			aData		The descriptor to be checked for an escape triple.
	@param			aHexValue	The output argument with the value of the escape triple 
	if it exists.
	@return			A boolean value of ETrue if there is an escape triple at the start of 
	the input descriptor, EFalse otherwise.
*/
EXPORT_C TBool EscapeUtils::IsEscapeTriple(const TDesC16& aData, TInt& aHexValue)
	{
	return CheckAndConvertEscapeTriple(aData, aHexValue);
	}

/**
	returns the escape encoded descriptor output. This checks the every character of aData 
	against aCharsToEscape and if it exist then it escape encodes that character.

	@param			aData	The descriptor to be checked against escaping set of characters.
	@param			aCharsToEscape	The set of escape characters.
	@return			A pointer to the escape encoded descriptor.
*/
EXPORT_C HBufC8* EscapeUtils::SpecificEscapeEncodeL ( const TDesC8& aData, const TDesC8& aCharsToEscape )
	{
	// Descriptor to hex digits and excluded chars
	const TDesC& KHexChars = KHexDigit;

	const TInt length = aData.Length();
	
	// find out how many characters need escape encoding
	TInt count = 0;
	for( TInt i=0; i<length; ++i )
		{
		TChar current( aData[i] );
		if ( current <= 0x1F || aCharsToEscape.Locate ( current ) != KErrNotFound || current > 0x7E )
			{
			count++;
			}
		}
	if( count == 0)	// no encoding needed, just allocate and return the whole string
		{
		return aData.AllocL();
		}

	// pre-allocate space for the descriptor
	HBufC8* buf = HBufC8::NewLC( length + count*2 ); // two extra chars for each escaped
	TPtr8 escaped = buf->Des();

	for( TInt i=0; i<length; ++i )
		{
		// Check if current character must be escaped
		TChar current ( aData[i] );
		// Check if current character is excluded ( control characters and the character specified for escaping )
		TBool excluded = current <= 0x1F || ( aCharsToEscape.Locate ( current ) != KErrNotFound ) || current > 0x7E;

		if( excluded )
			{
			// Excluded char - escape encode
			escaped.Append(KEscapeIndicator);
			const TInt mostSignificantNibble = (current & 0xf0) >> 4;	// Get msNibble by masking against 11110000 and dividing by 16 (>>4)
			escaped.Append(KHexChars[mostSignificantNibble]);
			const TInt leastSignificantNibble = (current & 0x0f);		// Get lsNibble by masking against 00001111
			escaped.Append(KHexChars[leastSignificantNibble]);
			}
		else
			{
			// Not an excluded char - just append			  
			escaped.Append(current);
			}
		}
	CleanupStack::Pop(buf);
	return buf;		
	}

/** 
    The Dummy API is used to redirect to SpecificEscapeEncodeL() API in order to preserve BC and is made private                                                                                                       
	to ensure no-one else starts using it.
*/
EXPORT_C HBufC8* EscapeUtils::DummyForwardingFunctionForCompatibility( const TDesC8& aData, const TDesC8& aCharsToEscape )
	{
	return EscapeUtils::SpecificEscapeEncodeL ( aData, aCharsToEscape );
	}


//
//
// Implementation of LOCAL functions
//
//
/**
	Escape encodes the data, converting the reserved characters and excluded characters defined by 
	RFC2396 as escape triples.
						
	@since			6.0
	@warning		This function will panic if the output descriptor aEncodedData is
	not big enough to append all the data.
	@param			aData			A descriptor with the data to encode.
	@param			aReservedChars  Reserved characters set.
	@param			aEncodedData	The output descriptor pointer where the escaped encoded 
	data is placed.
	@return			An error code of KUriUtilsErr16BitChar if the data contains a 16-bit 
	character. KErrNone if the data was successfully encoded.
 */				
template<class TDesCType, class TPtrType>
TInt EscapeEncodeData(const TDesCType& aData, const TDesCType& aReservedChars, TPtrType& aEncodedData)
	{
	// Descriptor to hex digits and excluded chars
	const TDesC& KHexChars = KHexDigit;

	const TInt length = aData.Length();
	for( TInt i=0; i<length; ++i )
		{
		// Check if current character must be escaped, will return error if not 8-bit character
		TChar current = aData[i];
		if( current > 0xff )
			{
			__ASSERT_DEBUG(EFalse, User::Panic(KEscapeUtilsPanicCategory, KUriUtilsErr16BitChar));
			return (KUriUtilsErr16BitChar);
			}
		// Check if current character is excluded, a control character or a space
		TBool excluded = EscapeUtils::IsExcludedChar(current) || aReservedChars.Locate(current) != KErrNotFound;
	  if ( excluded )
		   {
       		// Excluded char - escape encode
			aEncodedData.Append(KEscapeIndicator);
			const TInt mostSignificantNibble = (current & 0xf0) >> 4;	// Get msNibble by masking against 11110000 and dividing by 16 (>>4)
			aEncodedData.Append(KHexChars[mostSignificantNibble]);
			const TInt leastSignificantNibble = (current & 0x0f);		// Get lsNibble by masking against 00001111
			aEncodedData.Append(KHexChars[leastSignificantNibble]);
			}
		else
			{
			// Not an excluded char or It's already Escape encode - just append			  
			aEncodedData.Append(current);
		    }
		}
	return KErrNone;
	}

/**
	Escape decodes the data, converting escape triples back to their single character value.
	
	@since			6.0
	@warning		This function will panic if the output descriptor aDecodedData is not big 
	enough to append all the data.
	@param			aData			A descriptor with the data to decode.
	@param			aDecodedData	The output descriptor pointer where the escaped decoded data 
	is placed.
	@return			An error code of KUriUtilsErr16BitChar if the data contains a 16-bit character. 
	KErrNone if the data was successfully encoded.
 */
template<class TDesCType, class TPtrType>
TInt EscapeDecodeData(const TDesCType& aData, TPtrType& aDecodedData)
	{
	// Go through the descriptor
	const TInt length = aData.Length();
	for( TInt i=0; i<length; ++i )
		{
		// See if at start of an escape triple
		TChar current = aData[i];
		if( current == KEscapeIndicator )
			{
			TInt hex;
			if( !CheckAndConvertEscapeTriple(aData.Mid(i), hex) )
				{
				// Either of the nibbles were not a valid hex character
				return KUriUtilsErrBadEscapeTriple;
				}
			// Append hex value
			aDecodedData.Append(hex);

			// Move index to get next character - add 2 to index
			i += 2;
			}
		else
			{
			// Not an escaped triple - just append
			aDecodedData.Append(current);
			}
		}
	return KErrNone;
	}

/**
	Checks for an escape triple at the start of the input descriptor. If there is a triple its 
	value is calculated and returned through the output argument aHexValue. If there is no escape 
	then triple this argument is left unchanged.
						
	@since			6.0
	@param			aData		The descriptor to be checked for an escape triple.
	@param			aHexValue	The output argument with the value of the escape triple 
	if it exists.
	@return			A boolean value of ETrue if there is an escape triple at the start 
	of the input descriptor, EFalse otherwise.
 */
template<class TDesCType>
TBool CheckAndConvertEscapeTriple(const TDesCType& aData, TInt& aHexValue)
	{
	// See if the descriptor is actually long enough
	if( aData.Length() < KEscapeTripleLength )
		{
		return EFalse;
		}
	// Check that the three characters form an escape triple - first char is '%'
	if( aData[KEscDelimiterPos] != KEscapeIndicator )
		{
		return EFalse;
		}
	// Descriptor to hex digits and excluded chars
	const TDesC& KHexChars = KHexDigit;

	// Check that next two characters are valid
	TChar mostSignificantNibble = aData[KMostSignificantNibblePos];
	TChar leastSignificantNibble = aData[KLeastSignificantNibblePos];

	TInt mostSignificantNibbleValue = KHexChars.LocateF(mostSignificantNibble);
	TInt leastSignificantNibbleValue = KHexChars.LocateF(leastSignificantNibble);

	if( mostSignificantNibbleValue == KErrNotFound || leastSignificantNibbleValue == KErrNotFound )
		{
		// Either of the nibbles were not a valid hex character
		return EFalse;
		}
	// Convert characters into hex value and return
	aHexValue = 0x10*mostSignificantNibbleValue + 0x01*leastSignificantNibbleValue;
	return ETrue;
	}
