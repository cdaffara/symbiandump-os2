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

#include <memspy/engine/memspyenginehelpersysmemtracker.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include "MemSpyEngineHelperSysMemTrackerImp.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerobserver.h>
#include "MemSpyEngineHelperSysMemTrackerOutputFormatter.h"


CMemSpyEngineHelperSysMemTracker::CMemSpyEngineHelperSysMemTracker()
    {
    }

    
CMemSpyEngineHelperSysMemTracker::~CMemSpyEngineHelperSysMemTracker()
    {
    delete iOutputFormatter;
    delete iImp;
    }


void CMemSpyEngineHelperSysMemTracker::ConstructL( CMemSpyEngine& aEngine )
    {
    iOutputFormatter = CMemSpyEngineHelperSysMemTrackerOutputFormatter::NewL( aEngine );
    //
    iImp = CMemSpyEngineHelperSysMemTrackerImp::NewL( aEngine );
    iImp->SetObserver( this );
    }


CMemSpyEngineHelperSysMemTracker* CMemSpyEngineHelperSysMemTracker::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperSysMemTracker* self = new(ELeave) CMemSpyEngineHelperSysMemTracker();
    CleanupStack::PushL( self );
    self->ConstructL( aEngine );
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C TBool CMemSpyEngineHelperSysMemTracker::IsActive() const
    {
    return iImp->IsActive();
    }


EXPORT_C void CMemSpyEngineHelperSysMemTracker::StartL()
    {
    const TMemSpyEngineHelperSysMemTrackerConfig& config = iOutputFormatter->Config();
    iImp->StartL( config );
    }


EXPORT_C void CMemSpyEngineHelperSysMemTracker::StartL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
    {
    SetConfigL( aConfig );
    StartL();
    }


EXPORT_C void CMemSpyEngineHelperSysMemTracker::StopL()
    {
    iImp->StopL();
    }


EXPORT_C void CMemSpyEngineHelperSysMemTracker::SetConfigL( const TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
    {
    TMemSpyEngineHelperSysMemTrackerConfig config( aConfig );

    // Cap the value to ensure it doesn't fall beneath minimum value.
    const TInt timeInSeconds = config.TimerPeriod().Int() / 1000000;
    if  ( timeInSeconds < KMemSpySysMemTrackerConfigMinTimerPeriod )
        {
        config.iTimerPeriod = KMemSpySysMemTrackerConfigMinTimerPeriod * 1000000;
        }
    iOutputFormatter->SetConfig( config );
    }


EXPORT_C void CMemSpyEngineHelperSysMemTracker::GetConfig( TMemSpyEngineHelperSysMemTrackerConfig& aConfig )
    {
    aConfig = iOutputFormatter->Config();
    }


EXPORT_C void CMemSpyEngineHelperSysMemTracker::SetObserver( MMemSpyEngineHelperSysMemTrackerObserver* aObserver )
    {
    iRealObserver = aObserver;
    }


EXPORT_C void CMemSpyEngineHelperSysMemTracker::RemoveObserver( MMemSpyEngineHelperSysMemTrackerObserver* aObserver )
    {
    if ( iRealObserver == aObserver )
        {
        iRealObserver = NULL;
        }
    }


EXPORT_C const RPointerArray< CMemSpyEngineHelperSysMemTrackerCycle >& CMemSpyEngineHelperSysMemTracker::CompletedCycles() const
    {
    return iImp->CompletedCycles();
    }


void CMemSpyEngineHelperSysMemTracker::Reset()
    {
    iImp->Reset();
    }


EXPORT_C void CMemSpyEngineHelperSysMemTracker::CheckForChangesNowL()
    {
    iImp->CheckForChangesNowL();
    }


EXPORT_C TInt CMemSpyEngineHelperSysMemTracker::MdcaCount() const
    {
    return iImp->MdcaCount();
    }


EXPORT_C TPtrC CMemSpyEngineHelperSysMemTracker::MdcaPoint( TInt aIndex ) const
    {
    return iImp->MdcaPoint( aIndex );
    }


void CMemSpyEngineHelperSysMemTracker::HandleCyclesResetL()
    {
    if  ( iRealObserver )
        {
        TRAP_IGNORE( iRealObserver->HandleCyclesResetL() );
        }
    }


void CMemSpyEngineHelperSysMemTracker::HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    if  ( iRealObserver )
        {
        TRAP_IGNORE( iRealObserver->HandleCycleStartedL( aCycle ) );
        }
    }

void CMemSpyEngineHelperSysMemTracker::HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    iOutputFormatter->OutputL( aCycle );
    //
    if  ( iRealObserver )
        {
        TRAP_IGNORE( iRealObserver->HandleCycleFinishedL( aCycle ) );
        }
    }




