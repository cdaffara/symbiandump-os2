/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __T_SHIFTJIS_KDDIAU_H__
#define __T_SHIFTJIS_KDDIAU_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_SHIFTJIS_KDDIAU : public CTestStep
    {
public:
    CT_SHIFTJIS_KDDIAU();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestIsIllFormedShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aShiftJis);
    void TestSplittingConvertingToUnicodeFromShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfShiftJisBytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalShiftJis);
    void TestTruncatedConversionToUnicodeFromShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalShiftJis);
    void TestSplittingConvertingFromUnicodeToShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfShiftJis, const TDesC8& aExpectedShiftJis, const TDesC16& aOriginalUnicode);
    void TestSplittingConvertingFromUnicodeToShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfShiftJis, const TDesC8& aExpectedShiftJis, const TDesC16& aOriginalUnicode, const TDesC16& aUnicodeAfterRoundTrip);
    void TestTruncatedConversionFromUnicodeToShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionFromUnicodeToShiftJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode, const TDesC16& aUnicodeAfterRoundTrip);
    
    
   
    };

_LIT(KTestStep_T_SHIFTJIS_KDDIAU, "T_SHIFTJIS_KDDIAU");

#endif //__T_SHIFTJIS_KDDIAU_H__
