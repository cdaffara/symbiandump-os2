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

#ifndef __T_FMT1_H__
#define __T_FMT1_H__

#include <test/testexecutestepbase.h>

class CT_FMT1 : public CTestStep
	{
public:
	CT_FMT1();
protected:
	TVerdict doTestStepL();
private:
	void TestDEF047316L();
	};

_LIT(KTestStep_T_FMT1, "T_FMT1");

#endif
