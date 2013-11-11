// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class CWSPDecodeTest 
// 
//

#ifndef		__CWSPDECODETEST_H__
#define		__CWSPDECODETEST_H__

#include <e32base.h>

#include "IpuTestUtils.h"
#include "ctestbase.h"
#include <wspdecoder.h>

class CWSPDecodeTest: public CTestBase
	{
public:
	static CWSPDecodeTest* NewLC(CIpuTestHarness* aTestHarness);
	~CWSPDecodeTest();
	void DoTestsL();

private:
	CWSPDecodeTest(CIpuTestHarness* aTestHarness);

	void VarTypesTestL();
	void TestVarTypesL();
	TBool VarType(TPtrC8 aBuf, TWspPrimitiveDecoder::TWspHeaderType aShouldBeType);

	void LengthValsTestL();
	void TestLengthValsL();
	TBool LenVal(TPtrC8 aBuf, TInt aShouldBeVal);

	void StringTestL();
	void TestStringL();
	TBool String(TPtrC8 aBuf, TPtrC8 aShouldBe, TInt aErr = KErrNone);

	void TokensTestL();
	void TestTokensL();
	TBool Token(TPtrC8 aBuf, TUint8 aShouldBe, TInt aErr= KErrNone);

	void IntegersTestL();
	void TestIntegersL();
	TBool Integer(TPtrC8 aBuf, TUint32 aShouldBe, TInt aErr = KErrNone);

	void LongIntTestL();
	void TestLongIntL();
	TBool LongInt(TPtrC8 aBuf, TUint32 aShouldBe, TInt aErr = KErrNone);

	void UintVarsTestL();
	void TestUintVarsL();
	TBool UintVar(TPtrC8 aBuf, TUint32 aShouldBe, TInt aErr = KErrNone);

	void VersionTestL();
	void TestVersionL();
	TBool VersionL(RStringPool aPool, TPtrC8 aBuf, TPtrC8 aShouldBe, TInt aErr = KErrNone);
	
	void DateTestL();
	void TestDateL();
	TBool Date(TPtrC8 aBuf, TTime aShouldBe, TInt aErr = KErrNone);

	void SegmenterTestL();
	void TestSegmenterL();
	TInt SegmenterCommonL(RStringPool aPool, TPtrC8 aHdr);
	TBool SegmenterL(RStringPool aPool, TPtrC8 aHdr);
	TBool SegmenterExpectCorruptL(RStringPool aPool, TPtrC8 aHdr);

private:
	CIpuTestHarness* iTestHarness;
	};



#endif //	__CWSPDECODETEST_H__
