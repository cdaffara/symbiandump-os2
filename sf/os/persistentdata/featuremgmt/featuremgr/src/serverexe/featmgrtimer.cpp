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
//



// INCLUDE FILES
#include <e32base.h>
#include "featmgrtimer.h"
#include "featmgrdebug.h"
#ifdef __WINS__
#include <e32svr.h>
#include <u32hal.h>
#endif

// LOCAL CONSTANTS
// KFeatMgrTimer is a patchable constant and can be modified during buildrom time.
// Note that it is not allowed to assign a value for constant in this file,
// because hence gets inlined by compiler and cannot be patched by buildrom.
IMPORT_C extern const TInt KFeatMgrTimer;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFeatMgrTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeatMgrTimer* CFeatMgrTimer::NewL( MFeatMgrTimerCallback& aCallback )
    {
    FUNC_LOG

    CFeatMgrTimer* p = new (ELeave) CFeatMgrTimer( aCallback );
    CleanupStack::PushL( p );
    p->ConstructL();
    CleanupStack::Pop( p );

    return p;
    }

// -----------------------------------------------------------------------------
// CFeatMgrTimer::CFeatMgrTimer
// Constructor with callback class as a parameter.
// -----------------------------------------------------------------------------
//
CFeatMgrTimer::CFeatMgrTimer( MFeatMgrTimerCallback& aCallback )
                              : CTimer(EPriorityHigh), 
                              iCallback(aCallback)                              
    {
    }

// -----------------------------------------------------------------------------
// CFeatMgrTimer::~CFeatMgrTimer
// Destructor.
// -----------------------------------------------------------------------------
//
CFeatMgrTimer::~CFeatMgrTimer()
    {
    }
    
// -----------------------------------------------------------------------------
// CFeatMgrTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFeatMgrTimer::ConstructL()
    {
    FUNC_LOG
    
    CTimer::ConstructL();

    TInt timerVal( KFeatMgrTimer );
#ifdef __WINS__
	// KFeatMgrTimer is a Rom patchable constant, so need an emulator equivalent
	// if WINS then read value from epoc.ini
	// requires licencees to set property in epoc.ini
	TInt load = 0;
	TInt err = UserSvr::HalFunction(EHalGroupEmulator,EEmulatorHalIntProperty,
	    (TAny*)"patchdata_featmgrserver_exe_KFeatMgrTimer",&load);
	    INFO_LOG1( "CFeatMgrTimer::ConstrutcL - timer err %d", err );
	if (err == KErrNone)
		{
		timerVal = load;
		}
#endif
    INFO_LOG1( "CFeatMgrTimer::ConstrutcL - timer value %d", timerVal );
    Set( timerVal );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTimer::Set
// Start the timer to complete after the specified number of microseconds.
// If the duration is zero, then timer is set to predefined maximum value.
// -----------------------------------------------------------------------------
//
void CFeatMgrTimer::Set(const TTimeIntervalMicroSeconds32& aInterval)
    {
    FUNC_LOG

    INFO_LOG1( "CFeatMgrTimer::Set(%d)", aInterval.Int() );

    __ASSERT_ALWAYS( CActiveScheduler::Current(), User::Invariant() );
    
    if (!IsAdded())
        {
        CActiveScheduler::Add(this);
        }
    
    // If the timer is already running, cancel it... 
    if (IsActive())
        {
        Cancel();
        }
        
    // And set the new timer. If timer has specified maximum time, enforce that.
    TTime now;
    now.HomeTime();
    
    // Otherwise just set the time to specified interval.
    After(aInterval);
    }

// -----------------------------------------------------------------------------
// CFeatMgrTimer::RunL
// RunL() function will be run after the specified system time expires,
// i.e. time is set by After() method,
// -----------------------------------------------------------------------------
void CFeatMgrTimer::RunL()
    {
    FUNC_LOG

    iCallback.TimerFired();
    }

//  End of File  
