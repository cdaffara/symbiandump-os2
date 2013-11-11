/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file addfield_logStep.h
*/
#if (!defined __ADDFIELD_LOG_STEP_H__)
#define __ADDFIELD_LOG_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_RFileLoggerSuiteStepBase.h"

class Caddfield_logStep : public CTe_RFileLoggerSuiteStepBase
	{
public:
	Caddfield_logStep();
	~Caddfield_logStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(Kaddfield_logStep,"addfield_logStep");

#endif
