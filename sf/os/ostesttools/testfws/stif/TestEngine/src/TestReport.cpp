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
* Description: This module contains implementation of CTestReport 
* class member functions.
*
*/

// INCLUDE FILES
#include <e32svr.h>
#include <f32file.h>
#include <hal.h>
#include "TestReport.h"
#include "TestEngineCommon.h"
#include "Logging.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
const TInt KMaxLenEol = 6;
const TInt KMaxReportData = 256;
typedef TBuf<KMaxReportData> TPrintInfo;
const TInt KSummaryLineEndLen = 12;
_LIT( KSummaryLineEnd,  "            ");

// XML Tags (alphabetically)
_LIT(KXMLCaseNumberTag,          "<CaseNumber>");
_LIT(KXMLCaseNumberTagEnd,       "</CaseNumber>");
_LIT(KXMLCaseTitleTag,           "<Title>");
_LIT(KXMLCaseTitleTagEnd,        "</Title>");
_LIT(KXMLConfigFileTag,          "<Config>");
_LIT(KXMLConfigFileTagEnd,       "</Config>");
_LIT(KXMLCpuSpeedTag,            "<CPUSpeed>");
_LIT(KXMLCpuSpeedTagEnd,         "</CPUSpeed>");
_LIT(KXMLCpuTag,                 "<CPU>");
_LIT(KXMLCpuTagEnd,              "</CPU>");
_LIT(KXMLCrashedTag,             "<Crashed>");
_LIT(KXMLCrashedTagEnd,          "</Crashed>");
_LIT(KXMLDateTag,                "<Date>");
_LIT(KXMLDateTagEnd,             "</Date>");
_LIT(KXMLDllSummaryTag,          "<Dlls>");
_LIT(KXMLDllSummaryTagEnd,       "</Dlls>");
_LIT(KXMLDllTag,                 "<Dll>");
_LIT(KXMLDllTagEnd,              "</Dll>");
_LIT(KXMLEndTimeTag,             "<EndTime>");
_LIT(KXMLEndTimeTagEnd,          "</EndTime>");
_LIT(KXMLEnvironmentInfoTag,     "<EnvironmentInfo>");
_LIT(KXMLEnvironmentInfoTagEnd,  "</EnvironmentInfo>");
_LIT(KXMLExecutionResultTag,     "<ExecutionResultCode>");
_LIT(KXMLExecutionResultTagEnd,  "</ExecutionResultCode>");
_LIT(KXMLFailedTag,              "<Failed>");
_LIT(KXMLFailedTagEnd,           "</Failed>");
_LIT(KXMLFileNameTag,            "<FileName>");
_LIT(KXMLFileNameTagEnd,         "</FileName>");
_LIT(KXMLHardwareInfoTag,        "<HardwareInfo>");
_LIT(KXMLHardwareInfoTagEnd,     "</HardwareInfo>");
_LIT(KXMLHwRevisionTag,          "<HwRevision>");
_LIT(KXMLHwRevisionTagEnd,       "</HwRevision>");
_LIT(KXMLLanguageTag,            "<Language>");
_LIT(KXMLLanguageTagEnd,         "</Language>");
_LIT(KXMLMachineUidTag,          "<MachineUID>");
_LIT(KXMLMachineUidTagEnd,       "</MachineUID>");
_LIT(KXMLManufacturerTag,        "<Manufacturer>");
_LIT(KXMLManufacturerTagEnd,     "</Manufacturer>");
_LIT(KXMLMemoryInfoTag,          "<MemoryInfo>");
_LIT(KXMLMemoryInfoTagEnd,       "</MemoryInfo>");
_LIT(KXMLModelTag,               "<Model>");
_LIT(KXMLModelTagEnd,            "</Model>");
_LIT(KXMLModuleNameTag,          "<ModuleName>");
_LIT(KXMLModuleNameTagEnd,       "</ModuleName>");
_LIT(KXMLModuleSummaryTag,       "<Modules>");
_LIT(KXMLModuleSummaryTagEnd,    "</Modules>");
_LIT(KXMLModuleTag,              "<Module>");
_LIT(KXMLModuleTagEnd,           "</Module>");
_LIT(KXMLPassedTag,              "<Passed>");
_LIT(KXMLPassedTagEnd,           "</Passed>");
_LIT(KXMLRamFreeTag,             "<RAMFree>");
_LIT(KXMLRamFreeTagEnd,          "</RAMFree>");
_LIT(KXMLRamTag,                 "<RAM>");
_LIT(KXMLRamTagEnd,              "</RAM>");
_LIT(KXMLResultDescrTag,         "<ResultDescription>");
_LIT(KXMLResultDescrTagEnd,      "</ResultDescription>");
_LIT(KXMLResultTag,              "<Result>");
_LIT(KXMLResultTagEnd,           "</Result>");
_LIT(KXMLSoftwareInfoTag,        "<SoftwareInfo>");
_LIT(KXMLSoftwareInfoTagEnd,     "</SoftwareInfo>");
_LIT(KXMLStartTimeTag,           "<StartTime>");
_LIT(KXMLStartTimeTagEnd,        "</StartTime>");
_LIT(KXMLSwBuildTag,             "<SwBuild>");
_LIT(KXMLSwBuildTagEnd,          "</SwBuild>");
_LIT(KXMLSwRevisionTag,          "<SwRevision>");
_LIT(KXMLSwRevisionTagEnd,       "</SwRevision>");
_LIT(KXMLTestCasesSummaryTag,    "<TestCasesSummary>");
_LIT(KXMLTestCasesSummaryTagEnd, "</TestCasesSummary>");
_LIT(KXMLTestCasesTag,           "<TestCases>");
_LIT(KXMLTestCasesTagEnd,        "</TestCases>");
_LIT(KXMLTestCaseTag,            "<TestCase>");
_LIT(KXMLTestCaseTagEnd,         "</TestCase>");
_LIT(KXMLTestReportTag,          "<TestReport>");
_LIT(KXMLTestReportTagEnd,       "</TestReport>");
_LIT(KXMLTestResultTag,          "<ResultCode>");
_LIT(KXMLTestResultTagEnd,       "</ResultCode>");
_LIT(KXMLTimeoutedTag,           "<Timeout>");
_LIT(KXMLTimeoutedTagEnd,        "</Timeout>");
_LIT(KXMLTimeTag,                "<Time>");
_LIT(KXMLTimeTagEnd,             "</Time>");
_LIT(KXMLTotalTag,               "<Total>");
_LIT(KXMLTotalTagEnd,            "</Total>");
_LIT(KXMLVersionTag,             "<Version>");
_LIT(KXMLVersionTagEnd,          "</Version>");
_LIT(KXMLSTIFVersionTag,         "<STIFVersion>");
_LIT(KXMLSTIFVersionTagEnd,      "</STIFVersion>");

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ==================== LOCAL FUNCTIONS =======================================
// None

// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: CTestReport

    Description: Default constructor

    C++ default constructor can NOT contain any code, that
    might leave.

    Parameters: const TTestReportMode aReportMode: in: Report mode

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestReport::CTestReport( const TTestReportMode aReportMode ):
    iReportMode( aReportMode ) 
    {
    iXML = EFalse;
    iReportHWInfo = TTestHWInfo();

    }

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: ConstructL

    Description: Symbian OS second phase constructor

    Symbian OS default constructor can leave.

    Parameters: CTestReportSettings& aTestReportSettings: in: Report settings

    Return Values: None

    Errors/Exceptions: Leaves if memory allocation for iTotalSummary fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestReport::ConstructL( CTestReportSettings& aTestReportSettings )
    {
    // Create summary for all test cases
    _LIT( KName, "All test cases" );
    iTotalSummary = new ( ELeave ) TTestSummary( KName() );

    User::LeaveIfError( iFs.Connect() );
    
    TPtrC path;
    path.Set( aTestReportSettings.iPath->Des() );
    TPtrC name;
    name.Set( aTestReportSettings.iName->Des() );

    iFormat = aTestReportSettings.iFormat;
    iOutput = aTestReportSettings.iOutput;
    iXML = aTestReportSettings.iXML;
    
    // XML format can be used only when report is of txt file type.
    if(iFormat != CStifLogger::ETxt && iOutput != CStifLogger::EFile && iXML)
        {
        RDebug::Print(_L("Stif: XML report is available only when TestReportFormat is TXT and TestReportOutput is FILE"));
        iXML = EFalse;
        }
    
    if( iOutput == CStifLogger::EFile )
        {
        iFs.MkDirAll( path );
        
        HBufC* pathAndFileBuf = HBufC::NewLC( path.Length() + name.Length() + 5 );
        TPtr pathAndFile(pathAndFileBuf->Des() );
        pathAndFile.Append( path );
        pathAndFile.Append( name );

        if(iXML)
            {
            pathAndFile.Append( _L(".xml") );
            }
        else
            {
            if( ( iFormat == CStifLogger::EHtml ) && 
                ( iOutput == CStifLogger::EFile ) )
                {
                pathAndFile.Append( _L(".html") );
                }
            else 
                {
                pathAndFile.Append( _L(".txt") );
                }
            }
        
        if( aTestReportSettings.iOverwrite )
            {
            User::LeaveIfError( iFile.Replace( iFs, 
                           pathAndFile,
                           EFileWrite | EFileStreamText | EFileShareExclusive ) );
            }
        else
            {
            TInt fileOpen = iFile.Open( iFs, 
                                        pathAndFile, 
                                        EFileWrite | EFileStreamText | EFileShareAny );
            if( fileOpen == KErrNotFound )
                {
                User::LeaveIfError( 
                    iFile.Create( iFs, 
                                  pathAndFile, 
                                  EFileWrite | EFileStreamText | EFileShareExclusive ) );
                }
            else if( fileOpen == KErrNone )
                {
                TInt endPosOfFile = 0;
                User::LeaveIfError( iFile.Seek( ESeekEnd, endPosOfFile ) );
                }
            else
                {
                User::Leave( fileOpen );
                }
            }
       
        CleanupStack::PopAndDestroy( pathAndFileBuf );
        }
            
    WriteHeaderL();
    
    // Add temporarily closing tags to keep valid xml structure
    CloseXMLTagsInUnfinishedFileL();
    }

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: NewL

    Description: Two-phased constructor.

    Parameters: CTestReportSettings& aTestReportSettings: in: Report settings
                const TTestReportMode aReportMode: in: Report mode
    
    Return Values: CTestReport* : pointer to created object

    Errors/Exceptions: Leaves if memory allocation for object fails
                       Leaves if ConstructL leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestReport* CTestReport::NewL( CTestReportSettings& aTestReportSettings,
                                const TTestReportMode aReportMode )
    {
    CTestReport* self = new ( ELeave ) CTestReport( aReportMode );
    CleanupStack::PushL( self );
    self->ConstructL( aTestReportSettings );
    CleanupStack::Pop( self );
    return self;

    }

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: ~CTestReport

    Description: Destructor

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestReport::~CTestReport()
    {
    iTestModulesVersionsInfo.ResetAndDestroy();
    iTestModulesVersionsInfo.Close();
        
    // Reset and destroy arrays
    iTestSummaries.ResetAndDestroy();
    delete iTotalSummary;
    if( iOutput != CStifLogger::ERDebug )
        {
        // Delete file
        iFile.Close();
        iFs.Close();
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: AddTestCaseResultL

    Description: Add new test case result.

    Parameters: const TTestInfo& aTestInfo: in: TTestInfo: Test info
                const TFullTestResult& aTestResult: in: TTestResult: Testresult
                const TInt aError: in: Symbian OS error: Additional error code

    Return Values: None

    Errors/Exceptions: Leave is iReportGenerated is generated
                       Leave if summary creation fails
                       Leave if summary adding fails

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestReport::AddTestCaseResultL( const TTestInfo& aTestInfo,
                                        const TFullTestResult& aTestResult,
                                        const TInt aError )
    {
    // Create summary for this test case
    TTestCaseSummary summary;
    
    summary.iTestInfo = aTestInfo;
    summary.iFullTestResult = aTestResult;

    TBool passed( EFalse );
    TBool crashed( EFalse );
    TBool timeout( EFalse );
    if( ( aError == KErrNone ) && 
         ( summary.iFullTestResult.iCaseExecutionResultType == TFullTestResult::ECaseExecuted ) &&
         ( KErrNone == summary.iFullTestResult.iTestResult.iResult ) )
        {
        passed = ETrue;
        }
    else if( summary.iFullTestResult.iCaseExecutionResultType == TFullTestResult::ECaseTimeout )
        {
        timeout = ETrue;
        }
    else if( ( summary.iFullTestResult.iCaseExecutionResultCode != KErrNone )
    		|| ( summary.iFullTestResult.iCaseExecutionResultType == TFullTestResult::ECasePanic )
    		|| ( summary.iFullTestResult.iCaseExecutionResultType == TFullTestResult::ECaseException ) )
        {
        crashed = ETrue;
        }

    if ( iReportMode & ETestReportCases )
        {
        // Print summary to file
        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLTestCaseTag);
            WriteLineL(_L("%S%S%S"), &KXMLModuleNameTag, &summary.iTestInfo.iModuleName, &KXMLModuleNameTagEnd);
            if(summary.iTestInfo.iConfig != KNullDesC)
                {
                WriteLineL(_L("%S%S%S"), &KXMLConfigFileTag, &summary.iTestInfo.iConfig, &KXMLConfigFileTagEnd);
                }
            WriteLineL(_L("%S%d%S"), &KXMLCaseNumberTag, summary.iTestInfo.iTestCaseInfo.iCaseNumber, &KXMLCaseNumberTagEnd);
            WriteLineL(_L("%S%S%S"), &KXMLCaseTitleTag, &summary.iTestInfo.iTestCaseInfo.iTitle, &KXMLCaseTitleTagEnd);
            }
        else
            {
            WriteLineL( _L("[%S][%S][%d] Title:[%S]"),
                &summary.iTestInfo.iModuleName,
                &summary.iTestInfo.iConfig,
                summary.iTestInfo.iTestCaseInfo.iCaseNumber,
                &summary.iTestInfo.iTestCaseInfo.iTitle );
            }

        const TInt KTimeFieldLength = 30;
        TBuf<KTimeFieldLength> startTime;
        TBuf<KTimeFieldLength> endTime;
        _LIT(KDateString4,"%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
        summary.iFullTestResult.iStartTime.FormatL(
            startTime,KDateString4 );
        summary.iFullTestResult.iEndTime.FormatL(
            endTime,KDateString4 );
            
        // Result description needs max length 0x80(Must be same as
        // TResultDes length) + specific characters see below.
        TBuf<KMaxReportData> printBuf2;
        TInt code = CStifLogger::ENoStyle;
        if( ( summary.iFullTestResult.iCaseExecutionResultType 
        	!= TFullTestResult::ECasePanic ) &&
        	( summary.iFullTestResult.iCaseExecutionResultCode 
            == KErrNone ) )
            {
            printBuf2.AppendFormat( _L("\tResult: %d [%S]"),
                summary.iFullTestResult.iTestResult.iResult,
                &summary.iFullTestResult.iTestResult.iResultDes );

            if ( KErrNone == 
                 summary.iFullTestResult.iTestResult.iResult )
                {
                printBuf2.AppendFormat( _L(" ==> PASSED"));
                }
            else
                {
                code = CStifLogger::ERed;
                printBuf2.AppendFormat( _L(" ==> FAILED"));
                }
            }
        else
            {
            code = CStifLogger::ERed;
            printBuf2.AppendFormat( _L("\tCaseExecutionResult: %S with %d"),
                &summary.iFullTestResult.iTestResult.iResultDes,
                summary.iFullTestResult.iCaseExecutionResultCode );
            }

        if(iXML)
            {
            WriteLineL(_L("%S%S%S"), &KXMLStartTimeTag, &startTime, &KXMLStartTimeTagEnd);
            WriteLineL(_L("%S%S%S"), &KXMLEndTimeTag, &endTime, &KXMLEndTimeTagEnd);
            WriteLineL(_L("%S%d%S"), &KXMLTestResultTag, summary.iFullTestResult.iTestResult.iResult, &KXMLTestResultTagEnd);
            WriteLineL(_L("%S%S%S"), &KXMLResultDescrTag, &summary.iFullTestResult.iTestResult.iResultDes, &KXMLResultDescrTagEnd);
            WriteLineL(_L("%S%d%S"), &KXMLExecutionResultTag, summary.iFullTestResult.iCaseExecutionResultCode, &KXMLExecutionResultTagEnd);
            }
        else
            {
            WriteLineL( _L("\tStartTime: %S, EndTime: %S"),
                &startTime,
                &endTime );
            WriteLineL( printBuf2, code );
            WriteDelimiterL( _L( "- " ), 10 );
            WriteLineL( _L( "" ) );
            }
        }
        
    // Add test summary
    // Check if the module already exists for this test case
    TTestSummary* moduleSummary = NULL;
    TInt count = iTestSummaries.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if ( iTestSummaries[i]->iName == aTestInfo.iModuleName )
            {
            moduleSummary = iTestSummaries[i];
            break;
            }
        }
    
    if( moduleSummary == NULL )
        {
        // Create new module array
        moduleSummary = new ( ELeave ) TTestSummary( aTestInfo.iModuleName );
        User::LeaveIfError( iTestSummaries.Append( moduleSummary ) );
        }

    if( passed )
        {
        moduleSummary->iPassedCases++;
        iTotalSummary->iPassedCases++;
        if(iXML && (iReportMode & ETestReportCases))
            {
            WriteLineL(_L("%SPASSED%S"), &KXMLResultTag, &KXMLResultTagEnd);
            }
        }
    else if( timeout )
        {
        moduleSummary->iTimeoutCases++;
        iTotalSummary->iTimeoutCases++;
        if(iXML && (iReportMode & ETestReportCases))
            {
            WriteLineL(_L("%STIMEOUT%S"), &KXMLResultTag, &KXMLResultTagEnd);
            }
        }
    else if( crashed ||
           ( aError != KErrNone ) )
        {
        moduleSummary->iCrashedCases++;
        iTotalSummary->iCrashedCases++;
        if(iXML && (iReportMode & ETestReportCases))
            {
            WriteLineL(_L("%SCRASHED%S"), &KXMLResultTag, &KXMLResultTagEnd);
            }
        }
    else
        {
        moduleSummary->iFailedCases++;
        iTotalSummary->iFailedCases++;
        if(iXML && (iReportMode & ETestReportCases))
            {
            WriteLineL(_L("%SFAILED%S"), &KXMLResultTag, &KXMLResultTagEnd);
            }
        }

    if(iXML && (iReportMode & ETestReportCases))
        {
        WriteLineL(_L("%S"), &KXMLTestCaseTagEnd);
        }

    // Add temporarily closing tags to keep valid xml structure
    CloseXMLTagsInUnfinishedFileL();
    }


/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: WriteHeaderL

    Description: Write test report header.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestReport::WriteHeaderL()
    {
    if( ( iFormat == CStifLogger::EHtml ) && 
        ( iOutput == CStifLogger::EFile ) )
        {        
        // Html start tags to whole page and header section
        iFile.Write(
            _L8( "\n<html>\n<head>\n<title>TestReport</title>\n</head>\n\n\n<body>\n" ) );
        }
    
    if(iXML)
        {
        WriteLineL(_L("<?xml version=\"1.0\" ?>"));
        WriteLineL(_L("%S"), &KXMLTestReportTag);            
        }
    else
        {
        WriteDelimiterL( _L( "***" ), 25 );
        }

    // Generate date and time
    TTime time;
    time.HomeTime();
    TBuf<30> date;
    TBuf<30> clock;
    // Date
    _LIT( KDate, "%E%D%X%N%Y %1 %2 %3" );
    time.FormatL( date, KDate );
    // Time
    _LIT( KClock,"%-B%:0%J%:1%T%:2%S%:3%+B" );
    time.FormatL( clock,KClock );
    // Add date and time
    if(iXML)
        {
        WriteLineL(_L("%S%S%S"), &KXMLDateTag, &date, &KXMLDateTagEnd);
        WriteLineL(_L("%S%S%S"), &KXMLTimeTag, &clock, &KXMLTimeTagEnd);
        }
    else
        {
        WriteLineL( _L( "%S" ), &date );
        WriteLineL( _L( "%S" ), &clock );
        }

    // Add STIF version info
    TInt majorV;
    TInt minorV;
    TInt buildV;
    TBuf<30> relDate;
    TStifUtil::STIFVersion(majorV, minorV, buildV, relDate);
    if(iXML)
        {
        WriteLineL(_L("%S%d.%d.%d (%S)%S"), &KXMLSTIFVersionTag, majorV, minorV, buildV, &relDate, &KXMLSTIFVersionTagEnd);
        }
    else
        {
        WriteLineL(_L("v.%d.%d.%d (%S)"), majorV, minorV, buildV, &relDate);
        }
    
    if ( iReportMode & ETestReportSummary )
        {
        if( iOutput == CStifLogger::EFile )
            {
            // Get current file position
            iSummaryPos = 0;
            User::LeaveIfError( iFile.Seek( ESeekCurrent, iSummaryPos ) );
            }
        // Generate Summary to Report
        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLTestCasesSummaryTag);
            WriteLineL(_L("%S%S%S"), &KXMLPassedTag, &KXMLPassedTagEnd, &KSummaryLineEnd);
            WriteLineL(_L("%S%S%S"), &KXMLFailedTag, &KXMLFailedTagEnd, &KSummaryLineEnd);
            WriteLineL(_L("%S%S%S"), &KXMLTimeoutedTag, &KXMLTimeoutedTagEnd, &KSummaryLineEnd);
            WriteLineL(_L("%S%S%S"), &KXMLCrashedTag, &KXMLCrashedTagEnd, &KSummaryLineEnd);
            WriteLineL(_L("%S%S%S"), &KXMLTotalTag, &KXMLTotalTagEnd, &KSummaryLineEnd);
            WriteLineL(_L("%S"), &KXMLTestCasesSummaryTagEnd);
            }
        else
            {
            WriteDelimiterL( _L( "-- " ), 25 );
            WriteLineL(_L("SUMMARY:") );
            WriteLineL(_L("\tPassed cases: %S"), &KSummaryLineEnd );
            WriteLineL(_L("\tFailed cases: %S"), &KSummaryLineEnd );
            WriteLineL(_L("\tTimeout cases: %S"), &KSummaryLineEnd );
            WriteLineL(_L("\tCrashed cases: %S"), &KSummaryLineEnd );
            WriteLineL(_L("\tTotal cases: %S"), &KSummaryLineEnd );
            WriteLineL( _L( "" ));
            }   
        }

    if ( iReportMode & ETestReportEnvironment )
        {
        const TInt KMegaByte = 1024*1024;
        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLEnvironmentInfoTag);
            WriteLineL(_L("%S"), &KXMLHardwareInfoTag);
            WriteLineL(_L("%S0x%x%S"), &KXMLManufacturerTag, iReportHWInfo.iHwInfo.iManufacturer, &KXMLManufacturerTagEnd);
            WriteLineL(_L("%S0x%x%S"), &KXMLMachineUidTag, iReportHWInfo.iHwInfo.iMachineUid, &KXMLMachineUidTagEnd);
            WriteLineL(_L("%S0x%x%S"), &KXMLModelTag, iReportHWInfo.iHwInfo.iModel, &KXMLModelTagEnd);
            WriteLineL(_L("%S0x%x%S"), &KXMLHwRevisionTag, iReportHWInfo.iHwInfo.iHwRev, &KXMLHwRevisionTagEnd);
            WriteLineL(_L("%S0x%x%S"), &KXMLCpuTag, iReportHWInfo.iHwInfo.iCpu, &KXMLCpuTagEnd);
            WriteLineL(_L("%S%d%S"), &KXMLCpuSpeedTag, iReportHWInfo.iHwInfo.iCpuSpeed/1000, &KXMLCpuSpeedTagEnd);
            WriteLineL(_L("%S%d%S"), &KXMLLanguageTag, iReportHWInfo.iHwInfo.iLanguage, &KXMLLanguageTagEnd);
            WriteLineL(_L("%S"), &KXMLHardwareInfoTagEnd);
            WriteLineL(_L("%S"), &KXMLSoftwareInfoTag);
            WriteLineL(_L("%S0x%x%S"), &KXMLSwRevisionTag, iReportHWInfo.iSwInfo.iSwRev, &KXMLSwRevisionTagEnd);
            WriteLineL(_L("%S0x%x%S"), &KXMLSwBuildTag, iReportHWInfo.iSwInfo.iSwBuild, &KXMLSwBuildTagEnd);
            WriteLineL(_L("%S"), &KXMLSoftwareInfoTagEnd);
            WriteLineL(_L("%S"), &KXMLMemoryInfoTag);
            WriteLineL(_L("%S%d%S"), &KXMLRamTag, iReportHWInfo.iMemoryInfo.iRAM/KMegaByte, &KXMLRamTagEnd);
            WriteLineL(_L("%S%d%S"), &KXMLRamFreeTag, iReportHWInfo.iMemoryInfo.iRAMFree/KMegaByte, &KXMLRamFreeTagEnd);
            WriteLineL(_L("%S"), &KXMLMemoryInfoTagEnd);            
            WriteLineL(_L("%S"), &KXMLEnvironmentInfoTagEnd);            
            }
        else
            {
            // HW Info
            WriteDelimiterL( _L( "-- " ), 25 );
            WriteLineL( _L("ENVIRONMENT INFO:") );
            WriteLineL( _L("HW Info:") );
    
            WriteLineL(
                _L("\tManufacturer: 0x%x, MachineUid: 0x%x, Model: 0x%x "),
                    iReportHWInfo.iHwInfo.iManufacturer,
                    iReportHWInfo.iHwInfo.iMachineUid,
                    iReportHWInfo.iHwInfo.iModel );
            WriteLineL(
                _L("\tHW Rev: 0x%x, CPU: 0x%x, CPU Speed: %d MHz "),
                    iReportHWInfo.iHwInfo.iHwRev,
                    iReportHWInfo.iHwInfo.iCpu,
                    iReportHWInfo.iHwInfo.iCpuSpeed/1000 );
            WriteLineL(_L("\tLanguage: %d "),
                iReportHWInfo.iHwInfo.iLanguage );
    
            // SW Info
            WriteLineL(_L("SW Info:") );
            WriteLineL(_L("\tSW Rev: 0x%x, SW Build: 0x%x"),
                iReportHWInfo.iSwInfo.iSwRev,
                iReportHWInfo.iSwInfo.iSwBuild );
    
            // Memory Info
            WriteLineL(_L("Memory Info:") );
            WriteLineL(_L("\tRAM: %d MB, RAM Free: %d MB"),
                ( iReportHWInfo.iMemoryInfo.iRAM/KMegaByte ),
                ( iReportHWInfo.iMemoryInfo.iRAMFree/KMegaByte ) );
    
            WriteLineL( _L( "" ) );
            }
        }
        
    if ( iReportMode & ETestReportCases )
        {
        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLTestCasesTag);
            }
        else
            {
            // Generate Test Cases to Report
            WriteDelimiterL( _L( "-- " ), 25 );
            WriteLineL( _L("TESTCASE SUMMARY:") );
            }
        }

    if ( iReportMode == ETestReportBlank )
        {
        if(!iXML)
            {
            WriteLineL( _L( "") );
            WriteLineL( _L( "'Empty' configuration given in initialization file's [Engine_Defaults] section" ) );
            WriteLineL( _L( "No test report created" ) );
            }
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: WriteTrailerL

    Description: Write test report trailer.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestReport::WriteTrailerL()
    {
    if(iXML && (iReportMode & ETestReportCases))
        {
        WriteLineL(_L("%S"), &KXMLTestCasesTagEnd);
        }

    if ( iReportMode & ETestReportSummary )
        {
        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLModuleSummaryTag);            
            }
        else
            {
            WriteDelimiterL( _L( "-- " ), 25 );
            WriteLineL(_L("TESTMODULE SUMMARIES:") );
            }
        // Generate module related reports
        for ( TInt k = 0; k < iTestSummaries.Count(); k++ )
            {
            if(iXML)
                {
                WriteLineL(_L("%S"), &KXMLModuleTag);
                WriteLineL(_L("%S%S%S"), &KXMLModuleNameTag, &iTestSummaries[k]->iName, &KXMLModuleNameTagEnd);
                WriteLineL(_L("%S%d%S"), &KXMLPassedTag, iTestSummaries[k]->iPassedCases, &KXMLPassedTagEnd);
                WriteLineL(_L("%S%d%S"), &KXMLFailedTag, iTestSummaries[k]->iFailedCases, &KXMLFailedTagEnd);
                WriteLineL(_L("%S%d%S"), &KXMLTimeoutedTag, iTestSummaries[k]->iTimeoutCases, &KXMLTimeoutedTagEnd);
                WriteLineL(_L("%S%d%S"), &KXMLCrashedTag, iTestSummaries[k]->iCrashedCases, &KXMLCrashedTagEnd);
                WriteLineL(_L("%S%d%S"), &KXMLTotalTag, iTestSummaries[k]->iPassedCases + iTestSummaries[k]->iFailedCases + iTestSummaries[k]->iTimeoutCases + iTestSummaries[k]->iCrashedCases, &KXMLTotalTagEnd);
                WriteLineL(_L("%S"), &KXMLModuleTagEnd);
                }
            else
                {
                WriteLineL(_L("Module: [%S]"),
                    &iTestSummaries[k]->iName );
                WriteLineL(_L("\tPassed cases: %d"),
                    iTestSummaries[k]->iPassedCases );
                WriteLineL(_L("\tFailed cases: %d"),
                    iTestSummaries[k]->iFailedCases );
                WriteLineL(_L("\tTimeout cases: %d"),
                    iTestSummaries[k]->iTimeoutCases );
                WriteLineL(_L("\tCrashed cases: %d"),
                    iTestSummaries[k]->iCrashedCases );
                TInt moduleTotal = iTestSummaries[k]->iPassedCases +
                    iTestSummaries[k]->iFailedCases +
                    iTestSummaries[k]->iTimeoutCases +
                    iTestSummaries[k]->iCrashedCases;
                WriteLineL(_L("\tTotal cases: %d"), moduleTotal );
                WriteLineL( _L( "" ) );
                }
            }

        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLModuleSummaryTagEnd);            
            }

        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLDllSummaryTag);            
            }
        else
            {
            WriteDelimiterL(_L( "-- " ), 25 );
            WriteLineL(_L("TEST MODULES VERSIONS:"));
            }

        // Generate test module versions info
        for( TInt i = 0; i < iTestModulesVersionsInfo.Count(); i++ )
            {
            TTestModuleVersionInfo* versionInfo = (TTestModuleVersionInfo*)iTestModulesVersionsInfo[i];
            if(iXML)
                {
                WriteLineL(_L("%S"), &KXMLDllTag);
                WriteLineL(_L("%S%S%S"), &KXMLFileNameTag, &(versionInfo->iTestModuleName), &KXMLFileNameTagEnd);
                WriteLineL(_L("%S%d.%d.%d%S"), &KXMLVersionTag, versionInfo->iMajor, versionInfo->iMinor, versionInfo->iBuild, &KXMLVersionTagEnd);
                WriteLineL(_L("%S"), &KXMLDllTagEnd);
                }
            else
                {
                WriteLineL(_L("%S %d.%d.%d"), &(versionInfo->iTestModuleName), versionInfo->iMajor, versionInfo->iMinor, versionInfo->iBuild);
                }
            }

        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLDllSummaryTagEnd);            
            }
        }
    
    if(!iXML)
        {
        WriteLineL( _L( "" ) );
        }
    
    if( ( iFormat == CStifLogger::EHtml ) && 
        ( iOutput == CStifLogger::EFile ) )
        {        
        // Html start tags to whole page and header section
        iFile.Write(
            _L8( "\n\n\n</html>\n</body>\n\n\n" ) );
        }

    if(iXML)
        {
        WriteLineL(_L("%S"), &KXMLTestReportTagEnd);            
        }
    }
            
/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: UpdateReportSummaryL

    Description: Updates the test report summary.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Draft

-------------------------------------------------------------------------------
*/
void CTestReport::UpdateReportSummaryL()
    {
    TInt currentPos = 0;
    // Add numeric information to summary
    if ( iReportMode & ETestReportSummary )
        {
        // Go to the summary start position of the file
        if( iOutput == CStifLogger::EFile )
            {
            User::LeaveIfError(iFile.Seek(ESeekCurrent, currentPos));//Get current position
            User::LeaveIfError( iFile.Seek( ESeekStart, iSummaryPos ) );
            }
        TBuf<KSummaryLineEndLen> numStr;        
        // Generate Summary to Report
        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLTestCasesSummaryTag);
            }
        else
            {
            WriteDelimiterL( _L( "-- " ), 25 );
            WriteLineL(_L("SUMMARY:") );
            }
        numStr.Num( iTotalSummary->iPassedCases );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLPassedTag, &numStr, &KXMLPassedTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tPassed cases: %S"), &numStr );
            }
        
        numStr.Num( iTotalSummary->iFailedCases );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLFailedTag, &numStr, &KXMLFailedTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tFailed cases: %S"), &numStr );
            }
            
        numStr.Num( iTotalSummary->iTimeoutCases );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLTimeoutedTag, &numStr, &KXMLTimeoutedTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tTimeout cases: %S"), &numStr );
            }
        
        numStr.Num( iTotalSummary->iCrashedCases );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLCrashedTag, &numStr, &KXMLCrashedTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tCrashed cases: %S"), &numStr );
            }
        
        TInt total = iTotalSummary->iPassedCases +
            iTotalSummary->iFailedCases +
            iTotalSummary->iTimeoutCases +
            iTotalSummary->iCrashedCases;
        numStr.Num( total );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLTotalTag, &numStr, &KXMLTotalTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tTotal cases: %S"), &numStr );
            }

        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLTestCasesSummaryTagEnd);
            }

        if(iOutput == CStifLogger::EFile)
            {
            User::LeaveIfError(iFile.Seek(ESeekStart, currentPos));
            }
        }
    }
            
/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: GenerateReportL

    Description: Generate the test report.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestReport::GenerateReportL()
    {
    WriteTrailerL();
    
    // Add numeric information to summary
    if ( iReportMode & ETestReportSummary )
        {
        // Go to the summary start position of the file
        if( iOutput == CStifLogger::EFile )
            {
            User::LeaveIfError( iFile.Seek( ESeekStart, iSummaryPos ) );
            }
        TBuf<KSummaryLineEndLen> numStr;
        // Generate Summary to Report
        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLTestCasesSummaryTag);
            }
        else
            {
            WriteDelimiterL( _L( "-- " ), 25 );
            WriteLineL(_L("SUMMARY:") );
            }

        numStr.Num( iTotalSummary->iPassedCases );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLPassedTag, &numStr, &KXMLPassedTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tPassed cases: %S"), &numStr );
            }

        numStr.Num( iTotalSummary->iFailedCases );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLFailedTag, &numStr, &KXMLFailedTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tFailed cases: %S"), &numStr );
            }
        
        numStr.Num( iTotalSummary->iTimeoutCases );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLTimeoutedTag, &numStr, &KXMLTimeoutedTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tTimeout cases: %S"), &numStr );
            }
        
        numStr.Num( iTotalSummary->iCrashedCases );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLCrashedTag, &numStr, &KXMLCrashedTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tCrashed cases: %S"), &numStr );
            }
        
        TInt total = iTotalSummary->iPassedCases +
            iTotalSummary->iFailedCases +
            iTotalSummary->iTimeoutCases +
            iTotalSummary->iCrashedCases;
        numStr.Num( total );
        if(iXML)
            {
            TPtrC spaces(KSummaryLineEnd().Ptr(), KSummaryLineEnd().Length() - numStr.Length());
            WriteLineL(_L("%S%S%S%S"), &KXMLTotalTag, &numStr, &KXMLTotalTagEnd, &spaces);
            }
        else
            {
            numStr.Append( KSummaryLineEnd().Ptr(), 
                           KSummaryLineEnd().Length() - numStr.Length() );
            WriteLineL(_L("\tTotal cases: %S"), &numStr );
            }

        if(iXML)
            {
            WriteLineL(_L("%S"), &KXMLTestCasesSummaryTagEnd);
            }
        }
    }

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: WriteLine

    Description: Write line to file.

    Parameters: TRefByValue<const TDesC> aStr: in: test to print

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestReport::WriteLineL( TRefByValue<const TDesC> aStr,... )
    {
    
    VA_LIST list;
    VA_START( list, aStr );
    TPrintInfo line;

    // Parse parameters
    line.AppendFormatList( aStr, list, NULL );
    
    if( iOutput == CStifLogger::ERDebug )
        {
        // RDebug output
        RDebug::Print( line );
        return;
        }
    
    HBufC8* buf = HBufC8::NewLC( line.Length() + KMaxLenEol );
    TPtr8 ptr( buf->Des() );
    ptr.Copy( line );    
            
    if( ( iFormat == CStifLogger::EHtml ) && 
        ( iOutput == CStifLogger::EFile ) )
        {
        // Html output, add linefeed
        ptr.Append( _L8("<BR>\r\n") );
        }
    else 
        {
        // Default: Text format to file
        // Add linefeed
        ptr.Append( _L8("\r\n") );
        }
    
    // Write to file    
    iFile.Write( ptr );
    CleanupStack::PopAndDestroy( buf );
    
    }
        
/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: WriteDelimiter

    Description: Write delimiter line to file.

    Parameters: const TDesC& aDelimiter: in: delimiter mark
                TInt aCount: in: number of delimiters written

    Return Values: None

    Errors/Exceptions: None

    Status: Proposal

-------------------------------------------------------------------------------
*/
void CTestReport::WriteDelimiterL( const TDesC& aDelimiter, TInt aCount )
    {
    
     if( iOutput == CStifLogger::ERDebug )
        {
        // RDebug output
        // Nothing is printed
        return;
        }
    
    HBufC8* buf = HBufC8::NewLC( aDelimiter.Length() );
    TPtr8 ptr( buf->Des() );
    ptr.Copy( aDelimiter );    
    
    for( TInt i = 0; i<aCount; i++ )
        {
        // Write to file    
        iFile.Write( ptr );
        }
    CleanupStack::PopAndDestroy( buf );
    
    TBuf8<KMaxLenEol> linefeed;        
    if( ( iFormat == CStifLogger::EHtml ) && 
        ( iOutput == CStifLogger::EFile ) )
        {
        // Html output, add linefeed
        linefeed.Append( _L8("<BR>\r\n") );
        }
    else 
        {
        // Default: Text format to file
        // Add linefeed
        linefeed.Append( _L8("\r\n") );
        }
     // Write to file    
    iFile.Write( linefeed );
    
    }        
        
/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: TTestReportHeader::TTestReportHeader

    Description: Constructor of TTestReportHeader

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestReport::TTestHWInfo::TTestHWInfo()
    {
    // Initialize, fills a specified block of data with binary zeroes
    Mem::FillZ( this, sizeof( CTestReport::TTestHWInfo ) );

    // Get HW Info
    HAL::Get( HALData::EManufacturer, iHwInfo.iManufacturer );
    HAL::Get( HALData::EMachineUid, iHwInfo.iMachineUid );
    HAL::Get( HALData::EManufacturerHardwareRev, iHwInfo.iHwRev );
    HAL::Get( HALData::EModel, iHwInfo.iModel );
    HAL::Get( HALData::ECPU, iHwInfo.iCpu );
    HAL::Get( HALData::ECPUSpeed, iHwInfo.iCpuSpeed );
    HAL::Get( HALData::ELanguageIndex, iHwInfo.iLanguage );

    // Get SW Info
    HAL::Get( HALData::EManufacturerSoftwareRev, iSwInfo.iSwRev );
    HAL::Get( HALData::EManufacturerSoftwareBuild, iSwInfo.iSwBuild );

    // Memory Info
    HAL::Get( HALData::EMemoryRAM, iMemoryInfo.iRAM );
    HAL::Get( HALData::EMemoryRAMFree, iMemoryInfo.iRAMFree );

    }
        
/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: TTestSummary::TTestSummary

    Description: Constructor of TTestSummary.

    Parameters: const TName& aName: in: Name of summary

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestReport::TTestSummary::TTestSummary( const TName& aName ) :
    iName( aName ),
    iPassedCases( 0 ),
    iFailedCases( 0 ),
    iCrashedCases( 0 ),
    iTimeoutCases( 0 )
    {

    }

/*
-------------------------------------------------------------------------------

	Class: CTestReport

    Method: AddTestModuleVersion
    
    Description: Method adds version of a test module to the list of versions
    			 that will be printed when stif is stopped.
    			 
    Parameters: TTestModuleVersionInfo aVersion : object contains information about version and
    			name of test module.

    Return Values: None

    Errors/Exceptions: None

-------------------------------------------------------------------------------
*/

void CTestReport::AddTestModuleVersion(TTestModuleVersionInfo& aVersion)
	{
	for(TInt i = 0; i < iTestModulesVersionsInfo.Count(); i++)
		{
		if( aVersion.iTestModuleName == iTestModulesVersionsInfo[i]->iTestModuleName )
			{
			return;
			}
		}
	
	TTestModuleVersionInfo* testModuleVersionInfo = new (ELeave) TTestModuleVersionInfo;
	testModuleVersionInfo->iMajor = aVersion.iMajor;
	testModuleVersionInfo->iMinor = aVersion.iMinor;
	testModuleVersionInfo->iBuild = aVersion.iBuild;
	testModuleVersionInfo->iTestModuleName = aVersion.iTestModuleName;
	
	TInt res = iTestModulesVersionsInfo.Append(testModuleVersionInfo);
	if( res != KErrNone )
		{
		delete testModuleVersionInfo;
		}
	}

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: TTestCaseSummary::TTestCaseSummary

    Description: Constructor of TTestCaseSummary.

    Parameters: None

    Return Values: None

    Errors/Exceptions: None

    Status: Approved

-------------------------------------------------------------------------------
*/
CTestReport::TTestCaseSummary::TTestCaseSummary()
    {

    }

/*
-------------------------------------------------------------------------------

    Class: CTestReport

    Method: CloseXMLTagsInUnfinishedFileL

    Description: Adds needed tags to get valid xml file.

    Parameters: None

    Return Values: None

    Errors/Exceptions: Seek operations may cause leaves

    Status: Approved

-------------------------------------------------------------------------------
*/
void CTestReport::CloseXMLTagsInUnfinishedFileL(void)
    {
    // For xml file keep current position in file.
    if(iXML)
        {
        TInt currentPos = 0;
        User::LeaveIfError(iFile.Seek(ESeekCurrent, currentPos));
        
        // Add closing tags to get valid xml file
        WriteLineL(_L("%S"), &KXMLTestCasesTagEnd);
        WriteLineL(_L("%S"), &KXMLTestReportTagEnd);
        
        // Move back to previous position
        User::LeaveIfError(iFile.Seek(ESeekStart, currentPos));
        }
    }

// ================= OTHER EXPORTED FUNCTIONS =================================

// None

// End of File
