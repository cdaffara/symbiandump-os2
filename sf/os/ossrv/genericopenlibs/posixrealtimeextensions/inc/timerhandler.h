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
// timer handler header file
// 
//


#ifndef __TIMERHANDLER__H
#define __TIMERHANDLER__H

#include "timer.h"
#include "timerserver.h"
#include "timerclient.h"

#define MAXTIMERLIMIT 512
#define DELAYTIMER_MAX 32

class CTimerReqHandler: public CBase
{
public:	
	friend class CTimerServer;
	friend class CLibRtTimer;
	TInt CreateTimer(TInt& aTimerId, struct sigevent *aSig);
	TInt RemoveTimer(const TInt& aTimerId);	
	TInt Time(const TInt& aTimerId, struct itimerspec *aTime);
	TInt SetTime(const TInt& aTimerId, TInt aFlag, const struct itimerspec *aIpTime,
			 struct itimerspec *aOpTime);	
	TInt OverrunCount(const TInt& aTimerId, TInt& aOverrunCount);
	TInt TimerRequestHandler();
	
	CTimerReqHandler();	 						   							   
	~CTimerReqHandler();
	RTimerSession session;
	inline void SetServerHandle(RServer2 aServer)
		{
		iServer = aServer;
		}
	RServer2 iServer;
	inline void ResetClientFlag()
	    {
	    session.ResetConnectionFlag();
	    }
	inline void ResetServerFlag()
		{
		iServConnectLock.Wait();
		iIsServStarted = EFalse;
		iServConnectLock.Signal();
		}
private:
	TInt Connect();
	CRtTimer* FindTimer(const TInt& aTimerId);
	TInt StartTimerServer();

	const static TInt KTimersGran = 4;			 						   
	CArrayPtrSeg<CRtTimer> iTimers;
	RMutex iTimersLock;	
	
	RThread	iServ; //timer server thread

	TBool 	iIsServStarted;
	RMutex 	iServConnectLock;
	RSemaphore iTimerSemaphore;
};

CTimerReqHandler* getTimerHandler();

#endif //__TIMERHANDLER__H

