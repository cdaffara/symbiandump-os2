// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "GmxmlTestUtils.h"


_LIT(KMsvTestFileOutputBase,"MsgLogs");

// this is used if we can't find MsgLogs anywhere
_LIT(KMsvTestFileDefaultOutputBase,"\\MsgLogs\\");
_LIT(KMsvPathSep,"\\");
#define KMaxLogLineLength 256



//
//
// CTestTimer
//
//

 CTestTimer::CTestTimer()
: CTimer(EPriorityLow)
	{}

 void CTestTimer::RunL()
	{
	if (!iReport)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		User::RequestComplete(iReport, iStatus.Int());
		iReport = NULL;
		}
	}

 CTestTimer* CTestTimer::NewL()
	{
	CTestTimer* self = new(ELeave) CTestTimer();
	CleanupStack::PushL(self);
	self->ConstructL(); // CTimer
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

 void CTestTimer::AfterReq(TTimeIntervalMicroSeconds32 aSec, TRequestStatus& aStatus)
	{
	iReport = &aStatus;
	aStatus = KRequestPending;
	After(aSec);
	}


//
//
// CTestUtils
//
//

 CTestUtils::CTestUtils(RTest& aRTest)
: iRTest(aRTest), iLogToConsole(ETrue), iLogToFile(ETrue)
	{
	TBuf<256> command;
		User::CommandLine(command);

	command.Trim();
	SetRunAuto(command.FindF(KTestShowMenu) == KErrNotFound);

	if (!RunAuto())
		iRTest.Printf(_L("Not Run Auto. Command line: %S\n"), &command);
	}

 CTestUtils::~CTestUtils()
	{
	CloseMainLog();
	iFs.Close();
	}

 void CTestUtils::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	OpenMainLogL();
	WriteToLog(KTestHeader);
	}

 void CTestUtils::Printf(TRefByValue<const TDesC> aFmt,...)
	{
	// Print to the log file.
	TTestOverflow overflow(*this);
	VA_LIST list;
	VA_START(list, aFmt);
	TBuf<0x100> aBuf;
	aBuf.AppendFormatList(aFmt, list, &overflow);

	if (iLogToConsole)
		{
		iRTest.Printf(aBuf);
		}

	if (iLogToFile)
		{
		WriteComment(aBuf);
		}
	}


 void CTestUtils::CreateAllTestDirectories()
	{
	TChar driveChar=RFs::GetSystemDriveChar();
 	TBuf<2> systemDrive;
 	systemDrive.Append(driveChar);
 	systemDrive.Append(KDriveDelimiter);
	
	iFs.SetSessionPath(systemDrive);

	if (!iFs.MkDir(KLogsDir))
		{
		Printf(_L("Created c:\\logs\\ directory\n"));
		}
	}

 TInt CTestUtils::ResolveLogFile(const TDesC& aFileName, TParse& aParseOut)
	{
	TFileName* savedPath = new TFileName;
	TFileName* fileName = new TFileName;
	if ((savedPath == NULL) || (fileName == NULL))
		return KErrNoMemory;

	fileName->Append(KMsvPathSep);
	fileName->Append(KMsvTestFileOutputBase);
	fileName->Append(KMsvPathSep);
	
	// file finder will look in the session drive first, then Y->A,Z
	// so set session drive to Y (save old and restore it afterwards)
	iFs.SessionPath(*savedPath);
	_LIT(KTopDrive,"Y:\\");
	iFs.SetSessionPath(KTopDrive);
    TFindFile file_finder(iFs);
    TInt err = file_finder.FindByDir(*fileName,KNullDesC);

	if(err==KErrNone)
		{
		fileName->Copy(file_finder.File());
		AppendTestName(*fileName);
		fileName->Append(KMsvPathSep);
		fileName->Append(aFileName);
		iFs.MkDirAll(*fileName);
		aParseOut.Set(*fileName,NULL,NULL);
		}
	iFs.SetSessionPath(*savedPath);
	delete savedPath;
	delete fileName;
	return(err);
	}

 TBool CTestUtils::RunAuto() const
	{
	return iRunAuto;
}
	
 void CTestUtils::SetRunAuto(TBool aRunAuto)
	{
	iRunAuto = aRunAuto;
	}


// appends .<PLATFORM>.<VARIANT>.log to aFileName;
void CTestUtils::AppendVariantName(TDes& aFileName)
	{
#if (defined(__THUMB__) || defined(__MARM_THUMB__))
	aFileName.Append(_L(".THUMB."));
#endif
#if (defined(__ARMI__) || defined(__MARM_ARMI__))
	aFileName.Append(_L(".ARMI."));
#endif
#if (defined(__ARM4__) || defined(__MISA__) || defined(__MARM_ARM4__))
	aFileName.Append(_L(".ARM4."));
#endif
#if defined(__WINSCW__)
	aFileName.Append(_L(".WINSCW."));
#else 
#if defined(__WINS__)
	aFileName.Append(_L(".WINS."));
#endif
#endif

#if defined(_DEBUG)
	aFileName.Append(_L("DEB."));
#else
	aFileName.Append(_L("REL."));
#endif
	aFileName.Append(_L("LOG"));
	}

// appends the test harness name to aFileName
void CTestUtils::AppendTestName(TDes &aFileName)
	{
	TInt start = RProcess().FileName().LocateReverse('\\');
	TInt end = RProcess().FileName().LocateReverse('.');

	if (end == KErrNotFound)
		end = RProcess().FileName().Length();

	aFileName.Append(RProcess().FileName().Mid(start + 1, end - start - 1));
	}	



TInt CTestUtils::OpenMainLogL()
	{
	_LIT(KDisplayLogFile,"Log File %S\n");
	TParse loglocation;
	TFileName logfile;
	TInt err=ResolveLogFile(KNullDesC, loglocation);
	if(err!=KErrNone) 
		{
		TChar driveChar=RFs::GetSystemDriveChar();
 		TBuf<2> systemDrive;
 		systemDrive.Append(driveChar);
 		systemDrive.Append(KDriveDelimiter);
 		TPath pathName(systemDrive) ;
		pathName.Append(KMsvTestFileDefaultOutputBase);		
		iFs.MkDirAll(pathName);
		err=ResolveLogFile(KNullDesC, loglocation);
		}
	User::LeaveIfError(err);
	logfile.Copy(loglocation.FullName());
	logfile.Delete(logfile.Length()-1,1);
	AppendVariantName(logfile);
	iRTest.Printf(KDisplayLogFile, &logfile);
	iFs.MkDirAll(logfile);

	iLogBuf=HBufC::NewL(KMaxLogLineLength);
	iLogBuf8=HBufC8::NewL(KMaxLogLineLength);
	return(iFile.Replace(iFs,logfile,EFileWrite|EFileShareAny));
	}

void CTestUtils::CloseMainLog()
	{
	delete iLogBuf;
	iLogBuf=0;
	delete iLogBuf8;
	iLogBuf8=0;

	iFile.Close();
	}


 void CTestUtils::TestStart(TInt aTest, const TDesC& aTestDescription)
	{
	WriteToLog(KTestStartingWithDesc, aTest, &aTestDescription);
	}


 void CTestUtils::TestFinish(TInt aTest, TInt aError)
	{
	aError ?
		WriteToLog(KTestFailed, aTest, aError)
		: WriteToLog(KTestPassed, aTest);
	}


 void CTestUtils::TestHarnessCompleted()
	{
	WriteToLog(KTestHarnessCompleted);
	}

 void CTestUtils::TestHarnessFailed(TInt aError)
	{	
	WriteToLog(KTestHarnessFailed, aError);
	}

 void CTestUtils::WriteComment(const TDesC& aComment)
	{
	/* Prepends KTestCommentPrepend to aComment, then writes it to aFlogger */

	TBuf<0x100> buf;
	buf.Copy(aComment);
	buf.Trim();
	buf.Insert(0,KTestCommentPrepend);
	WriteToLog(buf);
	}

void CTestUtils::WriteToLog(TRefByValue<const TDesC> aFmt,...)
	{

	_LIT(KDateFormatString, "%D%M%*Y%1%/1%2%/2%3 %H%:1%T%:2%S ");
	_LIT(Kcr,"\r\n");
	iLogBuf->Des().Zero();
	TTime date;
	date.HomeTime();
	TBuf<18> dateString;
	
	TRAPD(error,date.FormatL(dateString,(KDateFormatString)));
	if(error)
	{
		dateString.Copy(_L("Invalid Date"));
	}
	iLogBuf->Des().Copy(dateString);
	
	VA_LIST list;
	VA_START(list,aFmt);

	iLogBuf->Des().AppendFormatList(aFmt,list);
	iLogBuf->Des().Append(Kcr);
	iLogBuf8->Des().Copy(*iLogBuf);
	iFile.Write(*iLogBuf8);
	}
