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
* This contains CTestStep which is the base class for all test steps.
*
*/



/**
 @file TestExecuteStepBase.h
*/
#if (!defined __TESTEXECUTE_STEP_BASE_H__)
#define __TESTEXECUTE_STEP_BASE_H__

#include <e32base.h>
#include <f32file.h>
#include <iniparser.h>
#include <test/testexecutelogger.h>
#include <test/testexecuteclient.h>
#include <test/tefshareddata.h>
#include <test/testshareddata.h>

class CTestStep : public CBase
/**
@publishedPartner
@test
*/
	{
public:
	// Destructor
	IMPORT_C virtual ~CTestStep();

	// Get methods to read values from the ini file
	IMPORT_C TBool GetBoolFromConfig(const TDesC& aSectName,const TDesC& aKeyName,TBool& aResult);
	IMPORT_C TBool GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	IMPORT_C TBool GetStringFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TPtrC& aResult);

	// Set a user-friendly test step name referred for a test step object from the script file
	IMPORT_C void SetTestStepName(const TDesC& aStepName);

	IMPORT_C TBool GetHexFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt& aResult);
	
	// Set the actual test step error obtained to execution of test step to compare the expected error with the actual
	IMPORT_C void SetTestStepError(const TInt& aErrorCode);

	// Write methods to write values into ini file
	IMPORT_C TBool WriteIntToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt& aValue);
	IMPORT_C TBool WriteStringToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TPtrC& aValue);
	IMPORT_C TBool WriteBoolToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TBool& aValue);
	IMPORT_C TBool WriteHexToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt& aValue);

	//Updates the data to the shared data name passed either in Copy Mode/Append Mode
	IMPORT_C void WriteSharedDataL(const TDesC& aShareName, TDesC& aSharedDataVal, TModeCopy aModeCopy = ESetText);
	//Reads the data from the specified shared data object
	IMPORT_C void ReadSharedDataL(const TDesC& aShareName, TDes& aSharedDataVal);

	// Initialise the test step during cnstruction
	void InitialiseL(const TDesC& aStepArgs, const TDesC& aServerName, TBool& aSharedDataMode);

	// Initialises logger instance for the test step
	void SetLoggerL();

	// Closes the logger instance for the test step
	void CloseLogger();

	// Returns the section name specified in the script file for the current test step execution
	inline TDes& ConfigSection() {return iSection;};

	// Set the result of test step execution  to one of the TVerdict enum codes
	inline void SetTestStepResult(TVerdict aVerdict) {iTestStepResult = aVerdict;};

	// Returns the test step result at any point during execution of test step
	inline TVerdict TestStepResult() const {return iTestStepResult;};

	// Logger interface used for constructing result strings to be fed for the logger instance
	inline CTestExecuteLogger& Logger() {return *iLogger;};

	// Returns the user-friendly test step name
	inline const TDesC& TestStepName() const {return iTestStepName;};

	// Returns any error code set during the test step execution
	inline const TInt& TestStepError() const {return iTestStepError;};

	// Returns the boolean true of !OOM option is set from the script file, false otherwise
	inline TBool& ShouldRunOOMTest() {return iOOMRequestStatus;};

	// Returns true if test server is loaded using -SharedData mode from the script file
	inline TBool& IsSharedDataMode() {return iSharedDataMode;};

	// Returns the loop index of the current loop if the test runs within a START_REPEAT/EN_REPEAT block
	// Returns -1 if test is not within a loop
	inline TInt GetLoopIndex() {return iLoopIndex;};

	// Returns the setup value set using !Setup TEF parameter within the script file
	// Returns a value 0 if !Setup TEF parameter is not used
	inline TInt GetSetupState() {return iSetupParamValue;};

	// Returns the drive name from which the current script file is executed
	inline TDriveName ScriptDriveName() { return iScriptDriveName;};

	// Returns reference to the iEnableIniAccessLog bool which can be used by tests to switch off ini access logging on the run
	inline TBool& IniAccessLog() { return iEnableIniAccessLog; };

	// Returns the default system drive letter provided by call to RFs::GetSystemDrive()
	inline TDriveName GetSystemDrive() { return iSystemDriveForTest; };

	// Test Step virtuals - Test Implementation methods
	IMPORT_C virtual enum TVerdict	doTestStepL() = 0;
	IMPORT_C virtual enum TVerdict	doTestStepPreambleL();
	IMPORT_C virtual enum TVerdict	doTestStepPostambleL();

	// Setting simulated OOM and testing whether the code under test reached the simulated failure
	IMPORT_C virtual void SetHeapFailNext(TInt aFailureIndex);
	IMPORT_C virtual void ResetHeapFailNext();
	IMPORT_C virtual TBool CheckForHeapFailNext();
	
	// LEGACY methods
	IMPORT_C void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine);
	IMPORT_C void testBooleanTrue(TBool aCondition, const TText8* aFile, TInt aLine, TBool IgnoreStepResult);
	IMPORT_C void testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine);
	IMPORT_C void testBooleanTrueL(TBool aCondition, const TText8* aFile, TInt aLine, TBool IgnoreStepResult);
	IMPORT_C void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine );
	IMPORT_C void testBooleanTrueWithErrorCode(TBool aCondition, TInt aErrorCode, const TText8* aFile, TInt aLine, TBool IgnoreStepResult);
	IMPORT_C void testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode,
													const TText8* aFile, TInt aLine );
	IMPORT_C void testBooleanTrueWithErrorCodeL(TBool aCondition, TInt aErrorCode,
													const TText8* aFile, TInt aLine, TBool IgnoreStepResult );
	IMPORT_C void TestCheckPointCompareL(TInt aVal, TInt aExpectedVal, const TDesC& aText, const TText8* aFile, TInt aLine);

	// Retrieve the server name
	IMPORT_C const TDesC& GetServerName() const;

	// Retrieve the test case name/id
	IMPORT_C TBool TestCaseName(TDes& aTestCaseName);

protected:
	IMPORT_C CTestStep();
public:
	IMPORT_C TBool GetIntFromConfig(const TDesC& aSectName, const TDesC& aKeyName, TInt64& aResult);
	IMPORT_C TBool WriteIntToConfig(const TDesC& aSectName, const TDesC& aKeyName, const TInt64& aValue);
private:
	void LoadConfigFileL(const TDesC& aFilePath);
private:
	CTestExecuteLogger*					iLogger;
	TVerdict							iTestStepResult;
	TBuf<KMaxTestExecuteNameLength>		iSection;
	CIniData*							iConfigData;
	TBuf<KMaxTestStepNameLength>	    iTestStepName;		// Name of the test step
	TBuf<KMaxTestExecuteNameLength>		iServerName;		// Name of the test server
	TInt								iTestStepError;		// Expected Test step Error
	HBufC*								iIniFileName;		// Name of the inifile to be use for logging.
	TBool								iOOMRequestStatus;	// Status of OOM request from user
	TBool								iEnableIniAccessLog;// Testexecute.ini entry for logging INI access
	TBool								iSharedDataMode;	// Flag representing -SharedData server mode
	TInt								iSetupParamValue;	// TEFParameter !Setup value set in the script
	TInt								iLoopIndex;			// Loop index of the current loop if test runs within a START_REPEAT
	TBuf<KMaxTestExecuteNameLength>		iTestCaseID;		// Name/ID of the test case in which the current test step executes
	TDriveName							iScriptDriveName;	// Drive name from where the script file is executed
	TDriveName							iSystemDrive;		// Default System Drive name
	TDriveName							iSystemDriveForTest;// System Drive name for Test artifacts

	// Use this buffer when allocating additional memory for the class in future
	// To avoid breaks with other depdenant comps
	TBuf<KMaxTestStepNameLength>		iReserved4Future1;
	TBuf<KMaxTestStepNameLength>		iReserved4Future2;
	TBuf<KMaxTestStepNameLength>		iReserved4Future3;
	TBuf<KMaxTestStepNameLength>		iReserved4Future4;
	};

// LEGACY
// Autotest macros mapped to CTestStep functions

// Check a boolean is true
#define TESTL(a) testBooleanTrueL((a), (TText8*)__FILE__, __LINE__) 
#define TEST(a)  testBooleanTrue((a), (TText8*)__FILE__, __LINE__) 

// Versions that will always log irrespective of testcase pass
#define TEST1L(a, b) testBooleanTrueL((a), (TText8*)__FILE__, __LINE__, (b) ) 
#define TEST1(a, b)  testBooleanTrue((a), (TText8*)__FILE__, __LINE__, (b) ) 

// Check a boolean is true if not return error code b
#define TESTE(a, b) testBooleanTrueWithErrorCode((a), (b), (TText8*)__FILE__, __LINE__) 
#define TESTEL(a, b) testBooleanTrueWithErrorCodeL((a), (b), (TText8*)__FILE__, __LINE__)  

// Versions that will always log irrespective of testcase pass
#define TESTE1(a, b, c) testBooleanTrueWithErrorCode((a), (b), (TText8*)__FILE__, __LINE__, (c) ) 
#define TESTE1L(a, b, c) testBooleanTrueWithErrorCodeL((a), (b), (TText8*)__FILE__, __LINE__, (c) )  

#define TEST_CHECKL(p1, p2, p3) TestCheckPointCompareL((p1), (p2), (p3), (TText8*)__FILE__, __LINE__)	
// Leave error code
#define TEST_ERROR_CODE 84	


#endif
