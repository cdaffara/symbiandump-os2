/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file AccessStep.h
*/
#if (!defined __ACCESS_STEP_H__)
#define __ACCESS_STEP_H__
#include <test/testexecutestepbase.h>

// __EDIT_ME__ - Create your own test step definitions
class CAccessStep1 : public CTestStep
	{
public:
	CAccessStep1();
	~CAccessStep1();
	virtual TVerdict doTestStepL();
	};

class CAccessStep2 : public CTestStep
	{
public:
	CAccessStep2();
	~CAccessStep2();
	virtual TVerdict doTestStepL();
	};

_LIT(KAccessStep1,"AccessStep1");
_LIT(KAccessStep2,"AccessStep2");
#endif
