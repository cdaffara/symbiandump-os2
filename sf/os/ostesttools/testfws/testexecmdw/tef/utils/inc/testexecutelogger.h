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
 @file TestExecuteLogger.h
*/

#if !(defined __TESTEXECUTE_LOGGER_H__)
#define __TESTEXECUTE_LOGGER_H__

#include <e32base.h>
#include <e32cons.h>
#include <f32file.h>
#include <test/testexecutelogbase.h>
#include <iniparser.h>
#include <test/blockitems.h>
#include <test/tefscriptutils.h>

class CSerialWriter;
/**
 * @internalComponent
 * @test
 */
class CTestExecuteIniData : public CBase
	{
public:
	IMPORT_C CTestExecuteIniData();
	IMPORT_C ~CTestExecuteIniData();
	IMPORT_C static CTestExecuteIniData* NewL();
	IMPORT_C static CTestExecuteIniData* NewLC();
	IMPORT_C void ConstructL();
	IMPORT_C void ExtractValuesFromIni();
	IMPORT_C void GetKeyValueFromIni(const TDesC& aKeyName, TDes& aValue);
	IMPORT_C void GetKeyValueFromIni(const TDesC& aKeyName, TInt& aValue);
	IMPORT_C CTestExecuteIniData(TDriveName& aSysDrive);
	IMPORT_C static CTestExecuteIniData* NewL(TDriveName& aSysDrive);
	IMPORT_C static CTestExecuteIniData* NewLC(TDriveName& aSysDrive);
private:
	CIniData* iConfigData;
	TPtrC iHtmlLogPath;
	TPtrC iXmlLogPath;
	TPtrC iDefaultScriptDirectory;
	TInt iLogSeverity;
	TInt iLoggerOptions;
	TInt iJustInTime;
	TInt iRemotePanicDetection;
	TInt iEnableIniAccessLog;
	TInt iEnableTestsCountLog;
	TInt iEnableSystemStarter;
	TInt iLoggerChannel;
	TInt iPortNumber;
	TDriveName iDefaultSysDrive;
	TPtrC iIniSysDriveName;
	TInt iWaitForLoggingTime;
	};

/**
 * Override of base class virtual for TEF logger implementation
 * @internalComponent
 * @test
 */
class CTestExecuteLogger : public MTestExecuteLogBase
	{
public:
	IMPORT_C CTestExecuteLogger(TLoggerOptions aLogOptions=ELogHTMLOnly);
	IMPORT_C ~CTestExecuteLogger();
	IMPORT_C void LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,TRefByValue<const TDesC> aFmt,...);
	IMPORT_C void LogToXml(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aCommandName, const TInt aNumberOfParams, TExtraLogField* aLogFields);
	IMPORT_C void LogToXml(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aCommandName);
	IMPORT_C void InitialiseLoggingL(const TDesC& aScriptFilePath, TBool aSeparateLogFileMode, TInt aLogLevel);
	
	IMPORT_C void TerminateLoggingL(const TInt aCommentedCommandsCount, const TInt aRemotePanicsCount, const TInt aRunScriptFailCount);
	IMPORT_C void LogTestCaseResult(const TDesC& aFile, TInt aLine, TInt aSeverity, const TDesC& aCommand, const TDesC& aTestCaseName, TVerdict aResult = EPass);
	IMPORT_C void PrintCurrentScriptLine(TDes& aCurrentScriptLine);
	IMPORT_C void LogResult(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,const TDesC& aScriptFile,TInt aSeverity);
	IMPORT_C TVerdict LogBlock( TTEFItemArray* aItemArray, const TInt aScriptLineNumber );
	IMPORT_C void SetLoggerOptions(TInt aLogMode);
	IMPORT_C TInt Connect();
	IMPORT_C void Close();
	inline RTestExecuteLogServ& HtmlLogger() { return iHtmlLogger; };
	inline RTestExecutePIPSLogServ& PIPSLogger() { return iPIPSLogger; };
	inline RFileFlogger& XmlLogger() { return iXmlLogger; };
	IMPORT_C TInt ShareAuto();

	// Legacy methods provided for backwards compatibility
	// These only log to the html log file
	IMPORT_C void Write(const TDesC& aLogBuffer);
	IMPORT_C void WriteFormat(TRefByValue<const TDesC> aFmt,...);
	IMPORT_C void Write(const TDesC8& aLogBuffer);
	IMPORT_C void WriteFormat(TRefByValue<const TDesC8> aFmt,...);
	IMPORT_C static CTestExecuteLogger* NewL(TLoggerOptions aLogOptions=ELogHTMLOnly);
	IMPORT_C static CTestExecuteLogger* NewLC(TLoggerOptions aLogOptions=ELogHTMLOnly);
	IMPORT_C void SetLoggerChannel(TInt aLogChannel);
	IMPORT_C void SetPipeName(TPtrC& aPipeName) ;
	void OpenSerialPortL(const TInt aPortNumber);
protected:
	// parse the script string(!PanicString)
	TBool ParsePaincStringFromScriptCommandLine(const TDesC& aCommandStr,TLex& aScriptLineParamLex,TDes& aTEFPanicString);
private:
	// Connects to the HTML log client session and creating the logs with initial tags for logging
	void InitialiseHTMLLoggingL(const TDesC& aLogFilePath, TInt aLogLevel, const TInt& aEnableCommandCount, CScriptPreProcess& aScriptPreProcess);
	void InitialisePIPSLoggingL(TInt aLogLevel, const TInt& aEnableCommandCount, CScriptPreProcess& aScriptPreProcess);
	// Connects to the XML log client session and creates the logs in the path passed in
	// Also sets in the default threshold for log level
	void InitialiseXMLLoggingL(const TDesC& aLogFilePath, TInt aLogLevel, const TInt& aEnableCommandCount, CScriptPreProcess& aScriptPreProcess);
	void TerminateHTMLLoggingL(RFile* aLogFile, const TInt aCommentedCommandsCount, const TInt aRemotePanicsCount, const TInt aRunScriptFailCount);
	void TerminateXMLLoggingL(const TInt aCommentedCommandsCount, const TInt aRemotePanicsCount, const TInt aRunScriptFailCount);
	TVerdict LogHTMLBlockL( const TTEFItemArray& aItemArray );
	TVerdict LogXMLBlock( const TTEFItemArray& aItemArray, const TInt aScriptLineNumber );
	void InitialiseSerialLoggingL(TInt aLogLevel, const TInt& aEnableCommandCount, CScriptPreProcess& aScriptPreProcess, TInt portNumber);	
	void LogTestCaseResultSerialL(const TDesC& aFile, TInt aLine, TInt aSeverity, const TDesC& aCommand, const TDesC& aTestCaseName, TVerdict aResult = EPass);
	void LogResultSerialL(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,const TDesC& aScriptFile);
	void LogResultFileL(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,const TDesC& aScriptFile, TInt aSeverity);
	void LoggingTestCaseResultToSummaryResultL( const TDesC &aLogFileDir,const TDesC &aLogFileName,const TDesC8 &aResultBuf);
	
private:
	// HTML Log client session
	RTestExecuteLogServ iHtmlLogger;
	RTestExecutePIPSLogServ iPIPSLogger;
	// XML Log client session
	RFileFlogger iXmlLogger;
	TInt iLoggerOptions;
	// Member variables to perform TerminateLogging
	// Set up some vars for the test step result types
	TInt iPassCount;
	TInt iFailCount;
	TInt iAbortCount;
	TInt iUnknownCount;
	TInt iInconclusiveCount;
	TInt iPanicCount;
	TInt iUnexecutedCount;

	// Set up some vars for the RUN_PROGRAM result types
	TBool iRunProgramUsed;
	TInt iRunProgramPassCount;
	TInt iRunProgramFailCount;
	TInt iRunProgramAbortCount;
	TInt iRunProgramUnknownCount;
	TInt iRunProgramInconclusiveCount;
	TInt iRunProgramPanicCount;
	TInt iRunProgramUnexecutedCount;

	// Set up some vars for the testcase result types
	TBool iTestCasesUsed;
	TInt iTestCasePassCount;
	TInt iTestCaseFailCount;
	TInt iTestCaseInconclusiveCount;
	
	// Log file paths for HTML & XML
	TBuf<KMaxTestExecuteNameLength> iHtmlLogPath;
	TBuf<KMaxTestExecuteNameLength> iXmlLogPath;
	TInt iRemotePanic; // Flag to retrieve status of Remote Panic Detection from testexecute.ini
	TInt iRunScriptCommandCount; // Count of number of RUN_SCRIPT commands in the main script executed
	CSerialWriter *iSerialWriter ; 
	TInt iPortNumber ; 
	TInt iLoggerChannel;
	TInt iTestCaseSkippedCount;
	TBool iPIPSExists ; 
	};

#endif
