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

#ifndef T_SHAPETESTSTEP_H_
#define T_SHAPETESTSTEP_H_

#include <e32cmn.h>
#include <test/testexecutestepbase.h>

class CFontStore;
class CBitmapFont;
class CTestData;

class CShapeTestStep : public CTestStep
    {
public:
    static CTestStep * CreateShapeTestStep(const TDesC& aStepName);    
    
protected:
    TVerdict doTestStepL();

private:
    CShapeTestStep(TInt aTestDataIndex, TInt aNumberOfTestDataFiles, TUint32 aScript, TUint32 aLang, const TDesC& aScriptName, const TDesC& aTestID);
    
    void SetupTestDataL();
    
    void doShapeText(CBitmapFont* font, CTestData& aTestData, TBool aCheckOutput = EFalse);
    void shapeText(CBitmapFont* font, CTestData& aTestData, TBool aCheckOutput = EFalse);
    
    void doOomShapeText(CBitmapFont* font, CTestData& aTestData);
    void oomShapeText(CBitmapFont* font, CTestData& aTestData);
    
    void doRepeatShapeText(CBitmapFont* font, CTestData& aTestData);
    void repeatShapeText(CBitmapFont* aFont, CTestData& aTestData);
    
    void doRepeatShapeUsingVariousFontSizesTest(CFontStore* aFontStore, const TDesC aTypefaceName, const TDesC &aText1,
                                                const TDesC &aText2, const TDesC &aText3);
    void ShapeTestL();
    
    static const TDesC& getText1();
    static const TDesC& getText2();
    static const TDesC& getText3();
    
private:
    const TInt iTestDataIndex;
    const TInt iNumberOfTestDataFiles;
    const TUint32 iScript;
    const TUint32 iLang;
    const TBuf<32> iScriptName;
    const TBuf<64> iTestID;
    RPointerArray<CTestData> iTestDataArr;
    };

#endif /* T_SHAPETESTSTEP_H_ */
