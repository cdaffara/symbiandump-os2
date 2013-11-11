/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include "step.h"
#include "testexecuteinterface.h"
#include "t_testsetup.h"

using namespace TestExecuteInterface;

_LIT(KScriptName, "scriptname");
_LIT(KLogName, "logname");

CStep::CStep(TScriptTests* aTestTypes)
    : iTestTypes(aTestTypes)
	{
	iConsole = new(ELeave) CStepConsole(this);
	}

CStep::~CStep()
    {
    delete iConsole;
    }

TVerdict CStep::doTestStepPreambleL()
	{
	return TestStepResult();
	}

TVerdict CStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CStep::doTestStepL()
	{
	__UHEAP_MARK;
	TPtrC script, log;
	GetStringFromConfig(ConfigSection(), KScriptName, script);
	GetStringFromConfig(ConfigSection(), KLogName, log);

	TBool result = EFalse;
	CTestSetup::CreateAndRunTestsL(iTestTypes, script, log, EFalse, iConsole, &result);
	SetTestStepResult(result ? EPass : EFail);

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CStep::Read(TRequestStatus& aStatus)
	{
	TRequestStatus* stat = &aStatus;
	User::RequestComplete(stat, KErrNone);
	}

void CStep::Write(const TDesC& aDes)
	{
	Logger().Write(aDes);
	}

void CStepConsole::Read(TRequestStatus& aStatus)
	{
	iStep->Read(aStatus);
	}

void CStepConsole::Write(const TDesC& aDes)
	{
	iStep->Write(aDes);
	}

