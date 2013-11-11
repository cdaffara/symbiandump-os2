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
* Description:  ECOM plugin for communication over serial port
*
*/


// INCLUDE FILES
#include <badesca.h>
#include <f32file.h>

#include "HtiBtCommEcomPlugin.h"
#include "BtSerialClient.h"
#include <HtiCfg.h>
#include <HtiLogging.h>

// CONSTANTS
_LIT( KHtiCfgPath,       "\\" ); // root of drive
_LIT( KHtiBtCommCfg,     "HTIBtComm.cfg" );
_LIT8( KBtDeviceAddress, "BtDeviceAddress" );
_LIT8( KBtDeviceName,    "BtDeviceName" );


const TInt KBtAddressHexStringLength = 12; // 6 bytes
const TUint KPortSeparatorChar = 30; // ASCII control char "record separator"

// ----------------------------------------------------------------------------
CHtiBtCommEcomPlugin* CHtiBtCommEcomPlugin::NewL()
    {
    HTI_LOG_FUNC_IN( "CHtiBtCommEcomPlugin::NewL" );
    CHtiBtCommEcomPlugin* plugin = new (ELeave) CHtiBtCommEcomPlugin();
    CleanupStack::PushL( plugin );
    plugin->ConstructL();
    CleanupStack::Pop( plugin );
    HTI_LOG_FUNC_OUT( "CHtiBtCommEcomPlugin::NewL" );
    return plugin;
    }

// ----------------------------------------------------------------------------
CHtiBtCommEcomPlugin::~CHtiBtCommEcomPlugin()
    {
    HTI_LOG_FUNC_IN( "CHtiBtCommEcomPlugin::~CHtiBtCommEcomPlugin" );
    iBtCommInterface.CancelReceive();
    iBtCommInterface.CancelSend();
    iBtCommInterface.Close();
    delete iBtDeviceNameOrAddress;
    delete iCfg;
    HTI_LOG_FUNC_OUT( "CHtiBtCommEcomPlugin::~CHtiBtCommEcomPlugin" );
    }

// ----------------------------------------------------------------------------
CHtiBtCommEcomPlugin::CHtiBtCommEcomPlugin():iPort( -1 )
    {
    }

// ----------------------------------------------------------------------------
void CHtiBtCommEcomPlugin::ConstructL()
    {
    TRAPD( err, LoadConfigL() );
    if ( err == KErrNone )
        {
        ReadConfig();
        }
    else
        {
        TBufC8<1> emptyBuf( KNullDesC8 );
        iBtDeviceNameOrAddress = emptyBuf.AllocL();
        iPort = KErrNotFound;
        }
     // May take long!
    User::LeaveIfError( iBtCommInterface.Connect(
            *iBtDeviceNameOrAddress, iPort ) );

    // Get the connected service port number (channel) and store it if needed
    TInt connectedPort = iBtCommInterface.GetPortNumber();
    HTI_LOG_FORMAT( "Connected port number = %d", connectedPort );
    if ( connectedPort != iPort && connectedPort >= 0 )
        {
        TRAP( err, StorePortNumberL( connectedPort ) ); // ignore error
        }
    }

// ----------------------------------------------------------------------------
void CHtiBtCommEcomPlugin::Receive( TDes8& aRawdataBuf,
                                    TRequestStatus& aStatus )
    {
    iBtCommInterface.Receive( aRawdataBuf, aStatus );
    }

// ----------------------------------------------------------------------------
void CHtiBtCommEcomPlugin::Send( const TDesC8& aRawdataBuf,
                                       TRequestStatus& aStatus )
    {
    iBtCommInterface.Send( aRawdataBuf, aStatus );
    }

// ----------------------------------------------------------------------------
void CHtiBtCommEcomPlugin::CancelReceive()
    {
    iBtCommInterface.CancelReceive();
    }

// ----------------------------------------------------------------------------
void CHtiBtCommEcomPlugin::CancelSend()
    {
    iBtCommInterface.CancelSend();
    }

// ----------------------------------------------------------------------------
TInt CHtiBtCommEcomPlugin::GetSendBufferSize()
    {
    return iBtCommInterface.GetSendBufferSize();
    }

// ----------------------------------------------------------------------------
TInt CHtiBtCommEcomPlugin::GetReceiveBufferSize()
    {
    return iBtCommInterface.GetReceiveBufferSize();
    }

// ----------------------------------------------------------------------------
void CHtiBtCommEcomPlugin::LoadConfigL()
    {
    HTI_LOG_FUNC_IN( "CHtiBtCommEcomPlugin::LoadConfigL" );
    iCfg = CHtiCfg::NewL();
    iCfg->LoadCfgL( KHtiCfgPath, KHtiBtCommCfg );
    HTI_LOG_TEXT( "cfg file loaded" );
    HTI_LOG_FUNC_OUT( "CHtiBtCommEcomPlugin::LoadConfigL" );
    }

// ----------------------------------------------------------------------------
void CHtiBtCommEcomPlugin::ReadConfig()
    {
    HTI_LOG_FUNC_IN( "CHtiBtCommEcomPlugin::ReadConfig" );
    // First try to get address - if found and length is correct it is used
    TRAPD( paramErr, iBtDeviceNameOrAddress = iCfg->GetParameterL(
                                                KBtDeviceAddress ).AllocL() );

    if ( paramErr == KErrNone )
        {
        TInt port = ParsePortNumber();
        if ( port >= 0 ) iPort = port;
        else iPort = KErrNotFound;
        }

    if ( paramErr ||
         iBtDeviceNameOrAddress->Length() != KBtAddressHexStringLength )
        {
        HTI_LOG_TEXT( "BT address not defined or not valid - getting name" );
        delete iBtDeviceNameOrAddress;
        iBtDeviceNameOrAddress = NULL;
        // Address was not found - try to get name.
        TRAP( paramErr, iBtDeviceNameOrAddress = iCfg->GetParameterL(
                                                 KBtDeviceName ).AllocL() );
        if ( paramErr )
            {
            HTI_LOG_TEXT( "BT name not defined either" );
            TBufC8<1> emptyBuf( KNullDesC8 );
            iBtDeviceNameOrAddress = emptyBuf.AllocL();
            iPort = KErrNotFound;
            }
        else
            {
            TInt port = ParsePortNumber();
            if ( port >= 0 ) iPort = port;
            else iPort = KErrNotFound;
            }
        }
    HTI_LOG_TEXT( "BT device name or address:" );
    HTI_LOG_DES( *iBtDeviceNameOrAddress );
    HTI_LOG_FORMAT( "Port number: %d", iPort );
    HTI_LOG_FUNC_OUT( "CHtiBtCommEcomPlugin::ReadConfig" );
    }

// ----------------------------------------------------------------------------
TInt CHtiBtCommEcomPlugin::ParsePortNumber()
    {
    HTI_LOG_FUNC_IN( "CHtiBtCommEcomPlugin::ParsePortNumber" );
    TInt port = KErrNotFound;
    TInt separatorIndex = iBtDeviceNameOrAddress->LocateReverse(
            TChar( KPortSeparatorChar ) );
    if ( separatorIndex != KErrNotFound )
        {
        TPtrC8 value = iBtDeviceNameOrAddress->Right(
                iBtDeviceNameOrAddress->Length() - ( separatorIndex + 1 ) );
        TLex8 lex( value );
        lex.Val( port );
        // Strip away the port number
        iBtDeviceNameOrAddress->Des().SetLength( separatorIndex );
        }
    HTI_LOG_FORMAT( "ParsePortNumber returning %d", port );
    HTI_LOG_FUNC_OUT( "CHtiBtCommEcomPlugin::ParsePortNumber" );
    return port;
    }

// ----------------------------------------------------------------------------
void CHtiBtCommEcomPlugin::StorePortNumberL( TInt aPortNumber )
    {
    HTI_LOG_FUNC_IN( "CHtiBtCommEcomPlugin::StorePortNumberL" );
    if ( !iCfg )
        {
        iCfg = CHtiCfg::NewL();
        }
    iCfg->LoadCfgL( KHtiCfgPath, KHtiBtCommCfg );

    TBool changesMade = EFalse;

    // Add or replace port number to address parameter if it exists
    HBufC8* value = NULL;
    TRAPD( err, value = iCfg->GetParameterL( KBtDeviceAddress ).AllocL() );
    if ( !err )
        {
        HTI_LOG_FORMAT( "Found address param (length = %d)", value->Length() );
        CleanupStack::PushL( value );
        TInt separatorIndex = value->Locate( TChar( KPortSeparatorChar ) );
        if ( separatorIndex != KErrNotFound )
            {
            HTI_LOG_FORMAT( "Separator char found from index %d", separatorIndex );
            value->Des().SetLength( separatorIndex );
            }
        value = value->ReAllocL( value->Length() + 5 );
        // The old value has been deleted by ReAllocL and new allocated,
        // so update cleanup stack.
        CleanupStack::Pop();
        CleanupStack::PushL( value );
        TPtr8 ptr = value->Des();
        ptr.Append( TChar( KPortSeparatorChar ) );
        ptr.AppendNum( aPortNumber );
        TRAP( err, iCfg->SetParameterL( KBtDeviceAddress, *value ) );
        if ( !err ) changesMade = ETrue;
        CleanupStack::PopAndDestroy(); // value
        }

    // Add or replace port number to name parameter if it exists
    TRAP( err, value = iCfg->GetParameterL( KBtDeviceName ).AllocL() );
    if ( !err )
        {
        HTI_LOG_FORMAT( "Found name param (length = %d)", value->Length() );
        CleanupStack::PushL( value );
        TInt separatorIndex = value->Locate( TChar( KPortSeparatorChar ) );
        if ( separatorIndex != KErrNotFound )
            {
            HTI_LOG_FORMAT( "Separator char found from index %d", separatorIndex );
            value->Des().SetLength( separatorIndex );
            }
        value = value->ReAllocL( value->Length() + 5 );
        // The old value has been deleted by ReAllocL and new allocated,
        // so update cleanup stack.
        CleanupStack::Pop();
        CleanupStack::PushL( value );
        TPtr8 ptr = value->Des();
        ptr.Append( TChar( KPortSeparatorChar ) );
        ptr.AppendNum( aPortNumber );
        TRAP( err, iCfg->SetParameterL( KBtDeviceName, *value ) );
        if ( !err ) changesMade = ETrue;
        CleanupStack::PopAndDestroy(); // value
        }

    if ( changesMade )
        {
        iCfg->SaveCfgL( KHtiCfgPath, KHtiBtCommCfg );
        }
    HTI_LOG_FUNC_OUT( "CHtiBtCommEcomPlugin::StorePortNumberL" );
    }

// End of file
