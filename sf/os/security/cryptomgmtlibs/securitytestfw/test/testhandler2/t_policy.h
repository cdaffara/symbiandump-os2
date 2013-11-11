/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tpolicy.h
*
*/


#ifndef __TPOLICY_H__
#define __TPOLICY_H__

#include "t_testaction.h"

/**
 * This action is used to test whether security policies are correctly enforced.
 * This is done by specifying another test action to run, and the security
 * policy that is under test.  The action is then run in a seprate process.
 * This happens multiple times, to test that it passes when the process has the
 * required security settings and failes when it does not.
 *
 * The action body can contain the the following elements:
 *
 * testexe: The path of the executable to use to run the test
 * excludedcapabilities: Capabilites not to assign to the test executable when performing failure tests
 * policy: The security policy to be tested for
 * preactions: Optional, contains test actions to run to perform setup before the test
 * passaction: The action under test, when expected to pass
 * failaction: The action under test, when expected to fail
 * postactions: Optonal, contains test actions to run to perform cleanup after the test
 *
 * A new script is created containing the preactions if present, the test action
 * and the postactions (if present).  For failure tests, the testaction's expect
 * results is changed to KErrPermissionDenied.
 */
class CPolicyTest : public CTestAction
	{
public:
	static CPolicyTest* NewL(CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	static CPolicyTest* NewLC(CConsoleBase& aConsole, Output& aOut,
		const TTestActionSpec& aTestActionSpec);
	~CPolicyTest();

private:
	CPolicyTest(CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

public:
	virtual void PerformAction(TRequestStatus& aStatus);

private:
	// Methods overriden from CTestAction 
	virtual void DoReportAction();
	virtual void DoCheckResult(TInt aError);
	virtual void PerformCancel();
	virtual void Reset();

	// Methods for parsing the test spec
	void BadUsageL(const TDesC& aMessage);
	void SetTestExeL(const TDesC8& aPath);
	void SetPolicyL(const TDesC8& aSpec);
	void SetTestActionL(const TDesC8& aPassAction, const TDesC8& aFailAction);

	// Methods for running the test
	void GetNextTest();	
	void StartProcessL(const TDesC& aExe, const TDesC& aCommandLine, TRequestStatus& aStatus);
	void CheckProcessTermintationL();
	void SetupTestL(TRequestStatus& aStatus);
	void SetTestSecurityInfoL(TInt aSecureId, TInt aVendorId, TUint aCapSet, TRequestStatus& aStatus);
	void CheckSetCapsResultL();
	void RunTestL(TRequestStatus& aStatus);
	void ProcessResultsL(TRequestStatus& aStatus);
	void WriteScriptFileL(const TDesC& aPath, const TDesC8& aAction);
	
private:
	enum TState
		{
		EInit,
		ESetupTest,
		ERunTest,
		EProcessResults,
		EFinished
		};

	enum TTestState
		{
		ETestNone,
		ETestFailSID,
		ETestFailVID,
		ETestFailCap,
		ETestPass,
		ETestFinished
		};

private:
	RFs iFs;							///< File server handle
	TInt iProcessSecureId;				///< The secure id of this process
	TInt iProcessVendorId;				///< The vendor id of this process
	
	HBufC* iTestExe;					///< The path of the test harness to run
	TCapabilitySet iExcludedCaps;		///< Capabilties to exclude from failure tests
	TUint iSecureId;					///< The secure id of the policy under test, or zero
	TUint iVendorId;					///< The secure id of the policy under test, or zero
	RArray<TCapability> iCapabilities;	///< The capabilities of the policy under test
	HBufC8* iPreActions;				///< Script fragment for setup test actions
	HBufC8* iPassAction;				///< The action under test
	HBufC8* iFailAction;				///< The action under test
	HBufC8* iPostActions;				///< Script fragment for cleanup test actions
	
	TState iState;						///< The state we're in
	TTestState iTestState;				///< The test we're running when iState is ESetCaps or ERunTest
	TInt iCapIndex;						///< The capability we're testing when iTestState is ECapFailTests
	RProcess iProcess;					///< Used for running setcap and test harness
	TInt iFailCount;					///< Running count of tests failed
	TBuf16<50> iTestExeTmpNewPath;      ///< stores the executable as executable_policytest.exe
	};

#endif
