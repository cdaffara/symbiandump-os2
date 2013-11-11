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
* Description:  Implementation of ECOM plug-in service interface providing
*                process and application control service.
*
*/



#ifndef HTIAPPSERVICEPLUGIN_H__
#define HTIAPPSERVICEPLUGIN_H__

//  INCLUDES
#include <e32base.h>
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CDesC8ArrayFlat;

// CLASS DECLARATION
/**
*  Implementation of ECOM plug-in service interface.
*  Provides the service to control processes and applications.
*
*/
class CHtiAppServicePlugin : public CHTIServicePluginInterface
    {
protected:
    //commands
    enum TAppCommand
        {
        //commands
        //process control
        EStartProcess       = 0x02,
        EStartProcess_u     = 0x03,
        EStatusProcess      = 0x04,
        EStatusProcess_u    = 0x05,
        EStatusProcess_id   = 0x06,
        EStopProcess        = 0x08,
        EStopProcess_u      = 0x09,
        EStopProcess_id     = 0x0A,
        EListProcesses      = 0x0C,
        EListProcesses_u    = 0x0D,
        EStartProcessRetVal = 0x0E,
        EStartProcessRetVal_u = 0x07,
        EGetProcessExitCode = 0x0B,

        EProcessLastCommand = 0x0F, //!!! used to sort process control from
                                    //!!! app control

        //app control
        EStartApp           = 0x10,
        EStartApp_u         = 0x11,
        EStartApp_uid       = 0x12,
        EStartApp_uid_u     = 0x13,
        EStartDoc           = 0x14,
        EStartDoc_u         = 0x15,
        EStatusApp          = 0x16,
        EStatusApp_u        = 0x17,
        EStatusDoc          = 0x18,
        EStatusDoc_u        = 0x19,
        EStatusApp_uid      = 0x1A,
        EStopApp            = 0x1C,
        EStopApp_u          = 0x1D,
        EStopDoc            = 0x1E,
        EStopDoc_u          = 0x1F,
        EStopApp_uid        = 0x20,
        EListApps           = 0x24,
        EListApps_u         = 0x25,

        EAppLastCommand     = 0x2F, //!!! used to sort process control from
                                    //!!! app control
        EInstall         = 0x30,
        EInstall_u       = 0x31,
        EUnInstall       = 0x32,
        EUnInstallName_u = 0x33,
        EUnInstallName   = 0x34,

        ESisLastCommand = 0x3F, //!!! used to sort sis control from
                                //!!! app control
        //responses
        EOk             = 0xF0,
        ENotFound       = 0xF1,
        EAlreadyRunning = 0xF2,
        EAlreadyStopped = 0xF3,
        ERunning        = 0xF4,
        EKilled         = 0xF5,
        EPanic          = 0xF6
        };

public:
    static CHtiAppServicePlugin* NewL();

    // Interface implementation
    void ProcessMessageL(const TDesC8& aMessage,
                         THtiMessagePriority aPriority);
    
    /**
	* Called by HTI-framework to initialize this plugin
	*/
    void InitL();

protected:
    CHtiAppServicePlugin();
    void ConstructL();

    virtual ~CHtiAppServicePlugin();

    void HandleProcessControlL(const TDesC8& aMessage);

    /**
    * Extracts string from incoming request and convert it to unicode
    * for non-unicode request
    * aResult should have enough maz length and it will contain
    * Function returns either offset for a next parameter in aRequest
    * or some symbian error code
    */
    TInt ParseString( const TDesC8& aRequest,
                      TInt anOffset,
                      TBool aUnicode,
                      TDes& aResult);

    //command handlers
    void HandleStartProcessL( const TDesC& aProgramName,
                              const TDesC& aCmdLine,
                              TBool aStoreProcessHandle );

    void HandleStopProcessL( RProcess& aProcess );
    void HandleStatusProcessL( RProcess& aProcess );

    void HandleListProcessesL( const TDesC& aMatch );

    //construct and send short message
    void SendMessageL(TAppCommand aResponseCode, const TDesC8& aMsg = KNullDesC8 );

    /**
    * Helper to send error message
    * @return KErrNone or some system-wide error code
    */
    inline TInt SendErrorMsg( TInt anError, const TDesC8& aMsg );

    /**
    *
    */
    TInt OpenProcessL( RProcess& aProcess, const TDesC& aMatch );
    TInt OpenProcessL( RProcess& aProcess, const TUint32 aProcessId );

    /**
    * Convert string to T 32bit int (e.g. TUint, TInt32)
    */
    template<class T> T Parse32( const TDesC8& a32int);
    
    

protected:

    // Container array for process handles for querying the process exit codes
    RArray<RProcess> iProcessHandleArray;
    
private:
	CHTIServicePluginInterface* iAppServiceUiPlugin;
	RLibrary iLibrary;


    };

#endif

// End of file
