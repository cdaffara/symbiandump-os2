// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// defines scheduler panics & faults
// CSchServer - server class
// CSchSession - server-side session class
// 
//

#ifndef __SSCH_STD_H__
#define __SSCH_STD_H__

// System includes
#include <e32base.h>
#include <bsul/bsul.h>

// Classes referenced
class CSheduleServerLog;
class CTaskScheduler;
class CClientProxy;
class CSchStartupStateMgr;
class CSchedule;
class CClientMessage;

enum TESchPanic
	{
	EPanicNotRegistered,
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	};

enum TESchFault
	{
	EMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ETaskWithoutClient,
	ECreateTrapCleanup,
	ENotImplementedYet,
	};


NONSHARABLE_CLASS(CSchServer) : public CServer2
	{
private:
	enum {EPriority=1000};

public:
	static CSchServer* NewLC();
	~CSchServer();
	CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;

private:
	CSchServer(TInt aPriority);
	void ConstructL();
private:
	// Server owns one & only instance of CTaskScheduler
	CTaskScheduler* iTaskScheduler;

	CSchStartupStateMgr* iSSAMgr;

#ifdef __SCHLOGGING__
	CSheduleServerLog* iTheLog;
#endif	
	};

NONSHARABLE_CLASS(CSchSession) : public CSession2
	{
public:
	void ServiceL(const RMessage2 &aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	virtual ~CSchSession();
	CSchSession(CTaskScheduler& aScheduler);
	
private:
	//request handlers
	void RegisterClientL();
	void CreateTimeScheduleL();
	void CreateConditionScheduleL();
	void ScheduleTaskL();
	void DeleteTaskL();
	void EditTimeScheduleL();
	void EditConditionScheduleL();
	//retrieval
	void GetScheduleRefsL();//name & handle for each schedule
	void CountSchedulesL();
	void GetTimeScheduleDataL();
	void GetConditionScheduleDataL();
	void GetTaskDataL();

	void GetScheduleInfoL();
	void GetTaskRefsL();//name & handle for each task
	void GetTaskInfoL();
	void GetScheduleItemRefAndDueTimeL(); 
	void GetScheduleTypeL(); 

	void GetTaskDataSizeL();
	void CountTasksL();
	
	void CheckCapabilityL();
	void CheckPersistsInBackupL(const CSchedule& aSchedule);
	void DoServiceL();
private:	
	//Session is initialised with the one and only instance of CTaskScheduler
	CTaskScheduler* iTaskScheduler;
	//NULL on creation, assigned when registers
	CClientProxy* iClient;
	// current request for this client.  NULL on creation
	RMessage2 iPendingNotification;
	
	BSUL::CClientMessage *iClientMessage;
	};

#endif

