/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active Object for asynchronous scan request.
*
*/


#include "wlmscanrequest.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlmScanRequest* CWlmScanRequest::NewL(
    RWLMServer& aClient )
    {
    DEBUG( "CWlmScanRequest::NewL()" );

    CWlmScanRequest* self = new (ELeave) CWlmScanRequest(
        aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlmScanRequest::~CWlmScanRequest()
    {
    DEBUG( "CWlmScanRequest::~CWlmScanRequest()" );
    
    iScanList = NULL;
    iClientStatus = NULL;
    iCacheLifetime = NULL;
    iMaxDelay = NULL;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlmScanRequest::IssueRequest(
    ScanList& aScanList,
    const TDesC8& aSsid,
    TInt& aCacheLifetime,
    TUint& aMaxDelay,
    TRequestStatus* aStatus )
    {
    DEBUG( "CWlmScanRequest::IssueRequest()" );
    
    iScanList = &aScanList;
    iScanList->ClearAll();
    if ( !iScanList->SetCurrentMaxSize( KWlmScanListMaxSize ) )
        {
        DEBUG( "CWlmScanRequest::IssueRequest() - unable to set scan list size" );
        
        if ( !aStatus )
            {
            return KErrNoMemory;
            }

        User::RequestComplete( aStatus, KErrNoMemory );
        
        return KErrNone;
        }

    iScanListPtr.Set( iScanList->Data(), 0, KWlmScanListMaxSize );
    
    iDynamicScanListBuf().count = 0;
    iDynamicScanListBuf().size = 0;
    
    iSsidBuf().ssidLength = aSsid.Length();
    Mem::Copy(
        &iSsidBuf().ssid[0],
        aSsid.Ptr(),
        aSsid.Length() );

    iCacheLifetime = &aCacheLifetime;
    iMaxDelay = &aMaxDelay;
    
    iScanSchedulingBuf().cacheLifetime = aCacheLifetime;
    iScanSchedulingBuf().maxDelay = aMaxDelay;
    
    TIpcArgs params( &iScanListPtr, &iSsidBuf, &iDynamicScanListBuf, &iScanSchedulingBuf );

    if ( !aStatus )
        {
        TInt ret = iClient.SendReceive( EGetScanResults, params );
        if ( ret != KErrNone )
            {
            DEBUG1( "CWlmScanRequest::IssueRequest() - EGetScanResults failed (%d)",
                ret );

            return ret;
            }
        aCacheLifetime = iScanSchedulingBuf().cacheLifetime;
        aMaxDelay = iScanSchedulingBuf().maxDelay;

        return UpdateResults();        
        }
    else
        {
        iClientStatus = aStatus;
        *iClientStatus = KRequestPending;
        iClient.SendReceive( EGetScanResults, params, iStatus );
        SetActive();
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TRequestStatus* CWlmScanRequest::ClientStatus()
    {
    return iClientStatus;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlmScanRequest::RunL()
    {
    DEBUG( "CWlmScanRequest::RunL()" );

    *iCacheLifetime = iScanSchedulingBuf().cacheLifetime;
    *iMaxDelay = iScanSchedulingBuf().maxDelay;
    
    TInt ret = iStatus.Int();
    if ( ret == KErrNone )
        {
        ret = UpdateResults();
        }
    else
        {
        DEBUG1( "CWlmScanRequest::RunL() - EGetScanResults failed (%d)",
            ret );
        }

    User::RequestComplete( iClientStatus, ret );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlmScanRequest::RunError(
    TInt /* aError */ )
    {
    // Ignore the error.
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlmScanRequest::DoCancel()
    {
    DEBUG( "CWlmScanRequest::DoCancel()" );

    *iCacheLifetime = iScanSchedulingBuf().cacheLifetime;
    *iMaxDelay = iScanSchedulingBuf().maxDelay;
    
    // Send the command
    iClient.SendReceive( ECancelGetScanResults );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlmScanRequest::CWlmScanRequest(
    RWLMServer& aClient ) :
    CActive( CActive::EPriorityStandard ),
    iClient( aClient ),
    iScanList( NULL ),
    iScanListPtr( NULL, 0 ),
    iCacheLifetime( NULL ),
    iMaxDelay( NULL ),
    iClientStatus( NULL )
    {
    DEBUG( "CWlmScanRequest::CWlmScanRequest()" );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlmScanRequest::ConstructL()
    {
    DEBUG( "CWlmScanRequest::ConstructL()" );

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlmScanRequest::UpdateResults()
    {
    DEBUG( "CWlmScanRequest::UpdateResults()" );

    DEBUG1( "CWlmScanRequest::UpdateResults() - scan list count is %u",
    	iDynamicScanListBuf().count );
    DEBUG1( "CWlmScanRequest::UpdateResults() - scan list size is %u",
    	iDynamicScanListBuf().size );

    iScanList->Update( iDynamicScanListBuf().count, iDynamicScanListBuf().size );
    iScanList->Compress();

    return KErrNone;
    }
