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
 @file CUTrace_Trace_a1a2a3_Step.h
 @internalTechnology
*/
#ifndef CUTRACE_STATIC_STEP_H_
#define CUTRACE_STATIC_STEP_H_



#include <test/testexecutestepbase.h>
#include "te_utracesuitestepbase.h"

class CUTrace_Static_Step : public CTe_UTraceSuiteStepBase
	{
public:
	CUTrace_Static_Step();
	~CUTrace_Static_Step();
//	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
//	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KUTrace_Static_Step,"CUTrace_Static_Step");



#endif /*CUTRACE_STATIC_STEP_H_*/
