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



#ifndef __INC072424_STEP_H__
#define __INC072424_STEP_H__

#include <test/testexecutestepbase.h>
#include "TE_SSA_ScheduleStepBase.h"

//--------------------------------------------------------
_LIT(KSSA_StateTransitionStep, "SSA_StateTransitionStep");

class CSSA_StateTransitionStep : public CSSA_ScheduleStepBase
	{
public:
	CSSA_StateTransitionStep();
	~CSSA_StateTransitionStep();
	virtual TVerdict doTestStepL();

private:
	void GetParametersL(TStartupStateIdentifier& aState, TBool& aLaunch);
	};

//--------------------------------------------------------
_LIT(KSSA_PersistentScheduleStep, "SSA_PersistentScheduleStep");

class CSSA_PersistentScheduleStep : public CSSA_ScheduleStepBase
	{
public:
	CSSA_PersistentScheduleStep();
	~CSSA_PersistentScheduleStep();
	virtual TVerdict doTestStepL();

private:
	void GetParametersL(TInt& aDoNCTransition, TBool& aLaunch);
	};

//--------------------------------------------------------
_LIT(KSSA_ConditionScheduleStep, "SSA_ConditionScheduleStep");

class CSSA_ConditionScheduleStep : public CSSA_ScheduleStepBase
	{
public:
	CSSA_ConditionScheduleStep();
	~CSSA_ConditionScheduleStep();
	virtual TVerdict doTestStepL();

private:
	void GetParametersL(TInt& aMatchCondFirst, TInt& aUseTransient);
	};

//--------------------------------------------------------
_LIT(KSSA_EnvironmentChangeStep, "SSA_EnvironmentChangeStep");

class CSSA_EnvironmentChangeStep : public CSSA_ScheduleStepBase
	{
public:
	CSSA_EnvironmentChangeStep();
	~CSSA_EnvironmentChangeStep();
	virtual TVerdict doTestStepL();
	};

//--------------------------------------------------------
_LIT(KSSA_EditTransientStep, "SSA_EditTransientStep");

class CSSA_EditTransientStep : public CSSA_ScheduleStepBase
	{
public:
	CSSA_EditTransientStep();
	~CSSA_EditTransientStep();
	virtual TVerdict doTestStepL();
	};

//--------------------------------------------------------
_LIT(KSSA_EditPersistentStep, "SSA_EditPersistentStep");

class CSSA_EditPersistentStep : public CSSA_ScheduleStepBase
	{
public:
	CSSA_EditPersistentStep();
	~CSSA_EditPersistentStep();
	virtual TVerdict doTestStepL();
	};

//--------------------------------------------------------
_LIT(KSSA_EditConditionStep, "SSA_EditConditionStep");

class CSSA_EditConditionStep : public CSSA_ScheduleStepBase
	{
public:
	CSSA_EditConditionStep();
	~CSSA_EditConditionStep();
	virtual TVerdict doTestStepL();
	};

#endif
