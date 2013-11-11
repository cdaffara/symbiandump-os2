/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "threadedlogger.h"

CThreadedLogger* CThreadedLogger::NewLC(MOmxScriptTestLogger& aRealLogger)
	{
	CThreadedLogger* self = new(ELeave) CThreadedLogger(aRealLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CThreadedLogger::CThreadedLogger(MOmxScriptTestLogger& aRealLogger):
CActive(EPriorityHigh),
iRealLogger(aRealLogger)
	{
	iCreatorThreadId = iCreatorThread.Id();
	}

void CThreadedLogger::ConstructL()
	{
	User::LeaveIfError(iCreatorThread.Open(iCreatorThreadId));
	User::LeaveIfError(iMutex.CreateLocal());
	User::LeaveIfError(iSemaphore.CreateLocal(0));
	CActiveScheduler::Add(this);
	iStatus = KRequestPending;
	SetActive();
	}

CThreadedLogger::~CThreadedLogger()
	{
	Cancel();
	iSemaphore.Close();
	iMutex.Close();
	iCreatorThread.Close();
	}

void CThreadedLogger::Log(const TText8* aFile, TInt aLine, TOmxScriptSeverity aSeverity, const TDes& aMessage)
	{
	if(RThread().Id() == iCreatorThreadId)
		{
		iRealLogger.Log(aFile, aLine, aSeverity, aMessage);
		}
	else
		{
		// proxy the log to creator thread
		
		// mutex prevents multiple threads to proxy at the same time
		iMutex.Wait();
		
		// const_cast is regrettable but all we do is pass the params onto a another MOmxScriptTestLogger which will
		// also treat the args as const
		iFile = const_cast<TText8*>(aFile);
		iLine = aLine;
		iSeverity = aSeverity;
		iMessage = &const_cast<TDes&>(aMessage);
		
		// signal the creator thread (waking the Active Object)
		TRequestStatus* statusPtr = &iStatus;
		iCreatorThread.RequestComplete(statusPtr, KErrNone);
		
		// wait for creator thread to signal back
		
		iSemaphore.Wait();
		iMutex.Signal();
		}
	}

void CThreadedLogger::RunL()
	{
	if(iStatus.Int() == KErrCancel)
		{
		return;
		}
	iRealLogger.Log(iFile, iLine, iSeverity, *iMessage);
	iFile = NULL;
	iLine = 0;
	iSeverity = (TOmxScriptSeverity) 0;
	iMessage = NULL;
	iStatus = KRequestPending;
	SetActive();
	iSemaphore.Signal();	// signal the requestor thread
	}

void CThreadedLogger::DoCancel()
	{
	TRequestStatus* statusPtr = &iStatus;
	User::RequestComplete(statusPtr, KErrCancel);
	}
