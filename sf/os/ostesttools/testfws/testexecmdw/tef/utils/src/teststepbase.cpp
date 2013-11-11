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
* Contains the CTestStep base class implementation
*
*/



/**
 @file TestStepBase.cpp
*/

#include "testexecutestepbase.h"

/**
 * Default virtual implementation for pre-processing  within test steps
 * Virtual - May be overidden by subclass
 * @return One of TVerdict enum values defined
 */
EXPORT_C enum TVerdict CTestStep::doTestStepPreambleL(void)
	{
	SetTestStepResult(EPass);
	SetTestStepError(0);
	return TestStepResult();
	}

/**
 * Default virtual implementation for post-processing within test steps
 * Virtual - May be overidden by subclass
 * @return One of TVerdict enum values defined
 */
EXPORT_C enum TVerdict CTestStep::doTestStepPostambleL(void)
	{
	return TestStepResult();
	}

/**
 * Sets the current heap's to fail upon the subsequent "aFailureIndex" attempt
 * Virtual - may be overridden by derived test step
 * @param aFailureIndex - Index within heap allocation table that needs to be failed
 */
EXPORT_C void CTestStep::SetHeapFailNext(TInt aFailureIndex)
	{
	if (aFailureIndex >= 0)
		{
		__UHEAP_FAILNEXT(aFailureIndex);
		}
	}

/**
 * Resets the the current heap's simulated "fail next" mode (if active)
 * Virtual - may be overridden by derived test step
 */
EXPORT_C void CTestStep::ResetHeapFailNext()
	{
	__UHEAP_RESET;
	}

/**
 * Tests whether the current heap is in "fail next" mode. Used to check whether 
 * the code-under-test has "consumed" the simulated failure when OOM looping.
 * Virtual - may be overridden by derived test step
 * @return Boolean true if code has comsumed a simulated failure, false otherwise
 */
EXPORT_C TBool CTestStep::CheckForHeapFailNext()
	{
	TBool finishedCorrectly(EFalse);
	TInt* myInt = new TInt;
	if (myInt == NULL)
		finishedCorrectly = ETrue; // NULL means no memory
	delete myInt;
	return finishedCorrectly;
	}

/**
 * Destructor
 * Only heap resource is the Ini file data
 */
EXPORT_C CTestStep::~CTestStep()
	{
	CloseLogger();
	delete iConfigData;
	delete iIniFileName;
	delete iLogger ; 
	}

/**
 * Constructor
 */
EXPORT_C CTestStep::CTestStep()
	{
	// current logger can't using to log infomation.
	// because SetLoggerL must called before write logs.
	// the initial operation codes at SetLoggerL()
	iLogger = new CTestExecuteLogger();
	}

/**
 * Called by server base code that loads the test step
 * Can be called multiple times for any loaded test step instance.
 * Sets up the instance variables
 * @param aStepArgs - The arguments to the RUN_TEST_STEP command
 * @param aServerName - Name of the server that requested initialisation of its test step object
 * @param aSharedDataMode - Boolean true or false indicating the mode in which the test step is running
 * @Leave system wide errors
 */
void CTestStep::InitialiseL(const TDesC& aStepArgs, const TDesC& aServerName, TBool& aSharedDataMode)
	{
	// Assume pass
	iTestStepResult = EPass;
	iSharedDataMode = aSharedDataMode;
	iSection.SetLength(0);
	// In the case of re-initialise we need to free the existing resource
	if(iIniFileName)
		{
		delete iIniFileName;
		iIniFileName = NULL;
		}
	if(iConfigData)
		{
		delete iConfigData;
		iConfigData = NULL;
		}

	TLex lex(aStepArgs);

	// Extract the first token for OOM request status
	TPtrC oomRequestString(lex.NextToken());
	_LIT(KTEFOomOff,"OOM=0");
	if (oomRequestString.CompareF(KTEFOomOff) == 0)
		{
		// Not required to run OOM tests if OOM=0
		iOOMRequestStatus = EFalse;
		}
	else
		{
		// Required to run OOM tests if OOM=1
		iOOMRequestStatus = ETrue;
		}

	// Extract the second token for !Setup value
	TLex setupParamValue(lex.NextToken().Mid(7));
	TInt err = setupParamValue.Val(iSetupParamValue);
	if (err != KErrNone)
		{
		iSetupParamValue = TTestSetupState(ESetupNone);
		}
	
	// Extract the third token from aStepArgs for loop index
	TLex loopIndex(lex.NextToken());
	loopIndex.Val(iLoopIndex);

	iTestCaseID.Copy(lex.NextToken());

	TPtrC path(lex.NextToken());
	iScriptDriveName.Copy(path);

	path.Set(lex.NextToken());
	iSystemDrive.Copy(path);

	path.Set(lex.NextToken());
	iSystemDriveForTest.Copy(path);
	
	// Set the logger instance
	//before SetLogger(). iSystemDrive is needed, because SetLogger() will using iSystemDrive
	//before LoadConfigFileL(). SetLogger() is needed, because LoadConfigFileL() may needs write logs.
	SetLoggerL();
	// Get the ini file name if any
	TPtrC configFile(lex.NextToken());
	if(configFile.Length())
		{
		LoadConfigFileL(configFile);
		TPtrC iniSection(lex.NextToken());
		// In the case of re-initialise zero the old one
		ConfigSection().SetLength(0);
		// Get the ini section name if any
		if(iniSection.Length() && iniSection.Length() <= ConfigSection().MaxLength())
			ConfigSection().Copy(iniSection);
		}
	// Set the inifile name.
	iIniFileName = configFile.AllocL();
	// Set the server name
	iServerName = aServerName;
	}

/**
 * Creates a Cinidata instance for the ini file path specified for a particular test step
 * @param aFilePath - Full pathname of the ini file
 * @Leave system wide errors
 */
void CTestStep::LoadConfigFileL(const TDesC& aFilePath)
	{
	if(!aFilePath.Length())
		User::Leave(KErrNotFound);

		// Pass the system drive letter finalised to the Cinidata construction
		// So, that the ini files can be expanded for the ${SYSDRIVE} variables
		TRAPD(err,iConfigData = CIniData::NewL(aFilePath, GetSystemDrive()));		
		if(err != KErrNone)
			{
			//defect 121613: TEF will report ini file too large
			ERR_PRINTF3(_L("Leave when loading %S . Error Code = %d"),&aFilePath,err);
			// This is a hit message to user to split ini file to smaller one
			if (err == KErrNoMemory ) // -4
				{
				_LIT(KTEFLOADINIFILENOENOUGHMEM,"The configuration file is too large that TEF don't have enough memory to load. Please consider split the configuration file into smaller one or load the file in your own test codes.");
				INFO_PRINTF1(KTEFLOADINIFILENOENOUGHMEM);
				}
			User::Leave(err);
			}
	}

/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TBool reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the boolean
 * @return TBool - ETrue for found, EFalse for not found 
 */
EXPORT_C TBool CTestStep::GetBoolFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TBool& aResult) 
	{
	TPtrC result;
	if(!iConfigData)
		return EFalse;
	if(!iConfigData->FindVar(aSectName, aKeyName, result))
		return EFalse;
	_LIT(KTrue,"true");
	if (result.FindF(KTrue) == KErrNotFound) 
		aResult = EFalse;
	else
		aResult = ETrue;

	if (iEnableIniAccessLog)
		{
		INFO_PRINTF5(KTEFIniReadInteger,iIniFileName, &aSectName, &aKeyName , aResult);
		}
	return(ETrue);
	}

/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TInt reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the integer
 * @return TBool - ETrue for found, EFalse for not found 
 */
EXPORT_C TBool CTestStep::GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult) 
	{
	if(!iConfigData)
		return EFalse;
	if(!iConfigData->FindVar(aSectName, aKeyName, aResult))
		return EFalse;
	if (iEnableIniAccessLog)
		{
		INFO_PRINTF5(KTEFIniReadInteger,iIniFileName, &aSectName, &aKeyName , aResult);
		}
	return ETrue;
	}
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TInt64 reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The value of the 64-bit integer
 * @return TBool - ETrue for found, EFalse for not found 
 */
EXPORT_C TBool CTestStep::GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt64& aResult)
	{
	if(!iConfigData)
		return EFalse;
	if(!iConfigData->FindVar(aSectName, aKeyName, aResult))
		return EFalse;
	if (iEnableIniAccessLog)
		{
		INFO_PRINTF5(KTEFIniReadInt64,iIniFileName, &aSectName, &aKeyName , aResult);
		}
	return ETrue;
	}
	
/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TPtrC reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - Reference to the string on the heap
 * @return TBool - ETrue for found, EFalse for not found 
 */
EXPORT_C TBool CTestStep::GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult) 
	{
	if(!iConfigData)
		return EFalse;
	if(!iConfigData->FindVar(aSectName, aKeyName, aResult))
		return EFalse;
	if (iEnableIniAccessLog)
		{
		INFO_PRINTF5(KTEFIniReadString,iIniFileName, &aSectName, &aKeyName , &aResult);
		}
	return ETrue;
	}

/**
 * Reads the value present from the test steps ini file within the mentioned section name and key name
 * Copies the value to the TInt reference passed in
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aResult - The integer value of the Hex input
 * @return TBool - ETrue for found, EFalse for not found 
 */	
EXPORT_C TBool CTestStep::GetHexFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TInt& aResult)
	{
	TPtrC result;
	if(!iConfigData)
		return EFalse;
	if(!iConfigData->FindVar(aSectName, aKeyName, result))
		return EFalse;
	TLex lex(result);
	TInt err = lex.Val((TUint &)aResult, EHex);
	if(err)
		return EFalse;
	if (iEnableIniAccessLog)
		{
		INFO_PRINTF5(KTEFIniReadInteger,iIniFileName, &aSectName, &aKeyName , aResult);
		}
	return(ETrue);
	}

/**
 * Writes the integer value  passed in to the test steps ini file under the section and key name mentioned
 * Creates a new section or key name if it does not existing already
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aValue - Reference to the integer to be written on to the test steps ini file
 * @return TBool - ETrue for found, EFalse for not found 
 */
EXPORT_C TBool CTestStep::WriteIntToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt& aValue)
	{
	TBuf<KMaxTestExecuteNameLength> value;
	TBool retVal(ETrue);
	TInt err(KErrNone);
	value.AppendNum(aValue);
			
	if(!iConfigData)
		{
		retVal = EFalse;
		}
	else if(iConfigData->SetValue(aSectName,aKeyName,value)==KErrNotFound)
		{
		err = iConfigData->AddValue(aSectName,aKeyName,value);
		}
	
	if(retVal && KErrNone==err)
		{
		TRAP(err, iConfigData->WriteToFileL());
		if (iEnableIniAccessLog)
			{
			if (KErrNone == err)
				{
				INFO_PRINTF5(KTEFIniWriteInteger,iIniFileName, &aSectName, &aKeyName , aValue);
				}
			}
		}

	if (!retVal || KErrNone!=err)
		{
		retVal = EFalse;
		if (iEnableIniAccessLog)
			{
			_LIT(KWriteIntegerError, "Error occurs when writing integer to file, error code=%d");
			ERR_PRINTF2(KWriteIntegerError, err);
			}
		}
	return retVal;
	}
/**
 * Writes the 64-bit integer value  passed in to the test steps ini file under the section and key name mentioned
 * Creates a new section or key name if it does not existing already
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aValue - Reference to the 64-bit integer to be written on to the test steps ini file
 * @return TBool - ETrue for found, EFalse for not found 
 */
EXPORT_C TBool CTestStep::WriteIntToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt64& aValue)
	{
	TBuf<KMaxTestExecuteNameLength> value;
	TBool retVal(ETrue);
	TInt err(KErrNone);
	value.AppendNum(aValue);
			
	if(!iConfigData)
		{
		retVal = EFalse;
		}
	else if(iConfigData->SetValue(aSectName,aKeyName,value)==KErrNotFound)
		{
		err = iConfigData->AddValue(aSectName,aKeyName,value);
		}
	
	if(retVal && KErrNone==err)
		{
		TRAP(err, iConfigData->WriteToFileL());
		if (iEnableIniAccessLog)
			{
			if (KErrNone == err)
				{
				INFO_PRINTF5(KTEFIniWriteInt64,iIniFileName, &aSectName, &aKeyName , aValue);
				}
			}
		}

	if (!retVal || KErrNone!=err)
		{
		retVal = EFalse;
		if (iEnableIniAccessLog)
			{
			_LIT(KWriteIntegerError, "Error occurs when writing integer to file, error code=%d");
			ERR_PRINTF2(KWriteIntegerError, err);
			}
		}
	return retVal;	
	}

/**
 * Writes the string value  passed in to the test steps ini file under the section and key name mentioned
 * Creates a new section or key name if it does not existing already
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aValue - Reference to the string to be written on to the test steps ini file
 * @return TBool - ETrue for found, EFalse for not found 
 */
EXPORT_C TBool CTestStep::WriteStringToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TPtrC& aValue)
	{
	TBool retVal(ETrue);
	TInt err(KErrNone);
			
	if(!iConfigData)
		{
		retVal = EFalse;
		}
	else if(iConfigData->SetValue(aSectName,aKeyName,aValue)==KErrNotFound)
		{
		err = iConfigData->AddValue(aSectName,aKeyName,aValue);
		}
	
	if(retVal && KErrNone==err)
		{
		TRAP(err, iConfigData->WriteToFileL());
		if (iEnableIniAccessLog)
			{
			if (KErrNone == err)
				{
				INFO_PRINTF5(KTEFIniWriteString,iIniFileName, &aSectName, &aKeyName , &aValue);
				}
			}
		}

	if (!retVal || KErrNone!=err)
		{
		retVal = EFalse;
		if (iEnableIniAccessLog)
			{
			_LIT(KWriteIntegerError, "Error occurs when writing string to file, error code=%d");
			ERR_PRINTF2(KWriteIntegerError, err);
			}
		}
	return retVal;
	}
	
/**
 * Writes the boolean value  passed in to the test steps ini file under the section and key name mentioned
 * Creates a new section or key name if it does not existing already
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aValue - Reference to the boolean value to be written onto the test steps ini file
 * @return TBool - ETrue for found, EFalse for not found 
 */	
EXPORT_C TBool CTestStep::WriteBoolToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TBool& aValue)
	{
	TBuf<KMaxTestExecuteNameLength> value;
	TBool retVal(ETrue);
	TInt err(KErrNone);
	if(aValue==1)
		{
		_LIT(KTrue,"True");
		value.Append(KTrue);
		}
	else if(aValue==0) 
		{
		_LIT(KFalse,"False");
		value.Append(KFalse);
		}
	else 
		{
		return(EFalse);
		}
		
	if(!iConfigData)
		{
		retVal = EFalse;
		}
	else if(iConfigData->SetValue(aSectName,aKeyName,value)==KErrNotFound)
		{
		err = iConfigData->AddValue(aSectName,aKeyName,value);
		}
	
	if(retVal && KErrNone==err)
		{
		TRAP(err, iConfigData->WriteToFileL());
		if (iEnableIniAccessLog)
			{
			if (KErrNone == err)
				{
				INFO_PRINTF5(KTEFIniWriteInteger,iIniFileName, &aSectName, &aKeyName , aValue);
				}
			}
		}

	if (!retVal || KErrNone!=err)
		{
		retVal = EFalse;
		if (iEnableIniAccessLog)
			{
			_LIT(KWriteIntegerError, "Error occurs when writing bool to file, error code=%d");
			ERR_PRINTF2(KWriteIntegerError, err);
			}
		}
	return retVal;
	}
	
/**
 * Writes the hex equivalent of the integer value  passed in to the test steps ini file 
 * under the section and key name mentioned
 * Creates a new section or key name if it does not existing already
 * @param aSectName - Section within the test steps ini file
 * @param aKeyName - Name of a key within a section
 * @return aValue - Reference to the integer equivalent of the hex value to be written onto the test steps ini file
 * @return TBool - ETrue for found, EFalse for not found 
 */	
EXPORT_C TBool CTestStep::WriteHexToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt& aValue)
	{
	TBuf<KMaxTestExecuteNameLength> value;
	TBool retVal(ETrue);
	TInt err(KErrNone);
	value.Num(aValue,EHex);
	
	if(!iConfigData)
		{
		retVal = EFalse;
		}
	else if(iConfigData->SetValue(aSectName,aKeyName,value)==KErrNotFound)
		{
		iConfigData->AddValue(aSectName,aKeyName,value);
		}
	
	if(retVal && KErrNone==err)
		{
		TRAP(err, iConfigData->WriteToFileL());
		if (iEnableIniAccessLog)
			{
			if (KErrNone == err)
				{
				INFO_PRINTF5(KTEFIniWriteInteger,iIniFileName, &aSectName, &aKeyName , aValue);
				}
			}
		}

	if (!retVal || KErrNone!=err)
		{
		retVal = EFalse;
		if (iEnableIniAccessLog)
			{
			_LIT(KWriteIntegerError, "Error occurs when writing hex to file, error code=%d");
			ERR_PRINTF2(KWriteIntegerError, err);
			}
		}
	return retVal;
	}

/**
 * Updates the data to the shared data name passed either in Copy Mode/Append Mode
 * @param aShareName - Shared data name to be written
 * @param aSharedDataVal - Descriptor value to be used for updating the shared data
 * @param aModeCopy - Enum value indicating the descriptor value to be overwritten/appended
 * @Leave system wide errors
 */
EXPORT_C void CTestStep::WriteSharedDataL(const TDesC& aShareName, TDesC& aSharedDataVal, TModeCopy aModeCopy)
	{
	CTestSharedData* sharedObject = NULL;
	CTEFSharedData<CTestSharedData>* sharedData1 = CTEFSharedData<CTestSharedData>::NewLC(sharedObject, aShareName);
	sharedData1->EnterCriticalSection();
	switch(aModeCopy)
		{
		case ESetText:
			{
			sharedObject->SetText(aSharedDataVal);
			break;
			}
		case EAppendText:
			{
			sharedObject->AppendText(aSharedDataVal);
			break;
			}
		default:
			break;
		}
	sharedData1->ExitCriticalSection();
	CleanupStack::PopAndDestroy(sharedData1);
	}

/**
 * Reads the data from the specified shared data object
 * Copies it to the reference descriptor passed in
 * @param aShareName - Shared Data Name to be read
 * @param aSharedDataVal - Descriptor reference passed in for reading the data from shared data
 * @Leave system wide errors
 */
EXPORT_C void CTestStep::ReadSharedDataL(const TDesC& aShareName, TDes& aSharedDataVal)
	{
	CTestSharedData* sharedObject = NULL;
	CTEFSharedData<CTestSharedData>* sharedData1 = CTEFSharedData<CTestSharedData>::NewLC(sharedObject, aShareName);
	sharedData1->EnterCriticalSection();
	sharedObject->GetText(aSharedDataVal);
	sharedData1->ExitCriticalSection();
	CleanupStack::PopAndDestroy(sharedData1);
	}

/**
 * Sets the test step class with a user-friendly name to be referred from the script file
 * @param aStepName - Name of the test step
 */
EXPORT_C void CTestStep::SetTestStepName(const TDesC& aStepName)
	{
	iTestStepName.Copy(aStepName);
	}

/**
 * Used to set the actual error value within the test step object
 * @param aStepError - Test Step Error
 */
EXPORT_C void CTestStep::SetTestStepError(const TInt& aStepError)
	{
	iTestStepError = aStepError;
	if (aStepError < 0)
		SetTestStepResult(EFail);
	}

/**
 * Initialise the File Logger server
 * Secure and Non-secure variants
 * @Leave system wide errors
 */
void CTestStep::SetLoggerL()
	{
	if(iLogger)
		{
		CloseLogger();
		delete iLogger  ;
		iLogger = NULL;
		}
	TRAPD(err1,iLogger = CTestExecuteLogger::NewL());
	if(err1 != KErrNone)
		{
		User::Leave(err1);
		}

	// Create a cinidata object for parsing the testexecute.ini
	CTestExecuteIniData* parseTestExecuteIni = NULL;
	TBuf<KMaxTestExecuteNameLength> resultFilePath;
	TBuf<KMaxTestExecuteNameLength> xmlFilePath;
	TInt logMode;
	TInt loggerChannel ; 
	TInt logLevel;
	TInt ChanneloutPort;// serial port
	
	TRAPD(err,parseTestExecuteIni = CTestExecuteIniData::NewL(iSystemDrive));
	if (err == KErrNone)
		{
		CleanupStack::PushL(parseTestExecuteIni);
		parseTestExecuteIni->ExtractValuesFromIni();
		parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, resultFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFXmlKey, xmlFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogMode, logMode);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogSeverityKey, logLevel);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFEnableIniAccessLog, iEnableIniAccessLog);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogChannel, loggerChannel);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFOutputPort,ChanneloutPort);
		}
	else
		{
		TBuf<KMaxTestExecuteNameLength> testExecuteLogPath(KTestExecuteLogPath);
		testExecuteLogPath.Replace(0, 2, iSystemDrive);
		resultFilePath.Copy(testExecuteLogPath);
		xmlFilePath.Copy(testExecuteLogPath);
		logMode = TLoggerOptions(ELogHTMLOnly);
		logLevel = RFileFlogger::TLogSeverity(ESevrAll);
		iEnableIniAccessLog = ETrue;
		}
	Logger().SetLoggerOptions(logMode);
	Logger().SetLoggerChannel(loggerChannel) ; 
	if( loggerChannel == ESerial || loggerChannel == EBoth)		
		{
		//InitialiseLoggingL
		Logger().OpenSerialPortL(ChanneloutPort);
		}
	
	// Initialise a handle to the file logger
	User::LeaveIfError(Logger().Connect());
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);
	RFile file;
	TBuf<KMaxTestExecuteNameLength> xmlLogFile(xmlFilePath);
	TBuf<KMaxTestStepNameLength> logFile;
	TBuf<KMaxTestExecuteNameLength> logFileNameFile(resultFilePath);
	logFileNameFile.Append(KTestExecuteScheduleTestLogCompatibilityNameFile);
	if(file.Open(fS,logFileNameFile,EFileRead | EFileShareAny) != KErrNone)
		{
		// For the old flogger we have to create an individual file
		logFile.Copy(iTestStepName);
		_LIT(KTxtExtension,".txt");
		logFile.Append(KTxtExtension);
		logMode = TLoggerOptions(0);
		Logger().SetLoggerOptions(logMode);
		}
	else
		{
		CleanupClosePushL(file);
		TBuf8<KMaxTestExecuteNameLength> logFile8;
		TInt fileSize;
		User::LeaveIfError(file.Size(fileSize));
		User::LeaveIfError(file.Read(logFile8,fileSize));
		logFile.Copy(logFile8);
		xmlLogFile.Append(logFile);
		_LIT(KXmlExtension,".xml");
		xmlLogFile.Append(KXmlExtension);
		_LIT(KHtmExtension,".htm");
		logFile.Append(KHtmExtension);
		CleanupStack::Pop(&file);
		file.Close();
		}
	TBuf<KMaxTestExecuteLogFilePath> logFilePath(resultFilePath);
	logFilePath.Append(logFile);
	CleanupStack::Pop(&fS);
	fS.Close();
	
	if (logMode == 0 || logMode == 2)
		{
		TRAPD(err1,Logger().HtmlLogger().CreateLog(logFilePath,RTestExecuteLogServ::ELogModeAppend));
		if(err1 != KErrPathNotFound)
			{
			User::LeaveIfError(err1) ;
			}
		else
			{
			// if its other than an htm file , ignore it
			_LIT(KHtmExtension,".htm");
			if(logFilePath.Find(KHtmExtension)>-1)
				{
				User::LeaveIfError(err1) ;
				}
			}

		Logger().HtmlLogger().SetLogLevel(TLogSeverity(logLevel));
		}
	if (logMode == 1 || logMode == 2)
		{
		User::LeaveIfError(Logger().XmlLogger().CreateLog(xmlLogFile,RFileFlogger::ELogModeAppend));
		Logger().XmlLogger().SetLogLevel(RFileFlogger::TLogSeverity(logLevel));
		}
	if (parseTestExecuteIni != NULL)
		{
		CleanupStack::PopAndDestroy(parseTestExecuteIni);
		}
	}

/**
 * Just close our handle to Flogger
 */
void CTestStep::CloseLogger()
	{
	Logger().Close();
//	if(iLogger)
//		{
//		delete iLogger;
//		iLogger = NULL;
//		}
	}

///////
// LEGACY methods from scheduletest & autotest
///////
EXPORT_C void CTestStep::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine)
///< TEST(a)
	{
	if(TestStepResult() == EPass && !aCondition)			// If not true, change to fail!
		{
		SetTestStepResult(EFail);
		_LIT(KMessage,"ERROR: Test Failed");
		Logger().LogExtra(aFile, aLine, ESevrErr,KMessage);
		}
	}

EXPORT_C void CTestStep::testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine, TBool aIgnoreStepResult)
///< TEST(a)
	{
	if (aIgnoreStepResult)
		{
		if (!aCondition)
			{
			SetTestStepResult(EFail);
			_LIT(KMessage,"ERROR: Test Failed");
			Logger().LogExtra(aFile, aLine, ESevrErr,KMessage);
			}
		}
	else
		{
		testBooleanTrue(aCondition, aFile, aLine);
		}
	}

EXPORT_C void CTestStep::testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine)
///< TESTL(a)
	{
	testBooleanTrue(aCondition, aFile, aLine);
	if(!aCondition)
		User::Leave(TEST_ERROR_CODE);	// leave with standard error code
	}

EXPORT_C void CTestStep::testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine, TBool aIgnoreStepResult)
///< TESTL(a)
	{
	testBooleanTrue(aCondition, aFile, aLine, aIgnoreStepResult);
	if(!aCondition)
		User::Leave(TEST_ERROR_CODE);	// leave with standard error code
	}

EXPORT_C void CTestStep::testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
///< TESTEL(a,b)
	{
	testBooleanTrueWithErrorCode(aCondition, aErrorCode, aFile, aLine);
	if(!aCondition)
		User::Leave(aErrorCode);
	}

EXPORT_C void CTestStep::testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine, TBool aIgnoreStepResult)
///< TESTEL(a,b)
	{
	testBooleanTrueWithErrorCode(aCondition, aErrorCode, aFile, aLine, aIgnoreStepResult);
	if(!aCondition)
		User::Leave(aErrorCode);
	}

EXPORT_C void CTestStep::testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine)
///< TESTE(a,b)
	{
	if(TestStepResult() == EPass && !aCondition)			// If not true, change to fail!
		{
		SetTestStepResult(EFail);
		_LIT(KMessage,"Test Failed with error [%d]");
		//START defect 116178
		Logger().LogExtra(aFile, aLine, ESevrErr, KMessage, aErrorCode);
		//END defect 116178
		}
	}

EXPORT_C void CTestStep::testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine, TBool aIgnoreStepResult)
///< TESTE(a,b)
	{
	if (aIgnoreStepResult)
		{
		if(!aCondition)			
			{
			SetTestStepResult(EFail);
			_LIT(KMessage,"Test Failed with error [%d]");
		//	START defect 116178
			Logger().LogExtra(aFile, aLine, ESevrErr, KMessage, aErrorCode);
		//	END defect 116178
			}
		}
	else
		{
		testBooleanTrueWithErrorCode(aCondition, aErrorCode, aFile, aLine);
		}
	}

EXPORT_C void CTestStep::TestCheckPointCompareL(TInt aVal,TInt aExpectedVal, const TDesC& aText, const TText8* aFile,TInt aLine)
	{
	if(aVal != aExpectedVal)
		{
		// decode formated data for display on console
		TBuf <KMaxTestExecuteLogLineLength> LineBuf;
		_LIT(KMessage,"FAILED test:  Val = %d Exp Val = %d %S");
		LineBuf.AppendFormat(KMessage,aVal,aExpectedVal,&aText);
		// send the data to the log system via the suite
		Logger().LogExtra((TText8*)aFile, aLine, ESevrErr, LineBuf );
		User::Leave(aVal);
		}
	}

/**
 * Returns the server name that instantiated the test step
 * @return TDesC - CTestServer name
 */
EXPORT_C const TDesC& CTestStep::GetServerName() const
	{
	return iServerName;
	}

/**
 * TestCaseName
 * Returns boolean true if testcase name is available, false otherwise.
 * Updates the TDes& to the testcase name if available, blank string otherwise.
 * @param aTestCaseName - TDes reference to update the test case name
 * @return TBool - Boolean value indicating presense of test case name
 */
EXPORT_C TBool CTestStep::TestCaseName(TDes& aTestCaseName)
	{
	if (iTestCaseID.CompareF(KTEFTestCaseDefault) == 0)
		{
		aTestCaseName.Copy(KTEFNull);
		return EFalse;
		}
	else
		{
		aTestCaseName.Copy(iTestCaseID);
		return ETrue;
		}
	}
