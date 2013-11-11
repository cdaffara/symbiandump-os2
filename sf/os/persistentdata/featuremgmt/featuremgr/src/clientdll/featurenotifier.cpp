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
#include <featmgr/featurenotifier.h>
#include "featmgrdebug.h"
#include <featmgr/featurecmn.h>
#include "featmgrclient.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// FindByUid
// Returns Zero if UIDs do match.
// -----------------------------------------------------------------------------
//  
static TInt FindByUid( const TUid* aFeature, const TUid& aItem )
    {
    if ( aFeature->iUid == aItem.iUid )
        {
        return 1;
        }

    return 0;
    }

    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFeatureNotifier::CFeatureNotifier()
// -----------------------------------------------------------------------------
//
CFeatureNotifier::CFeatureNotifier( MFeatureObserver& aObserver ) :
    CActive( EPriorityStandard ),
    iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CFeatureNotifier::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CFeatureNotifier* CFeatureNotifier::NewL( MFeatureObserver& aObserver )
    {
    CFeatureNotifier* self = new( ELeave ) CFeatureNotifier( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CFeatureNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CFeatureNotifier::ConstructL()
    {
    FUNC_LOG

    iFeatMgrClient = new (ELeave) RFeatMgrClient;
    // Connect to Feature Manager server
    TInt err( iFeatMgrClient->Connect() );
    if (err!=KErrNone)
      {
      delete iFeatMgrClient;
      iFeatMgrClient=NULL;
      }
    User::LeaveIfError(err);

    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CFeatureNotifier::~CFeatureNotifier()
// -----------------------------------------------------------------------------
//
EXPORT_C CFeatureNotifier::~CFeatureNotifier()
    {
    FUNC_LOG
    
    if( iFeatMgrClient )
    	{
    	iFeatMgrClient->RequestNotifyCancelAll();
        iFeatMgrClient->Close();
        delete iFeatMgrClient;
    	}
    Cancel();

    iFeatures.Close();
    }

// -----------------------------------------------------------------------------
// CFeatureNotifier::NotifyRequest(TUid)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFeatureNotifier::NotifyRequest( TUid aFeature )
    {
    if(IsActive())
        {
        return KErrAlreadyExists;
        }

    iFeatures.Reset();
    TInt err=iFeatures.Append( aFeature );
    if (err!=KErrNone)
      {
      return err;
      }
    err = iFeatMgrClient->RequestNotification( iFeatures, iFeatureChanged, iStatus );
    if ( err == KErrNone )
    	{
        SetActive();
     	}
    else
      {
      iFeatures.Reset();
      }
     	    
    return err;
    }
    
// -----------------------------------------------------------------------------
// CFeatureNotifier::NotifyRequest(RFeatureUidArray&)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFeatureNotifier::NotifyRequest( RFeatureUidArray& aFeatures )
    {
    if(IsActive())
        {
        return KErrAlreadyExists;
        }

    iFeatures.Reset();
    TInt count = aFeatures.Count();
    TInt err=KErrNone;
    for(TInt i = 0; i < count; i++ )
        {
        // Do not append duplicate entries
        const TUid& uid( aFeatures[i] );
        TInt index = iFeatures.Find( uid, FindByUid );
        if( index == KErrNotFound )
            {
            err=iFeatures.Append( uid );
            if (err!=KErrNone)
              {
              iFeatures.Reset();
              return err;
              }
            }
        }

    err = iFeatMgrClient->RequestNotification( iFeatures, iFeatureChanged, iStatus );
    if ( err == KErrNone )
    	{
        SetActive();
     	}
    else
      {
      iFeatures.Reset();
      }
    return err;
    }
    
// -----------------------------------------------------------------------------
// CFeatureNotifier::NotifyCancel(TUid aFeature)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFeatureNotifier::NotifyCancel( TUid aFeature )
    {
    TInt err( KErrNotFound );
    TInt count = iFeatures.Count();
    
    for(TInt i = 0; i < count; i++ )
        {
        if( iFeatures[i].iUid == aFeature.iUid )
            {
            err = iFeatMgrClient->RequestNotifyCancel( aFeature );
            
            if( err == KErrNone )
                {
                iFeatures.Remove( i );
                // If this is the last feature in the list, then also cancel the Active Object
                if( 0 == iFeatures.Count() )
                	{
	                Cancel();
                	}
                break;
                }
            }
        }

    return err;
    }
    
// -----------------------------------------------------------------------------
// CFeatureNotifier::NotifyCancelAll()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CFeatureNotifier::NotifyCancelAll()
    {
    iFeatures.Reset();
    TInt err = iFeatMgrClient->RequestNotifyCancelAll( );
   	Cancel();
    
    return err;
    }

/**
 Gets the number of features in the server's array.
 This API is only for internal use and testing purposes.

 @return The number of features on the server for debug builds, otherwise it returns KErrNotSupported in release builds.
 @internalComponent
*/
EXPORT_C TInt CFeatureNotifier::NumberOfNotifyFeatures()
    {
#ifdef EXTENDED_FEATURE_MANAGER_TEST
    TInt count = iFeatMgrClient->NumberOfNotifyFeatures();
    return count;
#else
    return KErrNotSupported;
#endif
    }

/**
 Gets the number of heap cells in the thread.
 This API is only for internal use and testing purposes.

 @return The number of heap cells for debug build, otherwise it returns KErrNotSupported in release builds.
 @internalComponent
*/
EXPORT_C TInt CFeatureNotifier::CountAllocCells()
    {
#ifdef EXTENDED_FEATURE_MANAGER_TEST
    TInt count = iFeatMgrClient->CountAllocCells();
    return count;
#else
    return KErrNotSupported;
#endif
    }

// -----------------------------------------------------------------------------
// CFeatureNotifier::RunL()
// -----------------------------------------------------------------------------
//
void CFeatureNotifier::RunL( )
    {
    FUNC_LOG

    TInt status = iStatus.Int();
    if( status < 0 )
        {
        // Got an error
        if( status != KErrCancel )
            {
            iObserver.HandleNotifyError( status );
            }
        return;
        }

    // iStatus >= 0 means it contains the change type
    TFeatureChangeType changeType = static_cast<TFeatureChangeType>(status);

	// If the feature was deleted, remove it from the array of features with notifications requested
    if( changeType == EFeatureFeatureDeleted )
    	{
        TInt index = iFeatures.Find( iFeatureChanged, FindByUid );
        if( index != KErrNotFound )
            {
            iFeatures.Remove( index );
            }
    	}
    // Should we validate whether iFeatureChanged matches 
    // to any UID in the array of requested features?

    TFeatureEntry feature( iFeatureChanged );
    if( iFeatures.Count() > 0 )
        {
        // Asynchronously resubscribe notify request
        iFeatMgrClient->ReRequestNotification( iFeatureChanged, iStatus );
        // Next inform client about changed feature 
        // Should we validate whether iFeatureChanged matches 
        // to any UID in the array of requested features
        iObserver.HandleNotifyChange( changeType, feature );
        // Set us active
        SetActive();
        }
    else
        {
        // All features with notifications requested were deleted,
        // so don't need to resubscribe notify request -
        // just notify client about the deleted feature
        iObserver.HandleNotifyChange( changeType, feature );
        }
    }

// -----------------------------------------------------------------------------
// CFeatureNotifier::DoCancel()
// -----------------------------------------------------------------------------
//
void CFeatureNotifier::DoCancel( )
    {
    }

// -----------------------------------------------------------------------------
// CFeatureNotifier::RunError()
// -----------------------------------------------------------------------------
//
TInt CFeatureNotifier::RunError( TInt aError )
    {
    iObserver.HandleNotifyError( aError );

    return KErrNone;
    }

//  End of File  
