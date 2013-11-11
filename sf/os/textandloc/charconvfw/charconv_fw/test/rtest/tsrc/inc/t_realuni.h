/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_REALUNI_H__
#define __T_REALUNI_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_REALUNI : public CTestStep
    {
public:
    CT_REALUNI();
protected:  
    TVerdict doTestStepL();
private:
    void TestREALUNI();
    };

_LIT(KTestStep_T_REALUNI, "T_REALUNI");

#endif
