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
 @file conn_closeStep.h
*/
#if (!defined __CONN_CLOSE_STEP_H__)
#define __CONN_CLOSE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_RFileLoggerSuiteStepBase.h"

class Cconn_closeStep : public CTe_RFileLoggerSuiteStepBase
	{
public:
	Cconn_closeStep();
	~Cconn_closeStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(Kconn_closeStep,"conn_closeStep");

#endif
