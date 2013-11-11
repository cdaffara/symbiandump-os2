/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __CAPTESTFRAMEWORK_STEP_H__
#define __CAPTESTFRAMEWORK_STEP_H__

#include <test/testexecutestepbase.h>
#include "captestframeworkserver.h"

#include "captestframework.h"


struct TTestEnvironment
	{
	TTestEnvironment(const TCapabilitySet& aCaps, TUid aSid, TUid aVid, TBool aExpectPass);
	
	TCapabilitySet iCaps;
	TUid iSid;
	TUid iVid;
	TBool iExpectPass;
	};


class CCapTestFrameworkStep : public CTestStep
	{
public:
	enum TThoroughness 
		{
		EBasicChecks,			// Just test using no capabilities, and capabilities required
		EThoroughChecks,		// Test every subset required
		};
	
public:
	CCapTestFrameworkStep(TThoroughness aThoroughness = EBasicChecks);
	~CCapTestFrameworkStep();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TVerdict doTestStepL();

private:
	void RunTestDllL(const TDesC& aDllName);
	MCapabilityTestFactory* SetupFactoryL();
	
	void PrintCapabilitySet(const TCapabilitySet& aCapSet, const TDesC& aExtra=KNullDesC);

	void RunTestStepL(MCapabilityTest* aTest);

	void GenerateEnvironmentsL(const TCapabilitySet& aCapsNeeded, const TUid& aSidNeeded, const TUid& aVidNeeded, RArray<TTestEnvironment>& aEnvironments);

	TCapabilitySet InvertCapSet(const TCapabilitySet& aCapSet);

	
	// Sets the helper up with capabilities
	void SetupHelperL(const TTestEnvironment& aEnvironment);
	
	// Runs helper
	void RunHelperL(TInt aTestNumber, TBool aShouldPass);

private:
	RFs iFs;
	TThoroughness iThoroughness;
	TInt iCurrentTest;
	MCapabilityTestFactory* iFactory;
	RLibrary iLibrary;
	TPtrC iDllName;
	TBool iOmitTCBCapInComplementSet; // This functionality is needed for testing components that don't have the TCB capability, for example the SIF API.
	};

_LIT(KRunBasicCapabilityChecks,"RunBasicCapabilityChecks");
_LIT(KRunThoroughCapabilityChecks,"RunThoroughCapabilityChecks");

#endif /* #ifndef __CAPTESTFRAMEWORK_STEP_H__ */
