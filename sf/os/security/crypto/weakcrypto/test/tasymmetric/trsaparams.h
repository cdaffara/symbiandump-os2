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

#ifndef __RSAPARAMSTEST_H__
#define __RSAPARAMSTEST_H__

#include "t_testaction.h"

class CRSATestPublicKey : public CTestAction
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSATestPublicKey();
protected:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
//	virtual void DoPerformActionL();
//	virtual void DoPerformanceTestActionL();
	virtual void DoReportAction(void);
	virtual void DoCheckResult(TInt);
	virtual void PerformAction(TRequestStatus& aStatus);
private:
	CRSATestPublicKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	RFs& iFs;
	HBufC8 *iBody;
	TInt iExpectedReturnValue;
    };

class CRSATestPrivateKey : public CTestAction
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSATestPrivateKey();
protected:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
//	virtual void DoPerformActionL();
//	virtual void DoPerformanceTestActionL();
	virtual void DoReportAction(void);
	virtual void DoCheckResult(TInt);
	virtual void PerformAction(TRequestStatus& aStatus);
private:
	CRSATestPrivateKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	RFs& iFs;
	HBufC8 *iBody;
	TInt iExpectedReturnValue;
    };

class CRSATestPrivateKeyCRT : public CTestAction
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CRSATestPrivateKeyCRT();
protected:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
//	virtual void DoPerformActionL();
//	virtual void DoPerformanceTestActionL();
	virtual void DoReportAction(void);
	virtual void DoCheckResult(TInt);
	virtual void PerformAction(TRequestStatus& aStatus);
private:
	CRSATestPrivateKeyCRT(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	RFs& iFs;
	HBufC8 *iBody;
	TInt iExpectedReturnValue;
    };
#endif
