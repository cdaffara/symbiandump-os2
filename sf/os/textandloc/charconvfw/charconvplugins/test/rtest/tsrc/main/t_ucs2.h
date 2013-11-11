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

#ifndef __T_UCS2_H__
#define __T_UCS2_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_UCS2 : public CTestStep
    {
public:
    CT_UCS2();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void LEConversionSinglePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter);
    void LEConversionMultiplePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter);
    void BEConversionSinglePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter);
    void BEConversionMultiplePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter);
    void UCS2BEConversionSinglePassBOM(CCnvCharacterSetConverter& aCharacterSetConverter);
    void UCS2BEConversionMultiplePassBOM (CCnvCharacterSetConverter& aCharacterSetConverter);
    void TooSmallBufferConversion (CCnvCharacterSetConverter& aCharacterSetConverter);
    void CharacterSetValueAndMIBTests(CCnvCharacterSetConverter& aCharacterSetConverter, RFs& aFileServerSession);
    
    
    
    
    };

_LIT(KTestStep_T_UCS2, "T_UCS2");

#endif //__T_UCS2_H__
