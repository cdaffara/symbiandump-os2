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
	{return iStatus;}

inline void CActiveBase::SetActive()
	{
	if(!IsActive())
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
	if(!IsActive())
		CActive::SetActive();
	}

inline void CActiveBase::Complete(TInt aCode)
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,aCode);
	}

inline	CActiveBase::CActiveBase() : CActive(EPriorityStandard)
	{CActiveScheduler::Add(this);}

inline CActiveBase::~CActiveBase()
	{}

///////
inline const TDesC& CLogFileControl::LogFile()
	{return iLogFileName;}

inline void CLogFileControl::DoCancel()
	{}

inline void CLogFileControl::AddSession()
	{iSessionCount++;}

inline void CLogFileControl::RemoveSession()
	{iSessionCount--;}

inline TInt CLogFileControl::SessionCount() const
	{return iSessionCount;}

inline TBool CLogFileControl::QueueEmpty() const
	{return iQueue.IsEmpty();}

inline void CLogFileControl::AddLogBuffer(CLogBuffer& aBuffer)
	{iQueue.AddLast(aBuffer);}

///////
inline RPointerArray<CLogFileControl>& CLogServer::LogControl()
	{return iControl;}

inline RFs& CLogServer::Fs()
	{return iFs;}

///////
inline const TDesC8& CLogBuffer::Buf()
	{return iLogBuffer;} 

inline TInt CLogBuffer::LinkOffset()
	{return _FOFF(CLogBuffer,iLink);}
