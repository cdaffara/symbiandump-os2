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
* Description: This module contains the implementation of 
* CStartInfo class member functions.
*
*/


// INCLUDE FILES
#include <f32file.h>
#include <StifLogger.h>
#include "TestCombinerUtils.h"
#include "Logging.h"

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
#define LOGGER iLog

// LOCAL CONSTANTS AND MACROS
// None
 
// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: CStartInfo

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CStartInfo::CStartInfo():
    iModule(0,0),
    iCaseNum(0),
    iExpectedResult(0),
    iTimeout(0),
    iModuleBuf(0),
    iIniFileBuf(0),
    iConfigBuf(0),
    iTestIdBuf(0),
    iTitleBuf(0),
	iTestCaseArgumentsBuf(0)
    {

    iCategory = TFullTestResult::ECaseExecuted; 

    };
     
/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CStartInfo::ConstructL()
    {    
    }

/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters:    None
     
     Return Values: CStartInfo*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CStartInfo* CStartInfo::NewL()
    {

    CStartInfo* self = new (ELeave) CStartInfo();
     
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }    
    
/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: ~CStartInfo

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/     
CStartInfo::~CStartInfo()
    {

    delete iModuleBuf;
    delete iIniFileBuf;
    delete iConfigBuf;
    delete iTestIdBuf;
    delete iTitleBuf;
	delete iTestCaseArgumentsBuf;
    }

/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: SetModuleNameL

     Description: Set module name.
     
     Parameters: TDesC& aModule: in: Module name 
                 TInt aExtLength: in: Extra length reserved for buffer
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
 void CStartInfo::SetModuleNameL( TDesC& aModule, TInt aExtLength )
    {

    iModuleBuf = HBufC::NewL( aModule.Length() + aExtLength );
    iModule.Set( iModuleBuf->Des() );
    iModule.Append( aModule );
    
    // Remove optional '.DLL' from file name
    iModule.LowerCase();
    TParse parse;
    parse.Set( iModule, NULL, NULL );
    
    if ( parse.Ext() == _L(".dll") )
        {
        const TInt len = parse.Ext().Length();
        iModule.Delete ( iModule.Length()-len, len );
        }


    };
            
/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: SetIniFileL

     Description: Set initialization file name name.
     
     Parameters: TDesC& aIni: in: Initialization file name
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/        
void CStartInfo::SetIniFileL( TDesC& aIni )
    {

    iIniFileBuf = aIni.AllocL();
    iIniFile.Set( iIniFileBuf->Des() );

    };
            
/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: SetConfigL

     Description: Set configuration file name name.
     
     Parameters: TDesC& aConfig: in: Configuration file name
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/        
void CStartInfo::SetConfigL( TDesC& aConfig )
    {

    iConfigBuf = aConfig.AllocL();
    iConfig.Set( iConfigBuf->Des() );

    };
            
/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: SetTestIdL

     Description: Set test identifier.
     
     Parameters: TDesC& aTestId: in: test identifier
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/        
void CStartInfo::SetTestIdL( TDesC& aTestId )
    {

    iTestIdBuf = aTestId.AllocL();
    iTestId.Set( iTestIdBuf->Des() );

    };

/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: DeleteModuleName

     Description: Delete module name buffer for creating new module name.
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/        
void CStartInfo::DeleteModuleName()
    {
    delete iModuleBuf;
	iModuleBuf = NULL;
    };

/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: SetTitleL

     Description: Set title.
     
     Parameters: TDesC& aTitle: in: Test case title
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/        
void CStartInfo::SetTitleL(TDesC& aTitle)
    {

    iTitleBuf = aTitle.AllocL();
    iTitle.Set(iTitleBuf->Des());
    }

/*
-------------------------------------------------------------------------------

     Class: CStartInfo

     Method: SetTestCaseArguments

     Description: Sets test case arguments
     
     Parameters:  const TDesC& aTestCaseArguments: in: test case arguments.
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/        
void CStartInfo::SetTestCaseArgumentsL( const TDesC& aTestCaseArguments )
    {
    delete iTestCaseArgumentsBuf;
	iTestCaseArgumentsBuf = NULL;
    iTestCaseArgumentsBuf = aTestCaseArguments.AllocL();
	iTestCaseArguments.Set( iTestCaseArgumentsBuf->Des() );
    }


/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CSlaveInfo class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CSlaveInfo

     Method: CSlaveInfo

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: TUint32 aMaster: in: Master id

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CSlaveInfo::CSlaveInfo( TUint32 aMaster ):
    iState(ESlaveIdle),
    iMasterId(aMaster),
    iSlaveDevId(0)
    {
    
    }
     
/*
-------------------------------------------------------------------------------

     Class: CSlaveInfo

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    TDesC& aName: in: Slave name

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CSlaveInfo::ConstructL( TDesC& aName )
    {    

    iNameBuf = aName.AllocL();
    iName.Set( iNameBuf->Des() );
            
    }

/*
-------------------------------------------------------------------------------

     Class: CSlaveInfo

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters:    TDesC& aName: in: Slave name
                    TUint32 aMaster: in: Master id

     Return Values: CSlaveInfo*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CSlaveInfo* CSlaveInfo::NewL( TDesC& aName, TUint32 aMaster )
    {

    CSlaveInfo* self = new (ELeave) CSlaveInfo( aMaster);
     
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop();

    return self;

    }    
    
/*
-------------------------------------------------------------------------------

     Class: CSlaveInfo

     Method: ~CSlaveInfo

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/     
CSlaveInfo::~CSlaveInfo()
    {
    
    iEvents.ResetAndDestroy();
    iEvents.Close();
    delete iNameBuf;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CSlaveInfo

     Method: GetEvent

     Description: Returns event with given name.

     Parameters:  TDesC& aEventName: in; Event name

     Return Values: TEventTc: Event structure

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TEventTc* CSlaveInfo::GetEvent( TDesC& aEventName )
    {
    
    TInt count = iEvents.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iEvents[i]->Name() == aEventName )
            {
            return iEvents[i];
            }
        }
    return NULL;
    
    }
    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of CDefinedValue class 
    member functions.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: CDefinedValue

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CDefinedValue::CDefinedValue()
    {
    }
     
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: ConstructL

     Description: Symbian OS second phase constructor

     Symbian OS default constructor can leave.

     Parameters:    TDesC& aName: in: Define name
                    TDesC& aValue: in: Define value

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CDefinedValue::ConstructL( TDesC& aName, TDesC& aValue )
    {    

    iNameBuf = aName.AllocLC();
    iName.Set( iNameBuf->Des() );
    iValueBuf = aValue.AllocLC();
    iValue.Set( iValueBuf->Des() );
    CleanupStack::Pop( iValueBuf );
    CleanupStack::Pop( iNameBuf );

    }

/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: NewL

     Description: Two-phased constructor.
          
     Parameters:    TDesC& aName: in: Define name
                    TDesC& aValue: in: Define value

     Return Values: CDefinedValue*: new object

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CDefinedValue* CDefinedValue::NewL( TDesC& aName, TDesC& aValue )
    {

    CDefinedValue* self = new (ELeave) CDefinedValue();
     
    CleanupStack::PushL( self );
    self->ConstructL( aName, aValue );
    CleanupStack::Pop();

    return self;

    }    
    
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: ~CDefinedValue

     Description: Destructor
     
     Parameters:    None

     Return Values: None

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/     
CDefinedValue::~CDefinedValue()
    {
    
    delete iValueBuf;
    delete iNameBuf;
    
    }
    
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: SetValueL

     Description: Set new define value

     Parameters:    TDesC& aValue: in: Define value

     Return Values: None

     Errors/Exceptions: Leaves on error.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CDefinedValue::SetValueL( TDesC& aValue )
    {
    delete iValueBuf;
    iValueBuf = 0;
    iValueBuf = aValue.AllocLC();
    iValue.Set( iValueBuf->Des() );
    CleanupStack::Pop( iValueBuf );
    
    }

    
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: Name

     Description: Returns define name.

     Parameters:  None

     Return Values: TDesC: Define name

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TDesC& CDefinedValue::Name()
    { 
    return iName; 
    }
        
/*
-------------------------------------------------------------------------------

     Class: CDefinedValue

     Method: Value

     Description: Returns define value.

     Parameters:  None

     Return Values: TDesC: Define value

     Errors/Exceptions: None

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TDesC& CDefinedValue::Value()
    { 
    return iValue; 
    }    
    
/*
-------------------------------------------------------------------------------

    DESCRIPTION

    This module contains the implementation of TEventTc class 
    member functions. 

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

     Class: TEventTc

     Method: TEventTc

     Description: Default constructor

     C++ default constructor can NOT contain any code, that
     might leave.
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
TEventTc::TEventTc( CStifLogger* aLogger ):
    iReq(NULL),
    iLogger( aLogger )
    {
    __TRACE( KMessage, (_L("TEventTc::TEventTc") ) );
    }
        
/*
-------------------------------------------------------------------------------

     Class: TEventTc

     Method: TEventTc

     Description: Parametric constructor

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/ 
TEventTc::TEventTc( TName& aEventName, CStifLogger* aLogger ):
    iReq(NULL),
    iLogger( aLogger )
    {
    SetName( aEventName );
    SetType( EReqEvent );
    __TRACE( KMessage, (_L("TEventTc::TEventTc %S"), &aEventName ) );
    __TRACE( KMessage, (_L("TEvent::SetType Req") ) );
    }

/*
-------------------------------------------------------------------------------

     Class: TEventTc

     Method: ~TEventTc

     Description: Destructor

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/ 
TEventTc::~TEventTc()
    { 
    __TRACE( KMessage, (_L("TEventTc::~TEventTc %S"), &Name() ) );
    Complete( KErrNone ); 
    }
/*
-------------------------------------------------------------------------------

     Class: TEventTc

     Method: SetRequestStatus

     Description: Set request status member.

     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/ 
void TEventTc::SetRequestStatus( TRequestStatus* aStatus )
    { 
    iReq = aStatus; 
    *iReq = KRequestPending;
    __TRACE( KMessage, (_L("TEventTc::SetRequestStatus %S"), &Name() ) );
    }

/*
-------------------------------------------------------------------------------

     Class: TEventTc

     Method: Complete

     Description: Complete request status member.
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/ 
void TEventTc::Complete( TInt aError )
    { 
    if( iReq )
        { 
        __TRACE( KMessage, (_L("TEventTc::Complete %S"), &Name() ) );
        User::RequestComplete(iReq, aError ); 
        }
    }

/*
-------------------------------------------------------------------------------

     Class: TEventTc

     Method: SetEvent

     Description: Set event pending.
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/ 
void TEventTc::SetEvent( TEventType aEventType )
    {
    __TRACE( KMessage, (_L("TEventTc::SetEvent %S, type %d"),   
        &Name(), aEventType ) );

    SetEventType( aEventType );
    if( iReq )
        {
        Complete( KErrNone );
        if( EventType() == EState )
            {
            __TRACE( KMessage, (_L("TEvent::SetType Set") ) );
            SetType( ESetEvent ) ;
            }
        }
    else 
        {
        __TRACE( KMessage, (_L("TEvent::SetType Set") ) );
        SetType( ESetEvent ) ;
        }
    }
   
/*
-------------------------------------------------------------------------------

     Class: TEventTc

     Method: WaitEvent

     Description: Wait event.
     
     Parameters: None
     
     Return Values: None

     Errors/Exceptions: None

     Status: Proposal
    
-------------------------------------------------------------------------------
*/
void TEventTc::WaitEvent( TRequestStatus& aStatus )
    {
    __TRACE( KMessage, (_L("TEventTc::WaitEvent %S"), &Name() ) );
    SetRequestStatus( &aStatus );
    if( Type() == ESetEvent )
        {
        Complete( KErrNone );
        if( EventType() == EIndication )
            {
            __TRACE( KMessage, (_L("TEvent::SetType Req") ) );
            SetType( EReqEvent );
            }
        }
    }
    
// ================= OTHER EXPORTED FUNCTIONS =================================
// None

// End of File
