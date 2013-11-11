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
#ifndef __T_ISO2022JP1_2_H__
#define __T_ISO2022JP1_2_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_ISO2022JP1_2 : public CTestStep
    {
public:
    CT_ISO2022JP1_2();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestConversionFromUnicodeToIso(
                    CCnvCharacterSetConverter& aCharacterSetConverter,
                    const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionFromUnicodeToIso2022Jp(
                    CCnvCharacterSetConverter& aCharacterSetConverter,
                    const TDesC16& aOriginalUnicode);
    void TestSplittingConvertingFromUnicodeToIso2022Jp(
                    CCnvCharacterSetConverter& aCharacterSetConverter,
                    TInt aMaximumLengthLowerLimit,
                    TInt aMaximumLengthUpperLimit,
                    TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit,
                    const TDesC8& aExpectedFirstPartOfIso2022Jp,
                    const TDesC8& aExpectedSecondPartOfIso2022Jp,
                    const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionToUnicodeFromIso2022Jp(
                    CCnvCharacterSetConverter& aCharacterSetConverter,
                    const TDesC16& aExpectedUnicode,
                    const TDesC8& aOriginalIso2022Jp);
    void TestSplittingConvertingToUnicodeFromIso2022Jp(
                    CCnvCharacterSetConverter& aCharacterSetConverter,
                    TInt aMaximumLengthLowerLimit,
                    TInt aMaximumLengthUpperLimit,
                    TInt aExpectedNumberOfIso2022JpBytesNotConvertedAtSplit,
                    TInt aExpectedLengthOfFirstPartOfUnicode,
                    const TDesC16& aExpectedUnicode,
                    const TDesC8& aOriginalIso2022Jp);
    void TestIsIllFormedIso2022Jp(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aIso2022Jp);
    void TestIso2022Jp(CCnvCharacterSetConverter * characterSetConverter, TBool isS60version = ETrue);
    
    
    };

_LIT(KTestStep_T_ISO2022JP1_2, "T_ISO2022JP1_2");

#endif //__T_ISO2022JP1_2_H__
