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
* J5 charconv character converter
*
*/


#include <e32std.h>
#include <charconv.h>
#include <ecom/implementationproxy.h>
#include <utf.h>
#include <charactersetconverter.h>
#include <convutils.h>
#include "shiftjis.h"
#include "jisbase.h"
#include "j5.h"

#include "jisx0201.h"
#include "jisx0208.h"
#include "jisx0212.h"

#include "featmgr/featmgr.h"

/**
 J5 will use up to KMaxSizeAutoDetectSample to try to deterine the format of data.
 */
const TInt KMaxSizeAutoDetectSample = 1000;

const TUint8 KEscape = 0x1b;
const TInt KByteOrderMark = 0xfeff;

const TDesC8& CJ5Converter::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return CnvShiftJis::ReplacementForUnconvertibleUnicodeCharacters();
	}

/**
 This API should not be used as it is ambiguous as to what encoding is required.  
 The user should instead call the specific plug-in for the appropriate conversion.
 J5 ConvertFromUnicode() will convert to UTF8 as default.
@internalTechnology 
 */
TInt CJ5Converter::ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness /* aDefaultEndiannessOfForeignCharacters */, 
		const TDesC8& /* aReplacementForUnconvertibleUnicodeCharacters */, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& /* aIndicesOfUnconvertibleCharacters */)
	{
	return CnvUtfConverter::ConvertFromUnicodeToUtf8(aForeign, aUnicode);
	}

/**
 This will automatically determine one of the five supported encodings 
 to use and convert accordingly.  This plugin method is available to the 
 user though the CCnvCharacterSetConverter::ConvertToUnicode() method.  
 There is no way for the caller to determine which encoding has been used.
 
 NOTE: For debugging the selected character set is returned in the state.
 
  @released  9.1
  @param     aDefaultEndiannessOfForeignCharacters The default endian-ness to use when reading characters
             in the foreign character set.
  @param     aUnicode On return, contains the text converted into Unicode.
  @param     aForeign The non-Unicode source text to be converted.
  @param     aState Used to save state information across multiple calls
             to <code>ConvertToUnicode()</code>.
  @param     aNumberOfUnconvertibleCharacters On return, contains the number of bytes which were not
             converted.
  @param     aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, contains the index of the first bytein the
             input text that could not be converted. A negative
             value indicates that all the characters were
             converted.
  @return 	 The number of unconverted bytes left at the end of the input descriptor 
 		     (e.g. because the output descriptor is not long enough to hold all the text), 
 		     or one of the error values defined in TError. 
  @internalTechnology 
*/
TInt CJ5Converter::ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	// As the aState parameter is used to pass back the detected value
	// use a "hidden" internal state variable.
	TInt internalState = CCnvCharacterSetConverter::KStateDefault;
	
	// determine the encoding type and then decode appropriatly
	switch ( DetectEncoding(aDefaultEndiannessOfForeignCharacters, aForeign))
		{
		case EShiftjis:
			aState = EShiftjis;
			return CnvShiftJis::ConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, 
					aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);

		case EIso2022jp1: 
			aState = EIso2022jp1;
			return CnvJisBase::ConvertToUnicode(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, internalState,
					aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);

		case EEucjp: 
			aState = EEucjp;
			return ConvertEEucjpToUnicode(
					aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, internalState,
					aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);	

		case EUcs2:
			aState = EUcs2;
			return ConvertUcs2ToUnicode( aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, 
					aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);

		case EUtf8: 
			aState = EUtf8;
			return CnvUtfConverter::ConvertToUnicodeFromUtf8(aUnicode, aForeign);
			
		default:
			// fall though to the default, which is decode as UTF8
			aState = EUnknown;
			break;
		}

	// decode as UTF8
	return CnvUtfConverter::ConvertToUnicodeFromUtf8(aUnicode, aForeign);
	}

/**
 This API is used by CCnvCharacterSetConverter::AutoDetectCharacterSetL(). 
 This method returns a value between 0 and 100, indicating how likely it 
 is that this is the correct converter, for the text supplied.  As J5 is 
 NOT intended to be used with the existing auto-detect mechanism, it will 
 always return 0
 @internalTechnology 
 */
TBool CJ5Converter::IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& /* aSample */)
	{
  	/*
  	aSetToTrue - This value should be set to ETrue. It is used to indicate to 
  	CCnvCharacterSetConverter::AutoDetectCharacterSetL() that the plug-in DLL 
  	is implementing a function of this signature and is therefore not the empty 
  	*/
  	aSetToTrue=ETrue;
  	
 	/* no need to look at the sample as this always returns 0 
 	   as the autodetect feature is not supported by the J5 plug-in
 	*/
 	aConfidenceLevel=0;
	return ETrue;
	}

CJ5Converter* CJ5Converter::NewL()
	{
	CJ5Converter* self = new(ELeave) CJ5Converter();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);	
	return self;
	}

CJ5Converter::~CJ5Converter()
	{
    FeatureManager::UnInitializeLib();	
	}

CJ5Converter::CJ5Converter()
	{
	}

void CJ5Converter::ConstructL()
    {
    FeatureManager::InitializeLibL();
    }

const TImplementationProxy ImplementationTable[] = 
	{
#ifdef KDDIAU_TEST
		// for the test build use a special test UID
		IMPLEMENTATION_PROXY_ENTRY(0x01000002,	CJ5Converter::NewL)
#else
		IMPLEMENTATION_PROXY_ENTRY(KCharacterSetIdentifierJ5,	CJ5Converter::NewL)
#endif
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}
	
/**
 DetectEncoding determine the characterset encoding.
 The logic for this detection is based on the information in CJKV by Ken Lunde.
 A detailed diagram of this logic is in the J5 how to document section 2.4
 @return The detected character set as a enum CJ5Converter.
 @internalTechnology 
 */
enum CJ5Converter::TJ5Encoding CJ5Converter::DetectEncoding(
		CCnvCharacterSetConverter::TEndianness& aDefaultEndiannessOfForeignCharacters , 
		const TDesC8& aForeign)
	{
	
	// first check for UCS2
	CCnvCharacterSetConverter::TEndianness ucs2Endianness = CCnvCharacterSetConverter::ELittleEndian;
	if ( DetectUcs2(aForeign, ucs2Endianness ))
		{
		// if ucs2 is detected pass back the detected endianess
		aDefaultEndiannessOfForeignCharacters = ucs2Endianness;
		return EUcs2;
		}

	// next try EUC_JP
	TInt eucJpValidBytes = 0;
	CJ5Converter::TDectectCharacterSet result = DetectEucJp( aForeign, eucJpValidBytes );
	if ( result == EIsCharacterSet )
		{
		return EEucjp;
		}
		
	// next try Iso 2020JP
	if ( DetectIso2022( aForeign ) == EIsCharacterSet )
		{
		return EIso2022jp1;
		}
		
	// next try Utf8
	if ( DetectUtf8( aForeign ) == EIsCharacterSet )
		{
		return EUtf8;
		}
		
	// shiftjis
	TInt shiftjisValidBytes = 0;
	result = DetectShiftJis( aForeign, shiftjisValidBytes );
	if ( result == EIsCharacterSet )
		{
		return EShiftjis;
		}
		
	// no clear winner so go for the best 
	TInt sampleLength = Min(aForeign.Length(), KMaxSizeAutoDetectSample);
		
	// if more than half is shiftjis and more shiftjis than EUC_JP, 	
	if ((shiftjisValidBytes >  eucJpValidBytes ) && (shiftjisValidBytes * 2> sampleLength))
		return EShiftjis;

	// if more than half is EUC_JP and more EUC_JP than shiftjis, 	
	if ((eucJpValidBytes >  shiftjisValidBytes ) && (eucJpValidBytes * 2> sampleLength))
		return EEucjp;
			
	// return the default
	return EUcs2;
	}
	
	
/**
 Check if UCS2.
 If the first two bytes are the Unicode Endian Specifiers (0xfffe or 0xfeff)
 then this must be UCS2. Otherwise try lookiing for  0x**00 or 0x00**
 @param A sample of data to be checked
 @param The Endianness if USC2 is detected
 @return ETrue if UCS2 else EFalse
 @internalTechnology 
 */
TBool CJ5Converter::DetectUcs2( const TDesC8& aForeign, 
	CCnvCharacterSetConverter::TEndianness& aTEndianness )
	{
	// if the sample is not big enough
	if (aForeign.Length() < 2)
		{
		return EFalse;
		}
	else if (aForeign[0]==0xff && aForeign[1]==0xfe )
		{ 
		// we have found a Little Endian Byte order mark
		aTEndianness = CCnvCharacterSetConverter::ELittleEndian;
		return ETrue;
		}
	else if (aForeign[0]==0xfe && aForeign[1]==0xff )
		{ 
		// we have found a Big Endian Byte order mark 
		aTEndianness = CCnvCharacterSetConverter::EBigEndian;
		return ETrue;
		}

	// Next check for sequences of 0x**00 or 0x00** as UCS-2 is the only charset that 
	// specifies 0x**00 or 0x00** (according to endianness) for the ASCII range of characters. 
	// NB: This will fail if there are no ASCII characters in the text.
	TInt sampleLength = aForeign.Length();
	sampleLength = Min(aForeign.Length(), KMaxSizeAutoDetectSample);;

	// check the sample for sequences of 0x**00	or 0x00**
	TInt bigEndianConfidence = 0;
	TInt littleEndianConfidence = 0;
	TInt i=0;
	for(;i< (sampleLength-1); i+=2)
		{
		if( aForeign[i] == 0x00)
			{
			bigEndianConfidence +=2;
			}
		else if ( aForeign[i+1] == 0x00)
			{
	 		littleEndianConfidence +=2;
			}
		}

	// which occurs most BE or LE	
	TInt confidenceLevel = 0;
	if (bigEndianConfidence > littleEndianConfidence)
		{
		aTEndianness = CCnvCharacterSetConverter::EBigEndian;
		confidenceLevel = bigEndianConfidence;
		}
	else
		{
		aTEndianness = CCnvCharacterSetConverter::ELittleEndian;
		confidenceLevel = littleEndianConfidence;
		}
		
	// if more than 97% count as UCS2
	if ( confidenceLevel * 100/sampleLength > 97) 
		return ETrue;

	return EFalse;
	}	

/**
 Check if ShiftJis (reference CJKV by Ken Lunde page 175)
 @param A sample of data to be checked
 @param The number of input bytes that can be converted
 @return The result of the check as either EIsCharacterSet, EIsNotCharacterSet or EMaybeCharacterSet
 @internalTechnology 
 */
enum CJ5Converter::TDectectCharacterSet CJ5Converter::DetectShiftJis( const TDesC8& aForeign,TInt &aNumberOfBytesConverted )
	{
	// Get the sample length
	TInt sampleLength = Min(aForeign.Length(), KMaxSizeAutoDetectSample);;

	TInt i=0;
	aNumberOfBytesConverted = 0;
	
	TText8 character;
	TText8 characterPlus1;
	TText8 characterPlus2;
	
	// scan the sample text looking for valid shiftjis data
	while ( i < sampleLength )
		{
		// get the next few characters, use 0 if there is no more sample
		// as this will not match any test.
		character = aForeign[i];
		characterPlus1 = ( i < (sampleLength-1) ? aForeign[i+1]:0);
		characterPlus2 = ( i < (sampleLength-2) ? aForeign[i+2]:0);

		// SHIFTJIS	- 0x8e to 0x9f followed by 0x40 to 0xfc  
		if ((character >= 0x81) && (character <= 0x9f) &&
				(characterPlus1 >= 0x40) && (characterPlus1 <= 0xfc) ) 
			{
			// this is SHIFTJIS unless it is EUC JP code set 2 or 3
			if ((character == 0x8E) && (characterPlus1 >= 0xA1) && (characterPlus1 <= 0xDF))
				{
				// this could be EUC JP code set 2 (or shiftjis)
				aNumberOfBytesConverted+=2;
				i++;
				}
			else if ((character == 0x8F) && 
				(characterPlus1 >= 0xA1) && (characterPlus1 <= 0xDF) &&
					(characterPlus2 >= 0xA1) && (characterPlus2 <= 0xDF))
				{
				// this could be EUC JP code set 3 (or shiftjis)
				aNumberOfBytesConverted+=3;
				i+=2;
				}
			else
				{
				// this can only be shift jis 
				return EIsCharacterSet;
				}
			}
			
		// SHIFTJIS	- 0xE0 to 0xEF followed by .....
		else if ((character >= 0xE0) && (character <= 0xEF))
			{
			// 0x40 to 0xFC which overlaps UTF8 between 0x80 and 0xBF  
			// including Mopera extension to shiftjis from 0xEF80 to 0xEFFC
			
			if ( (characterPlus1 >= 0x40) && (characterPlus1 <= 0x7E) ) 
				{
				// this can only be shift jis 
				return EIsCharacterSet;
				}
			else if ( (characterPlus1 >= 0xC0) && (characterPlus1 <= 0xFC) ) 
				{
				// this could be EUC JP code set 1
				aNumberOfBytesConverted+=2;
				i++;
				}
				
			// problem here is the overlap between the UTF8 and shiftjis
			else if ( (characterPlus1 >= 0x80) && (characterPlus1 <= 0xBF) )
				{
				// this could be shiftjis or utf8
				aNumberOfBytesConverted+=2;
				i++;
				}		
			}
		// half width katakana A1-DF	
		else if ((character >= 0xA1) && (character <= 0xDF))
			{
			aNumberOfBytesConverted+=1;
			}
		// ASCII or JIS-Roman 20-7e	
		else if ( ((character >= 0x20) && (character <= 0x7E)) || (character == 0x0A) || (character == 0x0D))
			{
			aNumberOfBytesConverted+=1;
			}
		else
			{
			// This is not decoding as shiftjis, so reject
			aNumberOfBytesConverted =0;
			return EIsNotCharacterSet;
			}
		i++;
		}

	// if all the characters could be converted
	if (aNumberOfBytesConverted == sampleLength)
		{
		return EIsCharacterSet;
		}
	else if (aNumberOfBytesConverted == 0)
		{
		return EIsNotCharacterSet;
		}
	else
		{
		return EMaybeCharacterSet;
		}
	}
	
/**
 Check if UTF8 (reference CJKV by Ken Lunde page 189)
 @param A sample of data to be checked
 @param The number of input bytes that can be converted
 @return The result of the check as either EIsCharacterSet, EIsNotCharacterSet or EMaybeCharacterSet
 @internalTechnology 
 */
enum CJ5Converter::TDectectCharacterSet CJ5Converter::DetectUtf8( const TDesC8& aForeign )
	{
	// Get the sample length
	TInt sampleLength = Min(aForeign.Length(), KMaxSizeAutoDetectSample);;

	TInt i=0;	
	TText8 character;
	TText8 characterPlus1;
	TText8 characterPlus2;
	TText8 characterPlus3;
	
	// scan the sample text looking for valid UTF8
	while ( i < sampleLength )
		{
		// get the next few characters, use 0 if there is no more sample
		// as this will not match any test.
		character = aForeign[i];
		characterPlus1 = ( i < (sampleLength-1) ? aForeign[i+1]:0);
		characterPlus2 = ( i < (sampleLength-2) ? aForeign[i+2]:0);
		characterPlus3 = ( i < (sampleLength-3) ? aForeign[i+3]:0);

		// UTF8 range 110xxxxx followed by one valid UTF8 bytes
		if(((character & 0xe0)==0xc0) && (( characterPlus1 & 0xc0)==0x80) )
			{
			// two bytes of valid UTF8 found
			i+=2;
			}
		// UTF8 range 1110xxxx followed by two valid UTF8 bytes
		else if(((character & 0xf0)==0xe0) && (( characterPlus1 & 0xc0)==0x80) && (( characterPlus2 & 0xc0)==0x80))
			{
			// three bytes of valid UTF8 found
			i+=3;
			}
		// UTF8 range 11110xxx followed by three valid UTF8 bytes
		else if(((character & 0xf8)==0xf0) && (( characterPlus1 & 0xc0)==0x80) 
				&& (( characterPlus2 & 0xc0)==0x80) && (( characterPlus3 & 0xc0)==0x80) )
			{
			// four bytes of valid UTF8 found
			i+=4;
			}
		
		// ascii range 0 to 0x7F	
		else if((character & 0x80)==0x00)
			{
			// The value of character is in the range 0x00-0x7f
			// UTF8 maintains ASCII transparency. So it's a valid UTF8.
			i++;
			}
		// if the sample data is longer than KMaxSizeAutoDetectSample then except anything
		// for the last two bytes as they may not appear valid without more data	
		else if( i >= (KMaxSizeAutoDetectSample -2) )
			{
			i++;
			}
		else
			{
			// This is not decoding as UTF8 so reject
			return EIsNotCharacterSet;
			}
		}	
	
	// All the characters could be converted
	return EIsCharacterSet;
	
	}


/**
 Check if ISO2022JP by lookiing for the escape sequences.
 @param A sample of data to be checked
 @param The number of input bytes that can be converted
 @return The result of the check as either EIsCharacterSet, EIsNotCharacterSet or EMaybeCharacterSet
 @internalTechnology 
 */
enum CJ5Converter::TDectectCharacterSet CJ5Converter::DetectIso2022( const TDesC8& aForeign )
	{
	// Get the sample length
	TInt sampleLength = Min(aForeign.Length(), KMaxSizeAutoDetectSample);;

	TInt i=0;
	TText8 character;
	TText8 characterPlus1;
	TText8 characterPlus2;
	TText8 characterPlus3;
	TText8 characterPlus4;
	TText8 characterPlus5;
	
	// scan the sample text looking for valid UTF8
	while ( i < sampleLength )
		{
		// get the next few characters, use 0 if there is no more sample
		// as this will not match any test.
		character = aForeign[i];
		characterPlus1 = ( i < (sampleLength-1) ? aForeign[i+1]:0);
		characterPlus2 = ( i < (sampleLength-2) ? aForeign[i+2]:0);
		characterPlus3 = ( i < (sampleLength-3) ? aForeign[i+3]:0);


		// check for the JIS escape sequences of ISO 2022Jp
		// These values have been taken from JISBASE_SHARED
		if (character == KEscape)
			{
			// Escape Sequence For Jis C6226_1978 \x1b\x24\x40
			if ((characterPlus1 == 0x24) && (characterPlus2 == 0x40))
				{
				return EIsCharacterSet;
				}
				
			// Escape Sequence For Jis X0208_1983 \x1b\x24\x42
			else if ((characterPlus1 == 0x24) && (characterPlus2 == 0x42))
				{
				return EIsCharacterSet;
				}
			
			// Escape Sequence For Jis Roman \x1b\x28\x4a
			else if ((characterPlus1 == 0x28) && (characterPlus2 == 0x4A))
				{
				return EIsCharacterSet;
				}
				
			// Escape Sequence For Jis RomanIncorrect \x1b\x28\x48
			else if ((characterPlus1 == 0x28) && (characterPlus2 == 0x48))
				{
				return EIsCharacterSet;
				}

			// Escape Sequence For Ascii \x1b\x28\x42
			else if ((characterPlus1 == 0x28) && (characterPlus2 == 0x42))
				{
				return EIsCharacterSet;
				}
				
			// Escape Sequence For EscapeSequenceForHalfWidthKatakana \x1b\x28\x49
			else if ((characterPlus1 == 0x28) && (characterPlus2 == 0x49))
				{
				return EIsCharacterSet;
				}
				
			// Escape Sequence For Jis X0208_199x \x1b\x26\x40\x1b\x24\x42
			else if ((characterPlus1 == 0x26) && (characterPlus2 == 0x40))
				{
				characterPlus4 = ( i < (sampleLength-4) ? aForeign[i+4]:0);
				characterPlus5 = ( i < (sampleLength-5) ? aForeign[i+5]:0);

				if ((characterPlus3 == 0x1b) && (characterPlus4 == 0x24) && (characterPlus5 == 0x42))
					{
					return EIsCharacterSet;
					}
				}
			// Escape Sequence For Jis X0212_1990 \x1b\x24\x28\x44
			else if ((characterPlus1 == 0x24) && (characterPlus2 == 0x28)) 
				{
				if (characterPlus3 == 0x44)
					{
					return EIsCharacterSet;
					}
				}
				
			// check for the JIS escape sequences of ISO 2022Jp "B@" x42 x40
			else if ((characterPlus1 == 'B') || (characterPlus1 == '@'))
				{
				return EIsCharacterSet;
				}
				
			} // end of if ( character == KEscape )

		i++;
		}	

	// if escape sequences have been found then this is not ISO2022
	return EIsNotCharacterSet;
	
	}


/**
 Check if EUC JP (reference CJKV by Ken Lunde page 164)
 @param A sample of data to be checked
 @param The number of input bytes that can be converted
 @return The result of the check as either EIsCharacterSet, EIsNotCharacterSet or EMaybeCharacterSet
 @internalTechnology 
 */
CJ5Converter::TDectectCharacterSet CJ5Converter::DetectEucJp( const TDesC8& aForeign,TInt &aNumberOfBytesConverted )
	{
	// Get the sample length
	TInt sampleLength = Min(aForeign.Length(), KMaxSizeAutoDetectSample);;

	TInt i=0;
	aNumberOfBytesConverted = 0;
	
	TText8 character;
	TText8 characterPlus1;
	TText8 characterPlus2;
	
	// scan the sample text looking for valid shiftjis data
	while ( i < sampleLength )
		{
		// get the next few characters, use 0 if there is no more sample
		// as this will not match any test.
		character = aForeign[i];
		characterPlus1 = ( i < (sampleLength-1) ? aForeign[i+1]:0);
		characterPlus2 = ( i < (sampleLength-2) ? aForeign[i+2]:0);

		// EUCJP code set 0 0x21-0x7e
		if ( (character >= 0x21) && (character <= 0x7e))
			{
			aNumberOfBytesConverted++;
			}
		else if ( (character == 0x0a) || (character == 0x0d))
			{
			aNumberOfBytesConverted++;
			}
		// EUCJP code set 1
		else if ( (character >= 0xa1) && (character <= 0xff)
				&& (characterPlus1 >= 0xa1) && (characterPlus1 <= 0xff) ) 
			{
			aNumberOfBytesConverted+=2;
			i++;
			}
		 		
		// EUC JP code set 2, starts with the EUC JP SS2 character (0x8E)
		// and is followed by character in range 0xA1- 0xDF
		else if ((character == 0x8E) && (characterPlus1 >= 0xA1) && (characterPlus1 <= 0xDF) ) 
			{
			// this could be 2 bytes of EUC JP code set 2
			aNumberOfBytesConverted += 2;
			i++;
			}
		// EUC JP code set 3, starts with the EUC JP SS3 character (0x8F)
		// and is followed by two characters in range A1- DF A1 -FE
		else if ((character == 0x8F) && (characterPlus1 >= 0xA1) && (characterPlus1 <= 0xDF) 
				&& (characterPlus2 >= 0xA1) && (characterPlus2 <= 0xDF))
			{
			// this could be 3 bytes of EUC JP code set 3
			aNumberOfBytesConverted += 3;
			i+=2;
			}		
		else
			{
			// This is not a valid decoding as EUC JP so reject
			return EIsNotCharacterSet;
			}
		i++;
		}	
	
	
	// if all the characters could be converted
	if (aNumberOfBytesConverted == sampleLength)
		{
		return EIsCharacterSet;
		}
	else if (aNumberOfBytesConverted == 0)
		{
		return EIsNotCharacterSet;
		}
	else
		{
		return EMaybeCharacterSet;
		}
	}

			
/**
 Convert from UCS2 (Universal Character Set containing two bytes) to unicode
 Remove any byte order marks in the UCSs.
 @param aUnicode Contains the converted text in the Unicode character set.
 @param	aForeign The non-Unicode source text to be converted
 @param aNumberOfUnconvertibleCharacters Contains the number of bytes which were not converted. 
 @param aIndexOfFirstByteOfFirstUnconvertibleCharacter The index of the first byte of the first unconvertible character.
 @return the number of bytes converted
 @internalTechnology 
 */
 TInt CJ5Converter::ConvertUcs2ToUnicode(CCnvCharacterSetConverter::TEndianness& aDefaultEndiannessOfForeignCharacters, 
						   TDes16& aUnicode,	 
						   const TDesC8& aForeign, 
						   TInt& aNumberOfUnconvertibleCharacters,  
						   TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter) 

	{
	TInt numberOfBytesConverted = 0;
	TInt numberOfUnicodeCharacters =0;
	TChar nextChar;

	// start at begining of the output buffer provided
	aUnicode.Zero();

	// while there is at least 2 bytes of data to convert and space in the output buffer
	while ( (numberOfBytesConverted+1 < aForeign.Size()) && (numberOfUnicodeCharacters < aUnicode.MaxLength()) )
		{
		if (aDefaultEndiannessOfForeignCharacters == CCnvCharacterSetConverter::ELittleEndian )
			{
			// ELittleEndian 0x??00
			nextChar = aForeign[numberOfBytesConverted] + ( aForeign[numberOfBytesConverted+1] << 8);
			}
		else
			{
			// EBigEndian 0x00??
			nextChar = ( aForeign[numberOfBytesConverted] <<8 ) + aForeign[numberOfBytesConverted+1];
			}
			
		// save the unicode character extracted	unless it's a BOM
		if ( nextChar != KByteOrderMark )
			{
			aUnicode.Append( nextChar );
			numberOfUnicodeCharacters++;	
			}
			
		numberOfBytesConverted+=2;
		}
	
	// there are no uncovertable characters with UCS2, but there could be
	aNumberOfUnconvertibleCharacters = 0;
	// a negative value indicates that all characters converted
	aIndexOfFirstByteOfFirstUnconvertibleCharacter = -1;
				
	// returns the number of unconverted bytes left at the end of the input descriptor 
	// Note there could be 1 byte left over if an odd number of bytes provided for conversion
	return aForeign.Size() - numberOfBytesConverted;
	}
		
/**
 Convert from EUC_JP (Extended Unix Code encoding for Japanese)
 Using the standard Charconv method of an array of methods
 @return the number of bytes converted
 @internalTechnology 
 */
 TInt CJ5Converter::ConvertEEucjpToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& /*aState*/, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{
	TFixedArray<CnvUtilities::SMethod, 4> methods;
	methods[0].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToJisRoman;
	methods[0].iConvertToIntermediateBufferInPlace=DummyConvertToIntermediateBufferInPlace;
	methods[0].iConversionData=&CnvJisRoman::ConversionData();
	methods[0].iNumberOfBytesPerCharacter=1;
	methods[0].iNumberOfCoreBytesPerCharacter=1;
	methods[1].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToJisX0208;
	methods[1].iConvertToIntermediateBufferInPlace=ConvertToJisX0208FromEucJpPackedInPlace;
	methods[1].iConversionData=&CnvJisX0208::ConversionData();
	methods[1].iNumberOfBytesPerCharacter=2;
	methods[1].iNumberOfCoreBytesPerCharacter=2;
	methods[2].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToHalfWidthKatakana8;
	methods[2].iConvertToIntermediateBufferInPlace=ConvertToHalfWidthKatakana8FromEucJpPackedInPlace;
	methods[2].iConversionData=&CnvHalfWidthKatakana8::ConversionData();
	methods[2].iNumberOfBytesPerCharacter=2;
	methods[2].iNumberOfCoreBytesPerCharacter=1;
	methods[3].iNumberOfBytesAbleToConvert=NumberOfBytesAbleToConvertToJisX0212;
	methods[3].iConvertToIntermediateBufferInPlace=ConvertToJisX0212FromEucJpPackedInPlace;
	methods[3].iConversionData=&CnvJisX0212::ConversionData();
	methods[3].iNumberOfBytesPerCharacter=3;
	methods[3].iNumberOfCoreBytesPerCharacter=2;
	return CnvUtilities::ConvertToUnicodeFromHeterogeneousForeign(aDefaultEndiannessOfForeignCharacters, aUnicode, aForeign, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter, methods.Array());
	}		

