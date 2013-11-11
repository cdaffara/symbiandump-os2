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
* TestInterface.
*
*/

#ifndef TEST_INTERFACE_H
#define TEST_INTERFACE_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <StifTestModule.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES

// This defines drive letters that is available in STIF TF.
//@spe IMPORT_C const TChar StifDriveLetters( TInt aDriveNumber, TInt& aCount );

// This works and used only in EKA2 environment and used from STIF
// TestFramework internally.
// Executable module uses defined capabilities(PlatSec's Capability model)
// to start session. Function for starting the TestServer and
// Testmodule/TestClass.
IMPORT_C TInt StartSession();

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION
const TInt KMaxStifInfoName = 0x10; 
typedef TBuf<KMaxStifInfoName> TStifInfoName;

// DESCRIPTION
// TTestInfo defines the information for test case and Test Module.
class TTestInfo 
    {
    public:
        IMPORT_C TTestInfo();
    public:
        // The name of Test Module.
        TName iModuleName;
        // Test case info for test case execution.
        TTestCaseInfo iTestCaseInfo;
        // Config file to be used.
        TFileName iConfig;
    };
    
// Packaged TTestInfo
typedef TPckg<TTestInfo> TTestInfoPckg;

// CLASS DECLARATION

// DESCRIPTION
// TFullTestResult defines the full test case result information.
class TFullTestResult
    {
    public:
        IMPORT_C TFullTestResult();

        enum TCaseExecutionResult
        {
            ECaseOngoing,
            ECaseExecuted,
            ECaseCancelled,
            ECaseErrorFromModule,
            ECaseLeave,
            ECasePanic,
            ECaseException,
            ECaseTimeout,
            ECaseSuicided,
        };

    public:
        // Test case result.
        TTestResult iTestResult;
        // Timestamp when test case was started.
        TTime iStartTime;
        // Timestamp when test case was ended.
        TTime iEndTime;
        
        // Test result type
        TCaseExecutionResult iCaseExecutionResultType;      
        // Test result code
        TInt iCaseExecutionResultCode;
    };

// Packaged TFullTestResult
typedef TPckg<TFullTestResult> TFullTestResultPckg;


// CLASS DECLARATION

// DESCRIPTION
// TTestProgress defines the progress information from Test Module.
class TTestProgress 
    {
    public:
        IMPORT_C TTestProgress();
    public:
        // "Priority" of the progress information.
        TInt iPosition;
        // Description of the data to be printed.
        TStifInfoName iDescription;
        // The actual progress information.
        TName iText;
    };

// Packaged TTestProgress
typedef TPckg<TTestProgress> TTestProgressPckg;


// CLASS DECLARATION

// DESCRIPTION
// TErrorNotification defines the error notifications
class TErrorNotification 
    {
    public:
        IMPORT_C TErrorNotification();
    public:
        // Error priority
        TInt iPriority;
        // Module reporting error
        TStifInfoName iModule;
        // The actual error information.
        TName iText;
    };

// Attributes that can be changed with SetAttribute
enum TAttribute
    {
    ELogPath,           // Log file path
    ERebootPath,        // Reboot path definition
    ERebootFilename,    // Reboot filename definition
    EStifMeasurementOn,  // STIF's Measurement enable
    EStifMeasurementOff // STIF's Measurement disable
    };
    
// Packaged TErrorNotification
typedef TPckg<TErrorNotification> TErrorNotificationPckg;

// Different STIF internal commands.
enum TStifCommand
    {
    EStifCmdReceive,
    EStifCmdSend,
    EStifCmdReboot,
    EStifCmdStoreState,
    EStifCmdGetStoredState,
    EStifCmdRebootProceed,
    EStifCmdReceiveCancel,
    EStifCmdMeasurement,
    };
typedef TPckg<TStifCommand> TStifCommandPckg;

// Class for Reboot parameter
class TRebootParams
    {
    public:
        CTestModuleIf::TRebootType aType;
    };

// Class for Reboot state parameters
class TRebootStateParams
    {
    public:
        TInt aCode;
        TName aName;
    };

// Class for Reboot stored parameters
class TGetRebootStoredParams
    {
    public:
        TInt aCode;
        TName aName;
    };

// Class for Reboot stored parameters(references)
class TGetRebootStoredParamsRef
    {
    public:
        TGetRebootStoredParamsRef( 
            TInt& aCode, TName& aName ):iCode(aCode),iName(aName){};
    public:
        TInt& iCode;
        TName& iName;
    };

// Class for measurement stored parameters
class TGetMeasurementOptions
    {
    public:
        TInt iOptions;
    };

// Class for measurement stored parameters(options)
class TGetMeasurementOptionsRef
    {
    public:
        TGetMeasurementOptionsRef( TInt& aOptions ): iOptions(aOptions){};
    public:
        TInt& iOptions;
    };

// This class offer generic attribute for passing client-server interface
union TParams
    {
    TDes*                       aRemoteMsgRef;
    const TDesC*                aRemoteMsgConstRef;
    TRebootParams*              aRebootType;
    TRebootStateParams*         aRebootState;
    TGetRebootStoredParamsRef*  aRebootStoredRef;
    TGetMeasurementOptionsRef*  aMeasurementOption;
    };

// Packaged TRebootParams
typedef TPckg<TRebootParams> TRebootParamsPckg;

// Packaged TRebootStateParams
typedef TPckg<TRebootStateParams> TRebootStateParamsPckg;

// Packaged TGetRebootStoredParams
typedef TPckg<TGetRebootStoredParams> TGetRebootStoredParamsPckg;

// Packaged TGetMeasurementParams
typedef TPckg<TGetMeasurementOptions> TGetMeasurementOptionsPckg;


// CLASS DECLARATION

// DESCRIPTION
// Base class for test measurement modules. Measurement modules implements
// measurerement operations.
NONSHARABLE_CLASS( CSTIFTestMeasurementImplementation ) : public CBase
    {
    public: // Enumerations

    private: // Enumerations

    public: // Constructors and destructor

    public: // New functions

        /**
        * Pure virtual measurement start command.
        * Start method's implementation is in measurement module.
        */
        virtual TInt Start( ) = 0;

        /**
        * Pure virtual measurement stop command.
        * Stop method's implementation is in measurement module.
        */
        virtual TInt Stop( ) = 0;

       /**
        * Pure virtual measurement stop command.
        * Stop method's implementation is in measurement module.
        */
        virtual CSTIFTestMeasurement::TSTIFMeasurementType MeasurementType() = 0;


    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

    public: //Data

    protected: // Data

    private: // Data

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes

    };

// STIF utility class
class TStifUtil
	{
    public: // Enumerations

    private: // Enumerations

    public: // Constructors and destructor

    public: // New functions    
		/**
		 * Checks if file path contains drive letter. If not file is serched
		 * on all drives and first hit is added to file name.
		 */
    	IMPORT_C static void CorrectFilePathL( TDes& aFilePath );
    	
    	/**
    	 * Method used by both STIF and STIF UI to retrieve version of STIF  
    	 */
    	IMPORT_C static void STIFVersion(TInt& aMajorV, TInt& aMinorV, TInt& aBuildV, TDes& aRelDate);
    
    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

    public: //Data

    protected: // Data

    private: // Data

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes
	
	};
    
    
#endif // TEST_INTERFACE_H

// End of File
