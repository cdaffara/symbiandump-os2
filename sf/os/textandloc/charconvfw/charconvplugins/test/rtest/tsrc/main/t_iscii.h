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
#ifndef __T_ISCII_H__
#define __T_ISCII_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_ISCII : public CTestStep
    {
public:
    CT_ISCII();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void TestConversionToUnicodeFromISCII(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aExpectedUnicode, const TDesC8& aOriginalISCII, TInt aExpectedResult = 0);
    void TestConversionToUnicodeFromTruncatedISCII(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16&, const TDesC8& aOriginalISCII, TInt aExpectedResult = 0);
    void TestConversionFromUnicodeToISCII(CCnvCharacterSetConverter& aCharacterSetConverter, const TDesC16& aOriginalUnicode, const TDesC8& aExpectedISCII, TInt aExpectedResult = 0);
    void TestConversion( CCnvCharacterSetConverter& aCharacterSetConverter );
    
    
    };

_LIT(KTestStep_T_ISCII, "T_ISCII");

#endif //__T_ISCII_H__
