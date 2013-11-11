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
* Description: This module contains implementation of 
* CMenu-derived classes.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>

#include <f32file.h>
#include <stifinternal/UIEngineContainer.h>

#include "ConsoleUI.h"
#include "ConsoleMenus.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void LimitedAppend( TDes& aOriginal, const TDesC& aAppend);

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================

LOCAL_C void LimitedAppend( TDes& aOriginal, const TDesC& aAppend)
    {

    TInt spaceLeft = aOriginal.MaxLength() - aOriginal.Length();
    
    if (spaceLeft > aAppend.Length())
        {
        aOriginal.Append ( aAppend );
        }
    else
        {       
        aOriginal.Append ( aAppend.Left ( spaceLeft ) );
        }

    }

// None

// ================= MEMBER FUNCTIONS =========================================


/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CMenu::NewL( CConsoleMain* aConsole,
                    CMenu* aParent,
                    const TDesC& aName
                  )
    {

    CMenu* self = new ( ELeave ) CMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
				const TDesC& aHeader	  :in:		Text containing STIF version info
				
    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMenu::ConstructL( CConsoleMain* aConsole,
                        CMenu* aParent,
                        const TDesC& aName,
                        const TDesC& aHeader
                      )
    {
 
    // Store the parameters
    iMain = aConsole;
    iConsole = aConsole->GetConsole();
    iParent = aParent;
    iName = aName;
    iHeader = aHeader;

    // Get display size
    const TInt KOverHead = 5;
    iSize = iConsole->ScreenSize();
    iScreenSize = iSize.iHeight - KOverHead;   // Size available for menus

    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: CMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu::CMenu()
    {
    iDirection = 1;
    iPrevPos  = -1;     // Invalid starting value
    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: ~CMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CMenu::~CMenu()
    {
    
    iItems.ResetAndDestroy();
    
    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: TimerUpdate

    Description: TimerUpdate

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CMenu::TimerUpdate()
    {    
    iScrolledLine.Zero();
    RRefArray<TDesC> texts;
    User::LeaveIfError( ItemTexts( texts ) );
    
    TInt count = texts.Count();
    // If list is empty, do not scroll.
    if ( count < 1 )
        {
        texts.Close();
        return;
        }
    
    // Recalculate visible part of menu
    Recalculate( count );

    // If menu selection is updated, then restart scrolling
    if (iPrevPos != iFirst + iPosOnScreen)
        {
        iPrevPos = iFirst + iPosOnScreen;
        iStart = 0;
        iDirection = 1;
        }


    // If menu item have not been changed after last timer, then
    // start scrolling  
    const TDesC& name = texts[ iFirst + iPosOnScreen ];
    if ( name.Length() > ( iSize.iWidth - KMenuOverhead ) )
        {

        TInt y = iConsole->WhereY();
        TInt x = iConsole->WhereX();
        TBuf<80> iTmp;              

        iStart = iStart + iDirection;

        // "Right end"
        if ( iStart + iSize.iWidth > name.Length() + KMenuOverhead + 2)
            {
            iStart--;
            iDirection = -1;
            }
        
        // "Left end"
        if ( iStart == -1 )
            {
            iStart++;
            iDirection = 1;
            }
        
        if( iStart > name.Length() )
            {
            iStart = 0;
            }

        iTmp=_L(" *");
        LimitedAppend( iTmp, name.Mid ( iStart ) );
            
        iConsole->SetPos( 0, iPosOnScreen+1);       
        Print(iTmp);
        iScrolledLine.Copy(texts[iFirst + iPosOnScreen].Left(iScrolledLine.MaxLength())); 

        iConsole->SetPos(x,y);
        }
    texts.Close();

    }
    

/*
-------------------------------------------------------------------------------
    Class: CMenu

    Method: AppendBefore

    Description: Append text before line.

    Parameters: TInt aLine: in: line number 
                TDes& aLine: in: line text

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CMenu::AppendBefore( TInt aLineNum, TDes& aLine )
    {
    
    if( ( aLine.MaxLength() - aLine.Length() ) < 2 )
        {
        return;
        }        
    
    // If printing active line, print the marker
    if ( aLineNum == iPosOnScreen + iFirst )
        {
        aLine.Append( _L(" *") );
        }
    else
        {
        aLine.Append( _L("  ") );
        }
        
    }

/*
-------------------------------------------------------------------------------
    Class: CMenu

    Method: PrintMenuL

    Description: Prints the menu

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CMenu::PrintMenuL( TUpdateType aType )
    {

    iUpdateType = aType;
    
    TBuf<KMaxLineLength> line;
    
    //if we are moving between items located on the same page clearing screen is not needed
    if(!iPreventClearScreen)
        {
        // Clear display
        iConsole->ClearScreen();
        
        //Print STIF version information, only when defined (i.e. in main menu)
        if(iHeader != _L(""))
           Print(iHeader);
        
        // Print menu title
        Print( Name() );
        
        RRefArray<TDesC> texts;
        User::LeaveIfError( ItemTexts( texts ) );
        
        // Recalculate visible part of menu
        Recalculate( texts.Count() );
        
        //needed for setting cursor at correct line 
        iMenuItemsListStartIndex = iConsole->CursorPos().iY;
        
        // Print items
        for ( TInt i = iFirst; i <= iLast; i++ )
            {
            line.Zero();
            
            // Append text before line
            AppendBefore( i, line );
        
            // Get the menu line
            LimitedAppend ( line, texts[i] );
        
            // Print the line
            Print(line);
            }
        texts.Close();
        
        //needed for setting cursor at correct line 
        iMenuItemsListEndIndex = iConsole->CursorPos().iY;
        }
    else
        {
        //removing "*" sign from the previous cursor position
        iConsole->SetPos(0, iPrevPosOnScreen + iMenuItemsListStartIndex);
        //line.Append( _L("  ") );
        AppendBefore(iPrevPosOnScreen + iFirst, line);
        LimitedAppend(line, iScrolledLine);
        //iConsole->Printf(line);
        Print(line);
        iConsole->SetPos(0, iPosOnScreen + iMenuItemsListStartIndex);
        line.Zero();
        iScrolledLine.Zero();
        
        //writing "*" sign before the currently selected line
        line.Append( _L(" *") );
        iConsole->Printf(line);
        iConsole->SetPos(0, iMenuItemsListEndIndex);
        iPreventClearScreen = EFalse;
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: SelectL

    Description: Common method to count all variables needed for printing
                 page of the screen.

    Parameters: TInt aDelta       :in:      where to move current position
                 Value > 0 stands for moving down the menu.
                 Value < 0 stands for moving up the menu.

    Return Values: 

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMenu::MovePosition(TInt aDelta)
    {
    if(iItemCount > 0 && aDelta != 0)
        {
        // Valid screen size
        TInt screenSize = iScreenSize + 1;
        TInt realPosition = iFirst + iPosOnScreen;

        // Number of all pages and items in the last page
        TInt rest = iItemCount % screenSize;
        TInt pages = (iItemCount / screenSize) + ((rest == 0) ? (0) : (1));

        // Current page
        TInt currPage = realPosition / screenSize;
        
        // New page
        TInt newRealPosition = realPosition + aDelta;
        while(newRealPosition < 0)
            newRealPosition += iItemCount;
        newRealPosition %= iItemCount;
        TInt newPage = newRealPosition / screenSize;
        
        // Change position
        iFirst = newPage * screenSize;
        iLast = iFirst + screenSize - 1;
        if(iLast >= iItemCount)
            iLast = iItemCount - 1;
        iPrevPosOnScreen = iPosOnScreen;
        iPosOnScreen = newRealPosition % screenSize;
        if(newPage == pages - 1 && iPosOnScreen >= rest)
            iPosOnScreen = rest - 1;
            
        // Prevent refreshing
        iPreventClearScreen = (currPage == newPage);
        }
    else
        iPreventClearScreen = ETrue;    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: SelectL

    Description: Process keypresses in menu. Either updates position in
    menu or returns new menu.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    switch ( (TInt) aSelection )
    {

    // Exit
    case EKeyEscape:
        aContinue = EFalse;
        return this;

    // SelectL item
    case EKeyEnter:
    case EKeyRightArrow:
        return iItems[iPosOnScreen];

    // Going back
    case EKeyLeftArrow:
        return iParent;

    // Go down
    case EKeyDownArrow:
        MovePosition(1);
        break;
        
    // Go Up
    case EKeyUpArrow:
        MovePosition(-1);
        break;

    // Additional keys
    case EKeyHome:
    case '3':
        MovePosition(-iFirst - iPosOnScreen);
        break;

    case EKeyEnd:
    case '9':
        MovePosition(iItemCount - 1 - iFirst - iPosOnScreen);
        break;

    case EKeyPageUp:
    case '1':
        MovePosition((iFirst + iPosOnScreen - iScreenSize - 1 >= 0) ? (-iScreenSize - 1) : (-iFirst - iPosOnScreen));
        break;

    case EKeyPageDown:
    case '7':
        MovePosition((iFirst + iPosOnScreen + iScreenSize + 1 < iItemCount) ? (iScreenSize + 1) : (iItemCount - 1 - iFirst - iPosOnScreen));
        break;
    default:  // Bypass the keypress
        break;
    }

    // Continue normally and keep in the same menu
    aContinue = ETrue;
    return this;

    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    TInt count = iItems.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( aArray.Append( iItems[i]->Name() ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }

    return KErrNone;

    }
    
/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: SetParent

    Description: Sets menu parent menu.

    Parameters: CMenu* aMenu              :in:      Menu parent

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMenu::SetParent ( CMenu* aMenu )
    {

    iParent = aMenu;

    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: Print

    Description: Prints one line text and changes to next line. If line is
    too long, overhead is not printed..

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CMenu::Print( const TDesC& aPrint )
    {
    
    iConsole->Write ( aPrint.Left( iSize.iWidth - KMenuOverhead ) );
    iConsole->Write(_L("\n"));

    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: PrintMulti

    Description: Prints text. If line is too long, it will be continued to
    following lines.

    Parameters: const TDesC& aPrint       :in:      Text to print

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMenu::PrintMulti( const TDesC& aPrint )
    {

    // Get current line
    const TInt KMenuOverHead = 2;
    TInt y = iConsole->WhereY();

    const TInt KLineLen =iSize.iWidth -4;
    TBuf<KMaxLineLength+1> oneLine;

    // Loop through the line
    for (TInt i = 0; i < aPrint.Length(); i++)
        {
        oneLine.Append( aPrint[i] );

        // Print one line
        if (oneLine.Length() == KLineLen )
            {
            oneLine.Append (_L("\n"));
            iConsole->Printf(oneLine);
            oneLine=_L("");
            y++;
            }
    
        // Prevent display scrolling
        if ( y == iScreenSize + KMenuOverHead )
            {
            oneLine=_L("");
            break;
            }
        }

    // Print last part if required
    if ( oneLine.Length() != 0 )
        {
        oneLine.Append (_L("\n"));
        iConsole->Printf(oneLine);
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: AddItem

    Description: Add new item to menu

    Parameters: CMenu* aMenu              :in:      Menu to be added

    Return Values: None

    Errors/Exceptions: Leaves if can't add item to menu

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMenu::AddItemL( CMenu* aMenu )
    {

    User::LeaveIfError ( iItems.Append (aMenu) );
    
    }


/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: Name

    Description: Returns menu name

    Parameters: None

    Return Values: const TDesC&                     Menu name

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
const TDesC& CMenu::Name( ) const
    {

    return iName;

    }

/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: MapKeyCode

    Description: Maps numeric keycodes to proper TKeyCode values

    Parameters: TKeyCode 

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/

void CMenu::MapKeyCode(TKeyCode &aSelection)
    {
    
    TInt asciiCode = (TInt) aSelection;
    
    // Handling numeric keys 2,4,6 and 8
    
    if(asciiCode == KMyKeyUpAsciiCode)
        {
        aSelection = EKeyUpArrow;
        }
        
    if(asciiCode == KMyKeyLeftAsciiCode)
        {
        aSelection = EKeyLeftArrow;
        }

    if(asciiCode == KMyKeyRightAsciiCode)
        {
        aSelection = EKeyEnter;
        }

    if(asciiCode == KMyKeyDownAsciiCode)
        {
        aSelection = EKeyDownArrow;
        }
    }
    
/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: Recalculate

    Description: Recalculates visible menu elements

    Parameters: TInt aItemCount    :in:      Menu items count

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CMenu::Recalculate( TInt aItemCount )
	{    
    TInt oldItemCount = iItemCount;
     
    iItemCount = aItemCount;
 
    // If first time in menu, update start and end positions
    if ( ( iFirst == 0 ) && ( iLast == 0 ) || (iItemCount == 0)) //if there are no items, update start and end positions too
    	{
    	if ( iItemCount - 1 > iScreenSize  )
    		{
    		iLast = iScreenSize;
    		}
    	else
    		{
    		iLast = iItemCount - 1;
    		}
    	iPosOnScreen = 0;
    	}
    // New menu entry added, update last visible element if needed
    else if ( oldItemCount < iItemCount )
    	{
    	if ( iFirst + iScreenSize < iItemCount )
    		{
    		iLast = iFirst + iScreenSize;
    		}
    	else
    		{
    		iLast = iItemCount - 1;
    		}
    	}
    // Menu entry removed, update last visible element if needed
    else if ( oldItemCount > iItemCount )
    	{
    	if ( iFirst + iScreenSize < iItemCount )
    		{
    		iLast = iFirst + iScreenSize;
    		}
    	else if ( iFirst > iItemCount - 1 )
    		{
    		iFirst = iItemCount - 1 - iScreenSize;
    		if ( iFirst < 0 )
    			{
    			iFirst = 0;
    			}
    		iLast = iItemCount - 1;
    		iPosOnScreen = iFirst - iLast;
    		}
    	else
    		{
    		iLast = iItemCount - 1;
    		}
    	
    	if ( iFirst + iPosOnScreen > iLast )
    		{
    		iPosOnScreen = iLast - iFirst;
    		}
    	}
    
    if (iFirst < 0)
        {
        iFirst = 0;
        }
    if (iLast > iItemCount)
        {
        iLast = iItemCount - 1;
        }
    if ( iFirst + iScreenSize < iItemCount )
        {
        iLast = iFirst + iScreenSize;
        }
	}

/*
-------------------------------------------------------------------------------

    Class: CFileNameQueryView

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CFileNameQueryView* CFileNameQueryView::NewL( CConsoleMain* aConsole,
                                              CMenu* aParent,
                                              const TDesC& aName )
    {

    CFileNameQueryView* self = new ( ELeave ) CFileNameQueryView();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CFileNameQueryView

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
void CFileNameQueryView::ConstructL( CConsoleMain* aConsole, 
                                     CMenu* aParent, 
                                     const TDesC& aName 
                                    )
    {

    CMenu::ConstructL( aConsole, aParent, aName);

    }

/*
-------------------------------------------------------------------------------

    Class: CFileNameQueryView

    Method: ~CFileNameQueryView

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CFileNameQueryView::~CFileNameQueryView()
    {

    iTestModuleNames.ResetAndDestroy();
    iTestModuleNames.Close();
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CFileNameQueryView

    Method: ItemText

    Description: Returns menu item text.

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFileNameQueryView::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    iTestModuleNames.ResetAndDestroy();    
    
    // Add loaded module names to menu
    TInt ret = iMain->UIStore().ListAllModules( iTestModuleNames );

    if( ret != KErrNone )
        {                 
        return ret;
        }
        
    TInt count = iTestModuleNames.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( aArray.Append( *iTestModuleNames[i] ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }

    return KErrNone; 

    }

/*
-------------------------------------------------------------------------------

    Class: CFileNameQueryView

    Method: SelectL

    Description: Process keypresses in menu. Just return the parent menu

    Parameters: TKeyCode aSelection       :in:      Not used
                TBool& aContinue          :out:     Has user pressed "Quit"?

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CFileNameQueryView::SelectL( TKeyCode aSelection, 
                                    TBool& aContinue )
    {  
    
    MapKeyCode(aSelection);

    if (aSelection == EKeyEnter || aSelection == EKeyRightArrow )
        {
        if( iPosOnScreen < iTestModuleNames.Count() )
            {
            TInt ret = 
                iMain->UIStore().AddTestModule( *iTestModuleNames[iPosOnScreen] );
            if( ret != KErrNone )
                {
                TErrorNotification error;
                error.iModule.Copy( _L("ConsoleUI") );
                error.iText.Copy( _L("Failed to load test module ") );
                error.iText.Append( *iTestModuleNames[iPosOnScreen] );
                error.iText.Append( _L(" error "));
                error.iText.AppendNum( ret );
                iMain->Error( error );
                }
        return iParent;
            }
        else
            {
            return this;
            }
        }  

    return CMenu::SelectL( aSelection, aContinue );

    }


/*
-------------------------------------------------------------------------------

    Class: CMainMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                const TDesC& aHeader	  :in:		Text containing STIF version information

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CMainMenu* CMainMenu::NewL( CConsoleMain* aConsole,
                            CMenu* aParent,
                            const TDesC& aName,
                            const TDesC& aHeader)
    {

    CMainMenu* self = new ( ELeave ) CMainMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName, aHeader );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    CMainMenu

    Method: ~CMainMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMainMenu::~CMainMenu()
    {
    delete iOutputView;
    iOutputView = NULL;
    delete iOperationMenu;
    iOperationMenu = NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CMainMenu

    Method: ItemText

    Description: Returns menu item text. Add "Exit" to normal SelectLion

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CMainMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    TInt ret = CMenu::ItemTexts( aArray );
    if ( ret != KErrNone )
    	{
    	return ret;
    	}
    // Add Exit to last one in menu
    if( aArray.Append( KExitTxt ) != KErrNone )
        {
        return KErrNoMemory;
        }

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CMainMenu

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                const TDesC& aHeader	  :in:		Text containing STIF version information

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMainMenu::ConstructL( CConsoleMain* aConsole, 
                            CMenu* aParent,
                            const TDesC& aName,
                            const TDesC& aHeader
                          )
    {
    CMenu::ConstructL( aConsole, aParent, aName, aHeader);
    iOutputView = NULL;
    iOperationMenu = NULL;
    }

/*
-------------------------------------------------------------------------------

    Class: CMainMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle SelectLion in this
    method, other keys are handles in CMenu class.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CMainMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    // If SelectLion == "exit" and right or enter
    if (aSelection == EKeyEnter || aSelection == EKeyRightArrow )
        {
        if ( iPosOnScreen == iItemCount-1 )
            {
            // Exit
            aContinue = EFalse;
            return this;
            }
        }

    // Normal menu handling
    CMenu* tmp = CMenu::SelectL(aSelection, aContinue);
    return tmp;

    }

/*
-------------------------------------------------------------------------------

    Class: CMainMenu

    Method: CreateOutputViewL

    Description: Creates test case output view.

    Parameters: CMenu* aParent: parent menu for case operation menu

    Return Values: CMenu*        New menu

    Errors/Exceptions: Leaves is CCaseExecutionView menu leaves during creation

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CMainMenu::CreateOutputViewL(CMenu* aParent)
    {
    // Get test cases that have been run
    RRefArray<CStartedTestCase> startedTestCases;
    CleanupClosePushL(startedTestCases);
    CMenu* res = NULL;

    startedTestCases.Reset();
    if(iMain->UIStore().StartedTestCases(startedTestCases, CUIStoreIf::EStatusAll) == KErrNone)
        {                 
        TInt count = startedTestCases.Count();
        if(count > 0)
            {
            // Create menu with execution info
            const CStartedTestCase* caseContainer = &startedTestCases[count - 1];
            delete iOutputView;
            iOutputView = NULL;
            delete iOperationMenu;
            iOperationMenu = NULL;

            iOperationMenu = CCaseOperationMenu::NewL(iMain,
                                                      aParent, //back to parent menu
                                                      caseContainer->TestInfo().TestCaseTitle(),
                                                      caseContainer
                                                     );

            iOutputView = CCaseExecutionView::NewL(iMain,
                                                   iOperationMenu, //back to Case Operation Menu
                                                   caseContainer->TestInfo().TestCaseTitle(),
                                                   caseContainer
                                                  );
            res = iOutputView;
            }
        }
        
    CleanupStack::PopAndDestroy(&startedTestCases);
    return res;
    }

/*
-------------------------------------------------------------------------------

    Class: CModuleListView

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CModuleListView* CModuleListView::NewL( CConsoleMain* aConsole,
                                        CMenu* aParent,
                                        const TDesC& aName )
    {    

    CModuleListView* self = new ( ELeave ) CModuleListView();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;
    
    }
/*
-------------------------------------------------------------------------------

    Class: CModuleListView

    Method: ~CModuleListView

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CModuleListView::~CModuleListView()
    {
    delete iModuleAdd;
    }

/*
-------------------------------------------------------------------------------

    Class: CModuleListView

    Method: ConstructL

    Description: Second level constructor. 

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: None

    Errors/Exceptions: Leaves if parent class ConstructL leaves
                       Leaves if module can't be appended to list

    Status: Draft

-------------------------------------------------------------------------------
*/
void CModuleListView::ConstructL( CConsoleMain* aConsole,
                                  CMenu* aParent,
                                  const TDesC& aName
                                 )
    {

    CMenu::ConstructL( aConsole, aParent, aName);
   
    // Construct module menu
    iModuleAdd  = CFileNameQueryView::NewL ( iMain, this, KSelectMenuTxt );    

    }

/*
-------------------------------------------------------------------------------

    Class: CModuleListView

    Method: ItemText

    Description: Returns menu item text.

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CModuleListView::ItemTexts( RRefArray<TDesC>& aArray )
    {
    // Add also "Add module" text
    if( aArray.Append( KAddMenuTxt ) != KErrNone )
        {
        return KErrNoMemory;
        }
    if( aArray.Append( KLoadMenuTxt ) != KErrNone )
        {
        return KErrNoMemory;
        } 
        
    return KErrNone; 

    }

/*
-------------------------------------------------------------------------------
    Class: CModuleListView

    Method: PrintMenuL

    Description: Prints the menu

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CModuleListView::PrintMenuL( TUpdateType aType )
    {
    
    // Get module names
    RRefArray<TDesC> modules;
    TInt ret = iMain->UIStore().Modules( modules );

    CMenu::PrintMenuL( aType );
    
    // Add loaded module names to menu
    if( ret != KErrNone )
        {
        modules.Reset();
        modules.Close();
        return;
        }
        
    TBuf<KMaxLineLength> line;
    TInt count = modules.Count();
    for( TInt i=0; i<count; i++ )
        {
        line = _L("    ");
        LimitedAppend ( line, modules[i] );

        // Print the line
        Print(line);
        }
    modules.Reset();
    modules.Close();
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CModuleListView

    Method: SelectL

    Description: Process keypresses in menu. Handle right arrow and enter
                 here, others in parent menu.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CModuleListView::SelectL( TKeyCode aSelection, TBool& aContinue )
    {
    
    MapKeyCode(aSelection);

    if( ( aSelection == EKeyRightArrow ||
            aSelection == EKeyEnter ) )
        {
        if( iPosOnScreen == EAddMenuTxtItem )
            {
            return iModuleAdd;
            }
        else if( iPosOnScreen == ELoadMenuTxtItem )
            {
            iMain->UIStore().LoadAllModules();
            return this;
            }
        else 
            {
            return this;
            }
        }

    return CMenu::SelectL( aSelection, aContinue );

    }

/*
-------------------------------------------------------------------------------

    Class: CCaseStartMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                TBool aShowOutput         :in:      Show output after test case is run

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CCaseStartMenu* CCaseStartMenu::NewL( CConsoleMain* aConsole,
                                      CMenu* aParent,
                                      const TDesC& aName,
                                      TBool aShowOutput)
    {

    CCaseStartMenu* self = new ( ELeave ) CCaseStartMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName, aShowOutput );
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CCaseStartMenu

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
void CCaseStartMenu::ConstructL( CConsoleMain* aConsole,
                                 CMenu* aParent,
                                 const TDesC& aName,
                                 TBool aShowOutput
                               )
    {

    iShowOutput = aShowOutput;
    CMenu::ConstructL( aConsole, aParent, aName);

    }

/*
-------------------------------------------------------------------------------

    CCaseStartMenu

    Method: ~CCaseStartMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CCaseStartMenu::~CCaseStartMenu()
    {
    iTestCases.Close();

    iMapFilteredToAll.Reset();
    iMapFilteredToAll.Close();
    }
    
/*
-------------------------------------------------------------------------------

    Class: CCaseStartMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CCaseStartMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {

    iTestCases.Reset();
    TInt ret = iMain->UIStore().TestCases( iTestCases );
    if( ret != KErrNone )
        {
        return ret;
        }
    
    const RPointerArray<TDesC> &filters = iMain->GetFilterArray();
    const TInt filterindex = iMain->GetFilterIndex(); 
    iMapFilteredToAll.Reset();
    
    TInt count = iTestCases.Count();
    for( TInt i = 0; i < count; i++ )
        {
        //If user has chosen filter, let's apply it
        if(filterindex > 0) //skip the first filter which always is "No filter"
            {
            //Check whether test title is matching the applied filter
            iTitleLowerCase.Copy(iTestCases[i].TestCaseTitle());
            iTitleLowerCase.LowerCase();
            iFilterLowerCase.Copy(*filters[filterindex]);
            iFilterLowerCase.LowerCase();

            if(iTitleLowerCase.Match(iFilterLowerCase) != KErrNotFound)
                {
                ret = aArray.Append(iTestCases[i].TestCaseTitle());
                if(ret != KErrNone)
                    {
                    return ret;
                    }
                ret = iMapFilteredToAll.Append(i);
                if(ret != KErrNone)
                    {
                    return ret;
                    }                
                }
            }
        else
            {
            ret = aArray.Append( iTestCases[i].TestCaseTitle() );
            if(ret != KErrNone)
                {
                return ret;
                }
            ret = iMapFilteredToAll.Append(i);
            if(ret != KErrNone)
                {
                return ret;
                }                
            }
        }
        
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CCaseStartMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle enter and right arrow here,
    others in CMenu::SelectL.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CCaseStartMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    switch ( aSelection )
        {
        case EKeyEnter:
        case EKeyRightArrow:
            {
            // Start the case
            if ( (iFirst + iPosOnScreen) < iMapFilteredToAll.Count() ) //changed from iTestCases to iMapFilteredToAll, because first table keeps info about all test cases, and second one about filtered cases
                {
                TInt index;
                User::LeaveIfError( 
                    iMain->UIStore().StartTestCase( 
                        iTestCases[iMapFilteredToAll[iFirst + iPosOnScreen]], index ) );
                // Create output screen (menu) if needed
                if(iShowOutput)
                    {
                    CMenu* m = iMain->GetMainMenu()->CreateOutputViewL(iParent);
                    if(m)
                        return m;
                    }
                return iParent;
                }
            else
                {
                return this;
                }
            }

        default:
            return CMenu::SelectL( aSelection, aContinue);
        }

    }

/*
-------------------------------------------------------------------------------

    CMultipleBaseMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CMultipleBaseMenu* CMultipleBaseMenu::NewL( CConsoleMain* aConsole,
                                            CMenu* aParent,
                                            const TDesC& aName )
    {

    CMultipleBaseMenu* self = new ( ELeave ) CMultipleBaseMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    CMultipleBaseMenu

    Method: CMultipleBaseMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMultipleBaseMenu::CMultipleBaseMenu(): 
    iSetFinished( ETrue ),
    iRunType( ERunSequential ),
    iMenuTotCount( 0 ),
    iMenuCount( 0 )
    {
    
    iMenuCount = EChangeAll + 1;
    iMenuTotCount = iMenuCount;
    
    }   
    
/*
-------------------------------------------------------------------------------

    CMultipleBaseMenu

    Method: ~CMultipleBaseMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMultipleBaseMenu::~CMultipleBaseMenu()
    {
    
    iSetState.Close();
    
    }   

/*
-------------------------------------------------------------------------------

    Class: CMultipleBaseMenu

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
void CMultipleBaseMenu::ConstructL( CConsoleMain* aConsole,
                               CMenu* aParent,
                               const TDesC& aName 
                             )
    {

    CCaseStartMenu::ConstructL( aConsole, aParent, aName );

    }


/*
-------------------------------------------------------------------------------

    Class: CMultipleBaseMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CMultipleBaseMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {

    if ( iSetFinished )
        {
        if( aArray.Append( KChangeAll ) != KErrNone )
            {
            return KErrNoMemory;
            }
        
        if( iUpdateType ==  EMenuPrint )
            {   
            TInt ret = UpdateTestCases();
            if( ret != KErrNone )
                {                 
                return ret;
                }
            }
        TInt count = iTestCases.Count();
        for( TInt j=0; j<count; j++ )
            {
            if( aArray.Append( iTestCases[j].TestCaseTitle() ) 
                != KErrNone )
                {
                return KErrNoMemory;
                }
            if( aArray.Count() > iSetState.Count())
                {
                // Appends iSetState only if aArray's size is bigger @js
                if( iSetState.Append(EFalse) != KErrNone )
                    {
                    return KErrNoMemory;
                    }
                }
            }
        }
    else
        {
        if( aArray.Append( KCancelSet ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }
    
    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------
    Class: CMultipleBaseMenu

    Method: AppendBefore

    Description: Append text before line.

    Parameters: TInt aLine: in: line number 
                TDes& aLine: in: line text

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CMultipleBaseMenu::AppendBefore( TInt aLineNum, TDes& aLine )
    {
    
    CMenu::AppendBefore( aLineNum, aLine );
    if( ( aLine.MaxLength() - aLine.Length() ) < 2 )
        {
        return;
        }        
    
    if( aLineNum >= iMenuTotCount )
        {
        if ( iSetState[ aLineNum - iMenuTotCount ] )
            {
            aLine.Append(_L("S "));
            }
        else
            {
            aLine.Append(_L("  "));
            }
        }
   
    }

/*
-------------------------------------------------------------------------------

    Class: CMultipleBaseMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle enter and right arrow here,
    others in CMenu::SelectL.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CMultipleBaseMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    if ( iSetFinished )
        {

        if ( ( aSelection == EKeyEnter  ) || ( aSelection == EKeyRightArrow ) )
            {
            // CMultipleBaseMenu relative position 
            TInt pos = iFirst + iPosOnScreen - iMenuTotCount + iMenuCount;
            if( pos  == EChangeAll )
                {
                TInt count = iTestCases.Count();    

                // Change all                                       
                for (TInt i = 0; i < count; i++)
                    {
                    iSetState[i] = !iSetState[i];
                    }

                return this;
                }
            else
                {
                // Normal item, change state...
                iSetState[ iFirst + iPosOnScreen - iMenuTotCount ] = 
                    !iSetState[ iFirst + iPosOnScreen - iMenuTotCount ];
                return this;
                }            
            
            }

		return CMenu::SelectL( aSelection, aContinue );
        }
    else
        {

        switch ( aSelection )
            {
            case EKeyEnter:
            case EKeyRightArrow:
                {
                if ( iOngoingCaseContainer != NULL )
                    {
                    iOngoingCaseContainer->UIEngineContainer().CancelTest();
                    }
                iSetFinished = ETrue;
                iFirst = 0;
                iLast = 0;
                return this;
                }
            default:
                return CMenu::SelectL( aSelection, aContinue );
            }



        }

    }

/*
-------------------------------------------------------------------------------

    Class: CMultipleBaseMenu

    Method: CheckIfAnyCaseIsSelected

    Description: Checks if any of testcases was selected in multiple selection menu

    Parameters: TInt aCount number of testcases in multiple selection menu

    Return Values: ETrue if any testcase was selected, EFalse if none of testcases was selected

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
TBool CMultipleBaseMenu::CheckIfAnyCaseIsSelected(TInt aCount)
{
	TInt currentPos = 0;
	for(; currentPos<aCount; currentPos++)	
	{
		//if any of testcases was selected then return true
		if (iSetState[currentPos])
			return ETrue;
	}
	//if none of testcases was selected then return false
	return EFalse;
}

/*
-------------------------------------------------------------------------------

    Class: CMultipleBaseMenu

    Method: SignalL

    Description: Handle test case starts

    Parameters: CStartedTestCase* aContainer: in: CStartedTestCase container

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CMultipleBaseMenu::SignalL( CStartedTestCase* /*aContainer*/ )
    {

    TInt count = iTestCases.Count();
    
    TBool started = EFalse;
    TInt index;
    TInt err = KErrNone;

    // Counter to prevent infinite loop when cases can't be started at all
    // when running cases in repeat mode.
    TInt casesChecked = 0;

    // If cases are not aborted or not in last case and at least one test
    // case was selected, start a new one
    if ( !iSetFinished && iCurrentRunPos != count && CheckIfAnyCaseIsSelected(count) )
        {
        // Search next case from set and execute it.
        while( iCurrentRunPos < count )
            {
            if ( iSetState[iCurrentRunPos] )
                {       
                err = iMain->UIStore().StartTestCase( 
                    iTestCases[ iCurrentRunPos ], 
                    index );

                iCurrentRunPos++;
                casesChecked++;

                // Go to start of list, if running cases in repeat mode. 
                // In other modes, just get out from the main loop
                if( ( iRunType == ERunRepeatSequential ) &&
                    ( iCurrentRunPos >= count ) )
                    {
                    // Restart from first one on next time.
                    iCurrentRunPos = 0;
                    }

                // Handle out of memory error separately, because there is
                // always new test case result display constructed when
                // starting case => Starting more new cases uses more memory
                // and it will not work, so stop starting new cases.
                if( err == KErrNoMemory )
                    {
                    iSetFinished = ETrue;
                    break;
                    }

                // Get pointer to ongoing case in case that user wants to
                // cancel it.
                iOngoingCaseContainer = 
                    &iMain->UIStore().StartedTestCaseL( index );

                // Store pointer to this object to allow 
                // SignalL call when test case is completed
                iOngoingCaseContainer->iBackPtr = (TAny*)this;

                if( err != KErrNone )
                    {
                    TFullTestResult result;  
                    result.iTestResult.iResult = err;
                    result.iTestResult.iResultDes.Copy( 
                                            _L("Failed to start testcase") );
                    // Prevent infinite loop in case, where
                    // ERunRepeatSequential and ALL cases fail to start.
                    if ( iRunType == ERunRepeatSequential && 
                                            casesChecked == count )
                        {
                        // In this state, started = EFalse so final check
                        // stops running
                        break;
                        }
                    }
                else if( ( iRunType == ERunSequential ) ||
                         ( iRunType == ERunRepeatSequential ) )
                    {
                    started = ETrue;
                    break;
                    }
                }
            else 
                {
                iCurrentRunPos++;
                casesChecked++;

                // Go to start of list, if running cases in repeat mode. 
                // In other modes, just get out from the main loop
                if( ( iRunType == ERunRepeatSequential ) &&
                    ( iCurrentRunPos >= count ) )
                    {
                    // Restart from first one
                    iCurrentRunPos = 0;
                    }
                }
            }
        }

    if ( err == KErrNoMemory )
    	{
    	iMain->ExitWithNoMemoryErrorMessage();
    	}
    
    // If nothing was selected, then mark set as finished. 
    // Or running cases at same time, so all have been started.
    if ( started == EFalse )
        {
        iFirst = 0;
        iLast = 0;
        iSetFinished = ETrue;
        iCurrentRunPos = 0;
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CMultipleBaseMenu

    Method: UpdateTestCases

    Description: 

    Parameters: 

    Return Values: 

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CMultipleBaseMenu::UpdateTestCases()
    {

    // Update test cases
    iTestCases.Reset();
    iSetState.Reset();
    return iMain->UIStore().TestCases( iTestCases );
            
    }

/*
-------------------------------------------------------------------------------

    CMultipleCaseMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CMultipleCaseMenu* CMultipleCaseMenu::NewL( CConsoleMain* aConsole,
                                            CMenu* aParent,
                                            const TDesC& aName )
    {

    CMultipleCaseMenu* self = new ( ELeave ) CMultipleCaseMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    CMultipleCaseMenu

    Method: CMultipleCaseMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMultipleCaseMenu::CMultipleCaseMenu()    
    {
    
    iMenuItemsCount = ERepeatRunSelection + 1;
    iMenuItemsCount++; //Select all between already selected
    iMenuTotCount += iMenuItemsCount;
    
    }   

/*
-------------------------------------------------------------------------------

    CMultipleCaseMenu

    Method: ~CMultipleCaseMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMultipleCaseMenu::~CMultipleCaseMenu()
    {
    }   

/*
-------------------------------------------------------------------------------

    Class: CMultipleCaseMenu

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
void CMultipleCaseMenu::ConstructL( CConsoleMain* aConsole,
                               CMenu* aParent,
                               const TDesC& aName 
                             )
    {

    CCaseStartMenu::ConstructL( aConsole, aParent, aName );

    }

/*
-------------------------------------------------------------------------------

    Class: CMultipleCaseMenu

    Method: StartRunningL

    Description: Start running multiple cases.

    Parameters: TRunType aRunType: in: Running type
                const TDesC& aModule: in: module name 
                const TDesC& aTestCaseFile: in: test case file name 

    Return Values: None

    Errors/Exceptions: Leaves on error

    Status: Draft

-------------------------------------------------------------------------------
*/
void CMultipleCaseMenu::StartRunningL( TRunType aRunType, 
                                       const TDesC& aModule, 
                                       const TDesC& aTestCaseFile )
    {
    
    // Update test cases
    iTestCases.Reset();
    iSetState.Reset();
    User::LeaveIfError( 
        iMain->UIStore().TestCases( iTestCases, aModule, aTestCaseFile ) );
    
    // Select all for running
    TInt count = iTestCases.Count();
    for( TInt j=0; j<count; j++ )
        {
        User::LeaveIfError( iSetState.Append(ETrue) );
        }
        
    // Start running
    iCurrentRunPos = 0;
    iSetFinished = EFalse;
    iRunType = aRunType;
    SignalL ( NULL );

    }

/*
-------------------------------------------------------------------------------

    Class: CMultipleCaseMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CMultipleCaseMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {

    if ( iSetFinished )
        {
        if( aArray.Append( KRunSequentially ) != KErrNone )
            {
            return KErrNoMemory;
            }
        if( aArray.Append( KRunParallel ) != KErrNone )
            {
            return KErrNoMemory;
            }
        if( aArray.Append( KRepeatRunSequentially ) != KErrNone )
            {
            return KErrNoMemory;
            }
        if(aArray.Append(KSelectFromTo) != KErrNone)
            {
            return KErrNoMemory;
            }
        }

    TInt ret = CMultipleBaseMenu::ItemTexts(aArray);
    if(ret != KErrNone)
        {
        return ret;
        }
    
    return KErrNone;    
    }

/*
-------------------------------------------------------------------------------

    Class: CMultipleCaseMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle enter and right arrow here,
    others in CMenu::SelectL.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CMultipleCaseMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    if ( iSetFinished )
        {
        switch ( aSelection )
            {
            case EKeyEnter:
            case EKeyRightArrow:
                {

                if ( iFirst + iPosOnScreen == ERunSelection )
                    {
                    // Start the first one
                    iCurrentRunPos = 0;
                    iSetFinished = EFalse;
                    iRunType = ERunSequential;
                    SignalL ( NULL );

                    return iParent;
                    }
                else if ( iFirst + iPosOnScreen == ERunSelectionParaller )
                    {
                    // Start the first one
                    iCurrentRunPos = 0;
                    iSetFinished = EFalse;
                    iRunType = ERunParallel;
                    SignalL ( NULL );

                    return iParent;
                    }
                else if ( iFirst + iPosOnScreen == ERepeatRunSelection )
                    {
                    // Start the first one
                    iCurrentRunPos = 0;
                    iSetFinished = EFalse;
                    iRunType = ERunRepeatSequential;
                    SignalL ( NULL );

                    return iParent;
                    }
                else if(iFirst + iPosOnScreen == ESelectFromTo)
                    {
                    TInt count = iTestCases.Count();
                    TInt indexFirst = count;
                    TInt indexLast = count - 1;
                    TInt i;
                    
                    // Find first test case which is set
                    for(i = 0; i < count; i++)
                        {
                        if(iSetState[i])
                            {
                            indexFirst = i;
                            break;
                            }
                        }
                    
                    // Find last test case which is set
                    if(indexFirst < count)
                        {
                        for(i = count - 1; i > indexFirst; i--)
                            {
                            if(iSetState[i])
                                {
                                indexLast = i;
                                break;
                                }
                            }
                        
                        // Set test cases between first and last
                        for(i = indexFirst; i <= indexLast; i++)
                            {
                            iSetState[i] = ETrue;
                            }
                        }

                    return this;
                    }
                }
                break;
            default:
                break;
            }
        }   
        
    return CMultipleBaseMenu::SelectL( aSelection, aContinue );
        
    }
    
/*
-------------------------------------------------------------------------------

    Class: CCaseMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                TCaseState aType          :in:      Case state

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CCaseMenu* CCaseMenu::NewL( CConsoleMain* aConsole, 
                            CMenu* aParent, 
                            const TDesC& aName, 
                            TInt aType )
    {

    CCaseMenu* self = new ( ELeave ) CCaseMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName, aType );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CCaseMenu

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                TCaseState aType          :in:      Case state

    Return Values: None

    Errors/Exceptions: Leaves if parent class ConstructL leaves
                       Leaves if memory allocation fails
                       Leaves if CCaseOperation menu NewL leaves
                       Leaves if UpcateCaseListL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
void CCaseMenu::ConstructL( CConsoleMain* aConsole, 
                            CMenu* aParent, 
                            const TDesC& aName, 
                            TInt aType
                          )
    {

    CMenu::ConstructL( aConsole, aParent, aName);
    iType = aType;

    }



/*
-------------------------------------------------------------------------------

    Class: CCaseMenu

    Method: ~CCaseMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CCaseMenu::~CCaseMenu()
    {

    delete iCaseOperationMenu;
    iCaseOperationMenu = 0;
    
    iTestCases.Close();

    }


/*
-------------------------------------------------------------------------------

    Class: CCaseMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CCaseMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    iTestCases.Reset();
    
    TInt ret = iMain->UIStore().StartedTestCases( iTestCases, iType );
    if ( ret != KErrNone )
    	{
    	return ret;
    	}

    TInt count = iTestCases.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( aArray.Append( iTestCases[i].TestInfo().TestCaseTitle() ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------
    Class: CCaseMenu

    Method: AppendBefore

    Description: Append text before line.

    Parameters: TInt aLine: in: line number
                TDes& aLine: in: line text

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CCaseMenu::AppendBefore(TInt aLineNum, TDes& aLine)
    {
    CMenu::AppendBefore(aLineNum, aLine);

    //START of --Add number to the test case title--
    //First check how much space we need
    TInt additionalSpace = 0;
    TInt currentCount = aLineNum + 1;

    if(currentCount >= 10000) additionalSpace = 10; //f.e. "10157521. "
    else if(currentCount >= 1000) additionalSpace = 6; //f.e. "1157. "
    else if(currentCount >= 100) additionalSpace = 5; //f.e. "101. "
    else if(currentCount >= 10) additionalSpace = 4; //f.e. "15. "
    else additionalSpace = 3; //f.e. "1. "

 //Create descriptor, add content to it, and finally append to line
    TBuf<20> num;

    if(iTestCases[aLineNum].Status() == CUIStoreIf::EStatusRunning )
 	   {
    if( iTestCases[aLineNum].UIEngineContainer().State() == CUIEngineContainer::EPaused)
     	{
     	  //If not enough space, return
     	if((aLine.MaxLength() - aLine.Length()) < additionalSpace + 4)
     	     {
     	     return;
     	     }
     		num.Format(_L("%d.(P) "), currentCount);
     	}
     else
     	{
        //If not enough space, return
     	if((aLine.MaxLength() - aLine.Length()) < additionalSpace)
     		{
     	  	return;
     	     }     		  			
     	num.Format(_L("%d."), currentCount);
     	}
 	   }
    aLine.Append(num);
    }
    //END of --Add number to test case title--
    

/*
-------------------------------------------------------------------------------

    Class: CCaseMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle Enter and Right Arrow
    here, other will be handled in CMenu::SelectL

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CCaseMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    switch (aSelection)
        {
        case EKeyEnter:
        case EKeyRightArrow:
            {
            if ( iFirst + iPosOnScreen < iTestCases.Count() )
                {
                delete iCaseOperationMenu;
                iCaseOperationMenu = 0;
                iCaseOperationMenu = CCaseOperationMenu::NewL( 
                    iMain, 
                    this, 
                    iTestCases[iFirst + iPosOnScreen].TestInfo().TestCaseTitle(), 
                    &iTestCases[iFirst + iPosOnScreen] );   

                return iCaseOperationMenu;
                }
            }
            return this;
        default:
            // Let the parent class handle rest
            return CMenu::SelectL(aSelection, aContinue);
        }
    
    }


/*
-------------------------------------------------------------------------------

    Class: CCaseOperationMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                CStartedTestCase* aCaseContainer:in: Case container

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CCaseOperationMenu* CCaseOperationMenu::NewL( CConsoleMain* aConsole,
                                 CMenu* aParent,
                                 const TDesC& aName,
                                 const CStartedTestCase* aCaseContainer
                               )
    {

    CCaseOperationMenu* self = new ( ELeave ) CCaseOperationMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName, aCaseContainer );
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CCaseOperationMenu

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                CStartedTestCase* aCaseContainer:in: Case container

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CCaseOperationMenu::ConstructL( CConsoleMain* aConsole,
                                     CMenu* aParent,
                                     const TDesC& aName,
                                     const CStartedTestCase* aCaseContainer
                                   )
    {

    CMenu::ConstructL( aConsole, aParent, aName); 
    iCaseContainer = aCaseContainer;
    
    // Indicate that this menu contains prints, update when needed
    iType = CUIStoreIf::EStatusExecuted;
    
    }


/*
-------------------------------------------------------------------------------

    Class: CMenu

    Method: ~CMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CCaseOperationMenu::~CCaseOperationMenu()
    {
    delete iView;
    iView = 0;
    }

/*
-------------------------------------------------------------------------------

    Class: CCaseOperationMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CCaseOperationMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    if( iCaseContainer->Status() & CUIStoreIf::EStatusRunning )
        {
        if( aArray.Append( KOutputViewDes ) != KErrNone )
            {
            return KErrNoMemory; 
            }
                
        // Set resume/pause text
        if( iCaseContainer->UIEngineContainer().State() == 
            CUIEngineContainer::EPaused )
            {
            if( aArray.Append( KResumeCaseDes ) != KErrNone )
                {
                return KErrNoMemory; 
                }
            }
        else
            {
            if( aArray.Append( KPauseCaseDes ) != KErrNone )
                {
                return KErrNoMemory; 
                }
            }
        if( aArray.Append( KAbortCaseDes ) != KErrNone )
            {
            return KErrNoMemory; 
            }
        }
    else
        {
        if(aArray.Append(KRestartCaseDes) != KErrNone)
            {
            return KErrNoMemory; 
            }
        if( aArray.Append( KOutputViewDes ) != KErrNone )
            {
            return KErrNoMemory; 
            }
        }

    return KErrNone;

    }

/*
-------------------------------------------------------------------------------
    Class: CMenu

    Method: PrintMenuL

    Description: Prints the menu and test case specific information after that.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Leaves if FormatL leaves

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CCaseOperationMenu::PrintMenuL( TUpdateType  aType )
    {

    // Print menu
    CMenu::PrintMenuL( aType );
    
    // Print the result description + result code
    // If case is finished, print also result string
    TInt state = iCaseContainer->Status();
    if  ( ! ( state & CUIStoreIf::EStatusRunning ) )
        {        

        TName resultLine;
        
        Print (_L(""));
        resultLine.Append(_L("Result info: ") );

        if( state & CUIStoreIf::EStatusPassed )
            {
            resultLine.Append( _L("Passed" ) );
            } 
        else if( state & CUIStoreIf::EStatusFailed )
            {
            resultLine.Append( _L("Failed" ) );
            } 
        else if( state & CUIStoreIf::EStatusAborted )
            {
            resultLine.Append( _L("Aborted" ) );
            } 
        else if( state & CUIStoreIf::EStatusCrashed )
            {
            resultLine.Append( _L("Crashed" ) );
            }
        PrintMulti( resultLine );
        resultLine.Zero();             

        // Print test results
        if( !( ( state & CUIStoreIf::EStatusAborted ) || 
               ( state & CUIStoreIf::EStatusCrashed ) || 
               ( state & CUIStoreIf::EStatusPassed ) ) )
            {

            resultLine.Format(_L("Result code %d"), 
                iCaseContainer->Result().iTestResult.iResult );
            PrintMulti ( resultLine );            
            }

        if( iCaseContainer->Result().iCaseExecutionResultType == 
            TFullTestResult::ECaseErrorFromModule )
            {
            PrintMulti ( _L("Error from RunTestCase():"));
            }

        // Print result descriptor if that is available
        const TDesC& des = iCaseContainer->Result().iTestResult.iResultDes;
        if ( des.Length() >0 )
            {
            PrintMulti(des);
            }

        // Print abort code if that is available
        if( ( state & CUIStoreIf::EStatusAborted ) ||
            ( state & CUIStoreIf::EStatusCrashed ) )
            {
            // Print abort type, if that is available
            TFullTestResult::TCaseExecutionResult res = 
                iCaseContainer->Result().iCaseExecutionResultType;
            
            if ( res == TFullTestResult::ECaseLeave ||
                 res == TFullTestResult::ECasePanic ||
                 res == TFullTestResult::ECaseException || 
                 res == TFullTestResult::ECaseErrorFromModule ||
                 res == TFullTestResult::ECaseSuicided )
                {
                TInt code = iCaseContainer->Result().iCaseExecutionResultCode;
                resultLine.Format(_L("Code %d (0x%x)"), code, code );
                PrintMulti ( resultLine );
                }
            }

        // Print start and end times
        const TInt KTimeFieldLength = 30;
        TBuf<KTimeFieldLength> time;
        _LIT(KDateString,"%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");

        resultLine = _L("Started:");
        iCaseContainer->Result().iStartTime.FormatL(time,KDateString);
        resultLine.Append(time);
        PrintMulti (resultLine);

        resultLine = _L("Completed:");
        iCaseContainer->Result().iEndTime.FormatL(time,KDateString);
        
        resultLine.Append(time);
        PrintMulti (resultLine);

        }

    }

/*
-------------------------------------------------------------------------------

    Class: CCaseOperationMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle Enter and right arrow
    here, others are handled in parent menu.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CCaseOperationMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {   

    MapKeyCode(aSelection);
    
    if (aSelection == EKeyEnter || aSelection == EKeyRightArrow )
        {        
        // restartOptionModifier is used because when test case is running
        // restart option is not available (and visible) and that's why
        // we need to modify switch by 1.
        // When test case is finished, restart option becomes available
        // and is put at the first place.
        TInt restartOptionModifier = 0;
        if(iCaseContainer->Status() & CUIStoreIf::EStatusRunning)
            restartOptionModifier = 1;
        
        switch (iPosOnScreen + iFirst + restartOptionModifier)
        {
        case ERestartCase:
            {
            TInt index;
            User::LeaveIfError(iMain->UIStore().StartTestCase(iCaseContainer->TestInfo(), index));
            // Create output screen
            CMenu* m = iMain->GetMainMenu()->CreateOutputViewL(iParent);
            return m;
            }
        case EOutputView:
            {
            delete iView;
            iView = 0;
            iView = CCaseExecutionView::NewL( 
                iMain, 
                this, 
                iCaseContainer->TestInfo().TestCaseTitle(), 
                iCaseContainer );

            return iView;
            }

        case EPauseResume:

            if( iCaseContainer->UIEngineContainer().State() == 
                CUIEngineContainer::ERunning )
                {    
                // Pausing a running case
                iCaseContainer->UIEngineContainer().PauseTest();

                }
            else if( iCaseContainer->UIEngineContainer().State() == 
                     CUIEngineContainer::EPaused )
                {
                // Resuming a paused case
                iCaseContainer->UIEngineContainer().ResumeTest();

                }
            // No other possibilities
            return this;

        case EAbortCase:
            iCaseContainer->UIEngineContainer().CancelTest();
            break;

        }
        
        return this;
        }
    else
        {
        return CMenu::SelectL(aSelection, aContinue);
        }
        
    }



/*
-------------------------------------------------------------------------------

    Class: CCaseExecutionView

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                CStartedTestCase* aCase :in:      Case container

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CCaseExecutionView* CCaseExecutionView::NewL( CConsoleMain* aConsole,
                                              CMenu* aParent,
                                              const TDesC& aName,
                                              const CStartedTestCase* aCase )
    {

    CCaseExecutionView* self = new ( ELeave ) CCaseExecutionView();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName, aCase );
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CCaseExecutionView

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                CStartedTestCase* aCase :in:      Case container

    Return Values: None

    Errors/Exceptions: Leaves if parent class ConstructL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
void CCaseExecutionView::ConstructL( CConsoleMain* aConsole,
                                     CMenu* aParent,
                                     const TDesC& aName,
                                     const CStartedTestCase* aCase
                                    )
    {

    CMenu::ConstructL( aConsole, aParent, aName );

    iCase = aCase;
    
    // Indicate that this menu contains prints, update when needed
    iType = CUIStoreIf::EPrintUpdate;

    iLine = 0;

    }

/*
-------------------------------------------------------------------------------

    CCaseExecutionView

    Method: ~CCaseExecutionView

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CCaseExecutionView::~CCaseExecutionView()
    {
    delete iLineTextBuf;

    }

/*
-------------------------------------------------------------------------------
    Class: CMenu

    Method: PrintMenuL

    Description: Prints the menu. Show the test status and the test case
    print statements.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CCaseExecutionView::PrintMenuL( TUpdateType  /*aType*/ )
    {
    
    if( iShowLine )
        {
        // User want to see whole line 
        iConsole->ClearScreen();
        this->PrintMulti( iLineTextBuf->Des() );
        return;
        }

    // Print the status
    TName tmp;
    TInt state = iCase->Status();
    if( state & CUIStoreIf::EStatusRunning )
        {
        if( iCase->UIEngineContainer().State() == 
            CUIEngineContainer::EPaused )
            {
            tmp = _L("Paused");
            }
        else
            {
            tmp = _L("Running");
            }
        }
    else if( state & CUIStoreIf::EStatusPassed )
        {
        tmp = _L("Passed");
        }
    else if( state & CUIStoreIf::EStatusFailed )
        {
        tmp = _L("Failed");
        }
    else if( state & CUIStoreIf::EStatusAborted )
        {
        tmp = _L("Aborted");
        }
    else if( state & CUIStoreIf::EStatusCrashed )
        {
        tmp = _L("Aborted");
        }
    else
        {
        tmp = _L("Unknown");
        }
    
    // Clear screen.
    iConsole->ClearScreen();
    TBuf<KMaxLineLength> line;

    line.Append(_L(" : "));
    LimitedAppend(line, iCase->TestInfo().TestCaseTitle() );
    Print( line );

    const RPointerArray<CTestProgress>& prints = iCase->PrintArray();

    Recalculate( prints.Count() );

    // Print the print statements.
    for ( TInt i = iFirst; i <= iLast; i++ )
        {
        line.Zero();
        if( iLine == i )
            {
            line = _L( "*" ); // For indicating current line
            }
        line.Append( prints[i]->iDescription );
        line.Append(_L(" : "));
        LimitedAppend(line, prints[i]->iText );

        // Print the line
        TSize screenSize;
        screenSize = iConsole->ScreenSize();
        Print ( line.Left(screenSize.iWidth ) );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CCaseExecutionView

    Method: SelectL

    Description: Process keypresses in menu. Either updates position in
    menu or returns new menu.

    Parameters: TKeyCode aSelection: in: Key
                TBool& aContinue: out: Has user pressed "Quit"

    Return Values: CMenu* New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CCaseExecutionView::SelectL( TKeyCode aSelection, TBool& /*aContinue*/ )
    {
    iShowLine = EFalse;
    MapKeyCode(aSelection);

    // Take prints
    const RPointerArray<CTestProgress>& prints = iCase->PrintArray();

    switch( aSelection  )
        {        
        case EKeyEnter:
        case EKeyRightArrow:
            {
            // Check that not out of range(too many EKeyUpArrow or EKeyDownArrow)
            if( iLine >= prints.Count() || iLine < 0)
                {
                // do nothing
                break;
                }
            TInt extra = 3; // ' : '
            TInt length( 0 );
            length = length + prints[iLine]->iDescription.Length();
            length = length + prints[iLine]->iText.Length();
            // Selected print line in seperate(operation will do in
            // CCaseExecutionView::PrintMenuL() method )
            delete iLineTextBuf;
			iLineTextBuf = NULL;
            TRAPD( ret, iLineTextBuf = HBufC::NewL( length + extra ) );
            if( ret != KErrNone )
                {
                User::InfoPrint( _L("Line show failed, maybe out of memory!!!") );
                break;
                }
            TPtr ptr = iLineTextBuf->Des();
            ptr.Copy( prints[iLine]->iDescription );
            ptr.Append( _L( " : " ) );
            ptr.Append( prints[iLine]->iText );
            iShowLine = ETrue;
            break;
            }
        case EKeyLeftArrow:
            {
            return iParent;
            }
		// Go down
		case EKeyDownArrow:
			{
			if ( iFirst + iPosOnScreen == iItemCount - 1 )
				{
				// If end of the list, go to beginning
				iPosOnScreen = 0;
				iFirst = 0;
				iLast = iScreenSize;
				if ( iLast > iItemCount - 1 )
					{
					iLast = iItemCount - 1;
					}
				iLine = 0;
				}
			else 
				{
				if ( iPosOnScreen == iScreenSize )
					{
					iLast++;
					iFirst++;
					}
				else
					{
					// Going down "in-screen", no need to update items
					iPosOnScreen++;
					}
				iLine++;
				}			
			break;
			}
			
		// Go Up
		case EKeyUpArrow:
			{
			if ( iFirst + iPosOnScreen == 0 )
				{
				// If in the beginning of the list

				iLast = iItemCount - 1;
				iFirst = iLast - iScreenSize;
				if ( iFirst < 0 )
					{
					iFirst = 0;            	
					}
				iPosOnScreen = iLast - iFirst;
				iLine = iItemCount - 1;
				}
			else 
				{
				if ( iPosOnScreen == 0 )
					{
					iLast--;
					iFirst--;
					}
				else
					{
					iPosOnScreen--;
					}
				iLine--;
			}
			break;
			}
		// Additional keys
		case EKeyHome:
		case '3':
			iPosOnScreen = 0;
			iLine = 0;
			iFirst = 0;
			iLast = iScreenSize;

			if ( iLast > iItemCount - 1 )
				{
				iLast = iItemCount - 1;
				}
			break;

		case EKeyEnd:
		case '9':
			iLast = iItemCount - 1;
			iFirst = iLast - iScreenSize;
			iLine = iItemCount - 1;
			
			if ( iFirst < 0 )
				{
				iFirst = 0;
				}
			iPosOnScreen = iLast - iFirst;        
			break;

		case EKeyPageUp:
		case '1':
			iFirst = iFirst - iScreenSize;
			iLast = iLast - iScreenSize;

			if ( iFirst < 0 )
				{
				iFirst = 0;
				iLast = iScreenSize;
				if ( iLast > iItemCount - 1 )
					{
					iLast = iItemCount - 1;
					}
				iPosOnScreen = 0;
				}
			
			iLine = iLine - iScreenSize;
			if ( iLine < 0 )
				{
				iLine = 0;
				}
			break;

		case EKeyPageDown:
		case '7':
			iFirst = iFirst + iScreenSize;
			iLast = iLast + iScreenSize;

			// Going too far
			if ( iLast > iItemCount - 1 )
				{
				iLast = iItemCount - 1;
				iFirst = iLast - iScreenSize;
				if ( iFirst < 0 )
					{
					iFirst = 0;
					}
				iPosOnScreen = iLast - iFirst;
				}

			iLine = iLine + iScreenSize;
			if ( iLine > iItemCount - 1 )
				{
				iLine = iItemCount - 1;
				}
			break;
        default:
            {
            break;
            }
        }
    
    return this;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetMenu* CTestSetMenu::NewL( CConsoleMain* aConsole,
                                        CMenu* aParent,
                                        const TDesC& aName )
    {    

    CTestSetMenu* self = new ( ELeave ) CTestSetMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;
    
    }
       
/*
-------------------------------------------------------------------------------

    Class: CTestSetMenu

    Method: CTestSetMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetMenu::CTestSetMenu():
    iTestSetCreated( EFalse )
    {
    
    iTestSetName.Copy( KDefaultSetName );    
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestSetMenu

    Method: ~CTestSetMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestSetMenu::~CTestSetMenu()
    {
    
    delete iSubMenu;
    iSubMenu = 0;
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetMenu

    Method: ConstructL

    Description: Second level constructor. 

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: None

    Errors/Exceptions: Leaves if parent class ConstructL leaves
                       Leaves if module can't be appended to list

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestSetMenu::ConstructL( CConsoleMain* aConsole,
                                  CMenu* aParent,
                                  const TDesC& aName
                                 )
    {

    CMenu::ConstructL( aConsole, aParent, aName);
   
    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetMenu

    Method: SetTestSetName

    Description: Set test set name.

    Parameters: const TFileName& aTestSetName: in: Test set name

    Return Values: None
    
    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestSetMenu::SetTestSetFileName( const TFileName& aTestSetName )
    {
    
    iTestSetName.Copy( aTestSetName );    
    
    }
        
/*
-------------------------------------------------------------------------------

    Class: CTestSetMenu

    Method: ItemText

    Description: Returns menu item text.

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    if( !iTestSetCreated )
        {
        if( aArray.Append( KSetCreate ) != KErrNone )
            {
            return KErrNoMemory;
            }
        if( aArray.Append( KSetLoad ) != KErrNone )
            {
            return KErrNoMemory;
            }        
        }
    else
        {
        if( aArray.Append( KSetShow ) != KErrNone )
            {
            return KErrNoMemory;
            }                
        if( aArray.Append( KSetStartSeq ) != KErrNone )
            {
            return KErrNoMemory;
            }                
        if( aArray.Append( KSetStartPar ) != KErrNone )
            {
            return KErrNoMemory;
            }
        if( aArray.Append( KSetUnload ) != KErrNone )
            {
            return KErrNoMemory;
            }                        
        if( aArray.Append( KSetRemove ) != KErrNone )
            {
            return KErrNoMemory;
            }                
        if( aArray.Append( KSetSave ) != KErrNone )
            {
            return KErrNoMemory;
            }                
        if( aArray.Append( KSetCaseAdd ) != KErrNone )
            {
            return KErrNoMemory;
            }                
        if( aArray.Append( KSetCaseRemove ) != KErrNone )
            {
            return KErrNoMemory;
            }                
        if( aArray.Append( KSetStartSeqNotRun ) != KErrNone )
            {
            return KErrNoMemory;
            }                
        }
        
    return KErrNone; 

    }

/*
-------------------------------------------------------------------------------
    Class: CTestSetMenu

    Method: PrintMenuL

    Description: Prints the menu

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestSetMenu::PrintMenuL( TUpdateType aType )
    {
    
    if( iTestSetCreated )
        {
        RRefArray<CTestSetInfo> setInfos;
        TInt ret = iMain->UIStore().TestSets( setInfos );

        CMenu::PrintMenuL( aType );

        if( (  ret != KErrNone ) ||
            ( setInfos.Count() != 1 ) )
            {
            // Should never ever happen
            User::Panic( KTestSetMenu, KErrGeneral );
            return;
            }
        const CTestSetInfo& set = setInfos[0];
        setInfos.Reset();
        setInfos.Close();
        
        const RRefArray<const CTestInfo> testCases = set.TestCases();
        
        TBuf<KMaxLineLength> line;
        TInt count = testCases.Count();
        for(TInt i = 0; i < count && i + ESetLAST <= iScreenSize; i++)
            {
            line = _L("    ");
            LimitedAppend ( line, testCases[i].TestCaseTitle() );

            // Print the line
            Print(line);
            }
        }
    else
        {
        CMenu::PrintMenuL( aType );
        }    
    
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestSetMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle right arrow and enter
                 here, others in parent menu.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CTestSetMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {
    TBuf<32> currSetName;
    MapKeyCode(aSelection);    
    
    if( ( aSelection != EKeyRightArrow ) &&
        ( aSelection != EKeyEnter ) )
        {
        return CMenu::SelectL( aSelection, aContinue );
        }            
    
    TInt ret = KErrNone;        
    if( !iTestSetCreated )
        {
        switch( iPosOnScreen )
            {
            case ESetCreate:
                ret = iMain->UIStore().CreateTestSet( iTestSetName );
                if( ret != KErrNone )
                    {
                    User::InfoPrint( _L("Test set creation failed") );
                    }
                break;
            case ESetLoad: 
            
            	return iItems[0];
            } 
        if( ret == KErrNone )
            {
            iTestSetCreated = ETrue;
            }      
        }
    else
        {
        CStartedTestSet::TSetType setType = CStartedTestSet::ESetSequential;
        switch( iPosOnScreen )
            {
            case ESetShow:   
            // Add the test sets menu
                delete iSubMenu;
                iSubMenu = 0;
                iSubMenu = CStartedTestSetMenu::NewL( iMain, 
                                        this, 
                                        _L("Started test sets menu"));
                return iSubMenu;
            case ESetUnload:
                ret = iMain->UIStore().UnloadTestSet( iTestSetName );
                if( ret != KErrNone )
                    {
                    User::InfoPrint( _L("Test set unload failed") );
                    }
                iTestSetCreated = EFalse;                  
                break;
            case ESetRemove: 
                ret = iMain->UIStore().RemoveTestSet( iTestSetName );
                if( ret != KErrNone )
                    {
                    User::InfoPrint( _L("Test set remove failed") );
                    }
                iTestSetCreated = EFalse;  
                break;
            case ESetCaseAdd:   
                delete iSubMenu;
                iSubMenu = 0;
                iSubMenu = CTestSetAddCaseMenu::NewL( 
                                iMain, 
                                this, 
                                _L("Add test cases to test set"),
                                iTestSetName );
                                
                return iSubMenu;
            case ESetCaseRemove: 
                delete iSubMenu;
                iSubMenu = 0;
                iSubMenu =  CTestSetRemoveCaseMenu::NewL( 
                                iMain, 
                                this, 
                                _L("Remove test cases from test set"),
                                iTestSetName );
                                
                                
                return iSubMenu;  
            case ESetSave:   
                // during the save operation default set name (ConsoleUI.set) is replaced with
                // new name created base on current date.
                // Set is the reloaded to make sure that 
                // old test set with default name will not affect test execution
                currSetName.Copy( iTestSetName );
                ret = iMain->UIStore().SaveTestSet2( iTestSetName );
                if( ret != KErrNone )
                    {
                    User::InfoPrint( _L("Test set saving failed at saving test set") );
                    } 
                
                break;
  
  			default:          
//            case ESetStartPar:   
//                setType = CStartedTestSet::ESetParallel;
//            case ESetStartSeq: 
            //case ESetStartRep: 
            	if( iPosOnScreen == ESetStartPar )
                  {
	            		setType = CStartedTestSet::ESetParallel;
                  }
              if( iPosOnScreen == ESetStartSeq || iPosOnScreen == ESetStartSeqNotRun)
	                {
	                RRefArray<CTestSetInfo> setInfos;
	                ret = iMain->UIStore().TestSets( setInfos );
	                if( (  ret != KErrNone ) ||
	                    ( setInfos.Count() != 1 ) )
	                    {
	                    // Should never ever happen
	                    User::Panic( KTestSetMenu, KErrGeneral );
	                    }
	                const CTestSetInfo& set = setInfos[0];
	                setInfos.Reset();
	                setInfos.Close();
	                
	                TInt index;
	                ret = KErrGeneral;
	                if(iPosOnScreen == ESetStartSeq)
	                    {
                        ret = iMain->UIStore().StartTestSet(set, index, setType);
                        }
                    else if(iPosOnScreen == ESetStartSeqNotRun)
                        {
	                    ret = iMain->UIStore().StartTestSet(set, index, setType, ETrue);
	                    }
	                if( ret != KErrNone )
	                    {
	                    User::InfoPrint( _L("Test set starting failed") );
	                    }                 
	                }    
                break;
            }
        }
        
    return this;

    }
/**
-------------------------------------------------------------------------------

Class: CTestSetChoiceMenu

Method: NewL

Description: Constructs new menu.

Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
            CMenu* aParent            :in:      Parent menu
            const TDesC& aName        :in:      Menu name

Return Values: CMenu*                           New menu

Errors/Exceptions: Leaves if memory allocation fails
                   Leaves if ConstructL leaves.

Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetChoiceMenu* CTestSetChoiceMenu::NewL( CConsoleMain* aConsole,
                                    CMenu* aParent,
                                    const TDesC& aName )
{    

	CTestSetChoiceMenu* self = new ( ELeave ) CTestSetChoiceMenu();
	CleanupStack::PushL( self );
	self->ConstructL( aConsole, aParent, aName );
	CleanupStack::Pop( self );
	return self;

}
/*
-------------------------------------------------------------------------------

    Class: CTestSetChoiceMenu

    Method: CTestSetChoiceMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetChoiceMenu::CTestSetChoiceMenu()
    
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetChoiceMenu

    Method: ~CTestSetChoiceMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestSetChoiceMenu::~CTestSetChoiceMenu()
    {
    
    

    iFileList.ResetAndDestroy();
    iFileList.Close();

    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetChoiceMenu

    Method: ConstructL

    Description: Second level constructor. 

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: None

    Errors/Exceptions: Leaves if parent class ConstructL leaves
                       Leaves if module can't be appended to list

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestSetChoiceMenu::ConstructL( CConsoleMain* aConsole,
                                  CMenu* aParent,
                                  const TDesC& aName
                                 )
    {

    CMenu::ConstructL( aConsole, aParent, aName);    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetChoiceMenu

    Method: ItemText

    Description: Returns menu item text.

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetChoiceMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {

   TInt ret = iMain->UIStore().GetTestSetsList( aArray );

	if ( ret != KErrNone )
		{
		return ret;
		}

   iFileList.ResetAndDestroy();
   TRAPD( err,
	//Assign aArray to iFileList, it is used in LoadTestSet
   for( TInt i=0 ; i < aArray.Count(); i++)
	  {
	  iFileList.AppendL(aArray[i].AllocL());
	  }
     ); // TRAPD
   return err;       
    }

/*
-------------------------------------------------------------------------------
    Class: CTestSetChoiceMenu

    Method: PrintMenuL

    Description: Prints the menu

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestSetChoiceMenu::PrintMenuL( TUpdateType aType )
    {
    
    iUpdateType = aType;
    
    TBuf<KMaxLineLength> line;
    
    // Clear display
    iConsole->ClearScreen();

    // Print menu title
    Print( Name() );
    
    RRefArray<TDesC> texts;
    if(  iMain->UIStore().GetTestSetsList( texts ) != KErrNone )
        {
        return;
        }
    
    TInt oldItemCount = iItemCount;
     
    iItemCount = texts.Count();

    // If first time in menu, update start and end positions
    if( (iFirst == iLast) ||        // First time here..
        ( iLast >= iItemCount ) ||
        ( oldItemCount != iItemCount ) )      // Menu size changed
        {
        iLast = iItemCount - 1;
        iPosOnScreen = 0;

        // If "overflow", then adjust the end
        if (iLast > iScreenSize )
            {
            iLast = iScreenSize;
            }
        }
    
    // Print items
    for ( TInt i = iFirst; i <= iLast; i++ )
        {
        line.Zero();
        
        // Append text before line
        AppendBefore( i, line );

        // Get the menu line
        LimitedAppend ( line, texts[i] );

        // Print the line
        Print(line);

        }
    texts.Close();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetChoiceMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle right arrow and enter
                 here, others in parent menu.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CTestSetChoiceMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {
    
    //return CMenu::SelectL( aSelection, aContinue );    
    //return iParent;
	    MapKeyCode(aSelection);
	    TInt ret = 0;

        switch ( aSelection )
            {
            case EKeyEnter:
            case EKeyRightArrow:
                {           
                if(iPosOnScreen < iFileList.Count())
                    {
                    ret = iMain->UIStore().LoadTestSet( iFileList.operator [](iPosOnScreen)->Des() );
                    if (ret == KErrNone)
                        {
                        ((CTestSetMenu*)iParent)->SetCreated();
                        ((CTestSetMenu*)iParent)->SetTestSetFileName(iFileList.operator [](iPosOnScreen)->Des());
                        }
                    return iParent;
                    }
                else
                    {
                    return this;
                    }
                }
            default:
                break;
            }
            
    return CMenu::SelectL( aSelection, aContinue );    

    }






/*
-------------------------------------------------------------------------------

    CTestSetAddCaseMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetAddCaseMenu* CTestSetAddCaseMenu::NewL( CConsoleMain* aConsole,
                                            CMenu* aParent,
                                            const TDesC& aName,
                                            TDesC& aTestSetName )
    {

    CTestSetAddCaseMenu* self = 
        new ( ELeave ) CTestSetAddCaseMenu( aTestSetName );
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    CTestSetAddCaseMenu

    Method: CTestSetAddCaseMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetAddCaseMenu::CTestSetAddCaseMenu( TDesC& aTestSetName ):
    iTestSetName( aTestSetName )    
    {
    
    iMenuItemsCount = EAddSelected + 1;
    iMenuTotCount += iMenuItemsCount;
    
    }   

/*
-------------------------------------------------------------------------------

    CTestSetAddCaseMenu

    Method: ~CTestSetAddCaseMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetAddCaseMenu::~CTestSetAddCaseMenu()
    {
    }   

/*
-------------------------------------------------------------------------------

    Class: CTestSetAddCaseMenu

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
void CTestSetAddCaseMenu::ConstructL( CConsoleMain* aConsole,
                               CMenu* aParent,
                               const TDesC& aName )
    {

    CCaseStartMenu::ConstructL( aConsole, aParent, aName );

    }


/*
-------------------------------------------------------------------------------

    Class: CTestSetAddCaseMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetAddCaseMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    if ( iSetFinished )
        {
        if( aArray.Append( KAddSelected ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }
    
    return CMultipleBaseMenu::ItemTexts( aArray );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetAddCaseMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle enter and right arrow here,
    others in CMenu::SelectL.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CTestSetAddCaseMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    if ( iSetFinished )
        {
        switch ( aSelection )
            {
            case EKeyEnter:
            case EKeyRightArrow:
                {

                if ( iFirst + iPosOnScreen == EAddSelected )
                    {
                    TInt count = iTestCases.Count();    
                    for( TInt i=0; i<count; i++ )
                        {
                        if ( iSetState[i] )
                            {
                            TInt ret = 
                                iMain->UIStore().AddToTestSet( iTestSetName,
                                                               iTestCases[i] );
                            if( ret != KErrNone )
                                {
                                User::InfoPrint( _L("Test case adding failed") );
                                }   
                            }
                        } 
                    return iParent;
                    }               
                }
                break;
            default:
                break;
            }
        }   
        
    return CMultipleBaseMenu::SelectL( aSelection, aContinue );
        
    }

/*
-------------------------------------------------------------------------------

    CTestSetAddCaseMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetRemoveCaseMenu* CTestSetRemoveCaseMenu::NewL( CConsoleMain* aConsole,
                                            CMenu* aParent,
                                            const TDesC& aName,
                                            TDesC& aTestSetName )
    {

    CTestSetRemoveCaseMenu* self = 
        new ( ELeave ) CTestSetRemoveCaseMenu( aTestSetName );
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;
    
    }

/*
-------------------------------------------------------------------------------

    CTestSetRemoveCaseMenu

    Method: CTestSetRemoveCaseMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetRemoveCaseMenu::CTestSetRemoveCaseMenu( TDesC& aTestSetName ):
    iTestSetName( aTestSetName )    
    {
    
    iMenuItemsCount = ERemoveSelected + 1;
    iMenuTotCount += iMenuItemsCount;
    
    }   

/*
-------------------------------------------------------------------------------

    CTestSetRemoveCaseMenu

    Method: ~CTestSetRemoveCaseMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetRemoveCaseMenu::~CTestSetRemoveCaseMenu()
    {
    }   

/*
-------------------------------------------------------------------------------

    Class: CTestSetRemoveCaseMenu

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
void CTestSetRemoveCaseMenu::ConstructL( CConsoleMain* aConsole,
                               CMenu* aParent,
                               const TDesC& aName )
    {

    CCaseStartMenu::ConstructL( aConsole, aParent, aName );

    }


/*
-------------------------------------------------------------------------------

    Class: CTestSetRemoveCaseMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetRemoveCaseMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {

    if ( iSetFinished )
        {
        if( aArray.Append( KRemoveSelected ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }
    
    return CMultipleBaseMenu::ItemTexts( aArray );
    
    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetRemoveCaseMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle enter and right arrow here,
    others in CMenu::SelectL.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CTestSetRemoveCaseMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    if ( iSetFinished )
        {
        switch ( aSelection )
            {
            case EKeyEnter:
            case EKeyRightArrow:
                {

                if ( iFirst + iPosOnScreen == ERemoveSelected )
                    {
                    TInt count = iTestCases.Count();    
                    for( TInt i=0; i<count; i++ )
                        {
                        if ( iSetState[i] )
                            {
                            TInt ret = 
                                iMain->UIStore().RemoveFromTestSet( iTestSetName,
                                                                    iTestCases[i] );
                            if( ret != KErrNone )
                                {
                                User::InfoPrint( _L("Test case adding failed") );
                                }   
                            }
                        } 
                    return iParent;
                    }               
                }
                break;
            default:
                break;
            }
        }   
        
    return CMultipleBaseMenu::SelectL( aSelection, aContinue );
        
    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetRemoveCaseMenu

    Method: UpdateTestCases

    Description: 

    Parameters: 

    Return Values: 

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetRemoveCaseMenu::UpdateTestCases()
    {
    
    TRAPD( err,
        const CTestSetInfo& set = iMain->UIStore().TestSetL( iTestSetName );                
        const RRefArray<const CTestInfo>& testCases = set.TestCases();

        // Update test cases
        iTestCases.Reset();
        iSetState.Reset();
        
        TInt count = testCases.Count();
        for( TInt i=0; i<count; i++ )
            {
            err = iTestCases.Append( testCases[i] );
            if( err != KErrNone )
                {
                break;
                }
            }

        ); // TRAPD
                
    return err;
                
    }

/*
-------------------------------------------------------------------------------

    Class: CCaseMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                TCaseState aType          :in:      Case state

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestSetMenu* CStartedTestSetMenu::NewL( CConsoleMain* aConsole, 
                            CMenu* aParent, 
                            const TDesC& aName )
    {

    CStartedTestSetMenu* self = new ( ELeave ) CStartedTestSetMenu();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSetMenu

    Method: CStartedTestSetMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CStartedTestSetMenu::CStartedTestSetMenu()
    {
    iType = CUIStoreIf::ESetAll;
    }

/*
-------------------------------------------------------------------------------

    Class: CStartedTestSetMenu

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                TCaseState aType          :in:      Case state

    Return Values: None

    Errors/Exceptions: Leaves if parent class ConstructL leaves
                       Leaves if memory allocation fails
                       Leaves if CCaseOperation menu NewL leaves
                       Leaves if UpcateCaseListL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
void CStartedTestSetMenu::ConstructL( CConsoleMain* aConsole, 
                            CMenu* aParent, 
                            const TDesC& aName )
    {

    CMenu::ConstructL( aConsole, aParent, aName);

    }



/*
-------------------------------------------------------------------------------

    Class: CStartedTestSetMenu

    Method: ~CStartedTestSetMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CStartedTestSetMenu::~CStartedTestSetMenu()
    {

    delete iCaseMenu;
    iCaseMenu = 0;
    
    iTestSets.Reset();
    iTestSets.Close();

    }


/*
-------------------------------------------------------------------------------

    Class: CStartedTestSetMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CStartedTestSetMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    iTestSets.Reset();
    
    TInt ret = iMain->UIStore().StartedTestSets( iTestSets, iType );
    if ( ret != KErrNone )
    	{
    	return ret;
    	}

    TInt count = iTestSets.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( aArray.Append( iTestSets[i].TestSetInfo().Name() ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }

    return KErrNone;
    }

/*
-------------------------------------------------------------------------------
    Class: CStartedTestSetMenu

    Method: AppendBefore

    Description: Append text before line.

    Parameters: TInt aLine: in: line number 
                TDes& aLine: in: line text

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CStartedTestSetMenu::AppendBefore( TInt aLineNum, TDes& aLine )
    {
    
    CMenu::AppendBefore( aLineNum, aLine );
    
    if( ( aLine.MaxLength() - aLine.Length() ) < 2 )
        {
        return;
        }        
    if( iTestSets[aLineNum].Status() & CUIStoreIf::ESetRunning )
        {
        aLine.Append( _L("O ") );
        }
    else if( iTestSets[aLineNum].Status() & CUIStoreIf::ESetPassed )
        {
        aLine.Append( _L("P ") );
        }    
    else if( iTestSets[aLineNum].Status() & CUIStoreIf::ESetFailed )
        {
        aLine.Append( _L("F ") );
        }    
            
    }
    
/*
-------------------------------------------------------------------------------

    Class: CStartedTestSetMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle Enter and Right Arrow
    here, other will be handled in CMenu::SelectL

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CStartedTestSetMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode(aSelection);
    
    switch (aSelection)
        {
        case EKeyEnter:
        case EKeyRightArrow:
            {
            if ( iFirst + iPosOnScreen < iTestSets.Count() )
                {
                delete iCaseMenu;
                iCaseMenu = 0;
                iCaseMenu = CTestSetCaseMenu::NewL( 
                    iMain, 
                    this, 
                    iTestSets[iFirst + iPosOnScreen].TestSetInfo().Name(), 
                    iTestSets[iFirst + iPosOnScreen] ); 

                return iCaseMenu;
                }
            }
            return this;
        default:
            break;
        }   
        
    // Let the parent class handle rest
    return CMenu::SelectL(aSelection, aContinue);
    
    }


/*
-------------------------------------------------------------------------------

    Class: CTestSetCaseMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                TCaseState aType          :in:      Case state

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetCaseMenu* CTestSetCaseMenu::NewL( CConsoleMain* aConsole, 
                                          CMenu* aParent, 
                                          const TDesC& aName,
                                          CStartedTestSet& aTestSet  )
    {

    CTestSetCaseMenu* self = new ( ELeave ) CTestSetCaseMenu( aTestSet );
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetCaseMenu

    Method: CTestSetCaseMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CTestSetCaseMenu::CTestSetCaseMenu( CStartedTestSet& aTestSet ):
    iTestSet( aTestSet )
    {
    }
    
/*
-------------------------------------------------------------------------------

    Class: CTestSetCaseMenu

    Method: ConstructL

    Description: Second level constructor.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name
                TCaseState aType          :in:      Case state

    Return Values: None

    Errors/Exceptions: Leaves if parent class ConstructL leaves
                       Leaves if memory allocation fails
                       Leaves if CCaseOperation menu NewL leaves
                       Leaves if UpcateCaseListL leaves

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestSetCaseMenu::ConstructL( CConsoleMain* aConsole, 
                            CMenu* aParent, 
                            const TDesC& aName )
    {

    CCaseMenu::ConstructL( aConsole, aParent, aName, 
                           CUIStoreIf::EStatusAll );

    }



/*
-------------------------------------------------------------------------------

    Class: CTestSetCaseMenu

    Method: ~CTestSetCaseMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestSetCaseMenu::~CTestSetCaseMenu()
    {
    }


/*
-------------------------------------------------------------------------------

    Class: CTestSetCaseMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CTestSetCaseMenu::ItemTexts( RRefArray<TDesC>& aArray )
    {
    
    if ( iTestSet.Status() & CUIStoreIf::ESetRunning )
        {
        if( aArray.Append( KCancelTestSet ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }
    
    const RRefArray<CStartedTestCase>& testCases = iTestSet.TestCases();
        
    iTestCases.Reset();
    
    TInt count = testCases.Count();
    for( TInt i=0; i<count; i++ )
        {
        if( iTestCases.Append( testCases[i] ) != KErrNone )
            {
            return KErrNoMemory;
            }  
        if( aArray.Append( testCases[i].TestInfo().TestCaseTitle() ) != KErrNone )
            {
            return KErrNoMemory;
            }   
        }

    return KErrNone;
    
    }

/*
-------------------------------------------------------------------------------
    Class: CTestSetCaseMenu

    Method: AppendBefore

    Description: Append text before line.

    Parameters: TInt aLine: in: line number 
                TDes& aLine: in: line text

    Return Values: None

    Errors/Exceptions: None

    Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestSetCaseMenu::AppendBefore( TInt aLineNum, TDes& aLine )
    {
    
    CMenu::AppendBefore( aLineNum, aLine );
   
    TInt testCaseNum = aLineNum; 
    if( iTestSet.Status() & CUIStoreIf::ESetRunning ) 
        {
        if( aLineNum == 0 )
            {
            // "Cancel test set" row
            return;
            }
        testCaseNum--;
        }
        
    if( ( aLine.MaxLength() - aLine.Length() ) < 2 )
        {
        return;
        }        
    
   if( iTestCases[testCaseNum].Status() & CUIStoreIf::EStatusRunning )
        {
        aLine.Append( _L("O ") );
        }
    else if( iTestCases[testCaseNum].Status() & CUIStoreIf::EStatusPassed )
        {
        aLine.Append( _L("P ") );
        }
    else if( iTestCases[testCaseNum].Status() & CUIStoreIf::EStatusFailed )
        {
        aLine.Append( _L("F ") );
        }
    else if( iTestCases[testCaseNum].Status() & CUIStoreIf::EStatusAborted )
        {
        aLine.Append( _L("A ") );
        }
    else if( iTestCases[testCaseNum].Status() & CUIStoreIf::EStatusCrashed )
        {
        aLine.Append( _L("C ") );
        }

    }

/*
-------------------------------------------------------------------------------

    Class: CTestSetCaseMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle enter and right arrow here,
    others in CMenu::SelectL.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CTestSetCaseMenu::SelectL( TKeyCode aSelection, TBool& aContinue )
    {
    
    MapKeyCode(aSelection);

    if( ( aSelection == EKeyEnter ) || ( aSelection == EKeyRightArrow ) )
        {
        TInt pos = iFirst + iPosOnScreen; 
        if( iTestSet.Status() & CUIStoreIf::ESetRunning )
            {
            pos--;
            }               
        if( pos == -1 )
            {
            User::LeaveIfError( 
                iMain->UIStore().AbortTestSet( iTestSet ) );
            return iParent;             
            }
        else if( pos < iTestCases.Count() )
            {
            delete iCaseOperationMenu;
            iCaseOperationMenu = 0;
            iCaseOperationMenu = CCaseOperationMenu::NewL( 
                iMain, 
                this, 
                iTestCases[pos].TestInfo().TestCaseTitle(), 
                &iTestCases[pos] ); 

            return iCaseOperationMenu;
            }
        }

        // Let the parent class handle rest
    return CMenu::SelectL(aSelection, aContinue);
    }
        
/*
-------------------------------------------------------------------------------

    Class: CFilterMenu

    Method: NewL

    Description: Constructs new menu.

    Parameters: CConsoleMain* aConsole    :in:      Pointer to main console
                CMenu* aParent            :in:      Parent menu
                const TDesC& aName        :in:      Menu name

    Return Values: CMenu*                           New menu

    Errors/Exceptions: Leaves if memory allocation fails
                       Leaves if ConstructL leaves.

    Status: Draft

-------------------------------------------------------------------------------
*/
CFilterMenu* CFilterMenu::NewL(CConsoleMain* aConsole,
                               CMenu* aParent,
                               const TDesC& aName)
    {
    CFilterMenu* self = new (ELeave) CFilterMenu();
    CleanupStack::PushL(self);
    self->ConstructL(aConsole, aParent, aName);
    CleanupStack::Pop(self);
    return self;
    }

/*
-------------------------------------------------------------------------------

    Class: CFilterMenu

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
void CFilterMenu::ConstructL(CConsoleMain* aConsole,
                             CMenu* aParent,
                             const TDesC& aName
                            )
    {
    CMenu::ConstructL(aConsole, aParent, aName);
    }

/*
-------------------------------------------------------------------------------

    CFilterMenu

    Method: CFilterMenu

    Description: Constructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CFilterMenu::CFilterMenu()
    {
    }
    
/*
-------------------------------------------------------------------------------

    CFilterMenu

    Method: ~CFilterMenu

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CFilterMenu::~CFilterMenu()
    {
    delete iTestCaseMenu;
    }
    
/*
-------------------------------------------------------------------------------

    Class: CFilterMenu

    Method: ItemText

    Description: Returns menu item text

    Parameters: const TInt                :in:      Menu index

    Return Values: const TDesC&                     Menu line text

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
TInt CFilterMenu::ItemTexts(RRefArray<TDesC>& aArray)
    {
    const RPointerArray<TDesC> &filters = iMain->GetFilterArray();
    TInt ret;
    
    TInt count = filters.Count();
    for(TInt i = 0; i < count; i++)
        {    
        ret = aArray.Append(*filters[i]);
        if(ret != KErrNone)
            {
            return ret;
            }
        }
        
    return KErrNone;
    }

/*
-------------------------------------------------------------------------------

    Class: CFilterMenu

    Method: SelectL

    Description: Process keypresses in menu. Handle enter and right arrow here,
    others in CMenu::SelectL.

    Parameters: TKeyCode aSelection       :in:      Key
                TBool& aContinue          :out:     Has user pressed "Quit"

    Return Values: CMenu*                           New menu

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
CMenu* CFilterMenu::SelectL(TKeyCode aSelection, TBool& aContinue)
    {
    const RPointerArray<TDesC> &filters = iMain->GetFilterArray();
    
    MapKeyCode(aSelection);
    
    switch ( aSelection )
        {
        case EKeyEnter:
        case EKeyRightArrow:
            {
            // Start the case
            if((iFirst + iPosOnScreen) < filters.Count())
                {
                // Show test cases menu
                iMain->SetFilterIndex(iFirst + iPosOnScreen);
                if(iTestCaseMenu)
                    {
                    return iTestCaseMenu;
                    }
                else
                    {
                    RDebug::Print(_L("STIF: CFilterMenu::SelectL: test case menu is not set, however it should be"));
                    return this;
                    }
                }
            else
                {
                return this;
                }
            }

        default:
            return CMenu::SelectL( aSelection, aContinue);
        }
    }
    
/*
-------------------------------------------------------------------------------

    Class: CFilterMenu

    Method: SetTestCaseMenu

    Description: Set pointer to test case menu, so it can be shown when user
                 selects filter.
                 This menu will be deleted along with the filter menu.

    Parameters: CMenu* aTestCaseMenu      :in:      test case menu

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CFilterMenu::SetTestCaseMenu(CMenu* aTestCaseMenu)
    {
    if(iTestCaseMenu)
        {
        RDebug::Print(_L("STIF: CFilterMenu::SetTestCaseMenu: test case menu is already set!"));
        delete iTestCaseMenu;
        iTestCaseMenu = NULL;
        }
    iTestCaseMenu = aTestCaseMenu;
    }

// End of File
