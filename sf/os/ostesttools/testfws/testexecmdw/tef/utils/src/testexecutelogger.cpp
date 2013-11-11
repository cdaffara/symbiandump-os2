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
* Interface class which exposes methods to TestExecute to mediate calls to HTML and XML log client sessions
* Includes construction of console which was previosly within the testexecute.cpp
* Includes methods for initialising log creation & writing initial and end up tags and results summary
*
*/



/**
 @file TestExecuteLogger.cpp
*/

#include "testexecutelogger.h"
#include "version.h"
#include "tefconst.h"
#include "serialwriter.h"
#include <test/wrapperutilsplugin.h>
#include <test/tefutils.h>

_LIT8(KHtmlBegin8,"<html><body><pre>\n");
_LIT8(KHtmlEnd8,"</pre></body></html>\n");
_LIT(KHtmlBegin16,"<html><body><pre>");
_LIT(KHtmlEnd16,"</pre></body></html>");

// Test Block XML literals
_LIT(KObjectType,		"OBJECT_TYPE");
_LIT(KObjectSection,	"OBJECT_SECTION");
_LIT(KSection,			"SECTION");
_LIT(KFunctionName,		"FUNCTION_NAME");
_LIT(KDelay,			"DELAY");
_LIT(KInterval,			"INTERVAL");
_LIT(KEErrorCode,		"EXPECTED_ERROR_CODE");
_LIT(KErrorCode,		"ERROR_CODE");		
_LIT(KEAsyncErrorCode,	"EXPECTED_ASYNC_ERROR_CODE");
_LIT(KAsyncErrorCode,	"ASYNC_ERROR_CODE");
_LIT(KBadCommand,		"BAD_COMMAND");

/**
 * Logging Test result information to html formatted file and, at same time, 
 * a simplified test file. 
 * The html file is TEF traditional log result file used to acting as TestDB.
 * The simplified file is used to enable super amount test cases.
 * @param aLogFileDir - Directory of log file, usually @ testexecute.ini 
 * 							HTML 	= ${SYSDRIVE}\logs\testexecute\ 
 * @param aLogFileName - name of log file. 
 * @param aResultBuf -  log result this is buffer of log.
 * 
 * @internalComponent
 */
void CTestExecuteLogger::LoggingTestCaseResultToSummaryResultL( const TDesC &aLogFileDir,const TDesC &aLogFileName,const TDesC8 &aResultBuf)
	{
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);

	TBuf<KMaxTestExecuteNameLength> resultFileName(aLogFileDir);
	resultFileName.Append(aLogFileName);
	RFile logFile;
	User::LeaveIfError(logFile.Open(fS,resultFileName, EFileWrite | EFileRead | EFileShareAny));
	CleanupClosePushL(logFile);

	TInt pos=0;
	(void)logFile.Seek(ESeekEnd, pos);
	(void)logFile.Write(aResultBuf);

	// Closing of objects
	CleanupStack::Pop(&logFile);
	logFile.Close();
	CleanupStack::Pop(&fS);
	fS.Close();
	}


/**
 * @param aLogOptions - One of the 3 enum values for HTML, XML and both HTML & XML logging
 * Constructor
 * Initialises HTML & XML objects for logging
 */
EXPORT_C CTestExecuteLogger::CTestExecuteLogger(TLoggerOptions aLogOptions) : 
	iLoggerOptions(aLogOptions),
	iSerialWriter(NULL),
	iPIPSExists(false)
	{
	}

/**
 * Destructor
 */
EXPORT_C CTestExecuteLogger::~CTestExecuteLogger()
	{
	}

/**
 * @param aFile - Name of the file that intiates the LogExtra call
 * @param aLine - Line number within the file that initiates the call
 * @param aSeverity - One of the 4 enum values indicating the severity of the message to be logged
 * @param aFmt - Format string that indicates the number & type of inputs required for the variable argument call
 * Function displays the message in the log based on the number of inputs followed by the format string
 */
EXPORT_C void CTestExecuteLogger::LogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,TRefByValue<const TDesC> aFmt,...)
	{
	// Create a variable list and collect the arguments into it
	VA_LIST aList;
	VA_START(aList, aFmt);

	if(iLoggerChannel == EFile || iLoggerChannel == EBoth)
		{
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			{
			// Call the Html log client interface for logging into html log
			HtmlLogger().LogExtra(aFile, aLine, aSeverity, aFmt, aList);
			}
		if (iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth)
			{
			// Call the Xml log client interface for logging into xml log
			XmlLogger().Log(aFile, aLine, RFileFlogger::TLogSeverity(aSeverity), aFmt, aList);
			}
		if(iPIPSExists)
			{
			PIPSLogger().LogExtra(aFile, aLine, aSeverity, aFmt, aList);
			}
		}
	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)
		{
		HBufC* buffer = HBufC::New(KMaxTestExecuteLogLineLength*2);
		if (buffer)
			{
			TPtr ptr(buffer->Des());
			ptr.AppendFormatList(aFmt, aList);
			iSerialWriter->WriteDecorated(ptr);
			delete buffer;
			}
		}	
	
	VA_END(aList); 	
	}

EXPORT_C TInt CTestExecuteLogger::ShareAuto()
	{
	if(iPIPSExists)
		{
		PIPSLogger().ShareAuto();
		}
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		{
		return HtmlLogger().ShareAuto();
		}
	return KErrNone;
	}

EXPORT_C void CTestExecuteLogger::Write(const TDesC& aLogBuffer)
	{
	if(iLoggerChannel == EFile || iLoggerChannel == EBoth)
		{
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			{
			HtmlLogger().Write(aLogBuffer);
			}
		}
	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)	
		{
		iSerialWriter->WriteDecorated(aLogBuffer) ; 
		}
	if(iPIPSExists)
		{
		PIPSLogger().Write(aLogBuffer);
		}
	}

EXPORT_C void CTestExecuteLogger::WriteFormat(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST aList;
	VA_START(aList, aFmt);
	if(iLoggerChannel == EFile || iLoggerChannel == EBoth)
		{
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			{
			HtmlLogger().WriteFormat(aFmt, aList);
			}
		}
	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)
		{
		HBufC* buffer = HBufC::New(KMaxTestExecuteLogLineLength*2);
		if (buffer)
			{
			TPtr ptr(buffer->Des());
			ptr.AppendFormatList(aFmt, aList);
			iSerialWriter->WriteDecorated(ptr);
			delete buffer;
			}
		}
	if(iPIPSExists)
		{
		PIPSLogger().WriteFormat(aFmt, aList);
		}
		
	VA_END(aList);
	}

EXPORT_C void CTestExecuteLogger::Write(const TDesC8& aLogBuffer)
	{
	if(iLoggerChannel == EFile || iLoggerChannel == EBoth)
		{
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			{
			HtmlLogger().Write(aLogBuffer);
			}
		}
	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)
		{
		iSerialWriter->WriteDecorated(aLogBuffer) ; 
		}
	if(iPIPSExists)
		{
		PIPSLogger().Write(aLogBuffer);
		}
	}

EXPORT_C void CTestExecuteLogger::WriteFormat(TRefByValue<const TDesC8> aFmt,...)
	{
	// Set up a Variable argument list and call private method
	VA_LIST aList;
	VA_START(aList, aFmt);
	if(iLoggerChannel == EFile || iLoggerChannel == EBoth)
	{
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			{
			HtmlLogger().WriteFormat(aFmt, aList);
			}
	}
	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)
		{
		HBufC8* buffer = HBufC8::New(KMaxTestExecuteLogLineLength*2);
		if (buffer)
			{
			TPtr8 ptr(buffer->Des());
			ptr.AppendFormatList(aFmt, aList);
			iSerialWriter->WriteDecorated(ptr);
			delete buffer;
			}
		}
	if(iPIPSExists)
		{
		PIPSLogger().WriteFormat(aFmt, aList);
		}
	VA_END(aList); 
	}

/**
 * @param aFile - Name of the file that intiates the LogExtra call
 * @param aLine - Line number within the file that initiates the call
 * @param aSeverity - One of the 4 enum values indicating the severity of the message to be logged
 * @param aCommandName - Name of the command whose fields are logged with the call to the function
 * @param aNumberOfParams - Represents the size of the structure array being passed in to the function
 * @param aLogFields - Pointer to the structure array passed in, that contains the field name & their values
 * Logs the command name along with their field names and values corresponding to each of them
 * The array should be passed in with structures for holding each of the field name/value pairs
 */
EXPORT_C void CTestExecuteLogger::LogToXml(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aCommandName, const TInt aNumberOfParams, TExtraLogField* aLogFields)
	{
	if((iLoggerChannel == EFile || iLoggerChannel == EBoth)
		&&(iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth))
		{
		XmlLogger().Log(aFile, aLine, RFileFlogger::TLogSeverity(aSeverity), aNumberOfParams, aLogFields, KTEFStringFormat, &aCommandName);
		}
	}

/**
 * @param aFile - Name of the file that intiates the LogExtra call
 * @param aLine - Line number within the file that initiates the call
 * @param aSeverity - One of the 4 enum values indicating the severity of the message to be logged
 * @param aCommandName - Name of the command whose fields are logged with the call to the function
 * Logs the command name alone for those that does not have any valid fields for logging
 */
EXPORT_C void CTestExecuteLogger::LogToXml(const TText8* aFile, TInt aLine, TInt aSeverity,const TDesC& aCommandName)
	{
	if((iLoggerChannel == EFile || iLoggerChannel == EBoth)
		&&(iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth))
			{
			XmlLogger().Log(aFile, aLine, RFileFlogger::TLogSeverity(aSeverity), KTEFStringFormat, &aCommandName);
			}
	}
	
/**
 * @param aScriptFilePath - Relative path where the script file is placed
 * @param aSeparateLogFileMode - Flag that is switched on for separate file for logging, off otherwise
 * @param aLogLevel - Indicates the default threshold level for logging messages within the XML log
 * Initialises the logging for both HTML and XML, creating the directory path & files and adding initial tags for the logs
 */
EXPORT_C void CTestExecuteLogger::InitialiseLoggingL(const TDesC& aScriptFilePath, TBool aSeparateLogFileMode, TInt aLogLevel)
	{
	// Check to see if the plugin wrapper around the GetSystemDrive is loadable
	// If yes, then instantiate the wrapper object and obtain the default system drive
	// Else, use the hardcoded default drive as c:
	TDriveName defaultSysDrive(KTEFLegacySysDrive);
	RFs fileServer;
	TVersionName version(fileServer.Version().Name());
	
	RLibrary pluginLibrary;
	CWrapperUtilsPlugin* plugin = TEFUtils::WrapperPluginNew(pluginLibrary);
	
	if (plugin!=NULL)
		{
		TDriveUnit driveUnit(plugin->GetSystemDrive());
		defaultSysDrive.Copy(driveUnit.Name());
		delete plugin;
		pluginLibrary.Close();
		}

	// Create a object of Cinidata for parsing ini files
	CTestExecuteIniData* parseTestExecuteIni = NULL;
	TBuf<KMaxTestExecuteNameLength> htmlLogPath;
	TBuf<KMaxTestExecuteNameLength> xmlLogPath;
	TInt logLevel(aLogLevel);
	TInt enableCommandsCountLogging;
	TInt outPort;
	
	TRAPD(err,parseTestExecuteIni = CTestExecuteIniData::NewL(defaultSysDrive));
	if (err == KErrNone)
		{
		CleanupStack::PushL(parseTestExecuteIni);
		parseTestExecuteIni->ExtractValuesFromIni();
		parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, htmlLogPath);
		iHtmlLogPath.Copy(htmlLogPath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFXmlKey, xmlLogPath);
		iXmlLogPath.Copy(xmlLogPath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFRemotePanicDetection, iRemotePanic);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFEnableTestsCountLog, enableCommandsCountLogging);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogChannel, iLoggerChannel);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFOutputPort, outPort);
		CleanupStack::PopAndDestroy(parseTestExecuteIni);
		}
	else
		{
		TBuf<KMaxTestExecuteNameLength> testExecuteLogPath(KTestExecuteLogPath);
		testExecuteLogPath.Replace(0, 2, defaultSysDrive);
		iHtmlLogPath.Copy(testExecuteLogPath);
		iXmlLogPath.Copy(testExecuteLogPath);
		iRemotePanic = 0;
		enableCommandsCountLogging = KTEFOneValue;
		}

		RFs fS;
		User::LeaveIfError(fS.Connect());
		CleanupClosePushL(fS);
		// Create the path arrived above
		err = fS.MkDirAll(iHtmlLogPath);
		if(err != KErrNone && err != KErrAlreadyExists)
			User::Leave(err); // Leave if unable to create the HTML log path
		err = fS.MkDirAll(iXmlLogPath);
		if(err != KErrNone && err != KErrAlreadyExists)
			User::Leave(err); // Leave if unable to create the XML log path

		// Delete any existing KTestExecuteScheduleTestLogCompatibilityNameFile
		TBuf<KMaxTestExecuteNameLength> scheduleTestLogFileNameFile(iHtmlLogPath);
		scheduleTestLogFileNameFile.Append(KTestExecuteScheduleTestLogCompatibilityNameFile);
		(void)fS.Delete(scheduleTestLogFileNameFile);
		TBuf<KMaxTestExecuteNameLength> loggerFile;
		// Set up for separate log files as a safeguard
		_LIT(KScriptEngine,"ScriptEngine");
		loggerFile.Copy(KScriptEngine);
		
		// HTML format
		_LIT(KHtmExtension,".htm");
		// XML format
		_LIT(KXmlExtension,".xml");
		
		if(!aSeparateLogFileMode)
			{
			// Extract the log file name from the full script file path
			// For example - z:\somefolder\somefile.script
			// We try and extract "somefile".
			_LIT(KTEFScriptExtension,".script");
			TInt offset = aScriptFilePath.Find(KTEFScriptExtension);
			if(offset != KErrNotFound)
				{
				// Loop back to isolate \somefile.script
				TInt i = offset-1;
				for(;i;i--)
					{
					if(aScriptFilePath[i] == '\\')
						break;
					}
				if(i)
					{
					// Get rid of the '\'
					i++;
					// For defect 116013, check the name length+path length. If bigger than 80 then leave KErrTooBig
  					if (offset-i+iHtmlLogPath.Length()+KHtmExtension().Length() > KMaxTestExecuteNameLength
  						|| offset-i+iHtmlLogPath.Length()+KXmlExtension().Length() > KMaxTestExecuteNameLength)
  						{
  						User::Leave(KErrTooBig);
  						}
  					// END defect 116013
					// Isolated somefile.script
					TInt j(0);
					// Chop off the ".script"
					loggerFile.SetLength(offset-i);
					for(;i<offset;i++,j++)
						loggerFile[j]=aScriptFilePath[i];
					RFile file;
					CleanupClosePushL(file);
					// Create KTestExecuteScheduleTestLogCompatibilityNameFile
					User::LeaveIfError(file.Create(fS,scheduleTestLogFileNameFile,EFileShareAny | EFileWrite));
					TBuf8<KMaxTestExecuteNameLength> loggerFile8;
					loggerFile8.Copy(loggerFile);
					// Write "somefile"
					User::LeaveIfError(file.Write(loggerFile8));
					CleanupStack::Pop(&file);
					file.Close();
					}
				}
			}

		// Construct the full path for HTML
		TBuf<KMaxTestExecuteNameLength> loggerHtmlFilePath(iHtmlLogPath);
		loggerHtmlFilePath.Append(loggerFile);
		loggerHtmlFilePath.Append(KHtmExtension);
		// Construct the full path for XML
		TBuf<KMaxTestExecuteNameLength> loggerXmlFilePath(iXmlLogPath);
		loggerXmlFilePath.Append(loggerFile);
		loggerXmlFilePath.Append(KXmlExtension);

		// Delete the log files if it exists
		(void)fS.Delete(loggerHtmlFilePath);
		(void)fS.Delete(loggerXmlFilePath);

		// Delete the summary file if it exists
		TBuf<KMaxTestExecuteNameLength> resultSummary(iHtmlLogPath);
		resultSummary.Append(KTEFTestExecuteResultSummaryFile);
		(void)fS.Delete(resultSummary);
		RFile file;
		// Create a summary file and write the html tags
		User::LeaveIfError(file.Create(fS,resultSummary, EFileShareAny | EFileWrite));
		CleanupClosePushL(file);
		User::LeaveIfError(file.Write(KHtmlBegin8));
		CleanupStack::Pop(&file);
		file.Close();
		
		// Delete the simplified summary file
		TBuf<KMaxTestExecuteNameLength> resultSimplifiedSummaryFile(iHtmlLogPath);
		resultSimplifiedSummaryFile.Append(KTEFTestExecuteResultSimplifiedSummaryFile);
		(void)fS.Delete(resultSimplifiedSummaryFile);
		// Create the brand-new simplified summary file
		User::LeaveIfError(file.Create(fS,resultSimplifiedSummaryFile, EFileShareAny | EFileWrite));
		file.Close();		
		
		CleanupStack::Pop(&fS);
		fS.Close();
		
		// Connect to HTML and XML log client session
		User::LeaveIfError(Connect());

	// Update the array of commands for any additions to commands count
	// Also update the KTEFCommandCountsStructSize from the TEFExportConst.h for array size
	TPtrC commandList[KTEFCommandCountsStructSize] =
		{
		TPtrC(KTEFRunScriptCommand),
		TPtrC(KTEFStartTestCaseCommand),
		TPtrC(KTEFRunTestStepCommand),
		TPtrC(KTEFRunTestStepResultCommand),
		TPtrC(KTEFRunPanicStepCommand),
		TPtrC(KTEFRunPanicStepResultCommand),
		TPtrC(KTEFRunProgramCommand),
		TPtrC(KTEFRunWSProgramCommand),
		TPtrC(KTEFStartTestBlock)
		};
	
	iRunScriptCommandCount = 0;
	// Create an instance of the script pre-processor to extract count details of commands listed in array defined above
	CScriptPreProcess scriptPreProcess(aScriptFilePath, commandList);
	
	// enableCommandsCountLogging is a flag which is updated based on the configuration done in testexecute.ini
	// The script should be pre-processed only if the flag is set to ON in testexecute.ini
	// The flag is set to OFF by default
	if (enableCommandsCountLogging == KTEFOneValue)
		{
		// Start pre-processing and updates the counts in the structure array
		scriptPreProcess.CountNoOfOccurences();
		}
	
	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)		
			{
			InitialiseSerialLoggingL(logLevel, enableCommandsCountLogging, scriptPreProcess,outPort);	
			}
		if(iLoggerChannel == EFile || iLoggerChannel == EBoth)		
			{
			if (iLoggerOptions == ELogHTMLOnly)
				{
				// Call Initilise routine for HTML logging
				// Also need to pass in the status of the enableCommandsCountLogging and the pre-processor object
				InitialiseHTMLLoggingL(loggerHtmlFilePath, logLevel, enableCommandsCountLogging, scriptPreProcess);
				}
			else if (iLoggerOptions == ELogXMLOnly)
				{
				// Call Initilise routine for XML logging
				// Also need to pass in the status of the enableCommandsCountLogging and the pre-processor object
				InitialiseXMLLoggingL(loggerXmlFilePath, logLevel, enableCommandsCountLogging, scriptPreProcess);
				}
			else
				{
				// Call Initilise routine for HTML & XML logging
				// Also need to pass in the status of the enableCommandsCountLogging and the pre-processor object
				InitialiseHTMLLoggingL(loggerHtmlFilePath, logLevel, enableCommandsCountLogging, scriptPreProcess);
				InitialiseXMLLoggingL(loggerXmlFilePath, logLevel, enableCommandsCountLogging, scriptPreProcess);
				}
			}
		if(iPIPSExists)
			{
			InitialisePIPSLoggingL(logLevel, enableCommandsCountLogging, scriptPreProcess) ;
			}
	}



EXPORT_C void CTestExecuteLogger::SetPipeName(TPtrC& aPipeName) 

	{
	User::LeaveIfError(PIPSLogger().CreateLog(aPipeName));
	}
/**
 * Connects to the HTML log client session and creating the logs with initial tags for logging
 * @param aLogFilePath - Relative path that is arrived after retrieving the path from the user through testexecute.ini
 * @param aEnableCommandCount - Integer value representing the status of the EnableTestsCountLog option in the testexecute.ini
 * @param aScriptPreProcess - Reference to the script pre-processor object used for retrieving values from structure array for logging
 * @leave - If the logger instance is not able to create the HTML log file
 */
void CTestExecuteLogger::InitialisePIPSLoggingL( TInt aLogLevel, const TInt& aEnableCommandCount, CScriptPreProcess& aScriptPreProcess)
	{

	PIPSLogger().SetLogLevel(TLogSeverity(aLogLevel));
	PIPSLogger().Write(KHtmlBegin16); // Write initial HTML tags
	// Introduce a new information block called TEST SCRIPT SYNOPSIS
	// To display synopsis of the script file and to display the TEF version
	const TInt KTEFSynopsisStringMaxLength = 512;
	TBuf<KTEFSynopsisStringMaxLength> lineBuf(KTEFFontLightBlue);
	lineBuf.Append(KTEFTestSynopsisHTM);
	lineBuf.Append(KTEFFontEnd);
	lineBuf.Append(KTEFVersionMessage);
	lineBuf.Append(KTEFProductVersion);
	lineBuf.Append(KTEFNewLine);

	// If EnableTestsCountLog is set to ON, append the command and count details to the log file during intialisation
	if (aEnableCommandCount == KTEFOneValue)
		{
		TInt index;
		// Run a loop for the structure array size
		for (index=0;index<aScriptPreProcess.GetStructSize();index++)
			{
			TBuf<KMaxTestExecuteNameLength> commandName;
			TInt commandCount;
			// Retrieve the command name and count at each array index
			aScriptPreProcess.RetrieveValues(index, commandName, commandCount);

			// Append the command name and details to the synopsis block for logging
			if (commandCount > KTEFZeroValue)
				{
				if (commandName.CompareF(KTEFRunScriptCommand) == 0)
					{
					iRunScriptCommandCount = commandCount;
					}
				lineBuf.Append(commandName);
				lineBuf.Append(KTEFCountMessageHTM);
				lineBuf.AppendNum(commandCount);
				lineBuf.Append(KTEFNewLine);
				}
			}
		}
	// Write a time and date header
	_LIT(KStarted,"*** TestExecute Started ");
		
	lineBuf.Append(KTEFNewLine);
	lineBuf.Append(KTEFFontGreen);
	lineBuf.Append(KStarted);

	TTime now;
	now.UniversalTime();
	TDateTime dateTime = now.DateTime();
	_LIT(KFormat,"%02d/%02d/%04d %02d:%02d:%02d:%03d ***");
	// add the current time and date 
	lineBuf.AppendFormat(KFormat,
		dateTime.Day()+1,
		dateTime.Month()+1,
		dateTime.Year(),
		dateTime.Hour(),
		dateTime.Minute(),
		dateTime.Second(),
		(dateTime.MicroSecond()/1000) );
	lineBuf.Append(KTEFFontEnd);
	PIPSLogger().Write(lineBuf); // Append initial header message & time of logging at the start
	
	}
/**
 * Connects to the HTML log client session and creating the logs with initial tags for logging
 * @param aLogFilePath - Relative path that is arrived after retrieving the path from the user through testexecute.ini
 * @param aEnableCommandCount - Integer value representing the status of the EnableTestsCountLog option in the testexecute.ini
 * @param aScriptPreProcess - Reference to the script pre-processor object used for retrieving values from structure array for logging
 * @leave - If the logger instance is not able to create the HTML log file
 */
void CTestExecuteLogger::InitialiseHTMLLoggingL(const TDesC& aLogFilePath, TInt aLogLevel, const TInt& aEnableCommandCount, CScriptPreProcess& aScriptPreProcess)
	{
	// Connect to the file logger server and create the log file we've just constructed
	// This will be c:\logs\testexecute\somefile.htm or c:\logs\scriptengine.htm 
	
	// Create the log file in the specified path with overwrite mode on
	User::LeaveIfError(HtmlLogger().CreateLog(aLogFilePath,RTestExecuteLogServ::ELogModeOverWrite));
	HtmlLogger().SetLogLevel(TLogSeverity(aLogLevel));

	HtmlLogger().Write(KHtmlBegin16); // Write initial HTML tags

	// Introduce a new information block called TEST SCRIPT SYNOPSIS
	// To display synopsis of the script file and to display the TEF version
	const TInt KTEFSynopsisStringMaxLength = 512;
	TBuf<KTEFSynopsisStringMaxLength> lineBuf(KTEFFontLightBlue);
	lineBuf.Append(KTEFTestSynopsisHTM);
	lineBuf.Append(KTEFFontEnd);
	lineBuf.Append(KTEFVersionMessage);
	lineBuf.Append(KTEFProductVersion);
	lineBuf.Append(KTEFNewLine);

	// If EnableTestsCountLog is set to ON, append the command and count details to the log file during intialisation
	if (aEnableCommandCount == KTEFOneValue)
		{
		TInt index;
		// Run a loop for the structure array size
		for (index=0;index<aScriptPreProcess.GetStructSize();index++)
			{
			TBuf<KMaxTestExecuteNameLength> commandName;
			TInt commandCount;
			// Retrieve the command name and count at each array index
			aScriptPreProcess.RetrieveValues(index, commandName, commandCount);

			// Append the command name and details to the synopsis block for logging
			if (commandCount > KTEFZeroValue)
				{
				if (commandName.CompareF(KTEFRunScriptCommand) == 0)
					{
					iRunScriptCommandCount = commandCount;
					}
				lineBuf.Append(commandName);
				lineBuf.Append(KTEFCountMessageHTM);
				lineBuf.AppendNum(commandCount);
				lineBuf.Append(KTEFNewLine);
				}
			}
		}

	// Write a time and date header
	_LIT(KStarted,"*** TestExecute Started ");
		
	lineBuf.Append(KTEFNewLine);
	lineBuf.Append(KTEFFontGreen);
	lineBuf.Append(KStarted);

	TTime now;
	now.UniversalTime();
	TDateTime dateTime = now.DateTime();
	_LIT(KFormat,"%02d/%02d/%04d %02d:%02d:%02d:%03d ***");
	// add the current time and date 
	lineBuf.AppendFormat(KFormat,
		dateTime.Day()+1,
		dateTime.Month()+1,
		dateTime.Year(),
		dateTime.Hour(),
		dateTime.Minute(),
		dateTime.Second(),
		(dateTime.MicroSecond()/1000) );
	lineBuf.Append(KTEFFontEnd);
	HtmlLogger().Write(lineBuf); // Append initial header message & time of logging at the start
	}

/**
 * Connects to the XML log client session and creates the logs in the path passed in
 * Also sets in the default threshold for log level
 * @param aLogFilePath - Relative path that is arrived after retrieving the path from the user through testexecute.ini
 * @param aLogLevel - Indicates the default threshold level for logging messages within the XML log
 * @param aEnableCommandCount - Integer value representing the status of the EnableTestsCountLog option in the testexecute.ini
 * @param aScriptPreProcess - Reference to the script pre-processor object used for retrieving values from structure array for logging
 * @leave - If the logger instance is not able to create the HTML log file
 */
void CTestExecuteLogger::InitialiseXMLLoggingL(const TDesC& aLogFilePath, TInt aLogLevel, const TInt& aEnableCommandCount, CScriptPreProcess& aScriptPreProcess)
	{
	// Create the log file for XML logging & open it in overwrite mode for writing the results
	User::LeaveIfError(XmlLogger().CreateLog(aLogFilePath,RFileFlogger::ELogModeOverWrite));
	// Accept and set the default threshold log level for the XML logging from the user
	XmlLogger().SetLogLevel(RFileFlogger::TLogSeverity(aLogLevel));

	if (aEnableCommandCount == KTEFOneValue)
		{
		// Create an array of TLogField structure of size equal to the number of commands counted
		TExtraLogField logField[KTEFCommandsCountPlusOne];

		// The first structure array holds the TEF version information
		logField[0].iLogFieldName.Copy(KTEFVersionMessageXML);
		logField[0].iLogFieldValue.Copy(KTEFProductVersion);

		TInt index;
		TInt actualStructSize=KTEFOneValue;
		// Run through a loop for all the array indices
		for (index=0;index<aScriptPreProcess.GetStructSize();index++)
			{
			TBuf<KMaxTestExecuteNameLength> commandName;
			TInt commandCount;
			// Retrieve the command names and counts for each array index
			aScriptPreProcess.RetrieveValues(index, commandName, commandCount);

			// Append the command name and counts details to the TExtraLogField array created already
			if (commandCount > KTEFZeroValue)
				{
				if (commandName.CompareF(KTEFRunScriptCommand) == 0)
					{
					iRunScriptCommandCount = commandCount;
					}
				logField[actualStructSize].iLogFieldName.Copy(commandName);
				logField[actualStructSize].iLogFieldName.Append(KTEFCountMessageXML);
				logField[actualStructSize].iLogFieldValue.AppendNum(commandCount);
				actualStructSize++;
				}
			}
		
		// Pass in the updated TExtraLogField array along with a message block name
		// To construct a new XML message block during the XML logging initialisation
		LogToXml(((TText8*)__FILE__),__LINE__,RFileFlogger::TLogSeverity(ESevrHigh),KTEFTestSynopsisXML,actualStructSize,logField);
		}
	else
		{
		// Create an array of TLogField structure of size equal to the number of result categories for test steps
		TExtraLogField logField[KTEFOneValue];

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[0].iLogFieldName.Copy(_L("TEF_VERSION"));
		logField[0].iLogFieldValue.Copy(KTEFProductVersion);
		
		LogToXml(((TText8*)__FILE__),__LINE__,RFileFlogger::TLogSeverity(ESevrHigh),KTEFTestSynopsisXML,KTEFOneValue,logField);
		}
	}

void CTestExecuteLogger::OpenSerialPortL(const TInt aPortNumber)
	{
	if(iSerialWriter == NULL)
		{
		iSerialWriter = CSerialWriter::NewL();
		}
	iPortNumber = 	aPortNumber; 
	iSerialWriter->Settings(iPortNumber) ; 
	}
/**
 * Instantiates the Serial Logger and passes start log header
 * @param aLogLevel - Indicates the default threshold level for logging messages within the XML log
 * @param aEnableCommandCount - Integer value representing the status of the EnableTestsCountLog option in the testexecute.ini
 * @param aScriptPreProcess - Reference to the script pre-processor object used for retrieving values from structure array for logging
 * @param aPortNumber - Port to be used for serial logging.
 * @leave - If the logger instance is not successful
 */
void CTestExecuteLogger::InitialiseSerialLoggingL(TInt /*aLogLevel*/, const TInt& aEnableCommandCount, CScriptPreProcess& aScriptPreProcess,TInt aPortNumber)
	{
	OpenSerialPortL(aPortNumber);
	// Introduce a new information block called TEST SCRIPT SYNOPSIS
	// To display synopsis of the script file and to display the TEF version
	const TInt KTEFSynopsisStringMaxLength = 512;
	TBuf<KTEFSynopsisStringMaxLength> lineBuf(KTEFTestSynopsisHTM);
	lineBuf.Append(KTEFVersionMessage);
	lineBuf.Append(KTEFProductVersion);
	lineBuf.Append(KTEFNewLine);

	// If EnableTestsCountLog is set to ON, append the command and count details to the log file during intialisation
	if (aEnableCommandCount == KTEFOneValue)
		{
		TInt index;
		// Run a loop for the structure array size
		for (index=0;index<aScriptPreProcess.GetStructSize();index++)
			{
			TBuf<KMaxTestExecuteNameLength> commandName;
			TInt commandCount;
			// Retrieve the command name and count at each array index
			aScriptPreProcess.RetrieveValues(index, commandName, commandCount);

			// Append the command name and details to the synopsis block for logging
			if (commandCount > KTEFZeroValue)
				{
				if (commandName.CompareF(KTEFRunScriptCommand) == 0)
					{
					iRunScriptCommandCount = commandCount;
					}
				lineBuf.Append(commandName);
				lineBuf.Append(KTEFCountMessageHTM);
				lineBuf.AppendNum(commandCount);
				lineBuf.Append(KTEFNewLine);
				}
			}
		}
	_LIT(KStarted,"*** TestExecute Started ");
	lineBuf.Append(KTEFNewLine);
	lineBuf.Append(KStarted);
	iSerialWriter->WriteDecorated(lineBuf) ; 
	}

/**
 * @param aCommentedCommandsCount - Integer descriptor containing the counts
 *									of commented commands in the script for logging
 * Terminate Logging routine
 * Responsible for closing the objects created for logging and also to close the log client sessions
 * Also appends final end up tags for the logs
 */
EXPORT_C void CTestExecuteLogger::TerminateLoggingL(const TInt aCommentedCommandsCount, const TInt aRemotePanicsCount, const TInt aRunScriptFailCount)
	{
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);

	// Delete the KTestExecuteScheduleTestLogCompatibilityNameFile
	// May not exist but who cares
	TBuf<KMaxTestExecuteNameLength> scheduleTestLogFileNameFile(iHtmlLogPath);
	scheduleTestLogFileNameFile.Append(KTestExecuteScheduleTestLogCompatibilityNameFile);
	(void)fS.Delete(scheduleTestLogFileNameFile);
	// Open the result summary file
	TBuf<KMaxTestExecuteNameLength> resultFileName(iHtmlLogPath);
	//load simpifiled log results in case of out-of-memory
	resultFileName.Append(KTEFTestExecuteResultSimplifiedSummaryFile);
	RFile logFile;
	User::LeaveIfError(logFile.Open(fS,resultFileName, EFileWrite | EFileRead | EFileShareAny));
	CleanupClosePushL(logFile);
	TInt fileSize;
	// Read the complete result summary file onto the heap
	// It wont be that large
	User::LeaveIfError(logFile.Size(fileSize));
	HBufC* resultData = HBufC::NewLC(fileSize);
	HBufC8* resultData8 = HBufC8::NewLC(fileSize);	
	TPtr8 ptrData8(resultData8->Des());
	User::LeaveIfError(logFile.Read(ptrData8));
	TPtr ptrData(resultData->Des());
	ptrData.Copy(ptrData8);
	logFile.Close();
	CleanupStack::PopAndDestroy(resultData8);

	// Set up some vars for the test step result types
	iPassCount=0;
	iFailCount=0;
	iAbortCount=0;
	iUnknownCount=0;
	iInconclusiveCount=0;
	iPanicCount=0;
	iUnexecutedCount=0;

	// Set up some vars for the RUN_PROGRAM result types
	iRunProgramUsed=EFalse;
	iRunProgramPassCount=0;
	iRunProgramFailCount=0;
	iRunProgramAbortCount=0;
	iRunProgramUnknownCount=0;
	iRunProgramInconclusiveCount=0;
	iRunProgramPanicCount=0;
	iRunProgramUnexecutedCount=0;

	// Set up some vars for the testcase result types
	iTestCasesUsed=EFalse;
	iTestCasePassCount=0;
	iTestCaseFailCount=0;
	iTestCaseSkippedCount=0;
	iTestCaseInconclusiveCount=0;

	// We loop through looking for the ***Result test step &
	// testcase ***TestCaseResult tokens
	// The result code will be the token after the '='
	_LIT(KEquality,"=");

	TLex lex(ptrData);

	while (!lex.Eos())
		{
		// Find the ***Result keywords to retrieve the test step summary results

		TPtrC token(lex.NextToken());

		if(token==KTEFRunProgramCommand || token==KTEFRunWSProgramCommand)
			{
			iRunProgramUsed = ETrue;
			TPtrC token(lex.NextToken());
			while(token != KTEFResultTag)
				token.Set(lex.NextToken());
			if (lex.NextToken() != KEquality)
				continue;
			token.Set(lex.NextToken());
			if(token == KTEFResultAbort)
				iRunProgramAbortCount++;
			else if(token == KTEFResultFail)
				iRunProgramFailCount++;
			else if(token == KTEFResultPass)
				iRunProgramPassCount++;
			else if(token == KTEFResultInconclusive)
				iRunProgramInconclusiveCount++;
			else if(token == KTEFResultPanic)
				iRunProgramPanicCount++;
			else if (token == KTEFResultUnexecuted)
				iRunProgramUnexecutedCount++;
			else
				iRunProgramUnknownCount++;
			}
		else if(token == KTEFResultTag)
			{
			if (lex.NextToken() != KEquality)
				continue;
			TPtrC token(lex.NextToken());
			if(token == KTEFResultAbort)
				iAbortCount++;
			else if(token == KTEFResultFail)
				iFailCount++;
			else if(token == KTEFResultPass)
				iPassCount++;
			else if(token == KTEFResultInconclusive)
				iInconclusiveCount++;
			else if(token == KTEFResultPanic)
				iPanicCount++;
			else if (token == KTEFResultUnexecuted)
				iUnexecutedCount++;
			else
				iUnknownCount++;
			}
		else
			{
			if (token == KTEFTestCaseResultTag) // Find the ***TestCaseResult keywords to retrieve the test case summary results
				{
				iTestCasesUsed = ETrue;		
				if (lex.NextToken() != KEquality)
					continue;
				TPtrC token(lex.NextToken());
				if(token == KTEFResultPass)
					iTestCasePassCount++;
				else if(token == KTEFResultFail)
					iTestCaseFailCount++;
				else if(token == KTEFResultSkippedSelectively)
					iTestCaseSkippedCount++;
				else
					iTestCaseInconclusiveCount++;
				}
			}
		}
	// Cleanup heap memory
	CleanupStack::PopAndDestroy(resultData);
	
	//summary save to test result to "TestResults.htm" 
	resultFileName.Copy(iHtmlLogPath);
	resultFileName.Append(KTEFTestExecuteResultSummaryFile);
	User::LeaveIfError(logFile.Open(fS,resultFileName, EFileWrite | EFileRead | EFileShareAny));
	TInt pos=0;
	(void)logFile.Seek(ESeekEnd, pos);
	if(iLoggerChannel == EFile || iLoggerChannel == EBoth)
		{
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			{
			TerminateHTMLLoggingL(&logFile, aCommentedCommandsCount, aRemotePanicsCount, aRunScriptFailCount);	
			}
		if (iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth)
			{
			TerminateXMLLoggingL(aCommentedCommandsCount, aRemotePanicsCount, aRunScriptFailCount);
			}
		}	
	// Cleanup/Close allocated objects
	Close();
	CleanupStack::Pop(&logFile);
	logFile.Close();
	CleanupStack::Pop(&fS);
	fS.Close();
	}



/**
 * @param aLogFile - Pointer to the file object for appending end up tags for HTML log result
 * @param aCommentedCommandsCount - Integer descriptor containing the counts
 *									of commented commands in the script for logging
 * Called by the Termination routine based on the logger options set by the user for HTML specific log results
 */
void CTestExecuteLogger::TerminateHTMLLoggingL(RFile* aLogFile, const TInt aCommentedCommandsCount, const TInt aRemotePanicsCount, const TInt aRunScriptFailCount)
	{
	// Construct a summary buffer
	// And write it to the logger file and the result summary file
	TBuf<KMaxTestExecuteLogLineLength> summaryLine;
	summaryLine.Append(KTEFFontLightBlue);

	if (iTestCasesUsed)
		{
		_LIT(KTEFTestSummary,"TEST STEP SUMMARY:");
		summaryLine.Append(KTEFTestSummary);
		}
	else
		{
		_LIT(KSummary,"SUMMARY:");
		summaryLine.Append(KSummary);	
		}

	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	TBuf8<KMaxTestExecuteLogLineLength> summaryLine8;
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));

	summaryLine.Zero();
	summaryLine.Append(KTEFFontGreen);
	summaryLine.Append(KTEFResultPass);
	summaryLine.AppendFormat(KTEFEqualsInteger,iPassCount);
	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));
	
	summaryLine.Zero();
	summaryLine.Append(KTEFFontRed);
	summaryLine.Append(KTEFResultFail);
	summaryLine.AppendFormat(KTEFEqualsInteger,iFailCount);
	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));

	summaryLine.Zero();
	summaryLine.Append(KTEFFontBlue);
	summaryLine.Append(KTEFResultAbort);
	summaryLine.AppendFormat(KTEFEqualsInteger,iAbortCount);
	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));

	summaryLine.Zero();
	summaryLine.Append(KTEFFontBlue);
	summaryLine.Append(KTEFResultPanic);
	summaryLine.AppendFormat(KTEFEqualsInteger,iPanicCount);
	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));

	summaryLine.Zero();
	summaryLine.Append(KTEFFontBlue);
	summaryLine.Append(KTEFResultInconclusive);
	summaryLine.AppendFormat(KTEFEqualsInteger,iInconclusiveCount);
	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));
	
	summaryLine.Zero();
	summaryLine.Append(KTEFFontBlue);
	summaryLine.Append(KTEFResultUnknown);
	summaryLine.AppendFormat(KTEFEqualsInteger,iUnknownCount);
	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));

	summaryLine.Zero();
	summaryLine.Append(KTEFFontBlue);
	summaryLine.Append(KTEFResultUnexecuted);
	summaryLine.AppendFormat(KTEFEqualsInteger,iUnexecutedCount);
	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));

	summaryLine.Zero();
	summaryLine.Append(KTEFFontBlue);
	summaryLine.Append(KTEFCommentedCommands);
	summaryLine.AppendFormat(KTEFEqualsInteger,aCommentedCommandsCount);
	summaryLine.Append(KTEFFontEnd);
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(summaryLine);
	summaryLine8.Copy(summaryLine);
	summaryLine8.Append(KTEFEndOfLine);
	User::LeaveIfError(aLogFile->Write(summaryLine8));

	if (iRemotePanic != 0)
		{
		summaryLine.Zero();
		summaryLine.Append(KTEFFontBlue);
		_LIT(KTEFSystemPanics, "SYSTEM PANICS COUNT");
		summaryLine.Append(KTEFSystemPanics);
		summaryLine.AppendFormat(KTEFEqualsInteger,aRemotePanicsCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
		}

	if(iRunProgramUsed)
		{
		summaryLine.Zero();
		summaryLine.Append(KTEFFontLightBlue);
		_LIT(KTEFTestCaseSummary, "RUN_PROGRAM SUMMARY:");
		summaryLine.Append(KTEFTestCaseSummary);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		TBuf8<KMaxTestExecuteLogLineLength> summaryLine8;
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
	
		summaryLine.Zero();
		summaryLine.Append(KTEFFontGreen);
		summaryLine.Append(KTEFResultPass);
		summaryLine.AppendFormat(KTEFEqualsInteger,iRunProgramPassCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
	
		summaryLine.Zero();
		summaryLine.Append(KTEFFontRed);
		summaryLine.Append(KTEFResultFail);
		summaryLine.AppendFormat(KTEFEqualsInteger,iRunProgramFailCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));

		summaryLine.Zero();
		summaryLine.Append(KTEFFontBlue);
		summaryLine.Append(KTEFResultAbort);
		summaryLine.AppendFormat(KTEFEqualsInteger,iRunProgramAbortCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));

		summaryLine.Zero();
		summaryLine.Append(KTEFFontBlue);
		summaryLine.Append(KTEFResultPanic);
		summaryLine.AppendFormat(KTEFEqualsInteger,iRunProgramPanicCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));

		summaryLine.Zero();
		summaryLine.Append(KTEFFontBlue);
		summaryLine.Append(KTEFResultInconclusive);
		summaryLine.AppendFormat(KTEFEqualsInteger,iRunProgramInconclusiveCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
	
		summaryLine.Zero();
		summaryLine.Append(KTEFFontBlue);
		summaryLine.Append(KTEFResultUnknown);
		summaryLine.AppendFormat(KTEFEqualsInteger,iRunProgramUnknownCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));

		summaryLine.Zero();
		summaryLine.Append(KTEFFontBlue);
		summaryLine.Append(KTEFResultUnexecuted);
		summaryLine.AppendFormat(KTEFEqualsInteger,iRunProgramUnexecutedCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
		}

	if (iTestCasesUsed)
		{
		summaryLine.Zero();
		summaryLine.Append(KTEFFontLightBlue);
		_LIT(KTEFTestCaseSummary, "TEST CASE SUMMARY:");
		summaryLine.Append(KTEFTestCaseSummary);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		TBuf8<KMaxTestExecuteLogLineLength> summaryLine8;
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));

		summaryLine.Zero();
		summaryLine.Append(KTEFFontGreen);
		summaryLine.Append(KTEFResultPass);
		summaryLine.AppendFormat(KTEFEqualsInteger,iTestCasePassCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
	
		summaryLine.Zero();
		summaryLine.Append(KTEFFontRed);
		summaryLine.Append(KTEFResultFail);
		summaryLine.AppendFormat(KTEFEqualsInteger,iTestCaseFailCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));

		summaryLine.Zero();
		summaryLine.Append(KTEFFontGrey);
		summaryLine.Append(KTEFResultSkippedSelectively);
		summaryLine.AppendFormat(KTEFEqualsInteger,iTestCaseSkippedCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));

		summaryLine.Zero();
		summaryLine.Append(KTEFFontBlue);
		summaryLine.Append(KTEFResultInconclusive);
		summaryLine.AppendFormat(KTEFEqualsInteger,iTestCaseInconclusiveCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
		}

	if (iRunScriptCommandCount > 0)
		{
		summaryLine.Zero();
		summaryLine.Append(KTEFFontLightBlue);
		summaryLine.Append(KTEFRunScriptSummary);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		TBuf8<KMaxTestExecuteLogLineLength> summaryLine8;
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));

		summaryLine.Zero();
		summaryLine.Append(KTEFFontGreen);
		summaryLine.Append(KTEFScriptExecuted);
		TInt executedCount = iRunScriptCommandCount - aRunScriptFailCount;
		summaryLine.AppendFormat(KTEFEqualsInteger,executedCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
	
		summaryLine.Zero();
		summaryLine.Append(KTEFFontRed);
		summaryLine.Append(KTEFFailedOpen);
		summaryLine.AppendFormat(KTEFEqualsInteger,aRunScriptFailCount);
		summaryLine.Append(KTEFFontEnd);
		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			HtmlLogger().Write(summaryLine);
		summaryLine8.Copy(summaryLine);
		summaryLine8.Append(KTEFEndOfLine);
		User::LeaveIfError(aLogFile->Write(summaryLine8));
		}

	User::LeaveIfError(aLogFile->Write(KHtmlEnd8));
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		HtmlLogger().Write(KHtmlEnd16); // Write the summary results to the HTML log file

	}

/**
 * @param aCommentedCommandsCount - Integer descriptor containing the counts
 *									of commented commands in the script for logging
 * Called by the Termination routine based on the logger options set by the user for HTML specific log results
 * Appends the XML tags for displaying the summary results for the test cases and test steps executed
 * Closes the session with XML log client session
 */
void CTestExecuteLogger::TerminateXMLLoggingL(const TInt aCommentedCommandsCount, const TInt aRemotePanicsCount, const TInt aRunScriptFailCount)
	{
	TBuf<KMaxTestExecuteCommandLength> fieldValue;
	TBuf<KMaxTestExecuteCommandLength> commandName;

	// Create an array of TLogField structure of size equal to the number of result categories for test steps
	TExtraLogField logField[9];

	// The first member of the structure stores the field name
	// The second one holds the value for the particular field
	logField[0].iLogFieldName.Copy(KTEFResultPass);
	fieldValue.Copy(KNull);
	fieldValue.AppendNum(iPassCount);
	logField[0].iLogFieldValue.Copy(fieldValue);
	
	// The first member of the structure stores the field name
	// The second one holds the value for the particular field
	logField[1].iLogFieldName.Copy(KTEFResultFail);
	fieldValue.Copy(KNull);
	fieldValue.AppendNum(iFailCount);
	logField[1].iLogFieldValue.Copy(fieldValue);

	// The first member of the structure stores the field name
	// The second one holds the value for the particular field
	logField[2].iLogFieldName.Copy(KTEFResultAbort);
	fieldValue.Copy(KNull);
	fieldValue.AppendNum(iAbortCount);
	logField[2].iLogFieldValue.Copy(fieldValue);

	// The first member of the structure stores the field name
	// The second one holds the value for the particular field
	logField[3].iLogFieldName.Copy(KTEFResultUnknown);
	fieldValue.Copy(KNull);
	fieldValue.AppendNum(iUnknownCount);
	logField[3].iLogFieldValue.Copy(fieldValue);

	// The first member of the structure stores the field name
	// The second one holds the value for the particular field
	logField[4].iLogFieldName.Copy(KTEFResultInconclusive);
	fieldValue.Copy(KNull);
	fieldValue.AppendNum(iInconclusiveCount);
	logField[4].iLogFieldValue.Copy(fieldValue);

	// The first member of the structure stores the field name
	// The second one holds the value for the particular field
	logField[5].iLogFieldName.Copy(KTEFResultPanic);
	fieldValue.Copy(KNull);
	fieldValue.AppendNum(iPanicCount);
	logField[5].iLogFieldValue.Copy(fieldValue);

	// The first member of the structure stores the field name
	// The second one holds the value for the particular field
	logField[6].iLogFieldName.Copy(KTEFResultUnexecuted);
	fieldValue.Copy(KNull);
	fieldValue.AppendNum(iUnexecutedCount);
	logField[6].iLogFieldValue.Copy(fieldValue);

	_LIT(KTEFCommentedCommands,"COMMENTED_COMMANDS");
	logField[7].iLogFieldName.Copy(KTEFCommentedCommands);
	fieldValue.Copy(KNull);
	fieldValue.AppendNum(aCommentedCommandsCount);
	logField[7].iLogFieldValue.Copy(fieldValue);

	if (iRemotePanic != 0)
		{
		_LIT(KTEFSystemPanics, "SYSTEMPANICS");
		logField[8].iLogFieldName.Copy(KTEFSystemPanics);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(aRemotePanicsCount);
		logField[8].iLogFieldValue.Copy(fieldValue);

		commandName.Copy(KTEFTestStepSummary);

		// Call the Logger().LogToXml routine which handles XML logging for individual commands
		// Takes in the command name, number of fields and the struture array
		LogToXml(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrHigh, commandName, 9, logField);
		}
	else
		{
		commandName.Copy(KTEFTestStepSummary);

		// Call the Logger().LogToXml routine which handles XML logging for individual commands
		// Takes in the command name, number of fields and the struture array
		LogToXml(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrHigh, commandName, 8, logField);
		}

	if (iRunProgramUsed)
		{
		// Create an array of TLogField structure of size equal to the number of result categories for run program logging
		TExtraLogField logField[7];

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[0].iLogFieldName.Copy(KTEFResultPass);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iRunProgramPassCount);
		logField[0].iLogFieldValue.Copy(fieldValue);
		
		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[1].iLogFieldName.Copy(KTEFResultFail);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iRunProgramFailCount);
		logField[1].iLogFieldValue.Copy(fieldValue);

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[2].iLogFieldName.Copy(KTEFResultAbort);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iRunProgramAbortCount);
		logField[2].iLogFieldValue.Copy(fieldValue);

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[3].iLogFieldName.Copy(KTEFResultUnknown);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iRunProgramUnknownCount);
		logField[3].iLogFieldValue.Copy(fieldValue);

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[4].iLogFieldName.Copy(KTEFResultInconclusive);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iRunProgramInconclusiveCount);
		logField[4].iLogFieldValue.Copy(fieldValue);

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[5].iLogFieldName.Copy(KTEFResultPanic);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iRunProgramPanicCount);
		logField[5].iLogFieldValue.Copy(fieldValue);

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[6].iLogFieldName.Copy(KTEFResultUnexecuted);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iRunProgramUnexecutedCount);
		logField[6].iLogFieldValue.Copy(fieldValue);
		
		commandName.Copy(KTEFRunProgramSummary);

		// Call the Logger().LogToXml routine which handles XML logging for individual commands
		// Takes in the command name, number of fields and the struture array
		LogToXml(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrHigh, commandName, 7, logField);
		}
	
	if (iTestCasesUsed)
		{
		// Create a TLogField structure array
		// Size of array equals to number of fields to be displayed
		TExtraLogField logField[3];

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[0].iLogFieldName.Copy(KTEFResultPass);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iTestCasePassCount);
		logField[0].iLogFieldValue.Copy(fieldValue);
		
		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[1].iLogFieldName.Copy(KTEFResultFail);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iTestCaseFailCount);
		logField[1].iLogFieldValue.Copy(fieldValue);

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[1].iLogFieldName.Copy(KTEFResultSkippedSelectively);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iTestCaseSkippedCount);
		logField[1].iLogFieldValue.Copy(fieldValue);

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[2].iLogFieldName.Copy(KTEFResultInconclusive);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(iTestCaseInconclusiveCount);
		logField[2].iLogFieldValue.Copy(fieldValue);

		commandName.Copy(KTEFTestCaseSummary);

		// Call the Logger().LogToXml routine which handles XML logging for individual commands
		// Takes in the command name, number of fields and the struture array
		LogToXml(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrHigh, commandName, 3, logField);
		}

	if (iRunScriptCommandCount > 0)
		{
		// Create a TLogField structure array
		// Size of array equals to number of fields to be displayed
		TExtraLogField logField[2];

		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[0].iLogFieldName.Copy(KTEFScriptExecuted);
		fieldValue.Copy(KNull);
		TInt runScriptPassCount = iRunScriptCommandCount - aRunScriptFailCount;
		fieldValue.AppendNum(runScriptPassCount);
		logField[0].iLogFieldValue.Copy(fieldValue);
		
		// The first member of the structure stores the field name
		// The second one holds the value for the particular field
		logField[1].iLogFieldName.Copy(KTEFFailedOpen);
		fieldValue.Copy(KNull);
		fieldValue.AppendNum(aRunScriptFailCount);
		logField[1].iLogFieldValue.Copy(fieldValue);

		commandName.Copy(KTEFRunScriptSummary);

		// Call the Logger().LogToXml routine which handles XML logging for individual commands
		// Takes in the command name, number of fields and the struture array
		LogToXml(((TText8*)__FILE__), __LINE__,RFileFlogger::ESevrHigh, commandName, 2, logField);
		}
	}

/**
 * @param aFile - Name of the file that intiated the call
 * @param aLine - Line number within the file that initiates the call
 * @param aSeverity - One of the 4 enum values indicating the severity of the message to be logged
 * @param aCommand - Name of the command whose fields are logged with the call to the function
 * @param aResult - Result of the test case for logging
 * Logs the name of the test case and the result corresponding to each START_TESTCASE/END_TESTCASE pairs
 * This function branches out of the main one for logging over serial. 
 * This is done to avoid complexity in original code.
 */
void CTestExecuteLogger::LogTestCaseResultSerialL(const TDesC& aFile, TInt aLine, TInt aSeverity, const TDesC& aCommand, const TDesC& aTestCaseName, TVerdict aResult)
	{
	
	_LIT(KTEFSpace, " ");
	_LIT(KCommand, "Command = ");
	_LIT(KLineNumber,"Line = %d");
	if (aCommand == KTEFStartTestCaseCommand || aCommand == KTEFEndTestCaseCommand
		|| aCommand == KTEFStartSyncTestCaseCommand || aCommand == KTEFEndSyncTestCaseCommand )
		{
		
		// Create a heap buffer to hold the log string for HTML
		HBufC* resultBuffer = HBufC::NewLC(KMaxTestExecuteCommandLength*3);
		TPtr ptr(resultBuffer->Des());
		HBufC* simplifiedResultBuf = HBufC::NewLC(KMaxTestExecuteCommandLength*2);
		TPtr simplifiedPtr(simplifiedResultBuf->Des());
		simplifiedPtr.Zero();

		// Appends remainder of the information required for logging
		ptr.Append(aFile);
		ptr.Append(KTEFSpace);
		ptr.AppendFormat(KLineNumber,aLine);
		ptr.Append(KTEFSpace);
		ptr.Append(KCommand);
		ptr.Append(aCommand);
		simplifiedPtr.Append(aCommand);
		ptr.Append(KTEFSpace);
		simplifiedPtr.Append(KTEFSpace);
		ptr.Append(aTestCaseName);
		simplifiedPtr.Append(aTestCaseName);
		ptr.Append(KTEFSpace);
		simplifiedPtr.Append(KTEFSpace);

		TBuf<KMaxTestExecuteCommandLength> resultTmpBuf;
		resultTmpBuf.Zero();
		// Append ***TestCaseResult only in case of END_TESTCASE command with result of the test case
		if (aCommand == KTEFEndTestCaseCommand || aCommand == KTEFEndSyncTestCaseCommand 
			|| aResult==ESkippedSelectively) //append for when command is start_test_case 
											 //but this is being skipped selectively
			{
			resultTmpBuf.Append(KTEFTestCaseResultTag);
			resultTmpBuf.Append(KTEFSpaceEquals);
			if (aResult == EPass)
				{
				resultTmpBuf.Append(KTEFResultPass);
				}
			else if (aResult == EFail)
				{
				resultTmpBuf.Append(KTEFResultFail);
				}
			else if(aResult==ESkippedSelectively)
				{
				resultTmpBuf.Append(KTEFResultSkippedSelectively);
				}
			else
				{
				resultTmpBuf.Append(KTEFResultInconclusive);
				}
			ptr.Append(resultTmpBuf);
			simplifiedPtr.Append(resultTmpBuf);
			}

		ptr.Append(KTEFSpace);
		simplifiedPtr.Append(KTEFSpace);
		// Copy the 16 bit heap descriptor value to 8 bit descriptor using a TPtr pointer to 8 bit heap descriptor
		HBufC8* resultBuffer8 = HBufC8::NewLC(ptr.Length()+2);
		TPtr8 ptr8(resultBuffer8->Des());
		ptr8.Copy(ptr);
		ptr8.Append(KTEFEndOfLine); // Append new line & carriage return for the log string
		
		//the result summary file HTML 
		LoggingTestCaseResultToSummaryResultL(iHtmlLogPath,KTEFTestExecuteResultSummaryFile,ptr8);
		//the result summary file simplified text
		//re-use result buffer
		ptr8.Copy(simplifiedPtr);
		ptr8.Append(KTEFEndOfLine); 
		LoggingTestCaseResultToSummaryResultL(iHtmlLogPath,KTEFTestExecuteResultSimplifiedSummaryFile,ptr8);
		CleanupStack::PopAndDestroy(resultBuffer8);
		// Create objects of RFs & RFile for opening the result summary file for logging

		TInt bytesCopied = 0;

		while(bytesCopied < ptr.Length())
			{
			TInt bytesToCopy = ptr.Length() - bytesCopied;
			if(bytesToCopy > KMaxTestExecuteLogLineLength)
				{
				bytesToCopy = KMaxTestExecuteLogLineLength;
				}
			TPtrC tmp(&ptr[bytesCopied],bytesToCopy);
			bytesCopied += bytesToCopy;
			iSerialWriter->WriteDecorated(tmp, aSeverity);
			}
		// Cleanup heap allocations
		CleanupStack::PopAndDestroy(simplifiedResultBuf);
		CleanupStack::PopAndDestroy(resultBuffer);
		}
	}

/**
 * @param aFile - Name of the file that intiated the call
 * @param aLine - Line number within the file that initiates the call
 * @param aSeverity - One of the 4 enum values indicating the severity of the message to be logged
 * @param aCommand - Name of the command whose fields are logged with the call to the function
 * @param aResult - Result of the test case for logging
 * Logs the name of the test case and the result corresponding to each START_TESTCASE/END_TESTCASE pairs
 */
EXPORT_C void CTestExecuteLogger::LogTestCaseResult(const TDesC& aFile, TInt aLine, TInt aSeverity, const TDesC& aCommand, const TDesC& aTestCaseName, TVerdict aResult)
	{

	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)
		{
		LogTestCaseResultSerialL(aFile, aLine, aSeverity, aCommand, aTestCaseName, aResult);
		}
	if(iLoggerChannel != EFile && iLoggerChannel != EBoth)
		{
		return ; 
		}

	_LIT(KTEFSpace, " ");
	_LIT(KCommand, "Command = ");
	_LIT(KLineNumber,"Line = %d");

	if (aCommand == KTEFStartTestCaseCommand || aCommand == KTEFEndTestCaseCommand
		|| aCommand == KTEFStartSyncTestCaseCommand || aCommand == KTEFEndSyncTestCaseCommand )
		{
		// Create a heap buffer to hold the log string for HTML
		HBufC* resultBuffer = HBufC::NewLC(KMaxTestExecuteCommandLength*3);
		TPtr ptr(resultBuffer->Des());
		HBufC* simplifiedResultBuf = HBufC::NewLC(KMaxTestExecuteCommandLength*2);
		TPtr simplifiedPtr(simplifiedResultBuf->Des());
		simplifiedPtr.Zero();
		
		// Colour of the HTML result is controlled by the result of the command
		// Anyway, the START_TESTCASE has no result attached with it & hence is always set to PASS
		if (aResult == EPass)
			{
			// Log the message in Green
			ptr.Append(KTEFFontGreen);
			}
		else if (aResult == EFail)
			{
			// Log the message in Red
			ptr.Append(KTEFFontRed);
			}
		else if(aResult == ESkippedSelectively)
			{
			ptr.Append(KTEFFontGrey);
			}
		else
			{
			// Log the message in Blue
			ptr.Append(KTEFFontBlue);
			}

		// Appends remainder of the information required for logging
		ptr.Append(aFile);
		ptr.Append(KTEFSpace);
		ptr.AppendFormat(KLineNumber,aLine);
		ptr.Append(KTEFSpace);
		ptr.Append(KCommand);
		ptr.Append(aCommand);
		simplifiedPtr.Append(aCommand);
		ptr.Append(KTEFSpace);
		simplifiedPtr.Append(KTEFSpace);
		ptr.Append(aTestCaseName);
		simplifiedPtr.Append(aTestCaseName);
		ptr.Append(KTEFSpace);
		simplifiedPtr.Append(KTEFSpace);

		TBuf<KMaxTestExecuteCommandLength> resultTmpBuf;
		resultTmpBuf.Zero();
		// Append ***TestCaseResult only in case of END_TESTCASE command with result of the test case
		if (aCommand == KTEFEndTestCaseCommand || aCommand == KTEFEndSyncTestCaseCommand 
			|| aResult==ESkippedSelectively) //append for when command is start_test_case 
											 //but this is being skipped selectively
			{
			resultTmpBuf.Append(KTEFTestCaseResultTag);
			resultTmpBuf.Append(KTEFSpaceEquals);
			if (aResult == EPass)
				{
				resultTmpBuf.Append(KTEFResultPass);
				}
			else if (aResult == EFail)
				{
				resultTmpBuf.Append(KTEFResultFail);
				}
			else if(aResult==ESkippedSelectively)
				{	
				resultTmpBuf.Append(KTEFResultSkippedSelectively);	
				}
			else
				{
				resultTmpBuf.Append(KTEFResultInconclusive);
				}
			ptr.Append(resultTmpBuf);
			simplifiedPtr.Append(resultTmpBuf);
			}

		ptr.Append(KTEFSpace);
		ptr.Append(KTEFFontEnd);
		
		// Copy the 16 bit heap descriptor value to 8 bit descriptor using a TPtr pointer to 8 bit heap descriptor
		HBufC8* resultBuffer8 = HBufC8::NewLC(ptr.Length()+2);
		TPtr8 ptr8(resultBuffer8->Des());
		ptr8.Copy(ptr);
		ptr8.Append(KTEFEndOfLine); // Append new line & carriage return for the log string
	
		// Create objects of RFs & RFile for opening the result summary file for logging
		LoggingTestCaseResultToSummaryResultL(iHtmlLogPath,KTEFTestExecuteResultSummaryFile,ptr8);

		ptr8.Copy(simplifiedPtr);
		ptr8.Append(KTEFEndOfLine);
		LoggingTestCaseResultToSummaryResultL(iHtmlLogPath,KTEFTestExecuteResultSimplifiedSummaryFile,ptr8);
		
		
		CleanupStack::PopAndDestroy(resultBuffer8);
		CleanupStack::PopAndDestroy(simplifiedResultBuf);

		if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
			{
			TInt bytesCopied = 0;

			while(bytesCopied < ptr.Length())
				{
				TInt bytesToCopy = ptr.Length() - bytesCopied;
				if(bytesToCopy > KMaxTestExecuteLogLineLength)
					{
					bytesToCopy = KMaxTestExecuteLogLineLength;
					}
				TPtrC tmp(&ptr[bytesCopied],bytesToCopy);
				bytesCopied += bytesToCopy;
				HtmlLogger().Write(tmp); // Write results to HTML log file
				if(iPIPSExists)
					{
					PIPSLogger().Write(tmp); 
					}
				}
			}

		// Cleanup heap allocations
		CleanupStack::PopAndDestroy(resultBuffer);
		}
	
	if (iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth)
		{
		_LIT(KTEFTestCaseName,"TESTCASENAME");
		_LIT(KTEFSyncTestCaseName,"SYNCTESTCASENAME");
		_LIT(KTEFTestCaseResult,"RESULT");
		
		if (aCommand == KTEFStartTestCaseCommand || aCommand == KTEFStartSyncTestCaseCommand )
			{
			// Create a TExtraLogField object and set the TestCaseName for Logging
			TExtraLogField logField[1];
			if( aCommand == KTEFStartSyncTestCaseCommand )
				{
				logField[0].iLogFieldName.Copy(KTEFSyncTestCaseName); // Sync Test Case Name
				}
			else
				{
				logField[0].iLogFieldName.Copy(KTEFTestCaseName); // Test Case Name
				}
			logField[0].iLogFieldValue.Copy(aTestCaseName);

			// Log the command name along with the field-value pairs
			XmlLogger().Log(((TText8*)__FILE__), aLine, RFileFlogger::TLogSeverity(aSeverity), 1, logField, KTEFStringFormat, &aCommand);
			}

		// If the command is END_TESTCASE, an extra RESULT field also needs to be logged
		else if (aCommand == KTEFEndTestCaseCommand || aCommand == KTEFEndSyncTestCaseCommand)
			{
			// Create a TExtraLogField object and set the TestCaseName & Result fields for Logging
			TExtraLogField logField[2];
			if( aCommand == KTEFEndSyncTestCaseCommand )
				{
				logField[0].iLogFieldName.Copy(KTEFSyncTestCaseName); // Sync Test Case Name
				}
			else
				{
				logField[0].iLogFieldName.Copy(KTEFTestCaseName); // Test Case Name
				}

			logField[0].iLogFieldValue.Copy(aTestCaseName);

			logField[1].iLogFieldName.Copy(KTEFTestCaseResult); // Test Case Result

			// Set the result value based on the aResult parameter being passed in
			if (aResult == EPass)
				{
				logField[1].iLogFieldValue.Copy(KTEFResultPass);
				}
			else if (aResult == EFail)
				{
				logField[1].iLogFieldValue.Copy(KTEFResultFail);
				}
			else if(aResult == ESkippedSelectively)
				{
					logField[1].iLogFieldValue.Copy(KTEFResultSkippedSelectively);
				}
			else
				{
				logField[1].iLogFieldValue.Copy(KTEFResultInconclusive);
				}

			// Log the command name along with the field-value pairs & Result
			XmlLogger().Log(((TText8*)__FILE__), aLine, RFileFlogger::TLogSeverity(aSeverity), 2, logField, KTEFStringFormat, &aCommand);
			}
		}
	}

/**
 * @param aCurrentScriptLine - Reference to a descriptor that holds the current script line
 * @param aScriptFile - Name of the file where the current script file exists
 * Prints the current script line
 */
EXPORT_C void CTestExecuteLogger::PrintCurrentScriptLine(TDes& aCurrentScriptLine)
	{
	TInt length = aCurrentScriptLine.Length();
	(aCurrentScriptLine[length - 2] == '\r') ? (aCurrentScriptLine.SetLength(length - 2)) : (aCurrentScriptLine.SetLength(length - 1));
			
	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)
		{
		iSerialWriter->WriteDecorated(aCurrentScriptLine) ; 
		}
	if(		(iLoggerChannel == EFile || iLoggerChannel == EBoth)
		&& (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth))
		{
		// Log the current script line to Html result log
		HtmlLogger().WriteFormat(KTEFStringFormat,&aCurrentScriptLine);
		}
	if(iPIPSExists)
		{
		PIPSLogger().WriteFormat(KTEFStringFormat,&aCurrentScriptLine);
		}
	}

LOCAL_C TVerdict SetExpectedResult(TLex aResultValue)
	{
	TInt expectedResult;
	TInt err = aResultValue.Val(expectedResult);
	if (err != KErrNone)
		{
		TBuf <KTEFMaxVerdictLength> tempString(aResultValue.NextToken());
		tempString.UpperCase();
		if (tempString.CompareF(KTEFResultFail) == KTEFZeroValue)
			{
			expectedResult = EFail;
			}
		else if(tempString.CompareF(KTEFResultInconclusive) == KTEFZeroValue)
			{
			expectedResult = EInconclusive;
			}
		else if(tempString.CompareF(KTEFResultAbort) == KTEFZeroValue)
			{
			expectedResult = EAbort;
			}
		else if(tempString.CompareF(KTEFResultPass) == KTEFZeroValue)
			{
			expectedResult = EPass;
			}
		else
			{
			expectedResult = EInconclusive;
			}
		}
	return TVerdict(expectedResult);
	}

/**
 * @param aResult - Result of execution of test steps for logging
 * @param aPanicString - Reference descriptor containing the panic string if the test step results in a panic
 * @param aScriptLineNumber - Line number within the script file where the RUN_TEST_STEP command is available
 * @param aCommand - Command name being being executed (RUN_TEST_STEP & variants)
 * @param aScriptFile - Name of the script file being used for execution
 * @param aSeverity - One of the 4 enum values indicating the severity of the message to be logged
 * Responsible for logging results for the RUN_TEST_STEP command/variants
 */
void CTestExecuteLogger::LogResultSerialL(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,const TDesC& aScriptFile)
	{
	
	HBufC* resultBuffer = HBufC::NewLC(KMaxTestExecuteCommandLength*3);
	TPtr ptr(resultBuffer->Des());
	TLex scriptLineLex(aCommand);
	TLex scriptLineParamLex(aCommand);
	TLex xmlLogCommandParseLex(aCommand);
	TPtrC command(scriptLineLex.NextToken());
	TBool showPanicResult(ETrue);
	TBool showActualResult(EFalse);
	TBool showPanicInfo(EFalse);
	TVerdict actualResult(aResult);
	TVerdict actualPanic(aResult);
	TInt expectedPanicCode(0);
	TInt expectedReturnCode(EPass);
	TPtrC expectedPanicStringLex(KNull);
	TPtrC expectedErrorStringLex(KNull);
	
	// Newly added variables for implementing !Params for TEF
	TVerdict tefResult(EPass);
	TBuf<KTEFTestExecuteParamLength> tefError(KNull);
	TBuf<KMaxTestExecuteCommandLength> tefPanicString(KNull);
	TBuf<KTEFTestExecuteParamLength> tefPanicCode(KNull);
	TBool paramExistence(EFalse);
	TPtrC panicString;
	TPtrC errorString;
	TPtrC defaultStr(KNull);
	/// Start of defect Fix : defect 037262

	// RUN_TEST_STEP_RESULT
	// RUN_PANIC_STEP_RESULT
	//
	// Check that the expected result meets that of the  actual result.
	// 
	// RUN_TEST_STEP_RESULT will return :
	// 
	// PASS if the expected result meets the actual result
	// FAIL if expected result was not the same as actual, the actual result is also displayed
	// PANIC if the testcode panicked.
	//
	// RUN_PANIC_STEP_RESULT will return :
	//
	// PASS if Testcode panicked and expected/actual panic code/ strings match
	// PANIC if TestCode panicked but the expected/actual panic code/ strings differ
	// FAIL if Testcode did not panic, the actual result is also displayed
	//
	// RUN_PANIC_STEP will will now return : (for defect 039749)
	// PASS if Testcode panicked irrespective of panic code/ strings, the code & strings are displayed
	// FAIL if Testcode did not panic, the actual result is also displayed
	
	if (aPanicString.Length() >= KTEFMinPanicStringLength && aPanicString.Mid(0,KTEFMinPanicStringLength).CompareF(KErrorEquals) == KTEFZeroValue)
		{
	 	errorString.Set(aPanicString.Mid(KTEFMinPanicStringLength));
	 	panicString.Set(KNull);
	 	}
	else if (aPanicString.Length() >= KTEFMinPanicStringLength && aPanicString.Mid(0,KTEFMinPanicStringLength).CompareF(KPanicEquals) == KTEFZeroValue)
		{
		panicString.Set(aPanicString.Mid(KTEFMinPanicStringLength));
		defaultStr.Set(panicString);
		errorString.Set(KNull);
		}
	
	if(command == KTEFRunTestStepCommand && aResult!= EIgnore)
		{
		TInt firstChar;
		TPtrC commandStr(scriptLineParamLex.NextToken());
		while(!scriptLineParamLex.Eos())
			{
			TInt err(0);
			scriptLineParamLex.SkipSpace();
			commandStr.Set(scriptLineParamLex.NextToken());
			firstChar = commandStr[0];
			// 33 is the ascii value for "!". Used here for confirming switches
			if (firstChar == KTEFAsciiExclamation)
				{
				if (commandStr.Length() >= KTEFMinErrorParamLength && commandStr.Mid(0,KTEFMinErrorParamLength).CompareF(KTEFError) == KTEFZeroValue)
					{
					if(command == KTEFRunTestStepCommand)
						{
						paramExistence = ETrue;
						command.Set(KTEFRunErrorStepResultCommand);
						TRAP(err,tefError.Copy(commandStr.Mid(KTEFMinErrorParamLength)));
						if (err)
							tefError.Copy(KNull);
						defaultStr.Set(errorString);
						}
					}
				else if (commandStr.Length() >= KTEFMinResultParamLength && commandStr.Mid(0,KTEFMinResultParamLength).CompareF(KTEFResult) == KTEFZeroValue)
					{
					if (command == KTEFRunTestStepCommand)
						{
						paramExistence = ETrue;
						command.Set(KTEFRunTestStepResultCommand);
						TLex resultVal(commandStr.Mid(KTEFMinResultParamLength));
						tefResult = SetExpectedResult(resultVal);
						}
					}
				else if (commandStr.Length() >= KTEFMinPanicCodeParamLength && commandStr.Mid(0,KTEFMinPanicCodeParamLength).CompareF(KTEFPanicCode) == KTEFZeroValue)
					{
					if(command == KTEFRunTestStepCommand)
						{
						paramExistence = ETrue;
						command.Set(KTEFRunPanicStepResultCommand);
						TRAP(err,tefPanicCode.Copy(commandStr.Mid(KTEFMinPanicCodeParamLength)));
						if (err)
							tefPanicCode.Copy(KNull);
						defaultStr.Set(panicString);
						}
					else if(command == KTEFRunPanicStepResultCommand && tefPanicString.Length() != KTEFZeroValue)
						{
						TRAP(err,tefPanicCode.Copy(commandStr.Mid(KTEFMinPanicCodeParamLength)));
						if (err)
							tefPanicCode.Copy(KNull);
						defaultStr.Set(panicString);
						}
					}
				else if (commandStr.Length() >= KTEFMinPanicStringParamLength && commandStr.Mid(0,KTEFMinPanicStringParamLength).CompareF(KTEFPanicString) == KTEFZeroValue)
					{
					if(command == KTEFRunTestStepCommand)
						{
						paramExistence = ETrue;
						command.Set(KTEFRunPanicStepResultCommand);
						TRAP(err,tefPanicString.Copy(commandStr.Mid(KTEFMinPanicStringParamLength)));
						if (err)
							tefPanicString.Copy(KNull);
						else
							{
							if (tefPanicString.Mid(0,1) == KTEFOpenQuotes)
								tefPanicString.Copy(tefPanicString.Mid(1,tefPanicString.Length()-2));
							}
						defaultStr.Set(panicString);
						}
					else if(command == KTEFRunPanicStepResultCommand && tefPanicString.Length() != KTEFZeroValue)
						{
						TRAP(err,tefPanicString.Copy(commandStr.Mid(KTEFMinPanicStringParamLength)));
						if (err)
							tefPanicString.Copy(KNull);
						else
							{
							if (tefPanicString.Mid(0,1) == KTEFOpenQuotes)
								tefPanicString.Copy(tefPanicString.Mid(1,tefPanicString.Length()-2));
							}
						defaultStr.Set(panicString);
						}
					}
				}
			else
				break;
			}
		}

	if (aResult!= EIgnore)
		{
		if ((command == KTEFRunTestStepResultCommand) && (defaultStr.Length() == KTEFZeroValue))
			{
			// Pickout the expected returncode value
			TLex expectedReturnCodeLex(scriptLineLex.NextToken());
			TInt err;
			if (paramExistence)
				{
				expectedReturnCode = tefResult;
				err = 0;
				}
			else
				err = expectedReturnCodeLex.Val(expectedReturnCode);
			if (err == KTEFZeroValue)
				{
				if (expectedReturnCode == aResult)
					{
					aResult = EPass;
					}
				else
					{
					aResult				= EFail;
					showActualResult	= ETrue;
					}
				}
			}
		else if (command == KTEFRunPanicStepResultCommand)
			{
			if (defaultStr.Length()) // we have indeed panicked
				{
				// Pickout the expected panic code value
				// not necessarily 0, 106-110,  but any integer value set by test step
				TLex expectedPanicCodeLex(scriptLineLex.NextToken());
				//TInt expectedPanicCode;
				TInt err;
				if (paramExistence)
					{
					if (tefPanicCode.Length())
						{
						TLex PanicCodeLex(tefPanicCode);
						err = PanicCodeLex.Val(expectedPanicCode);
						}
					else
						{
						expectedPanicCode = aResult;
						err = 0;
						}
					}
				else
					err = expectedPanicCodeLex.Val(expectedPanicCode);

				if (err == KTEFZeroValue)
					{
					if (expectedPanicCode == aResult)
						{
						//TPtrC expectedPanicStringLex;
						// Now check ALSO that the panic string is identical.
						if (paramExistence)
							{
								if (tefPanicString.Length())
									expectedPanicStringLex.Set(tefPanicString);
								else
									expectedPanicStringLex.Set(defaultStr);
							}
						else
							expectedPanicStringLex.Set(scriptLineLex.NextToken());
						if (expectedPanicStringLex == defaultStr)
							{
							// Pretend its not a Panic
							aResult			= EPass;
							showPanicResult	= EFalse;
							}
						else
							{
							aResult			= EFail;
							showPanicInfo	= ETrue;
							showPanicResult	= EFalse;
							}
						}
					else
						{
						aResult			= EFail;
						showPanicInfo	= ETrue;
						showPanicResult	= EFalse;
						}
					}
				}
			else
				{
				// we haven't panicked, we were looking for one, so this is a FAIL.
				aResult				= EFail;
				showPanicResult		= EFalse;
				showActualResult	= ETrue;
				}
			}
		else if (command == KTEFRunPanicStepCommand)
			{
			if (defaultStr.Length()) // we have indeed panicked
				{
				// Pretend its not a Panic
				aResult			= EPass;
				showPanicInfo	= ETrue;
				showPanicResult = EFalse;
				}
			else
				{
				// we haven't panicked, we were looking for one, so this is a FAIL.
				aResult				= EFail;
				showPanicResult		= EFalse;
				showActualResult	= ETrue;
				}
			}
		else if (command == KTEFRunErrorStepResultCommand && paramExistence)
			{
			if (defaultStr.Length())
				{
				// Pickout the expected returncode value
				//TPtrC expectedErrorStringLex;
				expectedErrorStringLex.Set(tefError);
			
				if (expectedErrorStringLex == defaultStr)
					{
					// Pretend its not Error
					aResult = EPass;
					showPanicResult		= EFalse;
					}
				else
					{
					aResult				= EFail;
					showPanicResult		= EFalse;
					showPanicInfo		= ETrue;
					}
				}
			else
				{
				// We haven't got any error, so this is a FAIL
				aResult				= EFail;
				showPanicResult		= EFalse;
				showActualResult	= ETrue;
				}
			}
		else if (errorString.Length())
			{
			aResult				= EFail;
			defaultStr.Set(errorString);
			showPanicResult		= EFalse;
			showPanicInfo		= ETrue;
			}
		}

	// End of bulk of defect defect 037262, fix also includes references to showPanicResult below
	// allowing expected Panics to show as Passes + printing of actualResult.
	ptr.Append(aScriptFile);
	_LIT(KLineNumber," Line = %d");
	ptr.AppendFormat(KLineNumber,aScriptLineNumber);
	_LIT(KCommand," Command = ");
	ptr.Append(KCommand);
	ptr.Append(aCommand);
	ptr.Trim();
	_LIT(KTEFSpace," ");
	ptr.Append(KTEFSpace);

	_LIT(KCode," Code = %d ");
	if (showActualResult)
		{
		_LIT(KActualResult, "Returned: ");
		ptr.Append(KActualResult);
		if(actualResult == EPass)
			ptr.Append(KTEFResultPass);
		else if(actualResult == EFail)
			ptr.Append(KTEFResultFail);
		else if(actualResult == EInconclusive)
			ptr.Append(KTEFResultInconclusive);
		else if(actualResult == EAbort || aResult == KErrAbort)
			ptr.Append(KTEFResultAbort);
		else 
			{
			// Log the error code
			ptr.Append(KTEFResultUnknown);
			ptr.AppendFormat(KCode,(TInt)actualResult);
			}
		ptr.Append(KTEFSpace);
		}

	if (showPanicInfo)
		{
		if (errorString.Length())
			{
			TLex errorValue(defaultStr);
			TInt errorInt;
			errorValue.Val(errorInt);
			ptr.AppendFormat(KCode,(TInt)errorInt);
			ptr.Append(KTEFSpace);			
			}
		else
			{
			ptr.AppendFormat(KCode,(TInt)actualPanic);
			ptr.Append(KTEFSpace);
			ptr.Append(defaultStr);
			ptr.Append(KTEFSpace);
			}
		}

	ptr.Append(KTEFResultTag);
	ptr.Append(KTEFSpaceEquals);

	if ((defaultStr.Length()) && showPanicResult)
		{
		ptr.Append(KTEFResultPanic);
		_LIT(KHyphen," - ");
		ptr.Append(KHyphen);
		ptr.Append(defaultStr);
		ptr.AppendFormat(KCode,(TInt)aResult);
		}
	else if(aResult == EPass)
		ptr.Append(KTEFResultPass);
	else if(aResult == EFail)
		ptr.Append(KTEFResultFail);
	else if(aResult == EInconclusive)
		ptr.Append(KTEFResultInconclusive);
	else if(aResult == EAbort || aResult == KErrAbort)
		ptr.Append(KTEFResultAbort);
	else if (aResult == EIgnore)
		ptr.Append(KTEFResultUnexecuted);
	else
		{
		// Log the error code
		ptr.Append(KTEFResultUnknown);
		ptr.AppendFormat(KCode,(TInt)aResult);
		}
	ptr.Append(KTEFSpace);

	HBufC8* resultBuffer8 = HBufC8::NewLC(ptr.Length()+2);
	TPtr8 ptr8(resultBuffer8->Des());
	ptr8.Copy(ptr);
	ptr8.Append(KTEFEndOfLine);
	
	LoggingTestCaseResultToSummaryResultL(iHtmlLogPath,KTEFTestExecuteResultSummaryFile,ptr8);
	
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		{
		TInt bytesCopied = 0;

		while(bytesCopied < ptr.Length())
			{
			TInt bytesToCopy = ptr.Length() - bytesCopied;
			if(bytesToCopy > KMaxTestExecuteLogLineLength)
				{
				bytesToCopy = KMaxTestExecuteLogLineLength;
				}
			TPtrC tmp(&ptr[bytesCopied],bytesToCopy);
			bytesCopied += bytesToCopy;
			iSerialWriter->WriteDecorated(tmp);
			}
		}
	CleanupStack::PopAndDestroy(resultBuffer8);
	CleanupStack::PopAndDestroy(resultBuffer);
	}

/**
 * @param aResult - Result of execution of test steps for logging
 * @param aPanicString - Reference descriptor containing the panic string if the test step results in a panic
 * @param aScriptLineNumber - Line number within the script file where the RUN_TEST_STEP command is available
 * @param aCommand - Command name being being executed (RUN_TEST_STEP & variants)
 * @param aScriptFile - Name of the script file being used for execution
 * @param aSeverity - One of the 4 enum values indicating the severity of the message to be logged
 * Responsible for logging results for the RUN_TEST_STEP command/variants
 */
void CTestExecuteLogger::LogResultFileL(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,const TDesC& aScriptFile, TInt aSeverity)
	{
	HBufC* resultBuffer = HBufC::NewLC(KMaxTestExecuteCommandLength*3);
	TPtr ptr(resultBuffer->Des());
	HBufC* simplifiedResultBuf = HBufC::NewLC(KMaxTestExecuteCommandLength*3);
	TPtr simplifiedPtr(simplifiedResultBuf->Des());
	TLex scriptLineLex(aCommand);
	TLex scriptLineParamLex(aCommand);
	TLex xmlLogCommandParseLex(aCommand);
	TPtrC command(scriptLineLex.NextToken());
	TBool showPanicResult(ETrue);
	TBool showActualResult(EFalse);
	TBool showPanicInfo(EFalse);
	TBool isARealPanic(EFalse); //PDEF117769
	TVerdict actualResult(aResult);
	TVerdict actualPanic(aResult);
	TInt expectedPanicCode(0);
	TInt expectedReturnCode(EPass);
	TPtrC expectedPanicStringLex(KNull);
	TPtrC expectedErrorStringLex(KNull);
	
	// Newly added variables for implementing !Params for TEF
	TVerdict tefResult(EPass);
	TBuf<KTEFTestExecuteParamLength> tefError(KNull);
	// using 256 because in script this parameter could be very long
	TBuf<KMaxTestExecuteCommandLength> tefPanicString(KNull); 
	TBuf<KTEFTestExecuteParamLength> tefPanicCode(KNull);
	TBool paramExistence(EFalse);
	TPtrC panicString;
	TPtrC errorString;
	TPtrC defaultStr(KNull);
	/// Start of defect Fix : defect 037262

	// RUN_TEST_STEP_RESULT
	// RUN_PANIC_STEP_RESULT
	//
	// Check that the expected result meets that of the  actual result.
	// 
	// RUN_TEST_STEP_RESULT will return :
	// 
	// PASS if the expected result meets the actual result
	// FAIL if expected result was not the same as actual, the actual result is also displayed
	// PANIC if the testcode panicked.
	//
	// RUN_PANIC_STEP_RESULT will return :
	//
	// PASS if Testcode panicked and expected/actual panic code/ strings match
	// PANIC if TestCode panicked but the expected/actual panic code/ strings differ
	// FAIL if Testcode did not panic, the actual result is also displayed
	//
	// RUN_PANIC_STEP will will now return : (for defect 039749)
	// PASS if Testcode panicked irrespective of panic code/ strings, the code & strings are displayed
	// FAIL if Testcode did not panic, the actual result is also displayed

	if (aPanicString.Length() >= KTEFMinPanicStringLength && aPanicString.Mid(0,KTEFMinPanicStringLength).CompareF(KErrorEquals) == KTEFZeroValue)
		{
	 	errorString.Set(aPanicString.Mid(KTEFMinPanicStringLength));
	 	panicString.Set(KNull);
	 	}
	else if (aPanicString.Length() >= KTEFMinPanicStringLength && aPanicString.Mid(0,KTEFMinPanicStringLength).CompareF(KPanicEquals) == KTEFZeroValue)
		{
		panicString.Set(aPanicString.Mid(KTEFMinPanicStringLength));
		defaultStr.Set(panicString);
		errorString.Set(KNull);
		isARealPanic = ETrue;//PDEF117769
		if (panicString.Length() == KTEFMaxPanicStringLength ) 
			{
			//TEF not know whether the panic string is being turncated.
			//if received panic string == 16, TEF will give warning info to log.
			LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, KTEFWarnPanicString );
			}
		}
	
	//PDEF117769
	// the panic code (110) is SAME with define of EIgnore(110)
	// When Panic 110 really happens, the defect code will take this situation as EIgnore.
	// Solution is get the panic string when panic happens, then compare with expected panic string.
	// aPanicString.Length() > 0, when panic really happens.
	// So an extra condition at last.
	if((command == KTEFRunTestStepCommand || command == KTEFEndTestBlock) && (aResult!= EIgnore || isARealPanic ))	
		{
		TInt firstChar;
		TPtrC commandStr(scriptLineParamLex.NextToken());
		while(!scriptLineParamLex.Eos())
			{
			TInt err(0);
			scriptLineParamLex.SkipSpace();
			commandStr.Set(scriptLineParamLex.NextToken());
			if( commandStr.Length()>0 )
				{
				firstChar = commandStr[0];
				// 33 is the ascii value for "!". Used here for confirming switches
				if (firstChar == KTEFAsciiExclamation)
					{
					if (commandStr.Length() >= KTEFMinErrorParamLength && commandStr.Mid(0,KTEFMinErrorParamLength).CompareF(KTEFError) == KTEFZeroValue)
						{
						if(command == KTEFRunTestStepCommand || command == KTEFEndTestBlock)
							{
							paramExistence = ETrue;
							command.Set(KTEFRunErrorStepResultCommand);
							TRAP(err,tefError.Copy(commandStr.Mid(KTEFMinErrorParamLength)));
							if (err)
								tefError.Copy(KNull);
							defaultStr.Set(errorString);
							}
						}
					else if (commandStr.Length() >= KTEFMinResultParamLength && commandStr.Mid(0,KTEFMinResultParamLength).CompareF(KTEFResult) == KTEFZeroValue)
						{
						if (command == KTEFRunTestStepCommand || command == KTEFEndTestBlock)
							{
							paramExistence = ETrue;
							command.Set(KTEFRunTestStepResultCommand);
							TLex resultVal(commandStr.Mid(KTEFMinResultParamLength));
							tefResult = SetExpectedResult(resultVal);
							}
						}
					else if (commandStr.Length() >= KTEFMinPanicCodeParamLength && commandStr.Mid(0,KTEFMinPanicCodeParamLength).CompareF(KTEFPanicCode) == KTEFZeroValue)
						{
						if(command == KTEFRunTestStepCommand || command == KTEFEndTestBlock)
							{
							paramExistence = ETrue;
							command.Set(KTEFRunPanicStepResultCommand);
							TRAP(err,tefPanicCode.Copy(commandStr.Mid(KTEFMinPanicCodeParamLength)));
							if (err)
								tefPanicCode.Copy(KNull);
							defaultStr.Set(panicString);
							}
						//PDEF117769
						else if( command == KTEFRunPanicStepResultCommand )
							{
							TRAP(err,tefPanicCode.Copy(commandStr.Mid(KTEFMinPanicCodeParamLength)));
							if (err)
								tefPanicCode.Copy(KNull);
							defaultStr.Set(panicString);
							}
						}
					else if (commandStr.Length() >= KTEFMinPanicStringParamLength && commandStr.Mid(0,KTEFMinPanicStringParamLength).CompareF(KTEFPanicString) == KTEFZeroValue)
						{
						if(command == KTEFRunTestStepCommand || command == KTEFEndTestBlock)
							{
							paramExistence = ETrue;
							command.Set(KTEFRunPanicStepResultCommand);
							
							ParsePaincStringFromScriptCommandLine(commandStr,scriptLineParamLex,tefPanicString);							
							defaultStr.Set(panicString);
							}
						//PDEF117769
						else if(command == KTEFRunPanicStepResultCommand)
							{
							ParsePaincStringFromScriptCommandLine(commandStr,scriptLineParamLex,tefPanicString);
							defaultStr.Set(panicString);
							}

						// print out the panic string in panic dialog
						// this make easier to user check what happened, when panic string compare fail.
						if (isARealPanic && tefPanicString.Compare(panicString))
							{
							LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, KTEFExpectedPanicString, &tefPanicString);							
							LogExtra(((TText8*)__FILE__), __LINE__, ESevrInfo, KTEFReceivedPanicString, &panicString);
							}
						}
					}
				}
			else
				break;
			}
		}
	
	//PDEF117769
	// See more detail above
	if (aResult!= EIgnore || isARealPanic )
		{
		if ((command == KTEFRunTestStepResultCommand) && (defaultStr.Length() == KTEFZeroValue))
			{
			// Pickout the expected returncode value
			TLex expectedReturnCodeLex(scriptLineLex.NextToken());
			TInt err;
			if (paramExistence)
				{
				expectedReturnCode = tefResult;
				err = 0;
				}
			else
				err = expectedReturnCodeLex.Val(expectedReturnCode);
			if (err == KTEFZeroValue)
				{
				if (expectedReturnCode == aResult)
					{
					aResult = EPass;
					}
				else
					{
					aResult				= EFail;
					showActualResult	= ETrue;
					}
				}
			}
		else if (command == KTEFRunPanicStepResultCommand)
			{
			//PDEF117769 start
			if ( isARealPanic ) // if panic really happens aPanicString !=0 (or >=2)
										 // we have indeed panicked
			//PDEF117769 end
				{
				// Pickout the expected panic code value
				// not necessarily 0, 106-110,  but any integer value set by test step
				TLex expectedPanicCodeLex(scriptLineLex.NextToken());
				//TInt expectedPanicCode;
				TInt err;
				if (paramExistence)
					{
					if (tefPanicCode.Length())
						{
						TLex PanicCodeLex(tefPanicCode);
						err = PanicCodeLex.Val(expectedPanicCode);
						}
					else
						{
						expectedPanicCode = aResult;
						err = 0;
						}
					}
				else
					err = expectedPanicCodeLex.Val(expectedPanicCode);

				if (err == KTEFZeroValue)
					{
					if (expectedPanicCode == aResult)
						{
						//TPtrC expectedPanicStringLex;
						// Now check ALSO that the panic string is identical.
						if (paramExistence)
							{
								if (tefPanicString.Length())
									expectedPanicStringLex.Set(tefPanicString);
								else
									expectedPanicStringLex.Set(defaultStr);
							}
						else
							{
							expectedPanicStringLex.Set(scriptLineLex.NextToken());
							if (expectedPanicStringLex.Left(1) == KTEFOpenQuotes)
								{
								if (expectedPanicStringLex.Right(1) != KTEFOpenQuotes)
									{
									TBuf<KTEFTestExecuteParamLength> concatPanicString(expectedPanicStringLex.Mid(1,expectedPanicStringLex.Length()-1));
									TBool panicStringCompleted(EFalse);
									while(!scriptLineLex.Eos() && !panicStringCompleted)
										{
										TPtrC panicStringExt(scriptLineLex.NextToken());
										concatPanicString.Append(KTEFSpace);
										if(panicStringExt.Right(1) == KTEFOpenQuotes)
											{
											panicStringCompleted = ETrue;
											concatPanicString.Append(panicStringExt.Mid(0, panicStringExt.Length()-1));
											}
										else
											concatPanicString.Append(panicStringExt);
										}
									expectedPanicStringLex.Set(concatPanicString);
									}
								else
									expectedPanicStringLex.Set(expectedPanicStringLex.Mid(1,expectedPanicStringLex.Length()-2));
								}
							}
						//PDEF117769 
						if ( (expectedPanicStringLex.Length() == KTEFZeroValue && defaultStr.Length() == KTEFZeroValue ) || expectedPanicStringLex == defaultStr )
							{
							// Pretend its not a Panic
							aResult			= EPass;
							showPanicResult	= EFalse;
							}
						else
							{
							aResult			= EFail;
							showPanicInfo	= ETrue;
							showPanicResult	= EFalse;
							}
						}
					else
						{
						aResult			= EFail;
						showPanicInfo	= ETrue;
						showPanicResult	= EFalse;
						}
					}
				}
			else
				{
				// we haven't panicked, we were looking for one, so this is a FAIL.
				aResult				= EFail;
				showPanicResult		= EFalse;
				showActualResult	= ETrue;
				}
			}
		else if (command == KTEFRunPanicStepCommand)
			{
			if ( isARealPanic )  // we have indeed panicked
				{
				// Pretend its not a Panic
				aResult			= EPass;
				showPanicInfo	= ETrue;
				showPanicResult = EFalse;
				}
			else
				{
				// we haven't panicked, we were looking for one, so this is a FAIL.
				aResult				= EFail;
				showPanicResult		= EFalse;
				showActualResult	= ETrue;
				}
			}
		else if (command == KTEFRunErrorStepResultCommand && paramExistence)
			{
			if (defaultStr.Length())
				{
				// Pickout the expected returncode value
				//TPtrC expectedErrorStringLex;
				expectedErrorStringLex.Set(tefError);
			
				if (expectedErrorStringLex == defaultStr)
					{
					// Pretend its not Error
					aResult = EPass;
					showPanicResult		= EFalse;
					}
				else
					{
					aResult				= EFail;
					showPanicResult		= EFalse;
					showPanicInfo		= ETrue;
					}
				}
			else
				{
				// We haven't got any error, so this is a FAIL
				aResult				= EFail;
				showPanicResult		= EFalse;
				showActualResult	= ETrue;
				}
			}
		else if (errorString.Length())
			{
			aResult				= EFail;
			defaultStr.Set(errorString);
			showPanicResult		= EFalse;
			showPanicInfo		= ETrue;
			}
		}

	// End of bulk of defect defect 037262, fix also includes references to showPanicResult below
	// allowing expected Panics to show as Passes + printing of actualResult.

	// Construct result tags for HTML logging
	if ((defaultStr.Length()) && showPanicResult)
		ptr.Append(KTEFFontBlue);
	else if(aResult == EPass)
		ptr.Append(KTEFFontGreen);
	else if (aResult == EFail)
		ptr.Append(KTEFFontRed);
	else
		ptr.Append(KTEFFontBlue);
	ptr.Append(aScriptFile);
	_LIT(KLineNumber," Line = %d");
	ptr.AppendFormat(KLineNumber,aScriptLineNumber);
	_LIT(KCommand," Command = ");
	ptr.Append(KCommand);	
	ptr.Append(aCommand);
	ptr.Trim();
	simplifiedPtr.Append(aCommand);
	simplifiedPtr.Trim();
	ptr.Append(KTEFSpace);
	simplifiedPtr.Append(KTEFSpace);
	
	_LIT(KCode," Code = %d ");

	TBuf<KMaxTestExecuteCommandLength> resultTmpBuf;
	resultTmpBuf.Zero();
	if (showActualResult)
		{
		_LIT(KActualResult, "Returned: ");
		resultTmpBuf.Copy(KActualResult);
		if(actualResult == EPass)
			resultTmpBuf.Append(KTEFResultPass);
		else if(actualResult == EFail)
			resultTmpBuf.Append(KTEFResultFail);
		else if(actualResult == EInconclusive)
			resultTmpBuf.Append(KTEFResultInconclusive);
		else if(actualResult == EAbort || aResult == KErrAbort)
			resultTmpBuf.Append(KTEFResultAbort);
		else 
			{
			// Log the error code
			resultTmpBuf.Append(KTEFResultUnknown);
			resultTmpBuf.AppendFormat(KCode,(TInt)actualResult);
			}
		resultTmpBuf.Append(KTEFSpace);
		}
	ptr.Append(resultTmpBuf);
	simplifiedPtr.Append(resultTmpBuf);
	
	if (showPanicInfo)
		{
		if (errorString.Length())
			{
			TLex errorValue(defaultStr);
			TInt errorInt;
			errorValue.Val(errorInt);
			ptr.AppendFormat(KCode,(TInt)errorInt);
			ptr.Append(KTEFSpace);			
			}
		else
			{
			ptr.AppendFormat(KCode,(TInt)actualPanic);
			ptr.Append(KTEFSpace);
			ptr.Append(defaultStr);
			ptr.Append(KTEFSpace);
			}
		}

	ptr.Append(KTEFResultTag);
	simplifiedPtr.Append(KTEFResultTag);
	ptr.Append(KTEFSpaceEquals);
	simplifiedPtr.Append(KTEFSpaceEquals);
	
	resultTmpBuf.Zero();
	if ((defaultStr.Length()) && showPanicResult)
		{
		resultTmpBuf.Append(KTEFResultPanic);
		_LIT(KHyphen," - ");
		resultTmpBuf.Append(KHyphen);
		resultTmpBuf.Append(defaultStr);
		resultTmpBuf.AppendFormat(KCode,(TInt)aResult);
		}
	else if(aResult == EPass)
		resultTmpBuf.Append(KTEFResultPass);
	else if(aResult == EFail)
		resultTmpBuf.Append(KTEFResultFail);
	else if(aResult == EInconclusive)
		resultTmpBuf.Append(KTEFResultInconclusive);
	else if(aResult == EAbort || aResult == KErrAbort)
		resultTmpBuf.Append(KTEFResultAbort);
	else if (aResult == EIgnore)
		resultTmpBuf.Append(KTEFResultUnexecuted);
	else
		{
		// Log the error code
		resultTmpBuf.Append(KTEFResultUnknown);
		resultTmpBuf.AppendFormat(KCode,(TInt)aResult);
		}
	ptr.Append(resultTmpBuf);
	simplifiedPtr.Append(resultTmpBuf);
	
	ptr.Append(KTEFSpace);
	simplifiedPtr.Append(KTEFSpace);
	ptr.Append(KTEFFontEnd);

	HBufC8* resultBuffer8 = HBufC8::NewLC(ptr.Length()+2);
	TPtr8 ptr8(resultBuffer8->Des());
	ptr8.Copy(ptr);
	ptr8.Append(KTEFEndOfLine);

	// Only log the START_TEST_BLOCK command to the results file if it has failed
	//  and therefore the END_TEST_BLOCK command may not be logged.
	if( 0 != command.Compare(KTEFStartTestBlock) ||
		(0 == command.Compare(KTEFStartTestBlock) && EPass != aResult) )
		{
		LoggingTestCaseResultToSummaryResultL(iHtmlLogPath,KTEFTestExecuteResultSummaryFile,ptr8);
		//re-use result buffer
		ptr8.Zero();
		ptr8.Copy(simplifiedPtr);
		ptr8.Append(KTEFEndOfLine);
		LoggingTestCaseResultToSummaryResultL(iHtmlLogPath,KTEFTestExecuteResultSimplifiedSummaryFile,ptr8);
		}
		
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		{
		TInt bytesCopied = 0;

		while(bytesCopied < ptr.Length())
			{
			TInt bytesToCopy = ptr.Length() - bytesCopied;
			if(bytesToCopy > KMaxTestExecuteLogLineLength)
				{
				bytesToCopy = KMaxTestExecuteLogLineLength;
				}
			TPtrC tmp(&ptr[bytesCopied],bytesToCopy);
			bytesCopied += bytesToCopy;
			HtmlLogger().Write(tmp);
			if(iPIPSExists)
				{
				PIPSLogger().Write(tmp);
				}
			}
		}

	CleanupStack::PopAndDestroy(resultBuffer8);
	CleanupStack::PopAndDestroy(simplifiedResultBuf);
	CleanupStack::PopAndDestroy(resultBuffer);

	// Construct tags for XML logging
	if (iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth)
		{
		_LIT(KTimeout,"TIMEOUT");
		_LIT(KProgramName,"PROGRAMNAME");
		_LIT(KServerName,"SERVERNAME");
		_LIT(KStepName,"TESTSTEPNAME");
		_LIT(KIniFile,"INIFILENAME");
		_LIT(KTEFSection,"SECTIONNAME");
		_LIT(KEResult,"EXPECTEDRESULT");
		_LIT(KAResult,"ACTUALRESULT");
		_LIT(KEPanicCode,"EXPECTEDPANICCODE");
		_LIT(KAPanicCode,"PANICCODE");
		_LIT(KEPanicString,"EXPECTEDPANICSTRING");
		_LIT(KAPanicString,"PANICSTRING");
		_LIT(KEErrorCode,"EXPECTEDERRORCODE");
		_LIT(KAErrorCode,"ERRORCODE");
		_LIT(KHeapSize,"HEAPSIZE");
		_LIT(KMaximumHeapSize,"0x100000");

		TPtrC panicCodeExpected;
		TPtrC panicStringExpected;
		
		TBuf<KMaxTestExecuteCommandLength> taskHeapSize(KMaximumHeapSize);
		
		TBuf<KMaxTestExecuteCommandLength> fieldName;
		TBuf<KMaxTestExecuteCommandLength> fieldValue;
		TBuf8<KMaxTestExecuteCommandLength> string1;
		TBuf8<KMaxTestExecuteCommandLength> string2;
		
		TPtrC commandName(xmlLogCommandParseLex.NextToken()); // Collect the command name from current script line

		if(commandName == KTEFRunTestStepResultCommand)
			{
			// Mark the position to the timeout token within the current script line
			xmlLogCommandParseLex.NextToken();
			xmlLogCommandParseLex.SkipSpace();
			xmlLogCommandParseLex.Mark();
			}

		else if(commandName == KTEFRunPanicStepResultCommand)
			{
			// Mark the position to the timeout token within the current script line
			// Also retrieve the expected panic code & strings
			panicCodeExpected.Set(xmlLogCommandParseLex.NextToken());
			panicStringExpected.Set(xmlLogCommandParseLex.NextToken());
			xmlLogCommandParseLex.SkipSpace();
			xmlLogCommandParseLex.Mark();
			}
		else if(commandName == KTEFRunTestStepCommand || commandName == KTEFEndTestBlock || commandName == KTEFStartTestBlock )
			{
			// Mark the position to the timeout token within the current script line
			// The !TEF parameters are to be neglected if there are any
			TInt firstChar;
			TPtrC commandStr;

			while(!xmlLogCommandParseLex.Eos())
				{
				xmlLogCommandParseLex.SkipSpace();
				xmlLogCommandParseLex.Mark();			
				commandStr.Set(xmlLogCommandParseLex.NextToken());
				if( commandStr.Length()>0 )
					{
					firstChar = commandStr[0];
					// 33 is the ascii value for "!". Used here for confirming switches
					if (firstChar != KTEFAsciiExclamation)
						{
						break;
						}
					else
						{
						if (commandStr.Length() >= KTEFMinHeapParamLength && commandStr.Mid(0,KTEFMinHeapParamLength).CompareF(KTEFHeap) == KTEFZeroValue)
							taskHeapSize.Copy(commandStr.Mid(KTEFMinHeapParamLength));
						}
					}
				else
					{
					break;
					}
				}
			}
	
		TLex lexTimeout;
		// We need to skip the timeout if it's there.
		if(commandName == KTEFRunTestStepCommand || commandName == KTEFStartTestBlock)
			lexTimeout=xmlLogCommandParseLex.MarkedToken();
		else
			lexTimeout=xmlLogCommandParseLex.NextToken();
		TInt timeout;
		TPtrC serverName;

		// Extract the timeout value and server name
		if(lexTimeout.Val(timeout) != KErrNone)
			{
			if (commandName == KTEFRunProgramCommand)
				{
				serverName.Set(lexTimeout.NextToken());
				}
			else
				{
				// No timeout so use the second token
				serverName.Set(xmlLogCommandParseLex.MarkedToken());
				}
			const TInt KDefaultTimeoutSeconds=120;
			timeout=KDefaultTimeoutSeconds;
			}
		else
			{
			// Timeout value there
			serverName.Set(xmlLogCommandParseLex.NextToken());
			}
			
		TBuf<KMaxTestExecuteCommandLength> timeoutString;
		timeoutString.AppendNum(timeout); // Convert the integral timeout to its equivalent string
		TPtrC stepName;
		if( commandName != KTEFStartTestBlock && commandName != KTEFEndTestBlock )
			{
			stepName.Set( xmlLogCommandParseLex.NextToken() ); // Extract the step name
			}
		TPtrC iniName(xmlLogCommandParseLex.NextToken()); // Extract the ini file name
		TPtrC sectionName(xmlLogCommandParseLex.NextToken()); // Extract the section name
		
		TExtraLogField logField[15];
		TInt index = 0;
		
		if( commandName != KTEFEndTestBlock )
			{
			logField[index].iLogFieldName.Copy(KTimeout);
			logField[index].iLogFieldValue.Copy(timeoutString);
			index++;
			}

		if(commandName == KTEFRunProgramCommand || commandName == KTEFRunWSProgramCommand)
			{
			logField[index].iLogFieldName.Copy(KProgramName);
			logField[index].iLogFieldValue.Copy(serverName);
			index++;
			}
		else if( commandName != KTEFEndTestBlock )
			{
			logField[index].iLogFieldName.Copy(KServerName);
			logField[index].iLogFieldValue.Copy(serverName);
			index++;
			}

		if (commandName != KTEFRunProgramCommand && commandName != KTEFRunWSProgramCommand &&
			commandName != KTEFEndTestBlock && commandName != KTEFStartTestBlock )
			{
			logField[index].iLogFieldName.Copy(KStepName);
			logField[index].iLogFieldValue.Copy(stepName);
			index++;

			logField[index].iLogFieldName.Copy(KTEFSection);
			logField[index].iLogFieldValue.Copy(sectionName);
			index++;
			}
		
		if (commandName != KTEFRunProgramCommand && commandName != KTEFRunWSProgramCommand &&
			commandName != KTEFEndTestBlock )
			{
			logField[index].iLogFieldName.Copy(KIniFile);
			logField[index].iLogFieldValue.Copy(iniName);
			index++;
			}
			
		if (commandName == KTEFRunTestStepResultCommand)
			{
			fieldValue.Copy(KNull);
			fieldValue.AppendNum(expectedReturnCode);
			logField[index].iLogFieldName.Copy(KEResult);
			logField[index].iLogFieldValue.Copy(fieldValue);
			index++;

			fieldValue.Copy(KNull);
			fieldValue.AppendNum(actualResult);
			logField[index].iLogFieldName.Copy(KAResult);
			logField[index].iLogFieldValue.Copy(fieldValue);
			index++;
			}

		if (commandName == KTEFRunPanicStepResultCommand || commandName == KTEFEndTestBlock )
			{
			if (defaultStr.Length()) // we have indeed panicked
				{
				fieldValue.Copy(KNull);
				fieldValue.AppendNum(expectedPanicCode);
				logField[index].iLogFieldName.Copy(KEPanicCode);
				logField[index].iLogFieldValue.Copy(fieldValue);
				index++;

				fieldValue.Copy(KNull);
				fieldValue.AppendNum(actualPanic);
				logField[index].iLogFieldName.Copy(KAPanicCode);
				logField[index].iLogFieldValue.Copy(fieldValue);
				index++;

				if (expectedPanicCode == actualPanic)
					{
					fieldValue.Copy(expectedPanicStringLex);
					logField[index].iLogFieldName.Copy(KEPanicString);
					logField[index].iLogFieldValue.Copy(fieldValue);
					index++;

					fieldValue.Copy(defaultStr);
					logField[index].iLogFieldName.Copy(KAPanicString);
					logField[index].iLogFieldValue.Copy(fieldValue);
					index++;
					}
				}
			}
		
		if (commandName == KTEFRunErrorStepResultCommand)
			{
			if (errorString.Length()) // Error has occured
				{
				logField[index].iLogFieldName.Copy(KEErrorCode);
				logField[index].iLogFieldValue.Copy(tefError);
				index++;
				
				logField[index].iLogFieldName.Copy(KAErrorCode);
				logField[index].iLogFieldValue.Copy(errorString);
				index++;
				}
			}
			
		logField[index].iLogFieldName.Copy(KHeapSize);
		logField[index].iLogFieldValue.Copy(taskHeapSize);
		index++;

		if ((defaultStr.Length()) && showPanicResult)
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultPanic);
			index++;

			_LIT(KTEFPanicString, "PANICSTRING");
			logField[index].iLogFieldName.Copy(KTEFPanicString);
			logField[index].iLogFieldValue.Copy(defaultStr);
			index++;

			_LIT(KTEFPanicCode, "PANICCODE");
			logField[index].iLogFieldName.Copy(KTEFPanicCode);
			logField[index].iLogFieldValue.AppendNum(aResult);
			index++;
			}
		else if(aResult == EPass)
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultPass);
			index++;
			}
		else if(aResult == EFail)
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultFail);
			index++;
			}
		else if(aResult == EAbort || aResult == KErrAbort)
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultAbort);
			index++;
			}
		else if (aResult == EIgnore)
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultUnexecuted);
			index++;
			}
		else if(aResult == EInconclusive)
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultInconclusive);
			index++;
			}
		else
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			TBuf<KMaxTestExecuteNameLength> fieldValue(KTEFResultUnknown);
			fieldValue.Append(KTEFSpaceEquals);
			fieldValue.AppendNum(aResult);
			logField[index].iLogFieldValue.Copy(fieldValue);
			index++;
			}

		// Log the commmand name along with field-value pairs
		XmlLogger().Log(((TText8*)__FILE__), aScriptLineNumber, RFileFlogger::TLogSeverity(aSeverity), index, logField, KTEFStringFormat, &commandName);
		}
	}

/**
 * @param aResult - Result of execution of test steps for logging
 * @param aPanicString - Reference descriptor containing the panic string if the test step results in a panic
 * @param aScriptLineNumber - Line number within the script file where the RUN_TEST_STEP command is available
 * @param aCommand - Command name being being executed (RUN_TEST_STEP & variants)
 * @param aScriptFile - Name of the script file being used for execution
 * @param aSeverity - One of the 4 enum values indicating the severity of the message to be logged
 * Responsible for logging results for the RUN_TEST_STEP command/variants
 */
EXPORT_C void CTestExecuteLogger::LogResult(TVerdict aResult, const TDesC& aPanicString, TInt aScriptLineNumber,const TDesC& aCommand,const TDesC& aScriptFile, TInt aSeverity)
	{
	if((ESerial == iLoggerChannel) || (EBoth == iLoggerChannel  ))
		{
		TRAP_IGNORE( LogResultSerialL(aResult, aPanicString, aScriptLineNumber,aCommand,aScriptFile) );
		}
	if(  (EFile == iLoggerChannel ) 
		|| (EBoth== iLoggerChannel ) )
		{
		TRAP_IGNORE( LogResultFileL(aResult, aPanicString, aScriptLineNumber,aCommand,aScriptFile, aSeverity) ); 
		}
	}

EXPORT_C TVerdict CTestExecuteLogger::LogBlock( TTEFItemArray* aItemArray, const TInt aScriptLineNumber )
	{
	TVerdict	result = EPass;
	
	// HTML specific block logging
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		{
		result = LogHTMLBlockL( *aItemArray );
		}

	// XML specific block logging
	if (iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth)
		{
		result = LogXMLBlock( *aItemArray, aScriptLineNumber );
		}

	return result;
	}

TVerdict CTestExecuteLogger::LogHTMLBlockL( const TTEFItemArray& aItemArray )
	{
	TVerdict		result = EPass;
	TInt			numCommands = aItemArray.Count();
	
	// Iterate through the item array and verify all of the error codes
	for( TInt i = 0 ; i<numCommands ; i++ )
		{
		TVerdict	err = EPass;
		// Check the error code is correct
		TBuf<KMaxTestExecuteCommandLength*2> blockLine;

		// Log the block command
		// Set the correct coolour for the HTML
		if( !aItemArray.At(i).iExecuted )
			{
			err = EInconclusive;
			blockLine += KTEFFontBlue;
			}
		else if( aItemArray.At(i).iError != aItemArray.At(i).iExpectedError
				 || aItemArray.At(i).iAsyncError != aItemArray.At(i).iExpectedAsyncError )
			{
			err = EFail;
			blockLine += KTEFFontRed;
			}
		else
			{
			blockLine += KTEFFontGreen;
			}

		switch( aItemArray.At(i).iItemType )
			{
			case ETEFCreateObject:
				{
				blockLine += KTEFCreateObject;
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iObjectType;
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iSection;
				}
				break;
			case ETEFRestoreObject:
				{
				blockLine += KTEFRestoreObject;
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iObjectType;
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iSection;
				}
				break;
			case ETEFCommand:
				{
				blockLine += KTEFCommand;
				blockLine += KTEFSpace;
				blockLine += KTEFError;
				blockLine.AppendNum( aItemArray.At(i).iExpectedError );
				blockLine += KTEFSpace;
				blockLine += KTEFAsyncError;
				blockLine.AppendNum( aItemArray.At(i).iExpectedAsyncError );
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iCommand.iObject;
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iCommand.iFunction;
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iSection;
				}
				break;
			case ETEFStore:
				{
				blockLine += KTEFStore;
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iSection;
				}
				break;
			case ETEFOutstanding:
				{
				blockLine += KTEFOutstanding;
				blockLine += KTEFSpace;
				blockLine.AppendNum( aItemArray.At(i).iTime );
				blockLine += KTEFSpace;
				blockLine += aItemArray.At(i).iSection;
				}
				break;
			case ETEFDelay:
				{
				blockLine += KTEFDelay;
				blockLine += KTEFSpace;
				blockLine.AppendNum( aItemArray.At(i).iTime );
				}
				break;
			case ETEFAsyncDelay:
				{
				blockLine += KTEFAsyncDelay;
				blockLine += KTEFSpace;
				blockLine.AppendNum( aItemArray.At(i).iTime );
				}
				break;		
			case ETEFSharedActiveScheduler:
				{
				blockLine += KTEFSharedActiveScheduler;
				}
				break;
			case ETEFStoreActiveScheduler:
				{
				blockLine += KTEFStoreActiveScheduler;
				}
				break;

			default:
				{
				blockLine += _L("Bad Command.");
				result = EFail;
				err = EFail;
				}
				break;
			}
		
		// Append an extra space for clearer logging.
		blockLine += KTEFSpace;
		
		// Log the block command
		if( !aItemArray.At(i).iExecuted )
			{
			err = EInconclusive;
			// If a command has failed then the overall block has failed
			result = EFail;
			}
		else if( aItemArray.At(i).iError != aItemArray.At(i).iExpectedError
				 || aItemArray.At(i).iAsyncError != aItemArray.At(i).iExpectedAsyncError )
			{
			blockLine += KTEFErrorResult;
			blockLine.AppendNum(aItemArray.At(i).iError);
			blockLine += KTEFAsyncErrorResult;
			blockLine.AppendNum(aItemArray.At(i).iAsyncError);
			err = EFail;
			// If a command has failed then the overall block has failed
			result = EFail;
			}

		// Append the result
		blockLine += KTEFSpace;
		blockLine += KTEFResultTag;
		blockLine += KTEFSpace;
		blockLine += KTEFEquals;
		blockLine += KTEFSpace;
		if( EPass == err )
			{
			blockLine += KTEFResultPass;
			}
		else if( EFail == err )
			{
			blockLine += KTEFResultFail;
			}
		else
			{
			blockLine += KTEFResultInconclusive;
			}

		// Terminate the html line
		blockLine += KTEFSpace;
		blockLine += KTEFFontEnd;

		HBufC8* resultBuffer8 = HBufC8::NewLC(blockLine.Length()+2);
		TPtr8 ptr8(resultBuffer8->Des());
		ptr8.Copy(blockLine);

		// Write to the log file
		HtmlLogger().Write(ptr8);
		if(iPIPSExists)
			{
			PIPSLogger().Write(ptr8);
			}
		CleanupStack::PopAndDestroy(resultBuffer8);
		}
		
	return result;
	}
	
TVerdict CTestExecuteLogger::LogXMLBlock( const TTEFItemArray& aItemArray, const TInt aScriptLineNumber )
	{
	TVerdict		result = EPass;
	TInt			numCommands = aItemArray.Count();
	
	for( TInt i = 0 ; i<numCommands ; i++ )
		{
		TInt index = 0;
		TExtraLogField			logField[8];
		TBuf<KTEFMaxNameLength>	command;
		
		switch( aItemArray.At(i).iItemType )
			{
			case ETEFCreateObject:
				{
				command = KTEFCreateObject;
				logField[index].iLogFieldName.Copy(KObjectType);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iObjectType);
				index++;
				logField[index].iLogFieldName.Copy(KObjectSection);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iSection);			
				index++;
				}
				break;
			case ETEFRestoreObject:
				{
				command = KTEFRestoreObject;
				logField[index].iLogFieldName.Copy(KObjectType);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iObjectType);
				index++;
				logField[index].iLogFieldName.Copy(KObjectSection);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iSection);			
				index++;
				}
				break;
			case ETEFCommand:
				{
				command = KTEFCommand;
				logField[index].iLogFieldName.Copy(KEErrorCode);
				logField[index].iLogFieldValue.AppendNum(aItemArray.At(i).iExpectedError);
				index++;
				logField[index].iLogFieldName.Copy(KEAsyncErrorCode);
				logField[index].iLogFieldValue.AppendNum(aItemArray.At(i).iExpectedAsyncError);
				index++;
				logField[index].iLogFieldName.Copy(KErrorCode);
				logField[index].iLogFieldValue.AppendNum(aItemArray.At(i).iError);
				index++;
				logField[index].iLogFieldName.Copy(KAsyncErrorCode);
				logField[index].iLogFieldValue.AppendNum(aItemArray.At(i).iAsyncError);
				index++;
				logField[index].iLogFieldName.Copy(KObjectSection);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iCommand.iObject);
				index++;
				logField[index].iLogFieldName.Copy(KFunctionName);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iCommand.iFunction);
				index++;
				logField[index].iLogFieldName.Copy(KSection);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iSection);
				index++;
				}
				break;
			case ETEFStore:
				{
				command = KTEFStore;
				logField[index].iLogFieldName.Copy(KObjectSection);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iSection);
				index++;
				}
				break;
			case ETEFOutstanding:
				{
				command = KTEFOutstanding;
				logField[index].iLogFieldName.Copy(KInterval);
				logField[index].iLogFieldValue.AppendNum(aItemArray.At(i).iTime);
				index++;
				logField[index].iLogFieldName.Copy(KSection);
				logField[index].iLogFieldValue.Copy(aItemArray.At(i).iSection);
				index++;
				}
				break;
			case ETEFDelay:
				{
				command = KTEFDelay;
				logField[index].iLogFieldName.Copy(KDelay);
				logField[index].iLogFieldValue.AppendNum(aItemArray.At(i).iTime);
				index++;
				}
				break;
			case ETEFAsyncDelay:
				{
				command = KTEFAsyncDelay;
				logField[index].iLogFieldName.Copy(KDelay);
				logField[index].iLogFieldValue.AppendNum(aItemArray.At(i).iTime);
				index++;
				}
				break;
			default:
				{
				command = KBadCommand;
				}
				break;
			}

		// Set the correct result for the XML
		if( !aItemArray.At(i).iExecuted )
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultInconclusive);
			index++;
			// If a command has failed then the overall block has failed
			result = EFail;
			}
		else if( aItemArray.At(i).iError != aItemArray.At(i).iExpectedError
				 || aItemArray.At(i).iAsyncError != aItemArray.At(i).iExpectedAsyncError )
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultFail);
			index++;
			// If a command has failed then the overall block has failed
			result = EFail;
			}
		else
			{
			logField[index].iLogFieldName.Copy(KTEFResultString);
			logField[index].iLogFieldValue.Copy(KTEFResultPass);
			index++;
			}
	
		// Call the Xml log client interface for logging into xml log
		// Log the commmand name along with field-value pairs
		XmlLogger().Log(((TText8*)__FILE__), aScriptLineNumber, RFileFlogger::ESevrHigh, index, logField, KTEFStringFormat, &command);
		}
	
	return result;
	}

/**
 * @param aLogMode - Integer descriptor containing either of the 3 enum values
 *                   ELogHTMLOnly(0)/ELogXMLOnly(1)/ELogBoth(2)
 * Responsible for setting the logger options for the logger object based on user request
 */
EXPORT_C void CTestExecuteLogger::SetLoggerOptions(TInt aLogMode)
	{
	iLoggerOptions = TLoggerOptions(aLogMode);
	}

/**
 * @param aLogChannel - Integer descriptor containing either of the 3 enum values
 * Responsible for setting the logger Channel 
 */
EXPORT_C void CTestExecuteLogger::SetLoggerChannel(TInt aLogChannel)
	{
	iLoggerChannel = aLogChannel ; 
	}

/**
 * @return - System wide errors
 * Connects to the HTML & XML logger client sessions
 */
EXPORT_C TInt CTestExecuteLogger::Connect()
	{
	TInt err = KErrNone;
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		{
		// Call the Html log client interface for logging into html log
		err = HtmlLogger().Connect();
		}

	if ((iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth) && err == KErrNone)
		{
		err = XmlLogger().Connect();
		}
	
	//TRAPD(errPIP, PIPSLogger().Connect()) ;
	TInt errPIP = PIPSLogger().Connect() ; 
	iPIPSExists = (errPIP == KErrNone)?  true : false ; 

	return err;
	}

/**
 * Closes the connection with HTML & XML logger client sessions
 */
EXPORT_C void CTestExecuteLogger::Close()
	{

	if(iLoggerChannel == ESerial || iLoggerChannel == EBoth)
		{
		//iSerialWriter->UnlockResources();
		delete iSerialWriter;
		iSerialWriter = NULL;
		}	
	if (iLoggerOptions == ELogHTMLOnly || iLoggerOptions == ELogBoth)
		{
		// defect 116046
  		THandleInfo info;
  		HtmlLogger().HandleInfo(&info);
  		if (0 != info.iNumOpenInThread)
  			{
  			HtmlLogger().Close();
  			}
  		// END defect 116046
		}
	if (iLoggerOptions == ELogXMLOnly || iLoggerOptions == ELogBoth)
		{
		XmlLogger().Close();
		}
	if(iPIPSExists)
		{
		PIPSLogger().Close();
		}
	}

/**
 * Two phase Construction
 */
EXPORT_C CTestExecuteLogger* CTestExecuteLogger::NewL(TLoggerOptions aLogOptions)
	{
	CTestExecuteLogger* self = CTestExecuteLogger::NewLC(aLogOptions);
	CleanupStack::Pop();
	return self;
	}

/**
 * Two phase Construction
 */
EXPORT_C CTestExecuteLogger* CTestExecuteLogger::NewLC(TLoggerOptions aLogOptions)
	{
	CTestExecuteLogger* self = new(ELeave) CTestExecuteLogger(aLogOptions);
	CleanupStack::PushL(self);
	//self->ConstructL(); //if in future the contructor is expected to do anything.
	return self;
	}

//////////////////////////////////////////////////////
// Methods implementation for CTestExecuteIniData used
// For parsing the testexecute.ini
//////////////////////////////////////////////////////

/**
 * Constructor
 */
EXPORT_C CTestExecuteIniData::CTestExecuteIniData() : 
iHtmlLogPath(KNull),
iXmlLogPath(KNull),
iDefaultScriptDirectory(KNull),
iLogSeverity(RFileFlogger::ESevrAll),
iLoggerOptions(TLoggerOptions(ELogHTMLOnly)),
iJustInTime(KTEFZeroValue),
iRemotePanicDetection(KTEFZeroValue),
iEnableIniAccessLog(KTEFOneValue),
iEnableTestsCountLog(KTEFOneValue),
iEnableSystemStarter(KTEFOneValue),
iLoggerChannel(TLoggerChannels(EFile)),
iPortNumber(KTEFZeroValue),
iDefaultSysDrive(KTEFLegacySysDrive),
iIniSysDriveName(KTEFIniSysDrive)
	{
	iConfigData = NULL;
	}

/**
 * Two phase Construction
 */
EXPORT_C CTestExecuteIniData* CTestExecuteIniData::NewL()
	{
	CTestExecuteIniData* self = CTestExecuteIniData::NewLC();
	CleanupStack::Pop();
	return self;
	}

/**
 * Two phase Construction
 */
EXPORT_C CTestExecuteIniData* CTestExecuteIniData::NewLC()
	{
	CTestExecuteIniData* self = new(ELeave) CTestExecuteIniData();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CTestExecuteIniData::CTestExecuteIniData(TDriveName& aSysDrive) : 
iHtmlLogPath(KNull),
iXmlLogPath(KNull),
iDefaultScriptDirectory(KNull),
iLogSeverity(RFileFlogger::ESevrAll),
iLoggerOptions(TLoggerOptions(ELogHTMLOnly)),
iJustInTime(KTEFZeroValue),
iRemotePanicDetection(KTEFZeroValue),
iEnableIniAccessLog(KTEFOneValue),
iEnableTestsCountLog(KTEFOneValue),
iEnableSystemStarter(KTEFOneValue),
iLoggerChannel(TLoggerChannels(EFile)),
iPortNumber(KTEFZeroValue),
iDefaultSysDrive(aSysDrive),
iIniSysDriveName(KTEFIniSysDrive)
	{
	iConfigData = NULL;
	}

/**
 * Two phase Construction
 */
EXPORT_C CTestExecuteIniData* CTestExecuteIniData::NewL(TDriveName& aSysDrive)
	{
	CTestExecuteIniData* self = CTestExecuteIniData::NewLC(aSysDrive);
	CleanupStack::Pop();
	return self;
	}

/**
 * Two phase Construction
 */
EXPORT_C CTestExecuteIniData* CTestExecuteIniData::NewLC(TDriveName& aSysDrive)
	{
	CTestExecuteIniData* self = new(ELeave) CTestExecuteIniData(aSysDrive);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * ConstructL
 */
EXPORT_C void CTestExecuteIniData::ConstructL()
	{
	TBuf<KMaxTestExecuteNameLength> iniDefaultPath(KTEFDefaultPath);
	iniDefaultPath.Replace(0, 2, iDefaultSysDrive);
	TRAPD(err,iConfigData = CIniData::NewL(iniDefaultPath, iDefaultSysDrive));
	if (err != KErrNone)
		{
		TRAP(err,iConfigData = CIniData::NewL(KTEFAlternatePath, iDefaultSysDrive));
		if (err != KErrNone)
			{
			User::Leave(err);
			}
		}
	}

/**
 * Destructor
 */
EXPORT_C CTestExecuteIniData::~CTestExecuteIniData()
	{
	if (iConfigData != NULL)
		{
		delete iConfigData;
		}
	}

/**
 * Extracts the values for all the keys within testexecute.ini
 * And stores it into member variables for the class
 */
EXPORT_C void CTestExecuteIniData::ExtractValuesFromIni()
	{
	TBuf<KMaxTestExecuteNameLength> testExecuteLogPath(KTestExecuteLogPath);
	testExecuteLogPath.Replace(0, 2, iDefaultSysDrive);

	if (!iConfigData->FindVar(KTEFSection, KTEFHtmlKey, iHtmlLogPath))
		{
		iHtmlLogPath.Set(testExecuteLogPath);
		}
	if (iHtmlLogPath.Compare(KNull) == 0)
		{
		iHtmlLogPath.Set(testExecuteLogPath);
		}
	if (!iConfigData->FindVar(KTEFSection, KTEFXmlKey, iXmlLogPath))
		{
		iXmlLogPath.Set(testExecuteLogPath);
		}
	if (iXmlLogPath.Compare(KNull) == 0)
		{
		iXmlLogPath.Set(testExecuteLogPath);
		}
	if (!iConfigData->FindVar(KTEFSection, KTEFDefaultSysDrive, iIniSysDriveName))
		{
		iIniSysDriveName.Set(KTEFIniSysDrive);
		}
	TPtrC stringValue;
	if (iConfigData->FindVar(KTEFSection, KTEFDefaultScriptPath,stringValue))
		{
		iDefaultScriptDirectory.Set(stringValue);
		}
	TInt intValue = KTEFZeroValue;
	if (iConfigData->FindVar(KTEFSection, KTEFJustInTimeDebug,intValue))
		{
		iJustInTime = intValue;
		}
	intValue = KTEFZeroValue;
	if (iConfigData->FindVar(KTEFSection, KTEFOutputPort,intValue))
		{
		iPortNumber = intValue;
		}	
	
	if (!iConfigData->FindVar(KTEFSection, KTEFWaitForLoggingTime, iWaitForLoggingTime))
		{
		iWaitForLoggingTime = 5;
		}
	
	if(iConfigData->FindVar(KTEFSection, KTEFLogSeverityKey, stringValue))
		{
		_LIT(KBasic,"BASIC");
		_LIT(KMedium,"MEDIUM");
		_LIT(KFull,"FULL");
		if (stringValue.CompareF(KBasic) == 0)
			iLogSeverity = RFileFlogger::ESevrHigh;
		else if (stringValue.CompareF(KMedium) == 0)
			iLogSeverity = RFileFlogger::ESevrMedium;
		else if (stringValue.CompareF(KFull) == 0)
			iLogSeverity = RFileFlogger::ESevrTEFUnit;
		else
			iLogSeverity = RFileFlogger::ESevrAll;
		}

	stringValue.Set(KNull);
	if(iConfigData->FindVar(KTEFSection, KTEFLogMode, stringValue))
		{
		_LIT(KLogBoth, "BOTH");
		_LIT(KLogXml, "XML");
		if (stringValue.CompareF(KLogBoth) == 0)
			iLoggerOptions = TLoggerOptions(ELogBoth);
		else if (stringValue.CompareF(KLogXml) == 0)
			iLoggerOptions = TLoggerOptions(ELogXMLOnly);
		else
			iLoggerOptions = TLoggerOptions(ELogHTMLOnly);
		}

	stringValue.Set(KNull);
	if(iConfigData->FindVar(KTEFSection, KTEFLogChannel, stringValue))
		{
		_LIT(KLogFile, "File");
		_LIT(KLogSerial, "Serial");
		if (stringValue.CompareF(KLogFile) == 0)
			iLoggerChannel = TLoggerChannels(EFile);
		else if (stringValue.CompareF(KLogSerial) == 0)
			iLoggerChannel = TLoggerChannels(ESerial);
		else
			iLoggerChannel = TLoggerChannels(EBoth);
		}

	stringValue.Set(KNull);
	if (iConfigData->FindVar(KTEFSection, KTEFRemotePanicDetection, stringValue))
		{
		_LIT(KPanicDetectionOn,"ON");
		_LIT(KPanicDetectionOne,"1");
		if (stringValue.CompareF(KPanicDetectionOn) == 0 || stringValue.CompareF(KPanicDetectionOne) == 0)
			iRemotePanicDetection = 1;
		else
			iRemotePanicDetection = 0;
		}
		
	stringValue.Set(KNull);
	if (iConfigData->FindVar(KTEFSection, KTEFEnableIniAccessLog, stringValue))
		{
		_LIT(KEnableIniAccessLogOff,"OFF");
		_LIT(KEnableIniAccessLogZero,"0");
		if (stringValue.CompareF(KEnableIniAccessLogOff) == 0 || stringValue.CompareF(KEnableIniAccessLogZero) == 0)
			iEnableIniAccessLog = 0;
		else
			iEnableIniAccessLog = 1;
		}

	stringValue.Set(KNull);
	if (iConfigData->FindVar(KTEFSection, KTEFEnableTestsCountLog, stringValue))
		{
		_LIT(KEnableTestsCountLogOff,"OFF");
		_LIT(KEnableTestsCountLogZero,"0");
		if (stringValue.CompareF(KEnableTestsCountLogOff) == 0 || stringValue.CompareF(KEnableTestsCountLogZero) == 0)
			iEnableTestsCountLog = 0;
		else
			iEnableTestsCountLog = 1;
		}

	stringValue.Set(KNull);
	if (iConfigData->FindVar(KTEFSection, KTEFSystemStarter, stringValue))
		{
		_LIT(KEnableSystemStarterOff,"OFF");
		_LIT(KEnableSystemStarterZero,"0");
		if (stringValue.CompareF(KEnableSystemStarterOff) == 0 || stringValue.CompareF(KEnableSystemStarterZero) == 0)
			iEnableSystemStarter = 0;
		else
			iEnableSystemStarter = 1;
		}

	}

/**
 * @param aKeyName - Reference to string descriptor containing the name of the key within ini
 * @param aValue - Reference to string descriptor to store the value for the key in return
 * Copies the value for the key name input to the reference aValue string descriptor passed in
 */
EXPORT_C void CTestExecuteIniData::GetKeyValueFromIni(const TDesC& aKeyName, TDes& aValue)
	{
	if (aKeyName.Compare(KTEFHtmlKey) == 0)
		{
		aValue.Copy(iHtmlLogPath);
		}
	else if (aKeyName.Compare(KTEFXmlKey) == 0)
		{
		aValue.Copy(iXmlLogPath);
		}
	else if (aKeyName.Compare(KTEFDefaultScriptPath) == 0)
		{
		aValue.Copy(iDefaultScriptDirectory);
		}
	else if (aKeyName.Compare(KTEFDefaultSysDrive) == 0)
		{
		aValue.Copy(iIniSysDriveName);
		}
	else
		{
		aValue.Copy(KNull);
		}
	}

/**
 * @param aKeyName - Reference to string descriptor containing the name of the key within ini
 * @param aValue - Reference to integer descriptor to store the value for the key in return
 * Copies the value for the key name input to the reference aValue integer descriptor passed in
 */
EXPORT_C void CTestExecuteIniData::GetKeyValueFromIni(const TDesC& aKeyName, TInt& aValue)
	{
	if (aKeyName.Compare(KTEFLogSeverityKey) == 0)
		{
		aValue = iLogSeverity;
		}
	else if (aKeyName.Compare(KTEFLogMode) == 0)
		{
		aValue = iLoggerOptions;
		}
	else if (aKeyName.Compare(KTEFLogChannel) == 0)
		{
		aValue = iLoggerChannel;
		}
	else if (aKeyName.Compare(KTEFOutputPort) == 0)
		{
		aValue = iPortNumber;
		}
	else if (aKeyName.Compare(KTEFJustInTimeDebug) == 0)
		{
		aValue = iJustInTime;
		}
	else if (aKeyName.Compare(KTEFRemotePanicDetection) == 0)
		{
		aValue = iRemotePanicDetection;
		}
	else if (aKeyName.Compare(KTEFEnableIniAccessLog) == 0)
		{
		aValue = iEnableIniAccessLog;
		}
	else if (aKeyName.Compare(KTEFEnableTestsCountLog) == 0)
		{
		aValue = iEnableTestsCountLog;
		}
	else if (aKeyName.Compare(KTEFSystemStarter) == 0)
		{
		aValue = iEnableSystemStarter;
		}
	else if (aKeyName.CompareF(KTEFWaitForLoggingTime) == 0)
		{
		aValue = iWaitForLoggingTime;
		}
	else
		{
		aValue = KTEFZeroValue;
		}
	}

/**
 * @param aCommandStr - Current script command line, this string started form !PanicString.
 * @param aScriptLineParamLex - Current script line Lexer. This parameter will be used when exit this function, 
 * 								so aScriptLineParamLex must moved to the next parameter.
 * @param aTEFPanicString - The parse result . return the string in after !PanicString= .
 * 							EX1: !PanicString=STRING 			restult is:STRING, return ETrue
 * 							EX2: !PanicString="STRING" 			restult is:STRING, return ETrue
 * 							EX3: !PanicString="STRING panic" 	restult is:STRING panic, return ETrue
 * 							EX4: !PanicString=STRING panic		restult is:STRING, return EFalse
 * 							EX5: !PanicString="STRING panic 	restult is:STRING panic, return EFalse
 * @return - parse result of the funtion.
 * 	protected:
 * 	This fuction is being added for DEF120600.
 */
TBool CTestExecuteLogger::ParsePaincStringFromScriptCommandLine(const TDesC& aCommandStr,TLex& aScriptLineParamLex,TDes& aTEFPanicString)
	{
	TInt err(0);
	TBool panicStringCompleted(ETrue);// if the string are complexed within "" or just one word, this flag will be ture
	TRAP(err,aTEFPanicString.Copy(aCommandStr.Mid(KTEFMinPanicStringParamLength)));	
	if (err)
		{
		aTEFPanicString.Copy(KNull);
		panicStringCompleted = EFalse;
		}
	else
		{
		if (aTEFPanicString.Left(1) == KTEFOpenQuotes)
			{
			panicStringCompleted = EFalse;
			if (aTEFPanicString.Right(1) != KTEFOpenQuotes)
				{
				aTEFPanicString.Copy(aTEFPanicString.Mid(1,aTEFPanicString.Length()-1));
				while(!aScriptLineParamLex.Eos() && !panicStringCompleted)
					{
					aScriptLineParamLex.Mark();
					TPtrC spaceBegin( aScriptLineParamLex.RemainderFromMark() ); 
					TPtrC panicStringExt(aScriptLineParamLex.NextToken());
					TInt span = spaceBegin.Find(panicStringExt);
					
					// append the white space to the end of the expected panic string
					if (aTEFPanicString.MaxLength() >= aTEFPanicString.Length() + span )
						{
						aTEFPanicString.Append(spaceBegin.Left(span).Ptr(),span);// append write spaces to the panic string 
						}
					else
						{
						// this string too large
						panicStringCompleted = EFalse;
						break;
						}
					if(panicStringExt.Right(1) == KTEFOpenQuotes)
						{
						aTEFPanicString.Append(panicStringExt.Mid(0, panicStringExt.Length()-1));
						panicStringCompleted = ETrue;
						}
					else
						{
						aTEFPanicString.Append(panicStringExt);
						}
					}
				}
			else
				{
				aTEFPanicString.Copy(aTEFPanicString.Mid(1,aTEFPanicString.Length()-2));
				panicStringCompleted = ETrue;
				}
			}
		if (aTEFPanicString.Length()>KTEFMaxPanicStringLength)
			{
			// warnning panic string too long 
			LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, KTEFPanicStringTooLong, &KTEFPanicString, &aTEFPanicString  );
			LogExtra(((TText8*)__FILE__), __LINE__, ESevrWarn, KTEFPanicStringErrorInfo);
			}
		}
	return panicStringCompleted;
	}
