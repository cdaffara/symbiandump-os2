/**
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



#if (!defined __persist_hometime_float_STEP_H__)
#define __persist_hometime_float_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_floating_scheduleSuiteStepBase.h"

class Cpersist_hometime_floatStep : public CTe_floating_scheduleSuiteStepBase
	{
public:
	Cpersist_hometime_floatStep();
	~Cpersist_hometime_floatStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
	IMPORT_C TInt CreateScheduleL(TSchedulerItemRef& aRef, 
							RScheduler& aScheduler, 
							const TTsTime& aStartTime);

private:
	};

_LIT(Kpersist_hometime_floatStep,"persist_hometime_floatStep");

#endif
