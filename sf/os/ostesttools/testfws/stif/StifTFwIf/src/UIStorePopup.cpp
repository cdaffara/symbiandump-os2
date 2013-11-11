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
* Description: CUIStorePopup: This object executes test cases 
* from STIF Test Framework.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32svr.h>
#include <stifinternal/UIStore.h>
#include "UIStorePopup.h"
#include "Logging.h"


// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS

// MACROS
#ifdef LOGGER
#undef LOGGER
#endif
#define LOGGER iUIStore->iLogger


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

    Class: CUIStorePopup

    Method: CUIStorePopup

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: None
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIStorePopup::CUIStorePopup( CUIStore* aUIStore,
                              CUIEngineContainer* aContainer,
                              CStifTFwIfProt* aRemoteMsg,                             
                              CUIStore::TPopupPriority aPopupPriority) :
    CActive( CActive::EPriorityStandard ),
    iUIStore( aUIStore ),
    iRemoteMsg( aRemoteMsg ),
    iContainer( aContainer ),
    iPopupPriority( aPopupPriority )
    {
    __TRACE( KPrint, ( _L( "CUIStorePopup::CUIStorePopup") ) );
    __ASSERT_ALWAYS( aUIStore, User::Panic( _L("Null pointer"), KErrGeneral ) );

    CActiveScheduler::Add( this );
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStorePopup

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if called Open method returns error

    Status: Approved

-------------------------------------------------------------------------------
*/
void CUIStorePopup::ConstructL( const TDesC& aMsg )
    {    
    iMsg = aMsg.AllocL();
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStorePopup

    Method: NewL

    Description: Two-phased constructor.
    
    Parameters: CUIStore* CUIStore: in: Pointer to CUIStore Interface
                TTestInfo& aTestInfo: in: Test info

    Return Values: CUIStorePopup* : Pointer to created runner object

    Errors/Exceptions: Leaves if memory allocation for CUIStorePopup fails
                       Leaves if ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIStorePopup* CUIStorePopup::NewL( CUIStore* aUIStore,
                                    CUIEngineContainer* aContainer,
                                    CStifTFwIfProt* aRemoteMsg,
                                    CUIStore::TPopupPriority aPopupPriority,
                                    const TDesC& aMsg )
    {
    
    CUIStorePopup* self =  
        new ( ELeave ) CUIStorePopup( aUIStore, aContainer, aRemoteMsg, aPopupPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aMsg );
    CleanupStack::Pop();
    return self;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CUIStorePopup

    Method: ~CUIStorePopup

    Description: Destructor
    
    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CUIStorePopup::~CUIStorePopup()
    {
    __TRACE( KPrint, ( _L( "CUIStorePopup::~CUIStorePopup()") ) );
    Cancel();
    
    delete iMsg;
    delete iRemoteMsg;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStorePopup

    Method: Start

    Description: Call SETActive and return TRequestStatus member.

    Parameters: None

    Return Values: Symbian OS error code
    
    Errors/Exceptions: None
    
    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CUIStorePopup::Start( const TDesC& aLine1, 
                           const TDesC& aLine2 )
    {
    __TRACE( KPrint, ( _L( "CUIStorePopup::Start") ) );
    
    if(IsActive())
    { 
        __TRACE( KPrint, ( _L( "CUIStorePopup already started") ) );
       return KErrNone;  
    }
          
    iPopupWindow = iUIStore->iUIStoreIf->PopupMsg( 
        aLine1, aLine2, KDefaultPopupTimeout, iKey, iStatus );
    
    if( iPopupWindow < 0 )
        {
        return iPopupWindow;
        }

    SetActive();
    return KErrNone;
    
    }
/*
-------------------------------------------------------------------------------

    Class: CUIStorePopup

    Method: RunL

    Description: RunL handles completed requests.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if iStatus is not KErrNone, error is handled in
                       RunError called by CActiveObject

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIStorePopup::RunL()
    {
   
    __TRACE( KPrint, ( _L( "CUIStorePopup::RunL") ) );
    
    iUIStore->RemotePopupComplete( this, iStatus.Int(), 
                                   iContainer, iRemoteMsg, iKey );
        
    }

/*
-------------------------------------------------------------------------------

    Class: CUIStorePopup

    Method: DoCancel

    Description: Cancel active request.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CUIStorePopup::DoCancel()
    {
    __TRACE( KPrint, ( _L( "CUIStorePopup::DoCancel") ) );
    
    iUIStore->iUIStoreIf->Close( iPopupWindow );
        
    }

/*
-------------------------------------------------------------------------------

    DESCRIPTION

    CActiveTimer: This object prints running seconds to console screen.

-------------------------------------------------------------------------------
*/

// ================= MEMBER FUNCTIONS =========================================




// ================= OTHER EXPORTED FUNCTIONS ================================= 
// None

// End of File
