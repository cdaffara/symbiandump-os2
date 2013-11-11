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
#ifndef __T_JIS_H__
#define __T_JIS_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_JIS : public CTestStep
    {
public:
    CT_JIS();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestTruncatedConversionFromUnicodeToJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode);
    void TestSplittingConvertingFromUnicodeToJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, const TDesC8& aExpectedFirstPartOfJis, const TDesC8& aExpectedSecondPartOfJis, const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionToUnicodeFromJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalJis);
    void TestSplittingConvertingToUnicodeFromJis(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfJisBytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalJis);
    void TestIsIllFormedJis(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aJis);
    void ReadDesc(TDes8& aDes, const TDesC& aFilename, RFs& aFs);
        
    
    };

_LIT(KTestStep_T_JIS, "T_JIS");

#endif //__T_JIS_H__
