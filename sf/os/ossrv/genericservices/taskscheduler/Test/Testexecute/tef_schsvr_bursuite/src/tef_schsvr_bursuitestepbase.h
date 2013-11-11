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

#if (!defined __TEF_SCHSVR_BURSUITESTEPBASE__)
#define __TEF_SCHSVR_BURSUITESTEPBASE__

#include <test/testexecutestepbase.h>
#include <e32property.h>
#include "Thelpers.h"

// This TEF test has to wait for MinimalTaskHandler to signal a semaphore.
// The kernel will terminate the wait when the semaphore is 
// signaled. 
const TInt KTaskLaunchTimeout = 10000000;

const TInt KTestKey1 = 1;
const TUid KTestUid = TUid::Uid(0x101FA78A);
const TInt KKeyValue = 10;

_LIT(KMinimalTaskHandler, "MinimalTaskHandler");
_LIT(KSchexe, "Schexe");
_LIT(KTaskData, "Test data for Scheduler BUR suite.");

typedef CArrayFixFlat<TScheduleEntryInfo2>	CScheduleEntryInfoArray;
typedef CArrayFixFlat<TTaskSchedulerCondition>	CSchConditionArray;

/////////////////////////////////////////

class CTefSchsvrBURSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTefSchsvrBURSuiteStepBase();
	CTefSchsvrBURSuiteStepBase();

protected:
	void RestartSchedulerL();

	// Creates a time based persistent schedule    
	// The schedule is disabled
	void AddTimeScheduleL(const TDesC& aName,
								const TTsTime& aDueTime,
								TSchedulerItemRef& aRef);
								
	void AddTransientTimeScheduleL(const TDesC& aName,
								const TTsTime& aDueTime,
								TSchedulerItemRef& aRef);

	// Creates a condition based schedule    
	// The schedule is disabled
	void AddConditionScheduleL(const TDesC& aName,
							   const TTsTime& aDefaultRunTime,
							   TSchedulerItemRef& aRef,
				   			   CSchConditionArray& aConditionList);

	void AddTransientConditionScheduleL(const TDesC& aName,
							   const TTsTime& aDefaultRunTime,
							   CSchConditionArray& aConditionList);

	void AddTestConditionL(CSchConditionArray& aConditionList, TInt aKeyId);
	void AddBackupConditionL(CSchConditionArray& aConditionList);
	void AddRestoreConditionL(CSchConditionArray& aConditionList);	

	void CreatePSKeyL(TInt aKeyId);
	void GetParameterL(const TDesC& aKey, TInt& aValue);

	void BeginTestSetupL();

protected:
	RScheduler	iScheduler;
	STaskSemaphore iSem;
	};

#endif
