// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// This contains CTestCase which is the base class for all the TestCase DLLs
//just to test p4

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <f32file.h>
// Test system includes
#include "teststepcomsdbg.h"
#include <comms-infras/commsdebugutility.h>
#include "TestMessage.h"


// constructor
CTestStepFlogger::CTestStepFlogger() 
	{
	}

// destructor
CTestStepFlogger::~CTestStepFlogger()
	{
	}


TVerdict CTestStepFlogger::doTestStepPostambleL()
	{
        SetTestStepResult(EPass);	return TestStepResult();
	}

	
TInt CTestStepFlogger::executeStepL()
	{
		return KErrGeneral;
	}

TInt CTestStepFlogger::executeStepL( /*aLogger*/ TBool /*aStatus*/)
	{
		return KErrGeneral;
	}


TInt CTestStepFlogger::executeStep(CTestStepFlogger& aTestStep)
	{
	TInt r = 0;
	TInt ret= 0;

	TRAP( r, ret = aTestStep.executeStepL() );

	if ( r != KErrNone )
		ret = r;

	return ret;
	}

TInt CTestStepFlogger::executeStep( TBool bypassChecks )
	{
	TInt ret=0;
	TInt r;

	// bypassChecks is optional, so many clients don't know to supply it
	if (bypassChecks)
		{
		TRAP( r, ret = executeStepL(bypassChecks) );
		}
	else
		{
		TRAP( r, ret = executeStepL() );
		}

	if ( r != KErrNone )
		ret = r;

	return ret;
	}

TInt CTestStepFlogger::executeStep(CTestStepFlogger& aTestStep, TBool aStatus)
	{
	TInt result=KErrNone;
	TRAPD(err,result=aTestStep.executeStepL(aStatus));
	return (KErrNone!=err)?err:result;
	}

//
//This heap failure member function is used to ensure correct operation in low-memory
// situations. It requires the flogger server to be started in order to inform the server
// on each loop iteration as to the new memory requirement. Thus, this harness is no
// good for cases where the flogger server must be shut down and restarted or
// when server is not meant to be running before the test case commences.
//
TInt CTestStepFlogger::doTestStepWithHeapFailureL( CTestStepFlogger& aTestStep, TInt lowMemory, TInt highMemory, TInt aReturnValue, TBool bypassChecks)
/**
 @param bypassChecks allows a parameter to be passed to the executeStep to let it know that the
 case is being used for heap checking. This is needed because most flogger methods do
 not return an error code when they fail to write due to no memory. Thus, when running
 the heap test the part of the test case which ensures the data was written to disk
 may fail to find the data, and thus we need to bypass these checks.
 @param aReturnValue - expected return value if everything works
 @param lowMemory - amount of memory to start testing at - must be at least 10 lower than highMemory and test must fail due to low memory at this level
 @param highMemory - amount of memory to stop testing at - if we reach this, test has failed.
 */
	{
	TInt ret=0;
	TInt loop;
	TPtrC8 ptrSubSystemTmp;
	TPtrC8 ptrComponentTmp;
	ptrSubSystemTmp.Set(_L8("SubSystem"));
	ptrComponentTmp.Set(_L8("Component"));

	RFileLogger logger;
	User::LeaveIfError(logger.Connect());
	CleanupClosePushL(logger);
	logger.SetLogTags(KStdSubsysTag8, KStdCompTag8);
	// clear the log so that any previous test data is gone and not detected
	logger.ClearLog();
	User::After(KTimeToLog);

	for (loop = lowMemory; loop < highMemory ; loop++)
		{
		INFO_PRINTF2(_L("%d"),loop);
		logger.__DbgSetHeapFailure(loop);

		ret = aTestStep.executeStep(bypassChecks);

		if ( ret == KErrNoMemory)
			{
			//The heap failure has been trapped correctly
			continue;
			}
		else if (( ret == aReturnValue ) && (loop != lowMemory))
			{
			//Test step normal behaviour
			INFO_PRINTF4(_L("%S PASSED heap failure test, loop = %d return code==%d"), 
			&aTestStep.TestStepName(), loop, ret );
			SetTestStepResult(EPass);			break;
			}
		else
			{
			// test step has not returned the exepected error value ( which was either KErrNoMemory or aReturnValue )
			INFO_PRINTF5(_L("%S *FAILED* heap failure test, loop=%d return code:%d expected:%d"), 
				&aTestStep.TestStepName(), loop, ret, aReturnValue );
			SetTestStepResult(EFail);			break;
			}

		}
	// shutdown flogger server so we can see if any memory leaks - flogger svr will panic if there are
	// This also means the next test does not get any residuals in the log buffers.
	logger.ClearLog();
	logger.__DbgShutDownServer();
	CleanupStack::PopAndDestroy();	//logger
	if (loop == highMemory)
		{
		// often the return code isn't checked, so make sure the test harness sees that it failed.
		SetTestStepResult(EFail);		return KErrGeneral;
		}
	if ( ret != aReturnValue )
		{
		// often the return code isn't checked, so make sure the test harness sees that it failed.
		SetTestStepResult(EFail);		return KErrGeneral;
		}
	return KErrNone;
	}





TInt CTestStepFlogger::DoTestConnect(RFileLogger& aLogger)
	{
	TInt ret = KErrNone;
	TPtrC8 ptrSubSystem;
	TPtrC8  ptrComponent;
	ptrSubSystem.Set(_L8("SubSystem"));
	ptrComponent.Set(_L8("Component"));

	ret = aLogger.Connect();
	if (ret == KErrNone)
		ret = aLogger.SetLogTags(ptrSubSystem, ptrComponent); //SetLogTags() of Flogger called

	if (ret == KErrNone)
		ret = aLogger.ClearLog(); //clear the contents from the log
	
	return ret;
	}


TInt CTestStepFlogger::constructFloggerIniL( const TDesC8& additionalConfig )
/**
Replace the flogger.ini with a new one made up of the contents of ts_flogger.ini and additionalConfig.
@param additionalConfig string with the extra config items to be set in flogger.ini
@return KErrNone if no probs, otherwise an error code.
@note This function deletes then recreates the flogger.ini file, so during the short time between
 the delete and the recreate, if flogger server is running, it will report that there were errors in the ini file (because
 the ini file is not there).
 */
	{
	RFile theFile;
	RFile outFile;
	RFs fileServer; 
	User::LeaveIfError(fileServer.Connect());
	HBufC8 * fileContentsHeap;
	TInt fileSize;
	TInt returnCode;	

	returnCode = theFile.Open(fileServer,KFloggerTestIniMediaSourceFile,EFileRead);
	
	if (returnCode == KErrNone)
		{
		theFile.Size(fileSize);
		
		// allocate the heap space for the string given size of ts_flogger and
		// the length of the additional items string.
		TInt newFileSize = fileSize + additionalConfig.Length();  //just so we can see this during debugging

		fileContentsHeap = HBufC8::NewLC(newFileSize);

		TPtr8 fileContentsAppend(fileContentsHeap->Des());
		
		// read into the buffer the contents of ts_flogger.ini
		User::LeaveIfError(returnCode = theFile.Read(fileContentsAppend));
		
		// append addition items
		fileContentsAppend.Append(additionalConfig);

		// We must assume the flogger.ini is either there or not there,
		// If the flogger.ini is already there, and the flogger server is already running
		// and watching this file, we cannot do any of the following:
		//* Open and overwrite - if we overwrite a large file with a smaller one, it fails to overwrite the whole file
		//* Delete and create - after the delete but before the create flogger server will attempt to access
		//* use RFile::Replace - between the replace and the write flogger server will attempt access
		
		// so we must create a temporary file and then use the file system to overwrite the
		// current with our temp using "replace" which is an atomic operation as far as flogger server is concerned

		returnCode = outFile.Create(fileServer,KTempDuringCreationFloggerIniFile,EFileWrite);
		if (returnCode == KErrNone)
			{
			TInt pos = 0;
			outFile.Seek(ESeekStart,pos);
			outFile.Write(fileContentsAppend);
			
			outFile.Close();
			
			fileServer.Replace(KTempDuringCreationFloggerIniFile,KFloggerIniFile);
			}
		CleanupStack::PopAndDestroy(fileContentsHeap);
		theFile.Close();
		}
	
	fileServer.Close();

	if (returnCode != KErrNone)
		{
		return returnCode;
		}
	else
		{
		return KErrNone;
		}

	}
	
TInt CTestStepFlogger::replaceFloggerIniL( const TDesC8& newConfig )
/**
Replace the flogger.ini with a new one made up of the contents of newConfig
@param newConfig string with all the config items to be set in flogger.ini
@return KErrNone if no probs, otherwise an error code.
 */
	{
	RFile outFile;
	RFs fileServer; 
	User::LeaveIfError(fileServer.Connect());
	HBufC8 * fileContentsHeap;
	TInt returnCode;	

	// allocate the heap space for the string given size of 
	// the items string.
	TInt newFileSize = newConfig.Length();  //just so we can see this during debugging

	fileContentsHeap = HBufC8::NewLC(newFileSize);

	TPtr8 newFileContents(fileContentsHeap->Des());
	
	// add the config items
	newFileContents.Append(newConfig);

	// We must assume the flogger.ini is either there or not there,
	// If the flogger.ini is already there, and the flogger server is already running
	// and watching this file, we cannot do any of the following:
	//* Open and overwrite - if we overwrite a large file with a smaller one, it fails to overwrite the whole file
	//* Delete and create - after the delete but before the create flogger server will attempt to access
	//* use RFile::Replace - between the replace and the write flogger server will attempt access
	
	// so we must create a temporary file and then use the file system to overwrite the
	// current with our temp using "replace" which is an atomic operation as far as flogger server is concerned

	returnCode = outFile.Create(fileServer,KTempDuringCreationFloggerIniFile,EFileWrite);
	if (returnCode == KErrNone)
		{
		TInt pos = 0;
		outFile.Seek(ESeekStart,pos);
		outFile.Write(newFileContents);
		
		outFile.Close();
		
		fileServer.Replace(KTempDuringCreationFloggerIniFile,KFloggerIniFile);
		}
	CleanupStack::PopAndDestroy(fileContentsHeap);
	
	fileServer.Close();

	if (returnCode != KErrNone)
		{
		return returnCode;
		}
	else
		{
		return KErrNone;
		}

	}


/**
* Function  Name		: ForceLogFlush
* Input parameters		: None
* Output parameters		: RFileLogger 
* Description 			: This function writes enough data to flogger to force it to flush its
  file buffer. We need to do this for heap tests since the timer is disabled during heap tests. 
  Normally, the timer would ensure the buffer is flushed each second. 
  We must write enough data to match the KHeapBufSize (50K) constant in flogger.
  However, it is usually necessary to wait one second after writing this data to ensure flogger
  gets a chance to move the data from its queue to the buffer.
  When determining how much to write, we take into account that flogger will have added the
  component, subsystem and a few other characters to each line.

* 						  
*/


void CTestStepFlogger::ForceLogFlush(RFileLogger& aLogger)
	{
	const TInt KLineLen = KLogBufferSize - KMarginTemplateSize;
	TBuf8<KLineLen> buf;
	buf.SetLength(KLineLen);
	
	for(TUint8 i = 0; i < KLineLen; i++)
		{
		buf[i] = i;
		}
	for(TInt j = 0; j < KHeapBufFillIterations; j++)
		{
		aLogger.Write(buf);
		}
	}
