// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test of the ESTW32 facilities for accessing Win32 stdin/stdout/stderr
// 
//

#include <e32std.h>
#include <e32base.h>
#include <e32svr.h>
#include <estw32.h>

void failed(int line, TInt aExpected, TInt aResult)
	{
	TBuf<80> buf;
	buf.Format(_L("Failed line %d: expected %d, got %d"), line, aExpected, aResult);
	for(;;)
		{
		User::InfoPrint(buf);
		User::After(5*1000000);	// 5 seconds
		}
	}

#define test(err,expected)			if (err!=expected) failed(__LINE__,expected,err)
#define test_status(status,expected)	if (status.Int()!=expected) failed(__LINE__,expected,status.Int())

/**
@SYMTestCaseID          SYSLIB-STDLIB-CT-1042
@SYMTestCaseDesc	    Tests for the ESTW32 facilities for accessing Win32 stdin/stdout/stderr
@SYMTestPriority 	    High
@SYMTestActions  	    Open RWin32Stream::stdin,stdout,stderr and test writing to these streams.
                        Check for KErrNone flag
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/		
void DoTest()
	{
	RWin32Stream::StartServer();

	RWin32Stream stdin;
	RWin32Stream stdout;
	RWin32Stream stderr;

	TRequestStatus status;
	TInt err;
	err=stdin.Open(Kstdin);
	test(err,KErrNone);
	err=stdout.Open(Kstdout);
	test(err,KErrNone);
	err=stderr.Open(Kstderr);
	test(err,KErrNone);

	TBuf8<80> outbuf;

	// stderr

	outbuf=_L8("Writing to stderr\n");
	stderr.Write(status,outbuf);
	User::WaitForRequest(status);
	test_status(status,KErrNone);

	outbuf=_L8("1234XXX89");
	stderr.Write(status,outbuf,4);
	User::WaitForRequest(status);
	test_status(status,KErrNone);

	// stdout

	outbuf=_L8("Writing to stdout\n");
	stdout.Write(status,outbuf);
	User::WaitForRequest(status);
	test_status(status,KErrNone);

	outbuf=_L8("1234XXX89");
	stdout.Write(status,outbuf,4);
	User::WaitForRequest(status);
	test_status(status,KErrNone);

	FOREVER
		{
		stdin.Read(status,outbuf);
		User::WaitForRequest(status);

		TRequestStatus outStatus;
		TBuf8<80> commentary;
		commentary.Format(_L8("\nread %d, status %d\n"), outbuf.Length(), status.Int());
		stderr.Write(outStatus,commentary);
		User::WaitForRequest(outStatus);
		test_status(outStatus,KErrNone);

		if (status.Int()==KErrEof)
			break;

		stdout.Write(outStatus,outbuf);
		User::WaitForRequest(outStatus);
		test_status(outStatus,KErrNone);
		}

	outbuf=_L8("Stdin closed\n");
	stderr.Write(status,outbuf);
	User::WaitForRequest(status);
	test_status(status,KErrNone);

	}

IMPORT_C void RegisterWsExe(const TDesC &aName);

GLDEF_C TInt E32Main()
	{     
	CTrapCleanup* TheTrapCleanup=CTrapCleanup::New();

#ifdef USE_FULL_GRAPHICAL_ENVIRONMENT
	// Cause the Eikon environment to come into existence
	RSemaphore sem;
	sem.CreateGlobal(_L("WsExeSem"),0);
	RegisterWsExe(sem.FullName());

	DoTest();
	User::InfoPrint(_L("Test passed"));

	sem.Wait();	// continue running Eikon until that exits as well
#else
	DoTest();
	User::InfoPrint(_L("Test passed"));
#endif

	return(KErrNone);
	}
