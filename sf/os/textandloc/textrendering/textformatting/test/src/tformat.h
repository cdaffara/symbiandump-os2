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

#ifndef TFORMAT_H_
#define TFORMAT_H_

#include <e32cmn.h> 
#include <test/testexecutestepbase.h>

class CTFormatStep : public CTestStep
    {    
public:
    CTFormatStep(){};
    void print(const TDesC& aBuf, const TText8* text, int line);
    void testpoint(TBool aCondition, const TText8* text, int line);
protected:  
    TVerdict doTestStepL();
    };

inline void CTFormatStep::print(const TDesC& aBuf, const TText8* text, int line)
    {
    Logger().LogExtra((text), line, ESevrInfo, (aBuf));
    }

inline void CTFormatStep::testpoint(TBool aCondition, const TText8* text, int line )
    {
    testBooleanTrue((aCondition), text, line); 
    }

_LIT(KTestStep_T_Format, "T_Format");

#endif /* TFORMAT_H_ */
