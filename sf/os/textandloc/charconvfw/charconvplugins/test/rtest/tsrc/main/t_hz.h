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
#ifndef __T_HZ_H__
#define __T_HZ_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_HZ : public CTestStep
    {
public:
    CT_HZ();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestTruncatedConversionFromUnicodeToHz(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode);
    void TestSplittingConvertingFromUnicodeToHz(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, const TDesC8& aExpectedFirstPartOfHz, const TDesC8& aExpectedSecondPartOfHz, const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionToUnicodeFromHz(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalHz);
    void TestSplittingConvertingToUnicodeFromHz(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfHzBytesNotConvertedAtSplit, TInt aExpectedStateAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalHz);
    void TestIsIllFormedHz(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aHz);
    
    
    
    };

_LIT(KTestStep_T_HZ, "T_HZ");

#endif //__T_HZ_H__
