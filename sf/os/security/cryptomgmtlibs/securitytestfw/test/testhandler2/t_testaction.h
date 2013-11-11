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


#ifndef __T_TESTACTION_H__
#define __T_TESTACTION_H__

#include <e32base.h>
#include <f32file.h>
#include "t_testactionspec.h"

class CConsoleBase;
class CConsoleBase;
class Output;
class CTestHandler;

const TInt KMaxErrorSize = 100;

class CTestAction : public CBase 
	{
public:
	IMPORT_C virtual ~CTestAction();

public:
	enum TScriptError
		{
		ENone,
		ESyntax,
		EFileNotFound
		};
	IMPORT_C virtual void PerformPrerequisite(TRequestStatus& aStatus);
	virtual void PerformAction(TRequestStatus& aStatus) = 0;
	IMPORT_C virtual void PerformPostrequisite(TRequestStatus& aStatus);
	TBool Finished();
	IMPORT_C virtual void ReportAction();
	IMPORT_C virtual void CheckResult(TInt aError);
	IMPORT_C void virtual FailTestNow(TRequestStatus& aStatus);
	IMPORT_C virtual TScriptError ScriptError(void);
	IMPORT_C virtual void ScriptError(TDes& aError);
	IMPORT_C virtual void SetScriptError(const TScriptError &iScriptError, const TDesC& aError);

	// Specific functions for testing cancellation
public:
	IMPORT_C virtual void PerformCancel();

	// Specific functions for testing OOM
public:
	IMPORT_C virtual void Reset();
	IMPORT_C virtual void AfterOOMFailure();
	IMPORT_C virtual void ResetState();

	// Functions for accessing data shared between all test actions in a test run
public:
	void SetTestHandler(CTestHandler& aTestHandler);
protected:
	IMPORT_C CBase* SharedData() const;
	IMPORT_C void SetSharedData(CBase* aData);
	
protected:
	IMPORT_C CTestAction(CConsoleBase& aConsole, Output& aOut);
	IMPORT_C void ConstructL(const TTestActionSpec& aTestActionSpec);

private:
	IMPORT_C virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	IMPORT_C virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void DoReportAction() = 0;
	virtual void DoCheckResult(TInt aError) = 0;
public:
	enum TActionState
		{
		EPrerequisite,
		EAction,
		EPostrequisite
		};

protected:
	TBool iFinished;
	TInt iExpectedResult;

public:
	/**
	 * After the test, this will be ETrue if the test succeeded or EFalse otherwise.
	 */
	TBool iResult;
	

	/**
	 * Inidicates if it s .script file
	 */
	TBool iTefScript;
	
	/**
	 * Denote a failure of any RUNTESTSTEP within a single TESTCASE
	 * */
	TBool iRunningResult;
	
	
	/**
	*	Indicates whether the test will fail because of known deferred defects etc
	*/
	TBool iKnownFailure;
	TActionState iActionState;
	HBufC8* iNameInfo;

protected:
	CConsoleBase& iConsole;
	Output& iOut;
	TBuf<KMaxErrorSize> aScriptErrorDescription;
	TScriptError iScriptError;
	CTestHandler* iTestHandler;
	};

#endif
