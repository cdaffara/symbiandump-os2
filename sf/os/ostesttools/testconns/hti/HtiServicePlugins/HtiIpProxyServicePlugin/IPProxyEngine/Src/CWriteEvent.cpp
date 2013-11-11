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
* Description:  Write event for queuing data to be sent
*
*/



// INCLUDE FILES
#include    "CWriteEvent.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWriteEvent::CWriteEvent
// -----------------------------------------------------------------------------
//
CWriteEvent::CWriteEvent( TProtocolDesc aProtocolDesc, TInt aPeerPort,
    TInt aOriginalPort ) :
    iPeerPort( aPeerPort ),
    iOriginalPort( aOriginalPort ),
    iProtocolDesc( aProtocolDesc )
    {
    }

// -----------------------------------------------------------------------------
// CWriteEvent::ConstructL
// -----------------------------------------------------------------------------
//
void CWriteEvent::ConstructL( const TDesC8& aData )
    {
    iData = aData.AllocL();
    }

// -----------------------------------------------------------------------------
// CWriteEvent::NewL
// -----------------------------------------------------------------------------
//
CWriteEvent* CWriteEvent::NewL( TProtocolDesc aProtocolDesc,
    const TDesC8& aData, TInt aPeerPort, TInt aOriginalPort )
    {
    CWriteEvent* self = CWriteEvent::NewLC( aProtocolDesc, aData, aPeerPort, aOriginalPort );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CWriteEvent::NewLC
// -----------------------------------------------------------------------------
//
CWriteEvent* CWriteEvent::NewLC( TProtocolDesc aProtocolDesc,
    const TDesC8& aData, TInt aPeerPort, TInt aOriginalPort )
    {
    CWriteEvent* self = new( ELeave ) CWriteEvent( aProtocolDesc, aPeerPort,
        aOriginalPort );
    CleanupStack::PushL( self );

    self->ConstructL( aData );
    return self;
    }


// Destructor
CWriteEvent::~CWriteEvent()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CWriteEvent::ProtocolDesc
// -----------------------------------------------------------------------------
//
TProtocolDesc CWriteEvent::ProtocolDesc() const
    {
    return iProtocolDesc;
    }

// -----------------------------------------------------------------------------
// CWriteEvent::Data
// -----------------------------------------------------------------------------
//
const TDesC8& CWriteEvent::Data() const
    {
    return *iData;
    }

// -----------------------------------------------------------------------------
// CWriteEvent::PeerPort
// -----------------------------------------------------------------------------
//
TInt CWriteEvent::PeerPort() const
    {
    return iPeerPort;
    }

// -----------------------------------------------------------------------------
// CWriteEvent::OriginalPort
// -----------------------------------------------------------------------------
//
TInt CWriteEvent::OriginalPort() const
    {
    return iOriginalPort;
    }


//  End of File
