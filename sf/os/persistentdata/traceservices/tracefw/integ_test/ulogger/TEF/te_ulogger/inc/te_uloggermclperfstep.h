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
 @file te_uloggermclperfstep.h
 @internalTechnology
*/
#if (!defined __TE_ULOGGERMCLPERFSTEP_H__)
#define __TE_ULOGGERMCLPERFSTEP_H__
#include <test/testexecutestepbase.h>
#include "te_uloggermclsuitestepbase.h"
#include "te_utracecmds.h"
#include "te_utracekernelcmds.h"
#include <uloggerclient.h>
#include "lightlogger.h"
#include <hal.h>
#include "te_setfilterparameters.h"

class CULoggerMCLUserPerfStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerMCLUserPerfStep();
	~CULoggerMCLUserPerfStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};
	
class CULoggerMCLKernelPerfStep : public CTe_ULoggerMCLSuiteStepBase
	{
public:
	CULoggerMCLKernelPerfStep();
	~CULoggerMCLKernelPerfStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KULoggerMCLUserPerfStep,"ULoggerMCLUserPerfStep");
_LIT(KULoggerMCLKernelPerfStep,"ULoggerMCLKernelPerfStep");

#endif
