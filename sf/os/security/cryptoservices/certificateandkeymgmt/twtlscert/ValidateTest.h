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
* CValidateTest class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __TWTLSCERTVAL_H
#define __TWTLSCERTVAL_H

#include "TestChain.h"
#include "tcertutils.h"
#include "t_testaction.h"

class CValidateTest : public CTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	void TestValidation(TRequestStatus& aStatus);
	~CValidateTest();

public:
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus);
	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);

private:
	CValidateTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	HBufC8* ReadFileLC(const TDesC& aFilename);
	HBufC8* ReadFilesLC(CDesCArray& aServerCerts);
	void WriteError(TValidationError aError);
	static void CleanupCertArray(TAny* aCertArray);

private:
	void HandleEInitL(TRequestStatus& aStatus);
	void HandleEValidationInitStoreManager1L(TRequestStatus& aStatus);

private:
	enum TState
		{
		EInit,
		EValidationStoreInitStoreManager1,
		EValidationStoreDepopulateStore1,
		EValidationStorePopulateStoreRoots,
		EValidationStorePopulateStoreExtras,
		EValidationStoreValidate,
		EValidationStoreValidated,
		EValidationStoreInitStoreManager2,
		EValidationStoreDepopulateStore2,
		EValidationStoreEnd,
		EValidationSuppliedInit,
		EValidationSuppliedValidate,
		EValidationSuppliedValidated,
		EFinished
		};

private:
	RFs& iFs;
	TRequestStatus *iOriginalRequestStatus;
	TState iState;
	TTime iStart;
	CTestChainValidate* iTestChain;
	CCertUtils* iCertUtils;
	CWTLSValidationResult* iValidationResult;
	TTime iTime;
	CWTLSCertChain* iChain;
	};

#endif
