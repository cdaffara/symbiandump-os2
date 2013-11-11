/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_concurrentcertstore.h"
#include "t_input.h"
#include "t_certstoredefs.h"

_LIT8(KConcurrentTestStart, "<KConcurrentTesting>");
_LIT8(KConcurrentTestEnd, "</KConcurrentTesting>");

CTestAction* CTestConcurrentCertStore::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
											const TTestActionSpec& aTestActionSpec)
{
	CTestAction* me = CTestConcurrentCertStore::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(me);
	return (me);
}
											
CTestAction* CTestConcurrentCertStore::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
											const TTestActionSpec& aTestActionSpec)
{
	CTestConcurrentCertStore* me = new (ELeave) CTestConcurrentCertStore(aFs, aConsole, aOut);
	CleanupStack::PushL(me);
	me->ConstructL(aTestActionSpec);
	return (me);
}

CTestConcurrentCertStore::~CTestConcurrentCertStore()
{}

CTestConcurrentCertStore::CTestConcurrentCertStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CCertStoreTestAction(aFs, aConsole, aOut)
{}

void CTestConcurrentCertStore::PerformAction(TRequestStatus& aStatus)
{
	iFinished = ETrue;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);	
}

void CTestConcurrentCertStore::DoReportAction()
{}

void CTestConcurrentCertStore::DoCheckResult(TInt /*aError*/)
{
	iResult = ETrue;
}

void CTestConcurrentCertStore::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CTestAction::ConstructL(aTestActionSpec);

//	Switch on performance testing?    
	TInt err = KErrNone;
	TInt pos = 0;
	TPtrC8 enablePtr = Input::ParseElement(aTestActionSpec.iActionBody, KConcurrentTestStart, KConcurrentTestEnd, pos, err);
    
	_LIT8(KTrueVal, "ETrue");
	_LIT8(KFalseVal, "EFalse");

	TBool enable = EFalse;
    if (enablePtr== KTrueVal)
        enable = ETrue;
    else if (enablePtr!=KFalseVal)
        User::Leave(KErrArgument);

	CConcurrentTester::SetDoingConcurrentTesting(enable);
	
	HBufC* result = HBufC::NewLC(aTestActionSpec.iActionResult.Length());
 	TPtr(result->Des()).Copy(aTestActionSpec.iActionResult);
 	Input::GetExpectedResultL(Input::ParseElement(*result, KReturnStart, KReturnEnd), iExpectedResult);
 	CleanupStack::PopAndDestroy(result);

}

void CTestConcurrentCertStore::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CTestConcurrentCertStore::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, aStatus.Int());
	}

///////////////////////////////////////////////////////////

GLDEF_C TBool gMultiThreadTest;

void CConcurrentTester::SetDoingConcurrentTesting(TBool aTestConcurrent)
{
	gMultiThreadTest=aTestConcurrent;
}

TBool CConcurrentTester::IsDoingConcurrentTesting()
{
	return (gMultiThreadTest);
}

void CConcurrentTester::SanitizeTestResult(Output& aOut, TBool& aResult)
{//	If running tests with multiple threads, failure may be expected
//	Check if this is the case, and write a message to output
	if (IsDoingConcurrentTesting())
	{
		_LIT(KConcurrent, "*** Concurrency testing is expecting failure ***");
		aOut.writeString(KConcurrent);
		aOut.writeNewLine();
		aResult = ETrue;
	}
}

