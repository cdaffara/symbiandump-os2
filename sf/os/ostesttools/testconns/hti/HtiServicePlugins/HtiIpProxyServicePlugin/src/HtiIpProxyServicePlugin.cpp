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
* Description:  HtiIpProxyServicePlugin implementation
*
*/


// INCLUDE FILES
#include "HtiIpProxyServicePlugin.h"
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>
#include <badesca.h>
#include <in_sock.h>
#include "MSocketObserver.h"
#include "Mhostconnectionobserver.h"
#include "CIPProxyEngine.h"

// CONSTANTS
const static TUid KIpProxyServiceUid = { 0x10210CD3 };

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// Create instance of concrete ECOM interface implementation
CHtiIpProxyServicePlugin* CHtiIpProxyServicePlugin::NewL()
    {
    CHtiIpProxyServicePlugin* self = new( ELeave ) CHtiIpProxyServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Constructor
CHtiIpProxyServicePlugin::CHtiIpProxyServicePlugin() :
    iSocketObserver( NULL ),
    iHostObserver( NULL ),
    iProxyEngine( NULL ),
    iOutgoingArray( NULL ),
    iBusy( EFalse ),
    iConnected( EFalse )
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin" );
    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin" );
    }

CHtiIpProxyServicePlugin::~CHtiIpProxyServicePlugin()
    {
    HTI_LOG_FUNC_IN( "~CHtiIpProxyServicePlugin" );

    if ( iProxyEngine )
        {
        iProxyEngine->StopListening();
        }
    delete iProxyEngine;
    delete iOutgoingArray;

    HTI_LOG_FUNC_OUT( "~CHtiIpProxyServicePlugin" );
    }

// Second phase construction.
void CHtiIpProxyServicePlugin::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::ConstructL" );

    iOutgoingArray = new( ELeave ) CDesC8ArraySeg( 10 );
    iProxyEngine = CIPProxyEngine::NewL( this );

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::ConstructL" );
    }


void CHtiIpProxyServicePlugin::InitL()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::InitL" );

    if ( iProxyEngine )
        {
        iProxyEngine->StartListening();
        }

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::InitL" );
    }

TBool CHtiIpProxyServicePlugin::IsBusy()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::IsBusy" );
    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::IsBusy" );
    return iBusy;
    }

void CHtiIpProxyServicePlugin::ProcessMessageL( const TDesC8& aMessage,
                                                THtiMessagePriority /* aPriority */ )
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::ProcessMessageL" );

    iBusy = ETrue;

    __ASSERT_ALWAYS( iSocketObserver, User::Panic( _L ( "IP-Proxy ECom-plugin" ), KErrBadHandle ) );
    iSocketObserver->DataReceivedL( this, aMessage );

    iBusy = EFalse;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::ProcessMessageL" );
    }

void CHtiIpProxyServicePlugin::NotifyMemoryChange( TInt aAvailableMemory )
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::NotifyMemoryChange" );

    iBusy = ETrue;

    TInt count = iOutgoingArray->Count();
    if ( !count )
        {
        // No outgoing data
        return;
        }

    // Calculate needed outgoing buffer size
    TInt i = 0;
    TInt neededSize = 0;
    for ( ; i < count; ++i )
        {
        TInt size = ( *iOutgoingArray )[i].Size();
        if ( neededSize + size <= aAvailableMemory )
            {
            neededSize += size;
            }
        }

    if ( !neededSize )
       {
       // Not enough memory
       return;
       }

    // Create an outgoing buffer and join as many buffers as possible
    HBufC8* message = HBufC8::New( neededSize );
    if ( !message )
        {
        // Not enough memory
        return;
        }

    TPtr8 messagePtr( message->Des() );
    for ( TInt j = 0; j < i; ++j )
        {
        messagePtr.Append( ( *iOutgoingArray )[j] );
        }

    // Dispatch message to HTI-Framework which takes the ownership of message
    TInt err = iDispatcher->DispatchOutgoingMessage( message, KIpProxyServiceUid );
    if ( err == KErrNone )
        {
        message = NULL;
        iOutgoingArray->Delete( 0, i );
        iDispatcher->RemoveMemoryObserver( this );
        }
    else if ( err != KErrNoMemory )
        {
        delete message;
        iDispatcher->RemoveMemoryObserver( this );

        __ASSERT_ALWAYS( iHostObserver, User::Panic( _L ( "IP-Proxy ECom-plugin" ), KErrBadHandle ) );
        TRAP( err, iHostObserver->HostConnectionErrorL( err ) );
        }

    iBusy = EFalse;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::NotifyMemoryChange" );
    }

MSocket* CHtiIpProxyServicePlugin::GetSocket()
    {
    return this;
    }

MHostConnection* CHtiIpProxyServicePlugin::GetHostConnection()
    {
    return this;
    }

RSocket* CHtiIpProxyServicePlugin::GetRSocket()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::GetRSocket" );

    RSocket* KSocket = NULL;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::GetRSocket" );
    return KSocket;
    }

TUint CHtiIpProxyServicePlugin::LocalPort() const
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::LocalPort" );

    const TUint KLocalPort = 0;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::LocalPort" );
    return KLocalPort;
    }

TUint CHtiIpProxyServicePlugin::RemotePort() const
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::RemotePort" );

    const TUint KRemotePort = 0;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::RemotePort" );
    return KRemotePort;
    }

void CHtiIpProxyServicePlugin::SetObserver( MSocketObserver* aObserver )
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::SetObserver" );

    iSocketObserver = aObserver;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::SetObserver" );
    }

void CHtiIpProxyServicePlugin::SetSocketOwnershipMode( TBool /* aHasOwnership */ )
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::SetSocketOwnershipMode" );

    /* Empty implementation */

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::SetSocketOwnershipMode" );
    }

void CHtiIpProxyServicePlugin::IssueRead()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::IssueRead" );

    /* Empty implementation */

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::IssueRead" );
    }

void CHtiIpProxyServicePlugin::Cancel()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::Cancel" );

    /* Empty implementation */

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::Cancel" );
    }

void CHtiIpProxyServicePlugin::SocketInfo( TProtocolDesc& aDesc ) const
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::SocketInfo" );

    aDesc.iProtocol = KProtocolInetTcp;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::SocketInfo" );
    }

TBool CHtiIpProxyServicePlugin::IsUDP() const
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::IsUDP" );
    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::IsUDP" );
    return EFalse;
    }

void CHtiIpProxyServicePlugin::WriteL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::WriteL" );

    iBusy = ETrue;

    // Dispatcher takes ownership of the message if no error has occured
    HBufC8* message = aData.AllocL();
    TInt err = iDispatcher->DispatchOutgoingMessage( message, KIpProxyServiceUid );
    if ( err != KErrNone && err != KErrNoMemory )
        {
        // Some error occured while dispatching the message
        delete message;
        __ASSERT_ALWAYS( iHostObserver, User::Panic( _L ( "IP-Proxy ECom-plugin" ), KErrBadHandle ) );
        iHostObserver->HostConnectionErrorL( err );
        }
    else if ( err == KErrNoMemory )
        {
        // No memory was available so add message to queue
        delete message;
        iOutgoingArray->AppendL( aData.Mid( 0 ) );
        iDispatcher->AddMemoryObserver( this );
        }

    iBusy = EFalse;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::WriteL" );
    }

void CHtiIpProxyServicePlugin::IssueConnectL()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::IssueConnectL" );

    iConnected = ETrue;

    __ASSERT_ALWAYS( iHostObserver, User::Panic( _L ( "IP-Proxy ECom-plugin" ), KErrBadHandle ) );
    iHostObserver->ConnectionEstablishedL();

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::IssueConnectL" );
    }

void CHtiIpProxyServicePlugin::IssueDisconnect()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::IssueDisconnect" );

    __ASSERT_ALWAYS( iSocketObserver, User::Panic( _L ( "IP-Proxy ECom-plugin" ), KErrBadHandle ) );
    TRAP_IGNORE( iSocketObserver->DisconnectedL( this ) );

    iConnected = EFalse;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::IssueDisconnect" );
    }

void CHtiIpProxyServicePlugin::SetObserver( MHostConnectionObserver* aObserver )
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::SetObserver 2" );

    iHostObserver = aObserver;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::SetObserver 2" );
    }

TBool CHtiIpProxyServicePlugin::IsConnected()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::IsConnected" );
    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::IsConnected" );
    return iConnected;
    }

RSocket* CHtiIpProxyServicePlugin::Socket()
    {
    HTI_LOG_FUNC_IN( "CHtiIpProxyServicePlugin::Socket" );

    RSocket* KSocket = NULL;

    HTI_LOG_FUNC_OUT( "CHtiIpProxyServicePlugin::Socket" );
    return KSocket;
    }
