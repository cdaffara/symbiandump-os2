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
* Description: Implementation of RHtiBtCommInterface class.
*              Symbian side component uses this interface to receive and
*              send HTI data to HtiBtCommServer.
*              HtiBtCommServer acts as proxy to send and receive data
*              to and from PC.
*
*/


// INCLUDE FILES
#include "HtiBtClientServerCommon.h"
#include "HtiBtCommInterface.h"

#include <e32uid.h>
#include <HtiLogging.h>

// CONSTANTS
const TUint KBtCommServerDefaultMessageSlots = 4; // Read, write, readCancel, writeCancel

// Constants

/*---------------------------------------------------------------------------*/
EXPORT_C RHtiBtCommInterface::RHtiBtCommInterface()
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::RHtiBtCommInterface()" );
    }

/*---------------------------------------------------------------------------*/
EXPORT_C TInt RHtiBtCommInterface::Connect( TDesC8& aDeviceNameOrAddress,
                                            TInt aPort )
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::Connect()" );

    TInt error = StartThread(); // see HtiBtClientServerCommon.h
    if (error == KErrNone)
        {
        error = CreateSession( KBtCommServerName,
                               Version(),
                               KBtCommServerDefaultMessageSlots );
        }
    if ( error )
        return error;

    return ConnectBt( aDeviceNameOrAddress, aPort );
    }

/*---------------------------------------------------------------------------*/
EXPORT_C void RHtiBtCommInterface::Close()
    {
    HTI_LOG_FUNC_IN( "RHtiBtCommInterface::Close" );

    // Use thread finder to find the server thread
    TFullName threadName;
    TFullName matchPattern;
    matchPattern.Append( _L( "*" ) );
    matchPattern.Append( KBtCommServerName );
    matchPattern.Append( _L( "*" ) );

    TFindThread threadFinder;
    threadFinder.Find( matchPattern );
    TInt err = threadFinder.Next( threadName );
    if ( err )
        {
        HTI_LOG_FORMAT( "RHtiBtCommInterface::Close: Failed to find server thread: %d", err );
        User::Panic( _L( "BtIf Close, server thread not found" ), err );
        }

    HTI_LOG_TEXT( "RHtiBtCommInterface::Close: Found server thread:" );
    HTI_LOG_DES( threadName );

    RThread thread;
    err = thread.Open( threadName );
    if ( err )
        {
        HTI_LOG_FORMAT( "RHtiBtCommInterface::Close: Failed to open server thread: %d", err );
        User::Panic( _L( "BtIf Close, error opening server thread" ), err );
        }
    else
        {
        // initiate server shutdown
        RSessionBase::Close();

        // For clean server stop, wait for its death
        HTI_LOG_TEXT( "RHtiBtCommInterface::Close: Waiting for server thread to die..." );
        TRequestStatus status;
        thread.Logon( status );
        User::WaitForRequest( status );
        HTI_LOG_TEXT( "RHtiBtCommInterface::Close: Server thread dead." );
        }

    thread.Close();
    HTI_LOG_FUNC_OUT( "RHtiBtCommInterface::Close" );
    }

/*---------------------------------------------------------------------------*/
EXPORT_C TVersion RHtiBtCommInterface::Version(void) const
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::Connect()" );
    return ( TVersion( KBtCommServerMajorVersionNumber,
                       KBtCommServerMinorVersionNumber,
                       KBtCommServerBuildVersionNumber ) );

    }

/*---------------------------------------------------------------------------*/
EXPORT_C TInt RHtiBtCommInterface::GetSendBufferSize() const
    {
    return KClientSendBufferMaxSize;
    }

/*---------------------------------------------------------------------------*/
EXPORT_C TInt RHtiBtCommInterface::GetReceiveBufferSize() const
    {
    return KClientReceiveBufferMaxSize;
    }

/*---------------------------------------------------------------------------*/
EXPORT_C void RHtiBtCommInterface::Receive(TDes8& aData,
                                        TRequestStatus& aStatus)
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::Receive()" );
    SendReceive( EBtCommServerRecv, TIpcArgs( &aData ), aStatus );
    }

/*---------------------------------------------------------------------------*/
EXPORT_C void RHtiBtCommInterface::Send(const TDesC8& aData,
                                        TRequestStatus& aStatus)
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::Send()" );
    SendReceive( EBtCommServerSend, TIpcArgs( &aData ), aStatus );
    }

/*---------------------------------------------------------------------------*/
EXPORT_C void RHtiBtCommInterface::CancelReceive()
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::CancelReceive()" );
    SendReceive( ECancelBtCommServerRecv, TIpcArgs( NULL ) );
    }

/*---------------------------------------------------------------------------*/
EXPORT_C void RHtiBtCommInterface::CancelSend()
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::CancelSend()" );
    SendReceive( ECancelBtCommServerSend, TIpcArgs( NULL ) );
    }

/*---------------------------------------------------------------------------*/
EXPORT_C TInt RHtiBtCommInterface::GetPortNumber() const
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::GetPortNumber()" );
    TPckgBuf<TInt> pckg;
    TInt ret = SendReceive( EGetServicePortNumber, TIpcArgs( &pckg ) );
    if ( ret == KErrNone )
        {
        ret = pckg();
        }
    return ret;
    }

/*---------------------------------------------------------------------------*/
TInt RHtiBtCommInterface::ConnectBt( TDesC8& aDeviceNameOrAddress, TInt aPort )
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::ConnectBt()" );
    return SendReceive( EBtCommServerConnect,
                            TIpcArgs( &aDeviceNameOrAddress, aPort ) );
    }

/*---------------------------------------------------------------------------*/
GLDEF_C TInt E32Main()
    {
    HTI_LOG_TEXT( "RHtiBtCommInterface::E32Main()" );
    return KErrNone;
    }

// End of the file
