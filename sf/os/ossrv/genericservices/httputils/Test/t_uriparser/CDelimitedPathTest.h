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

#ifndef __CDELIMITEDPATHTEST_H__
#define __CDELIMITEDPATHTEST_H__

#include <e32base.h>

#include "ctestbase.h"

class CDelimitedPathTest : public CTestBase
	{
public:

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CDelimitedPathTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CDelimitedPathTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CDelimitedPathTest();

	// Does the tests
	//
	void DoTestsL();

private:

	// C'tor
	//
	CDelimitedPathTest(CIpuTestHarness* aTestHarness);

	// 2nd phase construction
	//
	void ConstructL();

//
//
//	DelimitedPath Tests
//
//

	// Tests extraction of path segments from left to right.
	//
	// In:
	// aPath	- descriptor with path
	// ...		- list of one or more segments
	//
	void TestDelimitedPathExtractionL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests extraction of path segments from right to left.
	//
	// In:
	// aPath	- descriptor with path
	// ...		- list of one or more segments
	//
	void TestDelimitedPathReverseExtractionL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests for presence of front and back delimiters
	//
	// In:
	// aPath		- the path
	// aFrontDelim	- indicated whethere there is a front delimiter or not
	// aBackDelim	- indicated whethere there is a back delimiter or not
	//
	void TestDelimitedPathDelimiterPresenceL(const TDesC& aPath, TBool aFrontDelim, TBool aBackDelim) const;

	// Tests remainder functionality for forward parsing
	//
	// In:
	// aPath	- the path
	// ...		- list of the expected remainders
	//
	void TestDelimitedPathRemainderL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests remainder functionality for reverse parsing
	//
	// In:
	// aPath	- the path
	// ...		- list of the expected remainders
	//
	void TestDelimitedPathRemainderReverseL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests Des() function
	//
	// In:
	// aPath	- the path
	//
	void TestDelimitedPathDesL(const TDesC& aPath) const;

	// Tests PushFront(), NewL() and NewLC()
	//
	// In:
	// aPath	- the path
	// ...		- list of the path segments and expected full path for each push
	//
	void TestDelimitedPathPushFrontL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests PushBack(), NewL() and NewLC()
	//
	// In:
	// aPath	- the path
	// ...		- list of the path segments and expected full path for each push
	//
	void TestDelimitedPathPushBackL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests PopFront(), NewL() and NewLC()
	//
	// In:
	// aPath	- the path
	// ...		- list of the expected full path for each pop
	//
	void TestDelimitedPathPopFrontL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests PopBack(), NewL() and NewLC()
	//
	// In:
	// aPath	- the path
	// ...		- list of the expected full path for each pop
	//
	void TestDelimitedPathPopBackL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests adding and triming of fromt and back delimiters
	//
	// In:
	// aPath	- the path
	//
	void TestDelimitedPathAddAndTrimFrontAndBackDelimiterL(const TDesC& aPath) const;

	// Tests InsertCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPath		- the path
	// ...			- list of the path segments and expected full path for each insert
	//
	void TestDelimitedPathInsertAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const;

	// Tests RemoveCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPath		- the path
	// ...			- list of the expected full path for each remove
	//
	void TestDelimitedPathRemoveAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const;

	// Tests InsertCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPath		- the path
	// ...			- list of the path segments and expected full path for each insert
	//
	void TestDelimitedPathInsertAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const;

	// Tests RemoveCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPath		- the path
	// ...			- list of the expected full path for each remove
	//
	void TestDelimitedPathRemoveAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const;

	// Tests InsertAndEscapeCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPath		- the path
	// ...			- list of the path segments and expected full path for each insert
	//
	void TestDelimitedPathInsertEscapeAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const;

	// Tests InsertAndEscapeCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPath		- the path
	// ...			- list of the path segments and expected full path for each insert
	//
	void TestDelimitedPathInsertEscapeAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const;

	// Tests PushAndEscapeFront(), NewL() and NewLC()
	//
	// In:
	// aPath	- the path
	// ...		- list of the path segments and expected full path for each push
	//
	void TestDelimitedPathPushAndEscapeFrontL(TRefByValue<const TDesC> aPath, ...) const;

	// Tests PushAndEscapeBack(), NewL() and NewLC()
	//
	// In:
	// aPath	- the path
	// ...		- list of the path segments and expected full path for each push
	//
	void TestDelimitedPathPushAndEscapeBackL(TRefByValue<const TDesC> aPath, ...) const;

private:	// Attributes

	// Test harness
	//
	CIpuTestHarness*	iTestHarness;

	};

#endif	// __CDELIMITEDPATHTEST_H__
