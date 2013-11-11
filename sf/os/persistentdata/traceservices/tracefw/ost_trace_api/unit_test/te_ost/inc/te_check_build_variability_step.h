// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file te_check_build_variability_step.h
 @internalTechnology
*/
#ifndef TE_USER_STEP_H_
#define TE_USER_STEP_H_
#include <test/testexecutestepbase.h>
#include "te_suite_step_base.h"


class CCheckBuildVariabilityStep : public CTe_UTraceSuiteStepBase
	{
public:
	CCheckBuildVariabilityStep();
	~CCheckBuildVariabilityStep();
	//virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
//	virtual TVerdict doTestStepPostambleL();
private:
	TVerdict CheckBuildtimeVariability();
	TVerdict CheckLoggingVariabilityL();
	TBool TestTracesL();
	TBool DoSendTraceL(TTraceApiUsed aApiUsed);
	TBool WasTraceVariabilitySuccessful(TInt aTraceResult);
	};

_LIT(KCheckBuildVariabilityStep,"CCheckBuildVariabilityStep");

#endif /*TE_USER_STEP_H_*/
