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
* Description: CStifTFwIf: This object executes test cases from 
* STIF Test Framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include "UIEngineEvent.h"
#include <stifinternal/UIEngineContainer.h>
#include "StifTFwIf.h"
#include "StifTFw.h"
#include "Logging.h"
#include "StifPython.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES
// None

// CONSTANTS
// None

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iStifTFw->iLogger

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS ======================================= 

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: CStifTFwIf

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C CStifTFwIf::CStifTFwIf()
    {
    __RDEBUG( _L( "CStifTFwIf::CStifTFwIf()" ) );
    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: ~CStifTFwIf

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C CStifTFwIf::~CStifTFwIf()
    {
    __RDEBUG( _L( "CStifTFwIf::~CStifTFwIf()" ) );
    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: Open

    Description: Open test engine.

    Parameters: TDesC& aTestFrameworkIni: in: Initialization file to Test Framework

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::Open( TDesC& aTestFrameworkIni )
    {
    RDebug::Print(_L("CStifTFwIf::Open"));
    TInt err( KErrNone );
    TRAP( err,
        iStifTFw = CStifTFw::NewL( this );
        )

    return iStifTFw->iUIEngine->Open( aTestFrameworkIni );

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: Close

    Description: Close test engine.

    Parameters: None

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::Close()
    {
    RDebug::Print(_L("CStifTFwIf::Close"));

    iStifTFw->DeleteData(); //Delete objects before closing session to TestEngine server
    iStifTFw->iUIEngine->Close();

    delete iStifTFw;
    iStifTFw = 0;

    return KErrNone;

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: SetAttribute

    Description: Used to set attributes for Test Framework.

    Parameters: TAttribute aAttribute: in: Attribute type
                TDesC& aValue: in: Attribute value

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::SetAttribute( TAttribute aAttribute,
                                        const TDesC& aValue )
    {
    RDebug::Print(_L("CStifTFwIf::SetAttribute"));
    // Check aValue max length
    if( aValue.Length() > KMaxName )
        {
        __TRACE( KPrint, ( CStifLogger::EError, _L( "CStifTFwIf::SetAttribute() method's second parameter length is incorrect" ) ) );
        return KErrArgument;
        }

    TInt ret = iStifTFw->iUIEngine->TestEngine().SetAttribute( aAttribute, aValue );

    if ( ret != KErrNone )
        {
        __TRACE( KPrint, ( _L( "CStifTFwIf::SetAttribute() method fails with error:%d" ), ret  ) );
        return ret;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: AddTestModule

    Description: Add test module to module list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which is added to module list
                TDesC& aIniFile: in: Initialization file to the test module

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::AddTestModule( TDesC& aModuleName,
                                         TDesC& aIniFile )
    {
    RDebug::Print(_L("CStifTFwIf::AddTestModule"));

    return iStifTFw->iUIEngine->AddTestModule( aModuleName, aIniFile );

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: RemoveTestModule

    Description: Add test module to module list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which is removed of module list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::RemoveTestModule( TDesC& aModuleName )
    {
    RDebug::Print(_L("CStifTFwIf::RemoveTestModule"));

    return iStifTFw->iUIEngine->RemoveTestModule( aModuleName );

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: AddTestCaseFile

    Description: Add test case file to test case file list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which own test cases of test case list.
                TDesC& aCaseFile: in: Test case list, which is added to test case list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::AddTestCaseFile( TDesC& aModuleName, TDesC& aCaseFile )
    {
    RDebug::Print(_L("CStifTFwIf::AddTestCaseFile"));

    return iStifTFw->iUIEngine->AddTestCaseFile( aModuleName, aCaseFile );

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: RemoveTestCaseFile

    Description: Remove test case file of test case file list of test engine

    Parameters: TDesC& aModuleName: in: Testmodule, which own test cases of test case list
                TDesC& aCaseFile: in: Test case list, which is removed of test case list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::RemoveTestCaseFile( TDesC& aModuleName, TDesC& aCaseFile )
    {
    RDebug::Print(_L("CStifTFwIf::RemoveTestCaseFile"));

    return iStifTFw->iUIEngine->RemoveTestCaseFile( aModuleName, aCaseFile );

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: GetTestCases

    Description: Get test cases of enumerated list of test engine. Method is copied
                 list of test cases to aTestCaseInfo. Copied tests are test cases
                 of test module (aTestModule) and defined
                 in test case file (aTestCaseFile), which are given as argument

                 Method copied all enumerated test cases if aTestModule
                 and aTestCaseFile are not defined

    Parameters: RPointerArray<CTestInfo>& aTestCaseInfo: inout: List of test cases
                TDesC& aModuleName: in: Testmodule, which own test cases of test case list
                TDesC& aTestCaseFile: in: Test case list, which is got of test case list

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::GetTestCases( RPointerArray<CTestInfo>& aTestCaseInfo,
                                        TDesC& aTestModule,
                                        TDesC& aTestCaseFile )
    {
    RDebug::Print(_L("CStifTFwIf::GetTestCases"));

    TRAPD( ret,
        ret = iStifTFw->iUIEngine->GetTestCasesL( aTestCaseInfo,
                                                  aTestModule,
                                                  aTestCaseFile );
        );

    return ret;

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: StartTestCase

    Description: Start test case execution.

    Parameters: TInt& aTestId: in: ID for test case. Test ID given of address of
                current CStifTFwIfContainer.

                const CTestInfo& aTestInfo: in: Test case information

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::StartTestCase( TInt& aTestId,
                                         const CTestInfo& aTestInfo )
    {
    RDebug::Print(_L("CStifTFwIf::StartTestCase"));

    CUIEngineContainer* container = NULL;

    TInt ret = iStifTFw->iUIEngine->StartTestCase( container,
                                                   aTestInfo );

    if( ret != KErrNone )
        {
        return ret;
        }

    // Add container to test case execution table
    ret = iStifTFw->iTestExecutionTable.Append( container );
    if( ret != KErrNone )
        {
        iStifTFw->iUIEngine->AbortStartedTestCase( container );
        return ret;
        }

    // Get test id.
    aTestId = (TInt)container;

    __TRACE( KPrint, (  _L( "aTestId: %d"), aTestId ) );
    __TRACE( KPrint, (  _L( "aTest case: %S"), &(aTestInfo.TestCaseTitle()) ) );

    return ret;

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: CancelTestCase

    Description: Cancel test case execution

    Parameters: TInt& aTestId: in: ID for test case

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::CancelTestCase( TInt aTestId )
    {
    RDebug::Print(_L("CStifTFwIf::CancelTestCase"));
    TInt ret(KErrNone);

    // Find test case from execution table
    TInt index = iStifTFw->iTestExecutionTable.Find( ( CUIEngineContainer* )aTestId );

    // Check if test id is found
    if( index < 0)
        {
        __TRACE( KPrint, (  _L( "CStifTFwIf::CancelTestCase. Container not found. TestId = %d"), aTestId ) );

        return index;
        }

    // Cancel test
    ret = iStifTFw->iTestExecutionTable[index]->CancelTest();
    __TRACE( KPrint, (  _L( "CStifTFwIf::CancelTestCase. Canceled testId %d"), aTestId ) );

    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: PauseTestCase

    Description: Pause test case execution

    Parameters: TInt& aTestId: in: ID for test case

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::PauseTestCase(  TInt aTestId  )
    {
    RDebug::Print(_L("CStifTFwIf::PauseTestCase"));
    TInt ret(KErrNone);

    // Find test case from execution table
    TInt index = iStifTFw->iTestExecutionTable.Find( ( CUIEngineContainer*) aTestId );

    // Check if test id is found
    if( index < 0 )
        {
        __TRACE( KPrint, (  _L( "CStifTFwIf::PauseTestCase. Container not found: testId = %d"), aTestId ) );

        return index;
        }

    // Pause test
    ret = iStifTFw->iTestExecutionTable[index]->PauseTest();

    __TRACE( KPrint, (  _L( "CStifTFwIf::PauseTest. Paused testId %d"), aTestId ) );
    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: ResumeTestCase

    Description: Resume test case execution

    Parameters: TInt& aTestId: in: ID for test case

    Return Values: Symbian OS error: Error code

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::ResumeTestCase( TInt aTestId )
    {
    RDebug::Print(_L("CStifTFwIf::ResumeTestCase"));
    TInt ret(KErrNone);

    // Find test case from execution table
    TInt index = iStifTFw->iTestExecutionTable.Find( ( CUIEngineContainer* ) aTestId );

    // Check if test id is found
    if( index < 0 )
        {
        __TRACE( KPrint, (  _L( "CStifTFwIf::ResumeTestCase. Container not found: TestId = %d"), aTestId ) );

        return index;
        }

    // Resume test
    ret = iStifTFw->iTestExecutionTable[index]->ResumeTest();

    __TRACE( KPrint, ( _L( "CStifTFwIf::ResumeTestCase. Resumed testId %d"), aTestId ) );

    return ret;
    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: AtsReceive

    Description: Handles remote protocol message receives

    Parameters: const TDesC& aMessage: in: Protocol message received

    Return Values: Symbian OS error code: In oom and is protocol message header
        parsing fails, i.e. error in <msgtype> <srcid> <dstid>

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFwIf::AtsReceive( const TDesC& aMessage )
    {
    RDebug::Print(_L("CStifTFwIf::AtsReceive"));

    CStifTFwIfProt* msg = NULL;
    CStifTFwIfProt* resp = NULL;
    TRAPD( err, msg = CStifTFwIfProt::NewL(); );
    if( err != KErrNone )
        {
        return err;
        }
    TRAP( err, resp = CStifTFwIfProt::NewL(); );
    if( err != KErrNone )
        {
        delete msg;
        return err;
        }
    TInt error = KErrNone;
    TInt ret = KErrNone;

    // Parse received message
    TRAP( err,
        error = msg->SetL( aMessage )
    );
    if( err != KErrNone )
        {
        __TRACE( KError, ( _L( "AtsReceive: message header parsing failed [%d]"), err ) );
        delete msg;
        delete resp;
        return err;
        }

    // Create response
    TRAP( err, resp->CreateL(); );
    if( err != KErrNone )
        {
        delete msg;
        delete resp;
        return err;
        }

    resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgResponse );

    if( error != KErrNone )
        {
        __TRACE( KError, ( _L( "AtsReceive: message parsing failed [%d]"), error ) );
        resp->AppendId( SETID( DevId(), 0 ) );
        resp->AppendId( msg->SrcId() );
        resp->Append( CStifTFwIfProt::MsgType, msg->iMsgType );
        resp->Append( CStifTFwIfProt::RespParam, CStifTFwIfProt::ERespResult, error );
        AtsSend( resp->Message() );

        // Error reported with protocol message, return success
        delete msg;
        delete resp;
        return KErrNone;
        }

    switch( msg->iMsgType )
        {
        case CStifTFwIfProt::EMsgReserve:
            {
            if( msg->iRemoteType != CStifTFwIfProt::ERemotePhone )
                {
                __TRACE( KError, ( _L( "AtsReceive: reserve for illegal type received") ) );
                error = KErrGeneral;
                }
            // Check IDs
            if( ( msg->SrcDevId() == 0 ) ||
                ( msg->SrcTestId() == 0 ) )
                {
                __TRACE( KError, ( _L( "AtsReceive: reserve for illegal srcid received") ) );
                error = KErrGeneral;
                }
            if( msg->DstId() != 0 )
                {
                // Not a broadcast
                if( ( msg->DstDevId() != DevId() ) ||
                    ( msg->DstTestId() != 0 ) )
                    {
                    __TRACE( KError, ( _L( "AtsReceive: reserve for illegal dstid received") ) );
                    error = KErrGeneral;
                    }
                }
            TInt index = iStifTFw->iMasterArray.Find( msg->SrcId() );
            if( index >= 0 )
                {
                error = KErrInUse;
                __TRACE( KError, ( _L( "AtsReceive: master has already reserved slave") ) );
                }
            else
                {
                iStifTFw->iMasterArray.Append( msg->SrcId() );
                }

            resp->AppendId( SETID( DevId(), 0 ) );
            resp->AppendId( msg->SrcId() );
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgReserve );
            if( error != KErrNone )
                {
                resp->Append( CStifTFwIfProt::RespParam, CStifTFwIfProt::ERespResult, error );
                }
            AtsSend( resp->Message() );

            }
            break;
        case CStifTFwIfProt::EMsgRelease:
            {
            // Check protocol ids
            if( ( msg->SrcDevId() == 0 ) ||
                ( msg->SrcTestId() == 0 ) ||
                ( msg->DstTestId() != 0 ) )
                {
                __TRACE( KError, ( _L( "AtsReceive: release for illegal srcid or dstid received") ) );
                error = KErrGeneral;
                }
            else if( msg->DstDevId() != DevId() )
                {
                __TRACE( KError, ( _L( "AtsReceive: release for illegal dstid received") ) );
                error = KErrNotFound;
                }
            else
                {
                // Check Master identifier
                // First check that specific master has reserved us
                TInt index = iStifTFw->iMasterArray.Find( msg->SrcId() );
                if( index < 0 )
                    {
                    error = KErrNotFound;
                    }
                else
                    {
                    // Search test case from execution table
                    TInt count = iStifTFw->iTestRemoteExecutionTable.Count();
                    for( TInt ind = 0; ind < count; ind++ )
                        {
                        if( iStifTFw->iTestRemoteExecutionTable[ind]->RemoteId() ==
                            msg->SrcId() )
                            {
                            __TRACE( KError, (  _L( "Release: test case running for %x"),
                                msg->SrcId() ) );
                            error = KErrInUse;
                            break;
                            }
                        }
                    if( error == KErrNone )
                        {
                        iStifTFw->iMasterArray.Remove( index );
                        }
                    }
                }

            resp->AppendId( msg->DstId() );
            resp->AppendId( msg->SrcId() );
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRelease );

            if( error != KErrNone )
                {
                resp->Append( CStifTFwIfProt::RespParam, CStifTFwIfProt::ERespResult, error );
                }
            AtsSend( resp->Message() );
            }
            break;
        case CStifTFwIfProt::EMsgRemote:
            {
            // Check protocol ids
            TInt index = iStifTFw->iMasterArray.Find( msg->SrcId() );
            if( ( msg->SrcDevId() == 0 ) ||
                ( msg->SrcTestId() == 0 ) ||
                ( msg->DstDevId() == 0 ) )
                {
                __TRACE( KError, ( _L( "AtsReceive: remote for illegal srcid or dstid received") ) );
                error = KErrGeneral;
                }
            else if( index < 0 )
                {
                __TRACE( KError, ( _L( "AtsReceive: remote for illegal srcid received") ) );
                error = KErrNotFound;
                }
            else
                {

                __TRACE( KError, ( _L( "AtsReceive: Call AtsRemote") ) );
                RDebug::Print(_L("CStifTFwIf::AtsReceive remote command received, calling AtsRemote"));
                error = iStifTFw->AtsRemote( *msg, *resp );
                RDebug::Print(_L("CStifTFwIf::AtsReceive remote command received, AtsRemote return"));
                }

            if( error != KErrNone )
                {
                resp->AppendId( msg->DstId() );
                resp->AppendId( msg->SrcId() );
                resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
                resp->Append( CStifTFwIfProt::CmdType, msg->iCmdType );
                resp->Append( CStifTFwIfProt::RespParam,
                             CStifTFwIfProt::ERespResult,
                             error );
                }
            AtsSend( resp->Message() );
            }
            break;
        case CStifTFwIfProt::EMsgResponse:
            {
            // Responses ae forwarded to master
            CUIEngineContainer* container = NULL;
            TInt count = iStifTFw->iTestRemoteExecutionTable.Count();
            TInt index = 0;
            for( index = 0 ; index < count; index++ )
                {
                if( iStifTFw->iTestRemoteExecutionTable[index]->MasterId() ==
                    msg->SrcId() )
                    {
                    __TRACE( KError, (  _L( "Response for %x"),
                        msg->SrcId() ) );
                    container = iStifTFw->iTestRemoteExecutionTable[index];
                    break;
                    }
                }
            if( container == NULL )
                {
                count = iStifTFw->iTestExecutionTable.Count();
                for( index = 0 ; index < count; index++ )
                    {
                    if( iStifTFw->iTestExecutionTable[index]->MasterId() ==
                        msg->DstId() )
                        {
                        __TRACE( KError, (  _L( "Response for %x"),
                            msg->SrcId() ) );
                        container = iStifTFw->iTestExecutionTable[index];
                        break;
                        }
                    }
                }

            if( container == NULL )
                {
                __TRACE( KError, ( _L( "AtsReceive: response to srcid %x that is not a running master"),
                    msg->SrcId() ) );
                ret = KErrNotFound;
                }
            else
                {
                ret = container->RemoteReceive( aMessage );
                }
            }
            break;
        default:
            __TRACE( KError, ( _L( "AtsReceive: invalid message")) );
            ret = KErrNotSupported;
        }

    delete msg;
    delete resp;
    return ret;

    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    CStifTFw: This object executes test cases from STIF Test Framework.

-------------------------------------------------------------------------------
*/

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iLogger

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: NewL

    Description: Construct the CStifTFw class

    Parameters: None

    Return Values: CStifTFw*                    New object

    Errors/Exceptions: Leaves if memory allocation fails or
                       ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CStifTFw* CStifTFw::NewL( CStifTFwIf* aStifTFwIf )
    {
    RDebug::Print(_L("CStifTFw::NewL"));

    CStifTFw* self = new ( ELeave ) CStifTFw( aStifTFwIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: ConstructL

    Description: Second level constructor.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves on error

    Status: Draft

-------------------------------------------------------------------------------
*/
void CStifTFw::ConstructL()
    {
    RDebug::Print(_L("CStifTFw::ConstructL"));

    iUIEngine = CUIEngine::NewL( (CUIIf*) this );

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: CStifTFw

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CStifTFw::CStifTFw( CStifTFwIf* aStifTFwIf ):
    iStifTFwIf( aStifTFwIf )
    {

    __RDEBUG( _L( "CStifTFw::CStifTFw()" ) );

    }


/*
-------------------------------------------------------------------------------

    Class: CStifTFwIf

    Method: ~CStifTFwIf

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CStifTFw::~CStifTFw()
    {
    __RDEBUG( _L( "CStifTFw::~CStifTFw() begin" ) );

    RDebug::Print(_L("CStifTFw::~CStifTFw resetting iTestExecutionTable cnt=%d"), iTestExecutionTable.Count());
    iTestExecutionTable.Reset();
    RDebug::Print(_L("CStifTFw::~CStifTFw resetting iTestRemoteExecutionTable cnt=%d"), iTestRemoteExecutionTable.Count());
    iTestRemoteExecutionTable.Reset();
    RDebug::Print(_L("CStifTFw::~CStifTFw resetting iEventArray cnt=%d"), iEventArray.Count());
    iEventArray.ResetAndDestroy();
    RDebug::Print(_L("CStifTFw::~CStifTFw resetting iMasterArray cnt=%d"), iMasterArray.Count());
    iMasterArray.Reset();

    RDebug::Print(_L("CStifTFw::~CStifTFw closing iTestExecutionTable cnt=%d"), iTestExecutionTable.Count());
    iTestExecutionTable.Close();
    RDebug::Print(_L("CStifTFw::~CStifTFw closing iTestRemoteExecutionTable cnt=%d"), iTestRemoteExecutionTable.Count());
    iTestRemoteExecutionTable.Close();
    RDebug::Print(_L("CStifTFw::~CStifTFw closing iEventArray cnt=%d"), iEventArray.Count());
    iEventArray.Close();
    RDebug::Print(_L("CStifTFw::~CStifTFw closing iMasterArray cnt=%d"), iMasterArray.Count());
    iMasterArray.Close();

    //Moved from the beginning of destructor, because objects from some arrays during resetting and/or closing
    //have tried to call methods of iUIEngine.
    RDebug::Print(_L("CStifTFw::~CStifTFw deleteing iUiEngine address=%d"), iUIEngine);
    delete iUIEngine;

    RDebug::Print(_L("CStifTFw::~CStifTFw end"));
    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: AtsRemote

    Description: Remote ATS call

    Parameters: const TDesC& aMessage: in:

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStifTFw::AtsRemote( CStifTFwIfProt& aReq, CStifTFwIfProt& aResp )
    {
    RDebug::Print(_L("CStifTFw::AtsRemote start, iCmdType=%d "), aReq.iCmdType);

    TInt ret = KErrNone;

    switch( aReq.iCmdType )
        {
        case CStifTFwIfProt::ECmdRun:
            {
            RDebug::Print(_L("CStifTFw::AtsRemote iCmdType CStifTFwIfProt::ECmdRun "));
            ret = AtsRemoteRun( aReq, aResp );
            break;
            }
        case CStifTFwIfProt::ECmdPause:
        case CStifTFwIfProt::ECmdResume:
        case CStifTFwIfProt::ECmdCancel:
            {
            RDebug::Print(_L("CStifTFw::AtsRemote iCmdType CStifTFwIfProt::ECmdPause,Resume,Cancel "));
            ret = AtsRemoteTestCtl( aReq, aResp );
            break;
            }
        case CStifTFwIfProt::ECmdRequest:
        case CStifTFwIfProt::ECmdRelease:
            {
            RDebug::Print(_L("CStifTFw::AtsRemote iCmdType CStifTFwIfProt::ECmdRequest,Release "));
            ret = AtsRemoteEventCtl( aReq, aResp );
            break;
            }
        case CStifTFwIfProt::ECmdSetEvent:
        case CStifTFwIfProt::ECmdUnsetEvent:
        	{
            RDebug::Print(_L("CStifTFw::AtsRemote iCmdType CStifTFwIfProt::ECmdSetEvent,Unset start "));
            ret = AtsRemoteEventCtl( aReq, aResp );
            RDebug::Print(_L("CStifTFw::AtsRemote iCmdType CStifTFwIfProt::ECmdSetEvent,Unset end "));
            break;
			}
        default:
            {
            RDebug::Print(_L("CStifTFw::AtsRemote iCmdType UNSUPPORTED value=%d "), aReq.iCmdType);
            return KErrNotSupported;
            }
        }

    return ret;
    }

/*
-------------------------------------------------------------------------------

     Class: CStifTFw

     Method: GetCaseNumByTitle

     Description: Enumerates test cases from given module and finds test case index by title

     Parameters:    TDesC& aModule: in: module name
                    TDesC& aTestCaseFile: in: cfg file
	                TDesC& aTitle: in: test case title
                    TInt& aCaseNum: in out: test case index

     Return Values: KErrNone if everything is ok
                    KErrNotFound if there is no such test case

     Errors/Exceptions: None.

     Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CStifTFw::GetCaseNumByTitle(TDesC& aModule, TDesC& aTestCaseFile, TDesC& aTitle, TInt& aCaseNum)
    {
    RDebug::Print(_L("CStifTFw::GetCaseNumByTitle looking for test case number module=[%S] cfg=[%S] title=[%S]"), &aModule, &aTestCaseFile, &aTitle);
	//Allocate table
    RPointerArray<CTestInfo> allTestInfos;
    CleanupClosePushL(allTestInfos);
    TInt ret = KErrNone;

    //Enumerate test cases
    RDebug::Print(_L("CStifTFw::GetCaseNumByTitle enumerating test cases"));
    if(aTestCaseFile == _L("dummy.cfg"))
        {
		TName t = _L("");
        ret = iStifTFwIf->GetTestCases(allTestInfos, aModule, t);
		}
    else
        {
        ret = iStifTFwIf->GetTestCases(allTestInfos, aModule, aTestCaseFile);
		}

    if(ret != KErrNone)
        {
		RDebug::Print(_L("Test cases enumeration from module [%S] and file [%S] has returned an error %d"), &aModule, &aTestCaseFile, ret);
		}
    else
        {
		//Searching through the table
        RDebug::Print(_L("CStifTFw::GetCaseNumByTitle searching through the test cases table containing %d cases"), allTestInfos.Count());
        ret = KErrNotFound;
        for(TInt i = 0; i < allTestInfos.Count(); i++)
            {
            if(allTestInfos[i]->TestCaseTitle() == aTitle)
                {
                aCaseNum = i;
                //TestScripter relies on 1-based indexing
                if((aModule.Find(KTestScripterName) != KErrNotFound) || (aModule.Find(KPythonScripter) != KErrNotFound) || (aModule.Find(KTestCombinerName) != KErrNotFound)) 
                    aCaseNum++;
                ret = KErrNone;
                break;
                }
            }
		}
	if(ret == KErrNotFound)
	    {
        RDebug::Print(_L("CStifTFw::GetCaseNumByTitle test case not found"));
		}
	else if(ret == KErrNone)
	    {
        RDebug::Print(_L("CStifTFw::GetCaseNumByTitle test case number found: %d"), aCaseNum);
		}
	else
	    {
        RDebug::Print(_L("CStifTFw::GetCaseNumByTitle error %d"), ret);
		}

    //Some cleaning
    allTestInfos.Reset();
    CleanupStack::PopAndDestroy();

    return ret;
    }

/*
    Class: CStifTFw

    Method: AtsRemoteRun

    Description: Remote run ATS call

    Parameters: const TDesC& aMessage: in:

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStifTFw::AtsRemoteRun( CStifTFwIfProt& aReq, CStifTFwIfProt& aResp )
    {
    RDebug::Print(_L("CStifTFw::AtsRemoteRun start"));

    TInt ret = KErrNone;
    TUint16 testid = 0;

    if( ( aReq.DstDevId() == 0 ) ||
        ( aReq.DstTestId() != 0 ) )
        {
        RDebug::Print(_L("CStifTFw::AtsRemoteRun Protocol violation"));
        // Protocol violation
        __TRACE( KError, ( _L( "AtsReceive: remote run for illegal dstid received") ) );
        return KErrGeneral;
        }

    if ( aReq.DstDevId() != iStifTFwIf->DevId() )
        {
        RDebug::Print(_L("CStifTFw::AtsRemoteRun Not our protocol message"));
        // Not our protocol message
        __TRACE( KError, ( _L( "AtsReceive: remote run for illegal dstdevid received") ) );
        return KErrNotFound;
        }


    if( aReq.iModule.Length() == 0 )
        {
        RDebug::Print(_L("CStifTFw::AtsRemoteRun No test module name given"));
        __TRACE( KError, ( _L("No mandatory test module name given as run parameter") ) );
        ret = KErrNotFound;
        }
    else if(aReq.iTestCaseNumber < 0 && aReq.iTitle.Length() == 0)
        {
        RDebug::Print(_L("CStifTFw::AtsRemoteRun No test case number given"));
        __TRACE( KError, ( _L("No mandatory test case number given as run parameter") ) );
        ret = KErrNotFound;
        }
    else
        {
        TBufC<KTestScripterNameLength> myTestscripter;
        myTestscripter = KTestScripterName;

        if( ( aReq.iModule.Find( myTestscripter ) != KErrNotFound ) )
            {
            RDebug::Print(_L("CStifTFw::AtsRemoteRun TestScripter module"));
            // If testmodule's type is testscripter, then iModule has name
            // "testscripter_testcasefile". AddTestModule method takes
            // testscripter type of module without _testcasefile, so we cannot
            // give iModule name as parameter.

            // Adding testscripter type of testmodule
            RDebug::Print(_L("CStifTFw::AtsRemoteRun calling AddTestModule"));
            ret = iStifTFwIf->AddTestModule( myTestscripter, aReq.iIniFile );
            if( ret != KErrNone )
                {
                __TRACE( KError, ( _L( "CStifTFw::AtsRemoteRun: AddTestModule failed [%d]"), ret ) );

                // It is ok if test module is already loaded to test engine
                if( ret == KErrAlreadyExists )
                    {
                    __TRACE( KPrint, ( _L( "Test module already exists at testengine, no problem") ) );
                    }
                }
            // Adding testscripter testcasefile
            RDebug::Print(_L("CStifTFw::AtsRemoteRun calling AddTestCaseFile"));
            ret = iStifTFwIf->AddTestCaseFile( myTestscripter, aReq.iTestCaseFile );
            if( ret != KErrNone )
                {
                // It is ok if test case file is already loaded to test engine
                if( ret == KErrAlreadyExists )
                    {
                    __TRACE( KPrint, ( _L( "Testcasefile already exists at testengine, no problem") ) );
                    }
                // Got other error, log info about it
                else
                    {
                    __TRACE( KError, ( _L( "CStifTFw::AtsRemoteRun: AddTestCaseFile failed [%d]"), ret ) );
                    __TRACE( KError, ( _L( "Testscripter needs to have test case file loaded successfully, please check path and file name!") ) );
                    }
                }
            }
        else
            {
            RDebug::Print(_L("CStifTFw::AtsRemoteRun It is not a TestScripter module "));
            // For testmodules other than testscripter
            RDebug::Print(_L("CStifTFw::AtsRemoteRun calling AddTestModule "));
            ret = iStifTFwIf->AddTestModule( aReq.iModule, aReq.iIniFile );
            if( ret != KErrNone )
                {
                __TRACE( KError, ( _L( "CStifTFw::AtsRemoteRun: AddTestModule failed [%d]"), ret ) );

                // It is ok if test module is already loaded to test engine
                if( ret == KErrAlreadyExists )
                    {
                    __TRACE( KPrint, ( _L( "Test module already exists at testengine, no problem") ) );
                    }
                }
            }
        if(aReq.iTitle.Length() != 0)
            {
			aReq.iTestCaseNumber = -1;
            //Get test case number from title
            ret = GetCaseNumByTitle(aReq.iModule, aReq.iTestCaseFile, aReq.iTitle, aReq.iTestCaseNumber);
            //Check test case number
            if(aReq.iTestCaseNumber < 0)
                {
                RDebug::Print(_L("CStifTFw::AtsRemoteRun No test case number could be found using given title parameter"));
                __TRACE(KError, (_L("No mandatory test case number could be found using given title parameter")));
                return KErrNotFound;
                }
            }

        CTestInfo *testCase = CTestInfo::NewL();
        CleanupStack::PushL( testCase );
        // Copy to CTestInfo.

        if( ( aReq.iModule.Find( KTestScripterName ) != KErrNotFound ) )
            {
            testCase->SetModuleName( KTestScripterName );
            }
        else
            {
            testCase->SetModuleName(  aReq.iModule );
            }
        testCase->SetTestCaseNumber( aReq.iTestCaseNumber );
        testCase->SetTestCaseFile( aReq.iTestCaseFile );

        // Create container.
        CUIEngineContainer* container = NULL;

        RDebug::Print(_L("CStifTFw::AtsRemoteRun calling StartTestCase "));
        ret = iUIEngine->StartTestCase( container,
                                        *testCase );

        CleanupStack::PopAndDestroy( testCase );

        if( ret != KErrNone )
            {
            RDebug::Print(_L("CStifTFw::AtsRemoteRun test case starting FAILED "));
            __TRACE( KError, ( _L( "Test case starting failed [%d]"), ret ) );
            }
        else
            {
            RDebug::Print(_L("CStifTFw::AtsRemoteRun Add container"));
            // Add container to test case execution table
            container->SetRemoteId( aReq.SrcId() );
            testid = TESTID( (TInt)container );
            ret = iTestRemoteExecutionTable.Append( container );
            __TRACE( KError, ( _L( "Test case started [%x]"), testid ) );
            }
        }

    aResp.AppendId( SETID( iStifTFwIf->DevId(), testid ) );
    aResp.AppendId( aReq.SrcId() );
    aResp.Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
    aResp.Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRun );
    if( ret != KErrNone )
        {
        aResp.Append( CStifTFwIfProt::RunStatus,
                     CStifTFwIfProt::ERunError );
        aResp.Append( CStifTFwIfProt::RunStatusParams,
                     CStifTFwIfProt::ERunResult, ret );
        }
    else
        {
        aResp.Append( CStifTFwIfProt::RunStatus,
                     CStifTFwIfProt::ERunStarted );
        }
    // Response is created, return success
    ret = KErrNone;

    RDebug::Print(_L("CStifTFw::AtsRemoteRun end "));
    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: AtsRemoteTestCtl

    Description: Remote run ATS call

    Parameters: const TDesC& aMessage: in:

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStifTFw::AtsRemoteTestCtl( CStifTFwIfProt& aReq, CStifTFwIfProt& aResp )
    {
    RDebug::Print(_L("CStifTFw::AtsRemoteTestCtl"));

    TInt ret = KErrNone;

    if( ( aReq.DstDevId() == 0 ) ||
        ( aReq.DstTestId() == 0 ) )
        {
        // Protocol violation
        __TRACE( KError, ( _L( "AtsReceive: remote test control for illegal dstid received") ) );
        return KErrGeneral;
        }

    if ( aReq.DstDevId() != iStifTFwIf->DevId() )
        {
        // Not our protocol message
        __TRACE( KError, ( _L( "AtsReceive: remote test control for illegal dstdevid received") ) );
        return KErrNotFound;
        }

    // Find test case from execution table
    TInt count = iTestRemoteExecutionTable.Count();
    TInt index = 0;
    for(; index < count; index++ )
        {
        if( TESTID( iTestRemoteExecutionTable[index] ) ==
            aReq.DstTestId() )
            {
            break;
            }
        }

    // Check that test id is found
    if( index == count )
        {
        __TRACE( KError, (  _L( "PauseTestCase. Container not found: testId: %x"),
            aReq.DstTestId() ) );
        return KErrNotFound;
        }

    switch( aReq.iCmdType )
        {
        case CStifTFwIfProt::ECmdPause:
            // Pause test
            __TRACE( KPrint, (  _L( "PauseTest: %x"), aReq.DstTestId() ) );
            ret = iTestRemoteExecutionTable[index]->PauseTest();
            break;
        case CStifTFwIfProt::ECmdResume:
            // Resume test
            __TRACE( KPrint, (  _L( "ResumeTestCase: %x"), aReq.DstTestId() ) );
            ret = iTestRemoteExecutionTable[index]->ResumeTest();
            break;
        case CStifTFwIfProt::ECmdCancel:
            // Cancel test
            __TRACE( KPrint, (  _L( "CancelTestCase: %x"), aReq.DstTestId() ) );
            ret = iTestRemoteExecutionTable[index]->CancelTest();
            break;
        default:
            return KErrNotSupported;
        }
    if( ret == KErrNone )
        {
        // Success response is created here
        aResp.AppendId( aReq.DstId() );
        aResp.AppendId( aReq.SrcId() );
        aResp.Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
        aResp.Append( CStifTFwIfProt::CmdType, aReq.iCmdType );
        }

    return ret;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: AtsRemoteEventCtl

    Description: Remote run ATS call

    Parameters: const TDesC& aMessage: in:

    Return Values: KErrNotSupported

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStifTFw::AtsRemoteEventCtl( CStifTFwIfProt& aReq,
                                    CStifTFwIfProt& aResp )
    {
    RDebug::Print(_L("CStifTFw::AtsRemoteEventCtl start "));

    if( ( aReq.DstDevId() == 0 ) ||
        ( aReq.DstTestId() != 0 ) )
        {
        // Protocol violation
        __TRACE( KError, ( _L( "AtsReceive: remote event control for illegal dstid received") ) );
        return KErrGeneral;
        }

    if ( aReq.DstDevId() != iStifTFwIf->DevId() )
        {
        // Not our protocol message
        __TRACE( KError, ( _L( "AtsReceive: remote event control for illegal dstdevid received") ) );
        return KErrNotFound;
        }

    if( aReq.iEventName.Length() == 0 )
        {
        __TRACE( KError, ( _L("No event name given") ) );
        return KErrNotFound;
    }

    switch( aReq.iCmdType )
        {
        case CStifTFwIfProt::ECmdRequest:
            {
            RDebug::Print(_L("CStifTFw::AtsRemoteEventCtl ECmdRequest "));
            CUIEngineEvent* eventCtl = NULL;
            TRAPD( err, eventCtl = CUIEngineEvent::NewL( iUIEngine ); );
            if( err != KErrNone )
                {
                return err;
                }
            err = iEventArray.Append( eventCtl );
            if( err != KErrNone )
                {
                delete eventCtl;
                return err;
                }

            eventCtl->Request( aReq.iEventName, aReq.SrcId(),
                               SETID( iStifTFwIf->DevId(), 0 ) );

            aResp.AppendId( aReq.DstId() );
            aResp.AppendId( aReq.SrcId() );
            aResp.Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
            aResp.Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRequest );
            aResp.Append( CStifTFwIfProt::EventStatus,
                         CStifTFwIfProt::EEventActive );
            aResp.Append( aReq.iEventName );

            }
            break;
        case CStifTFwIfProt::ECmdRelease:
            {
            RDebug::Print(_L("CStifTFw::AtsRemoteEventCtl ECmdRelease "));
            TInt count = iEventArray.Count();
            TInt index = 0;
            for(; index < count; index++ )
                {
                if( ( iEventArray[index]->EventName() == aReq.iEventName ) &&
                    ( iEventArray[index]->Master() == aReq.SrcId() ) )
                    {
                    // Found correct event
                    CUIEngineEvent* eventCtl = iEventArray[index];
                    eventCtl->Release();
                    iEventArray.Remove(index);
                    delete eventCtl;
                    break;
                    }
                }
            if( index == count )
                {
                return KErrNotFound;
                }

            aResp.AppendId( aReq.DstId() );
            aResp.AppendId( aReq.SrcId() );
            aResp.Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
            aResp.Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRelease );
            aResp.Append( aReq.iEventName );

            }
            break;
        default:
            RDebug::Print(_L("CStifTFw::AtsRemoteEventCtl NotSupported"));
            return KErrNotSupported;
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: TestExecuted

    Description:

    Parameters: CUIEngineContainer* aContainer: Address of current
                CUIEngineContainer is test case ID.

                TFullTestResult& aFullTestResult: in: Test result of executed test case

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C void CStifTFw::TestExecuted( CUIEngineContainer* aContainer,
                                        TFullTestResult& aFullTestResult )
    {
    RDebug::Print(_L("CStifTFw::TestExecuted"));
    TInt testExecutionResult = aFullTestResult.iCaseExecutionResultCode;
    TTestResult testResult = aFullTestResult.iTestResult;

    // Find Container from test execution table
    TInt index = iTestExecutionTable.Find( aContainer );
    if( index >= 0 )
        {
        // Remove Container from test case execution table
        iTestExecutionTable.Remove( index );

        iStifTFwIf->TestCompleted( (TInt)aContainer, testExecutionResult, testResult );
        __TRACE( KPrint, ( _L( "CStifTFw::TestExecuted. TestID %d  executed"), (TInt)aContainer ) );
        }
     else
        {
        // Find Container from test remote execution table
        index = iTestRemoteExecutionTable.Find( aContainer );
        // Check if test id is found from remote tests
        if( index >= 0 )
            {
            // Remove Container from test case execution table
            iTestRemoteExecutionTable.Remove( index );

            CStifTFwIfProt* resp = NULL;
            TRAPD( err, resp = CStifTFwIfProt::NewL() );
            if( err != KErrNone )
                {
                // oom
                return;
                }

            // Create response
            TRAP( err, resp->CreateL(); );
            if( err != KErrNone )
                {
                delete resp;
                // Oom
                return;
                }
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgResponse );
            resp->AppendId( SETID( iStifTFwIf->DevId(), TESTID( (TInt) aContainer ) ) );
            resp->AppendId( aContainer->RemoteId() );
            resp->Append( CStifTFwIfProt::MsgType, CStifTFwIfProt::EMsgRemote );
            resp->Append( CStifTFwIfProt::CmdType, CStifTFwIfProt::ECmdRun );
            resp->Append( CStifTFwIfProt::RunStatus,
                         CStifTFwIfProt::ERunReady );
            if( testExecutionResult != KErrNone )
                {
                resp->Append( CStifTFwIfProt::RunStatusParams,
                             CStifTFwIfProt::ERunResult,
                             testExecutionResult );
                switch( aFullTestResult.iCaseExecutionResultType )
                    {
                    case TFullTestResult::ECasePanic:
                        resp->Append( CStifTFwIfProt::RunStatusParams,
                                     CStifTFwIfProt::ERunCategory,
                                     CStifTFwIfProt::ResultCategory,
                                     CStifTFwIfProt::EResultPanic );
                        break;
                    case TFullTestResult::ECaseTimeout:
                        resp->Append( CStifTFwIfProt::RunStatusParams,
                                     CStifTFwIfProt::ERunCategory,
                                     CStifTFwIfProt::ResultCategory,
                                     CStifTFwIfProt::EResultTimeout );
                        break;
                    case TFullTestResult::ECaseLeave:
                        resp->Append( CStifTFwIfProt::RunStatusParams,
                                     CStifTFwIfProt::ERunCategory,
                                     CStifTFwIfProt::ResultCategory,
                                     CStifTFwIfProt::EResultLeave );
                        break;
                    case TFullTestResult::ECaseException:
                        resp->Append( CStifTFwIfProt::RunStatusParams,
                                     CStifTFwIfProt::ERunCategory,
                                     CStifTFwIfProt::ResultCategory,
                                     CStifTFwIfProt::EResultException );
                        break;
                    default:
                        // The rest are normal execution results
                        break;
                    }
                }
            else
                {
                resp->Append( CStifTFwIfProt::RunStatusParams,
                             CStifTFwIfProt::ERunResult,
                             testResult.iResult );
                }

            iStifTFwIf->AtsSend( resp->Message() );
            __TRACE( KPrint, ( _L( "TestExecuted: %x"), TESTID(aContainer) ) );
            delete resp;
            }
        }

    if( index >= 0 )
        {
        __TRACE( KPrint, ( _L( "Execution result %d"), testExecutionResult ) );
        __TRACE( KPrint, ( _L( "Test result(%S) %d"), &testResult.iResultDes, testResult.iResult ) );
        }
    else
        {
        __TRACE( KPrint, ( _L( "CStifTFw::TestExecuted. Container not found: TestId = %d"),
            (TInt)aContainer ) );
        User::Panic( _L("CStifTFw"), KErrNotFound );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: PrintProg

    Description:

    Parameters: CUIEngineContainer* aContainer: in: Address of current
                CUIEngineContainer is test case ID

                TTestProgress& aProgress: in: Progress information from test case

    Return Values: TInt KErrNone: Always returned KErrNone

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFw::PrintProg( CUIEngineContainer* aContainer,
                                     TTestProgress& aProgress )
    {
    RDebug::Print(_L("CStifTFw::PrintProg"));

    if( aContainer == NULL )
        {
        return KErrArgument;
        }
    if( ( aProgress.iDescription.Length() == 0 ) &&
        ( aProgress.iText.Length() == 0 ) )

        {
        return KErrArgument;
        }

//Stif-50 - the following code has been commented out to get messages on slave
//    // Find Container from test execution table and remove it
//    TInt index = iTestExecutionTable.Find(aContainer);
//
//    // Check if test id is found
//    if( index >= 0 )
//        {
//        // Forward if found, prints are not forwarded from slave to master
        iStifTFwIf->Print( (TInt)aContainer, aProgress );
//        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: RemoteMsg

    Description:

    Parameters: const TDesC& aMessage: in: Remote message

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFw::RemoteMsg( CUIEngineContainer* /*aContainer*/,
                                     const TDesC& aMessage)
    {
    RDebug::Print(_L("CStifTFw::RemoteMsg"));

    if( aMessage.Length() == 0 )
        {
        return KErrArgument;
        }
    // Forward message
    iStifTFwIf->AtsSend( aMessage );

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: RemoteMsg

    Description:

    Parameters: const TDesC& aMessage: in: Remote message

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CStifTFw::GoingToReboot( CUIEngineContainer* /*aContainer*/,
                                         TRequestStatus& aStatus )
    {
    RDebug::Print(_L("CStifTFw::GoingToReboot"));

    aStatus = KRequestPending;
    // Forward reboot indication
    iStifTFwIf->GoingToDoReset();
    TRequestStatus* rs = &aStatus;
    User::RequestComplete( rs, KErrNone );

    return KErrNone;

    }
    
/*
-------------------------------------------------------------------------------

    Class: CStifTFw

    Method: DeleteData

    Description: Delete arrays before closing session to test engine server

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CStifTFw::DeleteData()
    {
    RDebug::Print(_L("CStifTFw::DeleteData resetting iTestExecutionTable cnt=%d"), iTestExecutionTable.Count());
    iTestExecutionTable.Reset();
    RDebug::Print(_L("CStifTFw::DeleteData resetting iTestRemoteExecutionTable cnt=%d"), iTestRemoteExecutionTable.Count());
    iTestRemoteExecutionTable.Reset();
    RDebug::Print(_L("CStifTFw::DeleteData resetting iEventArray cnt=%d"), iEventArray.Count());
    iEventArray.ResetAndDestroy();
    RDebug::Print(_L("CStifTFw::DeleteData resetting iMasterArray cnt=%d"), iMasterArray.Count());
    iMasterArray.Reset();
    }

// End of File
