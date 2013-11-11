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

#ifndef __LOGCLIENTOP_H__
#define __LOGCLIENTOP_H__

#include <e32base.h>
#include "logpackage.h"
#include <logcli.h>
#include "logservcli.h"
#include "LogServShared.h"

/**
@internalComponent
*/
NONSHARABLE_CLASS(CLogClientOp) : public CActive
	{
public:
	void Start(TRequestStatus& aObserver);
	TInt Start();

protected:
	CLogClientOp(RLogSession& aSession, CLogPackage& aPackage, TLogOperationType aType, TInt aPriority);
	~CLogClientOp();
	void FetchResultFromServerL(TInt aResult);
	void SetDataSlot1(TInt aValue);

private:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	TInt DoStartL();
	virtual void InitiateRequestToServerL()=0;
	virtual void CompleteL(TInt& aResult);

protected:
	CLogPackage& iPackage;
	RLogSession& iSession;
	TPckgBuf<TLogClientServerData> iData;

private:
	/**
	 * The observer's request status
	 */
	TRequestStatus* iObserver;
	};

/**
Add an event to the database
@internalComponent
*/
NONSHARABLE_CLASS(CLogAddEventClientOp) : public CLogClientOp
	{
public:
	CLogAddEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(CLogEvent& aEvent, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	void CompleteL(TInt& aResult);
	//
private:
	CLogEvent* iEvent;
	};

/**
Change an event in the database
@internalComponent
*/
NONSHARABLE_CLASS(CLogChangeEventClientOp) : public CLogClientOp
	{
public:
	CLogChangeEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(const CLogEvent& aEvent, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	const CLogEvent* iEvent;
	};

/**
Get an event from the database
@internalComponent
*/
NONSHARABLE_CLASS(CLogGetEventClientOp) : public CLogClientOp
	{
public:
	CLogGetEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(CLogEvent& aEvent, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	void CompleteL(TInt& aResult);
	//
private:
	CLogEvent* iEvent;
	};

/**
Delete an event from the database
@internalComponent
*/
NONSHARABLE_CLASS(CLogDeleteEventClientOp) : public CLogClientOp
	{
public:
	CLogDeleteEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(TLogId aId, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	TLogId iId;
	};

/**
Add an event type to the database
@internalComponent
*/
NONSHARABLE_CLASS(CLogAddTypeClientOp) : public CLogClientOp
	{
public:
	CLogAddTypeClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(const CLogEventType& aEventType, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	const CLogEventType* iEventType;
	};

/**
Change an event type in the database
@internalComponent
*/
NONSHARABLE_CLASS(CLogChangeTypeClientOp) : public CLogClientOp
	{
public:
	CLogChangeTypeClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(const CLogEventType& aEventType, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	const CLogEventType* iEventType;
	};

/**
Get event type details
@internalComponent
*/
NONSHARABLE_CLASS(CLogGetTypeClientOp) : public CLogClientOp
	{
public:
	CLogGetTypeClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(CLogEventType& aEventType, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	void CompleteL(TInt& aResult);
	//
private:
	CLogEventType* iEventType;
	};

/**
Delete an event type
@internalComponent
*/
NONSHARABLE_CLASS(CLogDeleteTypeClientOp) : public CLogClientOp
	{
public:
	CLogDeleteTypeClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(TUid aUid, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	TUid iUid;
	};

/**
Get engine configuration
@internalComponent
*/
NONSHARABLE_CLASS(CLogGetConfigClientOp) : public CLogClientOp
	{
public:
	CLogGetConfigClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(TLogConfig& aConfig, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	void CompleteL(TInt& aResult);
	//
private:
	TLogConfig* iConfig;
	};

/**
Change the configuration
@internalComponent
*/
NONSHARABLE_CLASS(CLogChangeConfigClientOp) : public CLogClientOp
	{
public:
	CLogChangeConfigClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(const TLogConfig& aConfig, TRequestStatus& aObserver);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	const TLogConfig* iConfig;
	};

/**
Clear the main event log
@internalComponent
*/
NONSHARABLE_CLASS(CLogClearLogClientOp) : public CLogClientOp
	{
public:
	CLogClearLogClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(const TTime& aDate, TRequestStatus& aObserver
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
			, TSimId aSimId = KLogNullSimId
#endif			
			);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	TTime iDate; // UTC
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TSimId iSimId;
#endif			
	};

/**
Clear a recent event log
@internalComponent
*/
NONSHARABLE_CLASS(CLogClearRecentClientOp) : public CLogClientOp
	{
public:
	CLogClearRecentClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	void Start(TLogRecentList aList, TRequestStatus& aObserver
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
			, TSimId aSimId = KLogNullSimId
#endif			
			);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	TLogRecentList iList;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	TSimId iSimId;
#endif			
	};

/**
Setup a view - Only synch version provided
@internalComponent
*/
NONSHARABLE_CLASS(CLogViewSetupClientOp) : public CLogClientOp
	{
public:
	CLogViewSetupClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	TInt Start(TLogViewId aViewId, const CLogFilterList& aFilterList, TInt aParam, TLogFilterConstructionType aFilterConstructionType);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	TLogViewId iViewId;
	const CLogFilterList* iFilterList;
	TInt iParam;
	};

/**
Remove an event from a view - Only synch version provided
@internalComponent
*/
NONSHARABLE_CLASS(CLogViewRemoveEventClientOp) : public CLogClientOp
	{
public:
	CLogViewRemoveEventClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	TInt Start(TLogViewId aViewId, TLogId aId);
	//
private:
	void InitiateRequestToServerL();
	//
private:
	TLogViewId iViewId;
	TLogId iId;
	};

/**
Do maintenance on the database - for view creation?
@internalComponent
*/
NONSHARABLE_CLASS(CLogMaintainClientOp) : public CLogClientOp
	{
public:
	CLogMaintainClientOp(RLogSession& aSession, CLogPackage& aPackage, TInt aPriority);
	//
private:
	void InitiateRequestToServerL();
	};

#endif
