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
// timer header file
// 
//


#ifndef __TIMER__H
#define __TIMER__H

#include <e32base.h>
#include <time.h>
#include <signal.h>

#define MAXTIMERLIMIT 512
#define DELAYTIMER_MAX 32

class CTimerReqHandler;	
class CRtTimer;
class CTimerServer;
NONSHARABLE_CLASS(CLibRtTimer): public CTimer
	{
	public:
	CLibRtTimer():CTimer(EPriorityHigh)
		{
		}
	void ConstructL()
		{
		CTimer::ConstructL();
		}
	void At(TTime aSetTime)
		{
		CTimer::At(aSetTime);
		}
	inline void Set(CRtTimer* CRtPtr)
		{
		ptr = CRtPtr;
		}
	inline CRtTimer* GetRtPtr()
	{
		return ptr;
	}
	void AddToAS();
	private:
		void RunL();
		CRtTimer* ptr;
	};

class CRtTimer:public CBase
{
public: 
	friend class CTimerReqHandler;	
	friend class CTimerServer;
	friend class CLibRtTimer;
	static CRtTimer* New(struct sigevent *aSig);	
	TInt SetTime(TInt aFlag, const struct itimerspec *aIpTime,
				 struct itimerspec *aOpTime);			 
	TInt Time(struct itimerspec *aTime) const;
	TInt OverrunCount(TInt& aOverrunCount) const;	
	virtual ~CRtTimer();
protected: 

	CRtTimer(struct sigevent *aSig);
private:
	CLibRtTimer iTimer;
	
	TInt iTimerId;
	TBool iIsArmed;
	TBool iIsTimerReset;

	struct sigevent iSigEvent;
	struct itimerspec iStartTime;
	struct timespec iEndTime;
};
#endif //__TIMER__H

