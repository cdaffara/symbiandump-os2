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
 @file te_check_performance_step.h
 @internalTechnology
*/

#ifndef TE_CHECK_PERFORMANCE_STEP_H_
#define TE_CHECK_PERFORMANCE_STEP_H_
#include <test/testexecutestepbase.h>
#include "te_suite_step_base.h"


class CCheckPerformanceStep : public CTe_UTraceSuiteStepBase
	{
public:
	CCheckPerformanceStep();
	~CCheckPerformanceStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
//	virtual TVerdict doTestStepPostambleL();
private:
	//not implemented...
	TInt FindLargestAndSmallest(RArray<TUint32> aList, TInt& aLargest, TInt& aSmallest);
	};

_LIT(KCheckPerformanceStep,"CCheckPerformanceStep");

#endif /*TE_CHECK_PERFORMANCE_STEP_H_*/
