// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Started by DWW, April 1997
// Tests active scheduler
// 
//

#include <e32test.h>
#include <basched.h>
#include <baerror.h>
#include <baerrhan.h>

#define KUidTestValue 0x43
const TUid KUidTest={KUidTestValue};

LOCAL_D RTest test(_L("T_SCHED"));

class CSimpleErrorHandler : public CBaErrorHandler
	{
private:
	TErrorHandlerResponse HandleError(TDes& aErrorText,TDes& aContextText);
	};

class CErrorHandlerWithState : public CBaErrorHandler
	{
public:
	void SetState(TInt aState) { iState=aState; }
private:
	TErrorHandlerResponse HandleError(TDes& aErrorText,TDes& aContextText);
private:
	TInt iState;
	};

TErrorHandlerResponse CSimpleErrorHandler::HandleError(TDes& aErrorText,TDes& aContextText)
	{
	aErrorText=_L("ERROR");
	aContextText=_L("CONTEXT");
	return(EAlertDisplay);
	}

TErrorHandlerResponse CErrorHandlerWithState::HandleError(TDes& aErrorText,TDes& aContextText)
	{
	aErrorText.Num(iState);
	aContextText.Zero();
	return(iState? EInfoDisplay: ENoDisplay);
	}

class CTestScheduler : public CBaActiveScheduler
	{
private: // from CBaActiveScheduler
	void DisplayError(TInt aError);
private:
	TInt iState;
	};

class CTestActive : public CActive
	{
public:
	static CTestActive* NewL();
private:
	CTestActive();
	~CTestActive();
	void DoCancel();
	void RunL();
	void Queue();
private:
	TInt iState;
	CSimpleErrorHandler iSimpleErrorHandler;
	CErrorHandlerWithState iComplexErrorHandler;
	};

CTestActive::CTestActive()
	: CActive(0)
	{
	}

CTestActive::~CTestActive()
	{
	Cancel();
	}

CTestActive* CTestActive::NewL()
	{ // static
	CTestActive* self=new(ELeave) CTestActive;
	CActiveScheduler::Add(self);
	self->Queue();
	return(self);
	}

void CTestActive::Queue()
	{
	TRequestStatus* pS=(&iStatus);
	User::RequestComplete(pS,0);
	SetActive();
	}

void CTestActive::DoCancel()
	{
	}

void CTestActive::RunL()
	{
	Queue();
	switch (iState++)
		{
	case 0:
		test.Next(_L("Extended error"));
		CBaActiveScheduler::DisplayExtendedError(KUidTest,66);
		test.Next(_L("Leave for alert"));
		CBaActiveScheduler::LeaveForAlert(KUidBaflDll,77);
		CBaActiveScheduler::DisplayExtendedError(KUidTest,88); // won't reach here
		break;
	case 1:
		test.Next(_L("Leave"));
		User::Leave(-22);
		CBaActiveScheduler::DisplayExtendedError(KUidBaflDll,9); // won't reach here
		break;
	case 2:
		test.Next(_L("Leave no alert"));
		CBaActiveScheduler::LeaveNoAlert();
		CBaActiveScheduler::DisplayExtendedError(KUidTest,55); // won't reach here
		break;
	case 3:
		test.Next(_L("Extended error again"));
		CBaActiveScheduler::DisplayExtendedError(KUidBaflDll,33);
		test.Next(_L("Leave for info print"));
		CBaActiveScheduler::LeaveForInfoPrint(KUidTest,11);
		CBaActiveScheduler::DisplayExtendedError(KUidBaflDll,88); // won't reach here
		break;
	case 4:
		test.Next(_L("Simple error handler"));
		CBaActiveScheduler::LeaveForErrorHandler(&iSimpleErrorHandler);
		CBaActiveScheduler::DisplayExtendedError(KUidBaflDll,87); // won't reach here
		break;
	case 5:
		test.Next(_L("Complex error handler - 1"));
		CBaActiveScheduler::LeaveForErrorHandler(&iComplexErrorHandler);
		CBaActiveScheduler::DisplayExtendedError(KUidBaflDll,86); // won't reach here
		break;
	case 6:
		test.Next(_L("Complex error handler - 2"));
		iComplexErrorHandler.SetState(6);
		CBaActiveScheduler::LeaveForErrorHandler(&iComplexErrorHandler);
		CBaActiveScheduler::DisplayExtendedError(KUidBaflDll,85); // won't reach here
		break;
	case 7:
		test.Next(_L("Exit"));
		CBaActiveScheduler::Exit();
		CBaActiveScheduler::DisplayExtendedError(KUidBaflDll,99); // won't reach here
		break;
	case 8:
		CBaActiveScheduler::DisplayExtendedError(KUidBaflDll,999); // won't reach here
		break;
		}
	}

void CTestScheduler::DisplayError(TInt aError)
	{
	TBuf<80> errorText;
	TBuf<80> contextText;
	TErrorHandlerResponse resp;
	switch (iState++)
		{
	case 0:
		test(aError==KErrExtended);
		test(iExtendedError.iComponent==KUidTest);
		test(iExtendedError.iErrorNumber==66);
		test(iExtendedError.iInformation==EFalse);
		break;
	case 1:
		test(aError==KErrExtended);
		test(iExtendedError.iComponent==KUidBaflDll);
		test(iExtendedError.iErrorNumber==77);
		test(iExtendedError.iInformation==EFalse);
		break;
	case 2:
		test(aError==-22);
		break;
	case 3:
		test(aError==KErrExtended);
		test(iExtendedError.iComponent==KUidBaflDll);
		test(iExtendedError.iErrorNumber==33);
		test(iExtendedError.iInformation==EFalse);
		break;
	case 4:
		test(aError==KErrExtended);
		test(iExtendedError.iComponent==KUidTest);
		test(iExtendedError.iErrorNumber==11);
		test(iExtendedError.iInformation);
		break;
	case 5:
		test(aError==KErrExtended);
		test(iExtendedError.iComponent==KUidBaflErrorHandler);
		resp=CBaErrorHandler::CallBack(iExtendedError.iErrorNumber,errorText,contextText);
		test(resp==EAlertDisplay);
		test(errorText==_L("ERROR"));
		test(contextText==_L("CONTEXT"));
		break;
	case 6:
		test(aError==KErrExtended);
		test(iExtendedError.iComponent==KUidBaflErrorHandler);
		resp=CBaErrorHandler::CallBack(iExtendedError.iErrorNumber,errorText,contextText);
		test(resp==ENoDisplay);
		test(errorText==_L("0"));
		test(!contextText.Length());
		break;
	case 7:
		test(aError==KErrExtended);
		test(iExtendedError.iComponent==KUidBaflErrorHandler);
		resp=CBaErrorHandler::CallBack(iExtendedError.iErrorNumber,errorText,contextText);
		test(resp==EInfoDisplay);
		test(errorText==_L("6"));
		test(!contextText.Length());
		break;
	case 8:
		test(EFalse); // should never reach here
		}
	}

/**
@SYMTestCaseID          SYSLIB-BAFL-CT-0442
@SYMTestCaseDesc        Tests for the Active Scheduler
@SYMTestPriority        High
@SYMTestActions         Tests for the functionality of ActiveScheduler
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/
void DoTests()
    {
	test.Next(_L(" @SYMTestCaseID:SYSLIB-BAFL-CT-0442 "));
	CActiveScheduler* as=new(ELeave) CTestScheduler;
	CActiveScheduler::Install(as);
	CActive* ac=CTestActive::NewL();
	TRAPD(ret,as->Start());
	test(ret==KLeaveExit);
	delete(as);
	delete(ac);
    }

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup *cleanup=CTrapCleanup::New();
	test.Title();
	test.Start(_L("Testing CBaActiveScheduler"));
    TRAPD(err,DoTests());
    test(err==KErrNone);
	test.End();
    test.Close();
    delete cleanup;
    __UHEAP_MARKEND;
	return(0);
    }
