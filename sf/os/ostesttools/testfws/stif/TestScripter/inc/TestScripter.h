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
* Description: This file contains the header file of the TestScripter 
* module of STIF Test Framework.
*
*/

#ifndef TestScripter_H
#define TestScripter_H

//  INCLUDES
#include <e32std.h>

#include <StifParser.h>
#include <StifLogger.h>
#include <StifTestModule.h>
#include "TestScripterInternal.h"
#include "TestKeywords.h"
#include "StifKernelTestClass.h"
#include "StifTestInterference.h"
#include "STIFMeasurement.h"

// CONSTANTS
// Printing priorities
const TInt KPrintPriLow = 10;
const TInt KPrintPriNorm = 30;
const TInt KPrintPriHigh = 50;
// Log dir and file
_LIT( KTestScripterLogDir, "\\Logs\\TestFramework\\TestScripter\\" );
_LIT( KTestScripterLogFile, "TestScripter.txt" );
_LIT( KTestScripterLogFileWithTitle, "TestScripter_[%S].txt");
// Configuration file tags
_LIT( KTestStartTag, "[Test]" );
_LIT( KTestEndTag, "[Endtest]" );

// Define tags
_LIT( KDefineStartTag, "[Define]" );
_LIT( KDefineEndTag, "[Enddefine]" );

// Loop counter macro
_LIT( KLoopCounter, "LOOP_COUNTER" );

// MACROS
// None

// DATA TYPES
class CTestRunner;
typedef CScriptBase* (*CInterfaceFactory)( CTestModuleIf& aTestModuleIf );

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestRunner;
class CTestContinue;
class TTestModule;
class TTestObjectBase;
class TTestObject;
class TTestInterference;
class CStifTestMeasurement;
class TTestMeasurement;

// CLASS DECLARATION

// DESCRIPTION
// Predefined value entry
class CDefinedValue
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

// CLASS DECLARATION

// DESCRIPTION
// CTestScripter contains the Test Module interface of TestScripter 
// for STIF Test Framework

class CTestScripter 
    :public CTestModuleBase
    {
    public: // Enumerations

    private: // Enumerations

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestScripter* NewL();
          
        /**
        * Destructor.
        */
        ~CTestScripter();

    public: // New functions

        /**
        * Call specified tst class object.
        */
        TInt CallTestClass( const TDesC& aLine );

        /**
        * Get pointer to test class object.
        */
        TInt GetTestScriptObject( const TDesC& aObjectName );

        /**
        * Return runner handle.
        */ 
        CTestRunner& TestRunner(){ return *iTestRunner; };

    public: // Functions from base classes

        /**
        * Test module initialization.
        */ 
        TInt InitL( TFileName& aIniFile, TBool aFirstTime );

       /**
        * GetTestCasesL is used to inquire testcases. 
        */
        TInt GetTestCasesL( const TFileName& aConfigFile, 
                            RPointerArray<TTestCaseInfo>& aTestCases );

        /**
        * RunTestCase is used to run an individual test case specified 
        * by aTestCase. 
        */
        TInt RunTestCaseL( const TInt aCaseNumber, 
                           const TFileName& aConfig, 
                           TTestResult& aResult );
    	
	    /**
	     * Internal fuction to get const value defined in 
	     * [Define]...[Enddefine] section of script file
	     */        
        IMPORT_C TInt GetConstantValue( const TDesC& aName, TDes& aValue );    

        /**
        * Set result description of test case.
        */        
        IMPORT_C void SetResultDescription(const TDesC& aDescr);

        /**
        * Internal fuction to set local value.
        */        
        IMPORT_C TInt SetLocalValue(const TDesC& aName, const TDesC& aValue);

        /**
        * Internal fuction to get local value.
        */        
        IMPORT_C TInt GetLocalValue(const TDesC& aName, TDes& aValue);
        
    protected: // New functions

    protected: // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CTestScripter();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * GetTestCaseL gets the specified test case section 
        * from the onfigfile.
        */
        CStifSectionParser* GetTestCaseL( TInt aCaseNumber, 
                                            const TFileName& aConfig );

        /**
        * RunTestL runs the testcase specified in section.
        */
        void RunTestL();

        /**
        * LoadTestModuleL loads test module.
        */
        TTestModule* LoadTestModuleL( TDesC& aModule );

        /**
        * CreateObjectL create new object.
        */
        void CreateObjectL( TDesC& aModule, TDesC& aObjectId );

        /**
        * CreateKernelObjectL create new kernel object.
        */
        void CreateKernelObjectL( TDesC& aDriver, TDesC& aObjectId );

        /**
        * DeleteObjectL deletes object with aObjectId.
        */
        TInt DeleteObjectL( TDesC& aObjectId );

        /**
        * GetObject returns object corresponding to aObjectId.
        */
        TTestObjectBase* GetObject( const TDesC& aObjectId );

        /**
        * Read initialization from file.
        */
        void ReadInitializationL( const TDesC& aIniFile, 
                                  RPointerArray<CDefinedValue>& aDefines );

        /**
        * Read sub section from test case file.
        */
        CStifSectionParser* GetSubL(const TDesC& aSubName);
        
        /**
        * Updates result of test case.
        */        
        void UpdateTestCaseResult(const TInt aError, const TDesC& aDescr);

    public: // Data
        // Logger 
        CStifLogger*                    iLog;

    protected: // Data

    private: // Data
        // Test case array of running/runned test cases 
        RPointerArray<TTestObjectBase>      iTestObjects;
        // Array of test modules
        RPointerArray<TTestModule>          iTestModules;

        // Test case result of the TestScripter
        TTestResult                         iResult;

        // Sectionparser for the current testcase
        CStifSectionParser*                 iSectionParser;
        
        // Currently used section parser (changed when executing a function (sub))
        CStifSectionParser*                 iCurrentParser;
        
        // It says if we need to call GetLine or GetNextLine for the parser
        TBool                               iCurrentParserReadFirstLine;
        
        // Stack of parsers for called functions (subs)
        RPointerArray<CStifSectionParser>   iParserStack;

        // Pointer to used testrunner
        CTestRunner*                        iTestRunner;

        // Predefined values are listed here
        RPointerArray<CDefinedValue>        iDefinedIni;
        RPointerArray<CDefinedValue>        iDefinedRuntime;
        RPointerArray<CDefinedValue>        iDefinedLocal;

        // Flag for OOM testing. If true test class's build
        // block execution result will ignored.
        TBool                               iOOMIgnoreFailure;

        // Flag which says if heap balance should be checked (affects only 
        // EKA2 environment). May be set in StifSettings section.
        TBool                               iCheckHeapBalance;
        
        // Standard logger 
        CStifLogger*                        iStdLog;

        // Test case Logger 
        CStifLogger*                        iTCLog;

        // Flag which says if test case title should be appended to the
        // log file name. This is read from SettingServer
        TBool                               iAddTestCaseTitleToLogName;

        // The running test case's config file name
        TFileName                           iConfig;

    public:      // Friend classes

    protected:  // Friend classes

    private:     // Friend classes
        friend class CTestRunner;
        friend class CTestContinue;

    };

// CLASS DECLARATION

// DESCRIPTION
// CTestRunner parses the TestScripter configuration file and 
// runs a testcase. CTestRunner is the friend of CTestScripter
// and uses its private member variables directly.

class CTestRunner 
    :public CActive
    {
    public: // Enumerations
         
    private: // Enumerations
        enum TRunnerState{
            ERunnerIdle,
            ERunnerRunning,
            ERunnerCancel,
            ERunnerError,
            ERunnerPaused,
            };

    public:  // Constructors and destructor  
        /**
        * Two-phased constructor.
        */
        static CTestRunner* NewL( CTestScripter* aTestScripter );

        /**
        * Destructor.
        */
        ~CTestRunner();

    public: // New functions
        /**
        * Calls SetActive() from CActive.
        */
        void SetRunnerActive();

        /**
        * Get test case result handle.
        */
        RArray<TInt>& TestCaseResults(){ return iTestCaseResults; }; 


    public: // Functions from base classes     
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions

    protected:  // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CTestRunner( CTestScripter* aTestScripter );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Executes one script line.
        */
        TBool ExecuteLineL( TDesC& aKeyword,
                            CStifItemParser* aItem );

        /**
        * Executes method call to object script line.
        */
        TBool ExecuteCommandL( TDesC& aObject,
                               CStifItemParser* aItem );

        /**
        * Executes event control script line.
        */                       
        TBool ExecuteEventL( TDesC& aKeyword,
                                CStifItemParser* aItem );

        /**
        * Preprocess hardcoded values in test case.
        */
        CStifItemParser* PreprocessLineL( TDesC& line );
        
        /**
        * Checks if aWord is defined value.
        */
        TInt CheckDefined( TPtrC& aWord );

        /**
        * Checks if aWord is a local variable.
        */
        TInt CheckDefinedLocals( TPtrC& aWord );

        /**
        * Handles 'oomignorefailure' keyword parsing.
        */
        void OOMIgnoreFailureL( CStifItemParser* aItem );

        /**
        * Handles 'oomheapfailnext' keyword parsing.
        */
        void OOMHeapFailNextL( CStifItemParser* aItem );

        /**
        * Handles 'oomheapsetfail' keyword parsing.
        */
        void OOMHeapSetFailL( CStifItemParser* aItem );

        /**
        * Initialize all OOM related variables.
        */
        void OOMHeapToNormal();

        /**
        * Handles 'testinterference' keyword parsing.
        */
        void TestInterferenceL( CStifItemParser* aItem );

        /**
        * Starts test interference.
        */
        void StartInterferenceL( TDesC& aName, CStifItemParser* aItem );

        /**
        * Stops test interference.
        */
        void StopInterferenceL( TDesC& aName );

        /**
        * Handles 'measurement' keyword parsing.
        */
        void MeasurementL( CStifItemParser* aItem );

        /**
        * Starts measurement.
        */
        void StartMeasurementL( const TDesC& aType, CStifItemParser* aItem );

        /**
        * Stops measurementplugin's measurement.
        */
        void StopMeasurementL( const TDesC& aType );

        /**
        * Adds new test case result. Used with 'allownextresult'
        * and 'allowerrorcodes' keywords.
        */
        void AddTestCaseResultL( CStifItemParser* aItem );
        
        /**
        * Sends key press event to AppUi
        */
        TBool PressKeyL( CStifItemParser* aItem );

        /**
        * Sends text to AppUi
        */
        TBool TypeTextL( CStifItemParser* aItem );

        /**
        * Sends pointer event to AppUi
        */
        TBool SendPointerEventL( CStifItemParser* aItem );
    public:     // Data

    protected:  // Data

    private:    // Data
        // Runner state
        TRunnerState            iState;

        // Backpointer to CTestScripter
        CTestScripter*          iTestScripter;

        // Allowed test case results
        RArray<TInt>            iTestCaseResults; 

        // Temporary line buffer 
        HBufC*                  iLine;

        // Store for requested events
        RPointerArray<HBufC>    iEventArray;

        // Pause timer
        RTimer                  iPauseTimer;

        // Pause value for timer
        TInt                     iRemainingTimeValue;

        // Loop times, counter and start position
        TInt                    iLoopTimes;
        TInt                    iLoopCounter;
        TInt                    iLoopStartPos;
        TBuf<10>                iLoopCounterDes;

        // Loop (timed loop option)
        TBool                   iTimedLoop;
        TTime                   iStartTime;
        TTimeIntervalMicroSeconds iExpectedLoopTime;

        // For OOM heap testing, FAILNEXT: count
        TInt                    iHeapFailNext;
        // For OOM heap testing, SETFAIL: type and value(rate)
        RHeap::TAllocFail       iHeapSetFailType;
        TInt                    iHeapSetFailValue;

        // Array for test interference
        RPointerArray<TTestInterference>    iTestInterferenceArray;

        // CStifTestMeasurement object
        RPointerArray<TTestMeasurement>     iTestMeasurementArray;

		// Message displayed when leave occurs during script parsing
        TBuf<256> iRunErrorMessage;
        
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// CLASS DECLARATION

// DESCRIPTION
// CTestContinue gets ContinueScript signals from test class
class CTestContinue 
    :public CActive
    {
    public: // Enumerations

    private: // Enumerations

    public:  // Constructors and destructor  
        /**
        * Two-phased constructor.
        */
        static CTestContinue* NewL( CTestScripter* aTestScripter,
                                    TTestObject* aObject );

        /**
        * Destructor.
        */
        ~CTestContinue();

    public: // New functions

    public: // Functions from base classes     
        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel.
        */
        void DoCancel();

        /**
        * RunError derived from CActive handles errors from active handler.
        */
        TInt RunError( TInt aError );

    protected:  // New functions

    protected:  // Functions from base classes

    private:
        /**
        * C++ default constructor.
        */
        CTestContinue( CTestScripter* aTestScripter,
                       TTestObject* aObject );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:     //Data

    protected:  // Data

    private:    // Data
        // Backpointer to CTestScripter
        CTestScripter*  iTestScripter;

        // Objectid for this object
        TTestObject*    iObject;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

// CLASS DECLARATION

// DESCRIPTION
// TTestObjectBase contains test object base information
class TTestObjectBase
    {
    public:
        enum TObjectType
            {
            EObjectNormal,
            EObjectKernel,
            };

    public: 
        TTestObjectBase( TObjectType aType );
        virtual ~TTestObjectBase();
    public:
        TName& ObjectId(){return iName;}
        TObjectType ObjectType(){ return iType; };

        virtual TInt RunMethodL( CStifItemParser& aItem ) = 0;
        virtual TBool Signal() = 0;
        virtual TBool Wait() = 0; 

    public: 
        TInt        iAsyncResult;

    protected:
        TObjectType iType;

    private:
        TName       iName;

    };

// CLASS DECLARATION

// DESCRIPTION
// TTestObject contains test object information
class TTestObject : public TTestObjectBase
    {
    public: 
        TTestObject();
        virtual ~TTestObject();

    public:
        TInt RunMethodL( CStifItemParser& aItem );
        inline TBool Signal(){ return ( iCount++ < 0 ); };
        inline TBool Wait(){ return ( --iCount >= 0 ); }; 

    public:
        CScriptBase*    iScript;
        CTestContinue*  iContinue;

    private:
        TInt iCount;
    };

// CLASS DECLARATION

// DESCRIPTION
// TTestObjectKernel contains kernel test object information
class TTestObjectKernel : public TTestObjectBase
    {
    public: 
        TTestObjectKernel();
        virtual ~TTestObjectKernel();

    public:
        TInt RunMethodL( CStifItemParser& aItem );

        // Signal and Wait not supported
        TBool Signal(){ return ETrue; };
        TBool Wait(){ return ETrue; }; 

        TName& LddName(){ return iLddName; }
        RStifKernelTestClass& KernelTestClass(){ return iTestClass; }

    private:
        RStifKernelTestClass    iTestClass;
        TName                   iLddName;

    };

// CLASS DECLARATION

// DESCRIPTION
// TTestModule contains test module information
class TTestModule
    {
    public: 
        TTestModule(){};
        ~TTestModule(){ iLibrary.Close(); };

    public:
        TName& ModuleName(){return iName;}

    public:
        RLibrary iLibrary;
        CInterfaceFactory iLibEntry;

    private:
        TName iName;

    };

// DESCRIPTION
// TTestInterference contains test interference object information
class TTestInterference
    {
    public: 
        TTestInterference(){ iInterference = NULL; };
        ~TTestInterference(){ delete iInterference; };

    public:

    public:
        // "object" name given in test case file.
        TName                   iName;
        // MSTIFTestInterference object.
        MSTIFTestInterference*  iInterference;

    private:

    };

// DESCRIPTION
// TTestMeasurement contains test measurement module information
class TTestMeasurement
    {
    public: 
        TTestMeasurement(){ iMeasurement = NULL; };
        ~TTestMeasurement(){ delete iMeasurement; };

    public:

    public:
        // "object" name given in test case file.
        TName                   iName;
        // MSTIFTestInterference object.
        CSTIFTestMeasurement*  iMeasurement;

    private:

    };

#endif        // TestScripter_H

// End of File
