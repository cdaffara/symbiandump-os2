// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <e32uid.h>
#include <f32file.h>
#include <bautils.h>


///////////////////////////////////////////////////////////////////////////////////////

RTest TheTest(_L("t_sqlitetclstarter"));

_LIT(KTclExecutableName,"tclsqlite3.exe");
_LIT(KTclScriptName,"all.test");

_LIT(KStdioConfigFilePath,	"C:\\system\\data\\");
_LIT(KStdioConfigFile,	"C:\\system\\data\\config.ini");

_LIT(KLogsFilePath, "C:\\logs\\testexecute\\");

RFs theFs;

///////////////////////////////////////////////////////////////////////////////////////

//
// Creates the appropriate stdio ini file to redirect stdio output to a file 
//

void CreateIniFileForStdoutRedirection(void)
	{
	TInt err(KErrNone);

	// check path exists otherwise create
	if (!BaflUtils::FolderExists(theFs, KStdioConfigFilePath))
		{
		err = theFs.MkDirAll(KStdioConfigFilePath);
		TheTest(err == KErrNone, __LINE__);
		}

	
	// create file
	RFile file;
	err = file.Create(theFs, KStdioConfigFile, EFileRead | EFileWrite);	
	TheTest(err == KErrNone, __LINE__);

	TheTest(KErrNone == file.Write(_L8("[STDIO]\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("STDIN = MEDIA1\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("STDOUT = MEDIA2\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("[MEDIA1]\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("type = console\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("width = -1\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("height = -1\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("[MEDIA2]\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("type = file\r\n")), __LINE__);
	TheTest(KErrNone == file.Write(_L8("path = C:\\logs\\testexecute\\t_sqlitetclstarter.htm\r\n")), __LINE__);
	TheTest(KErrNone == file.Flush(),__LINE__);

	file.Close();

	}

///////////////////////////////////////////////////////////////////////////////////////

//
// Deletes the stdio ini file
//

void DeleteIniFileForStdoutRedirection(void)
	{
	TInt err = theFs.Delete(KStdioConfigFile);
	TheTest(err == KErrNone || err==KErrNotFound || err==KErrPathNotFound, __LINE__);
	}

///////////////////////////////////////////////////////////////////////////////////////

//
// Ensure the directory for the Logs file exists
//

void EnsureLogsDirectoryExists(void)
	{
	TInt err(KErrNone);

	// check path exists otherwise create
	if (!BaflUtils::FolderExists(theFs, KLogsFilePath))
		{
		err = theFs.MkDirAll(KLogsFilePath);
		TheTest(err == KErrNone, __LINE__);
		}
	}

///////////////////////////////////////////////////////////////////////////////////////

//
// Start the tcl process with a script
//

void StartTclProcess(void)
	{
	RProcess process;
	TInt err = process.Create(KTclExecutableName,KTclScriptName);
	TheTest(err==KErrNone, __LINE__);
	
	TRequestStatus processStatus;
	process.Logon(processStatus);

	process.Resume();

	TheTest.Printf(_L("Wait for TCL tests to finish\r\n"));

	User::WaitForRequest(processStatus);

	TheTest.Printf(_L("TCL tests finished %d\r\n"),processStatus.Int());

	TheTest(processStatus.Int() == KErrNone, __LINE__);
	}

///////////////////////////////////////////////////////////////////////////////////////

TInt E32Main()
	{
	TheTest.Title();
	TheTest.Start(_L("TCL tests"));

	CTrapCleanup* tc = CTrapCleanup::New();
	
	__UHEAP_MARK;
	
	TInt err = theFs.Connect();
	TheTest(err == KErrNone, __LINE__);

	DeleteIniFileForStdoutRedirection();

	CreateIniFileForStdoutRedirection();

	EnsureLogsDirectoryExists();

	StartTclProcess();

	DeleteIniFileForStdoutRedirection();

	theFs.Close();

	__UHEAP_MARKEND;
	
	TheTest.End();
	TheTest.Close();
	
	delete tc;
	
	User::Heap().Check();
	return KErrNone;
	}
