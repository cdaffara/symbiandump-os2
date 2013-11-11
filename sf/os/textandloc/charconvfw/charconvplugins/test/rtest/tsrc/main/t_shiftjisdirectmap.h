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
#ifndef __T_SHIFTJISDIRECTMAP_H__
#define __T_SHIFTJISDIRECTMAP_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_SHIFTJISDIRECTMAP : public CTestStep
    {
public:
    CT_SHIFTJISDIRECTMAP();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestTruncatedConversionFromUnicodeToShiftJisDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode);
    void TestSplittingConvertingFromUnicodeToShiftJisDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfShiftJisDirectmap, const TDesC8& aExpectedShiftJisDirectmap, const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionToUnicodeFromShiftJisDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalShiftJisDirectmap);
    void TestSplittingConvertingToUnicodeFromShiftJisDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfShiftJisDirectmapBytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalShiftJisDirectmap);
    
   
    };

_LIT(KTestStep_T_SHIFTJISDIRECTMAP, "T_SHIFTJISDIRECTMAP");

#endif //__T_SHIFTJISDIRECTMAP_H__
