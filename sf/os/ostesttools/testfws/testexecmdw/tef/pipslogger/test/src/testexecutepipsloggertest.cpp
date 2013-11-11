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
* Demonstration use of the TestExecute standalone PIPS logger client API
*
*/



/**
 @file TestExecutePIPSLoggerTest.cpp
*/

#include <test/testexecutepipslog.h>
#include <e32cons.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

_LIT(KLogFile1,"c:/pipe_no1");
_LIT(K16BitText,"This is a short line of 16 Bit Text");
_LIT8(K8BitText,"This is a short line of  8 Bit Text");
_LIT(K16BitFormatText,"This is a short line of 16 Bit Format Text int = %d string = %S");
_LIT8(K8BitFormatText,"This is a short line of  8 Bit Format Text int = %d string = %S");
_LIT(K16BitString,"The String16");
_LIT8(K8BitString,"The String8");

#define HALF_DUPLEX		"c:/pipe_no1"
#define MAX_BUF_SIZE	255

void MainL()
	{
	
	/* Create the named - pipe */
    int ret_val = mkfifo(HALF_DUPLEX, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH);
    
    if ((ret_val == -1) && (errno != EEXIST)) {
        perror("Error creating the named pipe");
        return ; 
        //exit (1);
    }
    
    int pipe = open(HALF_DUPLEX, O_RDONLY | O_NONBLOCK);
    
	_LIT(KTitle,"TestExecute Standalone Logger Test Code");
	CConsoleBase* console = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);

	_LIT(KMessage1,"Connect() To Logger and Close() immediately\r\n");
	console->Printf(KMessage1);
	RTestExecutePIPSLogServ logger;
	User::LeaveIfError(logger.Connect());
	logger.Close();

	_LIT(KMessage2,"Immediate re-Connect() To Logger and Close() immediately\r\n");
	console->Printf(KMessage2);
	User::LeaveIfError(logger.Connect());
	logger.Close();

	_LIT(KMessage3,"Immediate re-Connect() To Logger CreateLog() and Close() immediately\r\n");
	console->Printf(KMessage3);
	User::LeaveIfError(logger.Connect());
	User::LeaveIfError(logger.CreateLog(KLogFile1));
	logger.Close();

	// OS needs time to shutdown the server
	// No delay means we get server terminated from the OS
	User::After(1000000);

	_LIT(KMessage4,"Connect() To Logger CreateLog() %S Call API's\r\n");
	TBuf<80> logFile(KLogFile1);
	console->Printf(KMessage4,&logFile);
	User::LeaveIfError(logger.Connect());
	User::LeaveIfError(logger.CreateLog(KLogFile1));
	TBuf<20> buf16(K16BitString);
	TBuf8<20> buf8(K8BitString);
	logger.Write(K16BitText);
	logger.Write(K8BitText);
	logger.WriteFormat(K16BitFormatText,16,&buf16);
	logger.WriteFormat(K8BitFormatText,8,&buf8);
	logger.LogExtra(((TText8*)__FILE__), __LINE__,ESevrInfo,K16BitFormatText,1,&buf16);

	_LIT(KMessage5,"ALL API's Called - Call Close(),\r\n");
	console->Printf(KMessage5);
	//console->Getch();
	logger.Close();
	
	//try an empty pipe 
	_LIT(KLogFileEmpty,"");
	_LIT(KMessageEmp,"Connect() To Logger CreateLog() %S Call API's\r\n");
	TBuf<80> logFiley(KLogFileEmpty);
	console->Printf(KMessageEmp,&logFiley);
	User::LeaveIfError(logger.Connect());
	User::LeaveIfError(logger.CreateLog(KLogFileEmpty));
	logger.Write(K16BitText);
	logger.Write(K8BitText);
	logger.WriteFormat(K16BitFormatText,16,&buf16);
	logger.WriteFormat(K8BitFormatText,8,&buf8);
	logger.LogExtra(((TText8*)__FILE__), __LINE__,ESevrInfo,K16BitFormatText,1,&buf16);
	console->Printf(KMessage5);
	console->Getch();
	logger.Close();

	CleanupStack::PopAndDestroy(console);

	}

// Entry point for all Epoc32 executables
// See PSP Chapter 2 Getting Started
int main(int,char**,char* )
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
	_LIT(KPanic,"PIPSLoggerTest");
	__ASSERT_ALWAYS(!err, User::Panic(KPanic,err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
