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

#ifndef __CDELIMITEDPATHSEGMENTTEST_H__
#define __CDELIMITEDPATHSEGMENTTEST_H__

#include <e32base.h>

#include "ctestbase.h"

class CDelimitedPathSegmentTest : public CTestBase
	{
public:

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CDelimitedPathSegmentTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CDelimitedPathSegmentTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CDelimitedPathSegmentTest();

	// Does the tests
	//
	void DoTestsL();

private:

	// C'tor
	//
	CDelimitedPathSegmentTest(CIpuTestHarness* aTestHarness);

	// 2nd phase construction
	//
	void ConstructL();

//
//
//	DelimitedPathSegment Tests
//
//

	// Tests extraction of path segment params from left to right.
	//
	// In:
	// aPathSegment	- descriptor with path segment
	// ...			- list of one or more params
	//
	void TestDelimitedPathSegmentExtractionL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests extraction of path segment params from right to left.
	//
	// In:
	// aPathSegment	- descriptor with path segment
	// ...			- list of one or more params
	//
	void TestDelimitedPathSegmentReverseExtractionL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests for presence of front and back delimiters
	//
	// In:
	// aPathSegment		- the path segment
	// aFrontDelim		- indicated whethere there is a front delimiter or not
	// aBackDelim		- indicated whethere there is a back delimiter or not
	//
	void TestDelimitedPathSegmentDelimiterPresenceL(const TDesC& aPathSegment, TBool aFrontDelim, TBool aBackDelim) const;

	// Tests remainder functionality for forward parsing
	//
	// In:
	// aPathSegment	- the path segment
	// ...			- list of the expected remainders
	//
	void TestDelimitedPathSegmentRemainderL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests remainder functionality for reverse parsing
	//
	// In:
	// aPathSegment	- the path segment
	// ...			- list of the expected remainders
	//
	void TestDelimitedPathSegmentRemainderReverseL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests Des() function
	//
	// In:
	// aPathSegment	- the path segment
	//
	void TestDelimitedPathSegmentDesL(const TDesC& aPathSegment) const;

	// Tests PushFront(), NewL() and NewLC()
	//
	// In:
	// aPathSegment	- the path segment
	// ...			- list of the params and expected full path segment for each push
	//
	void TestDelimitedPathSegmentPushFrontL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests PushBack(), NewL() and NewLC()
	//
	// In:
	// aPathSegment	- the path segment
	// ...			- list of the params and expected full path segment for each push
	//
	void TestDelimitedPathSegmentPushBackL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests PopFront(), NewL() and NewLC()
	//
	// In:
	// aPathSegment	- the path segment
	// ...			- list of the expected full path segment for each pop
	//
	void TestDelimitedPathSegmentPopFrontL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests PopBack(), NewL() and NewLC()
	//
	// In:
	// aPathSegment	- the path segment
	// ...			- list of the expected full path segment for each pop
	//
	void TestDelimitedPathSegmentPopBackL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests adding and triming of fromt and back delimiters
	//
	// In:
	// aPathSegment	- the path segment
	//
	void TestDelimitedPathSegmentAddAndTrimFrontAndBackDelimiterL(const TDesC& aPathSegment) const;

	// Tests InsertCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPathSegment	- the path segment
	// ...			- list of the params and expected full path segment for each insert
	//
	void TestDelimitedPathSegmentInsertAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests RemoveCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to remove
	// aPathSegment	- the path segment
	// ...			- list of the expected full path segment for each remove
	//
	void TestDelimitedPathSegmentRemoveAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests InsertCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPathSegment	- the path segment
	// ...			- list of the params and expected full path segment for each insert
	//
	void TestDelimitedPathSegmentInsertAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests RemoveCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to remove
	// aPathSegment	- the path segment
	// ...			- list of the expected full path segment for each remove
	//
	void TestDelimitedPathSegmentRemoveAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests InsertAndEscapeCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPathSegment	- the path segment
	// ...			- list of the params and expected full path segment for each insert
	//
	void TestDelimitedPathSegmentInsertEscapeAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests InsertAndEscapeCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aPathSegment	- the path segment
	// ...			- list of the params and expected full path segment for each insert
	//
	void TestDelimitedPathSegmentInsertEscapeAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests PushAndEscapeFront(), NewL() and NewLC()
	//
	// In:
	// aPathSegment	- the path segment
	// ...			- list of the params and expected full path segment for each push
	//
	void TestDelimitedPathSegmentPushAndEscapeFrontL(TRefByValue<const TDesC> aPathSegment, ...) const;

	// Tests PushAndEscapeBack(), NewL() and NewLC()
	//
	// In:
	// aPathSegment	- the path segment
	// ...			- list of the params and expected full path segment for each push
	//
	void TestDelimitedPathSegmentPushAndEscapeBackL(TRefByValue<const TDesC> aPathSegment, ...) const;

private:	// Attributes

	// Test harness
	//
	CIpuTestHarness*	iTestHarness;

	};

#endif	// __CDELIMITEDPATHSEGMENTTEST_H__


