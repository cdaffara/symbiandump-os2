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
* Description: This module contains implementation of CMenuNotifier 
* and CMenuDialog class implementations.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>
#include <f32file.h>
#include "ConsoleUI.h"
#include "ConsoleMenus.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KDefaultTime = 2;

// MACROS
#ifndef __WINS__
  #define GETPTR &
#else
  #define GETPTR
#endif

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ==================== LOCAL FUNCTIONS =======================================

// None

   

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CMenuNotifier

    Method: NewL

    Description: Constructs new menu notifier

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenuNotifier* CMenuNotifier::NewL( const TDesC& aError, CConsoleMain* aMain )
    {

    CMenuNotifier* self = new ( ELeave ) CMenuNotifier( aMain );
    CleanupStack::PushL( self );
    self->ConstructL( aError );
    CleanupStack::Pop( self );
    return self;

    }


/*
-------------------------------------------------------------------------------

    Class: CMenuNotifier

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMenuNotifier::ConstructL( const TDesC& aError )
    {
    
    TSize size = iMain->GetConsole()->ScreenSize();
    size.iWidth = Min( size.iWidth - KMenuOverhead, 
                       aError.Length() );
    size.iHeight = aError.Length()/size.iWidth + 2;
    
    iConsole = Console::NewL( _L("Error note"), size );
    
    iConsole->Printf( _L("Error:") );    
    iConsole->Printf( aError ); 
    iConsole->Printf( _L("\n") );
    iConsole->Printf( _L("Press any key") );

    iTimer.CreateLocal();

    CActiveScheduler::Add ( &iCallBack1 );
    CActiveScheduler::Add ( &iCallBack2 );

    // Get timer
    iTimer.After( iCallBack1.Activate(), 10000000 );

    // Poll keypresses
    iConsole->Read ( iCallBack2.Activate() );        

    }

/*
-------------------------------------------------------------------------------

    Class: CMenuNotifier

    Method: CMenuNotifier

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
#pragma warning( disable : 4355 )       // Incomplete this usage
CMenuNotifier::CMenuNotifier( CConsoleMain* aMain ): 
    iCallBack1 ( this, GETPTR CMenuNotifier::Run1 ),
    iCallBack2 ( this, GETPTR CMenuNotifier::Run1 ),
    iMain( aMain )          
    {
    }
#pragma warning( default : 4355 )

/*
-------------------------------------------------------------------------------
e
    Class: CMenuNotifier

    Method: ~CMenuNotifier

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CMenuNotifier::~CMenuNotifier()
    {
    iTimer.Close();
    delete iConsole;    

    }



/*
-------------------------------------------------------------------------------

    Class: CMenuNotifier

    Method: Run1

    Description: Callback function. Closes dialog

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CMenuNotifier::Run1()
    {

    if ( iCallBack1.isCompleted() && iCallBack2.isCompleted() )
        {
        // Both callback have been done, delete this object
        delete this;
        return;
        }
    else
        {
        // One callback finished, stop listening anything else
        iConsole->ReadCancel();
        iTimer.Cancel();
        }
    }  


// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CMenuDialog

    Method: NewL

    Description: Constructs new menu dialog

    Parameters: const TDesC& aMessage: in: Message to dialog

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenuDialog* CMenuDialog::NewL( CConsoleMain* aMain,
                                const TDesC& aLine1, 
                                const TDesC& aLine2, 
                                TInt aTimeInSecs )
    {

    CMenuDialog* self = new ( ELeave ) CMenuDialog( aMain );
    CleanupStack::PushL( self );
    self->ConstructL( aLine1, aLine2, aTimeInSecs );
    CleanupStack::Pop( self );
    return self;

    }


/*
-------------------------------------------------------------------------------

    Class: CMenuDialog

    Method: ConstructL

    Description: Second level constructor.

    Parameters: const TDesC& aMessage: in: Message to dialog
    
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMenuDialog::ConstructL( const TDesC& aLine1, 
                              const TDesC& aLine2, 
                              TInt aTimeInSecs )
    {
    
    TSize size = iMain->GetConsole()->ScreenSize();
    size.iWidth = Min( size.iWidth - KMenuOverhead, 
                       Max( aLine1.Length(), aLine2.Length() ) );
    size.iHeight = aLine1.Length()/size.iWidth + aLine2.Length()/size.iWidth + 2;
    
    iConsole = Console::NewL( _L("Info"), size );
    
    if( aLine1.Length() > 0 )
        {
        iConsole->Printf( aLine1 );
        }
    if( aLine2.Length() > 0 )
        {
        iConsole->Printf( _L("\n") );
        iConsole->Printf( aLine2 );
        }
    
    if( aTimeInSecs == 0 )
        {
        aTimeInSecs = KDefaultTime;
        }
    iTimer.CreateLocal();

    CActiveScheduler::Add ( &iCallBack1 );
    // Get timer
    iTimer.After( iCallBack1.Activate(), aTimeInSecs*1000*1000 );
           
    }

/*
-------------------------------------------------------------------------------

    Class: CMenuDialog

    Method: CMenuDialog

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
#pragma warning( disable : 4355 )       // Incomplete this usage
CMenuDialog::CMenuDialog( CConsoleMain* aMain ):  
    iMain( aMain ),      
    iCallBack1 ( this, GETPTR CMenuDialog::Run1 ), 
    iCallBack2 ( this, GETPTR CMenuDialog::Run1 )
    {
    }
#pragma warning( default : 4355 )

/*
-------------------------------------------------------------------------------
e
    Class: CMenuDialog

    Method: ~CMenuDialog

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CMenuDialog::~CMenuDialog()
    {
 
    //if( iCallBack1.IsActive() )
    //    {
        iTimer.Cancel();
        iTimer.Close();
        iCallBack1.Cancel();
    //    }
        
    if( iCallBack2.IsActive() )
        {
        User::RequestComplete( iStatus, KErrCancel );
		if( iConsole != NULL )
			{
			iConsole->ReadCancel();    
			}
        iCallBack2.Cancel();
        }
        
    delete iConsole;    

    }

/*
-------------------------------------------------------------------------------

    Class: CMenuDialog

    Method: Run1

    Description: Callback function. Closes dialog

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CMenuDialog::Run1()
    {

    if ( iCallBack2.isCompleted() )
        {            
        *iKeyCode = iConsole->KeyCode();
    
        User::RequestComplete( iStatus, iCallBack2.Status().Int() );
        }
        
    TInt index = iMain->iDialogs.Find( this );
    if( index >= 0 )
        {
        iMain->iDialogs.Remove( index );
        }
    // Timeout, delete this object
    delete this;
    return;

    }  


/*
-------------------------------------------------------------------------------

    Class: CMenuDialog

    Method: WaitForKeypress

    Description: Wait for keypress

    Parameters: None

    Return Values: Symbian OS error code

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CMenuDialog::WaitForKeypress( TKeyCode& aKeyCode, 
                                   TRequestStatus& aStatus )
    {
    
    aStatus = KRequestPending;
    iKeyCode = &aKeyCode;
    iStatus = &aStatus;
    
    CActiveScheduler::Add ( &iCallBack2 );
     
    // Poll keypresses
    iConsole->Read( iCallBack2.Activate() );        

    return KErrNone;
    
    }  

// ================= OTHER EXPORTED FUNCTIONS =================================

// End of File
