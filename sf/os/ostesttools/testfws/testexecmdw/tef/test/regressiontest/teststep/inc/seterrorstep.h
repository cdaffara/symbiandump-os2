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
 @file SetErrorStep.h
*/
#if (!defined __SETERROR_STEP_H__)
#define __SETERROR_STEP_H__
#include <test/testexecutestepbase.h>

class CSetErrorStep : public CTestStep
	{
public:
	CSetErrorStep();
	~CSetErrorStep();
	virtual TVerdict doTestStepL();

// Please add/modify your class members here:
private:
	};

_LIT(KSetErrorStep,"SetErrorStep");

#endif
