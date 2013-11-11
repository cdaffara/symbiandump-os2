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
 @file te_regperformancelowmemoryteststep.h
 @internalTechnology
*/
#if (!defined __TE_REG_PERFORMANCE_LOWMEMROY_TEST_STEP_H__)
#define __TE_REG_PERFORMANCE_LOWMEMROY_TEST_STEP_H__
#include <test/testexecutestepbase.h>

class CTe_RegPerformanceLowMemoryTestStep : public CTestStep
	{
public:
	CTe_RegPerformanceLowMemoryTestStep();
	~CTe_RegPerformanceLowMemoryTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KTe_RegPerformanceLowMemoryTestStep,"Te_RegPerformanceLowMemoryTestStep");

#endif
