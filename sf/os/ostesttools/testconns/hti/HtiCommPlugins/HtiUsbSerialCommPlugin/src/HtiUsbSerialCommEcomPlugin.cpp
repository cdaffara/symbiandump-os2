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
* Description:  ECOM plugin for serial communication over USB port
*
*/


// INCLUDE FILES
#include <badesca.h>
#include <f32file.h>

#include "HtiUsbSerialCommEcomPlugin.h"
#include <HtiCfg.h>
#include <HtiLogging.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT( KHtiUsbSerialError, "HtiUsbSerialError" );
_LIT( KHtiOkButton, "OK" );

_LIT( KHtiCfgPath,          "\\" ); // root of drive
_LIT( KHtiUsbSerialCommCfg, "HTIUsbSerialComm.cfg" );
_LIT8( KUsbPortNumber,      "PortNumber" );
_LIT8( KUsbDataRate,        "DataRate" );
_LIT8( KUsbRetryTimes,      "RetryTimes" );
_LIT8( KUsbRetryInterval,   "RetryInterval" );

// _LIT( KUsbPddName, "" );
_LIT( KUsbLddName, "EUSBC" );
_LIT( KUsbCsyName, "ECACM");

const TInt KDefaultUsbPort = 1;

const static TUint KReceiveBufferLength = 4 * 1024;
const static TUint KSendBufferLength =    4 * 1024;

const static TInt KMaxHtiNotifierLength = 128;

// MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHtiUsbSerialCommEcomPlugin* CHtiUsbSerialCommEcomPlugin::NewL()
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::NewL" );
    CHtiUsbSerialCommEcomPlugin* plugin =
        new ( ELeave ) CHtiUsbSerialCommEcomPlugin();
    CleanupStack::PushL( plugin );
    plugin->ConstructL();
    CleanupStack::Pop( plugin );
    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::NewL" );
    return plugin;
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::~CHtiUsbSerialCommEcomPlugin
// Destructor.
// -----------------------------------------------------------------------------
CHtiUsbSerialCommEcomPlugin::~CHtiUsbSerialCommEcomPlugin()
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::~CHtiUsbSerialCommEcomPlugin" );
    iCommPort.Close();
    iCommServ.Close();
    delete iCfg;
    User::FreeLogicalDevice( KUsbLddName() );
    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::~CHtiUsbSerialCommEcomPlugin" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::CHtiUsbSerialCommEcomPlugin
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CHtiUsbSerialCommEcomPlugin::CHtiUsbSerialCommEcomPlugin()
    {
    // default port settings
    iPortNumber = KDefaultUsbPort;
    iDataRate   = EBps115200;
    iParity     = EParityNone;
    iDataBits   = EData8;
    iStopBits   = EStop1;
    iHandshake  = 0;
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::ConstructL
// Symbian 2nd phase constructor - can leave.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::ConstructL()
    {
    TRAPD( err, LoadConfigL() );
    if ( err == KErrNone )
        {
        ReadConfig();
        }
    InitCommServerL();
    InitCommPortL();
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::LoadConfigL
// Loads the plugin configuration file from disk to iCfg.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::LoadConfigL()
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::LoadConfigL" );
    iCfg = CHtiCfg::NewL();
    HTI_LOG_TEXT( "CHtiCfg constructed - loading cfg file" );
    iCfg->LoadCfgL( KHtiCfgPath, KHtiUsbSerialCommCfg );
    HTI_LOG_TEXT( "Cfg file loaded" );
    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::LoadConfigL" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::ReadConfig
// Reads the parameters from loaded configuration file.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::ReadConfig()
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::ReadConfig" );

    TInt portNumberCfg = 0;
    TRAPD( paramErr,
            portNumberCfg = iCfg->GetParameterIntL( KUsbPortNumber ) );
    if ( paramErr != KErrNone )
        {
        HTI_LOG_FORMAT(
            "PortNumber not defined in cfg, using default value %d",
            iPortNumber );
        portNumberCfg = iPortNumber;
        }
    iPortNumber = portNumberCfg;

    TInt dataRateCfg = 0;
    TRAP( paramErr, dataRateCfg = iCfg->GetParameterIntL( KUsbDataRate ) );
    if ( paramErr != KErrNone )
        {
        HTI_LOG_TEXT( "DataRate not defined in cfg, using default" );
        }
    else
        {
        switch ( dataRateCfg )
            {
            case 2400:    iDataRate = EBps2400;    break;
            case 4800:    iDataRate = EBps4800;    break;
            case 9600:    iDataRate = EBps9600;    break;
            case 19200:   iDataRate = EBps19200;   break;
            case 38400:   iDataRate = EBps38400;   break;
            case 57600:   iDataRate = EBps57600;   break;
            case 115200:  iDataRate = EBps115200;  break;
            case 576000:  iDataRate = EBps576000;  break;
            case 1152000: iDataRate = EBps1152000; break;
            case 4000000: iDataRate = EBps4000000; break;
            default:
                HTI_LOG_FORMAT(
                    "Unsupported DataRate %d defined - using default",
                    dataRateCfg );
                break;
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::ReadConfig" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::InitCommServerL
// Starts the comm server, loads comms module and device drivers.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::InitCommServerL()
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::InitCommServerL" );

    TInt err = KErrNone;

    // start the comm server
    err = StartC32();
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        ShowErrorNotifierL( _L( "Failed to start comm server" ), err );
        User::Leave( err );
        }

    // connect to RCommServ
    err = iCommServ.Connect();
    if ( err != KErrNone )
        {
        ShowErrorNotifierL( _L( "Failed to connect to comm server" ), err );
        User::Leave( err );
        }

    // load comms module (CSY file)
    err = iCommServ.LoadCommModule( KUsbCsyName() );
    if ( err != KErrNone )
        {
        ShowErrorNotifierL( _L( "Failed to load comms module" ), err );
        User::Leave( err );
        }

    /* USB PDD is usually a kernel extension so no need to load separately.
    // load physical device driver
    TInt err = User::LoadPhysicalDevice( KUsbPddName );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        ShowErrorNotifierL( _L( "Failed to load USB PDD" ), err );
        User::Leave( err );
        }
    */

    // load logical device driver
    err = User::LoadLogicalDevice( KUsbLddName() );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        ShowErrorNotifierL( _L( "Failed to load USB LDD" ), err );
        User::Leave( err );
        }

    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::InitCommServerL" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::InitCommPortL
// Checks that the comms module is valid and opens the port.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::InitCommPortL()
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::InitCommPortL" );

    TInt err = KErrNone;
    // check the number of loaded comms modules
    TInt commsCount = 0;
    err = iCommServ.NumPorts( commsCount );
    if ( err != KErrNone || commsCount < 1 )
        {
        if ( err != KErrNone )
            {
            ShowErrorNotifierL( _L( "Failed to get comms module count" ), err );
            User::Leave( err );
            }
        else
            {
            ShowErrorNotifierL( _L( "No comms module loaded" ), KErrNotFound );
            User::Leave( KErrNotFound );
            }
        }

    HTI_LOG_FORMAT( "Found %d loaded comms modules", commsCount );

    // get info about our loaded comms module
    TSerialInfo serialInfo;
    TBool found = EFalse;
    for ( TInt i = 0; i < commsCount && !found; i++ )
        {
        TBuf<32> moduleName;
        err = iCommServ.GetPortInfo( i, moduleName, serialInfo );
        if ( err != KErrNone )
            {
            ShowErrorNotifierL( _L( "Failed to get port info" ), err );
            User::Leave( err );
            }
        HTI_LOG_FORMAT( "Found comms module %S", &moduleName );
        HTI_LOG_FORMAT( "Comms module description: %S",
            &( serialInfo.iDescription ) );
        if ( moduleName.CompareF( KUsbCsyName ) == 0 )
            {
            found = ETrue;
            HTI_LOG_FORMAT( "Lowest port num  = %d", serialInfo.iLowUnit );
            HTI_LOG_FORMAT( "Highest port num = %d", serialInfo.iHighUnit );
            }
        }

    if ( !found )
        {
        ShowErrorNotifierL( _L( "Failed to get port info" ), KErrNotFound );
        User::Leave( KErrNotFound );
        }

    // create port name
    TBuf<KMaxPortName + 4> commPort;
    commPort.Append( serialInfo.iName );
    commPort.AppendFill( ':', 2 );
    commPort.AppendNum( iPortNumber );
    HTI_LOG_FORMAT( "Opening port %S", &commPort );

    // try to open the port
    err = iCommPort.Open( iCommServ, commPort, ECommExclusive, ECommRoleDTE );
    if ( err )
        {
        //read retry parameters from cfg
        TInt retryTimes = 10;
        TRAPD( paramRetryTimesErr,
                retryTimes = iCfg->GetParameterIntL( KUsbRetryTimes ) );
        if ( paramRetryTimesErr != KErrNone )
            {
            HTI_LOG_TEXT( "RetryTimes is not defined in cfg, using default" );
            }
        HTI_LOG_FORMAT( " RetryTimes  = %d", retryTimes );
        
        TInt retryInterval = 10;
        TRAPD( paramRetryIntervalErr,
                retryInterval = iCfg->GetParameterIntL( KUsbRetryInterval ) );
        if ( paramRetryIntervalErr != KErrNone )
            {
            HTI_LOG_TEXT( "RetryInterval is not defined in cfg, using default" );
            }
        HTI_LOG_FORMAT( " RetryInterval  = %d(s)", retryInterval );

        // retry to open the port
        for( TInt i=0; i<retryTimes; i++ )
            {
            User::After( retryInterval * 1000000 );
            err = iCommPort.Open( iCommServ, commPort, ECommExclusive, ECommRoleDTE );
            if( !err )
                {
                break;
                }
            }

        if( err )
            {
            HTI_LOG_FORMAT( "Failed to open port %d", err );
            ShowErrorNotifierL( _L( "Failed to open port" ), err );
            }
        }
    User::LeaveIfError( err );
    HTI_LOG_TEXT( "Port open - checking port capabilities" );

    // check port data rate capability
    TCommCaps portCaps;
    iCommPort.Caps( portCaps );
    HTI_LOG_TEXT( "Port capabilities:" );
    HTI_LOG_FORMAT( " DataRate  = %d", portCaps().iRate );
    HTI_LOG_FORMAT( " Parity    = %d", portCaps().iParity );
    HTI_LOG_FORMAT( " DataBits  = %d", portCaps().iDataBits );
    HTI_LOG_FORMAT( " StopBits  = %d", portCaps().iStopBits );
    HTI_LOG_FORMAT( " Handshake = %d", portCaps().iHandshake );
    HTI_LOG_FORMAT( " Signals   = %d", portCaps().iSignals );
    HTI_LOG_FORMAT( " Fifo      = %d", portCaps().iFifo );
    HTI_LOG_FORMAT( " SIR       = %d", portCaps().iSIR );

    TUint reqRateCapsBitmask = RateCapsBitmaskFromRate( iDataRate );
    HTI_LOG_FORMAT( "Required data rate capability bitmask %d",
        reqRateCapsBitmask );

    if ( reqRateCapsBitmask == 0 ||
         ( reqRateCapsBitmask & portCaps().iRate ) == 0 )
        {
        ShowErrorNotifierL( _L( "Unsupported data rate configured" ),
            KErrNotSupported );
        User::Leave( KErrNotSupported );
        }
    HTI_LOG_TEXT( "Data rate supported - setting port configuration" );

    // set port configuration
    TCommConfig portSettings;
    iCommPort.Config( portSettings );
    portSettings().iRate      = iDataRate;
    portSettings().iParity    = iParity;
    portSettings().iDataBits  = iDataBits;
    portSettings().iStopBits  = iStopBits;
    portSettings().iFifo      = EFifoEnable;
    portSettings().iHandshake = iHandshake;

    err = iCommPort.SetConfig( portSettings );
    if ( err )
        {
        HTI_LOG_FORMAT( "Failed to set port settings %d", err );
        ShowErrorNotifierL( _L( "Failed to set port settings" ), err );
        User::Leave( err );
        }

    iCommPort.SetReceiveBufferLength( KReceiveBufferLength );

    HTI_LOG_TEXT( "Port open and configured" );
    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::InitCommPortL" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::Receive
// Receive data from comm port.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::Receive( TDes8& aRawdataBuf,
                                        TRequestStatus& aStatus )
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::Receive" );
    HTI_LOG_FORMAT( "Buf max len: %d", aRawdataBuf.MaxLength() );
    iCommPort.ReadOneOrMore( aStatus, aRawdataBuf );
    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::Receive" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::Send
// Write data to comm port.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::Send( const TDesC8& aRawdataBuf,
                                           TRequestStatus& aStatus )
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::Send" );
    iCommPort.Write( aStatus, aRawdataBuf );
    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::Send" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::CancelReceive
// Cancel a pending read.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::CancelReceive()
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::CancelReceive" );
    iCommPort.ReadCancel();
    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::CancelReceive" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::CancelSend
// Cancel a pending write.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::CancelSend()
    {
    HTI_LOG_FUNC_IN( "CHtiUsbSerialCommEcomPlugin::CancelSend" );
    iCommPort.WriteCancel();
    HTI_LOG_FUNC_OUT( "CHtiUsbSerialCommEcomPlugin::CancelSend" );
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::GetSendBufferSize
// -----------------------------------------------------------------------------
TInt CHtiUsbSerialCommEcomPlugin::GetSendBufferSize()
    {
    return KSendBufferLength;
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::GetReceiveBufferSize
// -----------------------------------------------------------------------------
TInt CHtiUsbSerialCommEcomPlugin::GetReceiveBufferSize()
    {
    return KReceiveBufferLength;
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::RateCapsBitmaskFromRate
// Converts a TBps enumeration value to a corresponding bitmask.
// -----------------------------------------------------------------------------
TUint CHtiUsbSerialCommEcomPlugin::RateCapsBitmaskFromRate( TBps aDataRate )
    {
    switch ( aDataRate )
        {
        case EBps50:      return KCapsBps50;
        case EBps75:      return KCapsBps75;
        case EBps110:     return KCapsBps110;
        case EBps134:     return KCapsBps134;
        case EBps150:     return KCapsBps150;
        case EBps300:     return KCapsBps300;
        case EBps600:     return KCapsBps600;
        case EBps1200:    return KCapsBps1200;
        case EBps1800:    return KCapsBps1800;
        case EBps2000:    return KCapsBps2000;
        case EBps2400:    return KCapsBps2400;
        case EBps3600:    return KCapsBps3600;
        case EBps4800:    return KCapsBps4800;
        case EBps7200:    return KCapsBps7200;
        case EBps9600:    return KCapsBps9600;
        case EBps19200:   return KCapsBps19200;
        case EBps38400:   return KCapsBps38400;
        case EBps57600:   return KCapsBps57600;
        case EBps115200:  return KCapsBps115200;
        case EBps230400:  return KCapsBps230400;
        case EBps460800:  return KCapsBps460800;
        case EBps576000:  return KCapsBps576000;
        case EBps1152000: return KCapsBps1152000;
        case EBps4000000: return KCapsBps4000000;
        default:          return 0;
        }
    }

// -----------------------------------------------------------------------------
// CHtiUsbSerialCommEcomPlugin::ShowErrorNotifierL
// Shows an error notifier dialog with text and error code.
// -----------------------------------------------------------------------------
void CHtiUsbSerialCommEcomPlugin::ShowErrorNotifierL( const TDesC& aText,
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
    notifier.Notify( KHtiUsbSerialError, errorMsg,
                     KHtiOkButton, KNullDesC, button, status );
    User::WaitForRequest( status );
    notifier.Close();
    }


// End of file
