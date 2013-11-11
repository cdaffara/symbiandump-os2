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
* Description: This module contains implementation of CSettingServer 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32std.h>
#include <e32svr.h>

#include "SettingServerClient.h"
#include "SettingServer.h"

#include <StifParser.h>
#include "STIFTestFrameworkSettings.h"

#include "Logging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// None

#undef LOGGER
#define LOGGER iSettingServer->Logger()

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: NewL

    Description: Create new CLoggerSetting (Setting server session)

    Parameters: RThread& aClient: in: Handle to thread client
                CSettingServer* aServer: in: Pointer to CSettingServer

    Return Values: CLoggerSetting* Pointer to new CLoggerSetting

    Errors/Exceptions: Leaves if memory allocation fails or ConstructL leaves.

    Status: Proposal

-------------------------------------------------------------------------------
*/
CLoggerSetting* CLoggerSetting::NewL( CSettingServer* aServer )
    {
    CLoggerSetting* self=new( ELeave ) CLoggerSetting();
    CleanupStack::PushL( self );
    self->ConstructL( aServer );
    CleanupStack::Pop();
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: CLoggerSetting

    Description: Constructor. Initialise base class.

    Parameters: RThread& aClient: in: Handle to client

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CLoggerSetting::CLoggerSetting() :
        CSession2()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: ~CLoggerSetting

    Description: Destructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
CLoggerSetting::~CLoggerSetting()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: ConstructL

    Description: Second level constructor.
    
    Parameters: CSettingServer* aServer: in: Server

    Return Values: None

    Errors/Exceptions: Leaves if base class CreateL leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CLoggerSetting::ConstructL( CSettingServer* aServer )
    {
    // @spe __TRACE( KInit,( _L( "CLoggerSetting::ConstructL - constructing server session" ) ) );
    iSettingServer = aServer;

    // second-phase construct base class
    //CSession2::CreateL();

//    iSettingServer = aServer;

     // Inform server class that session is opening. 
    iSettingServer->OpenSession();    

    __TRACE( KInit,( _L( "CLoggerSetting::ConstructL - constructing server session done" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: PanicClient

    Description: Panic clients.

    Parameters: TInt aPanic: in: Panic code

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CLoggerSetting::PanicClient( const TInt aPanic, const RMessage2& aMessage ) const
    {
    __TRACE( KError,( _L( "CLoggerSetting::PanicClient code = %d" ), aPanic ) );

    _LIT( KTxtModule,"CLoggerSetting" );

    aMessage.Panic( KTxtModule,aPanic );
    }

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: CloseSession

    Description: Close session

    Parameters: const RMessage&: out: Message to be completed.

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::CloseSession( /*const RMessage& aMessage*/ )
    {
    __TRACE( KVerbose,( _L( "CLoggerSetting::CloseSession in" ) ) );
             
    // Close session from server (and stop it if required)
    iSettingServer->CloseSession();

    // NOTE: This complete will make after DispatchMessageL in ServiceL method.
    // Complete message. This must be done after possible closing
    // of the server to make sure that server is first closed before
    // client can send new messages.
    // aMessage.Complete( KErrNone );

    __TRACE( KVerbose,( _L( "CLoggerSetting::CloseSession out" ) ) );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: ServiceL

    Description: Trap harness for dispatcher

    Parameters: const RMessage& aMessage: inout: Message

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CLoggerSetting::ServiceL( const RMessage2& aMessage )
    {
    __TRACE( KVerbose,( _L( "CLoggerSetting::ServiceL in" ) ) );

    TInt r( KErrNone );
    TBool isMessageSync( EFalse );
    
    TRAPD( ret, r = DispatchMessageL( aMessage, isMessageSync ) );

    if( ret != KErrNone )
        {
        // Complete message on leaving cases with leave code allways.
        __TRACE( KError,( _L( "CLoggerSetting::DispatchMessageL leaved" ) ) );
        aMessage.Complete( ret );
        }
    else if( isMessageSync )
        {
        // Message is syncronous, Complete message with error code 
        // originating from message handling
         __TRACE( KVerbose,( _L( "CLoggerSetting::DispatchMessageL completed with: %d" ), r ) );
        aMessage.Complete( r );
        }
    __TRACE( KVerbose,( _L( "CLoggerSetting::ServiceL out" ) ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: DispatchMessageL

    Description: Dispatch message, calls corresponding function to do it.

    Parameters: const RMessage& aMessage: inout: Message to be handled
                TBool& aIsMessageSync: inout: Indication of the message type

    Return Values: TInt: Error code

    Errors/Exceptions: Leaves if operation handling function leaves

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::DispatchMessageL( const RMessage2& aMessage,
                                        TBool& aIsMessageSync )
    {
    __TRACE( KInit ,( _L( "CLoggerSetting::DispatchMessageL in" ) ) );
    switch( aMessage.Function() )
        {
        case ESettingServerCloseSession: 
            {
            __TRACE( KInit ,( _L( "Closing Setting server session" ) ) );
            aIsMessageSync = ETrue;
            return CloseSession( /*aMessage*/ );
            }
        case ESetIniFileInformation:
            {
            __TRACE( KInit ,( _L( "Set initialization file setting" ) ) );
            aIsMessageSync = ETrue;
            return SetIniFileSettings( aMessage );
            }
        case EReadLoggerSettingsFromIniFile:
            {
            __TRACE( KInit ,( _L( "Read Logger setting(s) from initialization file" ) ) );
            aIsMessageSync = ETrue;
            return ReadLoggerSettingsFromIniFile( aMessage );
            }
        case EGetLoggerSettings:
            {
            __TRACE( KInit ,( _L( "Get Logger setting(s)" ) ) );
            aIsMessageSync = ETrue;
            return GetLoggerSettings( aMessage );
            }
        case ESetNewIniFileSetting:
            {
            __TRACE( KInit ,( _L( "Set new initialization file setting" ) ) );
            aIsMessageSync = ETrue;
            return SetNewIniFileSetting( aMessage );
            }
        case EGetEngineSettings:
            {
            __TRACE(KInit, (_L("Get Engine settings")));
            aIsMessageSync = ETrue;
            return GetEngineSettings(aMessage);
            }
        case EStoreEngineSettings:
            {
            __TRACE(KInit, (_L("Store Engine settings")));
            aIsMessageSync = ETrue;
            return StoreEngineSettings(aMessage);
            }
         // Invalid request
        default:
            {
            PanicClient( EBadRequest, aMessage );
            return KErrNotSupported;
            }
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: SetIniFileSettings

    Description: Set initialization filename and path settings to Setting
                 server.

    Parameters: const RMessage& aMessage: inout: Message to be handled

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::SetIniFileSettings( const RMessage2& aMessage )
    {
    __TRACE( KInit, ( _L( "CLoggerSetting::SetIniFileSettings" ) ) );

    // Get data from message
    TFileName iniFile;
    
    // Get length from message param 1
    // Read data from client descriptor (param 0)
    TRAPD( ret, aMessage.ReadL( 0, iniFile ) );

    if ( ret != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        return KErrBadDescriptor;
        }

    // Inifile name and path information
    iSettingServer->iIniFile = iniFile;

    __TRACE( KInit, ( _L( "New initialization file: [%S]" ), &iSettingServer->iIniFile ) );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: ReadLoggerSettingsFromIniFile

    Description: Read Logger setting from initialization file. Mainly use from
                 TestEngine side.

    Parameters: const RMessage& aMessage: inout: Message to be handled

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::ReadLoggerSettingsFromIniFile( const RMessage2& aMessage )
    {
    __TRACE( KInit, ( _L( "CLoggerSetting::ReadLoggerSettingsFromIniFile" ) ) );

    TInt ret = ReadLoggerDefaults();
    if( ret != KErrNone )
        {
        return ret;
        }

    // Copies logger settings to the package
    TPckg<TLoggerSettings> loggerSettingsPckg( iSettingServer->iLoggerSettings );

    // Writes a packege that includes the logger overwrite settings to aMessage
    TRAPD( err, aMessage.WriteL( 0, loggerSettingsPckg ) );

    if ( err != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CLoggerSetting

    Method: ReadLoggerDefaults

    Description: Parse Logger defaults from STIF initialization
                 file.

    Parameters: None

    Return Values: TInt: Return Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::ReadLoggerDefaults()
    {
    __TRACE( KInit,( _L( "" ) ) );
    __TRACE( KInit,( _L( "CLoggerSetting::ReadLoggerDefaults" ) ) );
    __TRACE( KInit,( _L( "Start reading and parsing Logger defaults" ) ) );

    TInt ret = KErrNone;
    
    // Create parser for parsing ini file
    CStifParser* parser = NULL;
    TRAPD( r, parser = CStifParser::NewL( _L(""), iSettingServer->iIniFile ) );
    if ( r != KErrNone )
        {
        __TRACE( KError,( CStifLogger::ERed, _L( "Can't open ini-file [%S], code %d" ), &iSettingServer->iIniFile, r ) );
        return r;
        }

    CSTIFTestFrameworkSettings* settings = NULL;
    TRAPD( settings_ret, settings = CSTIFTestFrameworkSettings::NewL() );
    if ( settings_ret != KErrNone )
        {
        __TRACE( KError,( CStifLogger::ERed, _L( "CSTIFTestFrameworkSettings class object creation fails") ) );
        return settings_ret;
        }

    TInt get_ret( KErrNone );

    CStifSectionParser* sectionParser = NULL;
    // Parse Logger's overwrite values
    _LIT( KDefaultsStart, "[Logger_Defaults]" );
    _LIT( KDefaultsEnd, "[End_Logger_Defaults]" );
    __TRACE( KInit,( _L( "Starting to search sections" ) ) );
    
    TRAP( r,
        sectionParser = parser->SectionL( KDefaultsStart, KDefaultsEnd );
        );
    // Get directory settings
    if ( ( r == KErrNone ) && sectionParser )
        {
        __TRACE( KInit,( _L( "Found '%S' and '%S' sections" ), &KDefaultsStart, &KDefaultsEnd ) );

        __TRACE( KInit,( _L( "Parsing Logger directory indicator" ) ) );
        TBool createDir( EFalse );
        TRAP( get_ret,
            get_ret = settings->GetBooleanSettingsL( sectionParser,
                                _L( "CreateLogDirectories=" ), createDir );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Logger directory indicator: %d"), createDir ) );
            iSettingServer->iLoggerSettings.iCreateLogDirectories = createDir;
            iSettingServer->iLoggerSettings.iIsDefined.iCreateLogDir = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Indicator not found or not given" ) ) );
            }

        // Get Logger path settings
        __TRACE( KInit,( _L( "Parsing Logger path setting (Emulator)" ) ) );
        TPtrC emulatorPath;
        get_ret = settings->GetFileSetting( sectionParser,
                                               _L( "EmulatorBasePath=" ),
                                               emulatorPath );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Logger base path setting: %S"), &emulatorPath ) );
            iSettingServer->iLoggerSettings.iEmulatorPath = emulatorPath;
            iSettingServer->iLoggerSettings.iIsDefined.iPath = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Path not found or not given (Emulator)" ) ) );
            }

        // Get Logger format settings
        __TRACE( KInit,( _L( "Parsing Logger file type setting (Emulator)" ) ) );
        CStifLogger::TLoggerType emulatorType = CStifLogger::ETxt;
        TRAP( get_ret, 
            get_ret = settings->GetFormatL( sectionParser,
                                    _L( "EmulatorFormat=" ), emulatorType );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "File type setting: %d"), emulatorType ) );
            iSettingServer->iLoggerSettings.iEmulatorFormat = emulatorType;
            iSettingServer->iLoggerSettings.iIsDefined.iFormat = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "File type not found or not given (Emulator)" ) ) );
            }

        // Get Logger output settings
        __TRACE( KInit,( _L( "Parsing Logger output setting (Emulator)" ) ) );
        CStifLogger::TOutput emulatorOutput = CStifLogger::EFile;
        TRAP( get_ret,
            get_ret = settings->GetOutputL( sectionParser,
                                    _L( "EmulatorOutput=" ), emulatorOutput );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Output setting: %d"), emulatorOutput ) );
            iSettingServer->iLoggerSettings.iEmulatorOutput = emulatorOutput;
            iSettingServer->iLoggerSettings.iIsDefined.iOutput = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Output not found or not given (Emulator)" ) ) );
            }

        // Get Logger HW path settings
        __TRACE( KInit,( _L( "Parsing Logger path setting (Hardware)" ) ) );
        TPtrC hwPath;
        get_ret = settings->GetFileSetting( sectionParser,
                                               _L( "HardwareBasePath=" ),
                                               hwPath );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Base path setting: %S"), &hwPath ) );
            iSettingServer->iLoggerSettings.iHardwarePath = hwPath;
            iSettingServer->iLoggerSettings.iIsDefined.iHwPath = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Path not found or not given (Hardware)" ) ) );
            }

        // Get Logger HW format settings
        __TRACE( KInit,( _L( "Parsing Logger file type setting (Hardware)" ) ) );
        CStifLogger::TLoggerType hwType = CStifLogger::ETxt;
        TRAP( get_ret,
            get_ret = settings->GetFormatL( sectionParser,
                                        _L( "HardwareFormat=" ), hwType );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "File type setting: %d"), hwType ) );
            iSettingServer->iLoggerSettings.iHardwareFormat = hwType;
            iSettingServer->iLoggerSettings.iIsDefined.iHwFormat = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "File type not found or not given (Hardware)" ) ) );
            }

        // Get Logger HW output settings
        __TRACE( KInit,( _L( "Parsing Logger output setting (Hardware)" ) ) );
        CStifLogger::TOutput hwOutput = CStifLogger::EFile;
        TRAP( get_ret, 
        get_ret = settings->GetOutputL( sectionParser,
                                    _L( "HardwareOutput=" ), hwOutput );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Output setting: %d"), hwOutput ) );
            iSettingServer->iLoggerSettings.iHardwareOutput = hwOutput;
            iSettingServer->iLoggerSettings.iIsDefined.iHwOutput = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Output not found or not given (Hardware)" ) ) );
            }

        // Get Logger file creation mode (overwrite settings)
        __TRACE( KInit,( _L( "Parsing file creation setting" ) ) );
        TBool overwrite( ETrue );
        TRAP( get_ret,
            get_ret = settings->GetOverwriteL( sectionParser,
                                _L( "FileCreationMode=" ), overwrite );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "File creation mode setting: %d"), overwrite ) );
            iSettingServer->iLoggerSettings.iOverwrite = overwrite;
            iSettingServer->iLoggerSettings.iIsDefined.iOverwrite = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "File creation not found or not given" ) ) );
            }

        // Get Time stamp settings
        __TRACE( KInit,( _L( "Parsing time stamp setting" ) ) );
        TBool timeStamp( ETrue );
        TRAP( get_ret,
            get_ret = settings->GetBooleanSettingsL( sectionParser,
                                _L( "WithTimeStamp=" ), timeStamp );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Time stamp setting: %d"), timeStamp ) );
            iSettingServer->iLoggerSettings.iTimeStamp = timeStamp;
            iSettingServer->iLoggerSettings.iIsDefined.iTimeStamp = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Time stamp not found or not given" ) ) );
            }

        // Get Test report line break settings
        __TRACE( KInit,( _L( "Parsing line break setting" ) ) );
        TBool lineBreak( ETrue );
        TRAP( get_ret,
            get_ret = settings->GetBooleanSettingsL( sectionParser,
                                _L( "WithLineBreak=" ), lineBreak );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Line break setting: %d"), lineBreak ) );
            iSettingServer->iLoggerSettings.iLineBreak = lineBreak;
            iSettingServer->iLoggerSettings.iIsDefined.iLineBreak = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Line break not found or not given" ) ) );
            }

        // Get Test report event ranking settings
        __TRACE( KInit,( _L( "Parsing event ranking setting" ) ) );
        TBool eventRanking( EFalse );
        TRAP( get_ret,
            get_ret = settings->GetBooleanSettingsL( sectionParser,
                            _L( "WithEventRanking=" ), eventRanking );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Event ranking setting: %d"), eventRanking ) );
            iSettingServer->iLoggerSettings.iEventRanking = eventRanking;
            iSettingServer->iLoggerSettings.iIsDefined.iEventRanking = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Event ranking not found or not given" ) ) );
            }

        // Get Test report thread id settings
        __TRACE( KInit,( _L( "Parsing thread id setting" ) ) );
        TBool threadId( EFalse );
        TRAP( get_ret,
            get_ret = settings->GetBooleanSettingsL( sectionParser,
                                    _L( "ThreadIdToLogFile=" ), threadId );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Thread id setting: %d"), threadId ) );
            iSettingServer->iLoggerSettings.iThreadId = threadId;
            iSettingServer->iLoggerSettings.iIsDefined.iThreadId = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Thread id not found or not given" ) ) );
            }
        // Get unicode setting
        __TRACE( KInit,( _L( "Parsing unicode setting" ) ) );
        TBool unicode(EFalse);
        TRAP( get_ret,
            get_ret = settings->GetBooleanSettingsL( sectionParser,
                                    _L( "FileUnicode=" ), unicode );
            );
        if ( get_ret == KErrNone )
            {
            __TRACE( KInit,( _L( "Unicode setting: %d"), unicode ) );
            iSettingServer->iLoggerSettings.iUnicode = unicode;
            iSettingServer->iLoggerSettings.iIsDefined.iUnicode = ETrue;
            }
        else
            {
            __TRACE( KInit,( _L( "Unicode setting not found or not given" ) ) );
            }

        // Check if test case title should be added to log file name
        __TRACE( KInit,( _L( "Parsing AddTestCaseTitle setting" ) ) );
        TBool addTestCaseTitle(EFalse);
        TRAP(get_ret,
            get_ret = settings->GetBooleanSettingsL(sectionParser,
                                    _L( "AddTestCaseTitle=" ), addTestCaseTitle);
            );
        if(get_ret == KErrNone)
            {
            __TRACE( KInit,( _L( "AddTestCaseTitle setting: %d"), addTestCaseTitle));
            iSettingServer->iLoggerSettings.iAddTestCaseTitle = addTestCaseTitle;
            iSettingServer->iLoggerSettings.iIsDefined.iAddTestCaseTitle = ETrue;
            }
        else
            {
            __TRACE(KInit, (_L("AddTestCaseTitle setting not found or not given")));
            }
        }
    else
        {
        __TRACE( KInit,( _L( "Not found '%S' and '%S' sections" ), &KDefaultsStart, &KDefaultsEnd ) );
        ret = KErrNotFound;
        }
    __TRACE( KInit,( _L( "End reading and parsing Logger defaults" ) ) );
    __TRACE( KInit,( _L( "" ) ) );
    delete sectionParser;
    delete settings;
    delete parser;

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: GetLoggerSettings

    Description: Get Logger settings. Mainly use from Logger side.

    If there are not any active sessions, then stop active scheduler and
    close whole server.

    Parameters: None

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::GetLoggerSettings( const RMessage2& aMessage )
    {
    __TRACE( KInit, ( _L( "CLoggerSetting::GetLoggerSettings in" ) ) );
    
    // Copies logger settings to the package
    TPckg<TLoggerSettings> loggerSettingsPckg( iSettingServer->iLoggerSettings );

    // Writes a packege that includes the logger overwrite settings to aMessage
    TRAPD( err, aMessage.WriteL( 0, loggerSettingsPckg ) );

    if ( err != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        }

    __TRACE( KInit, ( _L( "CLoggerSetting::GetLoggerSettings out" ) ) );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: SetNewIniFileSetting

    Description: Set new initialization file settings to WINS and HW platforms.

    Parameters: None

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::SetNewIniFileSetting( const RMessage2& aMessage )
    {
    __TRACE( KInit, ( _L( "CLoggerSetting::SetNewIniFileSetting" ) ) );
    
    TInt ret( 0 );

    TName newIniFileSetting;
    // Get data from message
    // NOTE! If message length is over TName, ReadL will cut the message
    // to allowed size and won't return any error code or leave code.
    TRAP( ret, aMessage.ReadL( 0, newIniFileSetting ) );

    if ( ret != KErrNone )
        {
        PanicClient( EBadDescriptor, aMessage );
        return KErrBadDescriptor;
        }

    iSettingServer->iLoggerSettings.iEmulatorPath = newIniFileSetting;
    iSettingServer->iLoggerSettings.iIsDefined.iPath = ETrue;
    iSettingServer->iLoggerSettings.iHardwarePath = newIniFileSetting;
    iSettingServer->iLoggerSettings.iIsDefined.iHwPath = ETrue;

    __TRACE( KInit, ( _L( "New emulator and hardware path: [%S]" ), &newIniFileSetting ) );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: GetEngineSettings

    Description: Get TestEngine settings.

    If there are not any active sessions, then stop active scheduler and
    close whole server.

    Parameters: None

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::GetEngineSettings(const RMessage2& aMessage)
    {
    __TRACE(KInit, (_L("CLoggerSetting::GetEngineSettings in")));
    
    // Copies logger settings to the package
    TPckg<TEngineSettings> engineSettingsPckg(iSettingServer->iEngineSettings);

    // Writes a packege that includes the logger overwrite settings to aMessage
    TRAPD(err, aMessage.WriteL(0, engineSettingsPckg));

    if(err != KErrNone)
        {
        PanicClient(EBadDescriptor, aMessage);
        }

    __TRACE(KInit, (_L("CLoggerSetting::GetEngineSettings out")));

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CSettingServer

    Method: StoreEngineSettings

    Description: Store TestEngine settings.

    If there are not any active sessions, then stop active scheduler and
    close whole server.

    Parameters: None

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CLoggerSetting::StoreEngineSettings(const RMessage2& aMessage)
    {
    __TRACE(KInit, (_L("CLoggerSetting::StoreEngineSettings in")));
    
    // Copies logger settings to the package
    TPckg<TEngineSettings> engineSettingsPckg(iSettingServer->iEngineSettings);

    // Reads a packege that includes the engine settings
    TRAPD(err, aMessage.ReadL(0, engineSettingsPckg));

    if(err != KErrNone)
        {
        PanicClient(EBadDescriptor, aMessage);
        }

    __TRACE(KInit, (_L("CLoggerSetting::StoreEngineSettings out")));

    return KErrNone;
    }

// End of File
