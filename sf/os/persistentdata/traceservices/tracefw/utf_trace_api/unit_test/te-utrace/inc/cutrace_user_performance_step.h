
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
 @file CUTrace_Performance_Step.h
 @internalTechnology
*/
#ifndef CUTRACE_PERFORMANCE_STEP_H_
#define CUTRACE_PERFORMANCE_STEP_H_
#include <test/testexecutestepbase.h>
#include "te_utracesuitestepbase.h"


class CUTrace_User_Performance_Step : public CTe_UTraceSuiteStepBase
	{
public:
	CUTrace_User_Performance_Step();
	~CUTrace_User_Performance_Step();
//	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
//	virtual TVerdict doTestStepPostambleL();
// Please add/modify your class members here:
private:
	TInt UserUTraceBench(TPrimaryFilter aF1, TInt aType, TBool aUID, TBool aContext,TBool aPC, TInt& aTime);
	TInt UserBTraceBench(TPrimaryFilter aF1, TInt aType, TBool aUID, TBool aContext,TBool aPC, TInt& aTime);
	};

_LIT(KUTrace_User_Performance_Step,"CUTrace_User_Performance_Step");


#endif /*CUTRACE_PERFORMANCE_STEP_H_*/
