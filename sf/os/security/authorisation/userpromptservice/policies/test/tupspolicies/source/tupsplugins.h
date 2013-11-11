/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TUPSPLUGINS_H
#define TUPSPLUGINS_H

#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>
#include <test/tefunit.h>
#include <ups/promptrequest.h>
#include <ups/upsdb.h>
#include <ups/dialogcreator.h>
#include <ups/fingerprint.h>
#include <ups/policyevaluator.h>

#include "activewaiter.h"
#include "pluginmanager.h"
#include "policycache.h"
#include "util.h"

class CTestPlugins : public CActiveTestFixture, public MActiveWaiterObserver  
	{
public:
	// From CTestFixture
	void SetupL();
	void TearDownL();
	
	// Implement MActiveWaiterObserver
	void DoCancel();	

	// Policy Evaluator Unit Test
	void TestCacheL();
	void TestPluginManagerL();
	void TestLengthsL();
	void TestOOML();
	
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	class TExpectedValue
		{
	public:
		TInt iFlags;
		TInt iMajorVersion;
		TInt iMinorVersion;
		TInt iPolicyEvaluator;
		TBool iSilent;
		TBuf8<32> iClientEntity;
		};	

	TBool DoRequestL(const TExpectedValue& aExpected, const UserPromptService::CPromptRequest& aRequest);	
	TBool DoOomRequestL(const TExpectedValue& aExpected, const UserPromptService::CPromptRequest& aRequest);
	
	UserPromptService::CPromptRequest* GetRequestParamsLC(const TDesC& aSection, const TDesC8& aClientEntity);
	void GetResponseL(const TDesC& aSection);
	TBool CheckPolicyL(const TExpectedValue& aExpected, const UserPromptService::CPolicy* aPolicy);
	void GetExpectedValuesL(const TDesC& aSection);
	void LoadTestDataL(TInt aNumRequests);

	UserPromptService::CPolicy::TOptions iResponse;
	UserPromptService::CDecisionRecord* iDecisionRecord;	
	
	CActiveWaiter* iActiveWaiter;
	TInt iExpectedError;
	RFs iFs;
	RPointerArray<UserPromptService::CPromptRequest> iRequests;
	RPointerArray<TExpectedValue> iExpectedValues;
	TBool iOom;				///< Whether an OOM test is running
	TBool iInteractive;		///< Whether to display the techview dialog
	};

#endif // TUPSPLUGINS_H

