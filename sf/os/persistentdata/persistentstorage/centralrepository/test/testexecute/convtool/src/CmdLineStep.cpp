// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#include <e32std.h>
#include "CmdLineStep.h"
#include "CentRepConvTool.h"

_LIT(KExpectedResult, "ExpectedResult");
_LIT(KConversionCmd, "ConversionCmd");
_LIT(Kpass, "pass");
_LIT(KNoDeleteOutFile, "NoDeleteOutFile");
_LIT(KMissingKey, "Missing %S in config file");
_LIT(KCorruptEntry, "Fail to parse %S = %S");

CCmdLineTestStep::CCmdLineTestStep()
	: iExpectPass(ETrue)
	{
	SetTestStepName(KCmdLineTestStep);
	}

TVerdict CCmdLineTestStep::doTestStepL()
	{
	TInt nPushed = 0;
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	nPushed++;
	CActiveScheduler::Install(sched); // but no Start()

	TServerResources::InitialiseL();
	CleanupStack::PushL(TCleanupItem(TCleanupOperation(&TServerResources::Close), NULL));
	nPushed++;

	GetCmdLineL();

	CCentRepConvTool* tool = CCentRepConvTool::NewL(iCmdLine,
		TServerResources::iFs, EFalse);
	CleanupStack::PushL(tool);
	nPushed++;

	TRAPD(ret, tool->ProcessCmdL());

	if (iExpectPass)
		{
		TEST(ret == KErrNone);
		}
	else
		{
		TEST(ret != KErrNone);
		}

	CleanupStack::PopAndDestroy(nPushed);
	return TestStepResult();
	}

void CCmdLineTestStep::GetCmdLineL()
	{
	TPtrC data;
	if (!GetStringFromConfig(ConfigSection(), KConversionCmd, data))
		{
		ERR_PRINTF2(KMissingKey, &KConversionCmd);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	iCmdLine.Set(data);

	TLex lex(data);

	TBool pass = ETrue;
	// Skip "-o"
	_LIT(KMinusOh, "-o");
	if (lex.NextToken().Compare(KMinusOh) != 0)
		{
		pass = EFalse;
		}
	else
		{
		iOutFileName.Set(lex.NextToken());
		iInFileName.Set(lex.NextToken());
		if (iInFileName.Length() == 0)
			{
			pass = EFalse;
			}
		}

	if (!pass)
		{
		ERR_PRINTF3(KCorruptEntry, &KConversionCmd, &data);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}

	if (!GetStringFromConfig(ConfigSection(), KExpectedResult, data))
		{
		ERR_PRINTF2(KMissingKey, &KExpectedResult);
		SetTestStepResult(EFail);
		User::Leave(KErrCorrupt);
		}
	else
		{
		iExpectPass = (data == Kpass);
		}
	}

