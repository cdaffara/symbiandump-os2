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
* Description:  Implementation of CHtiBtCommServerSession class. This class
*                represents the client session in server.
*
*/



// INCLUDE FILES
#include "HtiBtClientServerCommon.h"
#include "HtiBtCommServerSession.h"
#include "HtiBtCommServer.h"
#include "BtSerialClient.h"
#include "Logger.h"

#include <e32base.h>
#include <e32std.h>
#include <e32svr.h>

#include <HtiCommPluginInterface.h> // KErrComModuleReset;

// CONSTANTS
 _LIT(KBtComSessPanic, "BtComSessAssrt");

const TInt KBtAddressHexStringLength = 12; // 6 bytes
const TInt KIncomingDataBufSize = 32 * 1024;

//*****************************************************************************
//
// Class CHtiBtCommServerSession
//
//*****************************************************************************

/*---------------------------------------------------------------------------*/
CHtiBtCommServerSession::CHtiBtCommServerSession( CHtiBtCommServer* aServer )
    : CSession2()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: CHtiBtCommServerSession()")))

    __DECLARE_NAME( _S( "CHtiBtCommServerSession" ) );
    iBtCommServer = aServer;
    iReadRequestComplete = ETrue;  // ready to accept read request
    iWriteRequestComplete = ETrue;
    iConnectRequestComplete = ETrue;
    iBtCommServer->SessionCreated(this);
    }

/*---------------------------------------------------------------------------*/
CHtiBtCommServerSession* CHtiBtCommServerSession::NewL(
    CHtiBtCommServer* aServer )
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: NewL()")))
    CHtiBtCommServerSession* session =
        new (ELeave) CHtiBtCommServerSession( aServer );
    CleanupStack::PushL( session );
    session->ConstructL();
    CleanupStack::Pop( session );
    return session;
    }

/*---------------------------------------------------------------------------*/

void CHtiBtCommServerSession::ConstructL()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: ConstructL()")))
    iBtClient = CBtSerialClient::NewL(*this);
    iIncomingDataBuf = HBufC8::NewL(KIncomingDataBufSize);
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: ConstructL(): Done")))
    }

/*---------------------------------------------------------------------------*/
CHtiBtCommServerSession::~CHtiBtCommServerSession()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: ~CHtiBtCommServerSession()")))
    delete iBtClient;
    ResetAll(KErrCancel);
    delete iIncomingDataBuf;
    if ( iBtCommServer )
        {
        iBtCommServer->SessionFreed();
        }
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: ~CHtiBtCommServerSession(): Done")))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::ServiceL( const RMessage2& aMessage )
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: ServiceL()")))

    TRAPD( error, DispatchMessageL( aMessage ) );
    if ( error != KErrNone )
        {
        LOGFW(DebugLog(_L("CHtiBtCommServerSession::ServiceL error %d"), error);)
        }
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: ServiceL(): Done")))
    }


/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::DispatchMessageL( const RMessage2 &aMessage )
    {
    LOGFW( DebugLog( _L( "CHtiBtCommServerSession: DispatchMessageL()" ) ) )

    switch ( aMessage.Function() )
        {
        case EBtCommServerConnect:
            {
            HandleConnectRequestL( aMessage );
            }
            break;

        case EBtCommServerSend:
            {
            HandleSendRequestL( aMessage );
            }
            break;
        case EBtCommServerRecv:
            {
            HandleReadRequestL( aMessage );
            }
            break;

        case ECancelBtCommServerRecv:
        case ECancelBtCommServerSend:
            {
            HandleCancelRequestL( aMessage );
            }
            break;

        case EGetServicePortNumber:
            {
            HandlePortNumberRequestL( aMessage );
            }
            break;

        default:
            {
            LOGFW(ErrLog(_L("CHtiBtCommServerSession::DispatchMessageL: Unknown request: %d. Panicing Client"), aMessage.Function());)
            PanicClient( EBadRequest );
            User::Panic( KBtComSessPanic, 1 );
            }
            break;
        }
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: DispatchMessageL(): Done")))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::HandleConnectRequestL( const RMessage2& aMessage )
    {
    LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL()")))
    if ( !iConnectRequestComplete )
        {
        LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Already connecting")))
        aMessage.Complete( KErrNotReady );
        }
    else
        {
        if ( iBtClient->Connected() )
            {
            LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Already connected")))
            aMessage.Complete( KErrNone ); // Already connected. This is not an error?
            }
        else
            {
            LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Starting to connect to remote host")))
            iConnectRequest = aMessage;
            iConnectRequestComplete = EFalse;

            TInt dataLength = (TUint16)aMessage.GetDesLength( 0 ); //first message slot
            LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Data length = %d"), dataLength));

            TInt port = aMessage.Int1();
            LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Port = %d"), port));

            if ( dataLength < 1 )
                {
                LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): No address or name - need to ask device")))
                iBtClient->ConnectL(); // Request is completed in ConnectedToServer
                }
            else if ( dataLength == KBtAddressHexStringLength )
                {
                TBuf8<KBtAddressHexStringLength> addressBuf8;
                aMessage.ReadL( 0, addressBuf8 );
                TBuf<KBtAddressHexStringLength> addressBuf;
                addressBuf.Copy( addressBuf8 );
                TBTDevAddr address;
                TInt result = address.SetReadable( addressBuf );
                LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): SetReadable result = %d"), result));
                if ( result != KBtAddressHexStringLength )
                    {
                    LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Not valid address - use it as name")))
                    iBtClient->ConnectL( addressBuf, port );
                    }
                else
                    {
                    LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Valid address - connect")))
                    iBtClient->ConnectL( address, port );
                    }
                }
            else
                {
                LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Using name to connect")))
                TBTDeviceName8 deviceName8;
                aMessage.ReadL( 0, deviceName8 );
                TBTDeviceName deviceName;
                deviceName.Copy( deviceName8 );
                iBtClient->ConnectL( deviceName, port );
                }
            }
        }
    LOGFW(DebugLog(_L("CHtiBtCommServer:HandleConnectRequestL(): Done")))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::HandleReadRequestL( const RMessage2& aMessage )
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: HandleReadRequestL()")))

    if ( !iReadRequestComplete )
        {
        LOGFW(WarnLog(_L("CHtiBtCommServerSession: HandleReadRequestL(): Pending request. ret=KErrNotReady")))
        // Two simultaneous read requests are not allowed.
        aMessage.Complete(KErrNotReady);
        return;
        }

    // No data in read buffer. Must wait for new data before completing request
    iReadRequest = aMessage;
    iReadRequestComplete = EFalse; // Not ready to complete next request
    TryCompleteReadRequest(); // if there is something in the readbuffer,
                              // use it for completion.

    if ( iIncomingDataBuf->Des().Length() == 0 )
        {
        // Read some bytes to buffer even before client's request.
        iBtClient->ReadAsyncL(); // Issue async request to read more data.
        }

    LOGFW(DebugLog(_L("CHtiBtCommServerSession: HandleReadRequestL(): Done")))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::HandleSendRequestL( const RMessage2& aMessage )
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: HandleSendRequestL()")))

    TInt dataLength = (TUint16)aMessage.GetDesLength( 0 ); //first message slot
    LOGFW( InfoLog( _L( "Framework requested to send data: %d bytes" ), dataLength ) )
    if ( iBtClient->SendBufferMaxSize() < dataLength )
        {
        aMessage.Complete(KErrOverflow);
        LOGFW(ErrLog(_L("CHtiBtCommServerSession: HandleSendRequestL(): client is giving too big data. Cannot handle.")))
        }
    else if ( !iWriteRequestComplete )
        {
        LOGFW(WarnLog(_L("CHtiBtCommServerSession: HandleSendRequestL(): Pending request. ret=KErrNotReady")))
        // Already pending send request. Two simultaneous send requests not allowed.
        aMessage.Complete( KErrNotReady );
        }
    else
        {
        if ( iBtClient->FreeSpaceInSendBuffer() < dataLength )
            {
            LOGFW(DebugLog(_L("CHtiBtCommServerSession: HandleSendRequestL(): Completing send request delayed")))
            // No space in the iBtClient's send buffer. Copy the data to internal
            // buffer and wait for iBtClient to send its data out before adding
            // data to its send buffer.
            delete iSendBuffer;
            iSendBuffer = NULL;
            iSendBuffer = HBufC8::NewL(dataLength);
            TPtr8 ptr = iSendBuffer->Des();
            aMessage.ReadL( 0, ptr );
            iWriteRequestComplete = EFalse;
            iWriteRequest = aMessage;
            // Wait for callback call AllBufferedDataSent. Complete request there.
            }
        else
            {
            LOGFW(DebugLog(_L("CHtiBtCommServerSession: HandleSendRequestL(): Completing send request immediately")))
            // All data can be sent immediately, because there is enough space in
            // iBtClient's send buffer.
            HBufC8* data = HBufC8::NewLC( dataLength );
            TPtr8 ptr = data->Des();
            aMessage.ReadL( 0, ptr );
            iBtClient->SendL( ptr );
            CleanupStack::PopAndDestroy( data );
            aMessage.Complete( KErrNone );
            }
        }
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: HandleSendRequestL(): Done")))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::HandleCancelRequestL( const RMessage2& aMessage )
    {
    ResetAll( KErrCancel );
    aMessage.Complete( KErrNone );
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::HandlePortNumberRequestL(const RMessage2& aMessage)
    {
    TPckgBuf<TInt> p( iBtClient->ServicePort() );
    TInt err = aMessage.Write( 0, p, 0 );
    aMessage.Complete( err );
    }

/*---------------------------------------------------------------------------*/

void CHtiBtCommServerSession::PanicClient(TInt aPanic) const
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: PanicClient(): %d"), aPanic))
    LOGFW(WarnLog(_L("CHtiBtCommServerSession: PanicClient(): %d. NOT IMPLEMENTED"), aPanic))
    aPanic = aPanic;
    // should be done with RMessage2?
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::ResetAll(TInt aCompletionCode)
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: ResetAll()")))
    if ( !iWriteRequestComplete )
        iWriteRequest.Complete( aCompletionCode );
    if ( !iReadRequestComplete )
        iReadRequest.Complete( aCompletionCode );
    if ( !iConnectRequestComplete )
        iConnectRequest.Complete( aCompletionCode ); // error when making connection
    iWriteRequestComplete = ETrue;
    iReadRequestComplete = ETrue;
    iConnectRequestComplete = ETrue;
    delete iIncomingDataBuf;
    iIncomingDataBuf = NULL;
    delete iSendBuffer;
    iSendBuffer = NULL;
    iIncomingDataBuf = HBufC8::New(KIncomingDataBufSize);
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: ResetAll(): Done")))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::ConnectedToServer(TInt aError)
    {
    // Connected to server. Reading has been started.
    LOGFW(InfoLog(_L("CHtiBtCommServerSession: ConnectedToServer: connected to remote host")))
    iConnectRequestComplete = ETrue;
    iConnectRequest.Complete( aError );
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::DisconnectedFromServer()
    {
    LOGFW(InfoLog(_L("CHtiBtCommServerSession: DisconnectedFromServer: disconnected from remote host")))
    ResetAll( KErrDisconnected );
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::DataFromServer(const TDesC8& aData)
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: DataFromServer: %d bytes"), aData.Length()))
    TPtr8 ptr = iIncomingDataBuf->Des();
    if ( aData.Length() > ptr.MaxLength() - ptr.Length() )
        PanicClient( KErrUnderflow ); // Client is reading too slowly

    ptr.Append( aData );
    TryCompleteReadRequest();
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: DataFromServer: done"), aData.Length()))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::AllBufferedDataSent()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: AllBufferedDataSent")))
    if ( !iWriteRequestComplete )
        {
        // iBtClient has sent all of its data and is ready to send more.
        TPtr8 ptr = iSendBuffer->Des();
        TRAPD( err, iBtClient->SendL( ptr ); )
        iWriteRequest.Complete( err );
        iWriteRequestComplete = ETrue;
        delete iSendBuffer;
        iSendBuffer = NULL;
        }
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: AllBufferedDataSent: Done")))
    }

/*---------------------------------------------------------------------------*/
void CHtiBtCommServerSession::TryCompleteReadRequest()
    {
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: TryCompleteReadRequest")))
    TPtr8 ptr = iIncomingDataBuf->Des();

    if ( iReadRequestComplete || ptr.Length() == 0 )
        {
        LOGFW(DebugLog(_L("CHtiBtCommServerSession: TryCompleteReadRequest: Nothing to complete")))
        return; // No outstanding client request to complete
        }

    TInt dataMaxLength = (TUint16)iReadRequest.GetDesMaxLength(0); //first message slot
    TPtrC8 dataToClient = ptr.Left(dataMaxLength);
    TRAPD(err, iReadRequest.WriteL(0, dataToClient);)

    ptr.Delete(0, dataToClient.Length());
    iReadRequest.Complete(err);
    iReadRequestComplete = ETrue;
    LOGFW(DebugLog(_L("CHtiBtCommServerSession: TryCompleteReadRequest: Done")))
    }

// End of file
