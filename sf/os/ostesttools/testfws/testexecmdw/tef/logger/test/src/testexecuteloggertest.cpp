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
* Demonstration use of the TestExecute standalone logger client API
*
*/



/**
 @file TestExecuteLoggerTest.cpp
*/
#include <test/testexecutelog.h>
#include <e32cons.h>
#include <test/wrapperutilsplugin.h>

_LIT(KLogFile1,"?:\\TestExecuteLoggerTest.txt");

_LIT(K16BitText,"This is a short line of 16 Bit Text");
_LIT8(K8BitText,"This is a short line of  8 Bit Text");

_LIT(K16BitFormatText,"This is a short line of 16 Bit Format Text int = %d string = %S");
_LIT8(K8BitFormatText,"This is a short line of  8 Bit Format Text int = %d string = %S");
_LIT(K16BitString,"The String16");
_LIT8(K8BitString,"The String8");

LOCAL_C void MainL()
	{
	TDriveName defaultSysDrive(KTEFLegacySysDrive);

	RFs fileServer;
	TVersionName version(fileServer.Version().Name());
	
	if (fileServer.Version().iMajor >= 2 &&
		fileServer.Version().iBuild >= 1100)
		{
		RLibrary pluginLibrary;
		TInt pluginErr = pluginLibrary.Load(KTEFWrapperPluginDll);
		if (pluginErr == KErrNone)
			{
			TLibraryFunction newl;
			newl = pluginLibrary.Lookup(2);
			CWrapperUtilsPlugin* plugin = (CWrapperUtilsPlugin*)newl();
			TDriveUnit driveUnit(plugin->GetSystemDrive());
			defaultSysDrive.Copy(driveUnit.Name());
			delete plugin;
			pluginLibrary.Close();
			}
		}

	TFileName logFile1(KLogFile1);
	logFile1.Replace(0, 2, defaultSysDrive);

	_LIT(KTitle,"TestExecute Standalone Logger Test Code");
	CConsoleBase* console = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);

	_LIT(KMessage1,"Connect() To Logger and Close() immediately\r\n");
	console->Printf(KMessage1);
	RTestExecuteLogServ logger;
	User::LeaveIfError(logger.Connect());
	logger.Close();

	_LIT(KMessage2,"Immediate re-Connect() To Logger and Close() immediately\r\n");
	console->Printf(KMessage2);
	User::LeaveIfError(logger.Connect());
	logger.Close();

	_LIT(KMessage3,"Immediate re-Connect() To Logger CreateLog() and Close() immediately\r\n");
	console->Printf(KMessage3);
	User::LeaveIfError(logger.Connect());
	User::LeaveIfError(logger.CreateLog(logFile1,RTestExecuteLogServ::ELogModeOverWrite));
	logger.Close();

	// OS needs time to shutdown the server
	// No delay means we get server terminated from the OS
	User::After(1000000);

	_LIT(KMessage4,"Connect() To Logger CreateLog() %S Call API's\r\n");
	console->Printf(KMessage4,&logFile1);
	User::LeaveIfError(logger.Connect());
	User::LeaveIfError(logger.CreateLog(logFile1,RTestExecuteLogServ::ELogModeOverWrite));
	TBuf<20> buf16(K16BitString);
	TBuf8<20> buf8(K8BitString);
	logger.Write(K16BitText);
	logger.Write(K8BitText);
	logger.WriteFormat(K16BitFormatText,16,&buf16);
	logger.WriteFormat(K8BitFormatText,8,&buf8);
	logger.LogExtra(((TText8*)__FILE__), __LINE__,ESevrInfo,K16BitFormatText,1,&buf16);

	_LIT(KMessage5,"ALL API's Called - Call Close(),\r\nHit Any Key\r\n");
	console->Printf(KMessage5);
	console->Getch();
	logger.Close();

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
	_LIT(KPanic,"LoggerTest");
	__ASSERT_ALWAYS(!err, User::Panic(KPanic,err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
