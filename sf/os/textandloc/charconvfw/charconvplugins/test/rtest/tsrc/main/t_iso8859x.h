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
#ifndef __T_ISO8859X_H__
#define __T_ISO8859X_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_ISO8859X : public CTestStep
    {
public:
    CT_ISO8859X();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    void testDef050040L();
    void testInc043911L();
    void testInc042690L();
    void test1L();
    
    };

_LIT(KTestStep_T_ISO8859X, "T_ISO8859X");

#endif //__T_ISO8859X_H__
