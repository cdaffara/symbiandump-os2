/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* This contains CTEFTimer
*
*/



/**
 @prototype
 @test
*/

#include "teftimer.h"

EXPORT_C CTEFTimer* CTEFTimer::NewL(MTEFTimerCallback& aCallback, TInt aPriority)
/**
 * Two phase constructor
 *
 * @param	aCallback - owner to notify on completion of timer
 * @param	aPriority - priority of CActive
 *
 * @see		MTEFTimerCallback
 */
	{
	CTEFTimer*	ret=new (ELeave) CTEFTimer(aCallback, aPriority);
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

CTEFTimer::CTEFTimer(MTEFTimerCallback& aCallback, TInt aPriority)
/**
 * Constructor
 *
 * @param	aCallback - owner to notify on completion of timer
 * @param	aPriority - priority of CActive
 *
 * @see		MTEFTimerCallback
 */
:	CTimer(aPriority)
,	iCallback(aCallback)
	{

	}

void CTEFTimer::ConstructL()
/**
 * Second phase constructor
 *
 * @leave	system wide error
 */
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void CTEFTimer::RunL()
	{
	iCallback.TimerCompleted();
	}
