/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_testaction.h"

#include "t_output.h"
#include "t_input.h"
#include "t_testactionspec.h"
#include "tScriptSetup.h"
#include "t_testhandler.h"

EXPORT_C CTestAction::~CTestAction()
	{
	delete iNameInfo;
	}

EXPORT_C void CTestAction::PerformPrerequisite(TRequestStatus& aStatus)
	{
	DoPerformPrerequisite(aStatus);
	}

EXPORT_C void CTestAction::PerformPostrequisite(TRequestStatus& aStatus)
	{
	DoPerformPostrequisite(aStatus);
	}

EXPORT_C void CTestAction::AfterOOMFailure()
	{
	}

TBool CTestAction::Finished()
	{
	return iFinished;
	}

EXPORT_C void CTestAction::ReportAction()
	{
	iOut.writeString(*iNameInfo);
	iOut.writeNewLine();
	DoReportAction();
	}

EXPORT_C void CTestAction::CheckResult(TInt aError)
{

	// DoCheckResult must update iResult 
	//iResult = (iResult && iExpectedResult) || (!iResult && !iExpectedResult);
	DoCheckResult(aError);
	
	if (iKnownFailure)		//	Expecting an error because of known
		iResult = ETrue;	//	defect current deferred etc
	
	iOut.writeString(_L("\tStatus : "));
	iOut.writeError(aError);
	iOut.writeString(_L(" ("));
	iOut.writeNum(aError);
	iOut.writeString(_L(")"));
	iOut.writeNewLine();
	iOut.writeString(_L("\tTest outcome : "));
	if (iResult)
		{
		iOut.writeString(_L("PASSED"));
		}
	else
		{
		iOut.writeString(_L("FAILED"));
		}
	iOut.writeNewLine();
	iOut.writeNewLine();
	}

EXPORT_C void CTestAction::PerformCancel()
	{
	}

EXPORT_C void CTestAction::Reset()
	{
	}

EXPORT_C CTestAction::CTestAction(CConsoleBase& aConsole,
								  Output& aOut)
: iFinished(EFalse), iKnownFailure(EFalse), iConsole(aConsole), iOut(aOut)
	{
	}

EXPORT_C void CTestAction::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	iNameInfo = aTestActionSpec.iActionName.AllocL();
	
	iTefScript = aTestActionSpec.iTefScript;
	
	TRAPD(err, iExpectedResult = Input::ParseElementBoolL(
		aTestActionSpec.iActionResult, _L8("<result>")) );
	if(err == KErrArgument)
		{
		//if there is no <result> field or it has invalid text then we assume
		//it's a "normal" test.  ie it must be true to pass.
		iExpectedResult = ETrue; 
		}
	else if(err != KErrNone)
		{
		User::Leave(err);
		}
	
//	If there's a known defect because of deferred defects etc, the script
//	will flag it.  We record as much here and, when result is returned, check
//	against known failure and adjust result accordingly.  This should only be
//	used for reported defects that have been deferred etc rather than as a 
//	workaround for test failures.  The failure will be reported in a separate
//	part of the log file so it's not overlooked entirely.
	
	_LIT8(KTrue, "ETrue");
	// _LIT8(KFalse, "EFalse");

	TInt pos = 0; 
	err = KErrNone;
	
	TPtrC8 knownFailure = Input::ParseElement(aTestActionSpec.iActionBody, KKnownDefectStart, KKnownDefectEnd, pos, err);	
	if (knownFailure.Compare(KTrue)==0)
		iKnownFailure = ETrue;
	else	//	Assume if it's not true it's false or not there which == false
		iKnownFailure = EFalse;
}


EXPORT_C void CTestAction::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

EXPORT_C void CTestAction::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	// pass on any error from peform action
	User::RequestComplete(status, aStatus.Int()); 
	iFinished = ETrue;
	}

EXPORT_C void CTestAction::FailTestNow(TRequestStatus& aStatus)
	{
	iActionState=EPostrequisite;
	iResult=EFalse;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}


EXPORT_C CTestAction::TScriptError CTestAction::ScriptError(void)
	{
	return(iScriptError);
	}

EXPORT_C void CTestAction::SetScriptError(const TScriptError &aScriptError, const TDesC& aError)
	{
	iScriptError = aScriptError;
	aScriptErrorDescription.Copy(aError); 
	};

EXPORT_C void CTestAction::ScriptError(TDes& aError)
	{
	aError.Copy(aScriptErrorDescription);
	};

void CTestAction::SetTestHandler(CTestHandler& aTestHandler)
	{
	iTestHandler = &aTestHandler;
	}

EXPORT_C CBase* CTestAction::SharedData() const
	{
	ASSERT(iTestHandler);
	return iTestHandler->SharedData();
	}

EXPORT_C void CTestAction::SetSharedData(CBase* aData)
	{
	ASSERT(iTestHandler);
	iTestHandler->SetSharedData(aData);
	}

EXPORT_C void CTestAction::ResetState()
	{
	}
