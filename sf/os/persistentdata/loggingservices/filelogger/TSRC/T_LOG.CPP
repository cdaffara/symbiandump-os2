// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <flogger.h>
#include <f32file.h>

GLDEF_D RTest test(_L("FLOGGER Test Code"));
GLDEF_D RFs fs;

_LIT(KTestLogFileName1,"Log1.txt");
_LIT(KTestLogFileName2,"Log2.txt");
_LIT(KTestLogDir1,"Test1");
_LIT(KTestLogDir2,"Test2");
_LIT(KTestLogDir3,"Test3");
_LIT(KTestLogDir4,"Test4");
_LIT(KTestLogDir5,"Test5");
_LIT(KTestLogFullDir1,"c:\\Logs\\Test1\\");
_LIT(KTestLogFullDir2,"c:\\Logs\\Test2\\");
_LIT(KTestLogFullDir3,"c:\\Logs\\Test3\\");
_LIT(KTestLogFullDir4,"c:\\Logs\\Test4\\");
_LIT(KTestLogFullDir5,"c:\\Logs\\Test5\\");
_LIT(KTestLogFile1,"c:\\Logs\\Test1\\Log1.txt");
_LIT(KTestLogFile2,"c:\\Logs\\Test1\\Log2.txt");
_LIT(KTestLogFile3,"c:\\Logs\\Test2\\Log1.txt");
_LIT(KTestLogFile4,"c:\\Logs\\Test3\\Log1.txt");

// 2.2.5 Wait for shutdown after closure of session 
LOCAL_C void TestWaitForShutDown()
	{

	
	test.Printf(_L("\nWaiting for FLogger server shutdown"));
	TInt i;
	for (i=30; i>0; i--)
		{
		TFullName name=_L("*");
		_LIT(serverName,"FLogger server");
		name.Append(serverName);
		TFindThread find(name);
		if(find.Next(name)==KErrNone)
			test.Printf(_L("."));
		else
			break;
		User::After(1000000);
		}
	
	test(i);
	test.Printf(_L("\nFLogger Server Shutdown after %d secs\n"), 29-i);
	}

LOCAL_C TInt DeleteFolder(const TDesC& aFolder)
	{
    CFileMan* fileman=NULL;
    TRAPD(ret,fileman=CFileMan::NewL(fs));
    ret=fileman->RmDir(aFolder);
    delete fileman;
    if (ret==KErrPathNotFound || ret==KErrNotFound)
        {
        return KErrNone;
        }
    return ret;
	}

LOCAL_C void CreateFolderL(const TDesC& aFolder)
	{

	TInt ret=fs.MkDirAll(aFolder);
	if (ret!=KErrNone && ret!=KErrAlreadyExists)
		User::Leave(ret);
	}

LOCAL_C void WriteToLog(RFileLogger& aLog)
	{

	// 16 bit writes
	TUint num1=100;
	aLog.WriteFormat(_L("Writing a number to the log: %d"),num1);
	test(aLog.LastError()==KErrNone);
	TUint num2=200;
	aLog.WriteFormat(_L("Writing two numbers to the log: %d, %d"),num1,num2);
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(ETrue,ETrue);
	aLog.Write(_L("Line should begin with date and time"));
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(ETrue,EFalse);
	aLog.Write(_L("Line should begin with date only"));
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(EFalse,ETrue);
	aLog.Write(_L("Line should begin with time only"));
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(EFalse,EFalse);
	aLog.Write(_L("Line should begin with nothing"));
	test(aLog.LastError()==KErrNone);
	aLog.Write(_L(""));
	test(aLog.LastError()==KErrNone);
	aLog.Write(_L("There should be a blank line above and below this one"));
	test(aLog.LastError()==KErrNone);
	aLog.Write(_L(""));
	test(aLog.LastError()==KErrNone);

	// 8 bit writes
	aLog.WriteFormat(_L8("Writing a number to the log: %d"),num1);
	test(aLog.LastError()==KErrNone);
	aLog.WriteFormat(_L8("Writing two numbers to the log: %d, %d"),num1,num2);
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(ETrue,ETrue);
	aLog.Write(_L8("Line should begin with date and time"));
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(ETrue,EFalse);
	aLog.Write(_L8("Line should begin with date only"));
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(EFalse,ETrue);
	aLog.Write(_L8("Line should begin with time only"));
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(EFalse,EFalse);
	aLog.Write(_L8("Line should begin with nothing"));
	test(aLog.LastError()==KErrNone);
	aLog.Write(_L8(""));
	test(aLog.LastError()==KErrNone);
	aLog.Write(_L8("There should be a blank line above and below this one"));
	test(aLog.LastError()==KErrNone);
	aLog.Write(_L8(""));
	test(aLog.LastError()==KErrNone);

// 2.2.1.5 Dump hexadecimal values
	// Hex dump
	const TText* hdr=_S("123456");
	const TText* mgn=_S("      ");
	const TUint8* ptr=_S8("abcdefghijklmnopqrstuvwxyz");
	TInt len=26;
	aLog.HexDump(hdr, mgn, ptr, len);
	test(aLog.LastError()==KErrNone);
	aLog.HexDump(NULL, mgn, ptr, len);
	test(aLog.LastError()==KErrNone);
	aLog.HexDump(hdr, NULL, ptr, len);
	test(aLog.LastError()==KErrNone);
	aLog.HexDump(hdr, mgn, NULL, len);
	test(aLog.LastError()==KErrNone);
	aLog.SetDateAndTime(ETrue,ETrue);
	aLog.HexDump(hdr, mgn, ptr, len);
	test(aLog.LastError()==KErrNone);
	aLog.HexDump(NULL, mgn, ptr, len);
	test(aLog.LastError()==KErrNone);
	aLog.HexDump(hdr, NULL, ptr, len);
	test(aLog.LastError()==KErrNone);
	aLog.HexDump(hdr, mgn, NULL, len);	
	test(aLog.LastError()==KErrNone);
	}

// 2.2.1 Writing to two open log files at a time 
// 2.2.1.3 Files are different 
LOCAL_C void testTwoClientsL(TBool aValid)
	{

	if (!aValid)
		test(DeleteFolder(KTestLogFullDir1)==KErrNone);
	else
		CreateFolderL(KTestLogFullDir1);

	RFileLogger log1;
	test(log1.Connect()==KErrNone);
	RFileLogger log2;
	test(log2.Connect()==KErrNone);

	log1.CreateLog(KTestLogDir1,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log1.LastError()==KErrNone);
	test(log1.LogValid()==aValid);
	
	log2.CreateLog(KTestLogDir1,KTestLogFileName2,EFileLoggingModeOverwrite);
	test(log2.LastError()==KErrNone);
	test(log2.LogValid()==aValid);
	
	log1.Write(_L("This is written to log 1"));
	test(log1.LastError()==KErrNone);
	test(log1.LogValid()==aValid);
	log2.Write(_L("This is written to log 2"));
	test(log2.LastError()==KErrNone);
	test(log2.LogValid()==aValid);

	WriteToLog(log1);
	WriteToLog(log2);

	log1.CloseLog();
	log2.CloseLog();

	TUint temp;
	if (aValid)
		{
		test(fs.Att(KTestLogFile1,temp)==KErrNone);
		test(fs.Att(KTestLogFile2,temp)==KErrNone);
		}
	else
		{
		test(fs.Att(KTestLogFile1,temp)==KErrPathNotFound);
		test(fs.Att(KTestLogFile2,temp)==KErrPathNotFound);
		}

	log1.Close();
	log2.Close();
	}

// 2.2.2 Static appends 
LOCAL_C void testStaticWrites(TBool aValid)
	{
	
	if (!aValid)
		test(DeleteFolder(KTestLogFullDir2)==KErrNone);
	else
		{
		TRAPD(ret,(CreateFolderL(KTestLogFullDir2)));
		test(ret==KErrNone);
		}

	RFileLogger::Write(KTestLogDir2,KTestLogFileName1,EFileLoggingModeAppend,_L("Writing to the log"));
	TUint num1=100;
	RFileLogger::WriteFormat(KTestLogDir2(),KTestLogFileName1(),EFileLoggingModeAppend,_L("Writing a number to the log: %d"),num1);
	TUint num2=200;
	RFileLogger::WriteFormat(KTestLogDir2(),KTestLogFileName1(),EFileLoggingModeAppend,_L("Writing two numbers to the log: %d, %d"),num1,num2);

	RFileLogger::Write(KTestLogDir2,KTestLogFileName1,EFileLoggingModeAppend,_L8("Writing to the log"));
	RFileLogger::WriteFormat(KTestLogDir2(),KTestLogFileName1(),EFileLoggingModeAppend,_L8("Writing a number to the log: %d"),num1);
	RFileLogger::WriteFormat(KTestLogDir2(),KTestLogFileName1(),EFileLoggingModeAppend,_L8("Writing two numbers to the log: %d, %d"),num1,num2);

	const TText* hdr=_S("123456");
	const TText* mgn=_S("      ");
	const TUint8* ptr=_S8("abcdefghijklmnopqrstuvwxyz");
	TInt len=26;
	RFileLogger::HexDump(KTestLogDir2(),KTestLogFileName1(),EFileLoggingModeAppend, hdr, mgn, ptr, len);
		
	TUint temp;
	if (aValid)
		test(fs.Att(KTestLogFile3,temp)==KErrNone);
	else
		test(fs.Att(KTestLogFile3,temp)==KErrPathNotFound);
	}

// 2.2.1 Writing to two open log files at a time 
// 2.2.1.4 Files are the same 
LOCAL_C void testTwoClientsOneFileL(TBool aValid)
	{

	if (!aValid)
		test(DeleteFolder(KTestLogFullDir3)==KErrNone);
	else
		CreateFolderL(KTestLogFullDir3);

	RFileLogger log1;
	test(log1.Connect()==KErrNone);
	RFileLogger log2;
	test(log2.Connect()==KErrNone);

	log1.CreateLog(KTestLogDir3,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log1.LastError()==KErrNone);
	test(log1.LogValid()==aValid);
	
	log2.CreateLog(KTestLogDir3,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log2.LastError()==KErrNone);
	test(log2.LogValid()==aValid);
	
	log1.Write(_L("This is written from log session 1"));
	test(log1.LastError()==KErrNone);
	test(log1.LogValid()==aValid);
	log2.Write(_L("This is written from log session 2"));
	test(log2.LastError()==KErrNone);
	test(log2.LogValid()==aValid);

	WriteToLog(log1);
	WriteToLog(log2);

	log1.CloseLog();
	log2.CloseLog();

	TUint temp;
	if (aValid)
		test(fs.Att(KTestLogFile4,temp)==KErrNone);
	else
		test(fs.Att(KTestLogFile4,temp)==KErrPathNotFound);

	log1.Close();
	log2.Close();
	}

// 2.2.4 Invalid operations
//
// Check that buffers larger than KLogBufferSize can be written without error.
LOCAL_C void testInvalidOps1L()
	{

	CreateFolderL(KTestLogFullDir4);

	RFileLogger log;
	test(log.Connect()==KErrNone);

	log.CreateLog(KTestLogDir4,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log.LastError()==KErrNone);
	test(log.LogValid());

	TBuf<151> longBuf;
	TChar ch('X');
	longBuf.Fill(ch,151);
	test(longBuf.Length()==151);
	log.Write(longBuf);					// only those chars which will fit will be written
	test(log.LastError()==KErrNone);

	TBuf8<151> longBuf8;
	longBuf8.Fill(ch,151);
	test(longBuf8.Length()==151);
	log.Write(longBuf8);				// only those chars which will fit will be written
	test(log.LastError()==KErrNone);

	log.Close();
	}
	
// Check that overlong directory names as well as file names cause an overflow error.
LOCAL_C void testInvalidOps2L()
	{

	CreateFolderL(KTestLogFullDir4);

	RFileLogger log;
	test(log.Connect()==KErrNone);

	TFileName dirName, fileName;
	dirName.SetLength(dirName.MaxLength());
	log.CreateLog(dirName,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log.LastError()==KErrOverflow);

	fileName.SetLength(fileName.MaxLength());
	log.CreateLog(KTestLogDir1,fileName,EFileLoggingModeOverwrite);
	test(log.LastError()==KErrOverflow);

	log.Close();
	}

// 2.2.3 Access counting 
LOCAL_C void TestAccessCountingL(TBool aValid)
	{

	if (aValid)
		CreateFolderL(KTestLogFullDir5);
	else
		DeleteFolder(KTestLogFullDir5);

	RFileLogger log;
	test(log.Connect()==KErrNone);

	log.CreateLog(KTestLogDir5,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log.LastError()==KErrNone);

	log.CloseLog();

	log.CreateLog(KTestLogDir5,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log.LastError()==KErrNone);

	log.CloseLog();

	log.CreateLog(KTestLogDir5,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log.LastError()==KErrNone);

	log.Close();

// Test closing log twice

	test(log.Connect()==KErrNone);

	log.CreateLog(KTestLogDir5,KTestLogFileName1,EFileLoggingModeOverwrite);
	test(log.LastError()==KErrNone);

	log.CloseLog();
	log.CloseLog();			// timer will be running if file exists
	User::After(6000000);
	log.CloseLog();			// timer will have expired
	log.Close();

// Test closing session twice

	test(log.Connect()==KErrNone);

	log.Close();
	log.Close();			// timer will be running
	TestWaitForShutDown();
	log.Close();			// timer will have expired
	}

LOCAL_C void doExampleL()
	{

	User::LeaveIfError(fs.Connect());

	// delete all files and folders
	DeleteFolder(KTestLogFullDir1);	// ignore return value
	DeleteFolder(KTestLogFullDir2);	// ignore return value
	DeleteFolder(KTestLogFullDir3);	// ignore return value
	DeleteFolder(KTestLogFullDir4);

	test.Printf(_L("\nDo tests with Folders not existing....\n"));

	test.Start(_L("Two Sessions logging to different files: PDS-FILELOGGER-UT-4001"));
	testTwoClientsL(EFalse);	// folders do not exist
	
	test.Next(_L("Two sessions logging to one file: PDS-FILELOGGER-UT-4002"));
	testTwoClientsOneFileL(EFalse);		// folders do not exist
	
	test.Next(_L("Static logging: PDS-FILELOGGER-UT-4003"));
	testStaticWrites(EFalse);		// folders do not exist

	test.Next(_L("Test Closing and Access Counting: PDS-FILELOGGER-UT-4004"));
	TestAccessCountingL(EFalse);

	test.Printf(_L("\nDo tests with Folders existing....\n"));

	test.Start(_L("Two Sessions logging to different files: PDS-FILELOGGER-UT-4001"));
	testTwoClientsL(ETrue);		// folders exist

	test.Next(_L("Two sessions logging to one file: PDS-FILELOGGER-UT-4002"));
	testTwoClientsOneFileL(ETrue);		// folders exist

	test.Next(_L("Static logging: PDS-FILELOGGER-UT-4003"));
	testStaticWrites(ETrue);		// folders exist

	test.Next(_L("Invalid Operations: PDS-FILELOGGER-UT-4005"));
	testInvalidOps1L();
	test.Next(_L("Invalid Operations: PDS-FILELOGGER-UT-4006"));
	testInvalidOps2L();

	test.Next(_L("Test Closing and Access Counting: PDS-FILELOGGER-UT-4004"));
	TestAccessCountingL(ETrue);

	RFileLogger log;
	test(log.Connect()==KErrNone);
	log.Close();

	TestWaitForShutDown();
	test.End();

	fs.Close();
	}

GLDEF_C TInt E32Main()
	{

	CTrapCleanup* cleanup=CTrapCleanup::New();

	__UHEAP_MARK;
	
	test.Title();

	TRAPD(ret,doExampleL());
	test(ret==KErrNone);
	
    test.End();
	test.Close();

	__UHEAP_MARKEND;

	delete cleanup;
	User::Heap().Check();
	return KErrNone;
	}
