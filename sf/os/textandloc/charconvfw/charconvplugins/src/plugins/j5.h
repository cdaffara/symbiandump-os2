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


#if !defined(__J5_H__)
#define __J5_H__

/**
J5 charconv character converter which supports automatic conversion for the following
character sets:
	SHIFT-JIS (Windows-31J Microsoft code page 932), 
	ISO-2022-JP-1 (JIS-JP) 
	EUC_JP 
	UTF8 
	UCS2
@internalTechnology 
@released 9.1
*/
class CJ5Converter : public CCharacterSetConverterPluginInterface
	{

public:
	enum TJ5Encoding
		{
		EShiftjis = 1,
		EIso2022jp1, 
		EEucjp, 
		EUtf8, 
		EUcs2,
		EUnknown
		};

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

	static CJ5Converter* NewL();
	virtual ~CJ5Converter();

private:
	CJ5Converter();
	void ConstructL();
	
	enum TDectectCharacterSet
		{
		EIsCharacterSet = 1,
		EIsNotCharacterSet,
		EMaybeCharacterSet
		};
			
	enum TJ5Encoding DetectEncoding(
		CCnvCharacterSetConverter::TEndianness& aDefaultEndiannessOfForeignCharacters, 
		const TDesC8& aForeign );

	TBool DetectUcs2( const TDesC8& aForeign,
		CCnvCharacterSetConverter::TEndianness& aTEndianness );
		
	enum TDectectCharacterSet DetectShiftJis( const TDesC8& aForeign,
		TInt &aNumberOfBytesConverted );
	
	enum TDectectCharacterSet DetectUtf8( const TDesC8& aForeign );

	enum TDectectCharacterSet DetectEucJp( const TDesC8& aForeign,
		TInt &aNumberOfBytesConverted );

	enum TDectectCharacterSet DetectIso2022( const TDesC8& aForeign );
		
	TInt ConvertEEucjpToUnicode(
		CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters,
		TDes16& aUnicode, 
		const TDesC8& aForeign, 
		TInt& aState, 
		TInt& aNumberOfUnconvertibleCharacters, 
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	
	TInt ConvertUcs2ToUnicode(CCnvCharacterSetConverter::TEndianness& aDefaultEndiannessOfForeignCharacters, 
						   TDes16& aUnicode,		
						   const TDesC8& aForeign,	
						   TInt& aNumberOfUnconvertibleCharacters,   
						   TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter); 
									  
	};

	
							  
// EucJp conversion methods	
void DummyConvertToIntermediateBufferInPlace(TDes8&);
void ConvertToJisX0212FromEucJpPackedInPlace(TDes8& aDescriptor);
void ConvertToJisX0208FromEucJpPackedInPlace(TDes8& aDescriptor);
void ConvertToHalfWidthKatakana8FromEucJpPackedInPlace(TDes8& aDescriptor);
TInt NumberOfBytesAbleToConvertToJisRoman(const TDesC8& aDescriptor);
TInt NumberOfBytesAbleToConvertToJisX0208(const TDesC8& aDescriptor);
TInt NumberOfBytesAbleToConvertToHalfWidthKatakana8(const TDesC8& aDescriptor);
TInt NumberOfBytesAbleToConvertToJisX0212(const TDesC8& aDescriptor);

#endif

