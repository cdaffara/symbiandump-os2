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


#ifndef __T_CP1255_H__
#define __T_CP1255_H__

#include <test/testexecutestepbase.h>

class CT_CP1255 : public CTestStep
    {
public:
	CT_CP1255();
protected:  
    TVerdict doTestStepL();
private:
	void TestL();
	void OOMTestL();
    };

_LIT(KTestStep_T_CP1255, "T_CP1255");

#endif
