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

#ifndef GMXML_TEST_UTILS_H_
#define GMXML_TEST_UTILS_H_

#include <f32file.h>
#include <e32test.h>
#include <simtsy.h>


//	Constants

//Test Harness Logging

#define KLogsDir				_L("c:\\logs\\")
#define KTestHeader				_L("Test Results")
#define KTestStartingWithDesc	_L("Test %d Starting (%S)")
#define KTestPassed				_L("Test %d OK")
#define KTestFailed				_L("Test %d FAILED (error = %d)")
#define KTestHarnessCompleted	_L("Tests Completed")
#define KTestHarnessFailed		_L("Tests FAILED (error = %d)")
#define KTestCommentPrepend		_L("    ")

#define KTestShowMenu			_L("/manual")


//
//
// CTestTimer
//
//

class CTestTimer : public CTimer
	{
public:
	static CTestTimer* NewL();
	void AfterReq(TTimeIntervalMicroSeconds32 aSec, TRequestStatus& aStatus);
protected:
	CTestTimer();
	void RunL();

private:
	TRequestStatus* iReport;
	};

//
//
// CTestUtils
//
//

class CTestUtils : public CBase
	{
public:
//File Logging

	void TestStart(TInt aTest, const TDesC& aTestDescription);
	void TestFinish(TInt aTest, TInt aError = KErrNone);
	void TestHarnessCompleted();
	void TestHarnessFailed(TInt aError);
	void WriteComment(const TDesC& aComment);


//Test Utils
	
	 virtual void CreateAllTestDirectories();
    
	// File location
	
	
	TInt ResolveLogFile(const TDesC& aFileName, TParse& aParseOut);

	void Printf(TRefByValue<const TDesC> aFmt,...); //prints to RTest and LogFile, depending on iLogToConsole and iLogToFile


	inline RTest& Test() const;

	 TBool RunAuto() const;
	 void SetRunAuto(TBool aRunAuto = ETrue);

public:

protected:
	virtual void ConstructL();
	CTestUtils(RTest& aRTest);
	~CTestUtils();


	virtual void Panic(TInt aPanic) = 0;

	void AppendTestName(TDes &aFileName);
	void AppendVariantName(TDes& aFileName);

	TInt OpenMainLogL();
	void CloseMainLog();
	void WriteToLog(TRefByValue<const TDesC> aFmt,...);



protected:

	RFs						iFs;
	RFile					iFile;
	RTest&					iRTest;
	TBool					iLogToConsole;
	TBool					iLogToFile;

	TBool					iRunAuto;
	HBufC*					iLogBuf;
	HBufC8*					iLogBuf8;

	};

class TTestOverflow : public TDes16Overflow
	{
public:
	TTestOverflow(CTestUtils& aTest)
		: iTest(aTest) {};

	// TDes16Overflow pure virtual
	virtual void Overflow(TDes16& /*aDes*/) {iTest.Printf(_L("ERROR: Printf Overflow\n")); iTest.Test()(EFalse);};

private:
	CTestUtils& iTest;
	};



#include "GmxmlTestUtils.inl"

#endif
