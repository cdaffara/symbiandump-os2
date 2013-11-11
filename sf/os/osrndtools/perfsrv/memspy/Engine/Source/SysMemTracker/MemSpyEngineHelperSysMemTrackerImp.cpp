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

#include "MemSpyEngineHelperSysMemTrackerImp.h"

// System includes
#include <e32debug.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerobserver.h>
#include "MemSpyEngineHelperSysMemTrackerEntryManager.h"

// Constants
const TInt KMemSpyDefaultTrackerTimerPeriod = 30; // Seconds


CMemSpyEngineHelperSysMemTrackerImp::CMemSpyEngineHelperSysMemTrackerImp( CMemSpyEngine& aEngine )
:   CTimer( EPriorityStandard ), iEngine( aEngine )
    {
    CActiveScheduler::Add( this );
    iConfig.iTimerPeriod = TTimeIntervalMicroSeconds32( KMemSpyDefaultTrackerTimerPeriod * 1000000 );
    }

    
CMemSpyEngineHelperSysMemTrackerImp::~CMemSpyEngineHelperSysMemTrackerImp()
    {
    Cancel();
    //
    delete iEntryManager;
    //
    iCompletedCycles.ResetAndDestroy();
    iCompletedCycles.Close();
    }


void CMemSpyEngineHelperSysMemTrackerImp::ConstructL()
    {
    CTimer::ConstructL();
    }


CMemSpyEngineHelperSysMemTrackerImp* CMemSpyEngineHelperSysMemTrackerImp::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperSysMemTrackerImp* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerImp( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


void CMemSpyEngineHelperSysMemTrackerImp::StartL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
    {
    TRACE( RDebug::Printf("CMemSpyEngineHelperSysMemTrackerImp::StartL() - START - timer period: %d", Config().TimerPeriod().Int() ) );

    // Zap any old data
    Reset();

    // Cache client settings
    iConfig = aConfig;

    // Make new cycle
    PrepareInitialCycleL();

    // Start tracking via timer - future updates will by driven by RunL().
    After( Config().TimerPeriod() );
    TRACE( RDebug::Printf("CMemSpyEngineHelperSysMemTrackerImp::END() - START - timer period: %d", Config().TimerPeriod().Int() ) );
    }


void CMemSpyEngineHelperSysMemTrackerImp::StopL()
    {
    Cancel();
    }


void CMemSpyEngineHelperSysMemTrackerImp::SetObserver( MMemSpyEngineHelperSysMemTrackerObserver* aObserver )
    {
    iObserver = aObserver;
    }


void CMemSpyEngineHelperSysMemTrackerImp::RemoveObserver( MMemSpyEngineHelperSysMemTrackerObserver* aObserver )
    {
    if  ( iObserver == aObserver )
        {
        iObserver = NULL;
        }
    }


const RPointerArray< CMemSpyEngineHelperSysMemTrackerCycle >& CMemSpyEngineHelperSysMemTrackerImp::CompletedCycles() const
    {
    return iCompletedCycles;
    }


void CMemSpyEngineHelperSysMemTrackerImp::Reset()
    {
    Cancel();
    iCompletedCycles.ResetAndDestroy();
    //
    if  ( iObserver )
        {
        TRAP_IGNORE( iObserver->HandleCyclesResetL() );
        }
    }


void CMemSpyEngineHelperSysMemTrackerImp::CheckForChangesNowL()
    {
    const TInt count = iCompletedCycles.Count();
    TRACE( RDebug::Printf("CMemSpyEngineHelperSysMemTrackerImp::CheckForChangesNowL() - START - isActive: %d, cycleCount: %d", IsActive(), count ) );

    if  ( IsActive() )
        {
        Cancel();
        CheckForChangesL();
        After( Config().TimerPeriod() );
        }
    else
        {
        // Timer not running, so we're probably being poked by some external-to-memspy client that
        // wants to force an update.
        //
        // If we've never yet performed an initital cycle, then prep that. Otherwise, just proceed as normal
        // as if we had been called back via RunL().
        if  ( count == 0 )
            {
            PrepareInitialCycleL();
            }
        else
            {
            CheckForChangesL();
            }
        }

    TRACE( RDebug::Printf("CMemSpyEngineHelperSysMemTrackerImp::CheckForChangesNowL() - END - isActive: %d, cycleCount: %d", IsActive(), iCompletedCycles.Count() ) );
    }


TInt CMemSpyEngineHelperSysMemTrackerImp::MdcaCount() const
    {
    return iCompletedCycles.Count();
    }


TPtrC CMemSpyEngineHelperSysMemTrackerImp::MdcaPoint( TInt aIndex ) const
    {
    const CMemSpyEngineHelperSysMemTrackerCycle* cycleInfo = iCompletedCycles[ aIndex ];
    const TPtrC ret( cycleInfo->Caption() );
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerImp::NotifyObserverCycleStartedL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    if  ( iObserver )
        {
        TRAP_IGNORE( iObserver->HandleCycleStartedL( aCycle ) );
        }
    }


void CMemSpyEngineHelperSysMemTrackerImp::NotifyObserverCycleFinishedL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    if  ( iObserver )
        {
        TRAP_IGNORE( iObserver->HandleCycleFinishedL( aCycle ) );
        aCycle.DiscardChanges();
        }
    }


void CMemSpyEngineHelperSysMemTrackerImp::AddCycleAndPopL( CMemSpyEngineHelperSysMemTrackerCycle* aCycle )
    {
    iCompletedCycles.AppendL( aCycle );
    CleanupStack::Pop( aCycle );
    aCycle->FinalizeL();
    }


CMemSpyEngineHelperSysMemTrackerCycle* CMemSpyEngineHelperSysMemTrackerImp::LastCycleOrNull()
    {
    CMemSpyEngineHelperSysMemTrackerCycle* ret = NULL;
    //
    const TInt count = iCompletedCycles.Count();
    if  ( count )
        {
        ret = iCompletedCycles[ count - 1 ];
        }
    //
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerImp::PrepareInitialCycleL()
    {
    // Create seed items
    CMemSpyEngineHelperSysMemTrackerEntryManager* entryManager = CMemSpyEngineHelperSysMemTrackerEntryManager::NewL( *this );
    delete iEntryManager;
    iEntryManager = entryManager;

    // Everything changes on the first cycle.
    CMemSpyEngineHelperSysMemTrackerCycle* baselineCycle = CMemSpyEngineHelperSysMemTrackerCycle::NewLC( iConfig );
    NotifyObserverCycleStartedL( *baselineCycle );
    iEntryManager->EverythingHasChangedL( *baselineCycle );
    AddCycleAndPopL( baselineCycle );
    NotifyObserverCycleFinishedL( *baselineCycle );
    }


void CMemSpyEngineHelperSysMemTrackerImp::CheckForChangesL()
    {
    // Create info structure 
    CMemSpyEngineHelperSysMemTrackerCycle* cycle = CMemSpyEngineHelperSysMemTrackerCycle::NewLC( iConfig, LastCycleOrNull() );

    // Notify 
    NotifyObserverCycleStartedL( *cycle );

    // Find changes
    iEntryManager->IdentifyChangesL( *cycle );

    // Finalise the cycle and save it
    AddCycleAndPopL( cycle );

    // Notify
    NotifyObserverCycleFinishedL( *cycle );
    }


void CMemSpyEngineHelperSysMemTrackerImp::RunL()
    {
    CheckForChangesL();
    After( Config().TimerPeriod() );
    }


TInt CMemSpyEngineHelperSysMemTrackerImp::RunError( TInt aError )
    {
    RDebug::Printf( "CMemSpyEngineHelperSysMemTrackerImp::RunError() - aError: %d", aError );
    (void) aError;
    //
    Cancel();
    After( Config().TimerPeriod() );
    //
    return KErrNone;
    }

















