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


#include "sysif.h"
#include "timerserver.h"
#include "timerhandler.h"
#include "timer.h"
#include <pthread.h>
/* Implementation of the CShutDown class
 * Implements a timer
 */

// -------------------------------------------------------------------------------
// CShutdown::ConstructL
// -------------------------------------------------------------------------------
void CShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

// -------------------------------------------------------------------------------
// CShutdown::Start
// Starts the timer for the specified time given
// -------------------------------------------------------------------------------
void CShutdown::Start()
	{
	CActiveScheduler::Add(this);
	After(KTimerServerShutdownDelay);
	}

// -------------------------------------------------------------------------------
// CShutdown::RunL
// Initiate server exit when timer expires
// -------------------------------------------------------------------------------
void CShutdown::RunL()
    {
    getTimerHandler()->ResetClientFlag();
    getTimerHandler()->ResetServerFlag();
    CActiveScheduler::Stop();
    }

// -------------------------------------------------------------------------------
// CTimerServerSession::CreateL
// 2nd phase construction for sessions. Invoked by the CServer2 framework
// -------------------------------------------------------------------------------
void CTimerServerSession::CreateL()
    {
    Server().AddSession();
    }

//---------------------------------------------------------------------
//CTimerServerSession::~CTimerServerSession
//Destructor of the server side session
//---------------------------------------------------------------------
CTimerServerSession::~CTimerServerSession()
	{
	Server().DropSession();
	}

//---------------------------------------------------------------------
// Handle a client request.
// Leaving is handled by CMyServer::ServiceError() which reports
// the error code to the client
//---------------------------------------------------------------------
void CTimerServerSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	case EDeleteTimer:
		{
		TInt timerid = aMessage.Int0();
		TInt ret = Server().RemoveInternal(timerid);
		aMessage.Complete(ret);
		break;
		}
	case ESetTimer:
			{
			TInt timerid = aMessage.Int0();
			TInt ret = Server().SetTimer(timerid);
			aMessage.Complete(ret);
			break;
			}
	default:
		PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}

CTimerServer& CTimerServerSession::Server()
	{
	return *static_cast<CTimerServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

//------------------------------------------------------------------------------
// Handle an error from CTimerServerSession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
//------------------------------------------------------------------------------

void CTimerServerSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	PanicClient(aMessage,EPanicIllegalFunction);
	CSession2::ServiceError(aMessage,aError);
	}

//-----------------------------------------------------------------------------------------
// Implementation for CTimerServer class
// Implements the functionalities of the timer server
//------------------------------------------------------------------------------------------
CTimerServer* CTimerServer::NewLC()
	{
	CTimerServer* self=new(ELeave) CTimerServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -------------------------------------------------------------------------------
// CTimerServer::ConstructL
// Second phase construction. Create the shutdown timer object and the semaphore
// -------------------------------------------------------------------------------
void CTimerServer::ConstructL()
    {
    CShutdown* shutdown = new(ELeave) CShutdown;
    CleanupStack::PushL(shutdown);
    shutdown->ConstructL();
	CleanupStack::Pop();
	iShutdown = shutdown;
	shutdown = 0;
    }

// -------------------------------------------------------------------------------
// CTimerServer::AddToScheduler
// Add both the server and the shutdown timer objects to the active scheduler
// -------------------------------------------------------------------------------
TInt CTimerServer::AddToScheduler()
	{
	iShutdown->Start();
	return CServer2::Start(KNullDesC);
	}

// -------------------------------------------------------------------------------
// CFileDesTransferServer::RemoveFromScheduler
// Remove both the server and the shutdown timer objects from the active scheduler
// -------------------------------------------------------------------------------
void CTimerServer::RemoveFromScheduler()
	{
	if (iShutdown->IsAdded())
		{
		iShutdown->Deque();
		}
	Deque();
	}

// -------------------------------------------------------------------------------
// CFileDesTransferServer::NewSessionL
// Create a new client session.
// -------------------------------------------------------------------------------
CSession2* CTimerServer::NewSessionL(const TVersion&, const RMessage2&) const
    {
    return new(ELeave) CTimerServerSession;
    }

// -------------------------------------------------------------------------------
// CFileDesTransferServer::AddSession
// A new session is created. Cancel the shutdown timer if it was running.
// -------------------------------------------------------------------------------
inline void CTimerServer::AddSession()
    {
    if (iShutdown->IsAdded())
        {
        iShutdown->Deque();
        }
    ++iSessionCount;
    
    }

// -------------------------------------------------------------------------------
// CTimerServer::DropSession
// The session is destroyed. Stop the ActiveScheduler
// -------------------------------------------------------------------------------
void CTimerServer::DropSession()
    {
    if (--iSessionCount==0)
    	{
    	CTimerReqHandler* handler = getTimerHandler();
    	handler->ResetClientFlag();
    	if ((handler->iTimers.Count()) == 0 )
    		{
    	    getTimerHandler()->ResetServerFlag();
    	    CActiveScheduler::Stop();
    	    }
    	}
    }

//-------------------------------------------------------------------------
// CTimerServer::NewTimerServerL
// creates an active scheduler and installs it.
//-------------------------------------------------------------------------
TInt CTimerServer::NewTimerServerL()
	{
	CActiveScheduler* pScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(pScheduler);
	
	CTimerServer* pServer = CTimerServer::NewLC();
	
	CActiveScheduler::Install(pScheduler);

	TInt err = pServer->AddToScheduler();
    	if (err != KErrNone)
    		{
    		pServer->RemoveFromScheduler();
    		CActiveScheduler::Install(NULL);
    		User::Leave(err);
    		}
    	getTimerHandler()->SetServerHandle(pServer->Server());
	// pop both pScheduler and pServer from the cleanup stack
    	// Start the scheduler
    	RThread().Rendezvous(KErrNone);
    	CActiveScheduler::Start();
	CActiveScheduler::Install(NULL);
	
	CleanupStack::PopAndDestroy(2);
	return KErrNone;
	}
// -------------------------------------------------------------------------------
// CFileDesTransferServer::~CFileDesTransferServer
// -------------------------------------------------------------------------------
CTimerServer::~CTimerServer()
	{
	if(iShutdown)
		{
		delete iShutdown;
		iShutdown = 0;
		}
	}

//---------------------------------------------------------------------------------
// CTimerServer:: RemoveInternal
// Removing the timer object from the list 
//---------------------------------------------------------------------------------
TInt CTimerServer:: RemoveInternal(const TInt aTimerId)
	{
	TInt lTimerIdx = 0;
	TInt lRet = KErrArgument;
	TBool lSigReq = EFalse;
	CTimerReqHandler* handler = getTimerHandler();
	handler->iTimersLock.Wait();
	int lTimerCnt = handler->iTimers.Count();

	for(lTimerIdx = 0; lTimerIdx != lTimerCnt; lTimerIdx++)
		{
		if(handler->iTimers[lTimerIdx]->iTimerId == aTimerId)
			{
			if (handler->iTimers[lTimerIdx]->iSigEvent.sigev_notify == SIGEV_SIGNAL)
				lSigReq = ETrue;
			RHeap* oldHeap = User::SwitchHeap(Backend()->Heap());
			delete handler->iTimers[lTimerIdx];
			handler->iTimers.Delete(lTimerIdx);
			User::SwitchHeap(oldHeap);
			lRet = KErrNone;
			break;
			}
		}	
	if(lRet == KErrNone && lSigReq)
		{
		RHeap* oldHeap = User::SwitchHeap(Backend()->Heap());
		handler->iTimers.Compress();	
		User::SwitchHeap(oldHeap);
		#if (defined SYMBIAN_OE_POSIX_SIGNALS && defined SYMBIAN_OE_LIBRT)		
		Backend()->DeleteTimer(aTimerId);
		#endif		
		}
	handler->iTimersLock.Signal();
	if(handler->iTimers.Count() == 0)
		{
		DropSession();
		}
	return lRet;
	}

//thread startup function for SIGEV_THREAD
static TAny* sThreadFunc(TAny* aArgPtr)
	{
	struct sigevent* lTimerP = reinterpret_cast<struct sigevent *> (aArgPtr);
	lTimerP->sigev_notify_function(lTimerP->sigev_value);	
	return NULL;
	}

//---------------------------------------------------------------------------------
// CTimerServer:: SetTimer
// Server sets a new value for the timeout.
//---------------------------------------------------------------------------------
TInt CTimerServer:: SetTimer(const TInt aTimerId)
	{
	CRtTimer* lTimer = getTimerHandler()->FindTimer(aTimerId);
	if(lTimer)
		{
		TTime lSetTime(MAKE_TINT64 (0x00dcddb3 ,0x0f2f8000)); 
		if (lTimer->iIsArmed)
			{
			lTimer->iTimer.Cancel();	
			if(lTimer->iIsTimerReset)
				{
				lTimer->iTimer.Deque();
				lTimer->iIsArmed = EFalse;
				return 0;
				}
			}
		else
			lTimer->iTimer.ConstructL();
		
		lSetTime+=(TTimeIntervalSeconds) lTimer->iEndTime.tv_sec;
		lSetTime+=(TTimeIntervalMicroSeconds)(lTimer->iEndTime.tv_nsec/1000);
		
		lTimer->iTimer.AddToAS();
		lTimer->iTimer.At(lSetTime);	//Before requesting a timer event, add this active object to AS.					
		lTimer->iIsArmed = ETrue;										
		}		
	return KErrNone;
	}

void CLibRtTimer::AddToAS()
	{
	if(!IsAdded())
		CActiveScheduler::Add(this);
	else return;
	}

void CLibRtTimer::RunL()
	{
	CLocalSystemInterface* lClsiPtr = Backend();
		
		unsigned long lPeriodicTimeout = (1000000 * GetRtPtr()->iStartTime.it_interval.tv_sec) +
			  (GetRtPtr()->iStartTime.it_interval.tv_nsec/1000);
			  
		if(lPeriodicTimeout)
			{
			this->HighRes(lPeriodicTimeout);
			}
		else
			{
			GetRtPtr()->iIsArmed = EFalse;
			}						
		int val = GetRtPtr()->iSigEvent.sigev_notify;
		//run the handler here	
		switch(GetRtPtr()->iSigEvent.sigev_notify)
			{
			#if (defined SYMBIAN_OE_POSIX_SIGNALS && defined SYMBIAN_OE_LIBRT)
			case SIGEV_SIGNAL:
				{
				lClsiPtr->AddTimer(GetRtPtr()->iTimerId);	
				
				if(lClsiPtr->IncrementOverrun(GetRtPtr()->iTimerId) == 1)
					{
					lClsiPtr->RaiseTimerSignal(GetRtPtr()->iSigEvent.sigev_signo,\
							GetRtPtr()->iTimerId);
					}
				}
				break;
			#endif
			case SIGEV_THREAD:
				{
				pthread_t lthread = 0;
				pthread_create(	&lthread,
								(pthread_attr_t*)GetRtPtr()->iSigEvent.sigev_notify_attributes,
								sThreadFunc,
								&GetRtPtr()->iSigEvent );
				}
				break;

			case SIGEV_NONE:
			default:
				break;						
			}
	}

// --------------------------------------------------------------------------- 
// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
// ---------------------------------------------------------------------------
void PanicClient(const RMessagePtr2& aMessage,TMyPanic aPanic)
	{
	_LIT(KPanic,"TimerServer");
	aMessage.Panic(KPanic,aPanic);
	}

