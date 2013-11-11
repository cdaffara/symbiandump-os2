/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer class for platformwide events
*
*/


#include <centralrepository.h>
#include "wlmplatformsubscriber.h"
#include "am_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWlmPlatformSubscriber::CWlmPlatformSubscriber
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWlmPlatformSubscriber::CWlmPlatformSubscriber(
    TWlmSubscribeType aType,
    MWlmPlatformResponder& aCallback,
    const TUid& aCategory,
    const TUint aKey ) :
    CActive( CActive::EPriorityStandard ),
    iType( aType ),
    iCallback( aCallback ),
    iCategory( aCategory ),
    iKey( aKey ),
    iCenRep( NULL )
    {
    DEBUG( "CWlmPlatformSubscriber::CWlmPlatformSubscriber()" );
    }

// -----------------------------------------------------------------------------
// CWlmPlatformSubscriber::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWlmPlatformSubscriber::ConstructL()
    {
    DEBUG( "CWlmPlatformSubscriber::ConstructL()" );
    
    CActiveScheduler::Add( this );
    
    // In case PubSub, attach to a certain key
    if ( iType == EWlmSubscribeTypePubSub )
        {
        iProperty.Attach( iCategory, iKey );    
        }
    // In case CenRep, open connection to a certain repository
    else if ( iType == EWlmSubscribeTypeCenRep )
        {
        iCenRep = CRepository::NewL( iCategory );
        }
    }

// -----------------------------------------------------------------------------
// CWlmPlatformSubscriber::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWlmPlatformSubscriber* CWlmPlatformSubscriber::NewL(
    TWlmSubscribeType aType,
    MWlmPlatformResponder& aCallback,
    const TUid& aCategory,
    const TUint aKey )
    {
    DEBUG( "CWlmPlatformSubscriber::NewL()" );

    CWlmPlatformSubscriber* self = new( ELeave ) CWlmPlatformSubscriber(
        aType,
        aCallback,
        aCategory,
        aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CWlmPlatformSubscriber::~CWlmPlatformSubscriber()
    {
    DEBUG( "CWlmPlatformSubscriber::~CWlmPlatformSubscriber()" );

    Cancel();
    if ( iType == EWlmSubscribeTypePubSub )
        {        
        iProperty.Close();   
        }
    else if ( iType == EWlmSubscribeTypeCenRep )
        {
        delete iCenRep;
        }    
    }

// -----------------------------------------------------------------------------
// CWlmPlatformSubscriber::IssueRequest
// -----------------------------------------------------------------------------
//
void CWlmPlatformSubscriber::IssueRequest()
    {
    DEBUG( "CWlmPlatformSubscriber::IssueRequest()" );

    if( !IsActive() )
        {
        if ( iType == EWlmSubscribeTypePubSub )
            {
            DEBUG( "CWlmPlatformSubscriber::IssueRequest() - requestType == PubSub" );
            iProperty.Subscribe( iStatus );
            }
        else if ( iType == EWlmSubscribeTypeCenRep )     
            {
            DEBUG( "CWlmPlatformSubscriber::IssueRequest() - requestType == CenRep" );
            iCenRep->NotifyRequest( iKey, iStatus );
            }
        SetActive();
        }
    else
        {
        DEBUG("CWlmPlatformSubscriber::IssueRequest() - already active, ignoring");
        }
    }
    
// -----------------------------------------------------------------------------
// CWlmPlatformSubscriber::DoCancel
// -----------------------------------------------------------------------------
//
void CWlmPlatformSubscriber::DoCancel()
    {
    DEBUG( "CWlmPlatformSubscriber::DoCancel()" );

    if ( iType == EWlmSubscribeTypePubSub )
        {
        iProperty.Cancel();
        }
    else if ( iType == EWlmSubscribeTypeCenRep )    
        {
        iCenRep->NotifyCancel( iKey );
        }    
    }

// -----------------------------------------------------------------------------
// CWlmPlatformSubscriber::RunError
// -----------------------------------------------------------------------------
//
TInt CWlmPlatformSubscriber::RunError(
    TInt aError )
    {
    DEBUG( "CWlmPlatformSubscriber::RunError()" );
    
    if ( aError != KErrNone )
        {
        DEBUG1( "CWlmPlatformSubscriber::RunError() - aError = %d", aError );    
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWlmPlatformSubscriber::RunL
// -----------------------------------------------------------------------------
//
void CWlmPlatformSubscriber::RunL()
    {
    DEBUG( "CWlmPlatformSubscriber::RunL()" );
    
    DEBUG1( "CWlmPlatformSubscriber::RunL() - iStatus = %d", iStatus.Int() );
    iCallback.HandlePropertyChangedL( iCategory, iKey );
    IssueRequest();
    }
    
// -----------------------------------------------------------------------------
// CWlmPlatformSubscriber::Get
// -----------------------------------------------------------------------------
//    
TInt CWlmPlatformSubscriber::Get( TInt& aValue )
    {
    DEBUG( "CWlmPlatformSubscriber::Get()" );
              
    if ( iType == EWlmSubscribeTypePubSub )
        {
        return iProperty.Get( aValue );
        }
    else if ( iType == EWlmSubscribeTypeCenRep )
        {
        return iCenRep->Get( iKey, aValue );
        }
        
    return KErrNotFound;
    }
