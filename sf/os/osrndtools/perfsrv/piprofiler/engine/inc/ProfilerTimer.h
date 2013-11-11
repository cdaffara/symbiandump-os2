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

#ifndef PROFILERTIMER_H_
#define PROFILERTIMER_H_

#include <e32std.h>
#include <e32base.h>

class MProfilerTimerObserver
    {
public:
    virtual void HandleTimerExpiresL(TInt aError) = 0;
    };

class CProfilerTimer : public CActive
    {
public:
    static CProfilerTimer* NewL(const TInt aPriority, MProfilerTimerObserver& aObserver);
    ~CProfilerTimer();
    
public:
    
    void After(TUint aPeriodInSeconds);
    
protected:
    
    // From CActive
    void RunL();
    void DoCancel();
    
private:
    
    CProfilerTimer(const TInt aPriority, MProfilerTimerObserver& aObserver);
    void ConstructL(void);
    
private:
    
    RTimer      iTimer;
    MProfilerTimerObserver&    iObserver;
    };

#endif /* PROFILERTIMER_H_ */
