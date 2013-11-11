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
* Description:  HtiEchoServicePlugin implementation
*
*/


// INCLUDE FILES
#include "HtiEchoServicePlugin.h"
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

// CONSTANTS
const static TUid KEchoServiceUid = { 0x1020DEBF };

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// Create instance of concrete ECOM interface implementation
CHtiEchoServicePlugin* CHtiEchoServicePlugin::NewL()
    {
    CHtiEchoServicePlugin* self = new (ELeave) CHtiEchoServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Constructor
CHtiEchoServicePlugin::CHtiEchoServicePlugin():
    iReply( NULL )
    {
    }

CHtiEchoServicePlugin::~CHtiEchoServicePlugin()
    {
    HTI_LOG_TEXT( "CHtiEchoServicePlugin destroy" );
    delete iReply;
    }

// Second phase construction.
void CHtiEchoServicePlugin::ConstructL()
    {
    HTI_LOG_TEXT( "CHtiEchoServicePlugin::ConstructL" );
    }

void CHtiEchoServicePlugin::ProcessMessageL( const TDesC8& aMessage,
                                 THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiEchoServicePlugin::ProcessMessage" );
    HTI_LOG_FORMAT( "Msg len: %d", aMessage.Length() );

    if ( iReply )
        {
        HTI_LOG_TEXT( "sending previous echo, reject request" );
        return;
        }

    //make copy of the message and send it back
    iReply = HBufC8::NewL( aMessage.Length() );
    iReply->Des().Copy( aMessage );

    if ( iDispatcher->DispatchOutgoingMessage( iReply, KEchoServiceUid )
            == KErrNoMemory )
        {
        HTI_LOG_TEXT( "KErrNoMemory" );
        iDispatcher->AddMemoryObserver( this );
        }
    else
        {
        iReply = NULL;
        }
    HTI_LOG_FUNC_OUT( "CHtiEchoServicePlugin::ProcessMessage" );
    }

void CHtiEchoServicePlugin::NotifyMemoryChange( TInt aAvailableMemory )
    {

    if ( iReply )
        {
        if ( aAvailableMemory >= iReply->Size() )
            {
            TInt err = iDispatcher->DispatchOutgoingMessage( iReply,
                            KEchoServiceUid );

            if (  err == KErrNone )
                {
                iReply = NULL;
                iDispatcher->RemoveMemoryObserver( this );
                }
            else if ( err != KErrNoMemory ) //some other error
                {
                delete iReply;
                iReply = NULL;
                iDispatcher->RemoveMemoryObserver( this );
                }
            }
        }
    else
        {
        //some error, should not be called
        iDispatcher->RemoveMemoryObserver( this );
        }
    }


// End of File
