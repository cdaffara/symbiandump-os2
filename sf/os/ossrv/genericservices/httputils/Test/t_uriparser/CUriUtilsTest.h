// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CURIUTILSTEST_H__
#define __CURIUTILSTEST_H__

// System includes
//
#include <e32base.h>
#include <uriutils.h>
// Local includes
//
#include "IpuTestUtils.h"
#include "ctestbase.h"

class CUriUtilsTest : public CTestBase
	{
public:

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CUriUtilsTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CUriUtilsTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CUriUtilsTest();

	// Does the tests
	//
	void DoTestsL();

private:

	// C'tor
	//
	CUriUtilsTest(CIpuTestHarness* aTestHarness);

	// 2nd phase construction
	//
	void ConstructL();

	// Tests ConvertToInternetForm
	//
	// In:
	// aData		- the display uri
	// aExpected	- the internet form
	//
	void TestUriUtilsConvertToInternetFormL(const TDesC& aData, const TDesC& aExpected);
	void TestUriUtilsConvertToInternetFormDeprecatedL(const TDesC& aData, const TDesC& aExpected);

	// Tests ConvertToDisplayForm
	//
	// In:
	// aData		- the internet uri
	// aExpected	- the display form
	//
	void TestUriUtilsConvertToDisplayFormL(const TDesC& aData, const TDesC& aExpected);
	void TestUriUtilsConvertToDisplayFormDeprecatedL(const TDesC& aData, const TDesC& aExpected);
	// Tests HasInvalidChars (16-Bit)
	//
	// In:
	// aData			- the data
	// aHadInvalidChars - expected result
	//
	void TestUriUtilsHasInvalidCharsL(const TDesC16& aData, TBool aHasInvalidChars);

	// Tests HasInvalidChars (8-Bit)
	//
	// In:
	// aData			- the data
	// aHadInvalidChars - expected result
	//
	void TestUriUtilsHasInvalidCharsL(const TDesC8& aData, TBool aHasInvalidChars);

	// Tests HostType (16-bit)
	//
	// In:
	// aData			- the data
	// aHostType		- expected host type
	void TestUriUtilsHostTypeL(const TDesC16& aData, UriUtils::TUriHostType aHostType);

	// Tests HostType (8-bit)
	//
	// In:
	// aData			- the data
	// aHostType		- expected host type
	void TestUriUtilsHostTypeL(const TDesC8& aData, UriUtils::TUriHostType aHostType);

private:	// Attributes

	// Test harness
	//
	CIpuTestHarness*	iTestHarness;

	};

#endif	// __CURIUTILSTEST_H__

