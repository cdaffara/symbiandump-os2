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
* Description: This file contains the header file of the CUIStore.
*
*/

#ifndef UISTOREIF_H
#define UISTOREIF_H

// INCLUDES
#include <e32std.h>
#include <e32keys.h>

#include <StifTestInterface.h>

#include <stifinternal/RRefArray.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CUIStore;
class CStartedTestCase;
class CTestInfo;

// CONSTANTS

// CLASS DECLARATION

/**
* UI Store class interface. UI must derive this to use UI Store.
*/
class CUIStoreIf 
    :public CBase
    {
    public: // Enumerations
        // Updated flags
        enum TUpdateFlags
            {
            // Test case status updates 
            EStatusRunning  = 0x00000001,
            EStatusExecuted = 0x00000002,
            EStatusPassed   = 0x00000004,
            EStatusFailed   = 0x00000008,
            EStatusAborted  = 0x00000010,
            EStatusCrashed  = 0x00000020,
            EStatusAll      = 0x000000ff,
            // Test set updates
            ESetRunning     = 0x00000100,
            ESetExecuted    = 0x00000200,
            ESetPassed      = 0x00000400,
            ESetFailed      = 0x00000800,
            ESetAll         = 0x0000ff00,
            // Other updates
            EPrintUpdate    = 0x00010000,                              
            };
            
    private: // Enumerations
        // None
        
    public: // Constructors and destructor
    
        /**
        * Destructor.
        */      
        IMPORT_C ~CUIStoreIf();
        
    protected:
        /**
        * Constructor
        */
        IMPORT_C CUIStoreIf();

        /**
        * Symbian OS second phase constructor. This must be called 
        * from derived class ConstructL.
        */      
        IMPORT_C void ConstructL();
        
    public: // New functions
        /**
        * Returns reference to CUIStore object, 
        * which handles test cases and test modules.
        */
        IMPORT_C CUIStore& UIStore();

        /**
        * Receives output update notification from CUIStore.
        * UI implements this to get update notifications. 
        * aFlags are from 
        */
        virtual void Update( CStartedTestCase* /*aTestCase*/, 
                             TInt /* aFlags */ ){};
                                   
                                   
        /**
        * Function is called when test framework prints error.
        */
        virtual void Error( TErrorNotification& /*aError*/ ){};
        
        /**
        * Function is called to print message to UI.
        */
        virtual TInt PopupMsg( const TDesC& /* aLine1 */, 
                               const TDesC& /* aLine2 */, 
                               TInt /* aTimeInSecs */ )
            { return KErrNotSupported; };
            
        /**
        * Function is called to print message to UI
        * and wait key pressing.
        */
        virtual TInt PopupMsg( const TDesC& /* aLine1 */, 
                               const TDesC& /* aLine2 */, 
                               TInt /* aTimeInSecs */,
                               TKeyCode& /* aKey */,
                               TRequestStatus& /* aStatus */ )
            { return KErrNotSupported; };
       
       /**
        * Function is called to close instances.
        */
        virtual void Close( TInt /* aHandle */ ){};

    public: // Functions from base classes

    private: 

    private:
    
    private: // Data
		CUIStore*					    iUIStore;
		
    };
    
// CLASS DECLARATION

// DESCRIPTION
// CTestSetInfo contains test set information
class CTestSetInfo 
    :public CBase  
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CTestSetInfo* NewL( const TDesC& aName );

        /**
        * Destructor of CStartedTestCase.
        */
        virtual ~CTestSetInfo();

    public: // New functions
        
        /**
        * Get test set name.
        */
        inline const TDesC& Name() const 
            { return *iName; };
        
        /**
        * Get test cases.
        */
        inline const RRefArray<const CTestInfo>& TestCases() const
            { return iTestCaseRefs; };
        
        /**
        * Add test case to set.
        */
        TInt AddTestCase( const CTestInfo& aTestInfo );
        
        /**
        * Insert test case to specific location.
        */
        TInt InsertTestCase( const CTestInfo& aTestInfo, TInt aPos );
     
        /**
        * Remove test case from set.
        */
        TInt RemoveTestCase( const CTestInfo& aTestInfo );
        
        /**
        * NewL is first phase of two-phased constructor.
        */
        void CopyL( const CTestSetInfo& aTestSetInfo );
        
        /**
        * Sets the info which test case has been started lately.
        */
        void SetLastStartedCaseIndex(TUint aLastStartedCaseIndex);

        /**
        * Gets the info which test case has been started lately.
        */
        TUint GetLastStartedCaseIndex(void);
        /**
        * rename test set
        */
        void RenameTestSet(const TDesC& aTestSetName);
                
    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

    private:   // Functions from base classes
        /** 
        * C++ default constructor.
        */
        CTestSetInfo();       

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( const TDesC& aName );

    public:     // Data
    
    protected:  // Data
        // None

    private:    // Data
        RRefArray<const CTestInfo>      iTestCaseRefs;
        RPointerArray<CTestInfo>        iTestCases;
        HBufC*                          iName;
        // Which test case was started as a last one (index)
        TUint                           iLastStartedCaseIndex;
        
    public:     // Friend classes
        // None

    protected:  // Friend classes
        // None

    private:    // Friend classes
        // None

    };

#endif // UISTOREIF_H

// End of File
