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
 @file WriteStringStep.h
*/
#if (!defined __SHORT_FUNS_TEST_STEP_H__)
#define __SHORT_FUNS_TEST_STEP_H__
#include <test/testexecutestepbase.h>

class CShortFunsTestStep : public CTestStep
	{
public:
	CShortFunsTestStep();
	~CShortFunsTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

#endif
