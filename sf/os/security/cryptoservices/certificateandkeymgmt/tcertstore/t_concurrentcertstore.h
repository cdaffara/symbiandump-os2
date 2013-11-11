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
 @internalTechnology
*/

#ifndef __T_CONCURRENTCERTSTORE_H__
#define __T_CONCURRENTCERTSTORE_H__

#include "t_certstoreactions.h"
#include "t_testactionspec.h"
#include "t_output.h"

//	This class can be used to flag concurrent certstore testing
//	It inherits from CTestAction so it can be used in the script
//	but otherwise doesn't carry out any test.
//	When in a script and enabled, the concurrent tester singleton
//	is initialised and can then be referenced by all following tests in
//	various performance operations

class CTestConcurrentCertStore : public CCertStoreTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
							 const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
							  const TTestActionSpec& aTestActionSpec);
	~CTestConcurrentCertStore();
public:
	virtual void PerformAction(TRequestStatus& aStatus);
protected:
	CTestConcurrentCertStore(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
private:
	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
private:
	void ConstructL(const TTestActionSpec& aTestActionSpec);
};

class CConcurrentTester : public CBase
{
public:
	static void SetDoingConcurrentTesting(TBool aTestConcurrent);
	static TBool IsDoingConcurrentTesting();
public:
	static void SanitizeTestResult(Output& aOut, TBool& aResult);
private:
	CConcurrentTester();
	~CConcurrentTester();
};







#endif	//	__T_CONCURRENTCERTSTORE_H__
