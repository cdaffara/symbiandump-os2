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

#ifndef __T_ROMAN_H__
#define __T_ROMAN_H__

#include <test/testexecutestepbase.h>
#include "../sfields/FLDNUMS.H"

class CT_ROMAN : public CTestStep
    {
public:
	CT_ROMAN();
protected:  
    TVerdict doTestStepL();
private:
    void GetValue(HBufC* aBuf,TInt aDenery,TDeneryToCharBase& aNumeral);
    void DisplayValue(HBufC* aBuf,TInt aDenery,TDeneryToCharBase& aNumeral);
    void test1();
    void test2();
    void test3();
    void runTests();
    void setupCleanup();
    };

_LIT(KTestStep_T_ROMAN, "T_ROMAN");

#endif
