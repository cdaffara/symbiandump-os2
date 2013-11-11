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
//

// Epoc includes
#include <f32file.h>
#include <e32base.h>
#include <e32cons.h>
#include <e32test.h>
//TestBed includes
#include "TestController.h"
#include "ComponentInfo.h"
//ECom includes
#include <ecom/ecom.h>
//
#include "TestHarnessTemplate.h"

/**
	@internalComponent
	@fn				void DoTestingL(RTest& aRTest, TNewComponentTestLC aNewComponentTestLC)
	Intended Usage	: Run the tests discovered by the TestController.
	@param			aRTest A RTest instance.
	@param			aNewComponentTestLC A pointer to defined into ECOM test app global
					function. It should create derived CComponentTester object which
					knows about the tests required.
	@return			void.
	@leave  		KErrNoMemory, Any other codes possible from a test.
	@pre 			aNewComponentTestLC != NULL.
	Error Condition	: None
	@since			7.0s
 */
LOCAL_C void DoTestingL(RTest& aRTest, TNewComponentTestLC aNewComponentTestLC)
	{
	_LIT(KLogFileDirectory, "C:\\Logs\\TestBed\\");
	_LIT(KUnitTestMessage, "Unit Test %d: %S\n");
	_LIT(KNoTestsMessage, "No Tests found\n");

	// Avoid cleanup stack heap allocation expansion problems
	// when testing for heap allocation leaks within the test
	// harnesses, by expanding the stack now.
	const TInt KMaxCleanupFrames = 20;
	for (TInt i = 0; i < KMaxCleanupFrames; ++i)
		CleanupStack::PushL((TAny*)0);
	CleanupStack::Pop(KMaxCleanupFrames);

	// Check that the logging directory exists, and if it doesn't then create it
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TInt err = fs.MkDirAll(KLogFileDirectory);
	if(err != KErrAlreadyExists)	// Don't leave if it already exists
		User::LeaveIfError(err);
	fs.Close();

	// Set up the logging configuration information
	_LIT(KLogTitle, "Test Bed Test");
	TLoggingInfo* loggingInfo = new(ELeave) TLoggingInfo;
	CleanupStack::PushL(loggingInfo);
	loggingInfo->iTitle = &(KLogTitle);
	loggingInfo->iUseRDebug = ETrue;
	loggingInfo->iLogOutput = 0;
	loggingInfo->iReportOutput = 0;
	loggingInfo->iStyle = EHtml;

	// Create the test controller object and start the test
#ifdef LOG_PANIC_UNIT_TEST_FAILURE
	CTestController* theController = CTestController::NewLC(NULL, aNewComponentTestLC, &aRTest, loggingInfo);
#else
	CTestController* theController = CTestController::NewLC(NULL, aNewComponentTestLC, NULL, loggingInfo);
#endif //LOG_PANIC_UNIT_TEST_FAILURE

	_LIT(KControllerBuilt,"Test Controller built with the following tests...\n");
	aRTest.Printf(KControllerBuilt);

	// Get a list of the available tests and display them
	const RPointerArray<CComponentInfo>& testList = theController->FindComponents();
	TInt numTests = testList.Count();
	if(numTests)
		{
		for(TInt index = 0; index < numTests; ++index)
			{
			//Print all the unit tests
			const RPointerArray<CUnitTestInfo>& transList = testList[index]->UnitTestsInfo();
			TInt numTrans = transList.Count();
			if(numTrans)
				{
				for(TInt transIndex = 0; transIndex < numTrans; ++transIndex)
					{
					//Print the test component name
					aRTest.Printf(KUnitTestMessage,
						transIndex+1, &(transList[transIndex]->UnitTestId()));
					}
				}
			else
				aRTest.Printf(KNoTestsMessage);			// No tests found
			}

		// Run the tests
		theController->Start();
		}
	else
		aRTest.Printf(KNoTestsMessage);			// No tests found

	CleanupStack::PopAndDestroy(2, loggingInfo);
	}


/**
 	@fn				TInt E32Main_TestHarness(TNewComponentTestLC aNewComponentTestLC)
	Intended Usage	: Main entry point to the console app called by E32
	@param			aNewComponentTestLC A pointer to defined into ECOM test app global
					function. It should create derived CComponentTester object which
					knows about the tests required.
	@return			TInt KErrNone.
	@leave  		KErrNoMemory, Any other codes possible from a test.
	@pre 			aNewComponentTestLC != NULL.
	@since			7.0s
*/
EXPORT_C TInt E32Main_TestHarness(TNewComponentTestLC aNewComponentTestLC)
    {
	// Set up for heap leak checking
	__UHEAP_MARK;

	// Obtain for the system the exeutable filename of this process
	RProcess current;
	TParse exeFilename;
	exeFilename.SetNoWild(current.FileName(), NULL, NULL);

	// Startup the RTest framework
	RTest rTest(exeFilename.Name());
	rTest.Title();
	rTest.Start(_L(" @SYMTestCaseID:SYSLIB-FORM-LEGACY-TESTHARNESSTEMPLATE-0001 Test Bed Tester EXE "));

	// Leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	RThread thisThread;
	thisThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Create the clean up stack.
	CTrapCleanup* cleanup = CTrapCleanup::New();

	// Call the main function and trap the result
	TRAPD(retCode, DoTestingL(rTest, aNewComponentTestLC)); // perform test
	rTest(retCode==KErrNone);

 	// This line added to close handles associated to last plugin loaded.
 	// The last plugin DLL is only unloaded when the client makes another call into ECom
 	//   (assuming the client has destroyed the implementation).
 	// TLS area cleaned up only when DLL unloaded or thread ends.
 	// FinalClose() ensures that the last DLL is unloaded and TLS cleaned up.
 	// Memory leaks/open handles will occur if this is not called.
 	//
 	// If ECom is not used should have no effect.
 	REComSession::FinalClose();

	// Destroy the curernt cleanup stack
	delete cleanup;

	// Check for open handles
	thisThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
	if(startThreadHandleCount != endThreadHandleCount)
		{__DEBUGGER()}							// Oops leaked some handles

	// End the testing
	rTest.Next(_L("/n"));
	rTest.End();
	//rTest.Getch();
	rTest.Close();

	// End heap leak checking and exit
	__UHEAP_MARKEND;
	return KErrNone;
    }
