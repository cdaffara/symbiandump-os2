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

#ifndef __T_EUCJPDIRECTMAPS_H__
#define __T_EUCJPDIRECTMAP_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_EUCJPDIRECTMAP : public CTestStep
    {
public:
    CT_EUCJPDIRECTMAP();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestIsIllFormedEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC8& aEucJpDirectmap);
    void TestSplittingConvertingToUnicodeFromEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfEucJpDirectmapBytesNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfUnicode, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalEucJpDirectmap);
    void TestTruncatedConversionToUnicodeFromEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalEucJpDirectmap);
    void TestSplittingConvertingFromUnicodeToEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, TInt aMaximumLengthLowerLimit, TInt aMaximumLengthUpperLimit, TInt aExpectedNumberOfUnicodeCharactersNotConvertedAtSplit, TInt aExpectedLengthOfFirstPartOfEucJpDirectmap, const TDesC8& aExpectedEucJpDirectmap, const TDesC16& aOriginalUnicode);
    void TestTruncatedConversionFromUnicodeToEucJpDirectmap(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode);
    
    
    };

_LIT(KTestStep_T_EUCJPDIRECTMAP, "T_EUCJPDIRECTMAP");

#endif //__T_EUCJPDIRECTMAP_H__
