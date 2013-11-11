/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/




/**
 @file
*/

#ifndef __PERFORMANCETEST_H__
#define __PERFORMANCETEST_H__
#include <f32file.h>
#include "t_testaction.h"
#include "t_testsetup.h"
#include <e32std.h>

const TInt KMaxIterations = 500;

_LIT8(KIterationsStart, "<iterations>");
_LIT8(KIterationsEnd, "</iterations>");
_LIT8(KTypeOfCryptoStart, "<cryptotype>");
_LIT8(KTypeOfCryptoEnd, "</cryptotype>");
_LIT8(KRSAStandard, "EStandard");
_LIT8(KRSAStandardCRT, "EStandardCRT");

//	This class can be used to switch on/off performance testing
//	It inherits from CTestAction so it can be used in the script
//	but otherwise doesn't carry out any test.
//	When in a script and enabled, the performance tester singleton
//	is initialised and can then be used by all following tests in
//	various performance operations

class CPerformanceTest : public CTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
public:
	virtual ~CPerformanceTest();
	virtual void PerformAction(TRequestStatus& aStatus);
protected:
    CPerformanceTest(CConsoleBase& aConsole, Output& aOut);  
	virtual void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void DoReportAction(void){};
	virtual void DoCheckResult(TInt){};
};

//	static singleton tracks performance testing and provides
//	standard operations for all performance tests

class CPerformance : public CBase
{
public:
	static CPerformance* NewL();
public:
	static CPerformance* PerformanceTester();
	static void ClosePerformanceTester();
public:
	inline void SetTestingPerformance(TBool aTestPerformance) {iTestingPerformance=aTestPerformance;};
	inline TBool IsTestingPerformance() const {return (iTestingPerformance);};
private:
	CPerformance();
	~CPerformance();
private:
	static CPerformance* iPerformanceTester;
private:
	TBool iTestingPerformance;
};

#endif	//	__PERFORMANCETEST_H__
