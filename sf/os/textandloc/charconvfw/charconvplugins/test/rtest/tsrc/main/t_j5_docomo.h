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
#ifndef __T_J5_DOCOMO_H__
#define __T_J5_DOCOMO_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;
class RTest1
    {
    TInt x;
    };

class CT_J5_DOCOMO : public CTestStep
    {
public:
    CT_J5_DOCOMO();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void checkUCS2converstion(RTest1& aTest, CCnvCharacterSetConverter *aJ5converter );
    void checkConversionSpeedL(RTest1& aTest, RFs &aFileServerSession, const TDesC &aForeign);
    void checkNormalAutodetectL( RTest1& aTest, RFs &aFileServerSession, const TDesC &aForeign );
    void convertDataUsingJ5(RTest1& aTest,
        RFs &fileServerSession,
        const TDesC8 &aForeign,
        const TDesC16 &aUnicode );
    void checkDetection(RTest1& aTest,
        RFs fileServerSession,
        const TDesC &aForeign,
        TInt aExpectedCode );
    void convertFromUnicodeUsingJ5(RTest1& aTest,
        RFs fileServerSession,
        CCnvCharacterSetConverter*aJ5converter );
    void convertFileUsingJ5L(RTest1& aTest,
        RFs fileServerSession,
        CCnvCharacterSetConverter*aJ5converter,
        const TDesC &aForeign,
        const TDesC &aExpectedUnicode,
        const TDesC &aOutputUnicode );
    void checkJ5Available(RTest1 aTest, RFs &aFileServerSession);
    void SaveConvertedUnicodeAsFile( RFs &fileServerSession,
            const TDesC &aFileName,
            const TDesC &aConvertedUnicode);
    
        
    
    };

_LIT(KTestStep_T_J5_DOCOMO, "T_J5_DOCOMO");

#endif //__T_J5_DOCOMO_H__
