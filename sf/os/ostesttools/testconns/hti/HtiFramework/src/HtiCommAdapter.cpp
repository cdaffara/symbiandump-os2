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
* Description:  CHtiCommAdapter implementation
*
*/


#include "HtiCommAdapter.h"
#include "HtiDispatcher.h"
#include "HtiCommPluginInterface.h"
#include "HtiMessage.h"
#include "HtiNotifier.h"

#include "HtiLogging.h"

//default value for max message size for incoming messages
//used if value in constructor is not valid (<0)
const static TInt KMaxMessageSize = 10 * 1024; // 10 KB

const static TInt KHtiCommPriority = 2;

_LIT( KCommPanic, "HtiCommAdapter" );

CHtiCommAdapter* CHtiCommAdapter::NewLC( CHTICommPluginInterface* aCommPlugin,
    CHtiDispatcher* aDispatcher, TInt aMaxMsgSize )
    {
    CHtiCommAdapter* obj = new (ELeave) CHtiCommAdapter( aDispatcher,
                                        aCommPlugin, aMaxMsgSize );
    CleanupStack::PushL( obj );
    obj->ConstructL();
    return obj;
    }

CHtiCommAdapter* CHtiCommAdapter::NewL( CHTICommPluginInterface* aCommPlugin,
    CHtiDispatcher* aDispatcher, TInt aMaxMsgSize )
    {
    CHtiCommAdapter* obj = NewLC( aCommPlugin, aDispatcher, aMaxMsgSize );
    CleanupStack::Pop();
    return obj;
    }

CHtiCommAdapter::CHtiCommAdapter( CHtiDispatcher* aDispatcher,
                           CHTICommPluginInterface* aCommPlugin,
                           TInt aMaxMsgSize )
    :CActive( KHtiCommPriority ),
    iState( EIdle ),
    iDispatcher( aDispatcher ),
    iCommPlugin( aCommPlugin ),
    iBufferPtr( NULL, 0 ),
    iMsgToReceive( NULL ),
    iMsgToSend( NULL ),
    iMsgToSendPtr( NULL, 0 ),
    iMsgToSendOffset( 0 ),
    iSkipLength( 0 ),
    iMaxMessageSize( aMaxMsgSize > 0 ? aMaxMsgSize : KMaxMessageSize )
    {
    HTI_LOG_FORMAT( "MaxMsgSize %d", iMaxMessageSize );
    };

CHtiCommAdapter::~CHtiCommAdapter()
    {
    HTI_LOG_FUNC_IN( "CHtiCommAdapter::~CHtiCommAdapter" );
    Cancel();

    delete iBuffer;
    delete iLeftovers;

    delete iMsgToReceive;
    delete iMsgToSend;

    HTI_LOG_FUNC_OUT( "CHtiCommAdapter::~CHtiCommAdapter" );
    }

void CHtiCommAdapter::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiCommAdapter::ConstructL" );

    //allocate recommended buffer plus some extra space
    //needed to handle msg parsing
    iBuffer = HBufC8::NewL( iCommPlugin->GetReceiveBufferSize() +
                CHtiMessage::MinHeaderSize() );

    iBuffer->Des().SetLength( iCommPlugin->GetReceiveBufferSize() );
    iBufferPtr.Set( iBuffer->Des().LeftTPtr(
            iCommPlugin->GetReceiveBufferSize() ) );

    iLeftovers = HBufC8::NewL( CHtiMessage::MinHeaderSize() );

    CActiveScheduler::Add( this );
    HTI_LOG_FUNC_OUT( "CHtiCommAdapter::ConstructL" );
    }

void CHtiCommAdapter::ReceiveMessage()
    {
    ReceiveMessage( EFalse );
    }

void CHtiCommAdapter::ReceiveMessage( TBool aContinue )
    {
    HTI_LOG_FUNC_IN( "CHtiCommAdapter::ReceiveMessage" );

    iState = aContinue?EReceivingCont:EReceiving;
    iCommPlugin->Receive( iBufferPtr, iStatus );
    SetActive();

    HTI_LOG_FUNC_OUT( "CHtiCommAdapter::ReceiveMessage" );
    }

void CHtiCommAdapter::SendMessage( CHtiMessage* aMessage )
    {
    HTI_LOG_FUNC_IN( "CCommAdapter::SendMessage" );
    iState = ESending;
    if ( aMessage != NULL )
        {
        HTI_LOG_TEXT( "send first packet" );
        iMsgToSend = aMessage;
        iMsgToSendOffset = 0;

        //first send header
        iMsgToSendPtr.Set( iMsgToSend->Header() );
        }
    else if ( iMsgToSend != NULL )
        {
        //send next chunk
        if ( iMsgToSend->BodySize() <=
            ( iMsgToSendOffset + iCommPlugin->GetSendBufferSize() ) )
            {
            HTI_LOG_TEXT( "send remainder" );
            iMsgToSendPtr.Set( iMsgToSend->Body().Mid( iMsgToSendOffset ) );
            iMsgToSendOffset = iMsgToSend->BodySize(); //last sending
            }
        else
            {
            HTI_LOG_TEXT( "send packet" );
            iMsgToSendPtr.Set( iMsgToSend->Body().Mid( iMsgToSendOffset,
                                    iCommPlugin->GetSendBufferSize() ) );
            iMsgToSendOffset += iCommPlugin->GetSendBufferSize();
            }
        }
    else
        {
        User::Panic( KCommPanic, KErrGeneral );
        }

    iCommPlugin->Send( iMsgToSendPtr, iStatus );
    SetActive();

    HTI_LOG_FUNC_OUT( "CCommAdapter::SendMessage" );
    }

void CHtiCommAdapter::Reset()
    {
    HTI_LOG_FUNC_IN( "CHtiCommAdapter::Reset" );
    Cancel();
    //receiving msg
    delete iMsgToReceive;
    iMsgToReceive = NULL;

    //send msg
    delete iMsgToSend;
    iMsgToSend = NULL;
    iMsgToSendOffset = 0;

    //empty buffers
    iLeftovers->Des().Zero();
    iBuffer->Des().Zero();
    iSkipLength = 0;

    //set idle
    iState = EIdle;

    HTI_LOG_FUNC_OUT( "CHtiCommAdapter::Reset" );
    }

void CHtiCommAdapter::RunL()
    {
    HTI_LOG_FUNC_IN( "CHtiCommAdapter::RunL" );
    HTI_LOG_FORMAT( "iStatus %d", iStatus.Int() );

    //handle reset code common for all states
    if ( iStatus == KErrComModuleReset )
        {
        HTI_LOG_TEXT( "Reset received from comm module" );
        //reset dispatcher
        iDispatcher->Reset();
        //dispatcher will call Reset() for comm adapters and reissue requests
        return;
        }

    //check for critical (unrecoverable) communication errors
    else if ( iStatus == KErrServerTerminated )
        {
        if ( iDispatcher->GetShowErrorDialogs() )
            {
            User::Panic( KCommPanic, KErrServerTerminated );
            }
        else
            {
            User::Exit( KErrServerTerminated );
            }
        }
    // USB errors from d32usbc.h
    else if ( -6700 > iStatus.Int() && iStatus.Int() > -6712 )
        {
        if(iDispatcher->CommReconnect())
            {
            return;
            }

        if ( iDispatcher->GetShowErrorDialogs() )
            {
            TBuf<48> errorText;
            errorText.Append( _L( "USB connection lost (" ) );
            errorText.AppendNum( iStatus.Int() );
            errorText.Append( _L( "). HTI stopped." ) );
            CHtiNotifier::ShowErrorL( errorText );
            }
        User::Exit( KErrDisconnected );
        }
    else if ( iStatus == KErrDisconnected )
        {
        // This happens if Bluetooth connection is lost.
        if(iDispatcher->CommReconnect())
            {
            return;
            }
        
        if ( iDispatcher->GetShowErrorDialogs() )
            {
            CHtiNotifier::ShowErrorL(
                    _L( "Connection lost. HTI stopped." ), KErrDisconnected );
            }
        User::Exit( KErrDisconnected );
        }

    switch ( iState )
        {
        case EReceiving:
            //process
            HTI_LOG_TEXT( "EReceiving" );
            if ( iStatus == KErrNone )
                {
                iBuffer->Des().SetLength( iBufferPtr.Length() );
                TRAPD( err, HandleReceiveL() );
                if ( err != KErrNone )
                    {
                    HTI_LOG_FORMAT( "Error in HandleReceiveL %d", err );

                    delete iMsgToReceive;
                    iMsgToReceive = NULL;

                    iDispatcher->Notify( err );

                    ReceiveMessage();
                    }
                }
            else
                {
                HTI_LOG_FORMAT( "Error %d, reissue request", iStatus.Int() );
                iDispatcher->Notify( iStatus.Int() );
                User::After(2000000);
                ReceiveMessage();
                }
            break;
        case EReceivingCont:
            //process
            HTI_LOG_TEXT( "EReceivingCont" );
            if ( iStatus == KErrNone )
                {
                iBuffer->Des().SetLength( iBufferPtr.Length() );
                TRAPD( err, HandleReceiveContL() );
                if ( err != KErrNone )
                    {
                    HTI_LOG_FORMAT( "Error in HandleReceiveContL %d", err );

                    delete iMsgToReceive;
                    iMsgToReceive = NULL;

                    iDispatcher->Notify( err );

                    ReceiveMessage();
                    }
                }
            else
                {
                HTI_LOG_FORMAT( "Error %d, reissue request", iStatus.Int() );
                HTI_LOG_TEXT( "and dismiss received HTI msg beginning" );
                delete iMsgToReceive;
                iMsgToReceive = NULL;

                iDispatcher->Notify( iStatus.Int() );
                User::After(2000000);
                ReceiveMessage();
                }
            break;
        case ESending:
            HTI_LOG_TEXT( "ESending" );
            if ( iStatus == KErrNone )
                {
                HandleSend();
                }
            else
                {
                HTI_LOG_FORMAT( "Error %d, stop sending, go to EIdle", iStatus.Int() );

                delete iMsgToSend;
                iMsgToSend = NULL;
                iMsgToSendOffset = 0;
                iState = EIdle;

                iDispatcher->Notify( iStatus.Int() );
                }
            break;
        case EIdle:
            //ERROR
            HTI_LOG_TEXT( "EIdle" );
            User::Panic( KCommPanic, KErrGeneral );
            break;
        default:
            //ERROR
            HTI_LOG_TEXT( "default" );
            User::Panic( KCommPanic, KErrGeneral );
        };

    HTI_LOG_FUNC_OUT( "CHtiCommAdapter::RunL" );
    }

void CHtiCommAdapter::HandleReceiveL()
    {
    TBool toContinue = EFalse; //for reissuing state

    if ( iLeftovers->Length() > 0 )
        {
        HTI_LOG_TEXT( "handle leftovers" );
        //if something left from previous time
        //insert it to the buffer beggining
        //check that there is enough space (should be)
        if ( iBuffer->Length() + iLeftovers->Length() <=
             iBuffer->Des().MaxLength() )
            {
            iBuffer->Des().Insert( 0, *iLeftovers );
            }
        else
            {
            //error
            HTI_LOG_TEXT( "iMsgToReceive contain too much leftovers, drop them" );
            }

        iLeftovers->Des().Zero();
        }

    //use loop cause can be several HTI messages in iBuffer
    while ( iBuffer->Length() >= CHtiMessage::MinHeaderSize() )
        {
        HTI_LOG_FORMAT( "iBuffer.Length() %d", iBuffer->Length() );
        HTI_LOG_TEXT( "check header" );

        if ( CHtiMessage::CheckValidHtiHeader( *iBuffer ) )
            {
            HTI_LOG_TEXT( "valid header" );
            TInt msgSize = CHtiMessage::Size( *iBuffer );
            HTI_LOG_FORMAT( "msgSize %d", msgSize );
            HTI_LOG_HEX( iBuffer->Des().Ptr(), 14 );

            __ASSERT_ALWAYS( iMsgToReceive == NULL,
                            User::Panic( KCommPanic, KErrGeneral ) );

            //check msgSize is acceptable
            if ( msgSize > iMaxMessageSize )
                {
                //send err message
                iDispatcher->DispatchOutgoingErrorMessage( EHtiErrBigMessage );
                iSkipLength = msgSize;
                }
            else if ( msgSize > iDispatcher->GetFreeMemory() )
                {
                //send err message
                iDispatcher->DispatchOutgoingErrorMessage( EHtiErrNoMemory );
                iSkipLength = msgSize;
                }
            else
                {
                iMsgToReceive = CHtiMessage::NewL( *iBuffer );
                }

            if ( iMsgToReceive == NULL )
                {
                HTI_LOG_TEXT( "skip message" );
                if ( iSkipLength > iBuffer->Length() )
                    {
                    iSkipLength -= iBuffer->Length();
                    iBuffer->Des().Zero();
                    // and continue to receive HTI message
                    //switch state to EReceiveCont
                    toContinue = ETrue;
                    }
                else
                    {
                    //should not be here, but just in case
                    iBuffer->Des().Delete( 0, iSkipLength );
                    iSkipLength = 0;
                    }
                }
            else if ( iMsgToReceive->IsBodyComplete() )
                {
                HTI_LOG_TEXT( "handle small message" );
                //iMsgToReceive is already contain whole HTI message
                iDispatcher->DispatchIncomingMessage( iMsgToReceive );

                //delete sent msg from buffer (and procces the rest)
                iBuffer->Des().Delete( 0, iMsgToReceive->Size() );
                iMsgToReceive = NULL;
                }
            else
                {
                HTI_LOG_TEXT( "start receive big message" );
                //clear it to leave loop
                iBuffer->Des().Zero();
                // and continue to receive HTI message
                //switch state to EReceiveCont
                toContinue = ETrue;
                }
            }
        else
            {
            HTI_LOG_TEXT( "invalid header, dismiss buffer and reissue request" );
            HTI_LOG_HEX( iBuffer->Des().Ptr(), 14 );
            // clear all buffer cause don't know where is valid header starts
            // and wait for the next packet
            iBuffer->Des().Zero(); //to leave loop
            }
        }

    if ( iBuffer->Length() > 0 )
        {
        HTI_LOG_FORMAT( "copy leftovers %d", iBuffer->Length() );
        //header beggining left in the buffer
        //copy it to iLeftovers
        ( *iLeftovers ) = ( *iBuffer );
        }
    //reissue request
    ReceiveMessage( toContinue );
    }

void CHtiCommAdapter::HandleReceiveContL()
    {
    HTI_LOG_FORMAT( "In buffer: %d", iBuffer->Length() );
    TBool toContinue = ETrue;

    if ( iMsgToReceive != NULL && iSkipLength == 0 )
        {
        //add new packet to a message
        TInt copyLen = iMsgToReceive->AddToBody( *iBuffer );

        //delete what we copied to the message
        iBuffer->Des().Delete( 0, copyLen );

        if ( iMsgToReceive->IsBodyComplete() )
            {
            HTI_LOG_FORMAT( "body complete: %d", iMsgToReceive->BodySize() );

            iDispatcher->DispatchIncomingMessage( iMsgToReceive );
            iMsgToReceive = NULL;

            //message complete, go to state EReceive
            toContinue = EFalse;
            }
        }
    else if ( iSkipLength > 0 )
        {
        HTI_LOG_FORMAT( "continue skipping, remains %d", iSkipLength );

        if ( iSkipLength > iBuffer->Length() )
            {
            iSkipLength -= iBuffer->Length();
            iBuffer->Des().Zero();
            }
        else
            {
            //skip last bytes
            iBuffer->Des().Delete( 0, iSkipLength );
            iSkipLength = 0;

            toContinue = EFalse;
            }
        }

    if ( iBuffer->Length() > 0 )
        {
        //process remainder and reissue in HandleReceiveL()
        HandleReceiveL();
        }
    else
        {
        //reissue
        ReceiveMessage(toContinue);
        }
    }

void CHtiCommAdapter::HandleSend()
    {
    if ( iMsgToSendOffset < iMsgToSend->BodySize() )
        {
        //reissuse sending
        SendMessage( NULL );
        }
    else
        {
        HTI_LOG_TEXT( "finish send, go to idle" );
        iMsgToSendOffset = 0;
        delete iMsgToSend;
        iMsgToSend = NULL;
        iState = EIdle;
        iDispatcher->Notify( KErrNone );
        }
    }

TInt CHtiCommAdapter::RunError( TInt aError )
    {
    TInt err = aError;
    switch ( aError )
        {
        case KErrNoMemory:
            break;
        case KErrNotFound:
            break;
        }
    return err;
    }

void CHtiCommAdapter::DoCancel()
    {
    switch ( iState )
        {
        case EReceiving:
        case EReceivingCont:
            iCommPlugin->CancelReceive();
            break;
        case ESending:
            iCommPlugin->CancelSend();
            break;
        default:
            break;
        }
    }
