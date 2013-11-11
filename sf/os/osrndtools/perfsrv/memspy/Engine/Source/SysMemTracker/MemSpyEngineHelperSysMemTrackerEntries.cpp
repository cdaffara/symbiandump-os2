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

#include "MemSpyEngineHelperSysMemTrackerEntries.h"

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineutils.h>
#include <memspy/engine/memspyenginehelperheap.h>
#include <memspy/engine/memspyenginehelperchunk.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelpercodesegment.h>
#include "MemSpyEngineHelperSysMemTrackerImp.h"
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>




CMemSpyEngineHelperSysMemTrackerEntry::CMemSpyEngineHelperSysMemTrackerEntry( CMemSpyEngineHelperSysMemTrackerImp& aTracker, TMemSpyEngineSysMemTrackerType aType )
:   iTracker( aTracker ), iType( aType )
    {
    // Entry starts out alive - this also updates it's timestamp
    SetAlive();

    // Entry starts out new by default
    SetNew( ETrue );
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntry::Key() const
    {
    const TUint32 val = ( Type() << 28 ); 
    TUint64 ret = val;
    ret <<= 32;
    ret += Handle();
    return ret;
    }


void CMemSpyEngineHelperSysMemTrackerEntry::HandleNewCycleL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    __ASSERT_ALWAYS( !( IsNew() && IsDead() ), User::Panic(_L("MemSpySysMemT"), __LINE__ ) );

    // First of all, if the item is not dead, then we can update the cycle stats
    if  ( IsAlive() )
        {
        UpdateCycleStatistics( aCycle );
        }

    // We create change descriptors only if the item has changed state in some way.
    // I.e.:
    //
    // 1) If the entry is new or is dead, then we will require a change descriptor.
    // 2) If the entry has changed, we'll require a change descriptor.
    //
    // Case (1)
    if  ( IsNew() || IsDead() )
        {
        CreateChangeDescriptorL( aCycle );
        }
    else
        {
        // Case (2) 
        const TBool hasChanged = HasChangedL( aCycle.Config() );
        if  ( hasChanged )
            {
            CreateChangeDescriptorL( aCycle );
            }
        }
    }


CMemSpyEngine& CMemSpyEngineHelperSysMemTrackerEntry::Engine()
    {
    return iTracker.Engine();
    }


void CMemSpyEngineHelperSysMemTrackerEntry::UpdateTime()
    {
    iLastUpdateTime.HomeTime();
    }


void CMemSpyEngineHelperSysMemTrackerEntry::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& /*aEntry*/ )
    {
    }


TBool CMemSpyEngineHelperSysMemTrackerEntry::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    // This item does not internally change state once created
    return EFalse;
    }


void CMemSpyEngineHelperSysMemTrackerEntry::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }


void CMemSpyEngineHelperSysMemTrackerEntry::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }







