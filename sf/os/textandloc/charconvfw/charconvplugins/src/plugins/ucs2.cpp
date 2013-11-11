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
#include <convgeneratedcpp.h>
#include "ucs2.h"
#include <ecom/implementationproxy.h>
#include <charactersetconverter.h>

class CUCS2ConverterImpl : public CCharacterSetConverterPluginInterface
	{

public:
	virtual const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();

	virtual TInt ConvertFromUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
		TDes8& aForeign, 
		const TDesC16& aUnicode, 
		CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters);

	virtual TInt ConvertToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);

	virtual TBool IsInThisCharacterSetL(
		TBool& aSetToTrue, 
		TInt& aConfidenceLevel, 
		const TDesC8& aSample);

	static CUCS2ConverterImpl* NewL();
	virtual ~CUCS2ConverterImpl();

private:
	CUCS2ConverterImpl();

	};



// The following code has been copied and modified from the plugin computer generated code
// that is generated from the charconv/data/???.cpl && /???.txt files
// *** code begins ***
#define ARRAY_LENGTH(aArray) (sizeof(aArray)/sizeof((aArray)[0]))

#pragma warning (disable: 4049) // compiler limit : terminating line number emission

_LIT8(KLit8ReplacementForUnconvertibleUnicodeCharacters, "\xff\xfd");

GLDEF_C const TDesC8& ReplacementForUnconvertibleUnicodeCharacters_internal()
	{
	return KLit8ReplacementForUnconvertibleUnicodeCharacters;
	}

GLDEF_D const SCnvConversionData conversionData=
	{
	SCnvConversionData::EUnspecified,
	{NULL,NULL},
	{NULL,NULL},
	{NULL,NULL},
	NULL,
	NULL
	};


const TInt    KByteOrderMark = 0xfeff;
const TInt 	  KStateOffset = 0x1000;


const TDesC8& CUCS2ConverterImpl::ReplacementForUnconvertibleUnicodeCharacters()
	{
	return ReplacementForUnconvertibleUnicodeCharacters_internal();
	}


/**
 * Takes a 16 bit UCS2 descriptor with or without BOM and translates it to an
 * eight bit descriptor in Big Endian format.
 *
 * Note aDefaultEndiannessOfForeignCharacters is not used by this converter
 *
 * @param CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters
 * @param const TDesC8&
 * @param TDes8& aForeign
 * @param const TDesC16& aUnicode
 * @param CCnvCharacterSetConverter::TArrayOfAscendingIndices&
 *
 * returns number of converted characters
*/
TInt CUCS2ConverterImpl::ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness /*aDefaultEndiannessOfForeignCharacters*/, 
								 const TDesC8& /*aReplacementForUnconvertibleUnicodeCharacters*/, 
								 TDes8& aForeign, 
								 const TDesC16& aUnicode, 
								 CCnvCharacterSetConverter::TArrayOfAscendingIndices& /*aIndicesOfUnconvertibleCharacters*/)
	{
	TInt numberOfUnicodeCharacters =0;
	TInt nextChar;
	
	// start at the begining of the output buffer
	aForeign.Zero();
	
	// while there is unicode data to convert and space in the output buffer
	while ( (aForeign.Length() + 1 < aForeign.MaxLength()) && (numberOfUnicodeCharacters < aUnicode.Length()) )
		{
		nextChar = aUnicode[numberOfUnicodeCharacters];

		// Note - this always converts to EBigEndian 
		aForeign.Append((nextChar & 0xff00) >> 8);
		aForeign.Append(nextChar & 0xff );
			
		numberOfUnicodeCharacters++;
		}
		
	// returns the number of unconverted characters left at the end of the input descriptor  
	return aUnicode.Length() - numberOfUnicodeCharacters;
	}

/**
 * Takes an 8 bit descriptor with or without a BOM and translates it to unicode 
 * Input endiness is determined by Byte Order Markers (BOM) in the source text.
 * If no BOM is present aDefaultEndiannessOfForeignCharacters is used.
 *
 * When the data is too large to fit in the output buffer, the endiness is saved in the state
 * variable between conversions
 *
 * @param aDefaultEndiannessOfForeignCharacters Default endiness if no BOMs present in the source
 * @param aUnicode Contains the converted text in the Unicode character set
 * @param aForeign The non-Unicode source text to be converted
 * @param aState Not used by this converter
 * @param aNumberOfUnconvertibleCharacters Contains the number of bytes which could not be converted to unicode
 * @param aIndexOfFirstByteOfFirstUnconvertibleCharacter The index of the first unconvertable byte or -1 if all converted.
 *
 * @return aNumberOfUnconvertibleCharacters The number of unconverted bytes left at the end of the input 
 * descriptor (e.g. because the output descriptor is not long enough to hold all the text), or one of the 
 * error values defined in TError. 
 * @internalTechnology 
 */
TInt CUCS2ConverterImpl::ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
						   TDes16& aUnicode,	
						   const TDesC8& aForeign,
						   TInt& aState,
						   TInt& aNumberOfUnconvertibleCharacters, 
						   TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter) 
	{
	TInt numberOfBytesConverted = 0;
	TInt numberOfUnicodeCharacters =0;
	TChar nextChar;
	
	// work out what byte order to use
	CCnvCharacterSetConverter::TEndianness byteOrderMark;
	if ( aState==CCnvCharacterSetConverter::KStateDefault )
		{
		// this is the first call so use the default or BOM for byte order
		byteOrderMark = aDefaultEndiannessOfForeignCharacters;
		}
	else
		{
		// this is not the first call so use the saved byte order
		byteOrderMark = STATIC_CAST( CCnvCharacterSetConverter::TEndianness, aState - KStateOffset );
		}
		
	if ( aForeign.Length() < 2)
		{ // too small to do anything with		
		return -1;
		}
	// If the state is KStateDefault (this is the first call) check for BOM markers
	else if (aState==CCnvCharacterSetConverter::KStateDefault)
		{
		// is there a Little Endian BOM
		if (aForeign[0]==0xff && aForeign[1]==0xfe )
			{ 
			byteOrderMark = CCnvCharacterSetConverter::ELittleEndian; 
			}
		else if (aForeign[0]==0xfe && aForeign[1]==0xff )
			{
			byteOrderMark = CCnvCharacterSetConverter::EBigEndian; 
			}
		// remember the detected state
		aState = byteOrderMark + KStateOffset;
		}

	// start at begining of the output buffer provided
	aUnicode.Zero();
	
	// while there is at least 2 bytes of data to convert and space in the output buffer
	while ( (numberOfBytesConverted+1 < aForeign.Size()) && (numberOfUnicodeCharacters < aUnicode.MaxLength()) )
		{
		if (byteOrderMark == CCnvCharacterSetConverter::ELittleEndian )
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
	
	// there are no uncovertable characters with UCS2,
	aNumberOfUnconvertibleCharacters = 0;
	// a negative value indicates that all characters converted
	aIndexOfFirstByteOfFirstUnconvertibleCharacter = -1;
			
	// returns the number of unconverted bytes left at the end of the input descriptor 
	// Note there could be 1 byte left over if an odd number of bytes provided for conversion
	return aForeign.Size() - numberOfBytesConverted;
	}


/**
 * This converter does not support autodetect so always returns a confidence value of 0.
 * @internalTechnology 
 */
TBool CUCS2ConverterImpl::IsInThisCharacterSetL(TBool& aSetToTrue, TInt& aConfidenceLevel, const TDesC8&)
	{
	aSetToTrue=ETrue;
	aConfidenceLevel=0;
	return EFalse;
	}

CUCS2ConverterImpl* CUCS2ConverterImpl::NewL()
	{
	CUCS2ConverterImpl* self = new(ELeave) CUCS2ConverterImpl();
	return self;
	}

CUCS2ConverterImpl::~CUCS2ConverterImpl()
	{
	}

CUCS2ConverterImpl::CUCS2ConverterImpl()
	{
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x101FF492,	CUCS2ConverterImpl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

 
