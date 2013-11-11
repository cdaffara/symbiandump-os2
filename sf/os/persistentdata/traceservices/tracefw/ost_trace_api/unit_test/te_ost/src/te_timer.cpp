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
 @file te_timer.cpp
 @internalTechnology
 @prototype
*/


#include "te_timer.h"
#include "te_traceglobals.h"
#include <hal.h>


CUtfTimer::CUtfTimer()
	{
	iTimer.CreateLocal();
	}

CUtfTimer::~CUtfTimer()
	{
	iTimer.Close();
	}

TInt CUtfTimer::StartBackgroundTimer(TRequestStatus& aStatus)
	{
	TInt error = iTimer.CreateLocal();
	if(!error)
		{
		// Ensure timer is synchronized at the end of a tick
		// by waiting until it expires after 1 tick
		iTimer.HighRes(aStatus,1);
		User::WaitForRequest(aStatus);
		
		// Now we have a synchronized timer, so the next
		// delay starts exactly at the beginning of 1 timer
		// period
		iTimer.HighRes(aStatus, KTimerDuration);
		}
	return error;
	}

TUint32 CUtfTimer::CalculateTime(const TUint32 aCount)
	{
	return KTimerDuration * KMicroToNanoSecondConversion / aCount;
	}


