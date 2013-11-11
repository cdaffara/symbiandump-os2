// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <s32stor.h>
#include <e32cons.h>
#include <e32std.h>

_LIT(KConsoleName,   "Ecom Server Ini File Writer");
_LIT(KFailedMessage, "failed: leave code=%d \n[press any key]");
_LIT(KOkMessage,     "ok \n[press any key]");

// ini file
_LIT(KIniFileName, "_:\\EComSrvr.ini");
static TBuf<15> IniFileName(KIniFileName);

static TInt32 KEComUid     = 0x10009D8F;
static TInt32 KSsaUid      = 0x00000001;
static TInt8  KSsaEnabled  = 0x01;
static TInt8  KSsaDisabled = 0x00;
	
/**
 @fn void WriteIniFileL()
 Open the ini file and store the values
 */
LOCAL_C void WriteIniFileL()
	{
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	
	// create folder path
	fileSession.MkDirAll(IniFileName);
	
	RFileWriteStream fileWriteStream;
	User::LeaveIfError(fileWriteStream.Create(fileSession, IniFileName ,EFileWrite));
	CleanupClosePushL(fileWriteStream);
	
	//write the KEComUid to the stream
	fileWriteStream.WriteInt32L(KEComUid);
	
	//write the KSsaUid to the stream
	fileWriteStream.WriteInt32L(KSsaUid);
	
	//write the SSA value to the stream
	fileWriteStream.WriteInt8L(KSsaDisabled);
	
	//commit changes to the stream
	fileWriteStream.CommitL();

	// close: fileSession, fileWriteStream
	CleanupStack::PopAndDestroy(2);
	}

/**
 @fn void ReadIniFileL()
 Open the ini file and read the contents. Intended for manually testing the 
 write code.
 */
LOCAL_C void ReadIniFileL()
	{
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	
	RFileReadStream fileReadStream;
	User::LeaveIfError(fileReadStream.Open(fileSession, IniFileName, EFileRead));
	CleanupClosePushL(fileReadStream);

    TInt32 value1 = 0;
    TInt8 value2 = 0;

	//read the KEComUid and KSsaUid
	value1 = fileReadStream.ReadInt32L();
	value1 = fileReadStream.ReadInt32L();
	value2 = fileReadStream.ReadInt8L();

	// close: fileSession, fileReadStream
	CleanupStack::PopAndDestroy(2);
	}

/**
 @fn void StartConsoleL()
 Start the console and create the ini file, then call the read method
 to manually test the contents.
 */
LOCAL_C void StartConsoleL()
    {
    // set INI file drive
    IniFileName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
    
	// Create the console and put it on the cleanup stack
	CConsoleBase* console = 
		Console::NewL(KConsoleName, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);

	// Call the main function and trap the result
	TRAPD(error, WriteIniFileL()); // create the ini file
	if(!error)
		{
		TRAPD(error2, ReadIniFileL()); // perform test
		error = error2;
		}
	if (error)
		console->Printf(KFailedMessage, error);
	else
		console->Printf(KOkMessage);

	CleanupStack::PopAndDestroy(console);	// close console
    }

/**
 @fn TInt E32Main()
 Main entry point to the console app called by E32 
*/
GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	
	CTrapCleanup* cleanupStack = CTrapCleanup::New();// create clean-up stack
	TRAP_IGNORE(StartConsoleL());	// start program
	delete cleanupStack;			// destroy clean-up stack
	
	__UHEAP_MARKEND;
	
	return KErrNone;				// and return No Error
    }

