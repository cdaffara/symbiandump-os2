// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <featmgr/featmgr.h>
#include "featmgrtlsdata.h"
#include "featmgrdebug.h"

// ============================= LOCAL FUNCTIONS ===============================

static CFeatMgrTlsData* TlsData( )
    {
    CFeatMgrTlsData* tlsData = STATIC_CAST( CFeatMgrTlsData*, Dll::Tls() );
    _LIT( KPanicCategory, "RFeatureControl" );
    __ASSERT_ALWAYS( tlsData, User::Panic( KPanicCategory, EPanicBadHandle ) );

    return tlsData;
    }


EXPORT_C TInt GetClientCount( )
    {
        CFeatMgrTlsData* tlsData = STATIC_CAST( CFeatMgrTlsData*, Dll::Tls() );
        if (tlsData)
            {
            return tlsData->ClientCount();
            }
        else
            {
            return 0;
            }
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFeatureManager::CFeatureManager()
// -----------------------------------------------------------------------------
//

CFeatMgrTlsData::CFeatMgrTlsData() :
    iClientCount(0)
    {
    }

// -----------------------------------------------------------------------------
// CFeatureManager::ConstructL
// -----------------------------------------------------------------------------
//
void CFeatMgrTlsData::ConstructL()
    {
    // Connect to Feature Manager server
    FUNC_LOG
    TInt err( iFeatMgrClient.Connect() );
    User::LeaveIfError(err);
    }

// -----------------------------------------------------------------------------
// CFeatureManager::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFeatMgrTlsData* CFeatMgrTlsData::NewL()
    {
    CFeatMgrTlsData* self = new( ELeave ) CFeatMgrTlsData();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CFeatureManager::~CFeatMgrTlsData
// -----------------------------------------------------------------------------
//
CFeatMgrTlsData::~CFeatMgrTlsData()
    {
    FUNC_LOG
    iFeatMgrClient.Close();
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::CanBeFreed()
// -----------------------------------------------------------------------------
//
TBool CFeatMgrTlsData::CanBeFreed() const
    {
	if (iClientCount <= 0)
	    {
		INFO_LOG1( "FeatMgr: TLS can be freed, clients(%d)", iClientCount );
		return ETrue;
	    }
	else
	    {
		INFO_LOG1( "FeatMgr: TLS can NOT be freed, clients(%d)", iClientCount );
		return EFalse;
	    }
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::IncreaseClientCount()
// -----------------------------------------------------------------------------
//
void CFeatMgrTlsData::IncreaseClientCount()
    {
	++iClientCount;
	INFO_LOG1( "FeatMgr: TLS increase, clients now(%d)", iClientCount );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::DecreaseClientCount()
// -----------------------------------------------------------------------------
//

void CFeatMgrTlsData::DecreaseClientCount()
    {
	--iClientCount;
	INFO_LOG1( "FeatMgr: TLS decrease, clients now(%d)", iClientCount );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::ClientCount()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::ClientCount()
    {
        return iClientCount;
    }
// -----------------------------------------------------------------------------
// CFeatMgrTlsData::FeatureSupported()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::FeatureSupported( TFeatureEntry& aFeature ) const
    {
    return iFeatMgrClient.FeatureSupported( aFeature );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::FeaturesSupported()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::FeaturesSupported( RFeatureArray& aFeatures )
    {
    return iFeatMgrClient.FeaturesSupported( aFeatures );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::EnableFeature()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::EnableFeature( TUid aFeature ) const
    {
    return iFeatMgrClient.EnableFeature( aFeature );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::DisableFeature()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::DisableFeature( TUid aFeature ) const
    {
    return iFeatMgrClient.DisableFeature( aFeature );
    }
    
// -----------------------------------------------------------------------------
// CFeatMgrTlsData::SetFeature()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::SetFeature( TUid aFeature, TBool aEnabled, TInt aData ) const
    {
    return iFeatMgrClient.SetFeature( aFeature, aEnabled, aData );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::SetFeature()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::SetFeature( TUid aFeature, TInt aData ) const
    {
    return iFeatMgrClient.SetFeature( aFeature, aData );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::AddFeature()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::AddFeature( TFeatureEntry aFeature ) const
    {
    return iFeatMgrClient.AddFeature( aFeature );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::DeleteFeature()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::DeleteFeature( TUid aFeature ) const
    {
    return iFeatMgrClient.DeleteFeature( aFeature );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::ListSupportedFeaturesL()
// -----------------------------------------------------------------------------
//
void CFeatMgrTlsData::ListSupportedFeaturesL( RFeatureUidArray& aSupportedFeatures )
    {
    iFeatMgrClient.ListSupportedFeaturesL( aSupportedFeatures );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::ReRequestNotification(TUid&, TRequestStatus&)
// -----------------------------------------------------------------------------
//
void CFeatMgrTlsData::ReRequestNotification( TUid& aFeatUid, TRequestStatus& aStatus )
    {
    iFeatMgrClient.ReRequestNotification( aFeatUid, aStatus );
    }
        

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::RequestNotification(RFeatureUidArray&, TUid&, TRequestStatus&)
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::RequestNotification( RFeatureUidArray& aFeatures, TUid& aFeatUid, 
    TRequestStatus& aStatus )
    {
    return iFeatMgrClient.RequestNotification( aFeatures, aFeatUid, aStatus );
    }
        
// -----------------------------------------------------------------------------
// CFeatMgrTlsData::RequestNotifyCancel(RFeatureUidArray&, TRequestStatus&)
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::RequestNotifyCancel( TUid aFeature ) const
    {
    return iFeatMgrClient.RequestNotifyCancel( aFeature );
    }
        
// -----------------------------------------------------------------------------
// CFeatMgrTlsData::RequestNotifyCancelAll(RFeatureUidArray&, TRequestStatus&)
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::RequestNotifyCancelAll( ) const
    {
    return iFeatMgrClient.RequestNotifyCancelAll( );
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::DeleteClient()
// -----------------------------------------------------------------------------
//
void CFeatMgrTlsData::DeleteClient()
	{
	CFeatMgrTlsData* tlsData = TlsData();

	// Decrease the client count (self)
	tlsData->DecreaseClientCount();

	// Check if no more clients so that TLS can be freed.
	if (tlsData->CanBeFreed())
		{
		delete tlsData;
		Dll::SetTls( NULL );
		}
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::SWIStart()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::SWIStart( ) const
    {
    return iFeatMgrClient.SWIStart();
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::SWIEnd()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::SWIEnd( ) const
    {
    return iFeatMgrClient.SWIEnd(); 
	}

/////////////////////////////////////////////////////////////////////////////////

// debug only API functions
#ifdef EXTENDED_FEATURE_MANAGER_TEST

#pragma BullseyeCoverage off

/** 
*/
void CFeatMgrTlsData::ResourceMark()
    {
    iFeatMgrClient.ResourceMark();
    }

/** 
*/
void CFeatMgrTlsData::ResourceCheck()
    {
    iFeatMgrClient.ResourceCheck();
    }

/** 
*/
TInt CFeatMgrTlsData::ResourceCount()
    {
    return iFeatMgrClient.ResourceCount();
    }

/** 
*/
void CFeatMgrTlsData::SetHeapFailure(TInt aAllocFailType, TInt aRate)
    {
    iFeatMgrClient.SetHeapFailure(aAllocFailType, aRate);
    }

// -----------------------------------------------------------------------------
// CFeatMgrTlsData::NumberOfNotifyFeatures()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::NumberOfNotifyFeatures( void ) const
    {
    return iFeatMgrClient.NumberOfNotifyFeatures();
    }
// -----------------------------------------------------------------------------
// CFeatMgrTlsData::CountAllocCells()
// -----------------------------------------------------------------------------
//
TInt CFeatMgrTlsData::CountAllocCells( void ) const
    {
    return iFeatMgrClient.CountAllocCells();
    }

#pragma BullseyeCoverage on

#endif

//  End of File  
