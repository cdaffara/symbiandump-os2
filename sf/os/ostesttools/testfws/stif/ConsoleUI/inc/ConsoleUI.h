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
* Description: This file contains the header file of the ConsoleUI.
*
*/

#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H


//  INCLUDE
#include <e32std.h>
#include <e32base.h>

#include <stifinternal/UIStoreIf.h>
#include <stifinternal/UIStore.h>
#include <stifinternal/UIEngine.h>

#include "CallBack.h"

#include "TestModuleInfo.h"

#include "ConsoleMenus.h"

// CONSTANTS
const TInt KMaxLineLength = 80;            // Longest supported line length
const TInt KScrollPeriod = 300000;		   // Scrolling speed

// MACROS

// DATA TYPES

// Case state
// Menu update type
enum TConsoleUIPanic
    {
    EInvalidCaseCompletion,
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMenu;
class CConsoleMain;
class CConsoleReader;
class CMenuDialog;

// CLASS DECLARATION

// DESCRIPTION
// CModule contains a test module data

class CScrollerTimer
        :public CActive
    {
    public:  // Enumerations
        // None

    private: // Enumerations
        
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CScrollerTimer* NewL( CConsoleMain* aMain );

        /**
        * Destructor of CModule.
        */
        ~CScrollerTimer();

    public: // New functions

		/** 
        * StartL
        */
		void StartL();

		/** 
        * RunL
        */
		void RunL();

		/** 
        * DoCancel
        */
		void DoCancel();

		/** 
        * RunError
        */
		TInt RunError ( TInt aError );

    public: // Functions from base classes

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CScrollerTimer();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aMain );

    public:   //Data
        // None

    protected:  // Data
        // None

    private:    // Data
        CConsoleMain*     iMain;            // Pointer to main console
		RTimer            iTimer;

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

// DESCRIPTION
// Console UI main class
class CConsoleMain
        :public CUIStoreIf
    {
    public: // Enumerations
        // None

    private: // Enumerations
        enum TStartCasesOnStart
            {
            EStartCaseNo = 0,
            EStartSingleCase,
            EStartCasesSeq,
            EStartCasesPar,
            };

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CConsoleMain* NewL();

        /**
        * Start
        */
        TInt StartL();

        /**
        * Destructor of CConsoleMain.
        */
        ~CConsoleMain();


    public: // New functions
    
    
        /**
        * Receives output update notification from CUIStore.
        */
        void Update( CStartedTestCase* aTestCase, 
                     TInt aFlags );
                                                                      
        /**
        * Function is called when test framework prints error.
        */
        void Error( TErrorNotification& aError );
        
        /**
        * Function is called when testframework 
        * wants to print a popup window.
        */
        TInt PopupMsg( const TDesC& aLine1, 
                       const TDesC& aLine2, 
                       TInt aTimeInSecs );
        
        /**
        * Function is called when testframework 
        * wants to print a popup window and 
        * get the key pressed.
        */
        TInt PopupMsg( const TDesC& aLine1, 
                       const TDesC& aLine2, 
                       TInt aTimeInSecs,
                       TKeyCode& aKey,
                       TRequestStatus& aStatus );

        /**
        * Close instance.
        */
        void Close( TInt aHandle );
                
        /**
        * Console UI panic function
        */
        static void Panic( TConsoleUIPanic aPanic );

        /**
        * Return pointer to console
        */
        CConsoleBase* GetConsole();

        /**
        * Called from timer to perform scrolling
        */
        void TimerUpdate();
                
        /**
        * Process keystokes.
        */
        void KeyPressed();

        /**
         * Displays no memory error message and closes ConsoleUI
         */
        void ExitWithNoMemoryErrorMessage();
        
        /**
         * Returns MainMenu of console
         */
        CMainMenu* GetMainMenu();
        
        /**
         * Get filter array for reading purpose
         */
        const RPointerArray<TDesC>& GetFilterArray(void) const;
        
        /**
         * Set info about which index filter is used
         */
        void SetFilterIndex(TInt aFilterIndex);

        /**
         * Get info about which index filter is used
         */
        TInt GetFilterIndex(void);
        
    public: // Functions from base classes

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CConsoleMain();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


        /**
        * Print error from ConsoleUI.
        */  
        void UiError( const TDesC& aInfo );
        
        /**
        * Process command line parameters.
        */
        void ProcessCommandlineL( TFileName& aIniFile );

        /**
        * Parse and search for module info and fill list of modules.
        */
        void ParseTestModulesL(CStifParser* aParser, CTestModuleList* aModuleList, const TDesC& aSectionStart, const TDesC& aSectionEnd);
        
    public:   //Data
        RPointerArray<CMenuDialog> iDialogs;
        
    protected:  // Data
        // None

    private:    // Data
        CConsoleBase*       iConsole;       // Pointer to console

        CMenu*              iMainMenu;      // Root menu
        CMenu*              iCurrentMenu;   // Current menu

        CScrollerTimer*     iScroller;
        
        CConsoleReader*     iReader;        // Console reader                   
    
        // For starting runnning multiple cases upon start
        TStartCasesOnStart  iStartCases;
        HBufC*              iTestModule;
        HBufC*              iTestModuleIni;
        HBufC*              iTestCaseFile;
        TInt                iTestCaseNum;
        HBufC*              iTestSetName;

        // List of found modules (included from module given in parameters)
        CTestModuleList*    iModuleList;
        
        // List of filters (for test cases title)
        RPointerArray<TDesC> iFilters;
        
        // Chosen filter index
        TInt iChosenFilterIndex;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };
    
// DESCRIPTION
// Read keystrokes from console
class CConsoleReader
    :public CActive
    {
    public: // Enumerations
        // None

    private: // Enumerations
        // None

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CConsoleReader* NewL( CConsoleMain* aMain, 
                                     CConsoleBase* aConsole );

        /**
        * Start
        */
        void StartL();

        /**
        * Destructor of CConsoleReader.
        */
        ~CConsoleReader();


    public: // New functions

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();
        /**
        * RunError derived from CActive handles errors from active object
        */        
        virtual TInt RunError(TInt aError);

    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None

    private:

        /** 
        * C++ default constructor.
        */
        CConsoleReader( CConsoleMain* aMain, 
                        CConsoleBase* iConsole );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        CConsoleBase*   iConsole;   // Pointer to console
        CConsoleMain*   iMain;      // Backpointer

    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

#endif // CONSOLEUI_H

// End of File
