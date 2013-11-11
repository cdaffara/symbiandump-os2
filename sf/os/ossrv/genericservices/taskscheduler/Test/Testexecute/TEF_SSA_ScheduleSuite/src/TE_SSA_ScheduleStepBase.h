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



#ifndef __TE_SSA_SCHEDULESTEPBASE_H_
#define __TE_SSA_SCHEDULESTEPBASE_H_

#include <test/testexecutestepbase.h>

#include <e32property.h>
#include <tz.h>
#include "Thelpers.h"
#include <startup.hrh>
#include <domainmanager.h>

const TUid KTestUid = TUid::Uid(0x1027431C);
const TInt KTestKey1 = 1;
const TInt KTestKey2 = 2;
const TInt KKeyValue = 10;

_LIT(KMinimalTaskHandler, "MinimalTaskHandler");
_LIT(KSchexe, "Schexe");
_LIT(KTaskData, "Test data for SSA Schedule suite.");

typedef CArrayFixFlat<TScheduleEntryInfo2>	CScheduleEntryInfoArray;
typedef CArrayFixFlat<TTaskSchedulerCondition>	CSchConditionArray;

class CSSA_ScheduleStepBase : public CTestStep
	{
public:
	virtual ~CSSA_ScheduleStepBase();
	CSSA_ScheduleStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

protected:
	TInt RequestDomainStateTransition(TStartupStateIdentifier aTargetState);

	void RestartSchedulerL();

	// Creates a time based persistent schedule    
	// The schedule is disabled
	void AddPersistentScheduleL(const TDesC& aName,
								const TTsTime& aDueTime,
								TSchedulerItemRef& aRef);

	// Creates a time based transient schedule    
	// The schedule is disabled
	void AddTransientScheduleL(const TDesC& aName,
							   const TTsTime& aDueTime,
							   TSchedulerItemRef& aRef);

	// Creates a condition based schedule    
	// The schedule is disabled
	void AddConditionScheduleL(const TDesC& aName,
							   const TTsTime& aDefaultRunTime,
							   TSchedulerItemRef& aRef,
							   TInt aKeyId,
							   TBool aUseTransient);

	void CreatePSKeyL(TInt aKeyId);

	void BeginTestSetupL();

protected:
	RScheduler	iScheduler;
	RDmDomainManager iDomainManager;
	STaskSemaphore iSem;
	TBool iConnectedToDM;
	};

#endif
