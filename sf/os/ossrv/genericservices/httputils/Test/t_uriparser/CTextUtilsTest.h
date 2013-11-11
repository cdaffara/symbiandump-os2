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

#ifndef __CTEXTUTILSTEST_H__
#define __CTEXTUTILSTEST_H__

#include <e32base.h>

#include "IpuTestUtils.h"
#include "ctestbase.h"

class CTextUtilsTest : public CTestBase
	{
public:

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CTextUtilsTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CTextUtilsTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CTextUtilsTest();

	// Does the tests
	//
	void DoTestsL();

private:

	// C'tor
	//
	CTextUtilsTest(CIpuTestHarness* aTestHarness);

	// 2nd phase construction
	//
	void ConstructL();

	// Tests RemoveWhitespace
	//
	// In:
	// aData			- the data
	// aExpectedLeft	- expected result of removing left-most whitespace
	// aConsumedLeft	- the number of cosumed whitespaces from the left
	// aExpectedRight	- expected result of removing right-most whitespace
	// aConsumedRight	- the number of cosumed whitespaces from the right
	// aExpectedBoth	- expected result of removing left- and right-most whitespace
	// aConsumedBoth	- the number of cosumed whitespaces from the left and right
	//
	void TestTextUtilsRemoveL(const TDesC& aBase, const TDesC& aRemoveLeft, TInt aConsumedLeft,
							const TDesC& aRemoveRight, TInt aConsumedRight,
							const TDesC& aRemoveBoth, TInt aConsumedBoth);

	// Tests ConvertDescriptorToInt
	//
	// In:
	// aDes		- the descriptor
	// aInt		- expected int value (any number if aValid is EFalse)
	// aValid	- indicates whether aDes is a valid int representation.
	//
	void TestTextUtilsDesToIntConversionL(const TDesC& aDes, TInt aInt, TBool aValid);

	// Tests ConvertIntToDescriptor
	//
	// In:
	// aInt		- the int value
	// aDes		- the expecteddescriptor
	//
	void TestTextUtilsIntToDesConversionL(TInt aInt, const TDesC& aDes);

	void TestTextUtilsDesToHexConversionL(const TDesC& aDes, TInt aHex, TBool aValid);
	void TestTextUtilsDesToHexConversion8L(const TDesC& aDes, TInt aHex, TBool aValid);


	void TestTextUtilsHexToDesConversionL(TInt aHex, const TDesC& aDes);

	void TestTextUtilsExtractQuotedStringL(
										  const TDesC&	aBuffer, 
										  const TDesC&	aString, 
										  const TDesC&	aRemainder,
										  TInt			aExpectedError
										  );

	void TestTextUtilsExtractTokenFromListL(
										   const TDesC& aBuffer,
										   const TDesC& aToken0,
										   const TDesC& aToken1,
										   const TDesC& aToken2,
										   const TDesC& aRemainder,
										   const TDesC& aSeparator
										   );

	void TestTextUtilsExtractTokenFromListL(
										   const TDesC& aBuffer,
										   const TDesC& aToken0,
										   const TDesC& aToken1,
										   const TDesC& aToken2,
										   const TDesC& aRemainder,
										   TChar		aSeparator
										   );

private:	// Attributes

	// Test harness
	//
	CIpuTestHarness*	iTestHarness;

	};

#endif	// __CTEXTUTILSTEST_H__

