/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef VIDEOTESTTIMER_H
#define VIDEOTESTTIMER_H

#include <e32base.h>




class MTimerObserver
	{
public:
	virtual void TimerExpired() = 0;
	};

NONSHARABLE_CLASS(CVideoTestTimer) : public CTimer
	{
public:
	~CVideoTestTimer();
	static CVideoTestTimer* NewL(MTimerObserver& aTimerObserver);
	void Start(TTimeIntervalMicroSeconds32 aDelay);

private:
	CVideoTestTimer(MTimerObserver& aTimerObserver);
	void RunL();
	
private:
	MTimerObserver& iTimerObserver;
	};

#endif // VIDEOTESTTIMER_H
