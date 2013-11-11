// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CSIPURIANDAUTHORITYTEST_H__
#define __CSIPURIANDAUTHORITYTEST_H__

// System includes
//
#include <e32base.h>

// Local includes
#include "ctestbase.h"

// CSipUriAndAuthorityTest - test class for the Uri and Authority family of classes
//
class CSipUriAndAuthorityTest : public CTestBase
	{
public:

	// Static factory c'tor. Leaves pointer to created object on the cleanup stack.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CSipUriAndAuthorityTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CSipUriAndAuthorityTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CSipUriAndAuthorityTest();

	// Runs the defined tests.
	//
	void DoTestsL();

	// Default c'tor.
	//
	CSipUriAndAuthorityTest(CIpuTestHarness* aTestHarness);

private:	// Methods

	// Non-trivial c'tor. Second part of 2-phase construction - does all allocation.
	//
	void ConstructL();
private:	// Attributes

	// Open session and file
	TInt OpenSipURIFile();

	// In:
	// aError			- the error code
	// aSipUri			- the SIP URI that is currently being validated
	//
	void SipLogError(TInt aError, const TDesC16& aSipUri) const;

	// In:
	//aSipUri			- the sip uri that we are currently using
	// aError			- the error code
	//
	void LogErrorMessage(const TDesC16& aSipUri, const TInt aError)const;

	//Validate SIP URIs
	//
	TInt SipUriValidationL();

	//Compare 2 SIP URIs
	//
	TInt SipUriEquivalenceL();
	
	// Test harness
	CIpuTestHarness*	iTestHarness;
	
	TFileText iTUriParserFile;
	RFs iFsSession;
	TBool iFoundEquivalence;
	};

#endif	// __CSIPURIANDAUTHORITYTEST_H__
