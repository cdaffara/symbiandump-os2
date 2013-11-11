// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : timerhandler.cpp
// Part of     : librt-timer specific cpp file
// This is a project specific source file for building the 
// timer related functions as part of librt library.
// 
//


#include "sysif.h"
#include "timerhandler.h"
#include "timerclient.h"
#include "timerserver.h"
#include "timermessage.h"
#include "timer.h"
#include <pthread.h>

#ifdef __WINSCW__ 
#include <pls.h> // For emulator WSD API 
const TUid KLibrtUid3 = {0x2001E553}; 
#elif defined __X86GCC__
CTimerReqHandler gTimerHandler;
#endif


CTimerReqHandler* getTimerHandler()
	{
#ifdef __WINSCW__	
	// Access the PLS of this process
	CTimerReqHandler* lTimerHandlerPtr = Pls<CTimerReqHandler>(KLibrtUid3); 
	return lTimerHandlerPtr;  
#elif defined __X86GCC__
	return &gTimerHandler;
#else
	static CTimerReqHandler sgTimerHandler;
	return &sgTimerHandler;
#endif
	}


 CTimerReqHandler::CTimerReqHandler() : iTimers(CTimerReqHandler::KTimersGran)
	{
	iTimersLock.CreateLocal();
	iServConnectLock.CreateLocal();
	iTimerSemaphore.CreateLocal(0);
	}


 CTimerReqHandler::~CTimerReqHandler()
	{
	iTimersLock.Close();
	iTimers.ResetAndDestroy();
	iTimerSemaphore.Close();
	iServ.Close();
	iServConnectLock.Close();
	}

//method to create a timer
TInt CTimerReqHandler::CreateTimer(TInt& aTimerId, struct sigevent *aSig)
	{
	TInt lRet = KErrNone;
	if(iTimers.Count() >= MAXTIMERLIMIT)
		{
		lRet = KErrWouldBlock;
		}
		
	if(lRet == KErrNone)
		{
		if(aSig != NULL)
			{
			switch(aSig->sigev_notify)	
				{
				case SIGEV_SIGNAL:
#if (!defined SYMBIAN_OE_POSIX_SIGNALS || !defined SYMBIAN_OE_LIBRT)
					{
					aTimerId = -1;
					lRet = KErrNotSupported;
					break;	
					}
#else			
				
				if(aSig->sigev_signo < 1 || aSig->sigev_signo > SIGRTMAX)
					{
					aTimerId = -1;
					lRet = KErrArgument;						
					}
#endif	
					
				case SIGEV_NONE:					
				case SIGEV_THREAD:
					break;
				default:
					aTimerId = -1;	
					lRet = KErrArgument;
					break;
				}
			}
			
		if(lRet == KErrNone)	
			{
			RHeap* oldHeap = User::SwitchHeap(Backend()->Heap());
			CRtTimer *lTimer = CRtTimer::New(aSig);
			if(NULL == lTimer)
				{
				aTimerId = -1;	
				lRet = KErrNoMemory;
				User::SwitchHeap(oldHeap);
				}
			else
				{
				aTimerId = lTimer->iTimerId;				
				iTimersLock.Wait();	
				TRAP(lRet, iTimers.AppendL(lTimer));				
				User::SwitchHeap(oldHeap);
				iTimersLock.Signal();			
				}	
			}
		}
		
	return lRet;	
	}

//method to delete a timer
TInt CTimerReqHandler::RemoveTimer(const TInt& aTimerId)
	{
	if(FindTimer(aTimerId)== NULL)
		{
		return KErrArgument;
		}
	TInt lRet = Connect();
	if(lRet == KErrNone)
		{
		session.OnDemandConnect(iServer);
		lRet = session.DeleteTimer(aTimerId);
		}	
	return lRet;		
	}

//method to set a new timeout value for a timer
TInt CTimerReqHandler::SetTime(const TInt& aTimerId, TInt aFlag, const struct itimerspec *aIpTime,
			 struct itimerspec *aOpTime) 
	{
	TInt lRet = Connect();
	if(lRet == KErrNone)
		{
		CRtTimer* lTimer = FindTimer(aTimerId);	
		if(lTimer != NULL)
			lRet = lTimer->SetTime(aFlag, aIpTime, aOpTime);	
		else
			lRet = KErrArgument;	
		}
		
	return lRet;	
	}

//method to makesure that the timer server is started only once on demand.
TInt CTimerReqHandler::Connect()
	{
	TInt lRet = KErrNone;
	
	iServConnectLock.Wait();
	if(!iIsServStarted)
		{
		lRet = StartTimerServer();
		if(lRet == KErrNone)
			iIsServStarted = ETrue;
		}
	iServConnectLock.Signal();
	return lRet;		
	}

//start up function for the timer server.
static TInt sTimerServer(TAny*)
	{
	RHeap *oldHeap = User::SwitchHeap(Backend()->Heap());
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(ret, CTimerServer::NewTimerServerL());
	delete cleanup;
	User::SwitchHeap(oldHeap);
	return ret;
	}

//method to start the timer server
TInt CTimerReqHandler::StartTimerServer()
	{
	TRequestStatus status;
	TInt lRet = iServ.Create(KNullDesC, sTimerServer, 
			KDefaultStackSize, NULL, (TAny*)NULL);
	if(lRet == KErrNone)							
		{
		iServ.SetPriority(EPriorityAbsoluteHigh);// match the signal server priority.
		iServ.Rendezvous(status);
		iServ.Resume(); //get this ready
		User::WaitForRequest(status);
		}
	return lRet;
	}

//method to get the "time to expiration" of a timer
TInt CTimerReqHandler::Time(const TInt& aTimerId, struct itimerspec *aTime)
	{
 	CRtTimer* lTimer = FindTimer(aTimerId);
	if(lTimer == NULL)
		return KErrArgument;	
	return lTimer->Time(aTime);
	}

//method to get the overruns for a timer
TInt CTimerReqHandler::OverrunCount(const TInt& aTimerId, TInt& aOverrunCount)
	{
	CRtTimer* lTimer = FindTimer(aTimerId);
	if(lTimer == NULL)
		return KErrArgument;		
	return lTimer->OverrunCount(aOverrunCount);
	}

//find the timer with the given timer id
CRtTimer* CTimerReqHandler::FindTimer(const TInt& aTimerId)
	{
	CRtTimer* lRtTimerP = NULL;
	iTimersLock.Wait();	
	TInt lTimerCount = iTimers.Count();	
	for(TInt lIdx =0; lIdx < lTimerCount; lIdx++)
		{
		if(iTimers[lIdx]->iTimerId == aTimerId)
			{
			lRtTimerP = (iTimers[lIdx]);
			break;	
			}
		}
	iTimersLock.Signal();
	return lRtTimerP;	
	}

