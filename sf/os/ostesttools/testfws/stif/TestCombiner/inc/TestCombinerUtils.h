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
* TestCombinerUtils.
*
*/

#ifndef TESTCOMBINERUTILS_H
#define TESTCOMBINERUTILS_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <StifTestInterface.h>

#include "TestCombinerEvent.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None
    
// CLASS DECLARATION

// DESCRIPTION
// Testcase starting information
NONSHARABLE_CLASS(CStartInfo)
    :public CBase
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CStartInfo* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CStartInfo();
            
    public: // New functions
        /**
        * Set module name.
        */
        void SetModuleNameL( TDesC& aModule, TInt aExtLength=0 );
            
        /**
        * Set initialization file name.
        */
        void SetIniFileL( TDesC& aIni );
            
        /**
        * Set configuration file name.
        */
        void SetConfigL( TDesC& aConfig );
            
        /**
        * Set test identifier.
        */
        void SetTestIdL( TDesC& aTestId );

        /**
        * Delete module name buffer for creating new module name.
        */
        void DeleteModuleName();
        
        /**
        * Set test case title.
        */
        void SetTitleL(TDesC& aTitle);

        /**
         * Sets test case arguments.
         */
        void SetTestCaseArgumentsL( const TDesC& aTestCaseArguments );

    private:
        /**
        * C++ default constructor.
        */
        CStartInfo();
                    
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
                
    public:
        TPtr    iModule;
        TPtrC   iIniFile;
        TPtrC   iConfig;
        TPtrC   iTestId;
        TInt    iCaseNum;
        TInt    iExpectedResult;
        TFullTestResult::TCaseExecutionResult iCategory;
        TInt    iTimeout;
        TPtrC   iTitle;
		TPtrC   iTestCaseArguments;
        
    private:
        HBufC*  iModuleBuf;
        HBufC*  iIniFileBuf;
        HBufC*  iConfigBuf;
        HBufC*  iTestIdBuf;
        HBufC*  iTitleBuf;
        HBufC*  iTestCaseArgumentsBuf;
    };
    
// CLASS DECLARATION

// DESCRIPTION
// Slave information structure
NONSHARABLE_CLASS(CSlaveInfo)
    :public CBase
    {
    public:
        enum TSlaveState
            {
            ESlaveIdle,
            ESlaveReserveSent,
            ESlaveReserved,
            ESlaveReleaseSent,
            ESlaveReleased,
            };        
            
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CSlaveInfo* NewL( TDesC& aName, TUint32 aMaster );
        
        /**
        * Destructor.
        */
        virtual ~CSlaveInfo();
            
    public:            
        
        /**
        * Get event name requested from slave.
        */    
        TEventTc* GetEvent( TDesC& aEventName );
        
    private:
        /**
        * C++ default constructor.
        */
        CSlaveInfo( TUint32 aMaster );
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TDesC& aName );
        
    public:
        TInt    iState;

        TPtrC   iName;
        TUint32 iMasterId;
        TUint32 iSlaveDevId;
        
        RPointerArray<TEventTc>         iEvents;
        
    private:
        HBufC*  iNameBuf;
        
    };     

// CLASS DECLARATION

// DESCRIPTION
// Predefined value entry
NONSHARABLE_CLASS(CDefinedValue)
    :public CBase
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CDefinedValue* NewL( TDesC& aName, TDesC& aValue );
        
        /**
        * Destructor.
        */
        virtual ~CDefinedValue();

    public: // New functions
        /**
        * Returns define name.
        */
        TDesC& Name();
        
        /**
        * Returns define value.
        */
        TDesC& Value();
        
        /**
        * Returns new define value.
        */
        void SetValueL( TDesC& aValue );

    private:
        /**
        * C++ default constructor.
        */
        CDefinedValue();
        
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TDesC& aName, TDesC& aValue );
        
    private: // Data
        // Define name
        HBufC* iNameBuf;
        TPtrC  iName;

        // Define value
        HBufC* iValueBuf;
        TPtrC  iValue;

    };
    
#endif        // TESTCOMBINERUTILS_H

// End of File
