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


#if !defined(__SHIFTJIS_H__)
#define __SHIFTJIS_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__CHARCONV_H__)
#include <charconv.h>
#endif

#if !defined(__CONVUTILS_H__)
#include <convutils.h>
#endif

class CnvShiftJis 
/**
only to be used by CHARCONV plug-in DLLs and by locale-DLLs (Locl::FatUtilityFunctions)
@internalComponent
*/
	{
public:
	IMPORT_C static const TDesC8& ReplacementForUnconvertibleUnicodeCharacters();
	IMPORT_C static TInt ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
	                                        const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
	                                        TDes8& aForeign, const TDesC16& aUnicode, 
	                                        CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters);
	                                        
	IMPORT_C static TInt ConvertFromUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
	                                        const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
	                                        TDes8& aForeign, const TDesC16& aUnicode, 
	                                        CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters,
	                                        const TArray<CnvUtilities::SCharacterSet>& aArrayOfAdditionalCharacterSets);
	                                        
	IMPORT_C static TInt ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
	                                      TDes16& aUnicode, const TDesC8& aForeign, 
	                                      TInt& aNumberOfUnconvertibleCharacters, 
	                                      TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	                                      
	IMPORT_C static TInt ConvertToUnicode(CCnvCharacterSetConverter::TEndianness aDefaultEndiannessOfForeignCharacters, 
	                                      TDes16& aUnicode, const TDesC8& aForeign, 
	                                      TInt& aNumberOfUnconvertibleCharacters, 
	                                      TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, 
	                                      const TArray<CnvUtilities::SMethod>& aArrayOfAdditionalMethods);
	
private:

	static TInt DoConvertToUnicode(TDes16& aUnicode, const TDesC8& aForeign, 
	                               TInt& aNumberOfUnconvertibleCharacters, 
	                               TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter) ;
	                               
    static TInt DoConvertFromUnicode(const TDesC8& aReplacementForUnconvertibleUnicodeCharacters, 
                                     TDes8& aForeign, const TDesC16& aUnicode, CCnvCharacterSetConverter::TArrayOfAscendingIndices& aIndicesOfUnconvertibleCharacters) ;
	};


/** 
Returns the single character (as one or more byte values) which is used by 
default as the replacement for unconvertible Unicode characters. These byte 
values are output when converting from Unicode into a foreign character set 
for each Unicode character that has no equivalent in the foreign character 
set.

@return The single character which is used to replace unconvertible characters. 
@internalComponent
*/
GLREF_C const TDesC8& ReplacementForUnconvertibleUnicodeCharacters_internal();
	
#endif

