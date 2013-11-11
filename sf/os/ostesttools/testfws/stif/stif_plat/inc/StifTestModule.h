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
* CTestModuleBase and CTestModuleIf classes. Test Module inherits 
* the CTestModuleBase and uses the CTestModuleIf for printing 
* to user interface and event system control.
*
* The initialization of this interface must be done from the Test Module Server with:
* 	CTestModuleXXX *mod = new CTestModuleXXX;
* 	RThread thread;
* 	CTestModuleIf::NewL( this, (CTestModuleBase*) mod, thread.Id() );
* 
* CTestModuleIf is deleted by destructor of the CTestModuleBase. 
*
*/

#ifndef TESTMODULE_H
#define TESTMODULE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "StifCommand.h"

// CONSTANTS

// Define Old Test Module API version - API that doesn't support version of test module
const TInt KOldTestModuleAPIMajor = 1;
const TInt KOldTestModuleAPIMinor = 1;
const TInt KOldTestModuleAPIBuild = 0;

// Define Test Module API version
const TInt KTestModuleAPIMajor = 1;
const TInt KTestModuleAPIMinor = 2;
const TInt KTestModuleAPIBuild = 0;
const TInt KStifMaxResultDes = 128; // Maximum test result description length

// Max array value for STIF macros allow result.
const TInt KSTIFMacroResultArraySize = 10;

// Test module def-file check when setting parameters.
const TUint32 KStifTestModuleParameterChanged = 0xdeadbeef;

// MACROS
// Test result description. Maximum length is 128 characters.
typedef TBuf<KStifMaxResultDes> TResultDes;

// Macro for boolean.
#ifdef TL
    #pragma message( "============================================================" )
    #pragma message( " TL macro is already defined, now using STIF TF's TL macro" )
    #pragma message( "============================================================" )
#undef TL
#endif
// Macro for integer and one expected result.
#ifdef T1L
    #pragma message( "==============================================================" )
    #pragma message( " T1L macro is already defined, now using STIF TF's T1L macro" )
    #pragma message( "==============================================================" )
#undef T1L
#endif
// Macro for integer and two expected result.
#ifdef T2L
    #pragma message( "==============================================================" )
    #pragma message( " T2L macro is already defined, now using STIF TF's T2L macro" )
    #pragma message( "==============================================================" )
#undef T2L
#endif
// Macro for integer and three expected result.
#ifdef T3L
    #pragma message( "==============================================================" )
    #pragma message( " T3L macro is already defined, now using STIF TF's T3L macro" )
    #pragma message( "==============================================================" )
#undef T3L
#endif
// Macro for integer and four expected result.
#ifdef T4L
    #pragma message( "==============================================================" )
    #pragma message( " T4L macro is already defined, now using STIF TF's T4L macro" )
    #pragma message( "==============================================================" )
#undef T4L
#endif
// Macro for integer and five expected result.
#ifdef T5L
    #pragma message( "==============================================================" )
    #pragma message( " T5L macro is already defined, now using STIF TF's T5L macro" )
    #pragma message( "==============================================================" )
#undef T5L
#endif

// Macro for boolean. Recommended use with TestScripter's test class.
#ifdef TAL
    #pragma message( "==============================================================" )
    #pragma message( " TAL macro is already defined, now using STIF TF's TAL macro" )
    #pragma message( "==============================================================" )
#undef TAL
#endif
// Macro for integer and one expected result. Recommended use with TestScripter's test
// class.
#ifdef TA1L
    #pragma message( "================================================================" )
    #pragma message( " TA1L macro is already defined, now using STIF TF's TA1L macro" )
    #pragma message( "================================================================" )
#undef TA1L
#endif
// Macro for integer and two expected result. Recommended use with TestScripter's test
// class.
#ifdef TA2L
    #pragma message( "================================================================" )
    #pragma message( " TA2L macro is already defined, now using STIF TF's TA2L macro" )
    #pragma message( "================================================================" )
#undef TA2L
#endif
// Macro for integer and three expected result. Recommended use with TestScripter's test
// class.
#ifdef TA3L
    #pragma message( "================================================================" )
    #pragma message( " TA3L macro is already defined, now using STIF TF's TA3L macro" )
    #pragma message( "================================================================" )
#undef TA3L
#endif
// Macro for integer and four expected result. Recommended use with TestScripter's test
// class.
#ifdef TA4L
    #pragma message( "================================================================" )
    #pragma message( " TA4L macro is already defined, now using STIF TF's TA4L macro" )
    #pragma message( "================================================================" )
#undef TA4L
#endif
// Macro for integer and five expected result. Recommended use with TestScripter's test
// class.
#ifdef TA5L
    #pragma message( "================================================================" )
    #pragma message( " TA5L macro is already defined, now using STIF TF's TA5L macro" )
    #pragma message( "================================================================" )
#undef TA5L
#endif


//  TL ========================================================================
//  Macro for verify boolean value.
//  Example for use, 1: TL( Open() == KErrNone );
//                   1: TL( VerifyString1() == VerifyString2() );
//                   2: TL( 6 <= 8 );
//  ===========================================================================
    #define TL( aIsTrue )\
        if( (!(aIsTrue)) )\
        { TestModuleIf().StifMacroError( (0), (TText8*)__FILE__, __FUNCTION__, __LINE__ );\
        User::Leave( KErrArgument ); };

//  T1L =======================================================================
//  Macro for verify integer value from the expected 1 value.
//  Example: T1L( Open(), KErrNone );
//  ===========================================================================
    #define T1L( aResult, aExpected1 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) ) )\
            { TestModuleIf().StifMacroError(\
                (1), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), (aExpected1) );\
            User::Leave( KErrArgument ); };\
        };

//  T2L =======================================================================
//  Macro for verify integer value from the expected 1 and 2 values.
//  Example for use: T1L( Open(), KErrNone, KErrAlreadyExists );
//  ===========================================================================
    #define T2L( aResult, aExpected1, aExpected2 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) && (result) != (aExpected2) ) )\
            { TestModuleIf().StifMacroError(\
                (2), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), (aExpected1), (aExpected2) );\
            User::Leave( KErrArgument ); };\
        };

//  T3L =======================================================================
//  Macro for verify integer value from the expected 1, 2 and 3 values.
//  Example for use: T1L( Open(), KErrNone, KErrAlreadyExists, KErrDiskFull );
//  ===========================================================================
    #define T3L( aResult, aExpected1, aExpected2, aExpected3 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) && (result) != (aExpected2) && (result) != (aExpected3) ) )\
            { TestModuleIf().StifMacroError(\
                (3), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), (aExpected1), (aExpected2), (aExpected3) );\
            User::Leave( KErrArgument ); };\
        };

//  T4L =======================================================================
//  Macro for verify integer value from the expected 1, 2, 3 and 4 values.
//  Example for use:
//  T1L( Open(), KErrNone, KErrAlreadyExists, KErrDiskFull, KErrDisconnected );
//  ===========================================================================
    #define T4L( aResult, aExpected1, aExpected2, aExpected3, aExpected4 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) && \
              (result) != (aExpected2) && \
              (result) != (aExpected3) && \
              (result) != (aExpected4) ) )\
            { TestModuleIf().StifMacroError(\
                (4), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), \
                (aExpected1), (aExpected2), (aExpected3), (aExpected4) );\
            User::Leave( KErrArgument ); };\
        };

//  T5L =======================================================================
//  Macro for verify integer value from the expected 1, 2, 3, 4 and 5 values.
//  Example for use:
//  T1L( Open(), KErrNone, KErrDied, KErrInUse, KErrDisconnected, KErrAbort );
//  ===========================================================================
    #define T5L( aResult, aExpected1, aExpected2, aExpected3, aExpected4, aExpected5 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) && \
              (result) != (aExpected2) && \
              (result) != (aExpected3) && \
              (result) != (aExpected4) && \
              (result) != (aExpected5) ) )\
            { TestModuleIf().StifMacroError(\
                (5), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), \
                (aExpected1), (aExpected2), (aExpected3), (aExpected4), (aExpected5) );\
            User::Leave( KErrArgument ); };\
        };


//  TAL ========================================================================
//  Macro for verify boolean value. 
//  Recommended use with TestScripter's test class. Allowed result can be defined
//  with 'allownextresult' and 'allowerrorcodes' keywords.
//  Example for use, 1: TL( Open() == KErrNone );
//                   1: TL( VerifyString1() == VerifyString2() );
//                   2: TL( 6 <= 8 );
//  =============================================================================
    #define TAL( aIsTrue )\
        if( ( !(aIsTrue) ) && TestModuleIf().CheckAllowResult( 0 /* = EFalse*/ ) )\
            { TestModuleIf().StifMacroError( (0), (TText8*)__FILE__, __FUNCTION__, __LINE__ );\
            User::Leave( KErrArgument ); };

//  TA1L =======================================================================
//  Macro for verify integer value from the expected 1 value. 
//  Recommended use with TestScripter's test class. Allowed result can be defined
//  with 'allownextresult' and 'allowerrorcodes' keywords.
//  Example: T1L( Open(), KErrNone );
//  =============================================================================
    #define TA1L( aResult, aExpected1 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) ) && \
               TestModuleIf().CheckAllowResult( result ) ) \
            { TestModuleIf().StifMacroError(\
                (1), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), (aExpected1) );\
            User::Leave( KErrArgument ); };\
        };

//  TA2L =======================================================================
//  Macro for verify integer value from the expected 1 and 2 values.
//  Recommended use with TestScripter's test class. Allowed result can be defined
//  with 'allownextresult' and 'allowerrorcodes' keywords.
//  Example for use: T1L( Open(), KErrNone, KErrAlreadyExists );
//  =============================================================================
    #define TA2L( aResult, aExpected1, aExpected2 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) && (result) != (aExpected2) ) && \
               TestModuleIf().CheckAllowResult( result ) ) \
            { TestModuleIf().StifMacroError(\
                (2), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), (aExpected1), (aExpected2) );\
            User::Leave( KErrArgument ); };\
        };

//  TA3L =======================================================================
//  Macro for verify integer value from the expected 1, 2 and 3 values. 
//  Recommended use with TestScripter's test class. Allowed result can be defined
//  with 'allownextresult' and 'allowerrorcodes' keywords.
//  Example for use: T1L( Open(), KErrNone, KErrAlreadyExists, KErrDiskFull );
//  =============================================================================
    #define TA3L( aResult, aExpected1, aExpected2, aExpected3 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) && (result) != (aExpected2) && (result) != (aExpected3) ) && \
              TestModuleIf().CheckAllowResult( result ) )\
            { TestModuleIf().StifMacroError(\
                (3), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), (aExpected1), (aExpected2), (aExpected3) );\
            User::Leave( KErrArgument ); };\
        };

//  TA4L =======================================================================
//  Macro for verify integer value from the expected 1, 2, 3 and 4 values.
//  Recommended use with TestScripter's test class. Allowed result can be defined
//  with 'allownextresult' and 'allowerrorcodes' keywords.
//  Example for use:
//  T1L( Open(), KErrNone, KErrAlreadyExists, KErrDiskFull, KErrDisconnected );
//  =============================================================================
    #define TA4L( aResult, aExpected1, aExpected2, aExpected3, aExpected4 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) && \
              (result) != (aExpected2) && \
              (result) != (aExpected3) && \
              (result) != (aExpected4) ) && \
               TestModuleIf().CheckAllowResult( result ) ) \
            { TestModuleIf().StifMacroError(\
                (4), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), \
                (aExpected1), (aExpected2), (aExpected3), (aExpected4) );\
            User::Leave( KErrArgument ); };\
        };

//  TA5L =======================================================================
//  Macro for verify integer value from the expected 1, 2, 3, 4 and 5 values.
//  Recommended use with TestScripter's test class. Allowed result can be defined
//  with 'allownextresult' and 'allowerrorcodes' keywords.
//  Example for use:
//  T1L( Open(), KErrNone, KErrDied, KErrInUse, KErrDisconnected, KErrAbort );
//  =============================================================================
    #define TA5L( aResult, aExpected1, aExpected2, aExpected3, aExpected4, aExpected5 )\
        {\
        TInt result = aResult;\
        if( ( (result) != (aExpected1) && \
              (result) != (aExpected2) && \
              (result) != (aExpected3) && \
              (result) != (aExpected4) && \
              (result) != (aExpected5) ) && \
               TestModuleIf().CheckAllowResult( result ) ) \
            { TestModuleIf().StifMacroError(\
                (5), (TText8*)__FILE__, __FUNCTION__, __LINE__, (result), \
                (aExpected1), (aExpected2), (aExpected3), (aExpected4), (aExpected5) );\
            User::Leave( KErrArgument ); };\
        };


// Workaround for ARM RVCT compiler error. This added because if some
// environment do not include this definition.
#if !defined (NONSHARABLE_CLASS)
#define NONSHARABLE_CLASS(x) class x
#endif

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CTestModuleBase;
class TEventIf;
class CTestThreadContainer;
class MSTIFTestInterference;
class CSTIFTestMeasurement;
class CSTIFTestMeasurementImplementation;
class CUiEnvProxy;

// CLASS DECLARATION

// DESCRIPTION
// Base class for test case measurement . User can start measurement operations
// via this class.
NONSHARABLE_CLASS( CSTIFTestMeasurement )
        :public CBase 
    {
    public: // Enumerations

        /**
        * Enumeration for different measurement types.
        */
        enum TSTIFMeasurementType
            {
            KStifMeasurementEnableAll,
            KStifMeasurementPlugin01,
            KStifMeasurementPlugin02,
            KStifMeasurementPlugin03,
            KStifMeasurementPlugin04,
            KStifMeasurementPlugin05,
            KStifMeasurementBappeaProfiler,
            KStifMeasurementDisableAll,
            };

        /**
        * Enumeration for different measurement operations.
        */
        enum TSTIFMeasurementOperation
            {
            KMeasurementNew,
            KMeasurementStart,
            KMeasurementStop,
            };

        /**
        * Struct for measurement.
        */
        struct TStifMeasurementStruct
            {
            TSTIFMeasurementType        iMeasurementType;
            TFileName                   iConfigurationInfo;
            TSTIFMeasurementOperation   iMeasurementOperation;
            TInt                        iOperationResult;
            CTestModuleBase*            iPointerToMeasurementModule;
            };

    private: // Enumerations

    public: // Constructors and destructor

        /**
        * Two-phased constructor. Intializes test measurement module.
        */
        IMPORT_C static CSTIFTestMeasurement* NewL( 
                                            CTestModuleBase* aTestModuleBase,
                                            TSTIFMeasurementType aType,
                                            const TDesC& aConfigurationInfo );

    public: // New functions

        /**
        * Start commant for test measurement module.
        * Returns Symbian error code.
        */
        IMPORT_C TInt Start();

        /**
        * Stop command for test measurement module.
        * Returns Symbian error code.
        */
        IMPORT_C TInt Stop();

    public: // Functions from base classes

        /**
        * Destructor 
        */
        virtual ~CSTIFTestMeasurement();

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CSTIFTestMeasurement( CTestModuleBase* aTestModuleBase );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TSTIFMeasurementType aType,
                         const TDesC& aConfigurationInfo );

    public: //Data

    protected: // Data

    private: // Data

        // Pointer to STIF side
        CTestModuleBase*        iTestModuleBase;

        // Stuct that includes measurement related information
        TStifMeasurementStruct  iMeasurementStruct;

        // Indicates is measurement disable
        TInt                    iMeasurementOption;

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes
        friend class CTestModuleBase;

    };


// CLASS DECLARATION

// DESCRIPTION
// TTestResult defines the test case result information.
class TTestResult 
    {
    public:

        /**
        * Default constructor of TTestResult.
        */
        IMPORT_C TTestResult();

        /**
        * Sets result and result description of the test case.
        */
        IMPORT_C void SetResult( TInt aResultCode, const TDesC& aResultDes );

    public:

        // Test case result as Symbian OS error code.
        TInt iResult;
        // Descriptive test case result.
        TResultDes iResultDes;
    };

// CLASS DECLARATION

// DESCRIPTION
// TTestCaseInfo defines the information for test case execution.
class TTestCaseInfo
    {
    public:
        // Default test case priority values
        enum TPriority
            {    
            EPriorityLow = -100,
            EPriorityNormal = 0,
            EPriorityHigh = 100,
            };

    public:

        /**
        * Default constructor of TTestCaseInfo.
        */
        IMPORT_C TTestCaseInfo();

    public:

        // Test case number.
        TInt iCaseNumber;
        // Test case title.
        TName iTitle;
        // Test Case Timeout ( 0 == No timeout )
        TTimeIntervalMicroSeconds iTimeout; 
        // Test case priority
        TInt iPriority;

    };
// Packaged TTestCaseInfo
typedef TPckg<TTestCaseInfo> TTestCaseInfoPckg;

// CLASS DECLARATION

// DESCRIPTION
// CTestModuleIf provides interface for Test Module to perform 
// printing to user interface and to control event system.
class CTestModuleIf
        :public CBase 
    {
    public: // Enumerations

        // Possible test case exit reasons.
        enum TExitReason
            {    
            // Case will exit normally AND return test case result.
            ENormal,          
            // Case is expected to panic.
            EPanic,           
            // Case will end to exception.
            EException,
            };

        enum TRebootType
            {
            EDefaultReset = 0,  // Do not change the value
            EKernelReset,

            EDeviceReset0 = 100,
            EDeviceReset1,
            EDeviceReset2,
            EDeviceReset3,
            EDeviceReset4,
            EDeviceReset5
            };

        enum TTestBehavior
            {
            ETestLeaksMem       = 0x00000001,
            ETestLeaksRequests  = 0x00000002,
            ETestLeaksHandles   = 0x00000004,
            EOOMDisableLeakChecks = 0x00000008,
            };

    private: // Enumerations

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CTestModuleIf* NewL( CTestThreadContainer* aExecutionSession, 
                                             CTestModuleBase* aTestModule );

    public: // New functions

        /**
        * Printf is used to provide different information up to the UI 
        * that can be then printed e.g. to the Console Screen. 
        * The definition of data to be printed is copied to aDefinition. 
        * The aPriority tells how important information is returned. 
        * The priority can be used in the UI to decide if the information 
        * received from the Test Module will be discarded or not in 
        * the different performance situations. The priority is also 
        * used in the Test Module server and in the Test Engine to queue 
        * the Printf responses.
        * This method is implemented in Test Module Server and the Test Module 
        * can call it to provide printable information to the UI.
        */
        IMPORT_C void Printf( const TInt aPriority, 
                              const TDesC& aDefinition, 
                              TRefByValue<const TDesC> aFmt,... );

        /**
        * Event function is used to control and use the event system. 
        * TEvent &aEvent encapsulates the request type and 
        * the event name, see StifTestEventInterface.h for more information.
        * This method is implemented in Test Module Server and the Test Module 
        * can call it to control the event system.
        */
        IMPORT_C TInt Event( TEventIf& aEvent );

        /**
        * Asynchronous overload function for the previous. Only one 
        * asynchronous Event call can be active concurrently. Second 
        * concurrent call will block until first one is completed.
        */
        IMPORT_C void Event( TEventIf& aEvent, TRequestStatus& aStatus );

        /**
        * Cancel asynchronous event command. 
        */
        IMPORT_C TInt CancelEvent( TEventIf& aEvent,
                                   TRequestStatus* aStatus );

        /**
        * SetExitReason function is used to set different crash reasons to
        * acceptable test case result. 
        * Only one exit reason can be set at time. When function is called
        * again, previous exit reason is removed and new one is set to
        * acceptable exit reason.
        *
        * Function is typically used to accept test case panics as "passed"
        * test cases.
        * 
        * Test case that panics or gets exception is set to passed 
        * if aExitReason parameter matches with exit reason and 
        * aExitCode matches with panic code or exception type.
        */
        IMPORT_C void SetExitReason( const CTestModuleIf::TExitReason aExitReason, 
                                     const TInt aExitCode );


        /**
        * RemoteSend function is used send remote control protocol messages.
        */
        IMPORT_C TInt RemoteSend( const TDesC& aRemoteMsg );

        /**
        * RemoteReceive function is used receive remote 
        * control protocol messages.
        */
        IMPORT_C void RemoteReceive( TDes& aRemoteMsg, 
                                     TRequestStatus& aStatus );

        /**
        * RemoteReceiveCancel function is used cancel 
        * pending RemoteReceive.
        */
        IMPORT_C void RemoteReceiveCancel();

        /**
        * Reboot device, StoreState MUST have been called.
        */
        IMPORT_C TInt Reboot( TInt aType = EDefaultReset );

        /**
        * Store state before reboot. MUST be called before calling Reboot().
        */
        IMPORT_C TInt StoreState( TInt aCode, TName& aName );

        /**
        * Get state after reboot (stored with StoreState).
        */
        IMPORT_C TInt GetStoredState( TInt& aCode, TName& aName );

        /**
        * Set test case behavior, e.g. allow memory leak, handle leak etc.
        */
        IMPORT_C TInt SetBehavior( TTestBehavior aType, TAny* aPtr=NULL );

        /**
        * This can be called from test module but it is recommended to use
        * TL, T1L, T2L or T3L macro instead of this method.
        * For more information see macro definitions above on this file.
        */ 
        IMPORT_C TInt StifMacroError( TInt aMacroType,
                                      const TText8* aFile,
                                      const char* aFunction,
                                      TInt aLine,
                                      TInt aResult = 0,
                                      TInt aExpected1 = 0,
                                      TInt aExpected2 = 0,
                                      TInt aExpected3 = 0,
                                      TInt aExpected4 = 0,
                                      TInt aExpected5 = 0 );

        /**
        * Add thread handle to Array. Via array can handle test interference
        * thread's kill in panic etc. cases
        */ 
        TInt AddInterferenceThread( RThread aSTIFTestInterference );

        /**
        * Remove thread handle from Array.Test interference thread is stopped
        * and killed successfully
        */ 
        TInt RemoveInterferenceThread( RThread aSTIFTestInterference );

        /**
        * With this can be stored information about test measurement
        * to TestServer space.
        */ 
        TInt HandleMeasurementProcess( CSTIFTestMeasurement::TStifMeasurementStruct aSTIFMeasurementInfo );

        /**
        * Get measurement option(s) given from initialization file etc.
        */
        IMPORT_C TInt GetMeasurementOptions( TInt& aOptions );

        /**
        * Use with TAL, TA1L, TA2L, TA3L, TA4L and TA5L macros to allow
        * results.
        * Set test case allow result given by user. In TestScripter cases allow
        * result can set by 'allownextresult' or 'allowerrorcodes' keywords. In
        * Normal and Hardcoded test modules allow result can be set with this
        * method, reset should be done with ResetAllowResult method.
        */
        IMPORT_C TInt SetAllowResult( TInt aResult );

        /**
        * This is mainly used by STIF's TAL-TA5L macros internally.
        * Check is macros result allowed result.
        */
        IMPORT_C TInt CheckAllowResult( TInt aResult );

        /**
        * Use with TAL, TA1L, TA2L, TA3L, TA4L and TA5L macros to reset allowed
        * results.
        * Reset allow result(s) given with SetAllowResult. In TestScripter
        * cases this will be called automatically by STIF. Normal and Hardcoded
        * cases this should be called by user.
        */
        IMPORT_C TInt ResetAllowResult();

        /**
        * StopExecution function is used to stop the execution of test case.
        */
        IMPORT_C TInt StopExecution(TStopExecutionType aType, TInt aCode = KErrNone);

        /**
         * SendTestModuleVersion method is used to pass version of test module
         * to test engine
         */
        IMPORT_C TInt SendTestModuleVersion(TVersion aVersion, TFileName aModuleName);
        
        /**
         * SendTestModuleVersion method is used to pass version of test module
         * to test engine. This version with 3 parameters were added because version with
         * 2 parameters has arguments passed by value which is incorrect. So the third argument
         * was added only to overload the method and enable test modules using "old" version
         * (with 2 parameters) to work. Only this version (with three parameters) should be used.
         */
        IMPORT_C TInt SendTestModuleVersion(TVersion& aVersion, const TDesC& aModuleName, TBool aNewVersion);

        /**
        * Command function is used to send command to test engine.
        * It was created to provide possibility to kill test case by itself.
        */
        TInt Command(TCommand aCommand, const TDesC8& aParamsPckg);

        /**
         * Get test case execution arguments
         */
        IMPORT_C const TDesC& GetTestCaseArguments() const;        

        /**
        * GetTestCaseTitleL function is used to obtain test case title.
        */
        IMPORT_C void GetTestCaseTitleL(TDes& aTestCaseTitle);

        /**
        * Checks if testserver supports UI testing
        */
		IMPORT_C TBool UITesting();

        /**
        * Gets UIEnvProxy
        */
		IMPORT_C CUiEnvProxy* GetUiEnvProxy();

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CTestModuleIf( CTestThreadContainer* aTestExecution );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CTestModuleBase* aTestModule );

        /**
        * Destructor is also private. 
        * ONLY friend CTestModuleBase can delete CTestModuleIf.
        */
        ~CTestModuleIf();

        /**
        * Check that server is alive.
        */ 
        TBool IsServerAlive();

    public: //Data

    protected: // Data

    private: // Data
        CTestThreadContainer* iTestExecution;

        // Indicates is reboot operation ready to start.
        TBool iIsRebootReady;

        // Indicated StoreState() method's call times.
        TInt iStoreStateCounter;

        // Test case allowed results. Array is a static one for avoid dynamic
        // allocations.
        TInt iTestCaseResults[KSTIFMacroResultArraySize]; 
        // Test case allowed results count. This is used with
        // iTestCaseResults array.
        TInt iAllowTestCaseResultsCount; 

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes
        friend class CTestModuleBase;

    };


// CLASS DECLARATION

// DESCRIPTION
// Base class for test module parameters. According to parameters may
// configure test module. e.g. heap and stack configuring.
class CTestModuleParam : public CBase 
    {

    public: // Enumerations

        /**
        * Evolution version for test module parameters.
        **/
        enum TParameterVersio
            {
            EVersio1 = 1, // For heap and stack configuring.
            EVersio2 = 2, // For future needs.
            EVersio3 = 3, // For future needs.
            EVersio4 = 4, // For future needs.
            EVersio5 = 5, // For future needs.

            };

    private: // Enumerations

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        //IMPORT_C static CTestModuleParam* NewL();

        /**
        *
        */
        //IMPORT_C ~CTestModuleParam();

    public: // New functions

        /**
        * 
        */
        virtual TInt Version() = 0;

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        //CTestModuleParam();

        /**
        * By default Symbian OS constructor is private.
        */
        //void ConstructL();

    public: //Data

    protected: // Data

    private: // Data

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes

    };

// CLASS DECLARATION

// DESCRIPTION
// CTestModuleParamVer01 implements evolution version 1 for test module's 
// heap and stack configuring.
class CTestModuleParamVer01 : public CTestModuleParam
    {
    public: // Enumerations

    private: // Enumerations

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CTestModuleParamVer01* NewL();
        //IMPORT_C CTestModuleParamVer01* NewL();

        /**
        * C++ default constructor.
        */
        CTestModuleParamVer01();

        /**
        * Destructor.
        */
        IMPORT_C ~CTestModuleParamVer01(); 

    private: // Constructors and destructor

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public: // New functions

        /**
        *
        */
        inline TInt Version() { return iVersio; };

    public: // Functions from base classes
        
    protected: // New functions

    protected: // Functions from base classes

    private:

    public: // Data

        /**
        * Test thread stack size defined by user.
        **/
        TInt iTestThreadStackSize;

        /**
        * Test thread heap minimum size defined by user.
        **/
        TInt iTestThreadMinHeap;

        /**
        * Test thread heap maximum size defined by user.
        **/
        TInt iTestThreadMaxHeap;

    protected: // Data

    private: // Data

        /**
        *
        **/
        TInt iVersio;

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes

    };

// CLASS DECLARATION

// DESCRIPTION
// Test Module inherits the CTestModuleBase class that is used to provide 
// simple interface between Test Engine and Test Module. CTestModuleBase 
// consists of pure virtual methods that the Test Module shall implement 
// and virtual methods that the Test Module should implement.
class CTestModuleBase
        :public CBase
    {
    public: // Enumerations
    
        // For OOM execution
        enum TOOMFailureType 
            {    
            EFailNext
            };
    private: // Enumerations

    public: // Version query.

        /**
        * Returns the API version which this test module complies.
        * Test Framework uses this function to verify that loaded module
        * can be used in current framework release.
        *
        * Note that this function must always be the very first function
        * that is defined in CTestModuleBase.
        */
        virtual inline TVersion Version()
                                 { return TVersion ( KTestModuleAPIMajor,
                                                     KTestModuleAPIMinor,
                                                     KTestModuleAPIBuild); }

    public: // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CTestModuleBase(): iTestModuleIf(0) {};

        /**
        * Destructor.
        */
        virtual ~CTestModuleBase(){ delete iTestModuleIf; }; 

    public: // New functions

        /**
        * InitL is used to initialize the Test Module. The Test Modules may 
        * use the initialization file to read parameters for Test Module 
        * initialization but they can also have their own configure file or 
        * some other routine to initialize themselves.
        * This method is virtual and it is not mandatory to implement it 
        * to Test Module.
        */
        virtual TInt InitL( TFileName& /* aIniFile */,
                           TBool /* aFirstTime */ )
            { return KErrNone; };

        /**
        * Test cases are inquired from the Test Module by calling 
        * GetTestCasesL. Test cases may be read from the testcase file 
        * specified by aTestCaseFile or they can be e.g. hardcoded to testmodule. 
        * If the Test Module does not use testcase files for test case 
        * definition it does not use aTestCaseFile parameter. Test cases are 
        * appended to RPointerArray<TTestCaseInfo> that is a list 
        * consisting of several TTestCaseInfo objects.
        * The STIF Test Framework will be the owner of the objects stored to 
        * RPointerArray after the GetTestCasesL has appended them to the list
        * and it does the memory deallocation.
        * This method is pure virtual and the Test Module must implement it.
        */
        virtual TInt GetTestCasesL( const TFileName& aTestCaseFile, 
            RPointerArray<TTestCaseInfo>& aTestCases ) = 0;

        /**
        * RunTestCaseL is used to run an individual test case specified 
        * by aCaseNumber and aTestCaseFile (optional). 
        * This method is synchronous and test case result is returned as 
        * reference parameter aResult value. 
        * The function shall return KErrNone if case is started succesfully.
        * If test case is not available (i.e invalid testcase file or
        * or case number), then function shall return KErrNotFound. In other 
        * error situations another Symbian error code is returned.                
        * This method is pure virtual and the Test Module must implement it. 
        */
        virtual TInt RunTestCaseL( const TInt aCaseNumber, 
                                  const TFileName& aTestCaseFile,
                                  TTestResult& aResult ) = 0;

        /**
        * TestModuleIf returns the handle to the CTestModuleIf interface. 
        * This is an utility function that can be used if the upper layer 
        * interface is needed to be used.
        * This method must not be called from the constructor of Test Module 
        * that derives the CTestModuleBase.
        */
        inline CTestModuleIf& TestModuleIf(){ return *iTestModuleIf; };

        /**
        * Used to perform the test environment setup for a particular OOM test 
        * case. The Test Modules may use the initialization file to read parameters 
        * for Test Module initialization but they can also have their own 
        * configure file or some other routine to initialize themselves.
        *
        * This method is virtual and will be implemented if test case is to be
        * executed using OOM conditions.
        */
        virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                         const TInt /* aCaseNumber */ ) {};

        /**
        * Used to perform the test environment cleanup for a particular OOM 
        * test case.
        *         
        * This method is virtual and will be implemented if test case is to be
        * executed using OOM conditions.
        */
        virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                       const TInt /* aCaseNumber */ ) {};

        /**
        * Used to check if a particular Test Case should be run in OOM 
        * conditions and which memory allocations should fail.
        * Method returns ETrue if test is should run in OOM conditions, EFalse 
        * otherwise. 
        *
        * This method is virtual and will be implemented if test case is to be
        * executed using OOM conditions.
        */
        virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                     const TInt /* aCaseNumber */, 
                                     TOOMFailureType& aFailureType, 
                                     TInt& /* aFirstMemFailure */, 
                                     TInt& /* aLastMemFailure */ ) 
            { 
            aFailureType = EFailNext;
            return EFalse; 
            };

        /**
        * Used in OOM testing only. Provides a way to the derived TestModule to
        * handle warnings related to non-leaving or TRAPped allocations.
        * 
        * In some cases the allocation should be skipped, either due to problems
        * in the OS code or components used by the code being tested, or even
        * inside the tested components which are implemented this way on purpose
        * (by design), so it is important to give the tester a way to bypass
        * allocation failures.
        *
        * This method is virtual and will be implemented if test case is to be
        * executed using OOM conditions.
        */
        virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                        const TInt /* aCaseNumber */, 
                                        TInt& /* aFailNextValue */) {};

    public: // Functions from base classes

    protected: // New functions

    protected: // Functions from base classes

    private:

    public: // Data

    protected: // Data

    private: // Data
        CTestModuleIf* iTestModuleIf;

    public: // Friend classes

    protected: // Friend classes

    private: // Friend classes
        friend class CTestModuleIf;
        friend class CTestModuleParam;
        friend class MSTIFTestInterference;
        friend class CSTIFInterferenceThread;
        friend class CSTIFTestMeasurement;

    };

#endif      // TESTMODULE_H  

// End of File
