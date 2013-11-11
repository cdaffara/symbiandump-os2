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
 @file te_uloggerclearconfigstep.h
 @internalTechnology
*/
#if (!defined __TE_ULOGGERCLEARCONFIGSTEP_H__)
#define __TE_ULOGGERCLEARCONFIGSTEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggermclsuitestepbase.h"
#include <e32base.h>
#include <e32math.h>
#include <uloggerclient.h>

class CULoggerClearConfigStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerClearConfigStep();
	~CULoggerClearConfigStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KULoggerClearConfigStep,"ULoggerClearConfigStep");

#endif
