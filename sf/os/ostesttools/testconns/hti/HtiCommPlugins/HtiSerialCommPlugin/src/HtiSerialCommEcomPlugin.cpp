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

#include "HtiSerialCommEcomPlugin.h"
#include <HtiCfg.h>
#include <HtiLogging.h>

// CONSTANTS
_LIT( KHtiSerialError, "HtiSerialError" );
_LIT( KHtiOkButton, "OK" );

_LIT( KHtiCfgPath,          "\\" ); // root of drive
_LIT( KHtiSerialCommCfg,    "HTISerialComm.cfg" );
_LIT8( KCommPortNumber,     "CommPort"         );
_LIT8( KDataRate,           "DataRate"         );
_LIT8( KParity,             "Parity"           );
_LIT8( KDataBits,           "DataBits"         );
_LIT8( KStopBits,           "StopBits"         );
_LIT8( KSendDelay,          "SendDelay"        );
_LIT8( KHandshake,          "Handshake"        );

#if defined (__WINS__)
_LIT( SERIAL_PDD_NAME, "ECDRV" );
#else
_LIT( SERIAL_PDD_NAME, "EUART1" );
#endif
_LIT( SERIAL_LDD_NAME, "ECOMM" );
_LIT( RS232_CSY_NAME, "ECUART" );

const static TUint KReceiveBufferLength = 4*1024;
const static TUint KSendBufferLength = 4*1024;
const static TInt KMaxCommPortNumber = 99;
const static TInt KMaxHtiNotifierLength = 128;

CHtiSerialCommEcomPlugin* CHtiSerialCommEcomPlugin::NewL()
    {
    HTI_LOG_FUNC_IN( "Serial::NewL" );
    CHtiSerialCommEcomPlugin* plugin = new (ELeave) CHtiSerialCommEcomPlugin();
    CleanupStack::PushL( plugin );
    plugin->ConstructL();
    CleanupStack::Pop( plugin );
    HTI_LOG_FUNC_OUT( "Serial::NewL" );
    return plugin;
    }

CHtiSerialCommEcomPlugin::~CHtiSerialCommEcomPlugin()
    {
    HTI_LOG_FUNC_IN( "Serial::~CHtiSerialCommEcomPlugin" );
    iCommPort.Close();
    iCommServ.Close();
    delete iCfg;
    User::FreeLogicalDevice( SERIAL_LDD_NAME );
    User::FreePhysicalDevice( SERIAL_PDD_NAME );
    HTI_LOG_FUNC_OUT( "Serial::~CHtiSerialCommEcomPlugin" );
    }

CHtiSerialCommEcomPlugin::CHtiSerialCommEcomPlugin()
    {
    // default port settings
    iPortNumber = 0;
    iDataRate   = EBps115200;
    iParity     = EParityNone;
    iDataBits   = EData8;
    iStopBits   = EStop1;

    iSendDelay = 0;
    iHandshake = 0;
    }

void CHtiSerialCommEcomPlugin::ConstructL()
    {
    TRAPD( err, LoadConfigL() );
    if ( err == KErrNone )
        {
        ReadConfig();
        }
    InitCommServerL();
    InitCommPortL();
    }

void CHtiSerialCommEcomPlugin::LoadConfigL()
    {
    HTI_LOG_FUNC_IN( "Serial::LoadConfigL" );
    iCfg = CHtiCfg::NewL();
    HTI_LOG_TEXT( "CHtiCfg constructed - loading cfg file" );
    iCfg->LoadCfgL( KHtiCfgPath, KHtiSerialCommCfg );
    HTI_LOG_TEXT( "Cfg file loaded" );
    HTI_LOG_FUNC_OUT( "Serial::LoadConfigL" );
    }

void CHtiSerialCommEcomPlugin::ReadConfig()
    {
    HTI_LOG_FUNC_IN( "Serial::ReadConfig" );

    TInt portNumberCfg = 0;
    TRAPD( paramErr,
            portNumberCfg = iCfg->GetParameterIntL( KCommPortNumber ) );
    if ( paramErr != KErrNone )
        {
        HTI_LOG_FORMAT(
            "ComPort not defined in cfg, using default value %d",
            iPortNumber );
        portNumberCfg = iPortNumber;
        }
    if ( portNumberCfg > KMaxCommPortNumber || portNumberCfg < 0 )
        {
        HTI_LOG_FORMAT( "Unsupported ComPort %d defined in cfg, using default",
                            portNumberCfg );
        portNumberCfg = iPortNumber;
        }
    iPortNumber = portNumberCfg;

    TInt dataRateCfg = 0;
    TRAP( paramErr, dataRateCfg = iCfg->GetParameterIntL( KDataRate ) );
    if ( paramErr != KErrNone )
        {
        HTI_LOG_TEXT( "DataRate not defined in cfg, using default" );
        }
    else
        {
        switch ( dataRateCfg )
            {
            case 2400:
                iDataRate = EBps2400;
                break;
            case 4800:
                iDataRate = EBps4800;
                break;
            case 9600:
                iDataRate = EBps9600;
                break;
            case 19200:
                iDataRate = EBps19200;
                break;
            case 38400:
                iDataRate = EBps38400;
                break;
            case 57600:
                iDataRate = EBps57600;
                break;
            case 115200:
                iDataRate = EBps115200;
                break;
            case 576000:
                iDataRate = EBps576000;
                break;
            case 1152000:
                iDataRate = EBps1152000;
                break;
            case 4000000:
                iDataRate = EBps4000000;
                break;
            default:
                HTI_LOG_FORMAT(
                    "Unsupported DataRate %d defined - using default",
                    dataRateCfg );
                break;
            }
        }

    TBuf8<64> parityCfg( KNullDesC8 );
    TRAP( paramErr, parityCfg = iCfg->GetParameterL( KParity ) );
    if ( paramErr != KErrNone || parityCfg.Length() == 0 )
        {
        HTI_LOG_TEXT( "Parity not defined in cfg, using default" );
        }
    else
        {
        if ( !parityCfg.CompareF( _L8( "none" ) ) )
            {
            iParity = EParityNone;
            }
        else if ( !parityCfg.CompareF( _L8( "even" ) ) )
            {
            iParity = EParityEven;
            }
        else if ( !parityCfg.CompareF( _L8( "odd" ) ) )
            {
            iParity = EParityOdd;
            }
        else if ( !parityCfg.CompareF( _L8( "mark" ) ) )
            {
            iParity = EParityMark;
            }
        else if ( !parityCfg.CompareF( _L8( "space" ) ) )
            {
            iParity = EParitySpace;
            }
        else
            {
            HTI_LOG_TEXT( "Unsupported Parity defined - using default" );
            }
        }

    TInt dataBitsCfg = 0;
    TRAP( paramErr, dataBitsCfg = iCfg->GetParameterIntL( KDataBits ) );
    if ( paramErr != KErrNone )
        {
        HTI_LOG_TEXT( "DataBits not defined in cfg, using default value" );
        }
    else
        {
        switch ( dataBitsCfg )
            {
            case 5:
                iDataBits = EData5;
                break;
            case 6:
                iDataBits = EData6;
                break;
            case 7:
                iDataBits = EData7;
                break;
            case 8:
                iDataBits = EData8;
                break;
            default:
                HTI_LOG_FORMAT(
                    "Unsupported DataBits %d defined - using default",
                    dataBitsCfg );
                break;
            }
        }

    TInt stopBitsCfg = 0;
    TRAP( paramErr, stopBitsCfg = iCfg->GetParameterIntL( KStopBits ) );
    if ( paramErr != KErrNone )
        {
        HTI_LOG_TEXT( "StopBits not defined in cfg, using default value" );
        }
    else
        {
        switch ( stopBitsCfg )
            {
            case 1:
                iStopBits = EStop1;
                break;
            case 2:
                iStopBits = EStop2;
                break;
            default:
                HTI_LOG_FORMAT(
                    "Unsupported StopBits %d defined - using default",
                    stopBitsCfg );
                break;
            }
        }

    TInt sendDelayCfg = 0;
    TRAP( paramErr, sendDelayCfg = iCfg->GetParameterIntL( KSendDelay ) );
    if ( paramErr != KErrNone )
        {
        HTI_LOG_TEXT( "SendDelay not defined in cfg, using default value" );
        }
    else
        {
        iSendDelay = sendDelayCfg * 1000; // configured as milliseconds
        }

    TInt handshakeCfg = 0;
    TRAP( paramErr, handshakeCfg = iCfg->GetParameterIntL( KHandshake ) );
    if ( paramErr != KErrNone )
        {
        HTI_LOG_TEXT(
            "Handshake not defined in cfg, using default value" );
        }
    else
        {
        iHandshake = handshakeCfg;
        }

    HTI_LOG_FUNC_OUT( "Serial::ReadConfig" );
    }

void CHtiSerialCommEcomPlugin::InitCommServerL()
    {
    HTI_LOG_FUNC_IN( "Serial::InitCommServerL" );
    //load physical device driver
    TInt err = User::LoadPhysicalDevice( SERIAL_PDD_NAME );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        ShowErrorNotifierL( _L( "Failed to load serial PDD" ), err );
        User::Leave( err );
        }

    //load logical device driver
    err = User::LoadLogicalDevice( SERIAL_LDD_NAME );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        ShowErrorNotifierL( _L( "Failed to load serial LDD" ), err );
        User::Leave( err );
        }

#if !defined (__WINS__)
    //User::After(1500000); //resolve
    //starts the comm server
    err = StartC32();
    if ( err != KErrNone && err != KErrAlreadyExists )
        User::Leave( err );
#endif

    //connect to RCommServ
    User::LeaveIfError( iCommServ.Connect() );

    //load protocol module (CSY file)
    User::LeaveIfError( iCommServ.LoadCommModule( RS232_CSY_NAME ) );
    HTI_LOG_FUNC_OUT( "Serial::InitCommServerL" );
    }

void CHtiSerialCommEcomPlugin::InitCommPortL()
    {
    HTI_LOG_FUNC_IN( "Serial::InitCommPortL" );
    TInt numPorts;
    TInt err;
    err = iCommServ.NumPorts( numPorts );
    User::LeaveIfError( err );
    HTI_LOG_FORMAT( "NumPorts: %d", numPorts );

    // we can get port information for each loaded CSY in turn (note we
    // index them from 0) - we can find out the number of ports supported
    // together with their names, and their description. The information is
    // returned in a TSerialInfo structure together with the name of the
    // CSY that we've indexed

    TSerialInfo portInfo;
    TBuf16<12> moduleName;

    for ( TInt index=0 ; index < numPorts ; index++ )
        {
        err = iCommServ.GetPortInfo( index, moduleName, portInfo );
        User::LeaveIfError( err );
        HTI_LOG_FORMAT( "Port %d", index );
        HTI_LOG_DES( moduleName );
        HTI_LOG_DES( portInfo.iDescription );
        HTI_LOG_DES( portInfo.iName );
        HTI_LOG_FORMAT( "Low unit %d", portInfo.iLowUnit );
        HTI_LOG_FORMAT( "High unit %d", portInfo.iHighUnit );
        }

    TBuf<8> commPort;
    commPort.AppendFormat( _L( "COMM::%d" ), iPortNumber );
    HTI_LOG_FORMAT( "Open serial port COMM::%d", iPortNumber );
    err = iCommPort.Open( iCommServ, commPort, ECommExclusive );
    if ( err )
        {
        HTI_LOG_FORMAT( "Failed to open serial port %d", err );
        ShowErrorNotifierL( _L( "Failed to open serial port" ), err );
        }
    User::LeaveIfError( err );

    // Just for debug purposes log the port capabilities
    TCommCaps portCapabilities;
    iCommPort.Caps( portCapabilities );
    HTI_LOG_TEXT( "Port capabilities:" );
    HTI_LOG_FORMAT( " DataRate  = %d", portCapabilities().iRate );
    HTI_LOG_FORMAT( " Parity    = %d", portCapabilities().iParity );
    HTI_LOG_FORMAT( " DataBits  = %d", portCapabilities().iDataBits );
    HTI_LOG_FORMAT( " StopBits  = %d", portCapabilities().iStopBits );
    HTI_LOG_FORMAT( " Handshake = %d", portCapabilities().iHandshake );
    HTI_LOG_FORMAT( " Signals   = %d", portCapabilities().iSignals );
    HTI_LOG_FORMAT( " Fifo      = %d", portCapabilities().iFifo );
    HTI_LOG_FORMAT( " SIR       = %d", portCapabilities().iSIR );

    // Set port settings
    TCommConfig portSettings;
    iCommPort.Config( portSettings );
    portSettings().iRate      = iDataRate;
    portSettings().iParity    = iParity;
    portSettings().iDataBits  = iDataBits;
    portSettings().iStopBits  = iStopBits;
    portSettings().iFifo      = EFifoEnable;
    portSettings().iHandshake = iHandshake;

    HTI_LOG_TEXT( "Port settings enum values:" );
    HTI_LOG_FORMAT( " DataRate  %d", portSettings().iRate );
    HTI_LOG_FORMAT( " Parity    %d", portSettings().iParity );
    HTI_LOG_FORMAT( " DataBits  %d", portSettings().iDataBits );
    HTI_LOG_FORMAT( " StopBits  %d", portSettings().iStopBits );
    HTI_LOG_FORMAT( " Handshake 0x%x",  portSettings().iHandshake );
    HTI_LOG_FORMAT( " Fifo      %d", portSettings().iFifo );

    // Now activate the settings
    err = iCommPort.SetConfig( portSettings );
    if ( err )
        {
        HTI_LOG_FORMAT( "failed to set port settings %d", err );
        ShowErrorNotifierL( _L( "Failed to set port settings" ), err );
        }
    User::LeaveIfError( err );

    // Turn on DTR and RTS, and set our buffer size
    iCommPort.SetSignals( KSignalDTR, 0 );
    iCommPort.SetSignals( KSignalRTS, 0 );

    iCommPort.SetReceiveBufferLength( KReceiveBufferLength );

    // A null read or write powers up the port
    TRequestStatus readStat;
    TBuf8<1> temp_buffer;
    iCommPort.Read( readStat, temp_buffer, 0 );
    User::WaitForRequest( readStat );
    err = readStat.Int();
    if ( err )
        {
        HTI_LOG_FORMAT( "Failed to power up the port %d", err );
        ShowErrorNotifierL( _L( "Failed to power up the port" ), err );
        }
    User::LeaveIfError( err );

    HTI_LOG_FUNC_OUT( "Serial::InitCommPortL" );
    }

void CHtiSerialCommEcomPlugin::Receive( TDes8& aRawdataBuf,
                                        TRequestStatus& aStatus )
    {
    HTI_LOG_FUNC_IN( "Serial::Receive" );
    HTI_LOG_FORMAT( "Buf max len: %d", aRawdataBuf.MaxLength() );
    iCommPort.ReadOneOrMore( aStatus, aRawdataBuf );
    HTI_LOG_FUNC_OUT( "Serial::Receive" );
    }

void CHtiSerialCommEcomPlugin::Send( const TDesC8& aRawdataBuf,
                                           TRequestStatus& aStatus )
    {
    HTI_LOG_FUNC_IN( "Serial::Send" );
    iCommPort.Write( aStatus, aRawdataBuf );
    if ( iSendDelay > 0 )  // optional wait - can be set in serial comm cfg
        {
        // If sending large amounts of data from Symbian to PC is failing,
        // the wait here seems to help.
        User::After( iSendDelay );
        }
    HTI_LOG_FUNC_OUT( "Serial::Send" );
    }

void CHtiSerialCommEcomPlugin::CancelReceive()
    {
    HTI_LOG_FUNC_IN( "Serial::CancelReceive" );
    iCommPort.ReadCancel();
    HTI_LOG_FUNC_OUT( "Serial::CancelReceive" );
    }

void CHtiSerialCommEcomPlugin::CancelSend()
    {
    HTI_LOG_FUNC_IN( "Serial::CancelSend" );
    iCommPort.WriteCancel();
    HTI_LOG_FUNC_OUT( "Serial::CancelSend" );
    }

TInt CHtiSerialCommEcomPlugin::GetSendBufferSize()
    {
    return KSendBufferLength;
    }

TInt CHtiSerialCommEcomPlugin::GetReceiveBufferSize()
    {
    return KReceiveBufferLength;
    }

void CHtiSerialCommEcomPlugin::ShowErrorNotifierL( const TDesC& aText,
                                                   TInt aErr )
    {
    RNotifier notifier;
    User::LeaveIfError( notifier.Connect() );

    TBuf<KMaxHtiNotifierLength> errorMsg;
    // aText is cut if it's too long - leaving some space also for error code
    errorMsg.Append( aText.Left( errorMsg.MaxLength() - 10 ) );
    errorMsg.Append( _L("\n") );
    errorMsg.AppendNum( aErr );

    TRequestStatus status;
    TInt button;
    notifier.Notify( KHtiSerialError, errorMsg,
                     KHtiOkButton, KNullDesC, button, status );
    User::WaitForRequest( status );
    notifier.Close();
    }
