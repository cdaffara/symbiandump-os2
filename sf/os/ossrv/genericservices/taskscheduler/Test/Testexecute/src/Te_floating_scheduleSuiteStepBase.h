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



#if (!defined __TE_SCHSVR_FLOATING_SCHEDULE_STEP_BASE__)
#define __TE_SCHSVR_FLOATING_SCHEDULE_STEP_BASE__
#include <test/testexecutestepbase.h>

// Please add your include here if you have 
#include "Thelpers.h"
#include <e32property.h>
#include <tz.h>

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
static RScheduler	TheScheduler;

typedef CArrayFixFlat<TScheduleEntryInfo2>	CScheduleEntryInfoArray;
typedef CArrayFixFlat<TTaskSchedulerCondition>	CSchConditionArray;

const TUid KTestUid = TUid::Uid(0x101F7784);
const TInt KTestKey1 = 1;
const TInt KTestKey2 = 2;
const TInt KTestKey3 = 3;
const TInt KTestKey4 = 4;
const TInt KTestKey5 = 5;
const TInt KTestKey6 = 6;
const TInt KTestKey7 = 7;
const TInt KTestKey8 = 8;

_LIT(KMinimalTaskHandler, "MinimalTaskHandler");


class CTe_floating_scheduleSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_floating_scheduleSuiteStepBase();
	CTe_floating_scheduleSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

//Please add your class members which will be common to all individual test steps:

protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	};

#endif
