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

#ifndef __CDELIMITEDQUERYTEST_H__
#define __CDELIMITEDQUERYTEST_H__

#include <e32base.h>

#include "ctestbase.h"

class CDelimitedQueryTest : public CTestBase
	{
public:

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CDelimitedQueryTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CDelimitedQueryTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CDelimitedQueryTest();

	// Does the tests
	//
	void DoTestsL();

private:

	// C'tor
	//
	CDelimitedQueryTest(CIpuTestHarness* aTestHarness);

	// 2nd phase construction
	//
	void ConstructL();

//
//
//	Delimited Query Tests
//
//

	// Tests extraction of query segments from left to right.
	//
	// In:
	// aQuery	- descriptor with query
	// ...		- list of one or more segments
	//
	void TestDelimitedQueryExtractionL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests extraction of query segments from right to left.
	//
	// In:
	// aQuery	- descriptor with query
	// ...		- list of one or more segments
	//
	void TestDelimitedQueryReverseExtractionL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests for presence of front and back delimiters
	//
	// In:
	// aQuery		- the query
	// aFrontDelim	- indicated whethere there is a front delimiter or not
	// aBackDelim	- indicated whethere there is a back delimiter or not
	//
	void TestDelimitedQueryDelimiterPresenceL(const TDesC& aQuery, TBool aFrontDelim, TBool aBackDelim) const;

	// Tests remainder functionality for forward parsing
	//
	// In:
	// aQuery	- the query
	// ...		- list of the expected remainders
	//
	void TestDelimitedQueryRemainderL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests remainder functionality for reverse parsing
	//
	// In:
	// aQuery	- the query
	// ...		- list of the expected remainders
	//
	void TestDelimitedQueryRemainderReverseL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests Des() function
	//
	// In:
	// aQuery	- the query
	//
	void TestDelimitedQueryDesL(const TDesC& aQuery) const;

	// Tests PushFront(), NewL() and NewLC()
	//
	// In:
	// aQuery	- the query
	// ...		- list of the query segments and expected full query for each push
	//
	void TestDelimitedQueryPushFrontL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests PushBack(), NewL() and NewLC()
	//
	// In:
	// aQuery	- the query
	// ...		- list of the query segments and expected full query for each push
	//
	void TestDelimitedQueryPushBackL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests PopFront(), NewL() and NewLC()
	//
	// In:
	// aQuery	- the query
	// ...		- list of the expected full query for each pop
	//
	void TestDelimitedQueryPopFrontL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests PopBack(), NewL() and NewLC()
	//
	// In:
	// aQuery	- the query
	// ...		- list of the expected full query for each pop
	//
	void TestDelimitedQueryPopBackL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests adding and triming of fromt and back delimiters
	//
	// In:
	// aQuery	- the query
	//
	void TestDelimitedQueryAddAndTrimFrontAndBackDelimiterL(const TDesC& aQuery) const;

	// Tests InsertCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aQuery		- the query
	// ...			- list of the query segments and expected full query for each insert
	//
	void TestDelimitedQueryInsertAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const;

	// Tests RemoveCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aQuery		- the query
	// ...			- list of the expected full query for each remove
	//
	void TestDelimitedQueryRemoveAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const;

	// Tests InsertCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aQuery		- the query
	// ...			- list of the query segments and expected full query for each insert
	//
	void TestDelimitedQueryInsertAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const;

	// Tests RemoveCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aQuery		- the query
	// ...			- list of the expected full query for each remove
	//
	void TestDelimitedQueryRemoveAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const;

	// Tests InsertAndEscapeCurrent(), Parse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aQuery		- the query
	// ...			- list of the query segments and expected full query for each insert
	//
	void TestDelimitedQueryInsertEscapeAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const;

	// Tests InsertAndEscapeCurrent(), ParseReverse()
	//
	// In:
	// aStartPos	- position from the start of parsing of where to insert
	// aQuery		- the query
	// ...			- list of the query segments and expected full query for each insert
	//
	void TestDelimitedQueryInsertEscapeAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const;

	// Tests PushAndEscapeFront(), NewL() and NewLC()
	//
	// In:
	// aQuery	- the query
	// ...		- list of the query segments and expected full query for each push
	//
	void TestDelimitedQueryPushAndEscapeFrontL(TRefByValue<const TDesC> aQuery, ...) const;

	// Tests PushAndEscapeBack(), NewL() and NewLC()
	//
	// In:
	// aQuery	- the query
	// ...		- list of the query segments and expected full query for each push
	//
	void TestDelimitedQueryPushAndEscapeBackL(TRefByValue<const TDesC> aQuery, ...) const;

private:	// Attributes

	// Test harness
	//
	CIpuTestHarness*	iTestHarness;

	};

#endif	// __CDELIMITEDQUERYTEST_H__
