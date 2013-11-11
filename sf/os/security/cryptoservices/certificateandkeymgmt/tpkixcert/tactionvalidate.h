/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#ifndef __TACTIONVALIDATE_H__
#define __TACTIONVALIDATE_H__

#include "t_testaction.h"
#include "tpkixcertval.h"
#include <unifiedcertstore.h>
#include "tcertutils.h"

class CActionValidate : public CTestAction
{
public:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus) = 0;
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus) = 0;
	virtual void PerformAction(TRequestStatus& aStatus);
	TBool TestResult(TInt aError);
	virtual void PerformCancel();
	virtual void AfterOOMFailure();
	virtual void Reset();

	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);
	virtual ~CActionValidate();
protected:
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void CreateChainL() = 0;
	TBool AddParametersL(const TDesC& aBuf, TInt& aPos);
	void WritePolicies(const CDesCArray& aPolicySet);
	CActionValidate(RFs& aFs, CConsoleBase& aConsole,Output& aOut);
protected:
	enum TState
		{
		ERemoveCertsBeforeTest,
		EAddRoot,
		EAddIntermediateCerts,
		EDoValidateTestStart,
		EDoValidateTestValidate,
		EDoValidateTestValidated,
		EDoValidateTestFinished,
		ERemoveCertsAfterTest,
		EEnd
		};

	TState iState;
	RFs& iFs;
	CCertUtils* iCertUtils;

	CTestChain* iTestChain;
	CTestParameters* iTestIO;

	/**
	 * The certificate chain initialized based on whether the certificates are user supplied or to be retrieved
	 * from the certificate store.
	 */
	CPKIXCertChain* iChain;

	/**
	 * The policies extracted from the test case and passed as argument to Validate
	 */
	CArrayPtrFlat<HBufC>* iPolicies;

	/**
	 * The result of the validation test. Used for the validation and the build tests.
	 */
	CPKIXValidationResult* iValidationResult;

	/**
	 * The time, it is used to check if the certifcate is within its validity
	 * period. Used for the validation and the build tests.
	 */
	TTime iTime;
	TBuf<128> iCertPath;
};

class CActionValidateWithStore : public CActionValidate
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);

private:
	CActionValidateWithStore(RFs& aFs, CConsoleBase& aConsole,Output& aOut);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void CreateChainL();
	};

class CActionValidateWithSuppliedCerts : public CActionValidate
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CActionValidateWithSuppliedCerts();
private:
	CActionValidateWithSuppliedCerts(RFs& aFs, CConsoleBase& aConsole,Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void CreateChainL();
private:
	RPointerArray<CX509Certificate>* iRootCerts;
	HBufC8* iEndEntityAndIntermediateCerts;
	};

#endif

