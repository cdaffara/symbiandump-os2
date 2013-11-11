/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
*/
#if (!defined __API_STEP_H__)
#define __API_STEP_H__

#include <test/testexecutestepbase.h>
#include "tef_schsvr_bursuitesimstepbase.h"

class CAPIStep : public CTefSchsvrBURSuiteSimStepBase
	{
public:
	CAPIStep();
	~CAPIStep();

	void CallPersistingFunctionsL(TSchedulerItemRef& aRef1, TSchedulerItemRef& aRef2);
	void CreateScheduleForTestL(TSchedulerItemRef& aRef1, TSchedulerItemRef& aRef2);
	
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL(); 	
private:
	};

_LIT(KAPIStep,"APIStep");

#endif
