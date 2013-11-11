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
* Unit test for TestExecuteUtils , CTestExecuteLogger
* Code for logging over serial connection
* please make sure the logging channel is set to 'Serial' or Both 
* in the testexecute.ini file for this code to be able to cover the 
* serial logging code...
*
*/



/**
 @file TestExecuteLoggingTest.cpp
*/
#include <test/testexecutelogger.h>
#include <e32cons.h>

_LIT(K16BitText,"This is a short line of 16 Bit Text");
_LIT8(K8BitText,"This is a short line of  8 Bit Text");
_LIT(K16BitFormatText,"This is a short line of 16 Bit Format Text int = %d string = %S");
_LIT8(K8BitFormatText,"This is a short line of  8 Bit Format Text int = %d string = %S");
_LIT(K16BitString,"The String16");
_LIT8(K8BitString,"The String8");


LOCAL_C void TestWorkAPIsL(CTestExecuteLogger& aLogger,CConsoleBase*& aConsole)
{

	aConsole->Write(_L("Testing logger.Write(16BitText) \r\n"));
	aLogger.Write(K16BitText);
	
	aConsole->Write(_L("Testing logger.Write(8BitText) \r\n"));
	aLogger.Write(K8BitText);

	aConsole->Write(_L("Testing logger.WriteFormat(16BitText) \r\n"));
	TBuf<20> buf16(K16BitString);
	aLogger.WriteFormat(K16BitFormatText,16,&buf16);

	aConsole->Write(_L("Testing logger.WriteFormat(8BitText) \r\n"));
	TBuf8<20> buf8(K8BitString);
	aLogger.WriteFormat(K8BitFormatText,8,&buf8);

	aConsole->Write(_L("Testing logger.LogExtra() \r\n"));
	aLogger.LogExtra(((TText8*)__FILE__), __LINE__,ESevrInfo,K16BitFormatText,1,&buf16);

	aConsole->Write(_L("Testing logger.PrintCurrentScriptLine() \r\n"));
	TBuf<30> output(_L("Testing PrintCurrentScriptLine")) ; 
	aLogger.PrintCurrentScriptLine(output) ; 
	
	aConsole->Write(_L("Testing logger.LogTestCaseResult() \r\n"));
	TBuf<30> file(_L("TestExcuteLoggingTest.cpp"));
	TInt lineNo(68) ; 
	TBuf<20> testCsNm(_L("TestCaseSomething"));
	aLogger.LogTestCaseResult(file, lineNo, ESevrInfo, KTEFEndTestCaseCommand, testCsNm);

	aConsole->Write(_L("Testing logger.LogResult() \r\n"));
	TBuf<30> panicStringbuf(_L("Panic string")) ; 
	TBuf<30> fileRes(_L("TestExcuteLoggingTest.cpp"));
	TInt lineNum(70) ; 
	TInt severity(RFileFlogger::ESevrHigh) ; 

	aLogger.LogResult(EPass,panicStringbuf,lineNum,KTEFRunTestStepCommand,fileRes,RFileFlogger::ESevrHigh);
}


LOCAL_C void MainL()
	{
	_LIT(KTitle,"TestExecuteLogger Test Code for serial logging");
	CConsoleBase* console = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	CTestExecuteLogger logger;

	TInt logLevel =1  ;
	TBool separateLogFiles(EFalse); 
	_LIT(KScriptPath,"E:\\plattest\\Selective.script");
	TPtrC scriptFilePath(KScriptPath);

	console->Write(_L("logger.InitialiseLoggingL next2 \n")) ; 
	logger.InitialiseLoggingL(scriptFilePath, separateLogFiles, logLevel);
	
	console->Write(_L("post logger.InitialiseLoggingL next \n"));
	TestWorkAPIsL(logger, console)	;
	
	console->Write(_L("attempting TerminateLoggingL \n")) ; 
	logger.TerminateLoggingL(3, 4, 3); //suggested by todays fortune cookie...

	console->Write(_L("Done testing, press a key to finish \n")) ; 
	console->Getch() ; 
	CleanupStack::PopAndDestroy(console);	
	}

// Entry point for all Epoc32 executables
// See PSP Chapter 2 Getting Started
GLDEF_C TInt E32Main()
	{
	// Heap balance checking
	// See PSP Chapter 6 Error Handling
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	_LIT(KPanic,"LoggingTest");
	__ASSERT_ALWAYS(!err, User::Panic(KPanic,err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
