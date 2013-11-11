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

#ifndef PROFILER_STATUS_CHECKER_H
#define PROFILER_STATUS_CHECKER_H

#include <e32std.h>
#include <e32base.h>
#include <e32property.h>    // RProperty

// CONSTANTS
const TUid KEngineStatusPropertyCat = { 0x2001E5AD };
enum TEnginePropertyKeys
    {
    EProfilerEngineStatus = 8
    };

// CLASS DECLARATIONS
class MProfilerStatusObserver
    {
    public: // Enums
        enum KProfilerStatus
            {
            EIdle = 0,
            EInitializing,
            ERunning,
            EStopping,
            ERestarting
            };

    public: // New
        virtual void HandleProfilerStatusChange(
                KProfilerStatus aStatus ) = 0;
        virtual void HandleProfilerErrorL(
                TInt aError ) = 0;
    };


class CProfilerEngineStatusChecker : public CActive
    {
public:
    inline static CProfilerEngineStatusChecker* CProfilerEngineStatusChecker::NewL();
    inline ~CProfilerEngineStatusChecker();
    inline void SetObserver(MProfilerStatusObserver* aObserver);
    inline TInt GetInitialState();
private:
    inline CProfilerEngineStatusChecker();
    inline void ConstructL();
    inline void RunL();
    inline void DoCancel();
private:
    TInt                        iPrevStat;
    MProfilerStatusObserver*    iObserver;
    RProperty                   iEngineStatus;
    };

#include <piprofiler/ProfilerEngineStatusChecker.inl>


#endif // PROFILER_STATUS_CHECKER_H
