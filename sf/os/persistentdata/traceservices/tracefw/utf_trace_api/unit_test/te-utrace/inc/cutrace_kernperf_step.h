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
 @file CUTrace_KernPerf_Step.h
 @internalTechnology
*/
#ifndef CUTRACE_KERNPERF_STEP_H_
#define CUTRACE_KERNPERF_STEP_H_


#include <test/testexecutestepbase.h>
#include "te_utracesuitestepbase.h"


class CUTrace_KernPerf_Step : public CTe_UTraceSuiteStepBase
	{
	public:
	CUTrace_KernPerf_Step();
	~CUTrace_KernPerf_Step();
//	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
//	virtual TVerdict doTestStepPostambleL();

	private:
	TInt TestTraces();
	};

_LIT(KUTrace_KernPerf_Step,"CUTrace_KernPerf_Step");


#endif /*CUTRACE_KERNPERF_STEP_H_*/
