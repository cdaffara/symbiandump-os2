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
#ifndef __T_GB18030_H__
#define __T_GB18030_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_GB18030 : public CTestStep
    {
public:
    CT_GB18030();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestTruncatedConversionFromUnicodeToGb18030(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode);
    void TestSplittingConvertingFromUnicodeToGb18030(CCnvCharacterSetConverter& aCharacterSetConverter,
            TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit,
            TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfGb18030,
            const TDesC8& aExpectedGb18030, const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionToUnicodeFromGb18030(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalGb18030);
    void TestSplittingConvertingToUnicodeFromGb18030(CCnvCharacterSetConverter& aCharacterSetConverter,            
                    TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit,
                    TInt aExpectedNumberOfGb18030BytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode,
                    const TDesC16& aExpectedUnicode, const TDesC8& aOriginalGb18030);
    void CharacterSetValueAndMIBTests(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession);
    void TestConversionToUnicodeFromGb18030(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalGb18030, TInt aExpectedResult = 0);
    void TestConversionFromUnicodeToGb18030(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode, TInt aExpectedResult = 0);
    void OneByteConversion( CCnvCharacterSetConverter& aCharacterSetConverter );
    void TwoByteConversion( CCnvCharacterSetConverter& aCharacterSetConverter );
    void FourByteConversion( CCnvCharacterSetConverter& aCharacterSetConverter );
    void OneByteConversionNegative( CCnvCharacterSetConverter& aCharacterSetConverter );
    void TwoByteConversionNegative( CCnvCharacterSetConverter& aCharacterSetConverter );
    void FourByteConversionNegative( CCnvCharacterSetConverter& aCharacterSetConverter );
    void ProblemProneCharcters( CCnvCharacterSetConverter& aCharacterSetConverter );
    
    
 
    };

_LIT(KTestStep_T_GB18030, "T_GB18030");

#endif //__T_GB18030_H__
