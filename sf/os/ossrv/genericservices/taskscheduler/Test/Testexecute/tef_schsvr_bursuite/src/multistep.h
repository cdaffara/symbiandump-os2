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
#if (!defined __MULTI_STEP_H__)
#define __MULTI_STEP_H__

#include <test/testexecutestepbase.h>
#include "tef_schsvr_bursuitesimstepbase.h"

class CMultiStep : public CTefSchsvrBURSuiteSimStepBase
	{
public:
	CMultiStep();
	~CMultiStep();

	void AddTimeSchedulesL();
	
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL(); 	
	
private:
    TSchedulerItemRef iRef1;    
    TSchedulerItemRef iRef2;        
	};

_LIT(KMultiStep,"MultiStep");

#endif
