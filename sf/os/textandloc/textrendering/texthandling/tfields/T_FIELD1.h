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

#ifndef __T_FIELD1_H__
#define __T_FIELD1_H__

#include <test/testexecutestepbase.h>

class CT_FIELD1 : public CTestStep
    {
public:
	CT_FIELD1();
protected:  
    TVerdict doTestStepL();
private:
    TBool UpdateField(TInt aPos);
    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();
    void test8();
    void test_multiple_fields();
    void testFields();
    void setupCleanup();
    };

_LIT(KTestStep_T_FIELD1, "T_FIELD1");

#endif
