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
* Description: This module contains implementation of RTestServer, 
* RTestModule and RTestExecution class member functions.
*
*/

// INCLUDE FILES
#include <e32svr.h>
#include <stifinternal/TestServerClient.h>
#include "TestServerCommon.h"
//--PYTHON-- begin
#include "StifPython.h"
//--PYTHON-- end

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

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: RTestServer

    Method: RTestServer

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C RTestServer::RTestServer()
    {
    }


/*
-------------------------------------------------------------------------------

    Class: RTestServer

    Method: Connect

    Description: Connect method creates new RTestServer session.
    RTestServer session is used to manage the test case execution.
    
    Parameters: const TFileName& aModuleName: in: Module to be loaded
                const TDesC& aConfigFile: in: Test case(config) file
                name(Used in TestScripter case for parsing STIF settings).

    Return Values: TInt Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestServer::Connect( const TFileName& aModuleName,
                                    const TDesC& aConfigFile
                                    )
    {
    TFileName serverName;

    TInt ret = KErrNone;

    // Create global semaphore start-up. It will be indexed (if given name already exsists, index will be increased)
    RSemaphore startSemaphore;
    TName semaphoreName = _L("startupSemaphore");
    semaphoreName.Append( aModuleName );
    TInt x = 0;
    RBuf semName;
    ret = semName.Create(KMaxName);
    if(ret != KErrNone)
        {
        RDebug::Print(_L("RTestServer::Connect() Could not create buffer for semaphore name [%d]"), ret);
        return ret;
        }
    do
        {
        semName.Format(_L("%S%d"), &semaphoreName, x);
        if (semName.Length() > KMaxKernelName)
            {
            // if the cfg file name is larger than maximum length of acceptable length of semaphore .
            RDebug::Print(_L("Test Module name or Configuration File name is too long."));
            
            semName.Close();
            return KErrBadName;
            }
        
        ret = startSemaphore.CreateGlobal(semName, 0);
        RDebug::Print(_L("RTestServer::Connect() Creating global semaphore [%S] with result [%d]"), &semName, ret);
        if(ret != KErrAlreadyExists)
            {
            semaphoreName.Copy(semName);
            break;
            }
        x++;
        } while (ETrue);
    semName.Close();
    
    if ( ret != KErrNone )
        {
        startSemaphore.Close();
        return ret;
        }

    // By default start 'testserverstarter.exe'
    TFileName pathAndExeModule;
    if ( aModuleName.Find( _L( "testscripter_ui_" ) ) == 0 ) 
    	{
        pathAndExeModule.Copy( KDefaultUiExeName );    	
    	}
    else
    	{
        pathAndExeModule.Copy( KDefaultExeName );    	
    	}

    RProcess pr;

    // Data to be passed to new process. It will contain module name and synchronization semaphore name separated with space.
    // I.e. it will be: "mymodule startupSemaphore0"
    RBuf data; 
    ret = data.Create(KMaxFileName);
    if(ret != KErrNone)
        {
        RDebug::Print(_L("RTestServer::Connect() Could not create buffer for data to be passed to process [%d]"), ret);
        return ret;
        }
    data.Format(_L("%S %S"), &aModuleName, &semaphoreName);
    RDebug::Print(_L("RTestServer::Connect() Data for new process prepared [%S]"), &data);
    

    // Indication is Create() operation needed
    TBool doCreate( ETrue );

    #ifdef __WINS__ // CodeWarrior(emulator)
        // Create without path(Uses Symbian default path).
        // Note: If TestScriter used then module name is format:
        // testscripter_testcasefilename
        
        ret = pr.Create( aModuleName, data );
        if( ret == KErrNone )
            {
            RDebug::Print(_L("RTestServer::Connect() Combination (1): Caps modifier [%S], Data [%S]"), &aModuleName, &data);
            doCreate = EFalse;
            }
    #endif // __WINS__

    if ( doCreate )
        {
        TInt find_ret( KErrNone );
        TInt trapd_ret( KErrNone );
        // Check is TestScripter and is caps modifíer name given
        if( aModuleName.Find( KTestScripterName ) != KErrNotFound && aConfigFile != KNullDesC )
            {
            TInt ret_caps;
            TFileName capsModifierName;
            // In TestClass's test case (config) file can give name to caps
            // modifier module.
            ret_caps = GetCapsModifier( aConfigFile, capsModifierName );
            if( ret_caps != KErrNone )
                {
                RDebug::Print( _L( "RTestServer::Connect() Caps modifier was not defined. Default modifier will be used" ) );
                }
            else
                {
                #ifdef __WINS__ // CodeWarrior(emulator)
                    // Create without path(Uses Symbian default path)
                    ret = pr.Create( capsModifierName, data );
                    if( ret == KErrNone )
                        {
                        RDebug::Print( _L( "RTestServer::Connect() Combination (2): Caps modifier [%S], Data [%S]"), &capsModifierName, &data);
                        doCreate = EFalse;
                        }
                #endif // __WINS__
                if ( doCreate )
                    {
                    TRAP( trapd_ret, find_ret = FindExeL(
                                    capsModifierName, pathAndExeModule ) );
                    }
                }
            }
        //--PYTHON-- begin
        // If this is python module, use caps modifier
        else if(aModuleName.Length() >= KPythonScripterLength && aModuleName.Mid(0, KPythonScripterLength).Compare(KPythonScripter) == 0)
           {
           pathAndExeModule.Copy(KPythonScripter);
           }
        //--PYTHON-- end
        // Find exe from file system
        else if( aModuleName != _L( "testcombiner" ) )
           {
           // Check if e.g. TestModule.exe is available
           TRAP( trapd_ret, find_ret = FindExeL( aModuleName, pathAndExeModule  ) );
           }

        // Check FindExeL error codes
        if( find_ret != KErrNone )
			{
			// Module was not found, using default exe: testserverstarter.exe
			RDebug::Print( _L( "RTestServer::Connect() Correct caps modifier module not found, capabilities cannot set" ) );
			if ( aModuleName.Find( _L( "testscripter_ui_" ) ) == 0 ) 
				{
				pathAndExeModule.Copy( KDefaultUiExeName );    	
				}
			else
				{
				pathAndExeModule.Copy( KDefaultExeName );    	
				}           
			}
        if( trapd_ret != KErrNone )
           {
           // FindExeL fails
           RDebug::Print( _L( "RTestServer::Connect() Caps modifier module searching fails with error: %d" ), trapd_ret );
           startSemaphore.Close();
           data.Close();
           return trapd_ret;
           }

        }

    if ( doCreate )
        {
        ret = pr.Create( pathAndExeModule, data );
        RDebug::Print(
            _L( "RTestServer::Connect() Combination (3): Caps modifier [%S], Data [%S], Result [%d]" ),
            &pathAndExeModule, &data, ret );
        }

    data.Close();
    
    if ( ret != KErrNone )
        {
        startSemaphore.Close();  
        return ret;
        }

    // Process created, continue start-up
    pr.Resume();
    pr.Close();

    // Wait until server is started
    startSemaphore.Wait();
    startSemaphore.Close();     // Close semaphore

    serverName = aModuleName;

    // Server is up and running, connect to it    
    ret = CreateSession( serverName, Version() );    
    return ret;

    }


/*
-------------------------------------------------------------------------------

    Class: RTestServer

    Method: Version

    Description: Return client side version.

    Parameters: None

    Return Values: TVersion: Version number

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TVersion RTestServer::Version() const
    {

    return( TVersion( KTestServerMajorVersionNumber,
                       KTestServerMinorVersionNumber, 
                       KTestServerBuildVersionNumber 
                     ) );

    }


/*
-------------------------------------------------------------------------------

    Class: RTestServer

    Method: Close

    Description: Closes the RTestServer session.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C void RTestServer::Close()
    {

    // Check that server is connected
    if( Handle() != 0 )
        {
        TIpcArgs args( TIpcArgs::ENothing, TIpcArgs::ENothing, TIpcArgs::ENothing );
        SendReceive( ETestServerCloseSession, args );
        }

    RSessionBase::Close();

    }
 /*
-------------------------------------------------------------------------------

    Class: RTestServer

    Method: GetServerThreadId

    Description: Get server thread id

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestServer::GetServerThreadId(TThreadId& aThreadId)
    {

    TInt ret(0);

    // Check that server is connected
    if( Handle() != 0 )
        {
        TThreadId id;
        TPckg<TThreadId> threadIdPckg( id );
        TIpcArgs args( &threadIdPckg, TIpcArgs::ENothing, TIpcArgs::ENothing );
        ret = SendReceive( ETestServerGetServerThreadId, args );  
        
        aThreadId = threadIdPckg();        
        }
        
    return ret;

    }            

/*
-------------------------------------------------------------------------------

    Class: RTestServer

    Method: FindExeL

    Description: Find exe(CapsModifier) from file system(Used in EKA2)

    Parameters: TFileName aModuleName: in: Module name.
                TFileName& aPathAndExeModule: inout: Module's exe(CapsModifier)
                path information.

    Return Values: TBool: Symbian error code: If exe(CapsModifier) is found
                          return KErrNone else other error code will return.

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TBool RTestServer::FindExeL( TFileName aModuleName,
                                TFileName& aPathAndExeModule )
    {
    RFs fsSession;  // For the file session handling
    RFile file;

    // Connect to file server
	User::LeaveIfError( fsSession.Connect() ); // Start session

    TDriveList drivelist; 
	User::LeaveIfError( fsSession.DriveList(drivelist) );
	// A TDriveList (the list of available drives), is an array of 
	// 26 bytes. Each byte with a non zero value signifies that the 
	// corresponding drive is available.
    TInt driveNumber; 
	TChar driveLetter;
    
	for( driveNumber=EDriveA; driveNumber<=EDriveZ; driveNumber++ )
		{
		if( !drivelist[driveNumber] ) 
			{
			// If drive-list entry is zero, drive is not available
			continue;
			}
        User::LeaveIfError(fsSession.DriveToChar(driveNumber,driveLetter));

        aPathAndExeModule.Zero();
        aPathAndExeModule.Append( driveLetter );

        aPathAndExeModule.Append( _L(":\\sys\\bin\\") );

        aPathAndExeModule.Append( aModuleName );
        aPathAndExeModule.Append( _L( ".exe" ) );

        TInt found( KErrNone );
        found = file.Open( fsSession, aPathAndExeModule, EFileRead );
        if( found == KErrNone )
            {
            // exe(capsmodifier module) is founded.
            file.Close();
            fsSession.Close();
            return KErrNone;
            }
        }

    // If exe not found use testserverstarter.exe but capabilities cannot
    // set.
    if ( aModuleName.Find( _L( "testscripter_ui_" ) ) == 0 ) 
    	{
        aPathAndExeModule.Copy( KDefaultUiExeName );    	
    	}
    else
    	{
        aPathAndExeModule.Copy( KDefaultExeName );    	
    	}	
    file.Close();
    fsSession.Close();
 
    return KErrNotFound;
    
    }

/*
-------------------------------------------------------------------------------

    Class: RTestServer

    Method: GetCapsModifierName

    Description: Get caps modifier module name from TestScripter's test
                 case(config) file.

    Parameters: const TDesC& aConfigFile: in: TestScripter's test case(config)
                file path and name.
                TFileName& aCapsModifierName: inout: Parsed caps modifier name.

    Return Values: TInt: Symbian error code: Return KErrNone if caps modifier
                                             module name is successfully parsed
                                             or given else other error code
                                             will return.

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
TInt RTestServer::GetCapsModifier( const TDesC& aConfigFile,
                                        TFileName& aCapsModifierName )
    {
    // Create parser
    CStifParser* parser = NULL;
    TRAPD( err, parser = CStifParser::NewL( _L(""), aConfigFile ) );
    if( err != KErrNone )
        {
        return err;
        }

    CleanupStack::PushL( parser );

    // Create section
    CStifSectionParser* section = NULL;
    TRAP( err, section = parser->SectionL( KStifSettingsStartTag,
                                            KStifSettingsEndTag ) );
    if( err != KErrNone || section == NULL)
        {
        CleanupStack::PopAndDestroy( parser );
        return KErrNotFound;
        }

    CleanupStack::PushL( section );

    // Create item
    CStifItemParser* item = NULL;
    TRAP( err, item = section->GetItemLineL( KCapsModifier ) );
    if( err != KErrNone || item == NULL )
        {
        CleanupStack::PopAndDestroy( section );
        CleanupStack::PopAndDestroy( parser );
        return KErrNotFound;
        }
    CleanupStack::PushL( item );

    // Get caps modifier module name
    TPtrC capsModifierName;
    err = item->GetString( KCapsModifier, capsModifierName );
    if( err != KErrNone )
        {
        CleanupStack::PopAndDestroy( item );
        CleanupStack::PopAndDestroy( section );
        CleanupStack::PopAndDestroy( parser );
        return err;
        }

    aCapsModifierName.Copy( capsModifierName );

    // Remove optional '.EXE' from the name
    aCapsModifierName.LowerCase();
    TParse parse;
    parse.Set( aCapsModifierName, NULL, NULL );

    if ( parse.Ext() == _L(".exe") )
        {
        const TInt len = parse.Ext().Length();
        aCapsModifierName.Delete ( aCapsModifierName.Length()-len, len );
        }

    CleanupStack::PopAndDestroy( item );
    CleanupStack::PopAndDestroy( section );
    CleanupStack::PopAndDestroy( parser );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: Open

    Description: Create new subsession with the RTestServer server

    Parameters: RTestServer& aServer          :inout: Server
                TFileName& aIniFile           :in:    Initialization file

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestModule::Open( RTestServer& aServer,
                                 TFileName& aIniFile
                               )
    {
    TIpcArgs args( &aIniFile, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return CreateSubSession( aServer, ETestModuleCreateSubSession, args );
    }


/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: Close

    Description: Close a subsession

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestModule::Close()
    {

    RSubSessionBase::CloseSubSession( ETestModuleCloseSubSession );

    }


/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: EnumerateTestCases

    Description: Enumerates test cases

    Parameters: TFileName aConfigFile         :in:  Configuration file name
                TCaseSize& aCount             :out: Package for storing count
                TRequestStatus& aStatus       :out: Status

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void RTestModule::EnumerateTestCases( TDesC& aConfigFile,
                                               TCaseSize& aCount,
                                               TRequestStatus& aStatus
                                             )
    {
    TIpcArgs args( &aConfigFile, &aCount, TIpcArgs::ENothing );
    SendReceive( ETestModuleEnumerateTestCases, args, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: GetTestCases

    Description: Get test cases

    Parameters: CFixedFlatArray<TTestCaseInfo>& aTestCaseBuffer :Out:    Cases

    Return Values: TInt                             Return value from operation

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestModule::GetTestCases( CFixedFlatArray<TTestCaseInfo>& aTestCaseBuffer )
    {
    TIpcArgs args( &aTestCaseBuffer.Des(), TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestModuleGetTestCases, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: ErrorNotification

    Description: Request error notification

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C void RTestModule::ErrorNotification( TErrorNotificationPckg& aError,                                           
                                              TRequestStatus& aStatus )
    {
    TIpcArgs args( &aError, TIpcArgs::ENothing, TIpcArgs::ENothing );
    SendReceive( ETestModuleErrorNotification, args, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: CancelAsyncRequest

    Description: Cancel ongoing request

    Parameters: TInt aReqToCancel             :in:  Request to be cancelled

    Return Values: TInt                             Return value from operation

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestModule::CancelAsyncRequest( TInt aReqToCancel )
    {
    TIpcArgs args( aReqToCancel, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestModuleCancelAsyncRequest, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: Open

    Description: create new subsession with the RTestModule

    Parameters: RTestServer& aServer          :in:  Handle to server
                const TInt aCaseNumber        :in:  Case number
                const TFileName& aConfig      :in:  Configuration file

    Return Values: TInt                             Return value from operation

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::Open( RTestServer& aServer,
                                    const TInt aCaseNumber,
                                    const TFileName& aConfig
                                  )
    {
    TIpcArgs args( aCaseNumber, &aConfig, TIpcArgs::ENothing );
    return CreateSubSession( aServer, ETestExecutionCreateSubSession, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: Close

    Description: close a subsession

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C void RTestExecution::Close()
    {

    // Close subsession
    RSubSessionBase::CloseSubSession( ETestExecutionCloseSubSession );

    }




/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: RunTestCase

    Description: Runs a test case

    Parameters: TFullTestResultPckg& aResult  :out: Case result
                TRequestStatus& aStatus       :out: Request to be completed

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C void RTestExecution::RunTestCase( TFullTestResultPckg& aResult,
                                           TRequestStatus& aStatus 
                                         )
    {
    TIpcArgs args( &aResult, TIpcArgs::ENothing, TIpcArgs::ENothing );
    SendReceive( ETestExecutionRunTestCase, args, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: RunTestCase

    Description: Runs a test case

    Parameters: TFullTestResultPckg& aResult  :out: Case result
                const TDesC& aTestCaseArgs: Test case arguments
                TRequestStatus& aStatus       :out: Request to be completed

    Return Values: None

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C void RTestExecution::RunTestCase( TFullTestResultPckg& aResult,
                                           const TDesC& aTestCaseArgs,
                                           TRequestStatus& aStatus 
                                         )
    {
    TIpcArgs args( &aResult, &aTestCaseArgs, TIpcArgs::ENothing );
    SendReceive( ETestExecutionRunTestCase, args, aStatus );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: NotifyProgress

    Description: Notify about test case progress, i.e test case prints.

    Parameters: TTestProgressPckg& aProgress  :out: Print info
                TRequestStatus& aStatus       :out: Request to be completed

    Return Values: TInt                             Always KErrNone

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::NotifyProgress( TTestProgressPckg& aProgress,
                                              TRequestStatus& aStatus 
                                            )
    {
    TIpcArgs args( &aProgress, TIpcArgs::ENothing, TIpcArgs::ENothing );
    SendReceive( ETestExecutionNotifyProgress, args, aStatus );
    return KErrNone;
    }


/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: Pause

    Description: Pauses a test case

    Parameters: None

    Return Values: TInt                             Return value from operation

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::Pause()
    {
    TIpcArgs args( TIpcArgs::ENothing, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestExecutionPause, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: Resume

    Description: Resumes a test case

    Parameters: None

    Return Values: TInt                             Return value from operation

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::Resume()
    {
    TIpcArgs args( TIpcArgs::ENothing, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestExecutionResume, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: CancelAsyncRequest

    Description: Cancels an asynchronous request.

    Parameters: TInt aReqToCancel             :in:  Request to be cancelled.

    Return Values: TInt                             Return value from operation

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::CancelAsyncRequest( TInt aReqToCancel )
    {
    TIpcArgs args( aReqToCancel, TIpcArgs::ENothing, TIpcArgs::ENothing );
    return SendReceive( ETestExecutionCancelAsyncRequest, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: NotifyEvent

    Description: NotifyEvent is used to receive event requests
                 from the Test module.

    Parameters: TEventIfPckg& aEvent          :inout:  Event package.
                TRequestStatus& aStatus       :out:    Request to be completed
                TInt aError                   :in:     Error from event system

    Return Values: TInt                                Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::NotifyEvent( TEventIfPckg& aEvent,
                                           TRequestStatus& aStatus,
                                           TInt aError )
    {
    TIpcArgs args( &aEvent, aError, TIpcArgs::ENothing );
    SendReceive( ETestExecutionNotifyEvent, args, aStatus );
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: RTestModule

    Method: NotifyRemoteCmd

    Description: NotifyRemoteCmd is used to receive RemoteCmd requests
                 from the Test module.

    Parameters: TRemoteCmdPckg& aRemoteCommand: nout: Remote command's packege.
                TRemoteCmdType aType: in: Remote type.
                TRequestStatus& aStatus: inout: Request to be completed

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::NotifyRemoteCmd( TStifCommandPckg& aRemoteCommand,
                                               TPckg<TInt>& aMsgSizePckg,
                                               TRequestStatus& aStatus )
    {
// Construct message
    TIpcArgs args( &aRemoteCommand, &aMsgSizePckg, TIpcArgs::ENothing );
    SendReceive( ETestExecutionNotifyRemoteCmd, args, aStatus );
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: ReadRemoteCmdInfo

    Description: ReadRemoteCmdInfo is used to receive RemoteCmd requests
                 from the Test module.

    Parameters: TDesC& aMsg :inout: message.
                TRemoteCommand aType :in: Remote command's type
                TInt aError: in: Symbian error code

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::ReadRemoteCmdInfo( TDes8& aMsg,
                                                 TStifCommand aType,
                                                 TInt aError )
    {
    TInt value = 0;
    if( aError != KErrNone )
        {
        value = aError;
        }
    else
        {
        value = aMsg.Length();
        }

// Construct message
    TIpcArgs args( &aMsg, aType, value );
     // Synchronous method so return a value
    return SendReceive( ETestExecutionReadRemoteCmdInfo, args );
    }
    
#if 0
/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: ReadRemoteCmdInfo

    Description: ReadRemoteCmdInfo is used to receive RemoteCmd requests
                 from the Test module.

    Parameters: TRebootParamsPckg& aRemoteType :inout: Remote type.
                TRemoteCommand aType :in: Remote command's type
                TInt aError: in: Symbian error code

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::ReadRemoteCmdInfo( 
                                            TRebootParamsPckg& aRemoteType,
                                            TRemoteCommand aType,
                                            TInt aError )
    {
// Construct message
    TIpcArgs args( &aRemoteType, aType, aError );
    // Synchronous method so return a value
    return SendReceive( ETestExecutionReadRemoteCmdInfo, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: ReadRemoteCmdInfo

    Description: ReadRemoteCmdInfo is used to receive RemoteCmd requests
                 from the Test module.

    Parameters: TRebootStateParamsPckg& aRemoteState :inout: Remote state.
                TRemoteCommand aType :in: Remote command's type
                TInt aError: in: Symbian error code

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::ReadRemoteCmdInfo( 
                                        TRebootStateParamsPckg& aRemoteState,
                                        TRemoteCommand aType,
                                        TInt aError )
    {
    // Construct message
    TIpcArgs args( &aRemoteState, aType, aError );
    // Synchronous method so return a value
    return SendReceive( ETestExecutionReadRemoteCmdInfo, args );
    }

/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: ReadRemoteCmdInfo

    Description: ReadRemoteCmdInfo is used to receive RemoteCmd requests
                 from the Test module.

    Parameters: TGetRebootStoredParamsPckg& aRemoteStoredState :inout: Stored
                state.
                TRemoteCommand aType :in: Remote command's type
                TInt aError: in: Symbian error code

    Return Values: TInt: Always KErrNone

    Errors/Exceptions: None

    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::ReadRemoteCmdInfo( 
                                TGetRebootStoredParamsPckg& aRemoteStoredState,
                                TRemoteCommand aType,
                                TInt aError )
    {
    // Construct message
    TIpcArgs args( &aRemoteStoredState, aType, aError );
    // Synchronous method so return a value
    return SendReceive( ETestExecutionReadRemoteCmdInfo, args );
    }
#endif // 0

/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: NotifyCommand

    Description: NotifyCommand is used to receive command requests
                 from the Test module. DEPRECATED !!
                 Use NotifyCommand2 instead.

    Parameters: aStifCommandPckg:     command
                aParam1Pckg:          parameter of command
                aTestCaseHandlePckg:  handle to test case
                aStatus:              request status
                aError:               error

    Return Values: TInt               Always KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::NotifyCommand(TCommandPckg& /*aCommandPckg*/,
                                            TBuf8<KMaxCommandParamsLength>& /*aParamsPckg*/,
                                            TRequestStatus& /*aStatus*/,
                                            TInt /*aError*/)
    {
    /*TIpcArgs args(&aCommandPckg, &aParamsPckg, TIpcArgs::ENothing);
    SendReceive(ETestExecutionNotifyCommand, args, aStatus);
    return KErrNone;*/
    return KErrNotSupported;
    }

/*
-------------------------------------------------------------------------------

    Class: RTestExecution

    Method: NotifyCommand2

    Description: NotifyCommand is used to receive command requests
                 from the Test module.

    Parameters: aStifCommandPckg:     command
                aParam1Pckg:          parameter of command
                aTestCaseHandlePckg:  handle to test case
                aStatus:              request status
                aError:               error

    Return Values: TInt               Always KErrNone

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt RTestExecution::NotifyCommand2(TCommandPckg& aCommandPckg,
                                            TDes8& aParamsPckg,
                                            TRequestStatus& aStatus,
                                            TInt /*aError*/)
    {
    TIpcArgs args(&aCommandPckg, &aParamsPckg, TIpcArgs::ENothing);
    SendReceive(ETestExecutionNotifyCommand, args, aStatus);
    return KErrNone;
    }

// End of File
