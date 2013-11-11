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
* Description: This file contains the header file of the CStifTFwIf.
*
*/

#ifndef STIF_TFW_IF_H
#define STIF_TFW_IF_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifTestModule.h>
#include <StifTestInterface.h>


// CONSTANTS
// None

// MACROS
// Hardcoded deviceid used if DevId function is not overridden
const TUint16 KDefaultDevId = 0x0001;

// DATA TYPES
    
// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CStifTFw;

// CLASS DECLARATION
// Information of test case
class CTestInfo
    :public CBase
    {
    public:
        IMPORT_C static CTestInfo* NewL();
        ~CTestInfo();
        
    public:
        const TDesC& ModuleName () const { return iModuleName; };
        TInt SetModuleName ( const TDesC& aModuleName );
        const TDesC& TestCaseTitle() const { return iTitle; };
        TInt SetTestCaseTitle ( const TDesC& aTitle );
        const TDesC& TestCaseFile() const { return iTestCaseFile; };
        TInt SetTestCaseFile ( const TDesC& aTestCaseFile );
        TInt TestCaseNum() const { return iCaseNumber; };
        void SetTestCaseNumber ( TInt aTestCaseNumber )
                                { iCaseNumber = aTestCaseNumber; };
        TInt Priority() const { return iPriority; };
        void SetPriority ( TInt aPriority )
                                { iPriority = aPriority; };
        TTimeIntervalMicroSeconds Timeout() const { return iTimeout; };
        void SetTimeout ( const TTimeIntervalMicroSeconds& aTimeout )
                                { iTimeout = aTimeout; };
        TInt ExpectedResult() const { return iExpectedResult; };
        void CopyL( const CTestInfo& aTestInfo );
        TBool operator==(const CTestInfo& aTestInfo) const;
                                    
    private:
        CTestInfo();
        void ConstructL();
        CTestInfo( const CTestInfo& /*aTestInfo*/ ){};
        CTestInfo& operator= ( const CTestInfo& );
        
    private:
        TInt iCaseNumber;
        TInt iExpectedResult;
        TTimeIntervalMicroSeconds iTimeout;
        TInt iPriority; 
        TPtrC iModuleName;
        HBufC* iModuleNameBuf;
        TPtrC iTitle;
        HBufC* iTitleBuf;
        TPtrC iTestCaseFile;
        HBufC* iTestCaseFileBuf;
    
    };
    
// CLASS DECLARATION

// CStifTFwIf is the main class of STIF Test Framework Interface
class CStifTFwIf 
    :public CBase 
    {
    public: // Enumerations

    protected: // Enumerations
        
    private: // Enumerations

    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual IMPORT_C ~CStifTFwIf();

    protected: // Functions that can be called from derived class

        /**
        * Used to open TestFramework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt Open( TDesC& aTestFrameworkIni = (TDesC&) KNullDesC() );

        /**
        * Used to close TestFramework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt Close();

        /**
        * Used to set attributes for test framework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt SetAttribute( TAttribute aAttribute,
                                    const TDesC& aValue );

        /**
        * Receive message from ATS.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt AtsReceive( const TDesC& aMessage );

        /**
        * Used to add test module to test framework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt AddTestModule( TDesC& aModuleName, 
                            TDesC& aIniFile = (TDesC&) KNullDesC() );

        /**
        * Used to remove test module to test framework.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt RemoveTestModule( TDesC& aModuleName );

        /**
        * Used to add test case file for test module.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt AddTestCaseFile( TDesC& aModuleName,
                              TDesC& aCaseFile );

        /**
        * Used to remove test case file from test module.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt RemoveTestCaseFile( TDesC& aModuleName,
                                 TDesC& aCaseFile );

        /**
        * Get testcases from test modules. Returns test case information 
        * in aTestinfo.
        * Test module name and test case file can be used to limit the 
        * set of test cases returned.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt GetTestCases( RPointerArray<CTestInfo>& aTestInfo,
                           TDesC& aTestModule = (TDesC&) KNullDesC(),
                           TDesC& aTestCaseFile = (TDesC&) KNullDesC() );

        /**
        * Start selected test case identified with CTestInfo.
        * Return TestId, which is handle used in subsequent 
        * test case execution control.
        *
        * Returns Symbian OS error code.
        */

        IMPORT_C TInt StartTestCase( TInt& aTestId,
                            const CTestInfo& aTestInfo );

        /**
        * Cancel test case execution. TestCompleted() will be called 
        * with aTestExecutionResult set to KErrCancel.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt CancelTestCase( TInt aTestId );

        /**
        * Pause test case execution.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt PauseTestCase( TInt aTestId );

        /**
        * Resume test case execution.
        *
        * Returns Symbian OS error code.
        */
        IMPORT_C TInt ResumeTestCase( TInt aTestId );

        /**
        * C++ default constructor.
        */
        IMPORT_C CStifTFwIf();

    protected: // Functions that the derived class may implement.

        /**
        * Function is called when test execution is finished 
        * with information of test case, result of test case execution 
        * and test case result.
        */
        virtual void TestCompleted( TInt aTestId,
                                    TInt aTestExecutionResult,
                                    TTestResult& aTestResult )=0;

        /**
        * Function is called when test case prints.
        */
        virtual void Print( TInt /*aTestId*/,
                            TTestProgress& /*iProgress*/ ){};

        /**
        * Send message to ATS.
        */                            
        virtual void AtsSend( const TDesC& /*aMessage*/ ){};
     
        /**
        * Get device identifier, should be overridden to make them 
        * unique among slaves. 
        * Returns default value for every slave otherwise.
        */
        virtual TUint16 DevId(){ return KDefaultDevId; };      
        
        /**
        * Reboot indication. 
        * After this function returns, reboot is done
        * by test module or STIF TF.
        */                            
        virtual void GoingToDoReset(){};  
        
    private:

    public: // Data
    
    protected: // Data
        
    private: // Data
        
        CStifTFw*   iStifTFw;

    public: // Friend classes
        
    protected: // Friend classes

    private: // Friend classes
        friend class CStifTFw;
    };

#endif      // STIF_TFW_IF_H 

// End of File
