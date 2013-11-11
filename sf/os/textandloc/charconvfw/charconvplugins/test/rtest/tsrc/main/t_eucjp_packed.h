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

#ifndef __T_EUCJP_PACKED_H__
#define __T_EUCJP_PACKED_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_EUCJP_PACKED : public CTestStep
    {
public:
    CT_EUCJP_PACKED();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestTruncatedConversionFromUnicodeToEucJpPacked(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode);
    void TestSplittingConvertingFromUnicodeToEucJpPacked(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfEucJpPacked, const TDesC8& aExpectedEucJpPacked, const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionToUnicodeFromEucJpPacked(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalEucJpPacked);
    void TestSplittingConvertingToUnicodeFromEucJpPacked(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfEucJpPackedBytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalEucJpPacked);
    void TestIsIllFormedEucJpPacked(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aEucJpPacked);
    
    
    
    
    };

_LIT(KTestStep_T_EUCJP_PACKED, "T_EUCJP_PACKED");

#endif //__T_EUCJP_PACKED_H__
