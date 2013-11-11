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

// ------------------------------------------------------------------------------
//
// class CProfilerEngineStatusChecker
//
// ------------------------------------------------------------------------------
//
inline CProfilerEngineStatusChecker* CProfilerEngineStatusChecker::NewL()
    {
    CProfilerEngineStatusChecker* self = new(ELeave) CProfilerEngineStatusChecker();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------
inline CProfilerEngineStatusChecker::CProfilerEngineStatusChecker() :
    CActive(EPriorityStandard)
    {
    }

inline CProfilerEngineStatusChecker::~CProfilerEngineStatusChecker()
    {
    Cancel();
    iEngineStatus.Close();
    }

// --------------------------------------------------------------------------------------------
inline void CProfilerEngineStatusChecker::ConstructL()
    {
    User::LeaveIfError(iEngineStatus.Attach(KEngineStatusPropertyCat, EProfilerEngineStatus));
    CActiveScheduler::Add(this);
    
    // check engine status, if not available set to idle
    if(iEngineStatus.Get(iPrevStat) != KErrNone)
        {
        iPrevStat = MProfilerStatusObserver::EIdle;
        }

    // subscribe to P&S status property
    iEngineStatus.Subscribe(iStatus);
    SetActive();
    }

inline TInt CProfilerEngineStatusChecker::GetInitialState()
    {
    // check engine status, if not available set to idle
    TInt err(iEngineStatus.Get(KEngineStatusPropertyCat, EProfilerEngineStatus, iPrevStat));
    if(err != KErrNone)
        {
        iPrevStat = MProfilerStatusObserver::EIdle;
        }
    return iPrevStat;
    }

// --------------------------------------------------------------------------------------------
inline void CProfilerEngineStatusChecker::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iEngineStatus.Subscribe(iStatus);
    SetActive();

    TInt stat(0);
    if(iEngineStatus.Get(KEngineStatusPropertyCat, EProfilerEngineStatus, stat) == KErrNone)
        {
        // check if status one of error codes (< 0)
        if(stat < KErrNone)
            {
            // some error occurred on engine side => set UI idle and show an error note
            iObserver->HandleProfilerErrorL(stat);
            }
        else
            {
            if(iPrevStat != stat)
                {
                switch(stat)
                    {
                    case 0:
                        iObserver->HandleProfilerStatusChange(MProfilerStatusObserver::EIdle);
                        break;
                    case 1:
                        iObserver->HandleProfilerStatusChange(MProfilerStatusObserver::EInitializing);
                        break;
                    case 2:
                        iObserver->HandleProfilerStatusChange(MProfilerStatusObserver::ERunning);
                        break;
                    case 3:
                        iObserver->HandleProfilerStatusChange(MProfilerStatusObserver::EStopping);
                        break;
                    default:
                        iObserver->HandleProfilerErrorL(stat);
                        break;
                    }
                iPrevStat = stat;
                }
            }
        }
    }

// --------------------------------------------------------------------------------------------
 
inline void CProfilerEngineStatusChecker::DoCancel()
    {
    iEngineStatus.Cancel();
    }

// --------------------------------------------------------------------------------------------
inline void CProfilerEngineStatusChecker::SetObserver(MProfilerStatusObserver* aObserver)
    {
    iObserver = aObserver;
    }
