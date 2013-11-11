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




inline CConsoleBase& RConsoleLogger::Console() const
	{return iConsole;}

///////

inline RScriptTestServ::RScriptTestServ() : iSharedData(EFalse), iSessionCount(0)
	{}

inline TBool RScriptTestServ::SharedData() const
	{return iSharedData;}

inline TInt RScriptTestServ::SessionCount() const
	{return iSessionCount;}

inline void RScriptTestServ::AddSession()
	{iSessionCount++;}

inline void RScriptTestServ::RemoveSession()
	{iSessionCount--;}

///////

inline void RScriptTestSession::Close()
	{
	iServ->RemoveSession();
	RTestSession::Close();
	}

///////

inline CActiveBase::CActiveBase() : CActive(EPriorityStandard)
	{CActiveScheduler::Add(this);}

/**
 * Abstract class methods for priming, kicking and completing active objects. 
 */
inline void CActiveBase::Kick()
/**
 * Self Kick an active object into its RunL()
 */
	{
	Prime();
	Complete(KErrNone);
	}

inline void CActiveBase::Prime()
/**
 * Self Prime an Active Object for completion
 */
	{
	iStatus = KRequestPending;
	if(!IsActive())
		SetActive();
	}

inline void CActiveBase::Complete(TInt aErr)
/**
 * @param aErr - Error code to complete the Active Object 
 *
 * Self Complete an Active Object 
 */
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,aErr);
	}


inline CActiveBase::~CActiveBase()
	{}

///////
inline void CScriptActive::ChildCompletion(TInt aErr)
/**
 * @param aErr - Error code to complete the Active Object
 * @param aChild - Child object whose work is done
 * Callback from the child.
 * Self complete.
 */
	{
	Complete(aErr);
	}

inline CScriptActive::~CScriptActive()
	{}

inline CScriptActive::CScriptActive()
	{}

///////

inline CTestExecuteLogger& CScriptMaster::Logger() const
	{return iLogger;}

inline RConsoleLogger& CScriptMaster::ConsoleLogger() const
	{return iConsoleLogger;}

inline void CScriptMaster::DoCancel()
	{}

///////
inline CTestExecuteLogger& CScriptControl::Logger() const
	{return iLogger;}

inline RConsoleLogger& CScriptControl::ConsoleLogger() const
	{return iConsoleLogger;}

inline void CScriptControl::DoCancel()
	{}

///////

inline void CTaskTimer::ConstructL()
	{CTimer::ConstructL();}

///////

inline const CTestExecuteLogger& CTaskControlBase::Logger() const
	{return iLogger;}

inline void CTaskControlBase::SetTaskComplete(TBool aTaskCanComplete)
	{
	iTaskCanComplete = aTaskCanComplete;
	}

///////

void CClientControl::DoCancel()
	{}

///////

void CProgramControl::DoCancel()
	{}

///////

