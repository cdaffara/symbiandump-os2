/**
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file def130695_Step.h
 @internalTechnology
*/
#if (!defined ___DEF130695__STEP_H__)
#define ___DEF130695__STEP_H__
#include <test/testexecutestepbase.h>
#include "def130695_SuiteStepBase.h"
#include "libxml2_tree.h"
#include "libxml2_sax2.h"



class C_def130695_Step1 : public C_def130695_SuiteStepBase
	{
public:
	C_def130695_Step1();
	~C_def130695_Step1();
	virtual TVerdict doTestStepL();


private:
	};

_LIT(K_def130695_Step1,"def130695_Step1");

class C_def130695_Step2 : public C_def130695_SuiteStepBase
	{
public:
	C_def130695_Step2();
	~C_def130695_Step2();
	virtual TVerdict doTestStepL();


private:
	};

_LIT(K_def130695_Step2,"def130695_Step2");


#endif
