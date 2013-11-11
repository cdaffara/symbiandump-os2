// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "logclientop.h"

// User includes
#include "logservcli.h"


CLogClientOp::CLogClientOp(RLogSession& aSession, CLogPackage& aPackage, TLogOperationType aType, TInt aPriority)
:	CActive(aPriority), iPackage(aPackage), iSession(aSession)
//
//	Base client side operation
//
	{
	iData().iOperationType = aType;
	CActiveScheduler::Add(this);
	}

CLogClientOp::~CLogClientOp()
	{
	Cancel();
	}

void CLogClientOp::Start(TRequestStatus& aObserver)
//
//	Start the ball rolling - InitiateRequestToServerL called in RunL so it can leave
//
	{
	iObserver = &aObserver;
	aObserver = KRequestPending;

	// Just complete ourselves
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

TInt CLogClientOp::Start()
//
//	Synchronous way of running operations
//
	{
	TInt result = KErrNone;
	TRAP(result, result = DoStartL());
	iData().iOperationId = KLogNullOperationId;
	return result;
	}

TInt CLogClientOp::DoStartL()
	{
	iData().iOperationId = iSession.AllocateIdOperation();
	InitiateRequestToServerL();
	User::WaitForRequest(iStatus);
	//We have an asynchronous request completed synchronously. 
	//We have to do some iStatus cleanups.
	TRequestStatus status;//New TRequestStatus local variable. The default constructor will set iFlags data memebr to 0.
	status = iStatus.Int();//Only iStatus data member getzs initialized.
	iStatus = status;//Return back the same iStatus value but with iFlags data memeber cleared.
	User::LeaveIfError(iStatus.Int());

	TInt result = iStatus.Int();
	CompleteL(result);
	iData().iOperationId = KLogNullOperationId;
	return result;
	}

void CLogClientOp::RunL()
	{
	LOGTEXT2("CLogClientOp::RunL(%d)", iStatus.Int());

	User::LeaveIfError(iStatus.Int());

	// Set ourselves up - make the actual request?
	if (iData().iOperationId == KLogNullOperationId)
		{
		// Get the id of the operation
		iData().iOperationId = iSession.AllocateIdOperation();

		InitiateRequestToServerL();
		SetActive();
		}
	else
		{
		// Finish off the request
		iData().iOperationId = KLogNullOperationId;
		TInt result = iStatus.Int();
		CompleteL(result);
		User::RequestComplete(iObserver, result);
		}

	LOGTEXT("CLogClientOp::RunL() - end");
	}

void CLogClientOp::DoCancel()
//
//	Cancel the request to the server if we initiated one
//
	{
	LOGTEXT2("CLogClientOp::DoCancel() - OperationId: %d", iData().iOperationId);

	// Cancel this operation if we have an id
	if (iData().iOperationId > 0)
		{
		iSession.Send(ELogOperationCancel, TIpcArgs(&iData));
		//
		iData().iOperationId = KLogNullOperationId;
		}

	User::RequestComplete(iObserver, KErrCancel);
	LOGTEXT("CLogClientOp::DoCancel() - end");
	}

// Just complete the observer on error
TInt CLogClientOp::RunError(TInt aError)
	{
	iData().iOperationId = KLogNullOperationId;
	User::RequestComplete(iObserver, aError);
	return KErrNone;
	}

void CLogClientOp::CompleteL(TInt& /*aCompletionCode*/)
//
//	By default operations don't do anything after completion
//
	{
	}

void CLogClientOp::FetchResultFromServerL(TInt aResult)
	{
	iPackage.ResizeL(aResult);
	TPtr8 ptr(iPackage.Ptr());

	User::LeaveIfError(iSession.Send(ELogOperationGetResult, TIpcArgs(&iData,&ptr,ptr.Length())));
	}

/** 
 * Initialise the data slot values
 */
void CLogClientOp::SetDataSlot1(TInt aValue)
	{
	iData().iDataSlot1 = aValue;
	}







CLogAddEventClientOp::CLogAddEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationEventAdd, aPriority)
	{
	}

void CLogAddEventClientOp::Start(CLogEvent& aEvent, TRequestStatus& aObserver)
	{
	iEvent = &aEvent;
	CLogClientOp::Start(aObserver);
	}

void CLogAddEventClientOp::InitiateRequestToServerL()
	{
	iPackage.SetLogEventL(*iEvent);
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, &iPackage.Ptr()), iStatus);
	}

void CLogAddEventClientOp::CompleteL(TInt& aResult)
	{
	FetchResultFromServerL(aResult);
	iPackage.GetLogEventL(*iEvent);
	}







CLogChangeEventClientOp::CLogChangeEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationEventChange, aPriority)
	{
	}

void CLogChangeEventClientOp::Start(const CLogEvent& aEvent, TRequestStatus& aObserver)
	{
	iEvent = &aEvent;
	CLogClientOp::Start(aObserver);
	}

void CLogChangeEventClientOp::InitiateRequestToServerL()
	{
	iPackage.SetLogEventL(*iEvent);
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, &iPackage.Ptr()), iStatus);
	}






CLogGetEventClientOp::CLogGetEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationEventGet, aPriority)
	{
	}

void CLogGetEventClientOp::Start(CLogEvent& aEvent, TRequestStatus& aObserver)
	{
	iEvent = &aEvent;
	CLogClientOp::Start(aObserver);
	}

void CLogGetEventClientOp::InitiateRequestToServerL()
	{
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, iEvent->Id()), iStatus);
	}

void CLogGetEventClientOp::CompleteL(TInt& aResult)
	{
	FetchResultFromServerL(aResult);
	iPackage.GetLogEventL(*iEvent);
	}







CLogDeleteEventClientOp::CLogDeleteEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationEventDelete, aPriority)
	{
	}

void CLogDeleteEventClientOp::Start(TLogId aId, TRequestStatus& aObserver)
	{
	iId = aId;
	CLogClientOp::Start(aObserver);
	}

void CLogDeleteEventClientOp::InitiateRequestToServerL()
	{
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, iId), iStatus);
	}









CLogAddTypeClientOp::CLogAddTypeClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationTypeAdd, aPriority)
	{
	}

void CLogAddTypeClientOp::Start(const CLogEventType& aEventType, TRequestStatus& aObserver)
	{
	iEventType = &aEventType;
	CLogClientOp::Start(aObserver);
	}

void CLogAddTypeClientOp::InitiateRequestToServerL()
	{
	iPackage.SetLogEventTypeL(*iEventType);
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, &iPackage.Ptr()), iStatus);
	}








CLogChangeTypeClientOp::CLogChangeTypeClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationTypeChange, aPriority)
	{
	}

void CLogChangeTypeClientOp::Start(const CLogEventType& aEventType, TRequestStatus& aObserver)
	{
	iEventType = &aEventType;
	CLogClientOp::Start(aObserver);
	}

void CLogChangeTypeClientOp::InitiateRequestToServerL()
	{
	iPackage.SetLogEventTypeL(*iEventType);
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, &iPackage.Ptr()), iStatus);
	}







CLogGetTypeClientOp::CLogGetTypeClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationTypeGet, aPriority)
	{
	}

void CLogGetTypeClientOp::Start(CLogEventType& aEventType, TRequestStatus& aObserver)
	{
	iEventType = &aEventType;
	CLogClientOp::Start(aObserver);
	}

void CLogGetTypeClientOp::InitiateRequestToServerL()
	{
	TIpcArgs args(&iData, iEventType->Uid().iUid);
	iSession.Send(ELogOperationInitiate, args, iStatus);
	}

void CLogGetTypeClientOp::CompleteL(TInt& aResult)
	{
	FetchResultFromServerL(aResult);
	iPackage.GetLogEventTypeL(*iEventType);
	}








CLogDeleteTypeClientOp::CLogDeleteTypeClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationTypeDelete, aPriority)
	{
	}

void CLogDeleteTypeClientOp::Start(TUid aUid, TRequestStatus& aObserver)
	{
	iUid = aUid;
	CLogClientOp::Start(aObserver);
	}

void CLogDeleteTypeClientOp::InitiateRequestToServerL()
	{
	TIpcArgs args(&iData, iUid.iUid);
	iSession.Send(ELogOperationInitiate, args, iStatus);
	}








CLogGetConfigClientOp::CLogGetConfigClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationConfigGet, aPriority)
	{
	}

void CLogGetConfigClientOp::Start(TLogConfig& aConfig, TRequestStatus& aObserver)
	{
	iConfig = &aConfig;
	CLogClientOp::Start(aObserver);
	}

void CLogGetConfigClientOp::InitiateRequestToServerL()
	{
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData), iStatus);
	}

void CLogGetConfigClientOp::CompleteL(TInt& aResult)
	{
	FetchResultFromServerL(aResult);
	iPackage.GetLogConfigL(*iConfig);
	}









CLogChangeConfigClientOp::CLogChangeConfigClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationConfigChange, aPriority)
	{
	}

void CLogChangeConfigClientOp::Start(const TLogConfig& aConfig, TRequestStatus& aObserver)
	{
	iConfig = &aConfig;
	CLogClientOp::Start(aObserver);
	}

void CLogChangeConfigClientOp::InitiateRequestToServerL()
	{
	iPackage.SetLogConfigL(*iConfig);
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, &iPackage.Ptr()), iStatus);
	}









CLogClearLogClientOp::CLogClearLogClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority) :
	CLogClientOp(aSession, aPackage, ELogOperationClearLog, aPriority)
	{
	}

void CLogClearLogClientOp::Start(const TTime& aDate, TRequestStatus& aObserver
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		, TSimId aSimId
#endif		
		)
	{
	iDate = aDate; // UTC
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	iSimId = aSimId;
#endif	
	CLogClientOp::Start(aObserver);
	}

void CLogClearLogClientOp::InitiateRequestToServerL()
	{
	const TInt64 dateVal(iDate.Int64());
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TIpcArgs args(&iData, I64LOW(dateVal), I64HIGH(dateVal), iSimId);
#else
	TIpcArgs args(&iData, I64LOW(dateVal), I64HIGH(dateVal));
#endif
	iSession.Send(ELogOperationInitiate, args, iStatus);
	}








CLogClearRecentClientOp::CLogClearRecentClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationClearRecent, aPriority)
	{
	}

void CLogClearRecentClientOp::Start(TLogRecentList aList, TRequestStatus& aObserver
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
		, TSimId aSimId
#endif		
		)
	{
	iList = aList;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	iSimId = aSimId;
#endif	
	CLogClientOp::Start(aObserver);
	}

void CLogClearRecentClientOp::InitiateRequestToServerL()
	{
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, iList, iSimId), iStatus);
#else
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData, iList), iStatus);
#endif
	}







CLogMaintainClientOp::CLogMaintainClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationMaintain, aPriority)
	{
	}

void CLogMaintainClientOp::InitiateRequestToServerL()
	{
	iSession.Send(ELogOperationInitiate, TIpcArgs(&iData), iStatus);
	}








CLogViewSetupClientOp::CLogViewSetupClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationViewSetup, aPriority)
	{
	}

TInt CLogViewSetupClientOp::Start(TLogViewId aViewId, const CLogFilterList& aFilterList, TInt aParam, TLogFilterConstructionType aFilterConstructionType)
	{
	// Synchronous!
	iViewId = aViewId;
	iFilterList = &aFilterList;
	iParam = aParam;
	SetDataSlot1(aFilterConstructionType);
	//
	return CLogClientOp::Start();
	}

void CLogViewSetupClientOp::InitiateRequestToServerL()
	{
	TIpcArgs args (&iData, iViewId, &iPackage.Ptr(), iParam);
	iSession.Send(ELogViewOperationInitiate, args, iStatus);
	}







CLogViewRemoveEventClientOp::CLogViewRemoveEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority)
:	CLogClientOp(aSession, aPackage, ELogOperationViewRemoveEvent, aPriority)
	{
	}

TInt CLogViewRemoveEventClientOp::Start(TLogViewId aViewId, TLogId aId)
	{
	// Synchronous!
	iViewId = aViewId;
	iId = aId;
	return CLogClientOp::Start();
	}

void CLogViewRemoveEventClientOp::InitiateRequestToServerL()
	{
	TIpcArgs args(&iData, iViewId, iId, 0);
	iSession.Send(ELogViewOperationInitiate, args, iStatus);
	}





