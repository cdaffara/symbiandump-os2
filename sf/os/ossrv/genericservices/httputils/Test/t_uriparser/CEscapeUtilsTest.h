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

#ifndef __CESCAPEUTILSTEST_H__
#define __CESCAPEUTILSTEST_H__

#include <e32base.h>
#include <escapeutils.h>

#include "ctestbase.h"

class CEscapeUtilsTest : public CTestBase
	{
public:
 
	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CEscapeUtilsTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CEscapeUtilsTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CEscapeUtilsTest();

	// Does the tests
	//
	void DoTestsL();

private:

	// C'tor
	//
	CEscapeUtilsTest(CIpuTestHarness* aTestHarness);

	// 2nd phase construction
	//
	void ConstructL();

	// Tests EscapeEncodeL functions
	//
	// In:
	// aData		- the data to escape
	// aExpected	- the expected escaped result
	//
	void TestEscapeUtilsEscapeEncodeL(const TDesC& aData, const TDesC& aExpected, EscapeUtils::TEscapeMode aEscapeMode);

	// Tests EscapeDecodeL functions
	//
	// In:
	// aData		- the data to escape
	// aExpected	- the expected escaped result
	//
	void TestEscapeUtilsEscapeDecodeL(const TDesC& aData, const TDesC& aExpected);

	// Tests unicode/utf8 conversions - NOTE the utf8 version is escaped encoded.
	//
	// In:
	// aUnicode	- the unicode data
	// aUtf8	- the utf8 data (escape encoded)
	//
	void TestEscapeUtilsUnicodeUtf8ConversionsL(const TDesC& aUnicode, const TDesC& aUtf8);

	// Tests IsExcludedChar
	//
	// In:
	// aChar		- the test char
	// aIsExcluded	- the expected result
	//
	void TestEscapeUtilsIsExcludedCharL(TChar aChar, TBool aIsExcluded);

	// Tests IsEscapeTriple
	//
	// In:
	// aData		- the data to check
	// aIsTriple	- the expected result
	// aValue		- the value of the triple (set to anything if aIsTriple is EFalse)
	//
	void TestEscapeUtilsIsEscapeTripleL(const TDesC& aData, TBool aIsTriple, TInt aValue);
	
	// Tests Base64 Encoding and Decoding
	//
	// In:
	// aData		- the data to check
	//
	void TestEscapeUtilsBase64ConversionL(const TDesC& aData);

private:	// Attributes

	// Test harness
	//
	CIpuTestHarness*	iTestHarness;

	};

#endif	// __CESCAPEUTILSTEST_H__

