/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#include "t_testhandler.h"
#include "ttesthandlersettings.h"
#include "t_output.h"
#include "t_testaction.h"
#include "t_testsetup.h"
#include "tTestSpec.h"
#include "t_testrunner.h"


TTestSummary::TTestSummary() :
	iTestsRun(0), iTestsFailed(0)
	{
	}

void TTestSummary::PrintL(Output& aOut)
	{
	aOut.write(_L("%d tests failed out of %d\n"), iTestsFailed, iTestsRun);
	aOut.write(_L("\r\n</pre></body></html>\r\n"));
	}

TBool TTestSummary::AllTestsPassed()
	{
	return iTestsRun > 0 && iTestsFailed == 0;
	}

EXPORT_C CTestHandler* CTestHandler::NewLC(RFs& aFs,
										   MTestSpec& aTestSpec,
										   CTestHandlerSettings& aSettings,
										   CConsoleBase* aConsole,
										   Output* aOut)
	{
	CTestHandler* self = new(ELeave) CTestHandler(aFs, aTestSpec, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aSettings);
	return self;
	}

CTestHandler::~CTestHandler()
	{
	delete iSettings;
	delete iScheduler;
	delete iSharedData;
    delete iTestRunner;
    delete iNextTestRunner;

	iFailedTests.Reset();
	iFailedTestNames.ResetAndDestroy();
	iKnownDefects.ResetAndDestroy();
	}

CTestHandler::CTestHandler(RFs& aFs,
						   MTestSpec& aTestSpec,
						   CConsoleBase* aConsole,
						   Output* aOut) :
	iFs(aFs), iConsole(aConsole), iOut(aOut), iTestSpec(aTestSpec)
	{
	}

void CTestHandler::ConstructL(CTestHandlerSettings& aSettings) 
	{
	iSettings = CTestHandlerSettings::NewL(aSettings);
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);

	// Set up test handler based on command line argument
	if (aSettings.iOOM)
		{
		SetTestRunnerL(new (ELeave) COOMTestRunner(*iOut));
		}
	else if (aSettings.iCancel)
		{
		SetTestRunnerL(new (ELeave) CCancelTestRunner(*iOut));
		}
	else
		{		
		SetTestRunnerL(NULL);
		}
	}

TTestSummary CTestHandler::Summary()
	{
	TTestSummary result;
	result.iTestsRun = iActionCount;
	result.iTestsFailed = iFailedTests.Count();
	return result;
	}

EXPORT_C void CTestHandler::RunTestsL()
 {
	
	TBuf8<32>timeBuf;
	TTime time;
	time.UniversalTime();
	TDateTime dateTime = time.DateTime();
	_LIT8(KDateFormat,"%02d:%02d:%02d:%03d "); 
	timeBuf.AppendFormat(KDateFormat,dateTime.Hour(),dateTime.Minute(),dateTime.Second(),(dateTime.MicroSecond()/1000));

	iActionNumber = 0;
    CTestAction* action;
    HBufC8* currentID=NULL;
    HBufC8* previousID=NULL;
    TBool testCaseResult=ETrue;
    TBool isTefScript = EFalse;
    while(iTestSpec.GetNextTest(action))
        {
        iActionCount++;
        iActionNumber++;
        isTefScript = action->iTefScript;
//        action->iTefScript = EFalse;
        delete previousID;
        previousID = currentID;
        currentID = action->iNameInfo->AllocLC();
        
        if ( (previousID == NULL) || (*currentID != *previousID) )
        	{
        	if (previousID != NULL)
        		{  
        				
        		
        		if(action->iTefScript)
        			{
        			iOut->writeString(timeBuf);
        			iOut->writeString(_L("Command = END_TESTCASE "));
        			}
	    		iOut->writeString(*previousID);
	    		if(action->iTefScript)
	    			{
	    			iOut->writeString(_L(" ***TestCaseResult = "));
	    			}

	    		if (testCaseResult)
	    			{
	    			iOut->writeString(_L("PASS"));
	    			}
	    		else
	    			{
	    			iOut->writeString(_L("FAIL"));
	    			}
		    	iOut->writeString(_L("\r\n"));
		    	iOut->writeString(_L("\r\n"));
        		}
        	
        	
        	if(action->iTefScript)
        		{
        		iOut->writeString(timeBuf);
    	        iOut->writeString(_L("Command = START_TESTCASE  "));
        		}
	    	iOut->writeString(*(currentID));
	    	iOut->writeString(_L("\r\n"));
	    	testCaseResult = ETrue;
        	}
       
        TRAPD(err, RunTestL(action));
        if (!action->iResult)
        	{
        	testCaseResult = EFalse;
        	}
        if (err != KErrNone)
            {
            FailTestL(action, EFailInTestHandler, err);
            }
        CleanupStack::Pop(currentID);
	    }
   
    if (currentID != NULL)
    	{    	
    	    	
		
		if(isTefScript)
			{
			iOut->writeString(timeBuf);
			iOut->writeString(_L("Command = END_TESTCASE "));
			}
		iOut->writeString(*currentID);
		if(isTefScript)
			{
			iOut->writeString(_L(" ***TestCaseResult = "));
			}
		
		if (testCaseResult)
			{
			iOut->writeString(_L("PASS"));
			}
		else
			{
			iOut->writeString(_L("FAIL"));
			}
		iOut->writeString(_L("\r\n"));
		iOut->writeString(_L("\r\n"));
		}	
	delete previousID;
	delete currentID;
    
    DisplaySummary();
 }



void CTestHandler::RunTestL(CTestAction* aAction)
    {
    // If the last test set a new test runner, install it here
    if (iNextTestRunner)
        {
        delete iTestRunner;
        iTestRunner = iNextTestRunner;
		iNextTestRunner = NULL;
        }

    iOut->writeString(_L("Test "));
    iOut->writeNum(iActionNumber);
    iOut->writeNewLine();
    
    if (iActionNumber > 1)
		{
		iConsole->Printf(_L(", "));
		}
	iConsole->Printf(_L("%d: "), iActionNumber);

    aAction->ReportAction();
    aAction->SetTestHandler(*this); 

    TInt preErr = iTestRunner->PerformPrerequisiteL(aAction);
    if ((preErr != KErrNone) && !aAction->iResult)
        {
        FailTestL(aAction, EFailInPrerequisite, preErr);
        return;
        }
	TInt actionErr = 0;
	if (preErr == KErrNone)
		{
		actionErr = iTestRunner->PerformActionL(aAction);
		}
    TInt postErr = iTestRunner->PerformPostrequisiteL(aAction, actionErr);
	// investiage if passing actionErr would make more sense here
    aAction->CheckResult(postErr);    
    if (!aAction->iResult)
        {
        FailTestL(aAction, EFailInAction, actionErr);
        return;
        }

	iConsole->Printf(_L("passed"));
    }

const TDesC& CTestHandler::GetFailLocationName(TTestFailLocation aLoc)
    {
    _LIT(KTestHandler, "test handler");
    _LIT(KPrerequisite, "prerequisite");
    _LIT(KTestAction, "test action");

    const TDesC* result = NULL;

    switch (aLoc)
        {
        case EFailInTestHandler:
            result = &KTestHandler;
            break;
        case EFailInPrerequisite:
            result = &KPrerequisite;
            break;
        case EFailInAction:
            result = &KTestAction;
            break;
        }
    
    ASSERT(result);
    return *result;
    }

void CTestHandler::FailTestL(CTestAction* aAction, TTestFailLocation aLoc, TInt aErr)
    {
    iOut->writeString(_L("Test failed in "));
    iOut->writeString(GetFailLocationName(aLoc));
    iOut->writeString(_L(": "));
    iOut->writeNum(aErr);
    iOut->writeNewLine();
    
	iConsole->Printf(_L("failed"));

	AppendFailedTestL(iActionNumber, *aAction->iNameInfo);
    if(aAction->ScriptError() == CTestAction::EFileNotFound)
        {
        TBuf<KMaxErrorSize> scriptError;
        aAction->ScriptError(scriptError);
        iOut->writeString(_L("File Not Found \""));
        iOut->writeString(scriptError);
        iOut->writeString(_L("\" in test "));
        iOut->writeNum(iActionNumber);
        iOut->writeNewLine();
        }

    if (aAction->iKnownFailure)
        {//	Expecting a failure because of known defect - record details
        HBufC* name=HBufC::NewLC(aAction->iNameInfo->Length());
        name->Des().Copy(*(aAction->iNameInfo));
        User::LeaveIfError(iKnownDefects.Append(name));
        CleanupStack::Pop(name);
        }
    }

void CTestHandler::AppendFailedTestL(TInt aIndex, const TDesC8& aName)
	{
	iFailedTests.Append(aIndex);
    HBufC* name=HBufC::NewLC(aName.Length());
    name->Des().Copy(aName);
	User::LeaveIfError(iFailedTestNames.Append(name));
    CleanupStack::Pop(name);
	}
	
void CTestHandler::DisplaySummary()
	{
	//Display the RTest information
	iConsole->Printf(_L("\n"));

//	Log the details of tests that failed because of known (deferred) defects
	TInt theFailCount = iKnownDefects.Count();
	if (theFailCount > 0)
	{
		iOut->writeNewLine();
		iOut->writeString(_L("----------------------------------------------------------------------------------"));
		iOut->writeNewLine();
		iOut->writeString(_L("These tests may not pass because of known defects details of which are given below:"));
		iOut->writeNewLine();
		iOut->writeNewLine();
		
		TInt index = 0;
		for (; index<theFailCount; index++)
		{
			iOut->writeString(*iKnownDefects[index]);
			iOut->writeNewLine();
		}
		iOut->writeString(_L("----------------------------------------------------------------------------------"));
		iOut->writeNewLine();
		iOut->writeNewLine();
	}

	// Display the failed tests
	TInt iEnd = iFailedTests.Count();
	if (iEnd!=0)
		{
		iOut->writeString(_L(" Failed tests:-"));
		iOut->writeNewLine();
		for (TInt i = 0; i < iEnd; i++)
			{
			iOut->writeNum(iFailedTests[i]);
			iOut->writeSpaces(2);
			iOut->writeString(*iFailedTestNames[i]);
			iOut->writeNewLine();
			}
		iOut->writeNewLine();
		}
	 
	// Summary line is now printed in CTestSetup
	}

CBase* CTestHandler::SharedData() const
	{
	return iSharedData;
	}

void CTestHandler::SetSharedData(CBase* aData)
	{
	iSharedData = aData;
	}

EXPORT_C void CTestHandler::SetTestRunnerL(CTestRunner *aTestRunner)
    {
    /* We can't set iTestRunner directly, as we may be called while we're inside
     * one of its methods. */

	// This method can be called twice in a row, eg if the -o option is used
	if (iNextTestRunner)
		{
		delete iNextTestRunner;
		iNextTestRunner = NULL;
		}
	
    iNextTestRunner = aTestRunner ? aTestRunner : new (ELeave) CTestRunner(*iOut);
    }

void CTestHandler::SetHeapMark(TInt aAllocCount)
    {
    iHeapMark = aAllocCount;
    }

TInt CTestHandler::HeapMark()
    {
    return iHeapMark;
    }
