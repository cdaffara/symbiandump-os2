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
* Description:  Implements the functions to control STIF Test Framework
*                through the STIF TF interface.
*
*/



// INCLUDE FILES
#include <badesca.h>
#include <bautils.h>
#include <charconv.h>
#include <e32cons.h>
#include <HtiLogging.h>
#include "HtiStifTfIf.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const static TUid KStifTfServiceUid = { 0x10210CC3 };

enum TStifTfCommands
    {
    ECmdOpen            = 0x01,
    ECmdClose           = 0x02,
    ECmdLoadModule      = 0x03,
    ECmdUnloadModule    = 0x04,
    ECmdListCases       = 0x05,
    ECmdStartCase       = 0x06,
    ECmdCancelCase      = 0x07,
    ECmdPauseCase       = 0x08,
    ECmdResumeCase      = 0x09,
    ECmdAddCaseFile     = 0x0A,
    ECmdRemoveCaseFile  = 0x0B,
    ECmdCaseMsg         = 0x0C,
    ECmdSetDevId        = 0x0D,
    ECmdTestCompleted   = 0x0E,
    ECmdAtsMessage      = 0x0F,
    ECmdSetAttribute    = 0x10
    };

_LIT8( KParameterDelimiter, "|" );

// NOTE: Max length for error description is defined
// in HtiDispatcherInterface.h (currently 118).
_LIT8( KErrorStifTfNotOpen, "ERROR: STIF TF is not open" );
_LIT8( KErrorOpen, "ERROR: Failed to open STIF TF" );
_LIT8( KErrorAlreadyOpen, "ERROR: STIF TF is already open" );
_LIT8( KErrorClose, "ERROR: Failed to close STIF TF" );
_LIT8( KErrorUnknownCmd, "ERROR: Unknown STIF TF command" );
_LIT8( KErrorNoCmd, "ERROR: No command given" );
_LIT8( KErrorParameterMissing, "ERROR: Mandatory parameter missing" );
_LIT8( KErrorParameterTooLong, "ERROR: Parameter too long" );
_LIT8( KErrorTooManyParameters, "ERROR: Too many parameters" );
_LIT8( KErrorNoMemory, "ERROR: Memory allocation failure" );
_LIT8( KErrorLoadModule, "ERROR: Load Module failed" );
_LIT8( KErrorUnloadModule, "ERROR: Unload Module failed" );
_LIT8( KErrorInvalidCaseNumber, "ERROR: Invalid test case number parameter" );
_LIT8( KErrorModuleNotLoaded, "ERROR: Given test module is not loaded" );
_LIT8( KErrorGetTestCases, "ERROR: Failed to get test cases from module" );
_LIT8( KErrorGetTestCaseInfos, "ERROR: Failed to get test case infos" );
_LIT8( KErrorStartTestCase, "ERROR: Failed to start test case" );
_LIT8( KErrorCancelTestCase, "ERROR: Failed to cancel test case" );
_LIT8( KErrorPauseTestCase, "ERROR: Failed to pause test case" );
_LIT8( KErrorResumeTestCase, "ERROR: Failed to resume test case" );
_LIT8( KErrorInvalidDeviceId, "ERROR: Invalid device ID parameter" );
_LIT8( KErrorAddCaseFile, "ERROR: Add Case File failed" );
_LIT8( KErrorRemoveCaseFile, "ERROR: Remove Case File failed" );
_LIT8( KErrorFileServer, "ERROR: Failed to connect to file server" );
_LIT8( KErrorCaseFileNotFound, "ERROR: Given test case file was not found" );
_LIT8( KErrorIniFileNotFound, "ERROR: Given ini-file was not found" );
_LIT8( KErrorCaseMessage, "ERROR: Test case message sending failed" );
_LIT8( KErrorCharConv, "ERROR: Character conversion failed" );
_LIT8( KErrorInvalidAttributeId, "ERROR: Invalid attribute id parameter" );
_LIT8( KErrorSetAttribute, "ERROR: Failed to set attribute" );
_LIT8( KErrorUnknown, "ERROR: Unspecified error occured" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiStifTfIf::CHtiStifTfIf
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHtiStifTfIf::CHtiStifTfIf():iIsBusy( EFalse ),
                             iMessage( NULL ),
                             iErrorCode( 0 ),
                             iStifTfOpen( EFalse ),
                             iDevId( KDefaultDevId ),
                             iConsole( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CHtiStifTfIf::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::ConstructL( CHtiStifTfServicePlugin* aPlugin )
    {
    iCommandId = 0;
    iParameters = new (ELeave) CDesC8ArrayFlat( 5 );
    iPlugin = aPlugin;
    }

// -----------------------------------------------------------------------------
// CHtiStifTfIf::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHtiStifTfIf* CHtiStifTfIf::NewL( CHtiStifTfServicePlugin* aPlugin )
    {
    CHtiStifTfIf* self = new( ELeave ) CHtiStifTfIf;
    CleanupStack::PushL( self );
    self->ConstructL( aPlugin );
    CleanupStack::Pop();
    return self;
    }


// Destructor
CHtiStifTfIf::~CHtiStifTfIf()
    {
    if ( iParameters )
        {
        iParameters->Reset();
        }
    delete iParameters;
    iLoadedInfos.ResetAndDestroy();
    iLoadedInfos.Close();
    delete iMessage;
    if ( iStifTfOpen ) Close();
    iPlugin = NULL;  // Not owned, so not deleted.
    iConsole = NULL; // Not owned, so not deleted.
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::ProcessMessageL
// Interpretes the message and calls appropriate command handling method.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority /*aPriority*/)
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::ProcessMessageL" );
    HTI_LOG_FORMAT( "Message length = %d", aMessage.Length() );

    if ( iIsBusy )
        {
        HTI_LOG_TEXT( "Plugin is busy - leaving" );
        User::Leave( KErrInUse );
        }

    iIsBusy = ETrue;

    if ( aMessage.Length() < 1 )
        {
        User::LeaveIfError(
            SendErrorResponseMsg( KErrArgument, KErrorNoCmd ) );
        return;
        }

    iCommandId = aMessage[0];
    HTI_LOG_FORMAT( "Command = %d", iCommandId );

    TRAPD( err, ParseParametersL( aMessage ) );

    if ( err == KErrNoMemory )
        {
        User::LeaveIfError( SendErrorResponseMsg( err, KErrorNoMemory ) );
        return;
        }

    else if ( err )
        {
        User::LeaveIfError(
            SendErrorResponseMsg( KErrGeneral, KErrorUnknown ) );
        return;
        }

    // If STIF TF is not open, any other command than OPEN returns KErrNotReady
    if ( !iStifTfOpen )
        {
        if ( iCommandId == ECmdOpen )
            {
            HandleOpenCmdL();
            return;
            }
        else
            {
            User::LeaveIfError(
                SendErrorResponseMsg( KErrNotReady, KErrorStifTfNotOpen ) );
            return;
            }
        }


    // Go through the rest of the commands
    if ( iCommandId == ECmdLoadModule )
        {
        HandleLoadModuleCmdL();
        }

    else if ( iCommandId == ECmdUnloadModule )
        {
        HandleUnloadModuleCmdL();
        }

    else if ( iCommandId == ECmdListCases )
        {
        HandleListCasesCmdL();
        }

    else if ( iCommandId == ECmdStartCase )
        {
        HandleStartCaseCmdL();
        }

    else if ( iCommandId == ECmdCancelCase )
        {
        HandleCancelCaseCmdL();
        }

    else if ( iCommandId == ECmdPauseCase )
        {
        HandlePauseCaseCmdL();
        }

    else if ( iCommandId == ECmdResumeCase )
        {
        HandleResumeCaseCmdL();
        }

    else if ( iCommandId == ECmdAddCaseFile )
        {
        HandleAddCaseFileCmdL();
        }

    else if ( iCommandId == ECmdRemoveCaseFile )
        {
        HandleRemoveCaseFileCmdL();
        }

    else if ( iCommandId == ECmdCaseMsg )
        {
        HandleCaseMsgCmdL();
        }

    else if ( iCommandId == ECmdSetDevId )
        {
        HandleSetDevIdCmdL();
        }

    else if ( iCommandId == ECmdSetAttribute )
        {
        HandleSetAttributeCmdL();
        }

    else if ( iCommandId == ECmdClose )
        {
        HandleCloseCmdL();
        }

    else if ( iCommandId == ECmdOpen )
        {
        User::LeaveIfError(
            SendErrorResponseMsg( KErrArgument, KErrorAlreadyOpen ) );
        }

    else
        {
        User::LeaveIfError(
            SendErrorResponseMsg( KErrArgument, KErrorUnknownCmd ) );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::ProcessMessageL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleOpenCmdL
// Handles opening of the STIF Test Framework.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleOpenCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleOpenCmdL" );

    TInt err = KErrNone;

    // OPEN command has 1 optional parameter (test framework ini-file)
    if ( iParameters->Count() == 0 )
        {
        err = Open();
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorOpen );
            return;
            }

        iStifTfOpen = ETrue;
        SendResponseMsg( _L8( "OK" ) );
        }

    else if ( iParameters->Count() == 1 )
        {
        TBuf<KMaxFileName> iniFilePath;
        if ( (*iParameters)[0].Length() > iniFilePath.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }

        iniFilePath.Copy( (*iParameters)[0] );

        RFs fsSession;
        err = fsSession.Connect();
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorFileServer );
            return;
            }

        if ( !( BaflUtils::FileExists( fsSession, iniFilePath ) ) )
            {
            SendErrorResponseMsg( KErrNotFound, KErrorIniFileNotFound );
            fsSession.Close();
            return;
            }

        fsSession.Close();

        err = Open( iniFilePath );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorOpen );
            return;
            }

        // Load the test case infos of all currently loaded test
        // modules (modules listed in the given ini-file).
        TRAP( err, LoadTestCaseInfosL() );
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorGetTestCaseInfos );
            return;
            }

        iStifTfOpen = ETrue;
        SendResponseMsg( _L8( "OK" ) );
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleOpenCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleCloseCmdL
// Closes the STIF Test Framework.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleCloseCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleCloseCmdL" );

    TInt err = KErrNone;

    // CLOSE command has no parameters
    if ( iParameters->Count() == 0 )
        {
        err = Close();
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorClose );
            return;
            }

        iStifTfOpen = EFalse;
        iLoadedInfos.ResetAndDestroy(); // empty the cache of test case infos
        SendResponseMsg( _L8( "OK" ) );
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleCloseCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleLoadModuleCmdL
// Loads a STIF TF test module (optionally with ini-file).
// Also calls the LoadTestCaseInfosL helper method to load the test case info
// objects from the test module.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleLoadModuleCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleLoadModuleCmdL" );

    TInt err = KErrNone;

    TBuf<KMaxFileName> moduleName;
    TBuf<KMaxFileName> iniFilePath;

    // LOAD_MODULE command has 1 mandatory parameter (module name)
    // and 1 optional parameter (ini-file path).
    if ( iParameters->Count() == 0 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 1 )
        {
        if ( (*iParameters)[0].Length() > moduleName.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }
        moduleName.Copy( (*iParameters)[0] );
        err = AddTestModule( moduleName );
        }

    else if ( iParameters->Count() == 2 )
        {
        if ( (*iParameters)[0].Length() > moduleName.MaxLength() ||
             (*iParameters)[1].Length() > iniFilePath.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }

        moduleName.Copy( (*iParameters)[0] );
        iniFilePath.Copy( (*iParameters)[1] );

        RFs fsSession;
        err = fsSession.Connect();
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorFileServer );
            return;
            }

        if ( !( BaflUtils::FileExists( fsSession, iniFilePath ) ) )
            {
            SendErrorResponseMsg( KErrNotFound, KErrorIniFileNotFound );
            fsSession.Close();
            return;
            }

        fsSession.Close();

        err = AddTestModule( moduleName, iniFilePath );
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        return;
        }

    if ( err )
        {
        HTI_LOG_FORMAT( "LoadModule: Error %d", err );
        SendErrorResponseMsg( err, KErrorLoadModule );
        }

    else
        {
        TRAP( err, LoadTestCaseInfosL( moduleName ) );

        if ( err )
            {
            HTI_LOG_FORMAT(
                "LoadModule - Error from LoadTestCaseInfosL %d", err );
            RemoveTestModule( moduleName );
            SendErrorResponseMsg( err, KErrorGetTestCases );
            return;
            }

        // Sends the name of the loaded module as a response.
        SendResponseMsg( (*iParameters)[0] );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleLoadModuleCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleUnloadModuleCmdL
// Unloads a STIF TF test module.
// Also removes and deletes the corresponding CHtiTestModuleCaseInfos object
// from iLoadedInfos pointer array.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleUnloadModuleCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleUnloadModuleCmdL" );

    TInt err = KErrNone;

    TBuf<KMaxFileName> moduleName;

    // UNLOAD_MODULE command has always 1 parameter (module name).
    if ( iParameters->Count() == 0 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    if ( iParameters->Count() == 1 )
        {
        if ( (*iParameters)[0].Length() > moduleName.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }
        moduleName.Copy( (*iParameters)[0] );
        err = RemoveTestModule( moduleName );
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        return;
        }

    if ( err )
        {
        HTI_LOG_FORMAT( "UnloadModule: Error %d", err );
        SendErrorResponseMsg( err, KErrorUnloadModule );
        }

    else
        {
        // Remove the test case infos of the unloaded module.
        TInt moduleIndex = IndexByModuleName( moduleName );
        if ( moduleIndex != KErrNotFound )
            {
            CHtiTestModuleCaseInfos* module = iLoadedInfos[moduleIndex];
            iLoadedInfos.Remove( moduleIndex );
            delete module;
            }

        // Sends the name of the unloaded module as a response.
        SendResponseMsg( (*iParameters)[0] );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleUnloadModuleCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleListCasesCmdL
// Creates a list of titles of the test cases inside the given test module.
// Output is a string of test case titles separated with the | character.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleListCasesCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleListCasesCmdL" );

    TInt err = KErrNone;

    TBuf<KMaxFileName> moduleName;
    TBuf8<128> buf8;  // temporary buffer for case titles

    CHtiTestModuleCaseInfos* module = NULL;
    CTestInfo* caseInfo = NULL;

    // LIST_CASES command has 1 optional parameter (module name).

    // If no parameter given all cases of all currently loaded
    // modules are listed.
    if ( iParameters->Count() == 0 )
        {
        /*
        // (Re)load case infos from all loaded modules
        TRAP( err, LoadTestCaseInfosL() );

        if ( err )
            {
            HTI_LOG_FORMAT(
                "ListCases - Error from LoadTestCaseInfosL %d", err );
            SendErrorResponseMsg( err, KErrorGetTestCaseInfos );
            }
        */
        CBufFlat* caseListBuf = NULL;
        TRAP( err, caseListBuf = CBufFlat::NewL( 128 ) );
        if ( err )
            {
            SendErrorResponseMsg( KErrNoMemory, KErrorNoMemory );
            return;
            }

        // Loop all loaded test modules
        TInt pos = 0;
        for ( TInt i = 0; i < iLoadedInfos.Count(); i++ )
            {
            module = NULL;
            module = iLoadedInfos[i];
            HTI_LOG_FORMAT( "Listing cases from module %S",
                module->iModuleName );

            // Loop all case infos in this module
            for ( TInt j = 0; j < module->iTestInfos.Count(); j++ )
                {
                caseInfo = NULL;
                caseInfo = module->iTestInfos[j];

                if ( !( i == 0 && j == 0 ) )
                    {
                    buf8.Append( KParameterDelimiter );
                    }

                buf8.Append( caseInfo->TestCaseTitle() );

                TRAP( err, caseListBuf->ExpandL( pos, buf8.Length() ) );
                if ( err )
                    {
                    SendErrorResponseMsg( KErrNoMemory, KErrorNoMemory );
                    delete caseListBuf;
                    return;
                    }
                caseListBuf->Write( pos, buf8, buf8.Length() );
                pos += buf8.Length();
                buf8.Zero();
                }
            }

        if ( caseListBuf->Size() > 0 )
            {
            TPtr8 ptr = caseListBuf->Ptr( 0 );
            SendResponseMsg( ptr );
            }

        else
            {
            SendResponseMsg( buf8 );
            }

        delete caseListBuf;
        }

    else if ( iParameters->Count() == 1 )
        {
        if ( (*iParameters)[0].Length() > moduleName.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }

        moduleName.Copy( (*iParameters)[0] );
        TInt moduleIndex = IndexByModuleName( moduleName );

        if ( moduleIndex == KErrNotFound )
            {
            HTI_LOG_TEXT( "Module is not loaded" );
            SendErrorResponseMsg( KErrNotFound, KErrorModuleNotLoaded );
            return;
            }
        /*
        // (Re)load the case infos of given module
        TRAP( err, LoadTestCaseInfosL( moduleName ) );
        if ( err )
            {
            HTI_LOG_FORMAT(
                "ListCases - Error from LoadTestCaseInfosL %d", err );
            SendErrorResponseMsg( err, KErrorGetTestCaseInfos );
            return;
            }
        */
        module = iLoadedInfos[moduleIndex];
        CBufFlat* caseListBuf = NULL;

        TRAP( err, caseListBuf = CBufFlat::NewL( 128 ) );

        if ( err )
            {
            SendErrorResponseMsg( KErrNoMemory, KErrorNoMemory );
            return;
            }

        TInt pos = 0;
        for ( TInt i = 0; i < module->iTestInfos.Count(); i++ )
            {
            caseInfo = NULL;
            caseInfo = module->iTestInfos[i];
            buf8.Copy( caseInfo->TestCaseTitle() );
            if ( !( i == module->iTestInfos.Count() - 1 ) )
                {
                buf8.Append( KParameterDelimiter );
                }
            TRAP( err, caseListBuf->ExpandL( pos, buf8.Length() ) );
            if ( err )
                {
                SendErrorResponseMsg( KErrNoMemory, KErrorNoMemory );
                delete caseListBuf;
                return;
                }
            caseListBuf->Write( pos, buf8, buf8.Length() );
            pos += buf8.Length();
            buf8.Zero();
            }

        if ( module->iTestInfos.Count() > 0 )
            {
            TPtr8 ptr = caseListBuf->Ptr( 0 );
            SendResponseMsg( ptr );
            }

        else
            {
            SendResponseMsg( buf8 );
            }

        delete caseListBuf;
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleListCasesCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleStartCaseCmdL
// Starts one test case based on test module name and test case index number.
// STIF TF gives an ID to the started case and that ID is sent as a response
// message.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleStartCaseCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleStartCaseCmdL" );

    TInt err = KErrNone;
    TInt caseNumber = -1;

    // START_CASE command has 1 or 2 parameters.
    // If only one parameter given, it's the index of test case to start. The
    // index refers to the list of test cases of all currently loaded modules.
    // If two parameters given, then the first one is the module name and
    // second is the test case index number inside the given module,

    if ( iParameters->Count() == 0 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 1 )
        {
        TLex8 lex( (*iParameters)[0] );
        err = lex.Val( caseNumber );
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorInvalidCaseNumber );
            return;
            }

        CTestInfo* caseInfo = NULL;
        TRAP( err, caseInfo = GetCaseInfoL( caseNumber ) );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorInvalidCaseNumber );
            return;
            }

        TInt caseId = -1;

        err = StartTestCase( caseId, *caseInfo );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorStartTestCase );
            return;
            }

        TBuf8<32> msg;
        msg.AppendNum( caseId );
        SendResponseMsg( msg );
        }

    else if ( iParameters->Count() == 2 )
        {
        TBuf<KMaxFileName> moduleName;
        if ( (*iParameters)[0].Length() > moduleName.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }

        moduleName.Copy( (*iParameters)[0] );
        TLex8 lex( (*iParameters)[1] );
        err = lex.Val( caseNumber );
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorInvalidCaseNumber );
            return;
            }

        TInt moduleIndex = IndexByModuleName( moduleName );

        if ( moduleIndex == KErrNotFound )
            {
            // We don't have case infos of given module loaded - try to
            // load them.
            TRAP( err, LoadTestCaseInfosL( moduleName ) );
            if ( err )
                {
                HTI_LOG_FORMAT(
                    "StartCase - Error from LoadTestCaseInfosL %d", err );
                SendErrorResponseMsg( err, KErrorModuleNotLoaded );
                return;
                }
            // Now there should be the module added - get its index
            moduleIndex = IndexByModuleName( moduleName );
            }

        CHtiTestModuleCaseInfos* module = iLoadedInfos[moduleIndex];

        if ( caseNumber < 0 ||
             caseNumber >= module->iTestInfos.Count() ||
             module->iTestInfos.Count() == 0 )
            {
            SendErrorResponseMsg( KErrArgument, KErrorInvalidCaseNumber );
            return;
            }

        CTestInfo* caseInfo = module->iTestInfos[caseNumber];
        TInt caseId = -1;

        err = StartTestCase( caseId, *caseInfo );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorStartTestCase );
            return;
            }

        TBuf8<32> msg;
        msg.AppendNum( caseId );
        SendResponseMsg( msg );
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleStartCaseCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleCancelCaseCmdL
// Cancels test case execution based on test case ID.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleCancelCaseCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleCancelCaseCmdL" );

    TInt err = KErrNone;

    // CANCEL_CASE command always has 1 parameter (test case id).
    if ( iParameters->Count() == 0 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 1 )
        {
        TInt caseId = -1;
        TLex8 lex( (*iParameters)[0] );
        err = lex.Val( caseId );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorInvalidCaseNumber );
            return;
            }

        err = CancelTestCase( caseId );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorCancelTestCase );
            }

        else
            {
            SendResponseMsg( _L8( "OK" ) );
            }
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleCancelCaseCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandlePauseCaseCmdL
// Pauses test case execution based on test case ID.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandlePauseCaseCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandlePauseCaseCmdL" );

    TInt err = KErrNone;

    // PAUSE_CASE command always has 1 parameter (test case id).
    if ( iParameters->Count() == 0 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 1 )
        {
        TInt caseId = -1;
        TLex8 lex( (*iParameters)[0] );
        err = lex.Val( caseId );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorInvalidCaseNumber );
            return;
            }

        err = PauseTestCase( caseId );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorPauseTestCase );
            }

        else
            {
            SendResponseMsg( _L8( "OK" ) );
            }
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandlePauseCaseCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleResumeCaseCmdL
// Resumes execution of a paused test case based on test case ID.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleResumeCaseCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleResumeCaseCmdL" );

    TInt err = KErrNone;

    // RESUME_CASE command always has 1 parameter (test case id).
    if ( iParameters->Count() == 0 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 1 )
        {
        TInt caseId = -1;
        TLex8 lex( (*iParameters)[0] );
        err = lex.Val( caseId );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorInvalidCaseNumber );
            return;
            }

        err = ResumeTestCase( caseId );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorResumeTestCase );
            }

        else
            {
            SendResponseMsg( _L8( "OK" ) );
            }
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleResumeCaseCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleAddCaseFileCmdL
// Adds a test case file to an earlier loaded test module.
// Checks that the module is loaded and that the given test case file exists.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleAddCaseFileCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleAddCaseFileCmdL" );

    TInt err = KErrNone;

    TBuf<KMaxFileName> moduleName;
    TBuf<KMaxFileName> caseFilePath;

    // ADD_CASE_FILE command always has 2 mandatory parameters (module name
    // and test case file path).
    if ( iParameters->Count() == 0 || iParameters->Count() == 1 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 2 )
        {
        if ( (*iParameters)[0].Length() > moduleName.MaxLength() ||
             (*iParameters)[1].Length() > caseFilePath.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }

        moduleName.Copy( (*iParameters)[0] );
        caseFilePath.Copy( (*iParameters)[1] );

        RFs fsSession;
        err = fsSession.Connect();
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorFileServer );
            return;
            }

        if ( !( BaflUtils::FileExists( fsSession, caseFilePath ) ) )
            {
            SendErrorResponseMsg( KErrNotFound, KErrorCaseFileNotFound );
            fsSession.Close();
            return;
            }

        fsSession.Close();

        err = AddTestCaseFile( moduleName, caseFilePath );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorAddCaseFile );
            return;
            }

        // Test case file has been added - update test case infos
        TRAP( err, LoadTestCaseInfosL( moduleName ) );

        if ( err )
            {
            HTI_LOG_FORMAT(
                "AddCaseFile - Error from LoadTestCaseInfosL %d", err );
            SendErrorResponseMsg( err, KErrorGetTestCases );
            return;
            }

        SendResponseMsg( _L8( "OK" ) );
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleAddCaseFileCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleRemoveCaseFileCmdL
// Removes a test case file from test module.
// Checks that the module is loaded and that the given test case file exists.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleRemoveCaseFileCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleRemoveCaseFileCmdL" );

    TInt err = KErrNone;

    TBuf<KMaxFileName> moduleName;
    TBuf<KMaxFileName> caseFilePath;

    // REMOVE_CASE_FILE command always has 2 mandatory parameters (module name
    // and test case file path).
    if ( iParameters->Count() == 0 || iParameters->Count() == 1 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 2 )
        {
        if ( (*iParameters)[0].Length() > moduleName.MaxLength() ||
             (*iParameters)[1].Length() > caseFilePath.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }

        moduleName.Copy( (*iParameters)[0] );
        caseFilePath.Copy( (*iParameters)[1] );

        RFs fsSession;
        err = fsSession.Connect();
        if ( err )
            {
            SendErrorResponseMsg( err, KErrorFileServer );
            return;
            }

        if ( !( BaflUtils::FileExists( fsSession, caseFilePath ) ) )
            {
            SendErrorResponseMsg( KErrNotFound, KErrorCaseFileNotFound );
            fsSession.Close();
            return;
            }

        fsSession.Close();

        err = RemoveTestCaseFile( moduleName, caseFilePath );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorRemoveCaseFile );
            return;
            }

        // Test case file has been removed - update test case infos
        TRAP( err, LoadTestCaseInfosL( moduleName ) );

        if ( err )
            {
            HTI_LOG_FORMAT(
                "RemoveCaseFile - Error from LoadTestCaseInfosL %d", err );
            SendErrorResponseMsg( err, KErrorGetTestCases );
            return;
            }

        SendResponseMsg( _L8( "OK" ) );
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleRemoveCaseFileCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleCaseMsgCmdL
// Sends a message to STIF Test Framework.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleCaseMsgCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleCaseMsgCmdL" );

    TInt err = KErrNone;

    // CASE_MESSAGE command always has 1 parameter (the message).
    if ( iParameters->Count() == 0 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 1 )
        {
        HBufC* msg = HBufC::New( (*iParameters)[0].Length() );
        if ( msg == NULL )
            {
            SendErrorResponseMsg( KErrNoMemory, KErrorNoMemory );
            return;
            }
        msg->Des().Copy( (*iParameters)[0] );
        HTI_LOG_TEXT( "Sending case message:" );
        HTI_LOG_DES( *msg );
        err = AtsReceive( *msg );
        delete msg;

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorCaseMessage );
            }

        else
            {
            SendResponseMsg( _L8( "OK" ) );
            }
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleCaseMsgCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleSetDevIdCmdL
// Sets device ID. If successfull sends the set ID back as response message.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleSetDevIdCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleSetDevIdCmdL" );

    TInt err = KErrNone;

    // SET_DEVICEID command always has 1 parameter (device id).
    if ( iParameters->Count() == 0 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        return;
        }

    else if ( iParameters->Count() == 1 )
        {
        TUint16 deviceId = KDefaultDevId;
        TLex8 lex( (*iParameters)[0] );
        err = lex.Val( deviceId, EDecimal );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorInvalidDeviceId );
            return;
            }

        iDevId = deviceId;

        // On success sends the set device id as response.
        TBuf8<32> msg;
        msg.AppendNum( (TInt)iDevId );
        SendResponseMsg( msg );
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleSetDevIdCmdL" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::HandleSetAttributeCmdL
// Sets attributes for test framework.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::HandleSetAttributeCmdL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::HandleSetAttributeCmdL" );

    TInt err = KErrNone;

    // SET_ATTRIBUTE command always has 2 paramters (attribute id and value)
    if ( iParameters->Count() < 2 )
        {
        SendErrorResponseMsg( KErrArgument, KErrorParameterMissing );
        }

    else if ( iParameters->Count() == 2 )
        {
        TInt attributeId = -1;
        TLex8 lex( (*iParameters)[0] );
        err = lex.Val( attributeId );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorInvalidAttributeId );
            return;
            }

        TBuf<256> value;
        if ( (*iParameters)[1].Length() > value.MaxLength() )
            {
            SendErrorResponseMsg( KErrArgument, KErrorParameterTooLong );
            return;
            }
        value.Copy( (*iParameters)[1] );

        err = SetAttribute( (TAttribute)attributeId, value );

        if ( err )
            {
            SendErrorResponseMsg( err, KErrorSetAttribute );
            }

        else
            {
            SendResponseMsg( _L8( "OK" ) );
            }
        }

    else
        {
        SendErrorResponseMsg( KErrArgument, KErrorTooManyParameters );
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::HandleSetAttributeCmdL" );
    }

// -----------------------------------------------------------------------------
// CHtiStifTfIf::TestCompleted
// This method is called by the STIF TF when a test case execution completes.
// The test case result is send out as a formatted string.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::TestCompleted( TInt aTestId,
                                  TInt aTestExecutionResult,
                                  TTestResult& aTestResult )
    {
    // Note that the STIF TF is not aware whether the service is busy processing,
    // sending, or waiting to send a previous message. So if this method
    // is called by the STIF TF while the service is busy, the currently
    // pending message will be overridden by the message sent from this method.

    HTI_LOG_FUNC_IN( "CHtiStifTfIf::TestCompleted" );

    iIsBusy = ETrue;  // Will be set to EFalse when response have been sent

    // The maximum length of TResultDes is 128, so a buffer of 256
    // should be enough for the whole "Test Completed" message.
    TBuf8<256> buf8;

    // Add first the test id
    buf8.AppendFormat( _L8( "%d" ), aTestId );
    buf8.Append( KParameterDelimiter );

    // Add the test result - if it's 0 case has passed
    buf8.AppendFormat( _L8( "%d" ), aTestResult.iResult );
    buf8.Append( KParameterDelimiter );

    // Add the test execution result
    buf8.AppendFormat( _L8( "%d" ), aTestExecutionResult );
    buf8.Append( KParameterDelimiter );

    // Finally add the description (if there's any)
    if ( aTestResult.iResultDes.Length() > 0 )
        {
        buf8.Append( aTestResult.iResultDes );
        }

    // and send it
    SendResponseMsg( buf8, ECmdTestCompleted );
    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::TestCompleted" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::AtsSend
// This method is called by the STIF TF when it needs to send a message to ATS
// (or some other PC-side component listening for messages)
// The message is converted to 8-bit representation before sending.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::AtsSend( const TDesC& aMessage )
    {
    // Note that the STIF TF is not aware whether the service is busy processing,
    // sending, or waiting to send a previous message. So if this method
    // is called by the STIF TF while the service is busy, the currently
    // pending message will be overridden by the message sent from this method.

    HTI_LOG_FUNC_IN( "CHtiStifTfIf::AtsSend" );
    HTI_LOG_TEXT( "Message: " );
    HTI_LOG_DES( aMessage );

    iIsBusy = ETrue;  // Will be set to EFalse when response have been sent

    TInt err = KErrNone;

    CCnvCharacterSetConverter* converter = NULL;
    TRAP( err, converter = CCnvCharacterSetConverter::NewL() );

    if ( err )
        {
        SendErrorResponseMsg( err, KErrorCharConv );
        return;
        }

    RFs fsSession;
    err = fsSession.Connect();
    if ( err )
        {
        SendErrorResponseMsg( err, KErrorFileServer );
        delete converter;
        return;
        }

    CCnvCharacterSetConverter::TAvailability availability =
        CCnvCharacterSetConverter::ENotAvailable;

    TRAP( err, availability = converter->PrepareToConvertToOrFromL(
        KCharacterSetIdentifierAscii, fsSession ) );

    if ( err || availability == CCnvCharacterSetConverter::ENotAvailable )
        {
        SendErrorResponseMsg( err, KErrorCharConv );
        fsSession.Close();
        delete converter;
        return;
        }

    fsSession.Close();

    HBufC8* asciiBuffer = NULL;
    TRAP( err, asciiBuffer = HBufC8::NewL( aMessage.Length() ) );

    if ( err )
        {
        SendErrorResponseMsg( err, KErrorNoMemory );
        delete converter;
        return;
        }

    TPtr8 ptr8 = asciiBuffer->Des();
    ptr8.Zero();

    err = converter->ConvertFromUnicode( ptr8, aMessage );

    if ( err )
        {
        SendErrorResponseMsg( err, KErrorCharConv );
        }

    else
        {
        TPtr8 converted = asciiBuffer->Des();
        HTI_LOG_TEXT( "Converted message: " );
        HTI_LOG_DES( converted );
        SendResponseMsg( converted, ECmdAtsMessage );
        }

    delete converter;
    delete asciiBuffer;
    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::AtsSend" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::Print
// This method is called by the STIF TF when a test case prints.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::Print( TInt /*aTestId*/,
                          TTestProgress& aProgress )
    {
    if ( iConsole )
        {
        iConsole->Printf( _L("%S: %S\n"),
                         &aProgress.iDescription,
                         &aProgress.iText );
        }
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::LoadTestCaseInfosL
// A helper method that loads the test case info objects from all currently
// loaded test modules. Creates and inserts new CHtiTestModuleCaseInfos objects
// to iLoadedInfos pointer array. Leaves on failure.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::LoadTestCaseInfosL()
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::LoadTestCaseInfosL()" );

    RPointerArray<CTestInfo> allTestInfos;
    CleanupClosePushL( allTestInfos );

    HTI_LOG_TEXT( "Getting test cases" );
    // Ignoring possible errors
    GetTestCases( allTestInfos );
    HTI_LOG_FORMAT( "Got %d cases", allTestInfos.Count() );

    // Clear all old test case infos from iLoadedInfos
    iLoadedInfos.ResetAndDestroy();

    // Loop through all test case infos and add them to the correct modules
    // to iLoadedInfos
    for ( TInt i = 0; i < allTestInfos.Count(); i++ )
        {
        CHtiTestModuleCaseInfos* currentModule = NULL;
        CTestInfo* caseInfo = NULL;

        caseInfo = allTestInfos[i];

        // Get or create the CHtiTestModuleCaseInfos where this case belongs.
        TInt moduleIndex = IndexByModuleName( caseInfo->ModuleName() );

        if ( moduleIndex == KErrNotFound )
            {
            HTI_LOG_TEXT( "Module not found - adding it" );
            currentModule = CHtiTestModuleCaseInfos::NewLC(
                caseInfo->ModuleName() );
            User::LeaveIfError( iLoadedInfos.Append( currentModule ) );
            CleanupStack::Pop(); // currentModule
            }

        else
            {
            HTI_LOG_TEXT( "Module found - getting it" );
            currentModule = iLoadedInfos[moduleIndex];
            }

        // Add the case info to the CHtiTestModuleCaseInfos object
        currentModule->iTestInfos.Append( caseInfo );
        }

    HTI_LOG_FORMAT( "Case infos of %d modules added", iLoadedInfos.Count() );
    allTestInfos.Reset();
    CleanupStack::PopAndDestroy(); // allTestInfos
    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::LoadTestCaseInfosL()" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::LoadTestCaseInfosL
// A helper method that loads the test case info objects from the given test
// module. Creates and inserts a new CHtiTestModuleCaseInfos object to
// iLoadedInfos pointer array. Leaves on failure.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::LoadTestCaseInfosL( TDesC& aModuleName )
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::LoadTestCaseInfosL( moduleName )" );

    // Check if test case infos of this module already loaded
    TInt index = IndexByModuleName( aModuleName );
    if ( index != KErrNotFound )
        {
        // If infos already loaded, delete and load again
        HTI_LOG_TEXT( "Already loaded - updating" );
        CHtiTestModuleCaseInfos* module = iLoadedInfos[index];
        iLoadedInfos.Remove( index );
        delete module;
        }

    CHtiTestModuleCaseInfos* newModule =
            CHtiTestModuleCaseInfos::NewLC( aModuleName );

    HTI_LOG_TEXT( "Getting test cases" );

    // If the module is not loaded STIF TF seems to behave so that
    // GetTestCases method does not return error but it returns
    // zero test cases.
    // At least in some cases if the module is loaded but it does not have
    // any test cases, GetTestCases returns KErrNotFound.
    // So possible errors or zero test case situations from GetTestCases
    // are ignored here - if there's an error then the module will just
    // have 0 test case infos.
    GetTestCases( newModule->iTestInfos, aModuleName );

    HTI_LOG_FORMAT( "Got %d cases", newModule->iTestInfos.Count() );

    User::LeaveIfError( iLoadedInfos.Append( newModule ) );
    CleanupStack::Pop(); // newModule

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::LoadTestCaseInfosL( moduleName )" );
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::ParseParametersL
// A helper method for parsing the parameters from the given command message.
// Parameters must be separated with a character defined by the
// KParameterDelimiter. Parsed parameters are stored to iParameters array.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::ParseParametersL( const TDesC8& aCommand )
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::ParseParametersL" );

    // Clear the parameter array
    iParameters->Reset();

    // Then loop all the parameters.
    TInt sectionStart = 0;
    TInt sectionEnd = 0;
    while ( true )
        {
        sectionStart = sectionEnd + 1;
        if ( sectionStart >= aCommand.Length() )
            {
            HTI_LOG_FORMAT( "Parameters %d", iParameters->Count() );
            HTI_LOG_FUNC_OUT( "CHtiStifTfIf::ParseParametersL" );
            return;
            }

        sectionEnd = aCommand.Mid( sectionStart ).Find( KParameterDelimiter )
            + sectionStart;

        if ( sectionEnd >= sectionStart )
            {
            iParameters->AppendL(
                aCommand.Mid( sectionStart, sectionEnd - sectionStart ) );
            HTI_LOG_DES( aCommand.Mid( sectionStart, sectionEnd - sectionStart ) );
            }
        else
            {
            iParameters->AppendL( aCommand.Mid( sectionStart ) );
            HTI_LOG_DES( aCommand.Mid( sectionStart ) );
            HTI_LOG_FORMAT( "Parameters %d", iParameters->Count() );
            HTI_LOG_FUNC_OUT( "CHtiStifTfIf::ParseParametersL" );
            return;  // End of parameters.
            }
        }
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::SetDispatcher
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    iDispatcher = aDispatcher;
    iConsole = iDispatcher->GetConsole();
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::NotifyMemoryChange
// Called when HTI Framework has dispatched a message forward and the amount
// of free memory in the message queue has changed.
// -----------------------------------------------------------------------------
//
void CHtiStifTfIf::NotifyMemoryChange( TInt aAvailableMemory )
    {
    if ( iIsBusy && iMessage )
        {
        if ( aAvailableMemory >= iMessage->Size() )
            {

            if ( iErrorCode == 0 )
                {
                TInt err = iDispatcher->DispatchOutgoingMessage(
                    iMessage, KStifTfServiceUid );

                if ( err == KErrNone )
                    {
                    // Ownership of iMessage has been transferred
                    iMessage = NULL;
                    iIsBusy = EFalse;
                    iDispatcher->RemoveMemoryObserver(
                            ( MHtiMemoryObserver* ) iPlugin );
                    }

                else if ( err == KErrNoMemory )
                    {
                    // Just keep retrying.
                    }

                else // Give up on sending
                    {
                    delete iMessage;
                    iMessage = NULL;
                    iIsBusy = EFalse;
                    iDispatcher->RemoveMemoryObserver(
                            ( MHtiMemoryObserver* ) iPlugin );
                    }

                }

            else
                {
                TInt err = iDispatcher->DispatchOutgoingErrorMessage(
                    iErrorCode, *iMessage, KStifTfServiceUid );

                // If it was success or some other error than KErrNoMemory
                // we are done sending or trying to send this message.
                if ( err != KErrNoMemory )
                    {
                    delete iMessage;
                    iMessage = NULL;
                    iIsBusy = EFalse;
                    iDispatcher->RemoveMemoryObserver(
                            ( MHtiMemoryObserver* ) iPlugin );
                    }

                else
                    {
                    // Just keep retrying.
                    }
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::SendResponseMsg
// Sends a message out to the message dispatcher.
// -----------------------------------------------------------------------------
//
TInt CHtiStifTfIf::SendResponseMsg( const TDesC8& aMsg,
                        const TUint8 aCommandId )
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::SendResponseMsg" );

    iErrorCode = 0;

    if ( iDispatcher == NULL )
        {
        iIsBusy = EFalse;
        return KErrGeneral;
        }

    iDispatcher->RemoveMemoryObserver( ( MHtiMemoryObserver* ) iPlugin );

    delete iMessage;
    iMessage = NULL;
    iMessage = HBufC8::New( aMsg.Length() + 1 );


    if ( iMessage == NULL )
        {
        iIsBusy = EFalse;
        return KErrNoMemory;
        }

    TPtr8 ptr8 = iMessage->Des();
    if ( aCommandId != 0 )
        {
        ptr8.Append( aCommandId );
        }
    else
        {
        ptr8.Append( iCommandId );
        }

    ptr8.Append( aMsg );

    TInt err = KErrNone;

    err = iDispatcher->DispatchOutgoingMessage( iMessage, KStifTfServiceUid );

    if ( err == KErrNoMemory )
        {
        HTI_LOG_TEXT( "Message queue memory full - waiting" );
        iIsBusy = ETrue; // Should already be true, but just in case
        iDispatcher->AddMemoryObserver( ( MHtiMemoryObserver* ) iPlugin );
        // For the caller of this method all is OK, sending is just delayed
        err = KErrNone;
        }

    else if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "Message sent to dispatcher" );
        iMessage = NULL; // Ownership of iMessage has been transferred
        iIsBusy = EFalse;
        }

    else // give up on sending
        {
        HTI_LOG_FORMAT( "Other dispatcher error %d", err );
        delete iMessage;
        iMessage = NULL;
        iIsBusy = EFalse;
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::SendResponseMsg" );
    return err;
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::SendErrorResponseMsg
// Sends an error message out to the message dispatcher.
// -----------------------------------------------------------------------------
//
TInt CHtiStifTfIf::SendErrorResponseMsg( TInt aErrorCode,
                        const TDesC8& aErrorDescription,
                        const TUint8 aCommandId )
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::SendErrorResponseMsg" );

    iErrorCode = aErrorCode;

    if ( iDispatcher == NULL )
        {
        iIsBusy = EFalse;
        return KErrGeneral;
        }

    iDispatcher->RemoveMemoryObserver( ( MHtiMemoryObserver* ) iPlugin );

    delete iMessage;
    iMessage = NULL;
    iMessage = HBufC8::New( aErrorDescription.Length() + 1 );

    if ( iMessage == NULL )
        {
        iIsBusy = EFalse;
        return KErrNoMemory;
        }

    TPtr8 ptr8 = iMessage->Des();
    if ( aCommandId != 0 )
        {
        ptr8.Append( aCommandId );
        }
    else
        {
        ptr8.Append( iCommandId );
        }

    ptr8.Append( aErrorDescription );

    TInt err = KErrNone;

    err = iDispatcher->DispatchOutgoingErrorMessage(
        aErrorCode, *iMessage, KStifTfServiceUid );

    if ( err == KErrNoMemory )
        {
        HTI_LOG_TEXT( "Message queue memory full - waiting" );
        iIsBusy = ETrue; // Should already be true, but just in case
        iDispatcher->AddMemoryObserver( ( MHtiMemoryObserver* ) iPlugin );
        // For the caller of this method all is OK, sending is just delayed
        err = KErrNone;
        }

    else if ( err == KErrNone )
        {
        HTI_LOG_TEXT( "Error message sent to dispatcher" );
        delete iMessage;
        iMessage = NULL;
        iIsBusy = EFalse;
        }

    else // give up on sending
        {
        HTI_LOG_FORMAT( "Other dispatcher error %d", err );
        delete iMessage;
        iMessage = NULL;
        iIsBusy = EFalse;
        }

    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::SendErrorResponseMsg" );
    return err;
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::IndexByModuleName
// A helper method to search for a CHtiTestModuleCaseInfos object from the
// iLoadedInfos array based on the module name.
// -----------------------------------------------------------------------------
//
TInt CHtiStifTfIf::IndexByModuleName( const TDesC& aModuleName )
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::IndexByModuleName" );

    CHtiTestModuleCaseInfos* module;
    for ( TInt i = 0; i < iLoadedInfos.Count(); i++ )
        {
        module = NULL;
        module = iLoadedInfos[i];
        if ( module->iModuleName->Compare( aModuleName ) == 0 )
            {
            HTI_LOG_FORMAT( "Module found from index %d", i );
            HTI_LOG_FUNC_OUT( "CHtiStifTfIf::IndexByModuleName" );
            return i;
            }
        }
    HTI_LOG_TEXT( "Module not found" );
    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::IndexByModuleName" );
    return KErrNotFound;
    }


// -----------------------------------------------------------------------------
// CHtiStifTfIf::GetCaseInfoL
// A helper method to get the correct CTestInfo object based on the index
// number, referring to list of case infos of all loaded test modules.
// -----------------------------------------------------------------------------
//
CTestInfo* CHtiStifTfIf::GetCaseInfoL( TInt aCaseIndex )
    {
    HTI_LOG_FUNC_IN( "CHtiStifTfIf::GetCaseInfoL" );

    if ( aCaseIndex < 0 )
        {
        User::Leave( KErrArgument );
        }

    TInt index = -1;
    TInt caseCount = 0;
    CHtiTestModuleCaseInfos* module = NULL;
    for ( TInt i = 0; i < iLoadedInfos.Count(); i++ )
        {
        module = NULL;
        module = iLoadedInfos[i];
        caseCount += module->iTestInfos.Count();
        HTI_LOG_FORMAT( "Case count = %d", caseCount );
        if ( aCaseIndex < caseCount )
            {
            // This is the right module
            // Get the case index inside this module
            index = module->iTestInfos.Count() - ( caseCount - aCaseIndex );
            }
        if ( index >= 0 ) break; // Case found - break out of loop
        }

    if ( index < 0 )
        {
        HTI_LOG_TEXT( "Given index was too big - case not found" );
        User::Leave( KErrNotFound );
        }

    // Following should never be true - this is for testing.
    if ( index > module->iTestInfos.Count() )
        {
        HTI_LOG_TEXT( "**Calculated index > test info count of module**" );
        User::Leave( KErrGeneral );
        }

    HTI_LOG_FORMAT( "Case found from index %d", index );
    HTI_LOG_FUNC_OUT( "CHtiStifTfIf::GetCaseInfoL" );
    return module->iTestInfos[index];
    }


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CHtiTestModuleCaseInfos::NewL
// -----------------------------------------------------------------------------
//
CHtiTestModuleCaseInfos* CHtiTestModuleCaseInfos::NewL(
                                                const TDesC& aModuleName )
    {
    CHtiTestModuleCaseInfos* self = NewLC( aModuleName );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CHtiTestModuleCaseInfos::NewLC
// -----------------------------------------------------------------------------
//
CHtiTestModuleCaseInfos* CHtiTestModuleCaseInfos::NewLC(
                                                const TDesC& aModuleName )
    {
    CHtiTestModuleCaseInfos* self = new (ELeave) CHtiTestModuleCaseInfos;
    CleanupStack::PushL( self );
    self->ConstructL( aModuleName );
    return self;
    }


// -----------------------------------------------------------------------------
// CHtiTestModuleCaseInfos::ConstructL
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CHtiTestModuleCaseInfos::ConstructL( const TDesC& aModuleName )
    {
    iModuleName = aModuleName.AllocL();
    }


// -----------------------------------------------------------------------------
// CHtiTestModuleCaseInfos::CHtiTestModuleCaseInfos
// C++ default constructor
// -----------------------------------------------------------------------------
//
CHtiTestModuleCaseInfos::CHtiTestModuleCaseInfos()
    {
    }


// -----------------------------------------------------------------------------
// CHtiTestModuleCaseInfos::~CHtiTestModuleCaseInfos
// Destructor.
// -----------------------------------------------------------------------------
//
CHtiTestModuleCaseInfos::~CHtiTestModuleCaseInfos()
    {
    delete iModuleName;
    iTestInfos.ResetAndDestroy();
    iTestInfos.Close();
    }


// -----------------------------------------------------------------------------
// CHtiTestModuleCaseInfos::Compare
// Compares the order of two CHtiTestModuleCaseInfos objects using module name
// as the ordering key.
// -----------------------------------------------------------------------------
//
TInt CHtiTestModuleCaseInfos::Compare( const CHtiTestModuleCaseInfos& aFirst,
        const CHtiTestModuleCaseInfos& aSecond )
    {
    return ( aFirst.iModuleName )->Compare( *aSecond.iModuleName );
    }


// -----------------------------------------------------------------------------
// CHtiTestModuleCaseInfos::Match
// Checks the equality of two CHtiTestModuleCaseInfos objects.
// Objects are considered to be equal if their module name is equal.
// -----------------------------------------------------------------------------
//
TBool CHtiTestModuleCaseInfos::Match( const CHtiTestModuleCaseInfos& aFirst,
        const CHtiTestModuleCaseInfos& aSecond )
    {
    if ( ( aFirst.iModuleName )->Compare( *aSecond.iModuleName ) == 0 )
        {
        return true;
        }

    return false;
    }


//  End of File
