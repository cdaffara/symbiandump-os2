/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#include "videotesttimer.h"


CVideoTestTimer::CVideoTestTimer(MTimerObserver& aTimerObserver) 
: CTimer(EPriorityHigh), iTimerObserver(aTimerObserver)
	{
	CActiveScheduler::Add(this);
	}

CVideoTestTimer* CVideoTestTimer::NewL(MTimerObserver& aTimerObserver)
	{
	CVideoTestTimer* self = new (ELeave) CVideoTestTimer(aTimerObserver);
	CleanupStack::PushL(self);
	self->ConstructL(); // this call CTimer::ConstructL
	CleanupStack::Pop(self);
	return self;
	}

CVideoTestTimer::~CVideoTestTimer()
	{
	// cancel is called by CTimer destructor
	}

/** Start the timer */
void CVideoTestTimer::Start(TTimeIntervalMicroSeconds32 aDelay)
	{
	HighRes(aDelay);
	}

/** Handle completion */
void CVideoTestTimer::RunL()
	{
	if (iStatus == KErrNone)
		{
		iTimerObserver.TimerExpired();
		}
	}
