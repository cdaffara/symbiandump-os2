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

#ifndef TFORMHINDI_H_
#define TFORMHINDI_H_
#include <e32cmn.h> 
#include <test/testexecutestepbase.h>

class CTFormHindiStep : public CTestStep
    {    
public:
    CTFormHindiStep(){};
    void print(const TDesC& aBuf, const TText8* text, int line);
    void testpoint(TBool aCondition, const TText8* text, int line );
protected:  
    TVerdict doTestStepL();
    };

inline void CTFormHindiStep::print(const TDesC& aBuf, const TText8* text, int line)
    {
    Logger().LogExtra((text), line, ESevrInfo, (aBuf));
    }

inline void CTFormHindiStep::testpoint(TBool aCondition, const TText8* text, int line )
    {
    testBooleanTrue((aCondition), text, line); 
    }

_LIT(KTestStep_T_FormHindi, "T_FormHindi");

#endif /* TFORMHINDI_H_ */
