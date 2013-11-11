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


#ifndef TPOLICYREADER_H
#define TPOLICYREADER_H

#include <e32base.h>
#include <e32cmn.h>
#include <f32file.h>
#include <test/tefunit.h>
#include <ups/policy.h>

#include "policyreader.h"

class CTestPolicyReader : public CTestFixture
	{
public:
	// From CTestFixture
	void SetupL();
	void TearDownL();
   
	// Policy Reader Unit Test
	void TestPolicyReaderL();
	
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	TBool CheckPolicyL(const TDesC& aPolicySection);	
	void ReadExpectedValuesL(const TDesC& aPolicySection);
	TBool CompareExpectedValues(const UserPromptService::CPolicy& aPolicy);
	void CreatePolicyReaderL(const TDesC& aPolicyFile);
	void TestPolicyListL(const TDesC& aPolicyFile);

	TInt iExpectedError;
	TInt iSystemServerSecurity;
	UserPromptService::CPolicyReader* iPolicyReader;
	TInt iSidClasses;
	RArray<TSecureId> iSidList;	
	TPtrC iDestination;
	TInt iOptions;
	TUid iPolicyEvaluator;
	TUid iDialogCreator;
	TInt iMajorVersion;
	TInt iMinorVersion;
	TBool iDefault;
	TInt iFlags;
	RFs iFs;
	};

#endif // TPOLICYREADER_H

