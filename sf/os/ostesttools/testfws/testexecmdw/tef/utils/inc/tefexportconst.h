/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



/**
 @file TEFExportConst.h
 @publishedPartner
 @test 
*/

#ifndef __TEF_EXPORT_CONST_H__
#define __TEF_EXPORT_CONST_H__
#include <e32def.h>

// Logger Macros
#define INFO_PRINTF1(p1)							Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1))
#define INFO_PRINTF2(p1, p2)						Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2))
#define INFO_PRINTF3(p1, p2, p3)					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3))
#define INFO_PRINTF4(p1, p2, p3, p4)				Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4))
#define INFO_PRINTF5(p1, p2, p3, p4, p5)			Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5))
#define INFO_PRINTF6(p1, p2, p3, p4, p5, p6)		Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6))
#define INFO_PRINTF7(p1, p2, p3, p4, p5, p6, p7)	Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define WARN_PRINTF1(p1)							Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1)) 
#define WARN_PRINTF2(p1, p2)						Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2)) 
#define WARN_PRINTF3(p1, p2, p3)					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3)) 
#define WARN_PRINTF4(p1, p2, p3, p4)				Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4)) 
#define WARN_PRINTF5(p1, p2, p3, p4, p5)			Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5)) 
#define WARN_PRINTF6(p1, p2, p3, p4, p5, p6)		Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define WARN_PRINTF7(p1, p2, p3, p4, p5, p6, p7)	Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6), (p7)) 

#define ERR_PRINTF1(p1)								Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1)) 
#define ERR_PRINTF2(p1, p2)							Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2)) 
#define ERR_PRINTF3(p1, p2, p3)						Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3)) ;
#define ERR_PRINTF4(p1, p2, p3, p4)					Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4)) 
#define ERR_PRINTF5(p1, p2, p3, p4, p5)				Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5)) 
#define ERR_PRINTF6(p1, p2, p3, p4, p5, p6)			Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6)) 
#define ERR_PRINTF7(p1, p2, p3, p4, p5, p6, p7)		Logger().LogExtra(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

// Literals
// From TestExecuteLog.h
_LIT(KTestExecuteLogServerName,"TestExecuteLogEngine");
_LIT(KTestExecuteLogPath,"?:\\Logs\\TestExecute\\");
_LIT(KTestExecuteScheduleTestLogCompatibilityNameFile,"LogFileName.Txt");
_LIT(KTestExecuteName,"TestExecute");
_LIT8(KTestExecuteName8,"TestExecute");

_LIT(KTestExecutePIPSLogServerName,"testexecutepipslogengine");

// From TestExecuteClient.h
_LIT(KTestExecuteCommandLineFlagDebugMode,"-d");
_LIT(KTestExecuteCommandLineFlagSeparateLogFileMode,"-slf");
_LIT(KTestExecuteCommandLineFlagGraphicalWindowServer,"-gws");
_LIT(KTestExecuteCommandLineFlagIncludeSelectiveCases,"-tci");
_LIT(KTestExecuteCommandLineFlagExcludeSelectiveCases,"-tcx");
_LIT(KTestExecuteCommandLineFlagPipe,"-pipe");

// From TestExecuteServerUtils.h
_LIT(KNull,"");
_LIT(KPanicEquals,"P=");
_LIT(KErrorEquals,"E=");

// From TEFUnit.h
_LIT( KAssertFailedEquals, "AssertEquals Failed");
_LIT( KAssertFailedNull, "AssertNull Failed");
_LIT( KAssertFailedNotNull, "AssertNotNull Failed");
_LIT( KAssertFailedSame, "AssertSame Failed");
_LIT( KAssertFailedNotSame, "AssertNotSame Failed");
_LIT( KAssertFailedTrue, "AssertTrue Failed" );
_LIT( KAssertFailedFalse, "AssertFalse Failed");

// Constants
// From TestExecuteLog.h
const TInt KMaxTestExecuteLogFilePath = 256;
const TInt KMaxTestExecuteLogLineLength = 512;
const TInt KTestExecuteLoggerMajorVersion = 1;
const TInt KTestExecuteLoggerMinorVersion = 1;
const TInt KTestExecuteLoggerBuildVersion = 1;

// From TestExecuteStepBase.h
// this error number for test server leave a err num which conflict with system wide error.
//	example: when doteststepL {uesr::leave(KErrServerBusy);}
//			 testexecute will take this as test server is busy;
//			 in fact, this is not because doteststepL() has runned, not busy at all.
//			 we have to make the return value + offset(30000)
const TInt KErrTEFBaseErrorZero = -30000; 
const TInt KErrTestExecuteInUse = KErrTEFBaseErrorZero + KErrInUse;
const TInt KErrTestExecuteServerBusy = KErrTEFBaseErrorZero + KErrServerBusy; 

// From TestExecuteClient.h
const TInt KTestExecuteMajorVersionNumber=1;
const TInt KTestExecuteMinorVersionNumber=1;
const TInt KTestExecuteBuildVersionNumber=1;
const TInt KMaxTestExecuteNameLength = 80;
const TInt KMaxTestExecuteCommandLength = 256;
const TInt KMaxTestStepNameLength = 256;

// From TestExecuteServerUtils.h
const TInt KErrorStepResult = 0;

// From TEFUnit.h
const int KErrTEFUnitPass = 0;
const int KErrTEFUnitFail = 106;
const int KErrTEFUnitInconclusive = 107;
const int KErrTEFUnitTestSuiteError = 108;
const int KErrTEFUnitAbort = 109;
const int KErrTEFUnitIgnore = 110;

// Enumerations
enum TModeCopy
	{
	ESetText  = 1,
	EAppendText = 2
	};

enum TVerdict
	{
	EPass =0,			
	EFail = 106,
	EInconclusive,
	ETestSuiteError,
	EAbort,
	EIgnore,
	ESkippedSelectively
	};

enum TLoggerOptions
	{
	ELogHTMLOnly,
	ELogXMLOnly,
	ELogBoth
	};

enum TLoggerChannels
	{
	EFile,
	ESerial,
	EBoth
	};

enum TTestSetupState
	{
	ESetupNone = 0,
	ESetupOomServer = 1,
	ESetupLast = 100
	};

// Logging level
enum TLogSeverity{ESevrErr  = 1,ESevrHigh, ESevrWarn,ESevrMedium, ESevrInfo, ESevrLow, ESevrAll};

// export as well as non-exported files access
const TInt KTEFAsciiExclamation=33;
const TInt KTEFMinPanicStringParamLength=13;
const TInt KTEFMinPanicCodeParamLength=11;
const TInt KTEFMinErrorParamLength=7;
const TInt KTEFMinSetupParamLength=7;
const TInt KTEFMinResultParamLength=8;
const TInt KTEFMinHeapParamLength=6;
const TInt KTEFZeroValue=0;
const TInt KTEFOneValue=1;
const TInt KTEFMaxVerdictLength = 128;
const TInt KTEFCommandCountsStructSize = 9;
const TInt KTEFCommandsCountPlusOne = KTEFCommandCountsStructSize + 1;
const TInt KTEFMaxPanicStringLength = KMaxExitCategoryName; // 16 is Max length for User::Panic()

// OS version number
const TInt KOsVersionCount = 6;

_LIT(KTEFTestExecuteResultSummaryFile,"TestResults.htm");
_LIT(KTEFResultString,"RESULT");
_LIT(KTEFRunTestStepCommand,"RUN_TEST_STEP");
_LIT(KTEFRunErrorStepResultCommand,"RUN_ERROR_STEP_RESULT");
_LIT(KTEFRunPanicStepCommand,"RUN_PANIC_STEP");
_LIT(KTEFRunTestStepResultCommand,"RUN_TEST_STEP_RESULT");
_LIT(KTEFRunPanicStepResultCommand,"RUN_PANIC_STEP_RESULT");
_LIT(KTEFRunProgramCommand,"RUN_PROGRAM");
_LIT(KTEFRunWSProgramCommand,"RUN_WS_PROGRAM");
_LIT(KTEFStartTestCaseCommand, "START_TESTCASE");
_LIT(KTEFEndTestCaseCommand, "END_TESTCASE");
_LIT(KTEFStartSyncTestCaseCommand, "START_SYNCHRONISED_TESTCASE");
_LIT(KTEFEndSyncTestCaseCommand, "END_SYNCHRONISED_TESTCASE");
_LIT(KTEFPanicString,"!PanicString=");
_LIT(KTEFPanicCode,"!PanicCode=");
_LIT(KTEFError,"!Error=");
_LIT(KTEFHeap,"!Heap=");
_LIT(KTEFSpace, " ");
_LIT(KTEFNull, "");
_LIT(KTEFEquals, "=");
_LIT(KTEFScriptExtension,".script");
_LIT(KTEFCfgExtension,".tcs");
_LIT(KTEFHtmlKey, "HTML");
_LIT(KTEFDefaultScriptPath,"DefaultScriptDir");
_LIT(KTEFJustInTimeDebug,"JustInTimeDebug");
_LIT(KTEFSystemStarter, "SystemStarter");
_LIT(KTEFRemotePanicDetection, "RemotePanicDetection");
_LIT(KTEFEnableIniAccessLog, "EnableIniAccessLog");
_LIT(KTEFEnableTestsCountLog, "EnableTestsCountLog");
_LIT(KTEFLogSeverityKey, "LogLevel");
_LIT(KTEFLogMode, "LogMode");
_LIT(KTEFStringFormat,"%S");
_LIT(KTEFSlash, "\\");
_LIT(KTEFLogChannel, "LogChannel");
_LIT(KTEFOutputPort, "output_port");
_LIT(KTEFWaitForLoggingTime, "WaitForLoggingTime");

_LIT(KTEFResultPass,"PASS");
_LIT(KTEFResultSkippedSelectively,"SKIPPED_SELECTIVELY");
_LIT(KTEFResultFail,"FAIL");
_LIT(KTEFResultTag,"***Result");
_LIT(KTEFResult,"!Result=");

// export only
const TInt KTEFTestExecuteParamLength=30;
const TInt KTEFMinPanicStringLength=2;

_LIT(KTEFResultPanic,"PANIC");
_LIT(KTEFResultUnexecuted, "UNEXECUTED");
_LIT(KTEFTestCaseResultTag,"***TestCaseResult");
_LIT(KTEFCommentedCommands,"COMMENTED OUT COMMAND'S");
_LIT(KTEFTestCaseSummary, "TEST_CASE_SUMMARY");
_LIT(KTEFTestStepSummary, "TEST_STEP_BLOCK_SUMMARY");
_LIT(KTEFRunProgramSummary, "RUN_PROGRAM_SUMMARY");
_LIT(KTEFRunScriptSummary, "RUN_SCRIPT_SUMMARY:");
_LIT(KTEFScriptExecuted, "EXECUTED_SCRIPTS");
_LIT(KTEFFailedOpen, "FAILED_TO_OPEN");
_LIT(KTEFOpenQuotes,"\"");
_LIT(KTEFSpaceEquals," = ");
_LIT(KTEFSection, "TestExecute");
_LIT(KTEFXmlKey, "XML");
_LIT(KTEFEqualsInteger," = %d");
_LIT(KTEFIniReadInteger,"INI READ : %S %S %S %d");
_LIT(KTEFIniReadString,"INI READ : %S %S %S %S");
_LIT(KTEFIniWriteInteger,"INI WRITE : %S %S %S %d");
_LIT(KTEFIniWriteString,"INI WRITE : %S %S %S %S");
_LIT(KTEFDefaultPath,"?:\\system\\data\\testexecute.ini");
_LIT(KTEFAlternatePath,"z:\\system\\data\\testexecute.ini");
_LIT(KTEFComma,",");

_LIT(KTEFResultInconclusive,"INCONCLUSIVE");
_LIT(KTEFResultAbort,"ABORT");
_LIT(KTEFResultUnknown,"UNKNOWN");
_LIT(KTEFFontLightBlue, "<font size=2 color=00AFFF>");
_LIT(KTEFFontGreen,"<font size=2 color=00AF00>");
_LIT(KTEFFontRed,"<font size=2 color=FF0000>");
_LIT(KTEFFontBlue,"<font size=2 color=0000FF>");
_LIT(KTEFFontGrey,"<font size=2 color=C0C0C0>");
_LIT(KTEFFontEnd,"</font>");
_LIT(KTEFTestCaseDefault, "NOTESTCASE");

_LIT(KTEFDefaultSysDrive, "DefaultSysDrive");
_LIT(KTEFSysDrive, "${SYSDRIVE}");
_LIT(KTEFIniSysDrive, "SYSDRIVE");
_LIT(KTEFLegacySysDrive, "C:");
_LIT(KTEFWrapperPluginDll, "WrapperUtilsPlugin.dll");
_LIT(KTEFTabulator, "\t");
// the simplified summary file. in case of out-of-memory.
_LIT(KTEFTestExecuteResultSimplifiedSummaryFile,"TestResults.txt");
_LIT8(KLoggerNotEnoughMemory8, "Not enough memory to perform logging...");
_LIT(KTEFIniReadInt64,"INI READ : %S %S %S %ld");
_LIT(KTEFIniWriteInt64,"INI WRITE : %S %S %S %ld");
_LIT(KTEFPanicStringTooLong,"Expected panic string %S\"%S\" (in script) was TOO LONG.");
_LIT(KTEFPanicStringErrorInfo,"Keep the panic string length to 16 characters or less. Please Refer to User::Panic() in SDL for more information.");
_LIT(KTEFExpectedPanicString,"Expected panic string was \"%S\" (in script)");
_LIT(KTEFReceivedPanicString,"Received panic string was \"%S\"(in code)");
_LIT(KTEFWarnPanicString,"The actual length of the received panic string is 16 characters, which is possibly truncated. User::Panic will only pass through the first 16 characters of the panic string, the test result may not be reliable. Ensure this is 16 characters or less or it will be truncated.");
#endif
