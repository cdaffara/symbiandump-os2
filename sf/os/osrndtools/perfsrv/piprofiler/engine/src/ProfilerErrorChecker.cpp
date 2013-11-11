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


#include <e32std.h>
#include <e32base.h>
#include <e32property.h>

#include <piprofiler/ProfilerTraces.h>

#include "ProfilerErrorChecker.h"

// properties
const TUid KEngineStatusPropertyCat={0x2001E5AD};
enum TEnginePropertyKeys
    {
    EProfilerEngineStatus = 8,
    EProfilerErrorStatus
    };

/*
 *
 *  CProfilerErrorChecker class implementation
 *
 */
CProfilerErrorChecker* CProfilerErrorChecker::NewL()
    {
    CProfilerErrorChecker* self = new(ELeave) CProfilerErrorChecker;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------------------------
CProfilerErrorChecker::CProfilerErrorChecker() :
    CActive(EPriorityStandard)
    {
    }

CProfilerErrorChecker::~CProfilerErrorChecker()
    {
    Cancel();
    iErrorStatus.Close();
    }

// --------------------------------------------------------------------------------------------
void CProfilerErrorChecker::ConstructL()
    {
    
    LOGTEXT(_L("Trying to attach to profiler engine error status property"));
    User::LeaveIfError(iErrorStatus.Attach(KEngineStatusPropertyCat, EProfilerErrorStatus));
    CActiveScheduler::Add(this);

    // subscribe to P&S status property
    iErrorStatus.Subscribe(iStatus);
    SetActive();
    }

// --------------------------------------------------------------------------------------------
void CProfilerErrorChecker::SetObserver(MProfilerErrorObserver* aObserver)
    {
    iObserver = aObserver;
    }

// --------------------------------------------------------------------------------------------
TInt CProfilerErrorChecker::RunError(TInt aError)
    {
    iErrorStatus.Close();
    return aError;
    }
// --------------------------------------------------------------------------------------------
void CProfilerErrorChecker::RunL()
    {
    // resubscribe before processing new value to prevent missing updates
    iErrorStatus.Subscribe(iStatus);
    SetActive();

    TInt stat(0);
    if(iErrorStatus.Get(stat) != KErrNone)
        {
        // check if error status != KErrNone
        if(stat != 0)
            {
            iObserver->HandleProfilerErrorChangeL(stat);
            }
        
        // reset error code
        iErrorStatus.Set(KErrNone);
        }
    }

// --------------------------------------------------------------------------------------------
 
void CProfilerErrorChecker::DoCancel()
    {
    iErrorStatus.Cancel();
    }

// end of file
    
