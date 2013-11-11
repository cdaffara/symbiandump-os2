/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Notification from engine
*
*/


#include "wlmnotification.h"
#include "am_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSessionNotification::CSessionNotification
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSessionNotification::CSessionNotification(
    CWlmSession& aSession,
    TUint32 aNotificationMask ) :
    iSession( aSession ),
    iNotificationMask( aNotificationMask )
    {
    DEBUG( "CSessionNotification::CSessionNotification()" );
    }

// -----------------------------------------------------------------------------
// CSessionNotification::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSessionNotification* CSessionNotification::NewL(
    CWlmSession& aSession,
    TUint32 aNotificationMask )
    {
    DEBUG( "CSessionNotification::NewL()" );
    CSessionNotification* self =
        new( ELeave ) CSessionNotification( aSession, aNotificationMask );
    return self;
    }

// Destructor
CSessionNotification::~CSessionNotification()
    {
    DEBUG( "CSessionNotification::~CSessionNotification()" );
    }

// -----------------------------------------------------------------------------
// CSessionNotification::AddNotification
// -----------------------------------------------------------------------------
//
void CSessionNotification::AddNotification(
    TUint aNotification,
    TDes8& aData )
    {
    DEBUG( "CSessionNotification::AddNotification()" );

    if( iNotificationMask & aNotification )
        {
        iSession.AddNotification( aNotification, aData );
        }
    }

//  End of File  
