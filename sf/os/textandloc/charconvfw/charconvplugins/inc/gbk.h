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


#ifndef __GBK_H__
#define __GBK_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__CHARCONV_H__)
#include <charconv.h>
#endif

class CnvGbk 
/**
only to be used by CHARCONV plug-in DLLs and by locale-DLLs (Locl::FatUtilityFunctions)
@internalComponent
*/
	{
public:
	IMPORT_C static const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();
	IMPORT_C static TInt ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, TDes8& aForeign, const TDesC16& aUnicode, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters);
	IMPORT_C static TInt ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, TDes8& aForeign, const TDesC16& aUnicode, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters, TUint& aOutputConversionFlags, TUint aInputConversionFlags);
	IMPORT_C static TInt ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	IMPORT_C static TInt ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, TDes16& aUnicode, const TDesC8& aForeign, TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, TUint& aOutputConversionFlags, TUint aInputConversionFlags);
	IMPORT_C static TBool IsCharGBBased(TInt& aConfidenceLevel, const TDesC8& aSample);
	IMPORT_C static const SCnvConversionData& ConversionData();
	};

#endif

