/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __TACTION_BUILD_H__
#define __TACTION_BUILD_H__

#include "t_testaction.h"
#include <unifiedcertstore.h>
#include "tcertutils.h"

class CAddCall : public CBase
	{
public:
	static CAddCall* NewL(const TDesC& aBuf);
	static CAddCall* NewLC(const TDesC& aBuf);
	~CAddCall();
	CAddCall();
	virtual void ConstructL(const TDesC& aBuf);
	TBool AddCert(const TDesC& aBuf, const TDesC& aStart, const TDesC& aEnd, TInt& aPos, CDesCArray& aCerts);
	//
	CDesCArray* iCertsToAdd;
	};

class CTestActionBuild : public CTestAction
{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CTestActionBuild();

private:
	CTestActionBuild(RFs& aFs, CConsoleBase& aConsole,Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

public:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);
	TBool TestResult(TInt aError);
	virtual void PerformCancel();
	virtual void Reset();

	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);

private:
	TBool AddCert(const TDesC& aBuf, const TDesC& aStart,
		const TDesC& aEnd, TInt& aPos, CDesCArray& aCertsFileName, 
		CDesCArray& aCertsLabel);
	TBool AddAddCallsL(const TDesC& aBuf, TInt& aPos);
	TBool CompareChainsL(const CPKIXCertChain& aChain);
	TBool CompareCertL(const CX509Certificate& aCert, const TDesC& aFilename);

private:
	enum TState
		{
		EInitCertStoreManager1,
		ERemoveCertsBeforeTest,
		EAddRoot,
		EAddIntermediateCerts,
		EDoBuildTestStart,
		EDoBuildTestFinished,
		EInitCertStoreManager2,
		ERemoveCertsAfterTest,
		EEnd
		};

private:
	TState iState;
	RFs& iFs;
	CUnifiedCertStore* iUnifiedCertStore;
	CCertUtils* iCertUtils;

	TBuf<128> iRootCertFileName;
	TBuf<128> iRootCertLabel;
	CDesCArray* iExtraCertsFileName;
	CDesCArray* iExtraCertsLabel;
	CDesCArray* iInitCertsFileName;
	CDesCArray* iInitCertsLabel;

	CPKIXCertChain* iChain;
	RPointerArray<CAddCall> iAddCalls;

	TTime iTime;

	CPKIXValidationResult* iValidationResult;
	TBuf<128> iCertPath;
};

#endif
