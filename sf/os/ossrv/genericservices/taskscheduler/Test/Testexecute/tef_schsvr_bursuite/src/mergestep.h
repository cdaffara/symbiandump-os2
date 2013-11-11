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
#if (!defined __MERGE_STEP_H__)
#define __MERGE_STEP_H__

#include <test/testexecutestepbase.h>
#include "tef_schsvr_bursuitesimstepbase.h"

class CMergeStep : public CTefSchsvrBURSuiteSimStepBase
	{
public:
	CMergeStep();
	~CMergeStep();

	void AddTimeSchedulesL();
	void AddTimeSchedulesAfterBackupL();
	
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL(); 	
	
private:
    TSchedulerItemRef iRef1;    
    TSchedulerItemRef iRef2;
    TSchedulerItemRef iRef3;            
	};

_LIT(KMergeStep,"MergeStep");

#endif
