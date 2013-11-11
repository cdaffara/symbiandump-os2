/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  The class implementing MWlanControlInterface.
*
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "gendebug.h"
#include "wlancontrolimpl.h"

/**
 * Pairs ECom implementation UIDs with a pointer to the instantiation 
 * method for that implementation. Required for all ECom implementation
 * collections.
 */
//lint --e{611}
const TImplementationProxy ImplementationTable[] =
    {
        {{0x10282e0f}, reinterpret_cast<TProxyNewLPtr>(CWlanControlImpl::NewL)}
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanControlImpl* CWlanControlImpl::NewL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::NewL()" ) ) );

    CWlanControlImpl* self = new (ELeave) CWlanControlImpl;
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanControlImpl::~CWlanControlImpl()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::~CWlanControlImpl()" ) ) );

    iServer.Close();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::GetPacketStatistics(
    TWlanPacketStatistics& aStatistics )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::GetPacketStatistics()" ) ) );
    
    return iServer.GetPacketStatistics( aStatistics );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanControlImpl::ClearPacketStatistics()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::ClearPacketStatistics()" ) ) );
    
    iServer.ClearPacketStatistics();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::GetUapsdSettings(
    TWlanUapsdSettings& aSettings )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::GetUapsdSettings()" ) ) );
    
    return iServer.GetUapsdSettings( aSettings );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::SetUapsdSettings(
    const TWlanUapsdSettings& aSettings )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::SetUapsdSettings()" ) ) );
    
    return iServer.SetUapsdSettings( aSettings );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::GetPowerSaveSettings(
    TWlanPowerSaveSettings& aSettings )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::GetPowerSaveSettings()" ) ) );
    
    return iServer.GetPowerSaveSettings( aSettings );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//     
TInt CWlanControlImpl::SetPowerSaveSettings(
    const TWlanPowerSaveSettings& aSettings )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::SetPowerSaveSettings()" ) ) );
    
    return iServer.SetPowerSaveSettings( aSettings );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::GetAccessPointInfo(
    TWlanAccessPointInfo& aInfo )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::GetAccessPointInfo()" ) ) );
    
    return iServer.GetAccessPointInfo( aInfo );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::GetRoamMetrics(
    TWlanRoamMetrics& aRoamMetrics )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::GetRoamMetrics()" ) ) );
    
    return iServer.GetRoamMetrics( aRoamMetrics );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::GetRogueList(
    CArrayFixSeg<TWlanBssid>& aRogueList )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::GetRogueList()" ) ) );

    return iServer.GetRogueList( aRogueList );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::GetRegulatoryDomain(
    TWlanRegion& aRegion )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::GetRegulatoryDomain()" ) ) );

    return iServer.GetRegulatoryDomain( aRegion );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::GetPowerSaveMode(
    TWlanPowerSave& aPowerSaveMode )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::GetPowerSaveMode()" ) ) );

    return iServer.GetPowerSaveMode( aPowerSaveMode );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanControlImpl::SetPowerSaveMode(
    TWlanPowerSaveMode aMode )
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::SetPowerSaveMode()" ) ) );

    return iServer.SetPowerSaveMode( aMode );    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanControlImpl::CWlanControlImpl()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::CWlanControlImpl()" ) ) );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanControlImpl::ConstructL()
    {
    TraceDump( INFO_LEVEL, ( _L( "CWlanControlImpl::ConstructL()" ) ) );

    User::LeaveIfError( iServer.Connect() );
    }

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------
// Returns an instance of the proxy table.
// Returns: KErrNone
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }
