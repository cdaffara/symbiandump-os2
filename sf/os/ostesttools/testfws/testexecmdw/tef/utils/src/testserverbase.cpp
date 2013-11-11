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
* Implementation of the following classes
* CTestServer
* n x  CTestSession Maps to RTestServ session)
* n x  CStepControl or CPersistentStepControl (Maps to RTestSession)
* CTestStep (1 to 1 mapping with parent)
* CTestServer
* Derives CServer
* Test servers derive from this and implement the CreateTestStep() pure virtual
* CTestSession
* Derives CSharableSession
* Implements the ServiceL() pure virtual and creates CStepControl object(s)
* when a test step is opened.
* Implements the MSessionNotify interface for callbacks from CStepControl when
* a test step completes.
* CStepControl
* Derives CActive
* Runs a test step instance in its own thread and heap
* CTestStep
* Derives CBase
* Test Servers derive test steps from this
*
*/



/**
 @file TestClient.cpp
*/

#include "testexecuteserverbase.h"
#include "testserver2.h"
#include "testexecuteserverutils.h"
#include "testexecuteclient.h"
#include <e32math.h>
#include <test/wrapperutilsplugin.h>
#include <test/tefutils.h>

const TUint KDefaultHeapSize = 0x100000;

//> @internalComponent
// see the impletation below.
void SytemWideErrToTefErr(TInt &aErr);
/**
 * Constructor
 */
EXPORT_C CTestServer::CTestServer() : CServer2(EPriorityStandard)
,	iSeed(0)
,	iSessionCount(0)
	{
	// Random seed for unique thread id's
	iSeed = (TInt)this;
	// Default is not to allow Server Logging
	iLoggerStarted = EFalse;
	}

/**
 * Destructor
 */
EXPORT_C CTestServer::~CTestServer()
	{
	if (iLoggerStarted)
		{
		// Shut down the Servers' logger instance.
		Logger().Close();
		}
	}

/**
 * @param aName - Reference to the Server name
 * StartL + initiate server logging
 * Servers can StartL themselves or call this to gain server logging.
 */
EXPORT_C void CTestServer::ConstructL(const TDesC& aName)
	 {
	 StartL(aName);
	 StartLoggerL();
	 iServerName = aName;
	 }

/**
 * Will extract the script logfile name from the temporary file 'LogFileName.txt'
 * (assuming no ScheduleTest compatible logging) and then opens a logging session
 * to that file. If ScheduleTest logging is in effect then we will open ScriptEngine.htm
 * instead as the file to log to.
 */	 
void CTestServer::StartLoggerL()
	{
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

	CTestExecuteIniData* parseTestExecuteIni = NULL;
	TBuf<KMaxTestExecuteNameLength> resultFilePath;
	TBuf<KMaxTestExecuteNameLength> xmlFilePath;
	TInt logMode;
	TInt logLevel;

	TRAPD(err,parseTestExecuteIni = CTestExecuteIniData::NewL(defaultSysDrive));
	if (err == KErrNone)
		{
		CleanupStack::PushL(parseTestExecuteIni);
		parseTestExecuteIni->ExtractValuesFromIni();
		parseTestExecuteIni->GetKeyValueFromIni(KTEFHtmlKey, resultFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFXmlKey, xmlFilePath);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogMode, logMode);
		parseTestExecuteIni->GetKeyValueFromIni(KTEFLogSeverityKey, logLevel);
		}
	else
		{
		TBuf<KMaxTestExecuteNameLength> testExecuteLogPath(KTestExecuteLogPath);
		testExecuteLogPath.Replace(0, 2, defaultSysDrive);
		resultFilePath.Copy(testExecuteLogPath);
		xmlFilePath.Copy(testExecuteLogPath);
		logMode = TLoggerOptions(ELogHTMLOnly);
		logLevel = RFileFlogger::TLogSeverity(ESevrAll);
		}
	Logger().SetLoggerOptions(logMode);

	// Initialise a handle to the file logger
	User::LeaveIfError(Logger().Connect());
	RFs fS;
	User::LeaveIfError(fS.Connect());
	CleanupClosePushL(fS);
	RFile file;
	TBuf<KMaxTestExecuteNameLength> xmlLogFile(xmlFilePath);
	TBuf<KMaxTestExecuteNameLength> logFile;
	TBuf<KMaxTestExecuteNameLength> logFileNameFile(resultFilePath);
	logFileNameFile.Append(KTestExecuteScheduleTestLogCompatibilityNameFile);
	if(file.Open(fS,logFileNameFile,EFileRead | EFileShareAny) != KErrNone)
		{
		// If LogFileName.txt is not present then we are using ScheduleTest
		// compliant logging, any logging issued by the server will therefore
		// go to ScriptEngine.htm
		_LIT(KScriptEngine,"ScriptEngine");
		logFile.Copy(KScriptEngine);
		}
	else
		{
		CleanupClosePushL(file);
		TBuf8<KMaxTestExecuteNameLength> logFile8;
		TInt fileSize;
		User::LeaveIfError(file.Size(fileSize));
		User::LeaveIfError(file.Read(logFile8,fileSize));
		logFile.Copy(logFile8);
		CleanupStack::Pop(&file);
		file.Close();
		}
	
	xmlLogFile.Append(logFile);
	_LIT(KXmlExtension,".xml");
	xmlLogFile.Append(KXmlExtension);
	_LIT(KHtmExtension,".htm");
	logFile.Append(KHtmExtension);
	TBuf<KMaxTestExecuteLogFilePath> logFilePath(resultFilePath);
	logFilePath.Append(logFile);
	CleanupStack::Pop(&fS);
	fS.Close();
	
	if (logMode == 0 || logMode == 2)
		{
		User::LeaveIfError(Logger().HtmlLogger().CreateLog(logFilePath,RTestExecuteLogServ::ELogModeAppend));
		Logger().HtmlLogger().SetLogLevel(TLogSeverity(logLevel));
		}
		
	if (logMode == 1 || logMode == 2)
		{
		User::LeaveIfError(Logger().XmlLogger().CreateLog(xmlLogFile,RFileFlogger::ELogModeAppend));
		Logger().XmlLogger().SetLogLevel(RFileFlogger::TLogSeverity(logLevel));
		}
	
	User::LeaveIfError( Logger().ShareAuto() );
	iLoggerStarted = ETrue;
	if (parseTestExecuteIni != NULL)
		{
		CleanupStack::PopAndDestroy(parseTestExecuteIni);
		}
	}
	
/**
 * Last one out switch off the lights
 * Stop the active sheduler and hence the server, if this is the last session
 */	 
void CTestServer::SessionClosed()
	{
	iSessionCount--;
	if (iSessionCount == 0)
		CActiveScheduler::Stop();
	}

/**
 * @param RMessage - RMessage for the session open
 * Secure version
 */
EXPORT_C CSession2* CTestServer::NewSessionL(const TVersion& /*aVersion*/,const RMessage2& /*aMessage*/) const
	{
	CTestSession* session = new (ELeave) CTestSession();
	CONST_CAST(CTestServer*,this)->iSessionCount++;
	return session;
	}

/**
 * Constructor
 */
EXPORT_C CTestSession::CTestSession()
:	iPersistentStepControl(NULL)
,	iPersistentBlockControl(NULL)
,	iBlockArray(NULL)
	{
	}

/**
 * Destructor
 */
EXPORT_C CTestSession::~CTestSession()
	{
	if( iBlockArray )
		{
		delete iBlockArray;
		iBlockArray = NULL;
		}	
	
	CTestServer* p=(CTestServer*) Server();
	
	//delete the persistent step
	if(iPersistentStepControl)
		delete iPersistentStepControl;
	//delete the persistent block
	if(iPersistentBlockControl)
		delete iPersistentBlockControl;
	// Shuts Down the server if this is the last open session
	p->SessionClosed();
	}

/**
 * @param aMessage - Function and data for the session
 * Session was created by pure virtual CTestServer::NewSessionL()
 * Message Functions defined in TestExecuteClient.h
 * 
 * EOpenTestStep - Creates a new subsession
 * ERunTestStep - Executes the test step asynchronously
 * EAbortTestStep - Kill()'s the executing test step
 * ECloseTestStep - Free's the resource
 *
 * Secure and non-secure variants
 * There are two modes of operation:
 * Test step is opened with the shared data boolean set to FALSE:
 *		Create a new CStepControl instance and hence a new thread in its own heap
 *		Consecutive or Concurrent operation
 * Test step is opened with the shared data boolean set to TRUE:
 *		Create a CPersistentStepControl and keep reusing it, and its thread
 *		Consecutive operation only
 */
EXPORT_C void CTestSession::ServiceL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case EOpenTestStep :
			{
			// Open the test step
			// Buffer size policed on the client side
			TBuf<KMaxTestStepNameLength> stepName;
			// Read the step name from the descriptor
			TBool sharedData;
			aMessage.ReadL(0,stepName);
			// Find out what mode we're working in
			sharedData = aMessage.Int1();
			// Both types derive from base class and implement pure virtuals
			CControlBase* stepControl = NULL;
			if(sharedData)
				{
				// Shared data mode
				// Create the instance if it doesn't exist
				if(!iPersistentStepControl)
					iPersistentStepControl = new (ELeave)CPersistentStepControl(*(CTestServer*)Server());
				stepControl = iPersistentStepControl;
				iPersistentStepControl->StepName() = stepName;
				}
			else
				{
				// Default operation. Create a new instance
				stepControl = new (ELeave)CStepControl(*(CTestServer*)Server(),stepName);
				}
			// We pass back the address of the CStepControl class which is passed to
			// us in all calls on the subsession in Message 3
			TPtrC8 stepRef(REINTERPRET_CAST(TUint8*,&stepControl),sizeof(TInt));
			aMessage.Write(3,stepRef);
			aMessage.Complete(KErrNone);
			}
			break;
		case EOpenTestBlock :
			{
			// Open the test block
			// Buffer size policed on the client side
			TBuf<KMaxTestExecuteNameLength> stepName;

			// Find out what mode we're working in
			TBool sharedData = aMessage.Int1();
			// Both types derive from base class and implement pure virtuals
			CBlockControlBase* blockControl = NULL;
			if(sharedData)
				{
				// Shared data mode
				// Create the instance if it doesn't exist
				if(!iPersistentBlockControl)
					iPersistentBlockControl = new (ELeave)CPersistentBlockControl(*(CTestServer*)Server());
				blockControl = iPersistentBlockControl;
				}
			else
				{
				// Default operation. Create a new instance
				blockControl = new (ELeave)CBlockControl(*(CTestServer*)Server());
				}
			// We pass back the address of the CStepControl class which is passed to
			// us in all calls on the subsession in Message 3
			TPtrC8 blockRef(REINTERPRET_CAST(TUint8*,&blockControl),sizeof(TInt));
			aMessage.Write(3,blockRef);
			aMessage.Complete(KErrNone);
			}
			break;
		case ERunTestStep :
			{
			// Execute the test step
			// Buffer size policed on client side
			// Message 0 contains the test step arguments
			// Message 1 contains a descriptor for the panic string, if the test step panics
			TBuf<KMaxTestExecuteCommandLength> stepArgs;
			aMessage.ReadL(0,stepArgs);
						
			// Get the address of our CStepControl object
			CStepControlBase* stepControl = REINTERPRET_CAST(CStepControlBase*,aMessage.Int3());
			// Kick off the test step
			// Message completed when the test step completes
			// StartL() is mode dependent pure virtual
			TRAPD(err,stepControl->StartL(aMessage, stepArgs));
			if(err)
				// Complete now if we can't start the test step
				// Client has possibly called run before waiting for last completion
				aMessage.Complete(err);
			}
			break;
		case ERunTestBlock :
			{
			// Execute the test step
			// Buffer size policed on client side
			// Message 0 contains the test step arguments
			// Message 1 contains a descriptor for the panic string, if the test step panics
			TBuf<KMaxTestExecuteCommandLength> blockArgs;
			aMessage.ReadL(0,blockArgs);
						
			// Get the test block of commands
			HBufC8*	itemArray = HBufC8::NewLC( aMessage.GetDesMaxLengthL(2) );
			TPtr8	itemArrayPtr( itemArray->Des() );
			aMessage.ReadL( 2, itemArrayPtr );

			// Get the address of our CStepControl object
			CBlockControlBase* blockControl = REINTERPRET_CAST(CBlockControlBase*,aMessage.Int3());
			// Kick off the test step
			// Message completed when the test step completes
			// StartL() is mode dependent pure virtual
			TRAPD(err,blockControl->StartL(aMessage, blockArgs, itemArrayPtr ));
			if(err)
				// Complete now if we can't start the test step
				// Client has possibly called run before waiting for last completion
				aMessage.Complete(err);
			
			CleanupStack::PopAndDestroy( itemArray );
			}
			break;
		case EAbortTestStep :
			{
			CControlBase* control = REINTERPRET_CAST(CControlBase*,aMessage.Int3());
			// Stop is mode dependent pure virtual
			control->Stop();
			// Complete synchronously
			aMessage.Complete(KErrNone);
			break;
			}
		case ECloseTestStep :
			{
			CControlBase* control = REINTERPRET_CAST(CControlBase*,aMessage.Int3());
			// Only delete if we are in non-shared data mode
			if(	control != iPersistentStepControl &&
				control != iPersistentBlockControl )
				{
				delete control;
				}
			aMessage.Complete(KErrNone);
			}
			break;
		default:
			break;
		}
	}

/**
 * @param aServer - Reference to the CTestServer base class
 * @param aStepName - The test step name
 * The Non-Shared data step control implementation
 */
CStepControl::CStepControl(CTestServer& aServer, const TDesC& aStepName) :
	CStepControlBase(aServer)
	{
	StepName() = aStepName;
	}

/**
 * Thread completion
 */
void CStepControl::RunL()
	{
	// Error value if set in the test step will be saved in the Message()
	if (Error() != KNull)
		{
		TBuf<KMaxTestExecuteNameLength> errorParam(KErrorEquals);
		errorParam.Append(Error());
		Message().Write(1,errorParam);
		}

	// If the thread panicked, pick up the panic string and return it to the client
	// Overwrites the error value previously saved in Message()
	if(WorkerThread().ExitType() == EExitPanic)
		{
		TBuf<KMaxTestExecuteNameLength> panicParam(KPanicEquals);
		panicParam.Append(WorkerThread().ExitCategory());
		Message().Write(1,panicParam);
		}


	
	if (WorkerThread().ExitType() == EExitPanic)
		{
		TInt err = WorkerThread().ExitReason();
		SytemWideErrToTefErr(err);
		Message().Complete(err);
		}
	else
		{
		if (iStatus.Int() == KErrAbort && TimedOut())
			{
			if (Server().LoggerStarted())
				{
				Server().ERR_PRINTF1(_L("TEST IS ABOUT TO ABORT DUE TO TEF TIMEOUT"));				
				}
			}
		// iStatus.Int() is the same as the thread ExitReason
		Message().Complete(iStatus.Int());
		}
	
	// Close thread handle
	WorkerThread().Close();
	}

/**
 * Destructor
 */
CStepControl::~CStepControl()
	{
	}

/**
 * Step Execution module which is wrapped within UHEAP macros to trap memory leaks during execution
 * @param aStepControl - Is a generic type of class T passed as template parameter. Either CStepControl/CWorkerControl
 * for normal execution mode and persistant thread mode for concurrent execution of steps respectively
 * @param aStep - Object derived from CTestStep class for execution of test steps both in normal mode and persistent mode
 */
template<class T>
void ThreadStepExecutionL(T* aStepControl, CTestStep* aStep)
	{
	TInt loop = 0;
	TBool simulateOOM = EFalse; //ShouldRunOOMTest();

	FOREVER
		{
		TInt errRun = KErrNone;

		// Call the CTestStep virtuals 
		aStep->doTestStepPreambleL();
			
		// count cells so we can know how many we leaked
		TInt cellsStart = User::CountAllocCells();
		
		if (simulateOOM)
			{
			__UHEAP_MARK;
	
			// set allocator to fail on the loop'th alloc
			aStep->SetHeapFailNext(loop);
			}
		
		aStepControl->TimedOut() = ETrue;
		TRAP(errRun, aStep->doTestStepL());
		if (errRun != KErrNone && !simulateOOM)
			{
			if (errRun == KErrAbort)
				{
				aStepControl->TimedOut() = EFalse;
				}
			aStep->doTestStepPostambleL();
			User::Leave(errRun);
			}
			
		TBool finishedCorrectly = EFalse;
		// cancel the alloc failures
		if (simulateOOM)
			{
			if ((errRun == KErrNone) && (loop >= 1))
				{
				// claims to have finished correctly, and we're not failing every alloc
				finishedCorrectly = aStep->CheckForHeapFailNext();
				}
			aStep->ResetHeapFailNext();
			}
		
		aStep->doTestStepPostambleL();
		
		TInt cellsEnd = User::CountAllocCells();
			
		if (cellsStart < cellsEnd && simulateOOM)
			{
			// leaked.
			TInt leakedCells = cellsEnd - cellsStart;
			if (aStepControl->Server().LoggerStarted())
				{
				aStepControl->Server().ERR_PRINTF3(_L("On loop number %d we leaked %d cells. About to cause panic."),loop,leakedCells);				
				}
			aStep->SetTestStepResult(EFail);
			}
			
		if (simulateOOM)
			{
			// panic on leak (alloc nnnnnnnn)
			__UHEAP_MARKEND;
			}
	
		// check to see if we finished all OOM testing successfully
		if ((errRun == KErrNone) && (simulateOOM) && (finishedCorrectly))
			{
			// test completed successfully, or the User::Leave(KErrNoMemory) was trapped by something else. 
			// Need a cunning solution here. Hmm. Testing to see if the next alloc call fails won't work:
			//    eg, if a test has 3 allocs, heap currently set to fail every 2nd, this would be number 4,
			//    and if 2 was masked then we would think we are done.
			//
			// Fix PDEF115450, remove the line 			aStep->SetTestStepResult(EPass); and modify the information
			// to "Out of memory test completed after %d iterations."
			if (aStepControl->Server().LoggerStarted())
				{
				aStepControl->Server().INFO_PRINTF2(_L("Out of memory test completed after %d iterations."),loop);
				}
			break;
			}

		// check to see if we should run OOM testing.
		if (++loop == 1)
			{
			// first go.
			if (!aStep->ShouldRunOOMTest())
				break;
			else
				{
				if (aStepControl->Server().LoggerStarted())
					{
					aStepControl->Server().INFO_PRINTF1(_L("Test passed. About to run Out of Memory testing."));					
					}
				simulateOOM = ETrue;
				aStep->IniAccessLog() = EFalse;
				}
			}
		}
	}

/**
 * @param aStepControl - Pointer to the step control object which kicked us off
 * The thread code. Just drops through with no reuse.
 */
void ThreadFuncL(CStepControl* aStepControl)
	{
	// Call the server pure virtual to get a step instance
	CTestStep* step = CONST_CAST(CTestServer&,aStepControl->Server()).CreateTestStep(aStepControl->StepName());
	if(!step)
		User::Leave(KErrNotFound);
	CleanupStack::PushL(step);
	// Set up the step base class members
	TBool sharedData = EFalse;
	step->InitialiseL(aStepControl->Args(), aStepControl->Server().Name(), sharedData);

	ThreadStepExecutionL(aStepControl, step);

	// Return Error value set in test step to log result for comparison
	if (step->TestStepError() != 0)
		aStepControl->Error().Num(step->TestStepError());
	// EPass is 0
	// All the rest should be TRAP'd
	if(step->TestStepResult())
		User::Leave(step->TestStepResult());
	CleanupStack::PopAndDestroy(step);
 	}

/**
 * @param aParam - Pointer to a CStepControl object
 * The thread entry method
 */
TInt ThreadFunc(TAny* aParam)
	{
	// Create the thread's cleanup stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;
	// Trap it and return the error code to the OS
	TRAPD(err, ThreadFuncL(REINTERPRET_CAST(CStepControl*,aParam)));
	SytemWideErrToTefErr(err);
	delete cleanup;
	cleanup = NULL;
	return err;
	}

/**
 * @param aMessage - Keep a reference for async completion
 * @param aStepArgs - The RUN_TEST_STEP arguments
 * Secure and non-secure variants
 * Kick off the test step in its own thread
 * Pure virtual implementation
 */
void CStepControl::StartL(const RMessage2& aMessage, const TDesC& aStepArgs)
	{
	if(IsActive())
		User::Leave(KErrInUse);
	Message() = aMessage;
	Args().Copy(aStepArgs);
	TBuf<8> heapSizeBuf(KNull);
	TUint heapSize(0);
	aMessage.ReadL(1,heapSizeBuf);
	aMessage.Write(1,KNull);
	TLex heapSizeLex;
	
	if (heapSizeBuf.Length() >=3)
		{
		if ( heapSizeBuf.Mid(0,2).CompareF(_L("0x")) == 0 )
			{
			heapSizeLex.Assign(heapSizeBuf.Mid(2));
			}
		else
			{
			heapSizeLex.Assign(heapSizeBuf);
			}
		heapSizeLex.Val(heapSize,EHex);
		}
		
	TBuf<50> threadName;
	// Unique thread name guaranteed if we use the this pointer plus a random number
	// whose seed was initialised to the address of the CTestServer object
	_LIT(KWorker,"Worker%d %d");
	threadName.Format(KWorker,(TInt)this,Math::Rand(CONST_CAST(CTestServer&,Server()).RandSeed()));
	// Create with own heap so system cleans up if we kill it
	const TUint KMaxHeapSize = 0x100000;
	const TUint KMinSize = KMinHeapSize;
	if (heapSize < KMinSize)
		heapSize = KMaxHeapSize;			///< Allow a 1Mb max heap

	User::LeaveIfError(WorkerThread().Create(threadName, ThreadFunc, KDefaultStackSize + 0x1000,KMinHeapSize, heapSize,this, EOwnerProcess));
	// Prime ready for completion
	SetActive();
	// Use the appropriate variant call to get the thread exit
	WorkerThread().Logon(iStatus);
	WorkerThread().Resume();
	}

/**
 * Kill the thread if it's still running
 * The async completion will be picked up as normal with a KErrAbort status
 * Pure virtual implementation
 */
void CStepControl::Stop()
	{
	if(IsActive())
		WorkerThread().Kill(KErrAbort);
	}

/**
 * @param aStepControl - Pointer to the step control object.
 * The test step thread.
 * We reuse this thread so the test steps can store persistent data in the
 * CTestServer derived class
 * The thread synchronises with its creator via the CWorkerControl class
 * Implementation of the shared data mode control object
 */
void PersistentThreadFuncL(CWorkerControl* aControl)
	{
	// Thread entry is sync'd with a semaphore
	// Caller will Wait on this
	// Also set our main sync treq to pending.
	// It's completed to let us go in and execute the test step code.
	aControl->WorkerStatus() = KRequestPending;
	aControl->Semaphore().Signal();
	// Go into the main test step execution loop
	for(;;)
		{
		User::WaitForRequest(aControl->WorkerStatus());
		// Check
		if(aControl->WorkerStatus().Int() == KErrAbort)
			User::Leave(KErrAbort);
		CTestStep* step = CONST_CAST(CTestServer&,aControl->Server()).CreateTestStep(aControl->StepName());
		if(!step)
			User::Leave(KErrNotFound);
		CleanupStack::PushL(step);
		// Set up the step base class members
		TBool sharedData = ETrue;
		step->InitialiseL(aControl->Args(), aControl->Server().Name(), sharedData);

		ThreadStepExecutionL(aControl, step);

		// Pick up the final result
		// Set it in the controlling class
		aControl->Result() = step->TestStepResult();
		TBuf<KMaxTestExecuteNameLength> lError;
		if (step->TestStepError() != 0)
			{
			lError.Num(step->TestStepError());
			if (lError != KNull)
				{
				lError.Insert(0,KErrorEquals);
				aControl->PersistentError().Copy(lError);
				}
			}
		CleanupStack::PopAndDestroy(step);
		// Set our status for the wait at the top of the loop
		aControl->WorkerStatus() = KRequestPending;
		// Signal the status that our creator will be waiting on
		// Creator's thread handle in the control class
		TRequestStatus* status = &aControl->Status();
		aControl->ControllerThread().RequestComplete(status,KErrNone);
		}
 	}

/**
 * @param aParam - Pointer to a CTestStep control object
 * The thread entry method
 */
TInt PersistentThreadFunc(TAny* aParam)
	{
	// Create the thread's cleanup stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	if(!cleanup)
		return KErrNoMemory;
	// Trap it and return the error code to the OS
	//defect 116046
	CWorkerControl* workControl = REINTERPRET_CAST(CWorkerControl*,aParam);
	workControl->SetCleanupPtr(cleanup);
	TRAPD(err, PersistentThreadFuncL(workControl));
	//END defect 116046
	SytemWideErrToTefErr(err);
	delete cleanup;
	cleanup = NULL;
	return err;
	}

/**
 * @param aServer - Reference to the CTestServer derived class
 * Constructor
 */
CPersistentStepControl::CPersistentStepControl(CTestServer& aServer) :
	CStepControlBase(aServer),
	iInitialised(EFalse)
	{ 
	}

/**
 * @param aMessage - Client's message for completion
 * @param aStepArgs - Arguments to the RUN_xxx_STEP_COMMAND's
 * Implementation of base class pure virtual.
 * Necessarily complex because of thread reuse. Instantiates 2 classes:
 * WorkerMonitor class and WorkerControl class
 * WorkerMonitor picks up thread exit and WorkerControl picks up test step
 * return value.
 */
void CPersistentStepControl::StartL(const RMessage2& aMessage,const TDesC& aStepArgs)
	{
	if(IsActive())
		User::Leave(KErrInUse);
	Message() = aMessage;
	Args().Copy(aStepArgs);
	// Check to see if we're reusing the worker thread and classes
	if(!iInitialised)
		{
		// Need to construct the monitor and controller classes
		// They are both constructed with a reference to our iStatus
		// Either of them can complete us. We check their Active flags in our RunL()
		iWorkerControl = new (ELeave) CWorkerControl(Server(),iStatus);		
		// The worker thread needs our thread handle to RequestComplete us
		User::LeaveIfError(iWorkerControl->ControllerThread().Duplicate(RThread()));
		// Worker thread entry is sync'd with a semaphore.
		User::LeaveIfError(iWorkerControl->Semaphore().CreateLocal(0));
		TBuf<50> threadName;
		// Unique thread name guaranteed if we use the this pointer plus a random number
		// whose seed was initialised to the address of the CTestServer object
		// Create in our heap.
		_LIT(KWorker,"Worker%d %d");
		threadName.Format(KWorker,(TInt)this,Math::Rand(CONST_CAST(CTestServer&,Server()).RandSeed()));
		User::LeaveIfError(WorkerThread().Create(threadName,PersistentThreadFunc, KDefaultStackSize + 0x1000,NULL,iWorkerControl, EOwnerProcess));
		iWorkerMonitor = new (ELeave) CWorkerMonitor(iStatus);
		}
	// Worker thread needs the step arguments and the step name
	iWorkerControl->Args().Set(Args());
	iWorkerControl->StepName().Set(StepName());
	// Set this object ready for completion by either the monitor or controller objects
	Prime();
	// Set the child monitor and control objects ready for completion
	iWorkerMonitor->SetActive();
	iWorkerControl->Prime();
	// Use the monitor object to pick up thread exit
	// This should only happen for panic, leave and abort following the Stop() call
	WorkerThread().Rendezvous(iWorkerMonitor->Status());
	if(!iInitialised)
		{
		// Start the thread and sync up via the semaphore
		WorkerThread().Resume();
		iWorkerControl->Semaphore().Wait();
		iWorkerControl->Semaphore().Close();
		iInitialised = ETrue;
		}
	// Worker thread will be at the top of its loop waiting to execute
	// the test step virtuals.
	// Issue the request then it will drop through
	TRequestStatus* status = &iWorkerControl->WorkerStatus();
	WorkerThread().RequestComplete(status,KErrNone);
	}

/**
 * Destructor
 */
CPersistentStepControl::~CPersistentStepControl()
	{
	// Only need to clean up in the initialised state
	if(!iInitialised)
		return;

	// Check both objects
	// Neither of them should be active, but just in case
	if(iWorkerMonitor->IsActive())
		{
		// Cancelling means we don't get stray events
		WorkerThread().RendezvousCancel(iWorkerMonitor->Status());
		// Need to cancel the objeect itself
		iWorkerMonitor->Cancel();
		}
	if(iWorkerControl->IsActive())
		{
		// Complete the request then cancel
		TRequestStatus* status = &iWorkerControl->Status();
		User::RequestComplete(status,KErrNone);
		iWorkerControl->Cancel();
		}

	// The worker thread will currently be blocked on its TRequestStatus at the top
	// of its loop.
	// Signal the status with KErrAbort and the thread will check this value and leave.
	// If we Kill the thread then the cleanup stack for the thread is orphaned.
	// PersistentThreadFuncL() TRAP's the leave.
	// We logon and catch the thread exit.
	TRequestStatus status = KRequestPending;
	WorkerThread().Rendezvous(status);
	TRequestStatus* workerStatus = &iWorkerControl->WorkerStatus();
	WorkerThread().RequestComplete(workerStatus,KErrAbort);
	User::WaitForRequest(status);
	// Close both handles
	WorkerThread().Close();				
	iWorkerControl->ControllerThread().Close();

	delete iWorkerControl;
	delete iWorkerMonitor;
	}

/**
 * Necessarily complex because of the two sources of completion
 * We can tell which one completed us by checking their iActive members
 */
void CPersistentStepControl::RunL()
	{
	if (iWorkerControl->PersistentError() != KNull)
		{
		TBuf<KMaxTestExecuteNameLength> errorParam;
		errorParam.Copy(iWorkerControl->PersistentError()); // Error Value returned as Panic Result
		Message().Write(1,errorParam);
		iWorkerControl->PersistentError().Copy(KNull);
		}
	TInt ret = KErrNone;
	// Check which of the child objects completed us
	if(!iWorkerMonitor->IsActive())
		{
		// Unexpected exit from the worker thread
		iInitialised = EFalse;// this also make ~CPersistentStepControl not to delete twice.
		// Pick up the exit reason and panic code if it exists
		if(WorkerThread().ExitType() == EExitPanic)
			{
			TBuf<KMaxTestExecuteNameLength> panicParam(KPanicEquals);
				panicParam.Append(WorkerThread().ExitCategory()); // Panic Value returned as Result

			//START defect 116046, Cleanup memories.
			//iWorkerControl->Cleanup();
			//END defect 116046
				
			Message().Write(1,panicParam);
			}
			
		ret = WorkerThread().ExitReason();
		
		if (WorkerThread().ExitType() == EExitPanic)
			{
			SytemWideErrToTefErr(ret);
			}		
		if (ret == KErrAbort && iWorkerControl->TimedOut())
			{
			if (Server().LoggerStarted())
				{
				Server().ERR_PRINTF1(_L("TEST IS ABOUT TO ABORT DUE TO TEF TIMEOUT"));				
				}
			}

		// We need to complete and cancel the other request so we don't have stray events
		TRequestStatus* status = &iWorkerControl->Status();
		User::RequestComplete(status,KErrNone);
		iWorkerControl->Cancel();
		// Free the resource in the worker control object
		iWorkerControl->ControllerThread().Close();
		WorkerThread().Close();
		delete iWorkerControl;
		iWorkerControl = NULL;
		delete iWorkerMonitor;
		iWorkerMonitor = NULL;
		// Next time in to StartL() we create them from cleana
		}
	else if(!iWorkerControl->IsActive())
		{
		// Normal test step completion
		// We can reuse the thread next time into StartL()
		// The thread will be blocking on iWorkerStatus
		// We need to cancel the other object
		WorkerThread().RendezvousCancel(iWorkerMonitor->Status());
		iWorkerMonitor->Cancel();
		// Retrieve the test result
		ret = iWorkerControl->Result();
		SytemWideErrToTefErr(ret);
		}
	else
		// Unexpected
		{
		ret = iStatus.Int();
		}
	// Complete back to the client
	Message().Complete(ret);
	}

/**
 * Abort due to timeout
 * The worker monitor object will pick up the thread exit
 */
void CPersistentStepControl::Stop()
	{
	if(iWorkerMonitor->IsActive())
		{
		WorkerThread().Kill(KErrAbort);
		}
	}


CBlockControlBase::~CBlockControlBase()
	{
	if( iBlockArray )
		{
		delete iBlockArray;
		iBlockArray = NULL;	
		}
	}
	
TTEFItemArray* CBlockControlBase::BlockArray() const
	{
	return iBlockArray;
	}

void CBlockControlBase::CreateBlockArrayL( const TDesC8& aBlockArrayPckg )
	{
	if( iBlockArray )
		{
		delete iBlockArray;
		iBlockArray = NULL;
		}
	
	TTEFItemPkgBuf	itemPckgBuf;
	TInt count = aBlockArrayPckg.Size()/itemPckgBuf.Size();
	iBlockArray = new (ELeave) TTEFItemArray( count );
	TInt pos = 0;
	for( TInt i=0; i<count; i++ )
		{
		itemPckgBuf.Copy(aBlockArrayPckg.Mid(pos, itemPckgBuf.Size()));
		pos += itemPckgBuf.Size();
		iBlockArray->AppendL( itemPckgBuf() );
		}
	}
	
HBufC8* CBlockControlBase::CreateBlockArrayPckgLC()
	{
	TInt count = iBlockArray->Count();
	TTEFItemPkgBuf	itemPckgBuf;
	HBufC8* blockArrayPckg = HBufC8::NewLC( count * itemPckgBuf.Size() );
	TPtr8	ptr( blockArrayPckg->Des() );
	for( TInt i=0; i<count; i++ )
		{
		itemPckgBuf = iBlockArray->At(i);
		ptr.Append( itemPckgBuf );
		}
	
	return blockArrayPckg;
	}

CBlockControl::CBlockControl(CTestServer& aServer)
:	CBlockControlBase(aServer)
	{
	}
	
CBlockControl::~CBlockControl()
	{
	}

void CBlockControl::RunL()
	{
	// Write back the test block
	HBufC8*	blockArrayPckg = CreateBlockArrayPckgLC();
	TPtr8	blockArrayPtr(blockArrayPckg->Des());
	Message().WriteL( 2, blockArrayPtr );
	CleanupStack::PopAndDestroy(blockArrayPckg);
	
	// If the thread panicked, pick up the panic string and return it to the client
	// Overwrites the error value previously saved in Message()
	if(WorkerThread().ExitType() == EExitPanic)
		{
		TBuf<KMaxTestExecuteNameLength> panicParam(KPanicEquals);
		TPtrC	panicCat = WorkerThread().ExitCategory(); // Panic Value returned as Result
		if( panicCat.Length() == 0 )
			{
			panicParam.Append(_L("NULL"));
			}
		panicParam.Append( panicCat );
		Message().Write(1,panicParam);
		}

	if (WorkerThread().ExitType() == EExitPanic)
		{
		TInt err = WorkerThread().ExitReason();
		SytemWideErrToTefErr(err);
		Message().Complete(err);
		}
	else
		{
		if (iStatus.Int() == KErrAbort && TimedOut())
			{
			if (Server().LoggerStarted())
				{
				Server().ERR_PRINTF1(_L("TEST IS ABOUT TO ABORT DUE TO TEF TIMEOUT"));				
				}
			}
		// iStatus.Int() is the same as the thread ExitReason
		Message().Complete(iStatus.Int());
		}
	
	// Close thread handle
	WorkerThread().Close();
	}


void ThreadBlockFuncL(CBlockControl* aBlockControl)
	{
	// Call the server pure virtual to get a step instance
	CTestBlockController* block = REINTERPRET_CAST(CTestServer2&,aBlockControl->Server()).CreateTestBlock();
	if(!block)
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PushL(block);

	// Set up the block base class members
	TBool sharedData = EFalse;
	block->InitialiseL(aBlockControl->Args(), aBlockControl->Server().Name(), sharedData);
	block->SetBlockArray(aBlockControl->BlockArray());
	block->SetSharedData( REINTERPRET_CAST(CTestServer2*, &aBlockControl->Server() ));

	ThreadStepExecutionL(aBlockControl, block);

	// All the rest should be TRAP'd
	if( block->TestStepResult() )
		{
		User::Leave( block->TestStepResult() );
		}
	
	CleanupStack::PopAndDestroy(block);
 	}

TInt ThreadBlockFunc(TAny* aParam)
	{
	// Create the thread's cleanup stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;
	// Trap it and return the error code to the OS
	TRAPD(err, ThreadBlockFuncL(REINTERPRET_CAST(CBlockControl*,aParam)));
	SytemWideErrToTefErr(err);
	delete cleanup;
	cleanup = NULL;
	return err;
	}

void CBlockControl::StartL(const RMessage2& aMessage,const TDesC& aArgs, const TDesC8& aBlockArrayPckg)
	{
	if(IsActive())
		{
		User::Leave(KErrInUse);
		}

	Message() = aMessage;
	Args().Copy(aArgs);
	TBuf<8> heapSizeBuf(KNull);
	TUint heapSize(0);
	aMessage.ReadL(1,heapSizeBuf);
	aMessage.Write(1,KNull);
	TLex heapSizeLex;
	
	if (heapSizeBuf.Length() >=3)
		{
		if ( heapSizeBuf.Mid(0,2).CompareF(_L("0x")) == 0 )
			{
			heapSizeLex.Assign(heapSizeBuf.Mid(2));
			}
		else
			{
			heapSizeLex.Assign(heapSizeBuf);
			}
		heapSizeLex.Val(heapSize,EHex);
		}

	// Set the BlockArray so it can be passed and used by the TestBlockController
	CreateBlockArrayL( aBlockArrayPckg );
	
	// Unique thread name guaranteed if we use the this pointer plus a random number
	// whose seed was initialised to the address of the CTestServer object
	TBuf<50> threadName;
	_LIT(KWorker,"Worker%d %d");
	threadName.Format(KWorker,(TInt)this,Math::Rand(CONST_CAST(CTestServer&,Server()).RandSeed()));

	// Create with own heap so system cleans up if we kill it
	if( (TInt)heapSize < KMinHeapSize )
		{
		heapSize = KDefaultHeapSize;			///< Allow a 1Mb max heap
		}

	User::LeaveIfError(WorkerThread().Create(threadName, ThreadBlockFunc, KDefaultStackSize + 0x1000,KMinHeapSize, heapSize, this, EOwnerProcess));

	// Prime ready for completion
	SetActive();
	// Use the appropriate variant call to get the thread exit
	WorkerThread().Logon(iStatus);
	WorkerThread().Resume();
	}
	
void CBlockControl::Stop()
	{
	if(IsActive())
		{
		WorkerThread().Kill(KErrAbort);
		}
	}
	
CPersistentBlockControl::CPersistentBlockControl(CTestServer& aServer)
:	CBlockControlBase(aServer)
,	iWorkerControl(NULL)
,	iWorkerMonitor(NULL)
,	iInitialised(EFalse)
	{ 
	}
	
CPersistentBlockControl::~CPersistentBlockControl()
	{

	// Only need to clean up in the initialised state
	if(!iInitialised)
		return;

	// Check both objects
	// Neither of them should be active, but just in case
	if(iWorkerMonitor->IsActive())
		{
		// Cancelling means we don't get stray events
		WorkerThread().RendezvousCancel(iWorkerMonitor->Status());
		// Need to cancel the objeect itself
		iWorkerMonitor->Cancel();
		}
	if(iWorkerControl->IsActive())
		{
		// Complete the request then cancel
		TRequestStatus* status = &iWorkerControl->Status();
		User::RequestComplete(status,KErrNone);
		iWorkerControl->Cancel();
		}

	// The worker thread will currently be blocked on its TRequestStatus at the top
	// of its loop.
	// Signal the status with KErrAbort and the thread will check this value and leave.
	// If we Kill the thread then the cleanup stack for the thread is orphaned.
	// PersistentThreadFuncL() TRAP's the leave.
	// We logon and catch the thread exit.
	TRequestStatus status = KRequestPending;
	WorkerThread().Rendezvous(status);
	TRequestStatus* workerStatus = &iWorkerControl->WorkerStatus();
	WorkerThread().RequestComplete(workerStatus,KErrAbort);
	User::WaitForRequest(status);
	// Close both handles
	WorkerThread().Close();				
	iWorkerControl->ControllerThread().Close();

	delete iWorkerControl;
	delete iWorkerMonitor;
	}

void PersistentThreadBlockFuncL(CBlockWorkerControl* aControl)
	{
	// Thread entry is sync'd with a semaphore
	// Caller will Wait on this
	// Also set our main sync treq to pending.
	// It's completed to let us go in and execute the test step code.
	aControl->WorkerStatus() = KRequestPending;
	aControl->Semaphore().Signal();
	// Go into the main test step execution loop
	for(;;)
		{
		User::WaitForRequest(aControl->WorkerStatus());
		// Check
		if(aControl->WorkerStatus().Int() == KErrAbort)
			User::Leave(KErrAbort);
		CTestBlockController* block = REINTERPRET_CAST(CTestServer2&, aControl->Server()).CreateTestBlock();
		if(!block)
			User::Leave(KErrNotFound);
		CleanupStack::PushL(block);
		// Set up the step base class members
		TBool sharedData = ETrue;
		block->InitialiseL(aControl->Args(), aControl->Server().Name(), sharedData);
		
		block->SetBlockArray( aControl->BlockArray() );
		block->SetSharedData( REINTERPRET_CAST(CTestServer2*, &aControl->Server()) );

		ThreadStepExecutionL(aControl, block);

		// Pick up the final result
		// Set it in the controlling class
		aControl->Result() = block->TestStepResult();
		TBuf<KMaxTestExecuteNameLength> lError;
		if (block->TestStepError() != 0)
			lError.Num(block->TestStepError());
		if (lError != KNull)
			{
			lError.Insert(0,KErrorEquals);
			aControl->PersistentError().Copy(lError);
			}
		CleanupStack::PopAndDestroy(block);
		// Set our status for the wait at the top of the loop
		aControl->WorkerStatus() = KRequestPending;
		// Signal the status that our creator will be waiting on
		// Creator's thread handle in the control class
		TRequestStatus* status = &aControl->Status();
		aControl->ControllerThread().RequestComplete(status,KErrNone);
		}
 	}
 
TInt PersistentThreadBlockFunc(TAny* aParam)
	{
	// Create the thread's cleanup stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;
	// Trap it and return the error code to the OS
	TRAPD(err, PersistentThreadBlockFuncL(REINTERPRET_CAST(CBlockWorkerControl*,aParam)));
	SytemWideErrToTefErr(err);
	delete cleanup;
	cleanup = NULL;
	return err;
	}
 	
void CPersistentBlockControl::StartL(const RMessage2& aMessage,const TDesC& aStepArgs, const TDesC8& aBlockArrayPckg)
	{
	if(IsActive())
		User::Leave(KErrInUse);
	Message() = aMessage;
	Args().Copy(aStepArgs);
	
	// Set the BlockArray so it can be passed and used by the TestBlockController
	// Set the BlockArray so it can be passed and used by the TestBlockController
	CreateBlockArrayL( aBlockArrayPckg );
	
	// Check to see if we're reusing the worker thread and classes
	if(!iInitialised)
		{
		// Need to construct the monitor and controller classes
		// They are both constructed with a reference to our iStatus
		// Either of them can complete us. We check their Active flags in our RunL()
		iWorkerControl = new (ELeave) CBlockWorkerControl(Server(),iStatus);
		// The worker thread needs our thread handle to RequestComplete us
		User::LeaveIfError(iWorkerControl->ControllerThread().Duplicate(RThread()));
		// Worker thread entry is sync'd with a semaphore.
		User::LeaveIfError(iWorkerControl->Semaphore().CreateLocal(0));
		TBuf<50> threadName;
		// Unique thread name guaranteed if we use the this pointer plus a random number
		// whose seed was initialised to the address of the CTestServer object
		// Create in our heap.
		_LIT(KWorker,"Worker%d %d");
		threadName.Format(KWorker,(TInt)this,Math::Rand(CONST_CAST(CTestServer&,Server()).RandSeed()));
		User::LeaveIfError(WorkerThread().Create(threadName,PersistentThreadBlockFunc, KDefaultStackSize + 0x1000,NULL,iWorkerControl, EOwnerProcess));
		iWorkerMonitor = new (ELeave) CWorkerMonitor(iStatus);
		}

	// Set the Block Array
	iWorkerControl->SetBlockArray( BlockArray() );
	
	// Worker thread needs the step arguments and the step name
	iWorkerControl->Args().Set(Args());

	// Set this object ready for completion by either the monitor or controller objects
	Prime();
	// Set the child monitor and control objects ready for completion
	iWorkerMonitor->SetActive();
	iWorkerControl->Prime();
	// Use the monitor object to pick up thread exit
	// This should only happen for panic, leave and abort following the Stop() call
	WorkerThread().Rendezvous(iWorkerMonitor->Status());
	if(!iInitialised)
		{
		// Start the thread and sync up via the semaphore
		WorkerThread().Resume();
		iWorkerControl->Semaphore().Wait();
		iWorkerControl->Semaphore().Close();
		iInitialised = ETrue;
		}
	// Worker thread will be at the top of its loop waiting to execute
	// the test step virtuals.
	// Issue the request then it will drop through
	TRequestStatus* status = &iWorkerControl->WorkerStatus();
	WorkerThread().RequestComplete(status,KErrNone);
	}
	
void CPersistentBlockControl::RunL()
	{
	// Write back the test block
	HBufC8*	blockArrayPckg = CreateBlockArrayPckgLC();
	TPtr8	blockArrayPtr(blockArrayPckg->Des());
	Message().WriteL( 2, blockArrayPtr );
	CleanupStack::PopAndDestroy(blockArrayPckg);
	
	if (iWorkerControl->PersistentError() != KNull)
		{
		TBuf<KMaxTestExecuteNameLength> errorParam;
		errorParam.Copy(iWorkerControl->PersistentError()); // Error Value returned as Panic Result
		Message().Write(1,errorParam);
		iWorkerControl->PersistentError().Copy(KNull);
		}
	TInt ret = KErrNone;
	// Check which of the child objects completed us
	if(!iWorkerMonitor->IsActive())
		{
		// Unexpected exit from the worker thread
		iInitialised = EFalse;// this also make ~CPersistentBlockControl not to delete twice.
		// Pick up the exit reason and panic code if it exists
		if(WorkerThread().ExitType() == EExitPanic)
			{
			TBuf<KMaxTestExecuteNameLength> panicParam(KPanicEquals);
			TPtrC	panicCat = WorkerThread().ExitCategory(); // Panic Value returned as Result
			if( panicCat.Length() == 0 )
				{
				panicParam.Append(_L("NULL"));
				}
			panicParam.Append( panicCat );
			Message().Write(1,panicParam);
			}
			
		ret = WorkerThread().ExitReason();
		
		if (WorkerThread().ExitType() == EExitPanic)
			{
			SytemWideErrToTefErr(ret);
			}
		if (ret == KErrAbort && iWorkerControl->TimedOut())
			{
			if (Server().LoggerStarted())
				{
				Server().ERR_PRINTF1(_L("TEST IS ABOUT TO ABORT DUE TO TEF TIMEOUT"));				
				}
			}

		// We need to complete and cancel the other request so we don't have stray events
		TRequestStatus* status = &iWorkerControl->Status();
		User::RequestComplete(status,KErrNone);
		iWorkerControl->Cancel();
		// Free the resource in the worker control object
		iWorkerControl->ControllerThread().Close();
		WorkerThread().Close();
		delete iWorkerControl;
		iWorkerControl = NULL;
		delete iWorkerMonitor;
		iWorkerMonitor = NULL;
		// Next time in to StartL() we create them from cleana
		}
	else if(!iWorkerControl->IsActive())
		{
		// Normal test step completion
		// We can reuse the thread next time into StartL()
		// The thread will be blocking on iWorkerStatus
		// We need to cancel the other object
		WorkerThread().RendezvousCancel(iWorkerMonitor->Status());
		iWorkerMonitor->Cancel();
		// Retrieve the test result
		ret = iWorkerControl->Result();
		SytemWideErrToTefErr(ret);
		}
	else
		// Unexpected
		{
		ret = iStatus.Int();
		}
	// Complete back to the client
	Message().Complete(ret);
	}

void CPersistentBlockControl::Stop()
	{
	if(iWorkerMonitor->IsActive())
		{
		WorkerThread().Kill(KErrAbort);
		}
	}

/**
 * //> @internalComponent
 * @param aErr - Reference to the error number
 * this delling with Leave or set result  error number conflict with sys wide numbers[-1,-49]
 */
void SytemWideErrToTefErr(TInt &aErr)
{
    switch (aErr)
    	{
    	case KErrInUse:
    		{
    		aErr = KErrTestExecuteInUse;
    		}
    		break;
    	case KErrServerBusy:
    		{
    		aErr = KErrTestExecuteServerBusy;
    		}
    		break;
    	};
    return;
}
