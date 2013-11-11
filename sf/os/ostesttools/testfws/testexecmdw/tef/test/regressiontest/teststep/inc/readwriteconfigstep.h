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
 @file ReadConfigStep.h
*/
#if (!defined __READ_WRITE_CONFIG_STEP_H__)
#define __READ_WRITE_CONFIG_STEP_H__
#include <test/testexecutestepbase.h>

class CReadWriteConfigStep : public CTestStep
	{
public:
	CReadWriteConfigStep();
	~CReadWriteConfigStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	TBool TestInt();
	TBool TestInt64();
	TBool TestBool();
	TBool TestHex();
	TBool TestString();
	void test_Suite();
	void test_Int64_Step_Suite();
	void test_Int64_Print_Suite();
	void test_TInt64_STEP(TInt aShift);
	void test_TInt64_smoketest();
	void Print64bit(TInt aShift);
	};

#endif
