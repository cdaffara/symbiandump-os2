/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class to receive asynchronous notifications from WLAN drivers
*
*/


// INCLUDE FILES
#include <e32def.h>
#include <e32std.h>
#include "wlannotificationhandler.h"
#include "RWlanLogicalChannel.h"
#include "am_debug.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CWlanNotificationHandler::CWlanNotificationHandler(
    RWlanLogicalChannel& aChannel,
    MWlanNotificationCallback& aClient ):
    CActive( CActive::EPriorityStandard ),
    iChannel( aChannel ),
    iClient( aClient )
    {
    DEBUG( "CWlanNotificationHandler::CWlanNotificationHandler()" );
    };

// Destructor
CWlanNotificationHandler::~CWlanNotificationHandler()
    {
    DEBUG( "CWlanNotificationHandler::~CWlanNotificationHandler()" );
    Cancel();
    };

// ---------------------------------------------------------
// CWlanNotificationHandler::NewL
// ---------------------------------------------------------
//
CWlanNotificationHandler* CWlanNotificationHandler::NewL(
    RWlanLogicalChannel& aChannel,
    MWlanNotificationCallback& aClient )
    {
    DEBUG( "CWlanNotificationHandler::NewL()" );
    CWlanNotificationHandler* self = new(ELeave) CWlanNotificationHandler( aChannel, aClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWlanNotificationHandler::ConstructL
// ---------------------------------------------------------
//
void CWlanNotificationHandler::ConstructL()
    {
    DEBUG( "CWlanNotificationHandler::ConstructL()" );

    CActiveScheduler::Add( this );    
    }

// ---------------------------------------------------------
// CWlanNotificationHandler::DoCancel
// Cancel is called by the framework when Cancel() is called.
// Pending indication is cancelled from the device driver.
// ---------------------------------------------------------
//
void CWlanNotificationHandler::DoCancel()
    {
    DEBUG( "CWlanNotificationHandler::DoCancel() " );
    iChannel.CancelRequestSignal();
    }

// ---------------------------------------------------------
// CWlanNotificationHandler::RunL
// This is called when active object has received an
// indication.
// ---------------------------------------------------------
//
void CWlanNotificationHandler::RunL()
    {
    DEBUG( "CWlanNotificationHandler::RunL()" );
    iClient.OnNotify( iIndication );
    IssueRequest();
    }

// ---------------------------------------------------------
// CWlanNotificationHandler::Start
// ---------------------------------------------------------
//
TInt CWlanNotificationHandler::Start()
    {
    DEBUG( "CWlanNotificationHandler::Start()" );
    IssueRequest();
    return KErrNone;
    }

// ---------------------------------------------------------
// CWlanNotificationHandler::Stop
// ---------------------------------------------------------
//
void CWlanNotificationHandler::Stop()
    {
    DEBUG( "CWlanNotificationHandler::Stop()" );
    Cancel();
    }    

// ---------------------------------------------------------
// CWlanNotificationHandler::IssueRequest
// Registers pending indication buffer to the device driver.
// ---------------------------------------------------------
//
void CWlanNotificationHandler::IssueRequest()
    {
    DEBUG( "CWlanNotificationHandler::IssueRequest()" );
    iChannel.RequestSignal( iStatus, iIndication );
    SetActive();
    }

// End of file
