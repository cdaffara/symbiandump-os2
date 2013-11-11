/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __JISBASE_H__
#define __JISBASE_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__CHARCONV_H__)
#include <charconv.h>
#endif

class CnvJisBase
/**
@internalComponent
*/
	{
public:
	IMPORT_C static void IsCharacterJISBased(TInt& aConfidenceLevel, const TDesC8& aSample);
	IMPORT_C static TInt ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt& aState, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	IMPORT_C static const SCnvConversionData& HalfWidthKatakana7ConversionData();
private:
	static TInt ChangeToNonStandardStateJis7(TInt aState);
	static TInt ChangeToNonStandardStateJis8(TInt aState);
	static TInt ChangeToStandardState(TInt aState);
	static TInt AppendConvertToUnicodeFromModalForeign(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aModalForeign, TInt& aState, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, const TArray<CnvUtilities::SState>& aArrayOfStates, TUint& aOutputConversionFlags, TUint aInputConversionFlags);
	static TInt AppendConvertToUnicodeFromJis7(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aJis7, TInt&, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, const TArray<CnvUtilities::SState>&, TUint& aOutputConversionFlags, TUint aInputConversionFlags);
	static TInt AppendConvertToUnicodeFromJis8(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aJis8, TInt&, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, const TArray<CnvUtilities::SState>&, TUint& aOutputConversionFlags, TUint aInputConversionFlags);
	};

#endif

