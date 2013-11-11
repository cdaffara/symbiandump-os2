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

#include "ProfilerTimer.h"

/*
 *
 *  CProfilerTimer class implementation
 *
 */
// --------------------------------------------------------------------------------------------
CProfilerTimer::CProfilerTimer(const TInt aPriority, MProfilerTimerObserver& aObserver)
: 
CActive(aPriority),
iObserver(aObserver)
    {    
    } 

// --------------------------------------------------------------------------------------------
CProfilerTimer::~CProfilerTimer()
    {
    Cancel();
    iTimer.Close();
    } 

// --------------------------------------------------------------------------------------------
CProfilerTimer* CProfilerTimer::NewL(const TInt aPriority, MProfilerTimerObserver& aObserver)
    {
    CProfilerTimer* self = new (ELeave) CProfilerTimer(aPriority, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------
void CProfilerTimer::ConstructL(void)
    {
    CActiveScheduler::Add(this);
    iTimer.CreateLocal();
    }

// --------------------------------------------------------------------------------------------
void CProfilerTimer::After(TUint aPeriodInSeconds)
    {
    Cancel();
    TTimeIntervalMicroSeconds32 period = aPeriodInSeconds * 1000000;
    iTimer.After(iStatus, period);
    SetActive();
    }

// --------------------------------------------------------------------------------------------
void CProfilerTimer::DoCancel()
    {
    iTimer.Cancel();
    }

// --------------------------------------------------------------------------------------------
void CProfilerTimer::RunL()
    {
    iObserver.HandleTimerExpiresL(iStatus.Int());
    }
