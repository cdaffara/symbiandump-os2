/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_policy.h"
#include "t_input.h"
#include "t_output.h"
#include "t_testhandler.h"
#include "utf.h"

#include <s32file.h>

// run failure tests first

_LIT8(KTestExeStart, "<testexe>");
_LIT8(KExcludedCapsStart, "<excludedcapabilities>");
_LIT8(KPolicyStart, "<policy>");
_LIT8(KPreActionsStart, "<preactions>");
_LIT8(KPassAcionStart, "<passactions>");
_LIT8(KFailAcionStart, "<failactions>");
_LIT8(KPostActionsStart, "<postactions>");
_LIT8(KSecureIdStart, "<secureid>");
_LIT8(KSecureIdEnd, "</secureid>");
_LIT8(KVendorIdStart, "<vendorid>");
_LIT8(KVendorIdEnd, "</vendorid>");

_LIT(KFormat,"Action Name : %S \n");
_LIT(KSetCapExe, "setcap");
_LIT(KSetCapExe2, "setcap : ");
//NOTE :If the below literal is uncommented , remove the c:\hardcoded reference
// and replace with RFs::GetSystemDrive().
//_LIT(KTestExeTmpPath, "c:\\sys\\bin\\policytest_exe.exe");
_LIT(KPassScriptPath, "\\policytest_script_pass.txt");
_LIT(KFailScriptPath, "\\policytest_script_fail.txt");
_LIT(KLogTmpPath, "\\policytest_log.txt");
_LIT(KTestPath, "policytest_");
_LIT(KFailTestRunning, "Fail Test Script Running");
_LIT(KPassTestRunning, "Pass Test Script Running");


const TUint KCapabilityAll = 0xffffffff;

CPolicyTest* CPolicyTest::NewL(CConsoleBase& aConsole, 
							   Output& aOut,
							   const TTestActionSpec& aTestActionSpec)
	{
	CPolicyTest* self = CPolicyTest::NewLC(aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CPolicyTest* CPolicyTest::NewLC(CConsoleBase& aConsole, 
								Output& aOut,
								const TTestActionSpec& aTestActionSpec)
	{
	CPolicyTest* self = new(ELeave) CPolicyTest(aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CPolicyTest::CPolicyTest(CConsoleBase& aConsole, 
						 Output& aOut)
    : CTestAction(aConsole, aOut)
	{
	}

void CPolicyTest::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iExpectedResult = KErrNone;

	User::LeaveIfError(iFs.Connect());

	RProcess thisProcess;
	User::LeaveIfError(thisProcess.Open(thisProcess.Id()));
	iProcessSecureId = thisProcess.SecureId();
	iProcessVendorId = thisProcess.VendorId();
	thisProcess.Close();
	
	SetTestExeL(Input::ParseElement(aTestActionSpec.iActionBody, KTestExeStart));
	Input::ParseCapabilitySetL(Input::ParseElement(aTestActionSpec.iActionBody, KExcludedCapsStart), iExcludedCaps);
	SetPolicyL(Input::ParseElement(aTestActionSpec.iActionBody, KPolicyStart));
	iPreActions = Input::ParseElement(aTestActionSpec.iActionBody, KPreActionsStart).AllocL();
	SetTestActionL(Input::ParseElement(aTestActionSpec.iActionBody, KPassAcionStart),
				   Input::ParseElement(aTestActionSpec.iActionBody, KFailAcionStart));
	iPostActions = Input::ParseElement(aTestActionSpec.iActionBody, KPostActionsStart).AllocL();
	}

CPolicyTest::~CPolicyTest()
	{
	iFs.Close();
	delete iTestExe;
	iCapabilities.Close();
	delete iPreActions;
	delete iPassAction;
	delete iFailAction;
	delete iPostActions;
	iProcess.Close();
	}

void CPolicyTest::BadUsageL(const TDesC& aMessage)
	{
	iOut.writeString(_L("Error in script action testpolicy"));
	iOut.writeNewLine();
	iOut.writeString(aMessage);
	iOut.writeNewLine();
	User::Leave(KErrArgument);
	}

void CPolicyTest::SetTestExeL(const TDesC8& aPath)
	{
	if (aPath == KNullDesC8)
		{
		BadUsageL(_L("testexe not specified"));
		}
	
	iTestExe = HBufC::NewMaxL(aPath.Length());
	TPtr ptr = iTestExe->Des();
	ptr.Copy(aPath);
	}

void CPolicyTest::SetPolicyL(const TDesC8& aSpec)
	{
	iSecureId = Input::ParseIntElement(aSpec, KSecureIdStart, KSecureIdEnd);
	iVendorId = Input::ParseIntElement(aSpec, KVendorIdStart, KVendorIdEnd);

	TCapabilitySet capSet;
	Input::ParseCapabilitySetL(aSpec, capSet);

	// Extract capabilities into array
	for (TInt i = 0 ; i < ECapability_Limit ; ++i)
		{
		TCapability c = static_cast<TCapability>(i);
		if (capSet.HasCapability(c))
			{
			User::LeaveIfError(iCapabilities.Append(c));
			}
		}	
	}

void CPolicyTest::SetTestActionL(const TDesC8& aPassAction, const TDesC8& aFailAction)
	{
	if (aPassAction == KNullDesC8)
		{
		BadUsageL(_L("passactions not specified"));
		}

	iPassAction = aPassAction.AllocL();

	if (aFailAction == KNullDesC8)
		{
		BadUsageL(_L("failactions not specified"));
		}

	iFailAction = aFailAction.AllocL();
}

void CPolicyTest::PerformAction(TRequestStatus& aStatus)
	{
	if (aStatus < 0)
		{
		iState = EFinished;
		}

	switch (iState)
		{
		case EInit:
			{
			TDriveUnit sysDrive (RFs::GetSystemDrive());
			TDriveName sysDriveName (sysDrive.Name());
				
			TBuf<128> scriptFile (sysDriveName);
			scriptFile.Append(KPassScriptPath);
			WriteScriptFileL(scriptFile, *iPassAction);	
			
			scriptFile.Copy(sysDriveName);
			scriptFile.Append(KFailScriptPath);
			WriteScriptFileL(scriptFile, *iFailAction);
			}
			// fall through
		
		case ESetupTest:
			GetNextTest();
			if (iTestState == ETestFinished)
				{
				iState = EFinished;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNone);
				}
			else
				{
				SetupTestL(aStatus);
				iState = ERunTest;
				}
			break;

		case ERunTest:
			CheckProcessTermintationL();
			RunTestL(aStatus);
			iState = EProcessResults;
			break;
			
		case EProcessResults:
			CheckProcessTermintationL();
			ProcessResultsL(aStatus);
			iState = ESetupTest;
			break;

		case EFinished:
			iActionState = EPostrequisite;				
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			break;
		}
	}

void CPolicyTest::StartProcessL(const TDesC& aExe, const TDesC& aCommandLine, TRequestStatus& aStatus)
	{
	iOut.writeString(_L("Starting child process: "));
	iOut.writeString(aExe);
	iOut.writeString(_L(" "));
	iOut.writeString(aCommandLine);
	iOut.writeNewLine();
	
	User::LeaveIfError(iProcess.Create(aExe, aCommandLine));
	iProcess.Logon(aStatus);
	iProcess.Resume();
	}

void CPolicyTest::CheckProcessTermintationL()
	{
	if (iProcess.ExitType() == EExitPanic)
		{
		iOut.writeString(_L("Child process panicked: "));
		iOut.writeString(iProcess.ExitCategory());
		iOut.writeString(_L(" "));
		iOut.writeNum(iProcess.ExitReason());
		iOut.writeNewLine();
		User::Leave(KErrGeneral);
		}
	ASSERT(iProcess.ExitType() == EExitKill);
	iProcess.Close();
	}

void CPolicyTest::GetNextTest()
	{
	// Step through capabilities to be tested
	if (iTestState == ETestFailCap)
		{
		++iCapIndex;
		if (iCapIndex < iCapabilities.Count())
			return;
		}
	
	// Step through possible tests until we hit a vaild test
	do
		{
		iTestState = static_cast<TTestState>(iTestState + 1);
		}
	while (!((iTestState == ETestFailSID && iSecureId) ||
			 (iTestState == ETestFailVID && iVendorId) ||
			 (iTestState == ETestFailCap && iCapabilities.Count()) ||
			 (iTestState == ETestPass) ||
			 (iTestState == ETestFinished)));
	}

void CPolicyTest::SetupTestL(TRequestStatus& aStatus)
	{
	switch (iTestState)
		{
		case ETestFailSID:
			{
			TInt wrongSecureId = iSecureId + 1;
			iOut.write(_L("Failure test: Wrong SID (%08x):\n\n"), wrongSecureId);			
			SetTestSecurityInfoL(wrongSecureId, iVendorId, KCapabilityAll, aStatus);
			}
			break;

		case ETestFailVID:
			{
			TInt wrongVendorId = iVendorId + 1;
			iOut.write(_L("Failure test: Wrong VID (%08x):\n\n"), wrongVendorId);
			SetTestSecurityInfoL(iSecureId, wrongVendorId, KCapabilityAll, aStatus);
			}
			break;

		case ETestFailCap:
			{
			TCapability missingCap = iCapabilities[iCapIndex];
			
			iOut.writeString(_L("Failure test: Missing capability ("));
			iOut.writeCapabilityL(missingCap);
			iOut.writeString(_L("):\n\n"));

			TUint capSet = ~ (1 << missingCap);			
			SetTestSecurityInfoL(iSecureId, iVendorId, capSet, aStatus);
			}
			break;

		case ETestPass:
			{
			iOut.write(_L("Pass test:\n\n"));			

			TUint capSet = 0;
			for (TInt i = 0 ; i < iCapabilities.Count() ; ++i)
				{
				capSet |= 1 << iCapabilities[i];
				}
			
			SetTestSecurityInfoL(iSecureId, iVendorId, capSet, aStatus);
			}
			break;

		default:
			User::Invariant();
		}
	}

void CPolicyTest::SetTestSecurityInfoL(TInt aSecureId, TInt aVendorId, TUint aCapSet, TRequestStatus& aStatus)
	{
	// Remove excluded capabilities
	for (TInt i = 0 ; i < ECapability_Limit ; ++i)
		{
		if (iExcludedCaps.HasCapability(static_cast<TCapability>(i)))
			{
			aCapSet &= ~ (1 << i);
			}
		}
	
	TBuf<128> commandLine;
	commandLine.AppendFormat(_L("%S %08x "), iTestExe, aCapSet);
	if (aSecureId)
		{
		commandLine.AppendFormat(_L("-SID %08x "), aSecureId);
		}
	if (aVendorId)
		{
		commandLine.AppendFormat(_L("-VID %08x "), aVendorId);
		}
   // commandLine.Append(KTestExeTmpPath);
    iTestExeTmpNewPath = KTestPath;
	iTestExeTmpNewPath.Append(*iTestExe);
	commandLine.Append(iTestExeTmpNewPath);
    
    TBuf<128> isetcapTmpNewPath1; //stores the value of commandline
    TBuf<128> isetcapTmpNewPath;  //stores the value of KsetCapexe2
	isetcapTmpNewPath = KSetCapExe2;
	isetcapTmpNewPath1= commandLine;
	isetcapTmpNewPath.Append(isetcapTmpNewPath1);
	RDebug::RawPrint(isetcapTmpNewPath);
	StartProcessL(KSetCapExe, commandLine, aStatus);
	
	}

void CPolicyTest::WriteScriptFileL(const TDesC& aPath, const TDesC8& aAction)
	{	
	iFs.Delete(aPath); // ignore errors
	
	RFile file;
	User::LeaveIfError(file.Create(iFs, aPath, EFileShareExclusive | EFileWrite));
	CleanupClosePushL(file);
	
	User::LeaveIfError(file.Write(*iPreActions));
	User::LeaveIfError(file.Write(aAction));	
	User::LeaveIfError(file.Write(*iPostActions));
	
	CleanupStack::PopAndDestroy(&file);
	}

void CPolicyTest::RunTestL(TRequestStatus& aStatus)
	{
	  
	HBufC* hptr16; 
	hptr16 = CnvUtfConverter::ConvertToUnicodeFromUtf8L(*iNameInfo);
	RDebug::Print(KFormat,hptr16);
	delete hptr16;
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	
	TBuf<128> passScriptFile (sysDriveName);
	passScriptFile.Append(KPassScriptPath);
	
	TBuf<128> failScriptFile (sysDriveName);
	failScriptFile.Append(KFailScriptPath);
			
	TPtrC script = (iTestState == ETestPass) ? passScriptFile : failScriptFile;
	(iTestState == ETestPass) ? RDebug::RawPrint(KPassTestRunning) : RDebug::RawPrint(KFailTestRunning);
  	
	
	TBuf<128> logTmpFile (sysDriveName);
	logTmpFile.Append(KLogTmpPath);
	iFs.Delete(logTmpFile); // ignore errors
	
	TBuf<128> commandLine;
	commandLine.AppendFormat(_L("%S %S"), &script, &logTmpFile);	
	
	StartProcessL(iTestExeTmpNewPath, commandLine, aStatus);
	}

void CPolicyTest::ProcessResultsL(TRequestStatus& aStatus)
	{
	_LIT8(KSummaryLine, " tests failed out of ");
	_LIT8(KNewLine, "\r\n");

	TInt failCount = KErrNotFound, runCount;
	
	// Read entire log file into memory to process
	RFile file;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> logTmpFile (sysDrive.Name());
	logTmpFile.Append(KLogTmpPath);
	User::LeaveIfError(file.Open(iFs, logTmpFile, EFileShareReadersOnly | EFileRead));
	CleanupClosePushL(file);

	TInt size;
	User::LeaveIfError(file.Size(size));
	HBufC8* buffer = HBufC8::NewLC(size);
	TPtr8 ptr = buffer->Des();

	User::LeaveIfError(file.Read(ptr));

	iOut.writeString(_L("Child test output:\n"));

	TInt pos = 0;
	while (pos < size)
		{
		TInt nextNewline = buffer->Mid(pos).Find(KNewLine);

		// Split buffer into lines
		TPtrC8 line;
		if (nextNewline == KErrNotFound)
			{
			line.Set(buffer->Mid(pos));
			}
		else
			{
			line.Set(buffer->Mid(pos, nextNewline + KNewLine().Length()));
			}
		pos += line.Length();

		// Search for summary line
		TInt pos2 = line.Find(KSummaryLine);
		if (pos2 != KErrNotFound)
			{
			// Parse the summary line to work out if the test passed
			TLex8 lex1(line.Left(pos2));
			TInt err1 = lex1.Val(failCount);
			TLex8 lex2(line.Mid(pos2 + KSummaryLine().Length()));
			TInt err2 = lex2.Val(runCount);

			if (err1 != KErrNone || err2 != KErrNone)
				{
				iOut.writeString(_L("Failed to parse summary line\n"));
				User::LeaveIfError(err1);
				User::LeaveIfError(err2);
				}
			}
		else
			{
			// Don't print the summary line as this will confuse whatever parsed
			// the main log
			iOut.writeString(_L("> "));
			iOut.writeString(line);
			}		
		}
	
	if (failCount == KErrNotFound)
		{
		iOut.writeString(_L("Couldn't find summary line in test output\n"));
		User::Leave(KErrNotFound);
		}
	iFailCount += failCount;

	// Print results in different format
	iOut.write(_L("Tests run: %d\n"), runCount);
	iOut.write(_L("Tests failed: %d\n"), failCount);
	iOut.writeNewLine();
	
	CleanupStack::PopAndDestroy(2, &file);

	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CPolicyTest::PerformCancel()
	{
	// not implemented - need to pass original status object to LogonCancel
	User::Invariant(); 
	/*
	switch (iState)
		{
		case ESetCapsPass:
		case ERunTest:
			iProcess.LogonCancel();
			iProcess.Kill(KErrCancel);
			iProcess.Close();
			break;
		}
	*/
	}

void CPolicyTest::Reset()
	{
	iProcess.Close();
	iState = ESetupTest;
	iTestState = ETestNone;
	iCapIndex = -1;
	iFailCount = 0;
	}

void CPolicyTest::DoReportAction()
	{
	iOut.writeString(_L("Running policy tests...\n\n"));
	}

void CPolicyTest::DoCheckResult(TInt aError)
	{
	if (aError == KErrNone && iFailCount > 0)
		{
		iOut.write(_L("%d tests failed\n"), iFailCount);
		aError = KErrGeneral;
		}
	
	iResult = (aError == iExpectedResult);
	}
