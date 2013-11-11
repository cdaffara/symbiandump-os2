/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <charconv.h>
#include <convutils.h>
#include "jisx0201.h"
#include "jisx0208.h"
#include "shiftjis.h"
#include <convdata.h>
#include "charconv_tls.h"
#include "charconv_table_utilities.h"

// Lead byte ranges for 2-Byte Shift-JIS sequences
const TUint KSingleByteRangeFirstBlockEnd=0x80;
const TUint KSingleByteRangeSecondBlockStart=0xa0;
const TUint KSingleByteRangeSecondBlockEnd=0xdf;
const TUint KLeadByteMax = 0xfc;

//Trail byte ranges for 2-Byte Shift-JIS sequences
const TUint KTrailByteMin = 0x40 ;
const TUint KTrailByteMax = 0xFC ;
const TUint KTrailByteIllegal = 0x7F ;

const TUint8 KSJISLineFeed = 0x0a ;	// Shift-Jis value for Line Feed
const TUint8 KSJISCarriageReturn = 0x0d;	// Shift-Jis value for Carriage Return
const TUint16 KUnicodeLineFeed = 0x2028; // Unicode Line Feed
const TUint16 KUnicodeParagraphSeperator = 0x2029; // Unicode Paragraph seperator
const TUint16 KUnicodeCarriageReturn = KSJISCarriageReturn ;


/** 
 * Get the Shift-JIS replacement for Unicode characters which cannot be converted
 * 
 * @return const TDesC8& 8-bit descriptor containing the Shift-JIS data which will replace any untranslatable Unicode characters.
 * @since Internationalization_6.2
 * @internalTechnology
 */
EXPORT_C const TDesC8& CnvShiftJis::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return ReplacementForUnconvertibleUnicodeCharacters_internal();
	}


/**
 * Converts text from Unicode to Shift-JIS 
 * 
 * @since Internationalization_6.2
 * @internalTechnology 
 */
EXPORT_C TInt CnvShiftJis::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness /* aDefaultEndiannessOfForeignCharacters */,
                                              const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, TDes8& aForeign,
                                              const TDesC16& aUnicode,
                                              CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	return DoConvertFromUnicode(aReplacementForUnconvertibleUnicodeCharacters, 
	                            aForeign, aUnicode, aIndicesOfUnconvertibleCharacters) ;
	}

/**
 * Converts text from Unicode to Shift-JIS 
 * Note that CnvShiftJis does NOT support extensions through use of additional character sets,
 * instead multiple versions are built for the different (operator defined) implementations
 * of Shift-JIS which need to be supported and the correct one installed at ROM build time,
 * and this method is only supplied for purposes of interface compatibility. 
 * @since Internationalization_6.2
 * @internalTechnology
 */
EXPORT_C TInt CnvShiftJis::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness /* aDefaultEndiannessOfForeignCharacters */, 
                                              const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
                                              TDes8& aForeign, const TDesC16& aUnicode, 
                                              CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, 
                                              const TArray<CnvUtilities::SCharacterSet>& /* aArrayOfAdditionalCharacterSets */)
	{
	return DoConvertFromUnicode(aReplacementForUnconvertibleUnicodeCharacters, 
	                            aForeign, aUnicode, aIndicesOfUnconvertibleCharacters) ;
	}

EXPORT_C TInt CnvShiftJis::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness /* aDefaultEndiannessOfForeignCharacters */,
                                            TDes16& aUnicode, const TDesC8& aForeign, TInt& aNumberOfUnconvertibleCharacters,
                                            TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	return DoConvertToUnicode(aUnicode, aForeign, aNumberOfUnconvertibleCharacters, 
                              aIndexOfFirstByteOfFirstUnconvertibleCharacter) ;
	}


/**
 * Converts text from Shift-JIS to Unicode 
 * 
 * @since Internationalization_6.2
 * @internalTechnology
 */
EXPORT_C TInt CnvShiftJis::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness /* aDefaultEndiannessOfForeignCharacters */, 
                                            TDes16& aUnicode, const TDesC8& aForeign, 
                                            TInt& aNumberOfUnconvertibleCharacters, 
                                            TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
                                            const TArray<CnvUtilities::SMethod>& /* aArrayOfAdditionalMethods */)
	{
	return DoConvertToUnicode(aUnicode, aForeign, aNumberOfUnconvertibleCharacters, 
                              aIndexOfFirstByteOfFirstUnconvertibleCharacter) ;

	}
	
	

/**
This function actually does the work of converting Shift-JIS input to unicode output.

@param TDes16& aUnicode The output buffer
@param const TDesC8& aForeign The input buffer
@param TInt& aNumberOfUnconvertibleCharacters Number of input characters which were processed but couldn't be converted. 
@param TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter Locates first untranslatable character in input buffer
@return CCnvCharacterSetConverter::EErrorIllFormedInput if the input was invalid, otherwise the number of bytes in the input buffer which weren't be processed (e.g. due to output buffer overflow). 
@internalTechnology
*/
	
TInt CnvShiftJis::DoConvertToUnicode(TDes16& aUnicode, const TDesC8& aForeign, 
                                     TInt& aNumberOfUnconvertibleCharacters, 
                                     TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	TUint foreignBytePointer = 0 ;
	TUint row ;
	TUint column ;
	TUint8 foreignCharByte ;
	TUint8 foreignCharStart ;
	TChar unicodeChar ;
	TBool finished = EFalse ;
	TInt charsConverted = 0 ;
	TUint16 unicodeUnmappedCharacter = getUnicodeUnmappedCharacter() ;
	TInt unicodeBufferLength = aUnicode.MaxLength() ;
	TInt foreignDataLength = aForeign.Length() ;
		
	aIndexOfFirstByteOfFirstUnconvertibleCharacter = -1 ;
	aNumberOfUnconvertibleCharacters = 0 ;

	// Check for valid input and output buffers
	if (( unicodeBufferLength == 0) || foreignDataLength == 0)
		{
		finished = ETrue ;		
		}
		
	// Reset output buffer	
	aUnicode.Zero() ;

	// Perform conversion		
	while (!finished)
		{
		foreignCharStart = foreignBytePointer ;
		foreignCharByte = aForeign[foreignBytePointer++] ;

		// Look for (and handle) CR/LF pairs in ShiftJis input stream.
		// It is a specific requirement from Symbian KK that CR/LF pairs
		// in the input stream be converted to Unicode LF characters 
		if((KSJISCarriageReturn == foreignCharByte) && (foreignBytePointer < foreignDataLength))
			{
			// check next byte
			if(KSJISLineFeed == aForeign[foreignBytePointer]) 
				{
				// CR code
				unicodeChar = KUnicodeLineFeed ;
				foreignBytePointer++ ;
				}
			else
				{					
				unicodeChar = KUnicodeCarriageReturn ;
				}
			}
		else
			{
			if (((foreignCharByte > KSingleByteRangeFirstBlockEnd) && 
			     (foreignCharByte < KSingleByteRangeSecondBlockStart)) ||
			    ((foreignCharByte > KSingleByteRangeSecondBlockEnd) &&
			     (foreignCharByte <= KLeadByteMax)))
				{
				if (foreignBytePointer < foreignDataLength)
					{
					// Potential 2 byte shiftJis character
					row = foreignCharByte ;
					column = aForeign[foreignBytePointer] ;
					if (((column <= KTrailByteMax) && (column >= KTrailByteMin)) && column != KTrailByteIllegal)
						{
						foreignBytePointer++ ;	
						unicodeChar = lookupUnicodeChar(row,column) ;	
						}
					else
						{
						unicodeChar = unicodeUnmappedCharacter ;
						}
					}
				else
					{
					// Only got the first byte of a 2 byte character
					// reset "read" pointer to beginning of character
					// and bail out!
					finished = ETrue ;
					foreignBytePointer-- ;
					continue ;
					}
				}
			else
				{
				//Probably a single byte shiftJis chracter
				row = 0 ;
				column = foreignCharByte ;
				unicodeChar = lookupUnicodeChar(row,column) ;
				}
			}
			

		// Check for unconvertible characters.
		if (unicodeChar == unicodeUnmappedCharacter)
			{
			if (aIndexOfFirstByteOfFirstUnconvertibleCharacter == -1)
				{
				aIndexOfFirstByteOfFirstUnconvertibleCharacter = foreignCharStart ;
				}
			aNumberOfUnconvertibleCharacters++ ;
			}
			
		// Append the converted (or not!) character to the output buffer
		aUnicode.Append(unicodeChar);
		charsConverted++ ;
		
		// Check for end of input buffer or output buffer full
		if ((charsConverted >= unicodeBufferLength) || (foreignBytePointer >= foreignDataLength))
			finished = ETrue ;
		}
	
	// Evaluate success of the operation and either return error code (currently just 
	// invalid input) or return number of un-processed characters in input buffer in
	// case of output buffer being filled before input fully consumed (0 means all
	// characters consumed)
	TInt returnValue ;
	if (foreignDataLength && !charsConverted)
		{
		// Input must contain at least one complete character to be considered valid Shift-JIS.
		returnValue = CCnvCharacterSetConverter::EErrorIllFormedInput ;
		}
	else
		{
		returnValue = foreignDataLength - foreignBytePointer;
		}
	return returnValue ;
	}
	


	
/**
This function actually does the work of converting converting unicode input to Shift-JIS  output.

@param const TDesC8& aReplacementForUnconvertibleUnicodeCharacters byte sequence to be be used as output for unicode characters which have no mapping defined.
@param TDes16& aUnicode The input buffer
@param const TDesC8& aForeign The output buffer
@return The number of unicode characters in the input buffer which weren't be processed (e.g. due to output buffer overflow). 
@internalTechnology
*/
TInt CnvShiftJis::DoConvertFromUnicode(const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
	                                   TDes8& aForeign, const TDesC16& aUnicode, 
	                                   CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters)
	{
	TUint unicodePointer = 0 ;
	TUint row ;
	TUint column ;
	TUint8 foreignCharByte ;
	TUint16 foreignChar ;
	TUint16 unicodeChar ;
	TBool finished = EFalse ;
	TInt charsConverted = 0 ;
	TInt unicodeLength = aUnicode.Length() ;
	TInt foreignMaxLength = aForeign.MaxLength() ;
	TUint16 foreignUnmappedCharacter = getForeignUnmappedCharacter() ;
	CCnvCharacterSetConverter::TDowngradeForExoticLineTerminatingCharacters downgradeForExoticLineTerminatingCharacters = CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed;
	TBool downgradeExoticLineTerminatingCharacters	= EFalse ;
	
	
	// Check for valid input and output buffers
	if ((unicodeLength == 0) ||  foreignMaxLength == 0)
		{
		finished = ETrue ;		
		}
	
	// If we've been called from an instance of CCnvCharacterSetConverter we can retrieve its state
	// from Thread Local Storage. This really isn't very nice but there's no other way we can get
	// hold of settings like downgrade for unicode line termination characters without breaking
	// compatibility with the existing plug-in interface!
	CCnvCharacterSetConverter* currentCharacterSetConverter = (CCnvCharacterSetConverter*)TTlsData::CurrentCharacterSetConverter();
	if (currentCharacterSetConverter)
		{
		downgradeForExoticLineTerminatingCharacters = currentCharacterSetConverter->GetDowngradeForExoticLineTerminatingCharacters() ;
		downgradeExoticLineTerminatingCharacters = ETrue ;
		}
		
	// Reset output buffer
	aForeign.Zero();
		
	// Process input buffer
	while (!finished)
		{
		
		// Look up foreign Char
		unicodeChar = aUnicode[unicodePointer] ;
		
		// Check for any downgrade of Unicode line endings characters required if we've got
		// a Unicode Line-Feed or Paragraph-Seperator character to deal with. 
		if (downgradeExoticLineTerminatingCharacters && 
		    ((unicodeChar==KUnicodeLineFeed) || (unicodeChar==KUnicodeParagraphSeperator)))
			{
			if (downgradeForExoticLineTerminatingCharacters == CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToCarriageReturnLineFeed)
				{
				if (aForeign.Length() < (foreignMaxLength - 1))
					{
					aForeign.Append(KSJISCarriageReturn) ;
					aForeign.Append(KSJISLineFeed) ;
					charsConverted++ ;
					}
					else
					{
					// Foreign buffer full!
					finished = ETrue;
					}
				}
			else if (downgradeForExoticLineTerminatingCharacters == CCnvCharacterSetConverter::EDowngradeExoticLineTerminatingCharactersToJustLineFeed)
				{
				if (aForeign.Length() < foreignMaxLength)
					{
					aForeign.Append(KSJISLineFeed) ;
					charsConverted++ ;
					}
					else
					{
					// Foreign buffer full!
					finished = ETrue;
					}
				}		
			}
		else
			{	
			row = unicodeChar / 256 ;
			column = unicodeChar % 256 ;		
			foreignChar = lookupForeignChar(row, column) ;		

			// Check for untranslatable character 
			if ((foreignChar == foreignUnmappedCharacter) &&
			    (aForeign.Length() < (foreignMaxLength - 1)))
				{
				aIndicesOfUnconvertibleCharacters.AppendIndex(unicodePointer) ;
				aForeign.Append(aReplacementForUnconvertibleUnicodeCharacters) ;
				}	
			else if ((foreignChar <= 0xFF) && (aForeign.Length() < foreignMaxLength))
				{
				// Single byte character
				foreignCharByte = (TUint8) foreignChar ;
				aForeign.Append(foreignCharByte) ;
				charsConverted++ ;
				}
			else if (aForeign.Length() < (foreignMaxLength - 1))
				{
				// Two byte character
				foreignCharByte = (TUint8) (foreignChar >> 8 ) ;
				aForeign.Append(foreignCharByte) ;
				foreignCharByte = (TUint8) (foreignChar & 0xFF) ;
				aForeign.Append(foreignCharByte) ;
				charsConverted++ ;
				}
			else
				{
				// Foreign buffer full!
				finished = ETrue;
				}
			}
		
		// Check for terminating condition (input buffer consumed or output buffer full)	
		if (!finished && (++unicodePointer >= unicodeLength))
			{
			finished = ETrue ;
			}
		}
	
	// Return number of input characters *not* processsed (will be zero unless output
	// buffer has been filled before all input consumed)	
	return unicodeLength - unicodePointer;
	}

