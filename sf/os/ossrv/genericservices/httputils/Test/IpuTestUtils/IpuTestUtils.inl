// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

inline RTest& CIpuTestHarness::Test()
//
//	Return iTest
	{
	return iTest;
	}

inline void CIpuTestHarness::DoResourceLeakTest(TBool aValue)
//
// Turn on/off the resource handle leak test
	{
	iDoResourceLeakTest = aValue;
	}

inline void CIpuTestHarness::SetTestMode(TTestMode aTestMode)
	{
	iTestMode = aTestMode;
	}

inline void CIpuTestHarness::DefaultLogFileName(TDes& aFileName)
//
//	Sets aFilename to "KLogsDir\IpuLogsDirName\<TEST HARNESS NAME>.<PLATFORM>.<VARIANT>.log"
	{
	aFileName.Copy(RProcess().FileName());
	TInt start = aFileName.LocateReverse('\\');
	TInt end = aFileName.LocateReverse('.');
	aFileName = aFileName.Mid(start + 1, end - start - 1);

	// create the log filename
	aFileName.Insert(0, _L("\\"));
	aFileName.Insert(0, KInuLogsDirName);
	aFileName.Insert(0, KLogsDir);

#if defined(__WINS__)
	aFileName.Append(_L(".WINS."));
#else
	aFileName.Append(_L(".MARM."));
#endif

#if defined(_DEBUG)
	aFileName.Append(_L("DEB."));
#else
	aFileName.Append(_L("REL."));
#endif

	aFileName.Append(_L("LOG"));
	}

inline void CIpuTestHarness::WriteComment(const TDesC& aComment)
//
//	Writes aComment to test log file, logging file and test harness
	{
	if(aComment.Length() < (KMaxFileName - 2))
		{
		iTest.Printf(_L("%S\n"), &aComment);
		iFlogger.Write(aComment);
		}
	}

inline void CIpuTestHarness::CreateFlogger(const TDesC& aFileName, TInt aShowDate, TInt aShowTime)
//
//	Create log file in directory KLogsdir\KInuLogsDirName - Note: ingore Drive and Path of aFilename
	{
	iFlogger.Connect();
	TParse p;
	p.Set(aFileName, NULL, NULL);
	iFlogger.CreateLog(KInuLogsDirName, p.NameAndExt(), EFileLoggingModeOverwrite);
	iFlogger.SetDateAndTime(aShowDate, aShowTime);
	iFlogger.Write(KTestHeader);
	}

inline TPtr CIpuTestHarness::CTestInfo::Name() const
//
//	Returns name
	{
	return iName->Des();
	}

inline TInt CIpuTestHarness::CTestInfo::Number() const
//
//	Returns number
	{
	return iNumber;
	}

inline TInt CIpuTestHarness::CTestInfo::ErrorCode() const
//
//	Returns errorcode
	{
	return iErrorCode;
	}
