// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// UTrace Performance Tests User-Side Timer.
//



/**
 @file te_perfusertimer.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTTIMER_H
#define TE_UPTTIMER_H

#include <e32base.h>
#include <f32file.h>


/**
This is a user-side utility class to start the timer, get the elapsed time as a 64bit storage element 
and get the timer resolution 

@internalTechnology
@prototype
 */
class CUptTimer: public CBase
	{
public:
	CUptTimer();
	~CUptTimer();
	TInt	StartBackgroundTimer();
	TBool	IsTimerExpired();
	void	IncreaseCount(){iCount++;};
	TUint32 FinalTime();
	TInt	FinalCount();
	
private:	
	RTimer 				iTimer; //use to store timer session
	volatile TUint32	iCount;
	TRequestStatus 		iStatus;
	};

	
#endif


