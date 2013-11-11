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
 @file SampleStep.h
*/
#if (!defined __SAMPLE_STEP_H__)
#define __SAMPLE_STEP_H__
#include <test/testexecutestepbase.h>
#include "ctefunitserver.h"

// __EDIT_ME__ - Create your own test step definitions
class CSampleStep1 : public CTestStep
	{
public:
	CSampleStep1();
	~CSampleStep1();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	};

class CSampleStep2 : public CTestStep
	{
public:
	CSampleStep2(CTEFUnitServer& aParent);
	~CSampleStep2();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	CTEFUnitServer& iParent;
	};

class CSampleStep3 : public CTestStep
	{
public:
	CSampleStep3(CTEFUnitServer& aParent);
	~CSampleStep3();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	CTEFUnitServer& iParent;
	};


_LIT(KSampleStep1,"SampleStep1");
_LIT(KSampleStep2,"SampleStep2");
_LIT(KSampleStep3,"SampleStep3");


#endif
