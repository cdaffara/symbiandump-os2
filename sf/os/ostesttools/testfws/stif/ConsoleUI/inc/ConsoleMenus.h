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
* Description: This file contains the header file of the CMenu 
* derived classes.
*
*/


#ifndef CONSOLE_MENUS_H
#define CONSOLE_MENUS_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStoreContainer.h>
#include <stifinternal/UIEngineContainer.h>

#include "CallBack.h"

// CONSTANTS
const TInt KMenuOverhead = 4;

// Numeric key ascii code values for ConsoleUI's menu
const TInt KMyKeyDownAsciiCode = 56; // Ascii code for number '8'
const TInt KMyKeyLeftAsciiCode = 52; // Ascii code for number '4'
const TInt KMyKeyRightAsciiCode = 54; // Ascii code for number '6'
const TInt KMyKeyUpAsciiCode = 50;  // Ascii code for number '2'

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMenu;
class CConsoleMain;
class CCaseOperationMenu;
class CCaseExecutionView;

// CLASS DECLARATION

// DESCRIPTION
// Main menu
class CMenu
        :public CBase
    {
    public:  // Enumerations
        enum TUpdateType
            {
            EMenuPrint,
            EMenuRefresh,
            };

        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CMenu* NewL( CConsoleMain* aConsole,
                            CMenu* aParent,
                            const TDesC& aName);

       /**
        * Destructor of CConsoleMain.
        */
        virtual ~CMenu();

    public: // New functions
    
        /** 
        * Return menu name
        */
        virtual const TDesC& Name() const;

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

        /** 
        * Prints the menu
        */
        virtual void PrintMenuL( TUpdateType  aType );

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );

        /** 
        * Set parent menu
        */
        virtual void SetParent ( CMenu* aMenu );

        /** 
        * Print line
        */
        virtual void Print( const TDesC& aPrint);

        /** 
        * Multi line print
        */
        virtual void PrintMulti( const TDesC& aPrint );

        /** 
        * Add item to menu
        */
        virtual void AddItemL ( CMenu* aItem );
        
        /** 
        * Signals test completion
        */
        virtual void SignalL ( CStartedTestCase* /*aContainer*/ ){};
		
        /** 
        * Update display from timer
        */
		virtual void TimerUpdate();
		
		/**
		* Append text before original text.
		*/
		virtual void AppendBefore(  TInt aLineNum, TDes& aLine );
		
		/**
		* Get menu type.
		*/
		inline TInt Type(){ return iType; };
		
		/**
		* Map KeyCode 
		*/
		void MapKeyCode(TKeyCode &aSelection);

		/**
		 * Recalculates visible menu elements.
		 */
		void Recalculate( TInt aItemCount );
		
    public: // Functions from base classes
        // None

    protected:  // New functions
        // None

    private:   // New functions
        /**
        * Changes internal variables to move cursor in the menu.
        */
        void MovePosition(TInt aDelta);

    protected:  // Functions from base classes
        // None    

        /** 
        * C++ default constructor.
        */
        CMenu();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole,
                         CMenu* aParent,
                         const TDesC& aName,
                         const TDesC& aHeader = KNullDesC);
    
    private:   // Functions from base classes
        // None

    public:   // Data
        // None

    protected:  // Data    
    
        CConsoleBase*           iConsole;      // Pointer to console
        CConsoleMain*           iMain;         // Pointer to main console
        CMenu*                  iParent;       // Pointer to parent menu
        TName                   iName;         // Menu name
    	TName					iHeader;	   // Used to display STIF version information

        RPointerArray<CMenu>    iItems;        // Menu items

        TInt                    iPosOnScreen;  // Position on display
        TInt                    iPrevPosOnScreen; // Previous position on display
        TInt                    iMenuItemsListStartIndex;    // Y-index of the first line containing menu item
        TInt                    iMenuItemsListEndIndex;    // Y-index of the last line containing menu item
        TInt                    iFirst;        // First displayed item
        TInt                    iLast;         // Last displayed item
        TBool                   iPreventClearScreen;  // In case of moving cursor "in screen" cleaning screen is not needed
        TInt                    iItemCount;    // Last Item
        TSize                   iSize;         // Display size
        TInt                    iScreenSize;   // "Work area" size
		TInt                    iDirection;    // Scrolling direction
		TInt                    iStart;        // Scrolling position
		TInt                    iPrevPos;      // Previous position in scrolling

        // Menu type (which updates must cause console update) 
        // Flags from CUIStoreIf::TUpdateFlags 
        TInt                    iType;
        
        // Update type set from PrintMenuL
		TUpdateType             iUpdateType;
		
    private:    // Data
        TName           iScrolledLine; //Original value of line, that was recently scrolled

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };



// CONSTANTS
_LIT( KSelectMenuTxt, "Select module to load, if using ini or test case file use testframework.ini" );

// DESCRIPTION
// Filename query menu
class CFileNameQueryView 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CFileNameQueryView* NewL( CConsoleMain* aConsole, 
                                         CMenu* aParent, 
                                         const TDesC& aName);

        ~CFileNameQueryView();
        
    public: // New functions
       // None

    public: // Functions from base classes
        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );
        
        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );

    protected:  // New functions

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent, 
                         const TDesC& aName );

    protected:  // Functions from base classes
        // None

    private:

    public:   // Data
        // None

    protected:  // Data
        // None

    private:    // Data
        // List of module names
        RPointerArray<TDesC>    iTestModuleNames;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CONSTANTS
_LIT( KAddMenuTxt,  "Add test module" );
_LIT( KLoadMenuTxt,  "Load all test modules" );

// DESCRIPTION
// Module list menu
class CModuleListView 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        enum TModuleListMenu
            {
            EAddMenuTxtItem,
            ELoadMenuTxtItem,
            };
            
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CModuleListView* NewL( CConsoleMain* aConsole,
                                      CMenu* aParent,
                                      const TDesC& aName );

       /**
        * Destructor
        */
        ~CModuleListView();

    public: // New functions
        // None

    public: // Functions from base classes

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );
        
        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, 
                                TBool& aContinue );
 
        /** 
        * Prints the menu
        */
        virtual void PrintMenuL( TUpdateType  aType );
        
    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent,
                         const TDesC& aName );

    private:

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        CMenu*                     iModuleAdd;

    public:     // Friend classes
         // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// Test case starting menu
class CCaseStartMenu 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        /**
        * NewL is first phase of two-phased constructor.
        */
        static CCaseStartMenu* NewL( CConsoleMain* aConsole, 
                                     CMenu* aParent, 
                                     const TDesC& aName,
                                     TBool aShowOutput = EFalse);

        /**
        * Destructor.
        */
		~CCaseStartMenu();

    public: // New functions
       // None

    public: // Functions from base classes

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, 
                                TBool& aContinue );

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

    protected:  // New functions
        // None

    protected:  // Functions from base classes

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole,
                         CMenu* aParent,
                         const TDesC& aName,
                         TBool aShowOutput = EFalse);

    private:
        // None

    public:     // Data
        // None

    protected:  // Data
        // Test case array
        RRefArray<CTestInfo> iTestCases;

    private:    // Data
        // Show output automaticly after test case is started
        TBool iShowOutput;
        
        // Map table. Translates indexes from filtered to all loaded from module  
        RArray<TInt> iMapFilteredToAll;
        
        // Keeps filter name converted to lower case
        TFileName iFilterLowerCase;
        
        // Keeps title of test case converted to lower case
        TFileName iTitleLowerCase;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CONSTANTS
_LIT( KChangeAll, "Change all");
_LIT( KCancelSet, "Cancel set execution" );

// Test set creation menu
class CMultipleBaseMenu 
    :public CCaseStartMenu
  
    {
    public:  // Enumerations
         enum TMenuIndex
		    {  
		    EChangeAll = 0,
		    ECancelSet = 0,
		    };
        enum TRunType
            {
            ERunSequential,
            ERunParallel,
            ERunRepeatSequential,
            };
		    
    protected: // Enumerations

    public:  // Constructors and destructor
        /**
        * NewL is first phase of two-phased constructor.
        */
        static CMultipleBaseMenu* NewL( CConsoleMain* aConsole, 
                                        CMenu* aParent, 
                                        const TDesC& aName);
        
        /**
        * Destructor.
        */
		~CMultipleBaseMenu();

    public: // New functions
       // None

    public: // Functions from base classes

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

		/** 
        * Signals test completion
        */
        virtual void SignalL ( CStartedTestCase* aContainer );
        
        /** 
        * Append text before line print.
        */
        virtual void AppendBefore( TInt aLineNum, TDes& aLine );
        
    protected:  // New functions

        //inline void SetRunType( TRunType aRunType )
        //    { iRunType = aRunType; };
            
        /**
        * Update iTestCases();
        */ 
        virtual TInt UpdateTestCases();
        TBool CheckIfAnyCaseIsSelected(TInt aCount);
                      
    protected:  // Functions from base classes

        /**
        * Constructor.
        */
		CMultipleBaseMenu();
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole,
                         CMenu* aParent,
                         const TDesC& aName );

    private:
       
    public:     // Data

    protected:  // Data
		RArray<TBool>       iSetState;
		TInt                iCurrentRunPos;
		TBool               iSetFinished;
		CStartedTestCase*   iOngoingCaseContainer;
		TRunType            iRunType;
				
		// Number of items in "static" menu (including derived classes)
		TInt                iMenuTotCount;

    private:    // Data
		// Number of items in "static" menu of this class
		// (excluding derived classes)  
		TInt                iMenuCount;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// CONSTANTS
_LIT( KRunSequentially, "Run selection sequentially");
_LIT( KRunParallel, "Run selection parallel");
_LIT( KRepeatRunSequentially, "Repeat run selection sequentially");

_LIT( KSelectFromTo, "Select all between already selected"); 

// Test set creation menu
class CMultipleCaseMenu 
    :public CMultipleBaseMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        enum TMultipleMenuIndex
		    {  
		    ERunSelection = 0,
		    ERunSelectionParaller = 1,
		    ERepeatRunSelection = 2,
		    ESelectFromTo = 3,
		    };
		    
    public:  // Constructors and destructor
        /**
        * NewL is first phase of two-phased constructor.
        */
        static CMultipleCaseMenu* NewL( CConsoleMain* aConsole, 
                                        CMenu* aParent, 
                                        const TDesC& aName);
        
        /**
        * Destructor.
        */
		~CMultipleCaseMenu();

    public: // New functions
       /**
       * Start running.
       */
       virtual void StartRunningL( TRunType aRunType, 
                                   const TDesC& aModule = KNullDesC, 
                                   const TDesC& aTestCaseFile = KNullDesC );

    public: // Functions from base classes

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
    
        /**
        * Constructor.
        */
		CMultipleCaseMenu();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole,
                         CMenu* aParent,
                         const TDesC& aName );

    private:
       
    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data
        // Number of items in "static" menu of this class
		// (excluding derived classes)  
		TInt                iMenuItemsCount;
		
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };


// CONSTANTS
_LIT( KExitTxt,     "Exit" );

// DESCRIPTION
// The main menu class
class CMainMenu 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CMainMenu* NewL( CConsoleMain* aConsole, 
                                CMenu* aParent, 
                                const TDesC& aName,
                                const TDesC& aHeader );

        /**
        * Destructor
        */
        ~CMainMenu();

    public: // New functions
        /**
        * Creates CaseOperationMenu and CaseExecutionView.
        * Used when after test case starting its output view needs to be shown.
        */
        CMenu* CreateOutputViewL(CMenu* aParent);

    public: // Functions from base classes    

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );


    protected:  // New functions
        // None

    protected:  // Functions from base classes

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent,
                         const TDesC& aName,
                         const TDesC& aHeader );

    private:

    public:   //Data

    protected:  // Data

    private:    // Data
        // Menu representing output view, used to show it after test case is started
        CCaseExecutionView* iOutputView;

        // Operation menu used to show output view after test case is started
        CCaseOperationMenu* iOperationMenu;
		
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };


// DESCRIPTION
// The case list (passed/failed/...) menu class
class CCaseMenu 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CCaseMenu* NewL( CConsoleMain* aConsole, 
                                CMenu* aParent, 
                                const TDesC& aName, 
                                TInt aType );

        /**
        * Destructor
        */
        ~CCaseMenu();

    public: // New functions
        // None

    public: // Functions from base classes    

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );
        
        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

        /**
        * Append text before original text.
        */
        virtual void AppendBefore(TInt aLineNum, TDes& aLine);

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent, 
                         const TDesC& aName, 
                         TInt aType );

    private:    // Data
        // None

    public:     // Data
        // None

    protected:  // Data
        CCaseOperationMenu*             iCaseOperationMenu;    // Submenu
        RRefArray<CStartedTestCase>     iTestCases;

    private:    // Data
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CONSTANTS
_LIT( KOutputViewDes,   "View Output");
_LIT( KPauseCaseDes,    "Pause");
_LIT( KResumeCaseDes,   "Resume");
_LIT( KAbortCaseDes,    "Abort case");
_LIT( KRestartCaseDes,  "Restart");

// DESCRIPTION
// Case specific operations, like pause, abort etc.
class CCaseOperationMenu 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        enum TCaseOperationMenu
            {
            ERestartCase,
            EOutputView,
            EPauseResume,
            EAbortCase,   
            };

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CCaseOperationMenu* NewL( CConsoleMain* aConsole, 
                            CMenu* aParent, 
                            const TDesC& aName, 
                            const CStartedTestCase* aCaseContainer );
        
        /**
        * Destructor.
        */
        ~CCaseOperationMenu();
        
    public: // New functions

    public: // Functions from base classes

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );

        /** 
        * Prints the menu
        */
        virtual void PrintMenuL( TUpdateType  aType );

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

    protected:  // New functions
        // None

    protected:  // Functions from base classes

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent,
                         const TDesC& aName,
                         const CStartedTestCase* aCaseContainer );

    private:    // Functions from base classes
        // None

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        // Case container
        const CStartedTestCase* iCaseContainer; 
        // Execution view                       
        CCaseExecutionView*     iView;
        
    public:     // Friend classes
        // None        

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };


class CCaseExecutionView 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CCaseExecutionView* NewL( CConsoleMain* aConsole, 
                                         CMenu* aParent, 
                                         const TDesC& aName, 
                                         const CStartedTestCase* aCase);

         /**
        * Destructor.
        */
        ~CCaseExecutionView();

    public: // New functions
        // None

    public: // Functions from base classes

        /** 
        * Prints the menu
        */
        virtual void PrintMenuL( TUpdateType  aType );

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );
        
        /**
        * Get current testcase info.
        */ 
        inline const CStartedTestCase* TestCase(){ return iCase; };

    protected:  // New functions
        // None

    protected:  // Functions from base classes

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent,
                         const TDesC& aName,
                         const CStartedTestCase* aCase );

    private:   // Functions from base classes
        // None

    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data
        const CStartedTestCase* iCase;

        /**
        * Indicates current selected line.
        */
        TInt                    iLine;
        /**
        * Indicates will line show.
        */
        TBool                   iShowLine;
        /**
        * Line to be show.
        */
        HBufC*                  iLineTextBuf;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CMenu notifier class shows dialogs in UI.
class CMenuNotifier
        :public CBase
    {
    public:  // Enumerations

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CMenuNotifier* NewL( const TDesC& aError, 
                                    CConsoleMain* aMain );

       /**
        * Destructor of CConsoleMain.
        */
        virtual ~CMenuNotifier();

    public: // New functions  
            
    public: // Functions from base classes
        // None

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None    

        /** 
        * C++ default constructor.
        */
        CMenuNotifier( CConsoleMain* aMain );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TDesC& aError );
    
    private:   // Functions from base classes
        // None

    private:   // New functions
        /** 
        * C++ default constructor.
        */
        void Run1();	

    public:   // Data
        // None

    protected:  // Data      

    private:    // Data
        CActiveCallback <CMenuNotifier> iCallBack1;
        CActiveCallback <CMenuNotifier> iCallBack2;
        RTimer iTimer;
        CConsoleBase* iConsole;
        CConsoleMain* iMain;        
        TInt iCompletionCode; 

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CMenu dialog class shows dialogs in UI and waits for keypress.
class CMenuDialog
    :public CBase
    {
    public:  // Enumerations

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CMenuDialog* NewL( CConsoleMain* aMain, 
                                  const TDesC& aMessage, 
                                  const TDesC& aInstruction,
                                  TInt aTimeInSecs );

       /**
        * Destructor.
        */
        virtual ~CMenuDialog();

    public: // New functions  
            
    public: // Functions from base classes
        
        TInt WaitForKeypress( TKeyCode& aKeyCode, 
                              TRequestStatus& aStatus  );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None    

        /** 
        * C++ default constructor.
        */
        CMenuDialog( CConsoleMain* aMain );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TDesC& aMessage, 
                         const TDesC& aInstruction, 
                         TInt aTimeInSecs );
    
    private:   // Functions from base classes
        // None

    private:   // New functions
        /**
        * Handle completions.
        */
        void Run1();	

    public:   // Data
        // None

    protected:  // Data      

    private:    // Data
        // Backpointer
        CConsoleMain* iMain;        
        CConsoleBase* iConsole;

        // Timer support
        CActiveCallback <CMenuDialog> iCallBack1;
        RTimer iTimer;
        
        // Reading
        CActiveCallback <CMenuDialog> iCallBack2;
        
        // For reading keys        
        TRequestStatus* iStatus;
        TKeyCode*     iKeyCode;
    
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CONSTANTS
_LIT( KTestSetMenu,     "CTestSetMenu" );

_LIT( KDefaultSetName,  "ConsoleUI.set" );

_LIT( KSetCreate,       "Create test set" );
_LIT( KSetLoad,         "Load test set" );

_LIT( KSetShow,         "Show started test sets" );
_LIT( KSetUnload,       "Unload test set" );
_LIT( KSetRemove,       "Remove test set" );
_LIT( KSetCaseAdd,      "Add test case to test set" );
_LIT( KSetCaseRemove,   "Remove test case from test set" );
_LIT( KSetSave,         "Save test set" );

_LIT( KSetStartSeq,     "Start sequential test set execution" );
_LIT( KSetStartPar,     "Start parallel test set execution" );
//_LIT( KSetStartRep,     "Start repeating test set execution" );

_LIT( KSetStartSeqNotRun, "Start sequentially not started tests" );

// DESCRIPTION
// Test set menu
class CTestSetMenu 
    :public CMenu
  
    {
    public:  // Enumerations
        
    private: // Enumerations
        
        enum TCreateMenu
            {
            ESetCreate,
            ESetLoad,    
            };
            
        enum TEditMenu
            {
            ESetShow,
 
            ESetStartSeq,
            ESetStartPar,
            //ESetStartRep,
 
            ESetUnload,
            ESetRemove,
            ESetSave,
            ESetCaseAdd,
            ESetCaseRemove,
            
            ESetStartSeqNotRun,
            

            ESetLAST //This entry must remain LAST in this menu
            };
            
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestSetMenu* NewL( CConsoleMain* aConsole,
                                   CMenu* aParent,
                                   const TDesC& aName );

       /**
        * Destructor
        */
        ~CTestSetMenu();

    public: // New functions
        /**
        * Set test set file name.
        */
        void SetTestSetFileName( const TFileName& aTestSetFileName ); 
        
        /**
        * Set test set state created.
        */
        void SetCreated(){ iTestSetCreated = ETrue; };
         
    public: // Functions from base classes

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );
        
        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, 
                                TBool& aContinue );
 
        /** 
        * Prints the menu
        */
        virtual void PrintMenuL( TUpdateType  aType );
        
    protected:  // New functions

    protected:  // Functions from base classes
        /**
        * Constructor
        */
        CTestSetMenu();
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent,
                         const TDesC& aName );

    private:

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        TBool               iTestSetCreated;
        TFileName           iTestSetName;
        
        CMenu*              iSubMenu;
        
    public:     // Friend classes
         // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

    // CONSTANTS
    _LIT( KTestSetChoice,     "CTestSetChoiceMenu" );
    _LIT( KSetSelect,       "Select Test Set" );


    // DESCRIPTION
    // Test set choice menu
    class CTestSetChoiceMenu 
        :public CMenu

        {
        public:  // Enumerations
            
        private: // Enumerations
            
            
                
        public:  // Constructors and destructor

            /**
            * NewL is first phase of two-phased constructor.
            */
            static CTestSetChoiceMenu* NewL( CConsoleMain* aConsole,
                                       CMenu* aParent,
                                       const TDesC& aName );

           /**
            * Destructor
            */
            ~CTestSetChoiceMenu();

        public: // New functions
             
        public: // Functions from base classes

            /** 
            * Return item texts.
            */
           virtual TInt ItemTexts( RRefArray<TDesC>& aArray );
            
            /** 
            * Process keypresses on this menu
            */
            virtual CMenu* SelectL( TKeyCode aSelectLion, 
                                    TBool& aContinue );
     
            /** 
            * Prints the menu
            */
            virtual void PrintMenuL( TUpdateType  aType );
            
        protected:  // New functions

        protected:  // Functions from base classes
            /**
            * Constructor
            */
            CTestSetChoiceMenu();
           
            /**
            * By default Symbian OS constructor is private.
            */
            void ConstructL( CConsoleMain* aConsole, 
                             CMenu* aParent,
                             const TDesC& aName );

        private:

        public:   //Data
            // None

        protected:  // Data
            // None

        private:    // Data
            TFileName           iTestSetName;
            RPointerArray<HBufC> iFileList;
            
        public:     // Friend classes
             // None

        protected:  // Friend classes
            // None

        private:    // Friend classes
            // None

        };
    
// CONSTANTS
_LIT( KAddSelected, "Add selected cases");

// Test set case add menu
class CTestSetAddCaseMenu 
    :public CMultipleBaseMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        enum TTestSetAddCaseMenuIndex
		    {  
		    EAddSelected = 0,
		    };
		    
    public:  // Constructors and destructor
        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestSetAddCaseMenu* NewL( CConsoleMain* aConsole, 
                                         CMenu* aParent, 
                                         const TDesC& aName,
                                         TDesC& aTestSetName );
        
        /**
        * Destructor.
        */
		~CTestSetAddCaseMenu();

    public: // New functions
       // None

    public: // Functions from base classes

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

    protected:  // New functions
        // None

    protected:  // Functions from base classes
    
        /**
        * Constructor.
        */
		CTestSetAddCaseMenu( TDesC& aTestSetName );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole,
                         CMenu* aParent,
                         const TDesC& aName );

    private:
       
    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data
        // Number of items in "static" menu of this class
		// (excluding derived classes)  
		TInt    iMenuItemsCount;
		
        TPtrC   iTestSetName;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CONSTANTS
_LIT( KRemoveSelected, "Remove selected cases");

// Test set case add menu
class CTestSetRemoveCaseMenu 
    :public CMultipleBaseMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        enum TTestSetRemoveCaseMenuIndex
		    {  
		    ERemoveSelected = 0,
		    };
		    
    public:  // Constructors and destructor
        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestSetRemoveCaseMenu* NewL( CConsoleMain* aConsole, 
                                             CMenu* aParent, 
                                             const TDesC& aName,
                                             TDesC& aTestSetName );
        
        /**
        * Destructor.
        */
		~CTestSetRemoveCaseMenu();

    public: // New functions
       // None

    public: // Functions from base classes

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );
        
        /**
        * Update iTestCases;
        */ 
        virtual TInt UpdateTestCases();

    protected:  // New functions
        // None

    protected:  // Functions from base classes
    
        /**
        * Constructor.
        */
		CTestSetRemoveCaseMenu( TDesC& aTestSetName );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole,
                         CMenu* aParent,
                         const TDesC& aName );

    private:
       
    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data
        // Number of items in "static" menu of this class
		// (excluding derived classes)  
		TInt        iMenuItemsCount;
		
        TPtrC       iTestSetName;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };    

// DESCRIPTION
// The case list (passed/failed/...) menu class
class CTestSetCaseMenu  
    :public CCaseMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestSetCaseMenu* NewL( CConsoleMain* aConsole, 
                                       CMenu* aParent, 
                                       const TDesC& aName,
                                       CStartedTestSet& aTestSet );

        /**
        * Destructor
        */
        ~CTestSetCaseMenu();

    public: // New functions
        // None

    public: // Functions from base classes    

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );

        /** 
        * Prints the menu
        */
        void AppendBefore( TInt aLineNum, TDes& aLine );
        
        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );     
        
    protected:  // New functions

    protected:  // Functions from base classes

       

    private:    // Data
    
        CTestSetCaseMenu( CStartedTestSet& aTestSet );
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent, 
                         const TDesC& aName );
    public:     // Data
        // None

    protected:  // Data

    private:    // Data
        CStartedTestSet& iTestSet;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// CONSTANTS
_LIT( KCancelTestSet, "Cancel test set");
        
// DESCRIPTION
// Started test sets (passed/failed/...) menu class
class CStartedTestSetMenu 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CStartedTestSetMenu* NewL( CConsoleMain* aConsole, 
                                          CMenu* aParent, 
                                          const TDesC& aName );

        /**
        * Destructor
        */
        ~CStartedTestSetMenu();

    public: // New functions
        // None

    public: // Functions from base classes    

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL( TKeyCode aSelectLion, TBool& aContinue );
        
        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RRefArray<TDesC>& aArray );


        /** 
        * Prints the menu
        */
        void AppendBefore( TInt aLineNum, TDes& aLine );
        
        
    protected:  // New functions

    protected:  // Functions from base classes

       

    private:    // Data
    
        CStartedTestSetMenu();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CMenu* aParent, 
                         const TDesC& aName );
    public:     // Data
        // None

    protected:  // Data
        // None

    private:    // Data
        CTestSetCaseMenu*           iCaseMenu;    // Submenu
        RRefArray<CStartedTestSet>  iTestSets;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// Menu which shows available filters for test case titles
class CFilterMenu 
    :public CMenu
  
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor
        /**
        * NewL is first phase of two-phased constructor.
        */
        static CFilterMenu* NewL(CConsoleMain* aConsole, 
                                 CMenu* aParent, 
                                 const TDesC& aName);

        /**
        * Destructor.
        */
        ~CFilterMenu();

    public: // New functions
        /**
        * Set test case menu, so it can be shown when user selects filter
        */
        void SetTestCaseMenu(CMenu* aTestCaseMenu);

    public: // Functions from base classes

        /** 
        * Process keypresses on this menu
        */
        virtual CMenu* SelectL(TKeyCode aSelectLion, 
                               TBool& aContinue);

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts(RRefArray<TDesC>& aArray);

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        /** 
        * C++ default constructor.
        */
        CFilterMenu();
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(CConsoleMain* aConsole,
                        CMenu* aParent,
                        const TDesC& aName);

    private:    // Functions
        // None

    public:     // Data
        // None

    protected:  // Data
        // None
        
    private:    // Data
        /**
        * Test case menu. It will be shown when user selects filter.
        */
        CMenu *iTestCaseMenu;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };
   
#endif // CONSOLE_MENUS_H

// End of File
