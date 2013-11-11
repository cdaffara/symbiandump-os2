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
* Description: This file contains the header file of the 
* RSettingServer
*
*/

#ifndef SETTING_SERVER_CLIENT_H
#define SETTING_SERVER_CLIENT_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <TestEngineClient.h>


// CONSTANTS

// The server version.
// A version must be specified when creating a session with the server.
const TUint KSettingServerMajorVersionNumber=0;
const TUint KSettingServerMinorVersionNumber=1;
const TUint KSettingServerVersionNumber=1;

// MACROS

// DATA TYPES
// Opcodes used in message passing between client and server
enum TSettingServerRequests
    {
    // RSettingServer requests
    ESettingServerCloseSession,

    // RLoggerSetting requests
    EReadLoggerSettingsFromIniFile,
    EGetLoggerSettings,

    // IniFile information requests
    ESetIniFileInformation,

    // e.g. SetAttribute requests
    ESetNewIniFileSetting,
    
    // Engine
    EGetEngineSettings,
    EStoreEngineSettings,
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

// DESCRIPTION
// RSettingServer is a client class of Setting server
// The connect function starts the server, if it not already running.

class RSettingServer
        :public RSessionBase 
    {
    public: // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        
        /**
        * Constructor.
        */
        IMPORT_C RSettingServer();

        /**
        * Closes the RSettingServer session.
        */
        IMPORT_C void Close();

    public: // New functions
        
        /**
        * Connect method creates new RSettingServer session 
        */
        IMPORT_C TInt Connect();

        /**
        * Version returns the client side version number from the RSettingServer.
        */
        IMPORT_C TVersion Version() const;

        /**
        * Set initialization filename and path settings to Setting server.
        */
        IMPORT_C TInt SetIniFileInformation( const TFileName& aIniFile );

        /**
        * Read Logger setting from initialization file. Mainly use from
        * TestEngine side.
        */
        IMPORT_C TInt ReadLoggerSettingsFromIniFile( 
                                        TLoggerSettings& aLoggerSettings );

        /**
        * Get Logger settings. Mainly use from Logger side.
        */
        IMPORT_C TInt GetLoggerSettings( TLoggerSettings& aLoggerSettings );

        /**
        * Set new initialization file setting(e.g. SetAttribute).
        */
        IMPORT_C TInt SetNewIniFileSetting( TName& aNewIniFileSetting );
        
        /**
        * Get TestEngine settings
        */
        IMPORT_C TInt GetEngineSettings(TEngineSettings& aEngineSettings);

        /**
        * Set TestEngine settings
        */
        IMPORT_C TInt StoreEngineSettings(TEngineSettings& aEngineSettings);

    public:     // Functions from base classes
        // None

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:    // Functions from base classes
        // None

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data
        // None

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

#endif // SETTING_SERVER_CLIENT_H

// End of File
