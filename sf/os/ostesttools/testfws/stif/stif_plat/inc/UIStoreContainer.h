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
* Description: This file contains the header file of the 
* CTestCaseRunnner, CTestCasePrinter and CStartedTestCase
*
*/

#ifndef UISTORECASECONTAINER_H
#define UISTORECASECONTAINER_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <StifTestInterface.h>

#include <stifinternal/RRefArray.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CUIEngineContainer;
class CTestInfo;
class CUIStore;
class CTestSetInfo;

// DESCRIPTION
// CTestProgress defines the progress information from Test Module.
class CTestProgress
    :public CBase 
    {
    public:
        static CTestProgress* NewL( const TTestProgress& aProgress );

        ~CTestProgress();
        
        void ReplaceTextL( const TTestProgress& aProgress );
    
    private:
        void ConstructL( const TTestProgress& aProgress );

    public:
        // "Priority" of the progress information.
        TInt iPosition;
        // Description of the data to be printed.
        TPtrC iDescription;
        // The actual progress information.
        TPtrC iText;
    
    private:
        // Description of the data to be printed.
        HBufC* iDescriptionBuf;
        // The actual progress information.
        HBufC* iTextBuf;

    };

// DESCRIPTION
// CStartedTestCase contains active objects that handles one test case
class CStartedTestCase 
    :public CBase  
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CStartedTestCase* NewL( const CTestInfo& aTestInfo, 
                                       CUIEngineContainer& aContainer );

        /**
        * Destructor of CStartedTestCase.
        */
        virtual ~CStartedTestCase();

    public: // New functions
    
        /**
        * Get Engine container handle.
        */
        IMPORT_C CUIEngineContainer& UIEngineContainer() const;
            
        /**
        * Set case result
        */
        void SetResult( const TFullTestResult& aResult );

        /**
        * Get test case info.
        */    
        inline CTestInfo const& TestInfo() const
            { return *iTestInfo; }; 
               
        /**
        * Return the result
        */
        inline const TFullTestResult& Result() const
            { return iResult; };

        /**
        * Return the print array
        */
        inline RPointerArray<CTestProgress>& PrintArray()
            { return iPrints; };
        inline const RPointerArray<CTestProgress>& PrintArray() const
            { return iPrints; };
        
        /**
        * Get status.
        */
        inline TInt Status() const { return iStatus; }

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:   // Functions from base classes
        
        /** 
        * C++ default constructor.
        */
        CStartedTestCase( CUIEngineContainer& aContainer );
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const CTestInfo& aTestInfo );
        
        /**
        * Private constructor for internal use.
        */
        CStartedTestCase( CTestInfo* aTestInfo, 
                          TFullTestResult aResult, 
                          TInt aStatus  );
        
    public:      //Data
        /**
        * Backpointer that can be utilized by UI, not used from UI Engine.
        */
        TAny*       iBackPtr;
    
    protected:  // Data
        // None

    private:    // Data
        CUIEngineContainer*             iContainer;
        CTestInfo*                      iTestInfo;
        TFullTestResult                 iResult;   // Test case result
        
        RPointerArray<CTestProgress>    iPrints;   // Prints from case

        // Execution status (flags from CUIStoreIf::TUpdateFlags)
        TInt                            iStatus;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        friend class CUIStore;

    };

// DESCRIPTION
// CStartedTestSet contains test set information
class CStartedTestSet 
    :public CBase  
    {
    public: // Enumerations
        enum TSetType
            {
            ESetSequential,
            ESetParallel,
            };

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CStartedTestSet* NewL( CUIStore* aUIStore, 
                                      const CTestSetInfo& aTestSetInfo,
                                      TSetType aType );

        /**
        * Destructor of CStartedTestCase.
        */
        virtual ~CStartedTestSet();

    public: // New functions
    
        /**
        * Get test case info.
        */    
        inline CTestSetInfo const& TestSetInfo() const
            { return *iTestSetInfo; }; 
               
        /**
        * Return the result
        */
        inline const TFullTestResult& Result() const
            { return iResult; };

        /**
        * Get status.
        */
        inline TInt Status() const { return iStatus; }
        
        /**
        * Get test cases runned/running in test set.
        */
        inline const RRefArray<CStartedTestCase>& TestCases() const
            { return iTestCases; };
        
        /**
        * Get type.
        */ 
        inline TSetType Type(){ return iType; };        
        
        /**
        * Start next testcase. Returns KErrNotFound if does not exits.
        */ 
        TInt StartNext();
        
        /**
        * Abort running testcases.
        */ 
        TInt Abort();
        
        /**
        * Check if test case running in this test set.
        */
        TBool IsRunning( const CStartedTestCase* aTestCase ); 
        
        /**
        * Test from this set completed.
        */
        TInt TestCompleted( const CStartedTestCase* aTestCase, 
                            TFullTestResult aResult );
                            
        /**
        * Sets mode in which only not executed tests will be run.
        */
        void SetNotStartedCasesMode(TBool aNotStartedCasesMode);
        
        /**
        * Returns pointer to original test set.
        */
        CTestSetInfo* GetOriginalTestSet();

        /**
        * Sets pointer to original test set to NULL.
        */
        void NullOriginalTestSet();

    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:   // Functions from base classes
        /** 
        * C++ default constructor.
        */
        CStartedTestSet( CUIStore* aUIStore, TSetType aType );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const CTestSetInfo& aTestSetInfo );

    public:      //Data
        /**
        * Backpointer that can be utilized by UI, not used from UI Engine.
        */
        TAny*       iBackPtr;
    
    protected:  // Data
        // None

    private:    // Data
        // Backpointer
        CUIStore*                       iUIStore;
        
        CTestSetInfo*                   iTestSetInfo;
        TFullTestResult                 iResult;   // Test case result
        TBool                           iResultSet;
        
        RRefArray<CStartedTestCase>     iTestCases;
        
        TSetType                        iType;

        // Execution status (flags from CUIStoreIf::TUpdateFlags)
        TInt                            iStatus;
        
        // Next case to start        
        TInt                            iNext;
        // Number of running cases
        TInt                            iRunningCases;
        
        // In this mode only not executed tests will be run
        TBool                           iNotStartedCasesMode;
        
        // Original test set
        CTestSetInfo*                   iOriginalTestSet;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

#endif // UISTORECASECONTAINER_H

// End of File
