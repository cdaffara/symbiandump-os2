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
* Description: This file contains the header file of the CTestReport.
*
*/

#ifndef TEST_REPORT_H
#define TEST_REPORT_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "TestEngineClient.h"
#include "TestEngine.h"

// CONSTANTS
// Maximum length for date or time definitions 'Thursday 06th November 2003'
const TInt KMaxDateAndTimeStamp = 30;

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CStifLogger;

// CLASS DECLARATION
// None

// DESCRIPTION
// CTestReport is a class that can generate different test reports.
class CTestReport
        : public CBase
    {
    public:     // Enumerations

        // TTestReportMode defines the mode of test report
        enum TTestReportMode
            {
            ETestReportBlank = 0x00000000,
            ETestReportSummary = 0x00000001,
            ETestReportEnvironment = 0x00000002,
            ETestReportCases = 0x00000004,
            ETestReportFull = ETestReportSummary | ETestReportEnvironment | ETestReportCases
            };

        // TTestReportOutput defines the type of test report output
        enum TTestReportOutput
            {
            ETestReportNone = 0x00000000,
            ETestReportDebug = 0x00000001,
            ETestReportPlainText = 0x00000002,
            ETestReportHtml = 0x00000004,
            ETestReportAll = ETestReportDebug | ETestReportPlainText | ETestReportHtml
            };

    public:     // Structured classes

        /**
        *  TTestReportHeader includes header information for test report.
        *
        */
        struct THwInfo
            {
            TInt iManufacturer;
            TInt iMachineUid;
            TInt iModel;
            TInt iHwRev;
            TInt iCpu;
            TInt iCpuSpeed;
            TInt iLanguage;
            };

        struct TSwInfo
            {
            TInt iSwRev;
            TInt iSwBuild;
            };

        struct TMemoryInfo
            {
            TInt iRAM;
            TInt iRAMFree;
            };

        class TTestHWInfo
            {
            public: // New functions
                TTestHWInfo();

            public: // Data
                // HW Info
                THwInfo         iHwInfo;
                // SW Info
                TSwInfo         iSwInfo;
                // Memory Info
                TMemoryInfo     iMemoryInfo;
            };

        /**
        *  TTestCaseSummary includes information for summary of test cases.
        */
        class TTestCaseSummary
            {
            public:     // New functions
                TTestCaseSummary();

            public:     // Data
                // Test Info
                TTestInfo           iTestInfo;
                // Test Result
                TFullTestResult     iFullTestResult;
            };

        /**
        *  TTestSummary includes information for test report summary.
        */
        class TTestSummary
            {
            public:     // New functions
                TTestSummary( const TName& aName );

            public:     // Data
                // Name (e.g. test module name )
                const TName     iName;
                // Count of passed test cases
                TInt            iPassedCases;
                // Count of failed test cases
                TInt            iFailedCases;
                // Count of crashed cases
                TInt            iCrashedCases;
                // Count of timed out cases
                TInt            iTimeoutCases;
            };

    private:    // Enumerations

    public:     // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTestReport* NewL( CTestReportSettings& aTestReportSettings,
                                  const TTestReportMode aReportMode );
        
        /**
        * Destructor.
        */
        ~CTestReport();
        
    public:     // New functions
        
        /**
        * Add test case result.
        */
        void AddTestCaseResultL( const TTestInfo& aTestInfo,
                                 const TFullTestResult& aTestResult,
                                 const TInt aError );

        /**
        * Generate result reports.
        */
        void GenerateReportL();
        
        /**
        * Update result reports.
        */
        void UpdateReportSummaryL();
        
        /**
         * Adds version of test module to RPointerArray
         */
        void AddTestModuleVersion(TTestModuleVersionInfo& aVersion);

    public:     // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(  CTestReportSettings& aTestReportSettings );

        /**
        * Default C++ constructor.
        */
        CTestReport( const TTestReportMode aReportMode );
        
        /**
        * Write result file header.
        */
        void WriteHeaderL();
        
        /**
        * Write result file trailer.
        */
        void WriteTrailerL();
        
        /**
        * Write data to file.
        */
        void WriteLineL( TRefByValue<const TDesC> aStr,... );
        
        /**
        * Write delimiter line.
        */
        void WriteDelimiterL( const TDesC& aDelimiter, TInt aCount );
        
        /**
        * Adds needed tags to get valid xml file.
        */
        void CloseXMLTagsInUnfinishedFileL(void);

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Report file handles
        RFs                             iFs;
        RFile                           iFile;
        CStifLogger::TLoggerType        iFormat;
        CStifLogger::TOutput            iOutput;
    
        // Report Mode
        TTestReportMode                 iReportMode;

        // Report HW info
        TTestHWInfo                     iReportHWInfo;

        // Total summary
        TTestSummary*                   iTotalSummary;

        // Test summary array
        RPointerArray<TTestSummary>     iTestSummaries;
        
        // Summary start position
        TInt                            iSummaryPos;
        
        // Versions of test modules position
        TInt							iModulesVersionsPos;
        
        // Test modules versions
        RPointerArray<TTestModuleVersionInfo> iTestModulesVersionsInfo;

        // Will report be written in xml format 
        TBool                           iXML;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // TEST_REPORT_H

// End of File
