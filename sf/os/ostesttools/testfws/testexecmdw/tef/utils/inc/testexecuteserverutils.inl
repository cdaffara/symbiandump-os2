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




inline TRequestStatus& CActiveBase::Status()
	{
	return iStatus;
	}

inline void CActiveBase::SetActive()
	{
	if (!IsActive())
		CActive::SetActive();
	}

inline void CActiveBase::Kick()
	{
	Prime();
	Complete(KErrNone);
	}

inline void CActiveBase::Prime()
	{
	iStatus = KRequestPending;
	if (!IsActive())
		CActive::SetActive();
	}

inline void CActiveBase::Complete(TInt aCode)
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, aCode);
	}

inline CActiveBase::CActiveBase() :
	CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

inline CActiveBase::~CActiveBase()
	{
	}

inline TDes& CActiveBase::Error()
	{
	return iError;
	}

inline TBool& CActiveBase::TimedOut()
	{
	return iTimeOut;
	}

///////

inline CControlBase::CControlBase(CTestServer& aServer) :
	iServer(aServer)
	{
	}

inline CTestServer& CControlBase::Server()
	{
	return iServer;
	}

inline TDes& CControlBase::Args()
	{
	return iArgs;
	}

inline RMessage2& CControlBase::Message()
	{
	return iMessage;
	}

inline RThread& CControlBase::WorkerThread()
	{
	return iWorkerThread;
	}

inline void CControlBase::DoCancel()
	{
	}

///////

inline void CPersistentStepControl::ChildCompletion()
	{
	Complete(KErrNone);
	}

///////

inline void CPersistentBlockControl::ChildCompletion()
	{
	Complete(KErrNone);
	}

///////

inline CWorkerMonitor::CWorkerMonitor(TRequestStatus& aParentStatus) :
	iParentStatus(aParentStatus)
	{
	}

inline CWorkerMonitor::~CWorkerMonitor()
	{
	}

inline void CWorkerMonitor::RunL()
	{
	TRequestStatus* status = &iParentStatus;
	User::RequestComplete(status, KErrNone);
	}

inline void CWorkerMonitor::DoCancel()
	{
	}

///////

inline CWorkerControl::CWorkerControl(CTestServer& aServer,
		TRequestStatus& aParentStatus) :
	iServer(aServer), iParentStatus(aParentStatus)
	{
	}

inline CWorkerControl::~CWorkerControl()
	{
	}

inline void CWorkerControl::RunL()
	{
	TRequestStatus* status = &iParentStatus;
	User::RequestComplete(status, KErrNone);
	}

inline void CWorkerControl::DoCancel()
	{
	}

inline TRequestStatus& CWorkerControl::WorkerStatus()
	{
	return iWorkerStatus;
	}

inline RThread& CWorkerControl::ControllerThread()
	{
	return iControllerThread;
	}

inline RSemaphore& CWorkerControl::Semaphore()
	{
	return iSemaphore;
	}

inline CTestServer& CWorkerControl::Server()
	{
	return iServer;
	}

inline TInt& CWorkerControl::Result()
	{
	return iResult;
	}

inline TPtrC& CWorkerControl::Args()
	{
	return iArgs;
	}

inline TPtrC& CWorkerControl::StepName()
	{
	return iStepName;
	}

inline TDes& CWorkerControl::PersistentError()
	{
	return iPersistentError;
	}

//defect 116046
inline void CWorkerControl::SetCleanupPtr(CTrapCleanup* aCleanup)
	{
	iCleanup = aCleanup;
	}

inline void CWorkerControl::Cleanup()
	{
	if (iCleanup)
		delete iCleanup;
	iCleanup = NULL;
	}
//END defect 116046

///////

inline CBlockWorkerControl::CBlockWorkerControl(CTestServer& aServer,
		TRequestStatus& aParentStatus) :
	CWorkerControl(aServer, aParentStatus)
	{
	}

inline CBlockWorkerControl::~CBlockWorkerControl()
	{
	}

inline void CBlockWorkerControl::SetBlockArray(TTEFItemArray* aBlockArray)
	{
	iBlockArray = aBlockArray;
	}

inline TTEFItemArray* CBlockWorkerControl::BlockArray() const
	{
	return iBlockArray;
	}

///////

inline CStepControlBase::CStepControlBase(CTestServer& aServer) :
	CControlBase(aServer)
	{
	}

inline TDes& CStepControlBase::StepName()
	{
	return iStepName;
	}

///////

inline CBlockControlBase::CBlockControlBase(CTestServer& aServer) :
	CControlBase(aServer), iBlockArray(NULL)
	{
	}
