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
 @file te_upttimer.cpp
 @internalTechnology
 @prototype
*/


#include "te_usertimer.h"
#include "te_traceglobals.h" //for KMicroToNanoSecondConversion
#include <hal.h>


CUserTimer::CUserTimer()
	{
	/*Constructor*/
	
	}

CUserTimer::~CUserTimer()
	{
	/*Destructor*/
	iTimer.Close();
	}

/** 
 * This user-side method uses the fast tickcount timer in the hal.h class to get the initial start-time
 * as a tickcount and store it as a private data member for later manipulation.
 * @return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TInt CUserTimer::StartBackgroundTimer()
	{
	TInt error = iTimer.CreateLocal();
	iCount = 0;
	if(!error)
		{
		// Ensure timer is synchronized at the end of a tick
		// by waiting until it expires after 1 tick
		iTimer.HighRes(iStatus,1);
		User::WaitForRequest(iStatus);
		
		// Now we have a synchronized timer, so the next
		// delay starts exactly at the beginning of 1 timer
		// period
		iTimer.HighRes(iStatus, KTimerDuration);
		}
	return error;
	}


/** This user-side method uses the fast tickcount timer in the hal.h class to get the initial start-time
as a tickcount and store it as a private data member for later manipulation.
@return KErrNone if command was prepared correctly and a system wide error code otherwise.
 */
TUint32 CUserTimer::FinalTime()
	{
	//need to do this at some stage but dont want to add another method to do it, but also dont want to force people to call FinalCount()
	User::WaitForRequest(iStatus);	
	return (KTimerDuration * (TUint32) KMicroToNanoSecondConversion) / iCount;
	}

TBool	CUserTimer::IsTimerExpired()
	{
	return (iStatus == KErrNone);
	}

TInt CUserTimer::FinalCount()
	{
	return iCount;
	}
