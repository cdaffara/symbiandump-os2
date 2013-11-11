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
* @file
* @internalComponent 
*
*/


#ifndef TCUSTOMCHARMAPPING_H_
#define TCUSTOMCHARMAPPING_H_

#include <test/testexecutestepbase.h>

namespace LocalToTCustomCharMapping
    {
    _LIT(KTCustomCharMapping, "TCustomCharMapping");
    const TInt KDisplayWidth = 202;
    const TInt KDisplayHeight = 100;
    const TInt KPictureCharacter = 0xFFFC;

    enum TTestNum
        {
        EDefaultBehaviourInvisible = 0,
        EDefaultBehaviourVisible = 1,
        ECustomRemappingInvisible = 2,
        ECustomRemappingVisible = 3,
        ENewTest = 4
        //Test remapping with no custom remapper and some flags explicitly set to invisible
        };
    }

class CTextLayout;
class CTestGraphicsDevice;
class CTextView;

class CTCustomCharMappingStep : public CTestStep
    {    
public:
    CTCustomCharMappingStep();
protected:  
    TVerdict doTestStepL();

private:
    
    void DoTestL(TDes& aText, CTextLayout* /*aLayout*/, CTestGraphicsDevice* aDevice, CTextView* aView, LocalToTCustomCharMapping::TTestNum aTestNum);
    void RunInstallationTestsL();
    void RunDefaultBehaviourTestsL();
    void RunCustomBehaviourTestsL();

    };

_LIT(KTestStep_T_CustomCharMapping, "T_CustomCharMapping");


#endif /* TCUSTOMCHARMAPPING_H_ */
