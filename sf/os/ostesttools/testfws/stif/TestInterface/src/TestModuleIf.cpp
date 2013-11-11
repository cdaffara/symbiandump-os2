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
* Description: This file contains TestModuleIf implementation.
*
*/

// INCLUDE FILES
#include <e32svr.h>
#include "StifTFwIfProt.h"
#include "StifTestInterface.h"
#include "TestServerModuleIf.h"
#include "StifTestModule.h"
#include "TestThreadContainer.h"
#include "STIFMeasurement.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// Debugging is enabled with next define
#define __TRACING_ENABLED
#ifdef __TRACING_ENABLED
#define __RDEBUG(p) RDebug::Print p 
#else
#define __RDEBUG(p)
#endif

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    TDesOverflowHandler class contains a simple overflow handler implementation.

-------------------------------------------------------------------------------
*/
class TDesOverflowHandler : public TDes16Overflow
    {
    public:
        TDesOverflowHandler( CTestModuleIf* aModuleIf, 
                             const TInt aPriority, 
                             const TDesC& aDefinition)
            {
            iModuleIf = aModuleIf;
            iPriority = aPriority;
            iDefinition = aDefinition;
            }

        void Overflow(TDes16& /*aDes*/ )
            { 
            }
    
        CTestModuleIf* iModuleIf;
        TInt iPriority;
        TStifInfoName iDefinition;
    };

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CTestModuleIf class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: CTestModuleIf

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.
    
    Parameters: None
    
    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Approved
    
-------------------------------------------------------------------------------
*/
CTestModuleIf::CTestModuleIf( CTestThreadContainer* aTestExecution ) :
    iTestExecution( aTestExecution )
    {
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: CTestExecution* aTestExecution: in: Pointer to TestExecution
                CTestModuleBase* aTestModule: in: Pointer to TestModule
    
    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Approved
    
-------------------------------------------------------------------------------
*/
void CTestModuleIf::ConstructL( CTestModuleBase* aTestModule )
    {

    if ( aTestModule->iTestModuleIf != NULL)
        {
        delete aTestModule->iTestModuleIf;
        aTestModule->iTestModuleIf = NULL;
        }
    aTestModule->iTestModuleIf = this; 

    iIsRebootReady = EFalse;
    iStoreStateCounter = 0;

    // Used to "resets" iTestCaseResults array
    iAllowTestCaseResultsCount = 0;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestExecution* aTestExecution: in: Pointer to TestExecution
                CTestModuleBase* aTestModule: in: Pointer to TestModule
    
    Return Values: CTestModuleIf object.

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves

    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C CTestModuleIf* CTestModuleIf::NewL( CTestThreadContainer* aExecutionSession, 
                                             CTestModuleBase* aTestModule )
    {

    CTestModuleIf* self = 
        new (ELeave) CTestModuleIf( aExecutionSession );
    
    CleanupStack::PushL( self );
    self->ConstructL( aTestModule );

    CleanupStack::Pop();

    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: ~CTestModuleIf

    Description: Destructor
    
    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None
    
    Status: Approved
    
-------------------------------------------------------------------------------
*/    
CTestModuleIf::~CTestModuleIf()
    {
    iTestExecution = NULL;

    // Used to "resets" iTestCaseResults array
    iAllowTestCaseResultsCount = 0;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: Printf

    Description: Printing

    Printf is used to provide different information up to the UI 
    that can be then printed e.g. to the Console Screen. 
    The priority can be used in the UI to decide if the information 
    received from the Test DLL will be discarded or not in 
    the different performance situations. The priority is also 
    used in the Test DLL server and in the Test Engine to queue 
    the Printf responses.
    This method is implemented in Test DLL Server and the Test DLL 
    can call it to provide printable information to the UI.

    Parameters:  const TInt aPriority: in: 
                    Importance of the returned information
                 const TDesC& aDefinition: in: 
                    Definition of data to be printed 
                 TRefByValue<const TDesC> aFmt: in: Printed data
    
    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Approved
    
-------------------------------------------------------------------------------
*/
EXPORT_C void CTestModuleIf::Printf( const TInt aPriority, 
                                     const TDesC& aDefinition, 
                                     TRefByValue<const TDesC> aFmt,
                                     ... 
                                   )
    {           

    if( !IsServerAlive() )
        {
        return;
        }

    VA_LIST list;
    VA_START(list,aFmt);
    TName aBuf;
    RBuf buf;
    TInt ret = buf.Create(1024);
    if(ret != KErrNone)
        {
        __RDEBUG((_L("STF: Printf: Buffer creation failed [%d]"), ret));
        return;
        }

    // Cut the description length
    TInt len = aDefinition.Length();
    if ( len > KMaxInfoName )
        {
        len = KMaxInfoName;
        }

    TStifInfoName shortDescription = aDefinition.Left(len);

    // Create overflow handler
    TDesOverflowHandler overFlowHandler (this, aPriority, shortDescription);

    // Parse parameters
    buf.AppendFormatList(aFmt, list, &overFlowHandler);
    
    if(buf.Length() == 0)
        {
        __RDEBUG((_L("STF: Printf: Unable to prepare print buffer (probably printed string is too long)")));
        }

    // Print
    aBuf.Copy(buf.Left(aBuf.MaxLength()));
    buf.Close();

    iTestExecution->DoNotifyPrint( aPriority, shortDescription, aBuf );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: Event

    Description: Event control.

    Event function is used to control and use the event system. 
    TEventIf &aEvent encapsulates the request type and 
    the event name, see StifTestEventInterface.h for more information.
    This method is implemented in Test DLL Server and the Test DLL 
    can call it to control the event system.
  
    Parameters: TEventIf& aEvent: in: Event command
    
    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TInt CTestModuleIf::Event( TEventIf& aEvent )
    {

    if( !IsServerAlive() )
        {
        return KErrGeneral;
        }

    // All event commands are handled in testserver and testengine
    return iTestExecution->DoNotifyEvent( aEvent );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: Event

    Description: Event control.

    Asynchronous version of event control function.
    It is used to control and use the event system asynchronously. 
    TEventIf &aEvent encapsulates the request type and 
    the event number, see StifTestEventInterface.h for more information.
    This method is implemented in Test DLL Server and the Test DLL 
    can call it to control the event system.
  
    Parameters: TEventIf& aEvent: in: Event command
                TRequestStatus& aStatus: in: Request status parameter

    Return Values: None. 
    
    Errors/Exceptions: None

    Status: Approved
    
-------------------------------------------------------------------------------
*/      
EXPORT_C void CTestModuleIf::Event( TEventIf& aEvent, TRequestStatus& aStatus )
    {
    TInt ret = KErrNone;

    if( !IsServerAlive() )
        {
        __RDEBUG( (_L("iTestExecution not initialised")));
        ret = KErrGeneral;
        }
    else
        {        
        aStatus = KRequestPending;

        // All event commands are handled in testserver and testengine
        ret = iTestExecution->DoNotifyEvent( aEvent, &aStatus );
        }
    if( ret != KErrNone )
        {
        TRequestStatus* rs = &aStatus;
        User::RequestComplete( rs, ret );
        }
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: CancelEvent

    Description: Cancel asynchronous event control call.
  
    Parameters: TEventIf& aEvent: in: Event command to be cancelled.
                const TRequestStatus* aStatus: in: 
                    Pointer to TRequestStatus parameter that is cancelled

    Return Values: Symbian OS error code.
    
    Errors/Exceptions: None
    
    Status: Approved
    
-------------------------------------------------------------------------------
*/      
EXPORT_C TInt CTestModuleIf::CancelEvent( TEventIf& aEvent,
                                          TRequestStatus* aStatus )
    {
     if( !IsServerAlive() )
        {
        __RDEBUG( (_L("iTestExecution not initialised")));
        return KErrGeneral;
        }

    // All event commands are handled in testserver and testengine
    iTestExecution->CancelEvent( aEvent, aStatus );
    
    return KErrNone;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: SetExitReason

    Description: Set exit reason
  
    Parameters: const TExitReason aExitReason in: Exit reason
                const TInt aExitCode in: Exit code

    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/      
EXPORT_C void CTestModuleIf::SetExitReason( const CTestModuleIf::TExitReason aExitReason, 
                                            const TInt aExitCode )
        
    {

     if( !IsServerAlive() )
        {
        __RDEBUG( (_L("iTestExecution not initialised")));
        return;
        }
    
    iTestExecution->SetExitReason( aExitReason, aExitCode );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: RemoteSend

    Description: RemoteSend is used to send control protocol messages to slaves
        (e.g. another phone, call box, ...). 
    
    Parameters:  const TDesC& aRemoteMsg: in: 
                    Remote command protocol message 
                 
    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::RemoteSend( const TDesC& aRemoteMsg )
    {           

    if( !IsServerAlive() )
        {
        __RDEBUG( ( _L("iTestExecution not initialised") ) );
        return KErrNotReady;
        }

    TParams params;
    params.aRemoteMsgConstRef = &aRemoteMsg;

    TRequestStatus status = KRequestPending; 

    // Forward
    iTestExecution->DoRemoteReceive( EStifCmdSend, params, 
                                     aRemoteMsg.Length(), status );
    
    User::WaitForRequest( status );

    return status.Int();

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: RemoteReceive

    Description: RemoteReceive is used to receive control protocol messages to 
        slaves (e.g. another phone, call box, ...). 
    
    Parameters:  const TDesC& aRemoteMsg: in: 
                    Remote command protocol message 
                 TRequestStatus& aStatus: in: Request status parameter

    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C void CTestModuleIf::RemoteReceive( TDes& aRemoteMsg, 
                                            TRequestStatus& aStatus )
    {           
    aStatus = KRequestPending;
    if( !IsServerAlive() )
        {
        __RDEBUG( ( _L("iTestExecution not initialised") ) );
        TRequestStatus* rs = &aStatus;
        User::RequestComplete( rs, KErrNotReady );
        }

    TParams params;
    params.aRemoteMsgRef = &aRemoteMsg;

    iTestExecution->DoRemoteReceive( EStifCmdReceive, params, 
                                     aRemoteMsg.Length(), aStatus );

    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: RemoteReceiveCancel

    Description: RemoteReceiveCancel is used to cancel RemoteReceive.
     
    Parameters:  None
    
    Return Values: None
    
    Errors/Exceptions: None
    
    Status: Proposal
    
-------------------------------------------------------------------------------
*/
EXPORT_C void CTestModuleIf::RemoteReceiveCancel()
    {           
    if( !IsServerAlive() )
        {
        __RDEBUG( ( _L("iTestExecution not initialised") ) );
        return;
        }

    // Forward
    iTestExecution->DoRemoteReceiveCancel();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: Reboot

    Description: Start a reboot operation.

    Parameters: TInt aType: in: Reboot type

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: KErrNotReady returned if iTestExecution is NULL.
                       KErrNotReady returned if reboot not allowed(Store state
                       not called).

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::Reboot( TInt aType )
    {           
    if ( !IsServerAlive() )
        {
        __RDEBUG( ( _L( "iTestExecution not initialised" ) ) );
        return KErrNotReady;
        }
    if( !iIsRebootReady )
        {
        __RDEBUG( ( _L( "Reboot operation not ready" ) ) );
        return KErrNotReady;
        }
     
/*    switch( aType )
        {
        case EDefaultReset:
            __RDEBUG( ( _L( "Reboot, type default" )) );
            break;
        case EKernelReset:
            __RDEBUG( ( _L( "Reboot, type KernelReset" )) );
            break;
        case EDeviceReset0:
            __RDEBUG( ( _L( "Reboot, type Reset 0" )) );
            break;
        case EDeviceReset1:
            __RDEBUG( ( _L( "Reboot, type Reset 1" )) );
            break;
        case EDeviceReset2:
            __RDEBUG( ( _L( "Reboot, type Reset 2" )) );
            break;
        case EDeviceReset3:
            __RDEBUG( ( _L( "Reboot, type Reset 3" )) );
            break;
        case EDeviceReset4:
            __RDEBUG( ( _L( "Reboot, type Reset 4" )) );
            break;
        case EDeviceReset5:
            __RDEBUG( ( _L( "Reboot, type Reset 5" )) );
            break;
        default:
            __RDEBUG( ( _L( "Reboot type %d not supported" ), aType ) );
            return KErrNotSupported;    
        }
*/

    TParams params;
    TRebootParams rebootParams;
    
    params.aRebootType = &rebootParams;
    rebootParams.aType = ( TRebootType )aType;
    
    TRequestStatus status = KRequestPending; 

    // Forward
    iTestExecution->DoRemoteReceive( EStifCmdReboot, params, sizeof( aType ), status );
    
    User::WaitForRequest( status );

    return status.Int();
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: StoreState

    Description: Stores the current state before reboot.

    Parameters: TInt aCode: in: Reboot releated integer value.
                TName& aName: in: Reboot related string value.

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: KErrNotReady returned if iTestExecution is NULL.
                       KErrOverflow returned if aName length is over TName.
                       KErrInUse returned if method is called more than once.

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::StoreState( TInt aCode, TName& aName )
    {
    iStoreStateCounter++;   // Store state counter

    if ( !IsServerAlive() )
        {
        __RDEBUG( ( _L( "iTestExecution not initialised" ) ) );
        return KErrNotReady;
        }
    // Check aName length
    if ( aName.Length() > KMaxName )
        {
        __RDEBUG( ( _L( "CTestModuleIf::StoreState(): aName length is not valid" ) ) );
        return KErrOverflow;
        }
    // Only one store state call may be done
    if( iStoreStateCounter > 1 )
        {
        __RDEBUG( ( _L( "Store state allready called" ) ) );
        return KErrInUse;
        }

    TParams params;
    TRebootStateParams rebootStateParams;
    params.aRebootState = &rebootStateParams;
    rebootStateParams.aCode = aCode;
    rebootStateParams.aName = aName;

    TRequestStatus status = KRequestPending; 

    // Forward
    iTestExecution->DoRemoteReceive( EStifCmdStoreState, params,
                                     sizeof( TRebootStateParams), status );
    
    User::WaitForRequest( status );

    // If store state is done successfully reboot operation is allowed
    if( status.Int() == KErrNone )
        {
        iIsRebootReady = ETrue;
        }

    return status.Int();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: GetStoredState

    Description: Get saved store information after the reboot.

    Parameters: TInt aCode: inout: Get reboot releated integer value.
                TName& aName: inout: Get reboot related string value.

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: KErrNotReady returned if iTestExecution is NULL.

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::GetStoredState( TInt& aCode, TName& aName )
    {           
    if( !IsServerAlive() )
        {
        __RDEBUG( ( _L( "iTestExecution not initialised" ) ) );
        return KErrNotReady;
        }

    TInt code;
    TName name;
    
    TParams params;
    TGetRebootStoredParamsRef getRebootStoredParamsRef( code, name );
    
    params.aRebootStoredRef = &getRebootStoredParamsRef;

    TRequestStatus status = KRequestPending; 

    // Forward
    iTestExecution->DoRemoteReceive( EStifCmdGetStoredState, params,
                                     sizeof( TRebootStateParams), status );
    
    User::WaitForRequest( status );

    // Return results if getting state is done without error
    if(status.Int() == KErrNone)
        {
        aCode = code;
        aName = name;
        }

    return status.Int();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: SetBehavior

    Description: Set test case behavior.

    Parameters: TInt aCode: inout: Get reboot releated integer value.
                TName& aName: inout: Get reboot related string value.

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: KErrNotReady returned if iTestExecution is NULL.

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::SetBehavior( TTestBehavior aType, TAny* aPtr )
    {         
    
    if( !IsServerAlive() )
        {
        return KErrGeneral;
        }

    // All event commands are handled in testserver and testengine
    return iTestExecution->SetBehavior( aType, aPtr );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: StifMacroError

    Description: STIF TF's macros. Saves information for later use.

    Parameters: TInt aMacroType: in: Macro type(0:TL, 1:T1L, 2:T2L, etc.)
                const TText8* aFile: in: Uses __FILE__ macro and this includes
                path and file name. Maximun length for this is
                KStifMacroMaxFile. If length is more then cutted from left.
                char* aFunction: in: Uses __FUNCTION__ macro and this includes
                function name. Maximun length for this is
                KStifMacroMaxFunction. If length is more then cutted from
                rigth.
                TInt aLine: in: Uses __LINE__ macro and includes line number.
                TInt aResult: in: Result from called operations.
                TInt aExpected1: in: Users expected result.
                TInt aExpected2: in: Users expected result.
                TInt aExpected3: in: Users expected result.
                TInt aExpected4: in: Users expected result.
                TInt aExpected5: in: Users expected result.

    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::StifMacroError( TInt aMacroType, 
                                             const TText8* aFile,
                                             const char* aFunction,
                                             TInt aLine,
                                             TInt aResult,
                                             TInt aExpected1,
                                             TInt aExpected2,
                                             TInt aExpected3,
                                             TInt aExpected4,
                                             TInt aExpected5 )
    {
    if( !IsServerAlive() )
        {
        return KErrGeneral;
        }

    return iTestExecution->StifMacroError( aMacroType, aFile,
                                           aFunction, aLine,
                                           aResult, aExpected1,
                                           aExpected2, aExpected3,
                                           aExpected4, aExpected5 );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: ServerAlive

    Description: Get saved store information after the reboot.

    Parameters: TInt aCode: inout: Get reboot releated integer value.
                TName& aName: inout: Get reboot related string value.

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: KErrNotReady returned if iTestExecution is NULL.

    Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CTestModuleIf::IsServerAlive()
    {
    
    if( iTestExecution == NULL )
        {
        __RDEBUG( ( _L( "iTestExecution not initialised" ) ) );
        // Execution not initialized
        return EFalse;
        }
       
    return ETrue;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: AddInterferenceThread

    Description: 

    Parameters: RThread aSTIFTestInterference: in: Handle to RThread

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: 

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleIf::AddInterferenceThread( RThread aSTIFTestInterference )
    {
    // Add thread to Array. Via array can handle test interference thread's
    // kill in panic etc. cases
    return iTestExecution->AddInterferenceThread( aSTIFTestInterference );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: RemoveInterferenceThread

    Description: 

    Parameters: RThread aSTIFTestInterference: in: Handle to RThread

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: 

    Status: Proposal

-------------------------------------------------------------------------------
*/
TInt CTestModuleIf::RemoveInterferenceThread( RThread aSTIFTestInterference )
    {
    // Remove thread from Array.Test interference thread is stopped and killed
    // successfully
    return iTestExecution->RemoveInterferenceThread( aSTIFTestInterference );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: HandleMeasurementProcess

    Description: With this can be stored information about test measurement
                 to TestServer space.

    Parameters: RProcess aTestMeasurement: in: Handle to RProcess

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: 

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestModuleIf::HandleMeasurementProcess( 
            CSTIFTestMeasurement::TStifMeasurementStruct aSTIFMeasurementInfo )
    {
    // Add process to Array. Via array can handle test measurement process's
    // kill in panic etc. cases
    return iTestExecution->HandleMeasurementProcess( aSTIFMeasurementInfo );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: GetMeasurementOptions

    Description: Get measurement option(s) given from initialization file etc.

    Parameters: TInt& aOptions: inout: Get measurement option(s)

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: KErrNotReady returned if iTestExecution is NULL.

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::GetMeasurementOptions( TInt& aOptions )
    {           
    if( !IsServerAlive() )
        {
        __RDEBUG( ( _L( "iTestExecution not initialised" ) ) );
        return KErrNotReady;
        }

    TParams params;
    TGetMeasurementOptionsRef getMeasurementOptionsRef( aOptions );
    params.aMeasurementOption = &getMeasurementOptionsRef;

    TRequestStatus status = KRequestPending; 

    // Forward
    iTestExecution->DoRemoteReceive( EStifCmdMeasurement, params,
                                     sizeof( TGetMeasurementOptions ), status );
    
    User::WaitForRequest( status );

    aOptions = getMeasurementOptionsRef.iOptions;
    
    return status.Int();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: SetAllowResult

    Description: Use with TAL, TA1L, TA2L, TA3L, TA4L and TA5L macros to allow
                 results.
                 Set test case allow result given by user. In TestScripter
                 cases allow result can set by 'allownextresult' or
                 'allowerrorcodes' keywords. In Normal and Hardcoded test
                 modules allow result can be set with this method, reset should
                 be done with ResetAllowResult method.

    Parameters: TInt aResult: in: Result value to be appended.

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::SetAllowResult( TInt aResult )
    {
    for( TInt a = 0; a < iAllowTestCaseResultsCount; a++ )
        {
        // Check that result is not given already
        if ( iTestCaseResults[a] == aResult )
            {
            return KErrNone;
            }
        }
    // If values are given more that allow(see KSTIFMacroResultArraySize).
    // Array starts from 0...9 -> 10 => 10th should fail
    if( iAllowTestCaseResultsCount >= KSTIFMacroResultArraySize )
        {
        __RDEBUG( ( 
            _L( "STIF macro's SetAllowResult() allow only %d results, fails with %d" ),
            KSTIFMacroResultArraySize, KErrOverflow  ) );
        return KErrOverflow;
        }

    // New result
    iAllowTestCaseResultsCount++;
    iTestCaseResults[iAllowTestCaseResultsCount-1] = aResult;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: CheckAllowResult

    Description: This is mainly used by STIF's TAL-TA5L macros internally.
                 Check is macros result allowed result.

    Parameters: TInt aResult: in: Result value to be checked.

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::CheckAllowResult( TInt aResult )
    {
    // Check is result allowed
    for( TInt a = 0; a < iAllowTestCaseResultsCount; a++ )
        {
        if ( iTestCaseResults[a] == aResult )
            {
            // Result is allow
            __RDEBUG( ( _L( "STIF TAL-TA5L macro's result check. Test case result[%d] allowed" ),
                aResult ) );
            return KErrNone;
            }
        }

    __RDEBUG( ( 
        _L( "STIF TAL-TA5L macro's result check. Test case result[%d] not allowed" ),
        aResult ) );
    // No match with allow result
    return KErrGeneral;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: ResetAllowResult

    Description: Use with TAL, TA1L, TA2L, TA3L, TA4L and TA5L macros to reset
                 allowed results.
                 Reset allow result(s) given with SetAllowResult. In
                 TestScripter cases this will be called automatically by STIF.
                 Normal and Hardcoded cases this should be called by user.

    Parameters: None.

    Return Values: TInt: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::ResetAllowResult()
    {
    // Used to "resets" iTestCaseResults array
    iAllowTestCaseResultsCount = 0;

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: StopExecution

    Description: Causes that test case is going to be cancelled.

    Parameters: None

    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CTestModuleIf::StopExecution(TStopExecutionType aType, TInt aCode)
    {

    //Check parameters
    if((aType == EOk) && (aCode != KErrNone))
        {
        return KErrArgument;
        }
    else if((aType != EOk) && (aCode == KErrNone))
        {
        return KErrArgument;
        }

    //Check server
    if(!IsServerAlive())
        {
        return KErrGeneral;
        }

    //Everything is ok, we can continue with processing command
    _LIT(KStopExecution, "StopExecution");
    const TInt KPrintPriority = 30;

    switch(aType)
        {
        case EOk:
            Printf(KPrintPriority, KStopExecution, _L("***Test case PASSED***\n\n"));
            break;
        case EFail:
            Printf(KPrintPriority, KStopExecution, _L("***Test case FAILED***\n\n"));
            break;
        case EAbort:
            Printf(KPrintPriority, KStopExecution, _L("***Test case KILLED***\n\n"));
            break;
        default:
            return KErrNotFound;
        }

    TStopExecutionCommandParams params;
    params.iType = aType;
    params.iCode = aCode;
    __RDEBUG((_L("CTestModuleIf::StopExecution(): type [%d] code [%d]"), TInt(aType), aCode));

    TStopExecutionCommandParamsPckg pckg(params);

    TInt res = Command(EStopExecution, pckg);
    return res;
    }

/*
------------------------------------------------------------------------------

	Class: CTestModuleIf

    Method: SendTestModuleVersion
    
    Description: SendTestModuleVersion method is used to pass version of test module
    
------------------------------------------------------------------------------
 */
EXPORT_C TInt CTestModuleIf::SendTestModuleVersion(TVersion aVersion, TFileName aModuleName)
	{
	if( aModuleName.Length() == 0 )
		{
		return KErrArgument;
		}
	
	//Check server
	if(!IsServerAlive())
	    {
	    return KErrGeneral;
	    }
	
	const TInt KPrintPriority = 30;
	_LIT(KVersion ,"SendTestModuleVersion");
	Printf(KPrintPriority, KVersion, _L("Sending test module version"));
	
	TSendTestModuleVesionCommandParams params;
	params.iMajor = aVersion.iMajor;
	params.iMinor = aVersion.iMinor;
	params.iBuild = aVersion.iBuild;
	params.iTestModuleName = aModuleName;
	
	TSendTestModuleVesionCommandParamsPckg pckg(params);
	TInt res = Command( ESendTestModuleVersion, pckg );
	return res;
	}


/*
------------------------------------------------------------------------------

	Class: CTestModuleIf

    Method: SendTestModuleVersion
    
    Description: SendTestModuleVersion method is used to pass version of test module.
    			This version uses three parameters to enable the overloading of version with
    			two parameters. The version with two params has params passed by value which
    			is incorrect. The "old" version (with two parameters) is left here not to cause
    			binary break. Only this version (with three parameters) should be used.
    
------------------------------------------------------------------------------
 */
EXPORT_C TInt CTestModuleIf::SendTestModuleVersion(TVersion& aVersion, const TDesC& aModuleName, TBool /*aNewVersion*/)
	{
	if( aModuleName.Length() == 0 )
		{
		return KErrArgument;
		}
	
	//Check server
	if(!IsServerAlive())
	    {
	    return KErrGeneral;
	    }
	
	const TInt KPrintPriority = 30;
	_LIT(KVersion ,"SendTestModuleVersion");
	Printf(KPrintPriority, KVersion, _L("Sending test module version"));
	
	TSendTestModuleVesionCommandParams params;
	params.iMajor = aVersion.iMajor;
	params.iMinor = aVersion.iMinor;
	params.iBuild = aVersion.iBuild;
	params.iTestModuleName = aModuleName;
	
	TSendTestModuleVesionCommandParamsPckg pckg(params);
	TInt res = Command( ESendTestModuleVersion, pckg );
	return res;
	}

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: Command

    Description: Sends specific command to TestServer.

    Parameters: aCommand - command to be send

    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
TInt CTestModuleIf::Command(TCommand aCommand, const TDesC8& aParamsPckg)
    {
    TInt res = KErrNone;

    if( !IsServerAlive() )
        {
        return KErrGeneral;
        }

    switch(aCommand)
        {
        case EStopExecution:
            iTestExecution->DoNotifyCommand(aCommand, aParamsPckg);
            break;
        case ESendTestModuleVersion:
        	iTestExecution->DoNotifyCommand(aCommand, aParamsPckg);
        	break;
        default:
            __RDEBUG((_L("Command [%d] not recognized."), aCommand));
        }

    return res;
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: GetTestCaseTitleL

    Description: Returns title of current test case.

    Parameters: aCommand - command to be send

    Return Values: Symbian OS error code.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C void CTestModuleIf::GetTestCaseTitleL(TDes& aTestCaseTitle)
    {
    iTestExecution->GetTestCaseTitleL(aTestCaseTitle);
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: GetTestCaseArguments

    Description: Returns test case arguments.

    Parameters: none

    Return Values: test case arguments.

    Errors/Exceptions: None

    Status: proposal

-------------------------------------------------------------------------------
*/
EXPORT_C const TDesC& CTestModuleIf::GetTestCaseArguments() const
    {
    return iTestExecution->GetTestCaseArguments();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: UITesting

    Description: Gets information if it is UI test or not

    Parameters: none

    Return Values: True if it is UI test, in other case it returns false.

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TBool CTestModuleIf::UITesting()
	{
	return iTestExecution->UITesting();
	}

/*
-------------------------------------------------------------------------------

    Class: CTestModuleIf

    Method: GetUiEnvProxy

    Description: Gets UiEnvProxy

    Parameters: none

    Return Values: Pointer to UiEnvProxy

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CUiEnvProxy* CTestModuleIf::GetUiEnvProxy()
	{
	return iTestExecution->GetUiEnvProxy();
	}

// ================= OTHER EXPORTED FUNCTIONS =================================


/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CSTIFTestMeasurement class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestMeasurement

    Method: CSTIFTestMeasurement

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: CTestModuleBase* aTestModuleBase: in: Pointer to STIF

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFTestMeasurement::CSTIFTestMeasurement( CTestModuleBase* aTestModuleBase ) :
    iTestModuleBase( aTestModuleBase ),
    iMeasurementOption( 0 )
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestMeasurement

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: TSTIFMeasurement aType: in: Measurement type
                const TDesC& aConfigurationInfo: in: Configuration issues

    Return Values: None

    Errors/Exceptions: Leaves if HandleMeasurementProcess returns error code

    Status: Approved

-------------------------------------------------------------------------------
*/
void CSTIFTestMeasurement::ConstructL( TSTIFMeasurementType aType,
                                    const TDesC& aConfigurationInfo )
    {
    // aType into struct here. This is compared to
    iMeasurementStruct.iMeasurementType = aType;

    // Check that measurement is not disabled(This is given from UI level or
    // from TestFramework.ini file)
    TInt ret( 0 );
    ret = iTestModuleBase->TestModuleIf().GetMeasurementOptions( 
                                                        iMeasurementOption );
    if( ret != KErrNone )
        {
        __RDEBUG( ( _L( "GetMeasurementOptions() return an error[%d]" ), ret ) );
        User::Leave( ret );
        }

    // Check that measurement is not disabled
    if( ( iMeasurementOption & EDisableAll ) == EDisableAll )
        {
        __RDEBUG( _L( "Measurement disable option [KStifMeasurementDisableAll] is given by user" ) );
        return;
        }
    if( ( ( iMeasurementOption & EMeasurement01 ) == EMeasurement01 ) && aType == KStifMeasurementPlugin01 )
        {
        __RDEBUG( _L( "Measurement disable option [KStifMeasurement01] is given by user" ) );
        return;
        }
    if( ( ( iMeasurementOption & EMeasurement02 ) == EMeasurement02 ) && aType == KStifMeasurementPlugin02 )
        {
        __RDEBUG( _L( "Measurement disable option [KStifMeasurement02] is given by user" ) );
        return;
        }
    if( ( ( iMeasurementOption & EMeasurement03 ) == EMeasurement03 ) && aType == KStifMeasurementPlugin03 )
        {
        __RDEBUG( _L( "Measurement disable option [KStifMeasurement03] is given by user" ) );
        return;
        }
    if( ( ( iMeasurementOption & EMeasurement04 ) == EMeasurement04 ) && aType == KStifMeasurementPlugin04 )
        {
        __RDEBUG( _L( "Measurement disable option [KStifMeasurement04] is given by user" ) );
        return;
        }
    if( ( ( iMeasurementOption & EMeasurement05 ) == EMeasurement05 ) && aType == KStifMeasurementPlugin05 )
        {
        __RDEBUG( _L( "Measurement disable option [KStifMeasurement05] is given by user" ) );
        return;
        }
    if( ( ( iMeasurementOption & EBappea ) == EBappea ) && aType == KStifMeasurementBappeaProfiler )
        {
        __RDEBUG( _L( "Measurement disable option [KStifMeasurementBappea] is given by user" ) );
        return;
        }

    iMeasurementStruct.iConfigurationInfo = aConfigurationInfo;
    iMeasurementStruct.iMeasurementOperation = KMeasurementNew;
    iMeasurementStruct.iPointerToMeasurementModule = iTestModuleBase;

    // Move measurement execution initialization forward to TestServer.
    User::LeaveIfError( iTestModuleBase->iTestModuleIf->HandleMeasurementProcess(
            iMeasurementStruct ) );

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestMeasurement

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestModuleBase* aTestModuleBase: in: Pointer to STIF.
                TSTIFMeasurement aType: in: Measurement type.
                const TDesC& aConfigurationInfo: in: Configuration info.
    
    Return Values: CSTIFTestMeasurement object.

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C CSTIFTestMeasurement* CSTIFTestMeasurement::NewL( 
                                            CTestModuleBase* aTestModuleBase,
                                            TSTIFMeasurementType aType, 
                                            const TDesC& aConfigurationInfo )
    {
    CSTIFTestMeasurement* self = 
            new (ELeave) CSTIFTestMeasurement( aTestModuleBase );

    CleanupStack::PushL( self );
    self->ConstructL( aType, aConfigurationInfo );

    CleanupStack::Pop();

    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestMeasurement

    Method: ~CSTIFTestMeasurement

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CSTIFTestMeasurement::~CSTIFTestMeasurement()
    {
    // None

    }

/*
-------------------------------------------------------------------------------

    Class: CSTIFTestMeasurement

    Method: Start

    Description: Start commant for measurement.

    Parameters: None

    Return Values: Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestMeasurement::Start()
    {
    // Check that measurement is not disabled
    if( iMeasurementOption  == iMeasurementStruct.iMeasurementType )
        {
        __RDEBUG( ( _L( "Measurement with type[%d] is disabled by user" ), iMeasurementOption ) );
        // Cannot return error code because this causes problems in
        // TestScripter and TestCombiner error handling. Now testing continue
        // without measurement.
        return KErrNone;
        }

    iMeasurementStruct.iMeasurementOperation = KMeasurementStart;

    // Move measurement start execution forward to TestServer.
    return iTestModuleBase->iTestModuleIf->HandleMeasurementProcess(
                                                        iMeasurementStruct );

    }

/*
-------------------------------------------------------------------------------

    Class: CTestMeasurement

    Method: Stop

    Description: Stop command for measurement.

    Parameters: None

    Return Values: Symbian error code

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
EXPORT_C TInt CSTIFTestMeasurement::Stop()
    {
    // Check that measurement is not disabled
    if( iMeasurementOption  == iMeasurementStruct.iMeasurementType )
        {
        __RDEBUG( ( _L( "Measurement with type[%d] is disabled by user" ), iMeasurementOption ) );
        // Cannot return error code because this causes problems in
        // TestScripter and TestCombiner error handling. Now testing continue
        // without measurement.
        return KErrNone;
        }

    iMeasurementStruct.iMeasurementOperation = KMeasurementStop;

    // Move measurement stop execution forward to TestServer.
    return iTestModuleBase->iTestModuleIf->HandleMeasurementProcess(
                                                        iMeasurementStruct );

    }

// End of File
