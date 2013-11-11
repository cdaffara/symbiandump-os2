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
* CSettingServer.
*
*/

#ifndef SETTING_SERVER_H
#define SETTING_SERVER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifLogger.h>
#include "Logging.h"
#include "TestEngineClient.h"

// CONSTANTS

// Server heap size
const TUint KDefaultHeapSize = 0x10000;     // 64 K
const TUint KMaxHeapSize     = 0x20000;     // 128 K 

// MACROS

// DATA TYPES

// Panic reasons
enum TSettingServerPanic
    {
    EBadRequest,
    EBadDescriptor,
    EMainSchedulerError,
    ESvrCreateServer,
    ECreateTrapCleanup,
    ENoStartupInformation,
    ETooManyCloseSessions,
    EPopFromEmptyStack
    };

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

// DESCRIPTION
// CSettingServer is a server class.
class CSettingServer
    :public CServer2 // Inter Process Communication version 2 is used(EKA2)
    {
    public: // Enumerations
        enum { ESettingServerSchedulerPriority = CActive::EPriorityHigh };

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CSettingServer* NewL( const TName& aName );

        /**
        * Destructor of CSettingServer.
        */
        ~CSettingServer();

    public: // New functions

        /**
        * ThreadFunction is used to create new thread.
        */
        static TInt ThreadFunction( TAny* aStarted );       

        /**
        * PanicServer panics the CSettingServer
        */
        static void PanicServer( const TSettingServerPanic aPanic );

        /**
        * Return pointer to the Logger(iSettingServerLogger)
        */
        CStifLogger* CSettingServer::Logger();

    public: // Functions from base classes

        /**
        * NewSessionL creates new CSettingServer session.
        */
        CSession2* NewSessionL( const TVersion &aVersion,
                                    const RMessage2& aMessage ) const;
        /**
        * Open session
        */
        void OpenSession();

        /**
        * Close session
        */
        void CloseSession();

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CSettingServer();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data

        // NOTE:
        // There is one CSettingServer and may be many CLoggerSetting sessions
        // so variables are defined here and are available to all sessions.

        // Logger overwrite settings
        TLoggerSettings         iLoggerSettings;

        // Includes inifile name and path information
        TFileName               iIniFile;
        
        // Engine settings
        TEngineSettings         iEngineSettings;
    
    protected:  // Data
        // None

    private:    // Data

        // Session counter
        TInt                    iSessions;

        // Logger instance
        CStifLogger*            iSettingServerLogger;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// CLoggerSetting is a session class.
// Session for the CSettingServer server, to a single client-side session
// a session may own any number of CCounter objects

class CLoggerSetting
    :public CSession2
    {    
    public:
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor

        /**
        * Construct a clean-up server session
        */
        static CLoggerSetting* NewL( CSettingServer* aServer );

        /**
        * Destructor
        */
        virtual ~CLoggerSetting();

    public: // New functions

        /**
        * Dispatch message
        */
        TInt DispatchMessageL( const RMessage2& aMessage,
                                TBool& aIsMessageSync );

        /**
        * Close the session to CSettingpServer.
        */
        TInt CloseSession( /*const RMessage& aMessage*/ );

        /**
        * Read Logger setting from initialization file. Mainly use from
        * TestEngine side.
        */
        TInt ReadLoggerSettingsFromIniFile( const RMessage2& aMessage );

        /**
        * Set initialization filename and path settings to Setting server.
        */
        TInt SetIniFileSettings( const RMessage2& aMessage );

        /**
        * Get Logger settings. Mainly use from Logger side.
        */
        TInt GetLoggerSettings( const RMessage2& aMessage );

        /**
        * 
        */
        TInt SetNewIniFileSetting( const RMessage2& aMessage );

        /**
        * PanicClient panics the user of CLoggerSetting
        */
        void PanicClient( TInt aPanic, const RMessage2& aMessage ) const;
        
        /**
        * Get test engine settings
        */
        TInt GetEngineSettings(const RMessage2& aMessage);

        /**
        * Set (store) test engine settings
        */
        TInt StoreEngineSettings(const RMessage2& aMessage);

    public: // Functions from base classes

        /**
        * ServiceL handles the messages to CSettingServer
        */
        virtual void ServiceL( const RMessage2& aMessage );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CLoggerSetting();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CSettingServer* aServer );

        /**
        * Real resource reservation routine
        */
        TInt ReserveInternalL( const RMessage2& aMessage );

        /**
        * Read Logger default parameters for initialization file.
        */
        TInt ReadLoggerDefaults();

    public:     // Data
        // None
   
    protected:  // Data
        // None

    private:    // Data

        // Pointer to "parent"
        CSettingServer*         iSettingServer;

        // Indication for reading logger defaults from initialization file.
        TBool                   iReadOrUpdateLoggerDefaults;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

#endif // SETTING_SERVER_H

// End of File
