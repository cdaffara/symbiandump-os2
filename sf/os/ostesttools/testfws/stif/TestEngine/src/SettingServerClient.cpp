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
* Description: This module contain implementation of RSettingServer 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32svr.h>
#include "SettingServerClient.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// Function for starting the Setting server
IMPORT_C TInt StartNewServer( TName& aServerName );

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: RSettingServer

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C RSettingServer::RSettingServer()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: Connect

    Description: Connect method creates new RSettingServer session.
    First the server is tried to start. If start is successfull or server is
    already started, then a new session is created.  

    Parameters: None

    Return Values: TInt: Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RSettingServer::Connect()
    {
    TName serverName;
    TInt ret = StartNewServer( serverName );

    if( ret == KErrNone || ret == KErrAlreadyExists )
        {
        ret = CreateSession( serverName, Version() );
        }

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: Version

    Description: Return client side version.

    Parameters: None

    Return Values: TVersion: Version number

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TVersion RSettingServer::Version() const
    {
    return( TVersion( KSettingServerMajorVersionNumber,
                      KSettingServerMinorVersionNumber, 
                      KSettingServerVersionNumber 
                     ) );

    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: Close

    Description: Closes the RSettingServer session.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C void RSettingServer::Close()
    {
    // Check that server is connected and send close message to it if possible.
    if( Handle() != 0 )
        {
        TIpcArgs args( TIpcArgs::ENothing, TIpcArgs::ENothing, TIpcArgs::ENothing );
        // Goes to CSettingServer's DispatchMessageL() method
        SendReceive( ESettingServerCloseSession, args );
        }

    RSessionBase::Close();

    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: SetIniFileInformation

    Description: Set initialization filename and path settings to Setting
                 server.

    Parameters: const TFileName& aIniFile: in: Filename and path information.

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RSettingServer::SetIniFileInformation( const TFileName& aIniFile )
    {
    TIpcArgs args( &aIniFile, TIpcArgs::ENothing, TIpcArgs::ENothing );
    // Goes to CSettingServer's DispatchMessageL() method
    SendReceive( ESetIniFileInformation, args );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: ReadLoggerSettingsFromIniFile

    Description: Read Logger setting from initialization file. Mainly use from
                 TestEngine side.

    Parameters: TLoggerSettings& aLoggerSettings: inout: Logger's overwrite
                struct

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RSettingServer::ReadLoggerSettingsFromIniFile( 
                                            TLoggerSettings& aLoggerSettings )
    {
    // Package
    TPckg<TLoggerSettings> loggerSettingsPckg( aLoggerSettings );

	TIpcArgs args( &loggerSettingsPckg, TIpcArgs::ENothing, TIpcArgs::ENothing );
    // Goes to CSettingServer's DispatchMessageL() method
    SendReceive( EReadLoggerSettingsFromIniFile, args );
   
    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: GetLoggerSettings

    Description: Get Logger settings. Mainly use from Logger side.

    Parameters: TLoggerSettings& aLoggerSettings: in: Logger's overwrite struct

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RSettingServer::GetLoggerSettings( 
                                            TLoggerSettings& aLoggerSettings )
    {
    // Package
    TPckg<TLoggerSettings> loggerSettingsPckg( aLoggerSettings );

	TIpcArgs args( &loggerSettingsPckg, TIpcArgs::ENothing, TIpcArgs::ENothing );
    // Goes to CSettingServer's DispatchMessageL() method
    SendReceive( EGetLoggerSettings, args );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: SetIniFileSetting

    Description: Set new initialization file setting(e.g. SetAttribute).

    Parameters: TName& aNewIniFileSetting: in: New setting

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RSettingServer::SetNewIniFileSetting( TName& aNewIniFileSetting )
    {
	TIpcArgs args( &aNewIniFileSetting, TIpcArgs::ENothing, TIpcArgs::ENothing );
    // Goes to CSettingServer's DispatchMessageL() method
    SendReceive( ESetNewIniFileSetting, args );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: GetEngineSettings

    Description: Get Engine settings.

    Parameters: TEngineSettings& aEngineSettings: in: Engine settings struct

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RSettingServer::GetEngineSettings(TEngineSettings& aEngineSettings)
    {
    // Package
    TPckg<TEngineSettings> engineSettingsPckg(aEngineSettings);

    TIpcArgs args(&engineSettingsPckg, TIpcArgs::ENothing, TIpcArgs::ENothing);
    // Goes to CSettingServer's DispatchMessageL() method
    SendReceive(EGetEngineSettings, args);

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: RSettingServer

    Method: StoreEngineSettings

    Description: Store Engine settings.

    Parameters: TEngineSettings& aEngineSettings: in: Engine settings struct

    Return Values: TInt: Symbian error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RSettingServer::StoreEngineSettings(TEngineSettings& aEngineSettings)
    {
    // Package
    TPckg<TEngineSettings> engineSettingsPckg(aEngineSettings);

    TIpcArgs args(&engineSettingsPckg, TIpcArgs::ENothing, TIpcArgs::ENothing);
    // Goes to CSettingServer's DispatchMessageL() method
    SendReceive(EStoreEngineSettings, args);

    return KErrNone;
    }

// End of File
