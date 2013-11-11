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
// Implementation of test CWSPDecodeTest
// 
//

#include "cwspdecodetest.h"
#include <wspdummyconstants.h>



CWSPDecodeTest* CWSPDecodeTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CWSPDecodeTest* self = new(ELeave)CWSPDecodeTest(aTestHarness);
	CleanupStack::PushL(self);
	return self;
	}


CWSPDecodeTest::CWSPDecodeTest(CIpuTestHarness* aTestHarness):
	iTestHarness(aTestHarness)
	{}

CWSPDecodeTest::~CWSPDecodeTest()
	{}

void CWSPDecodeTest::DoTestsL()
	{
	VarTypesTestL();
	LengthValsTestL();
	StringTestL();
	TokensTestL();
	IntegersTestL();
	LongIntTestL();
	UintVarsTestL();
	VersionTestL();
	DateTestL();
	SegmenterTestL();
	}


//* VarTypes */

TBool CWSPDecodeTest::VarType(TPtrC8 aBuf, TWspPrimitiveDecoder::TWspHeaderType aShouldBeType)
	{
	TWspPrimitiveDecoder dec(aBuf);
	TWspPrimitiveDecoder::TWspHeaderType type=dec.VarType();
	
	TBool bSuccess  = EFalse;
	if (type == aShouldBeType)
		bSuccess = ETrue;

	TBuf8<25> typeStr;
	switch (aShouldBeType)
	{
	case TWspPrimitiveDecoder::ENotSet:
		typeStr.Copy(_L("ENotSet"));
		break;
	case TWspPrimitiveDecoder::ELengthVal:
		typeStr.Copy(_L("ELengthVal"));
		break;
	case TWspPrimitiveDecoder::EQuotedString:
		typeStr.Copy(_L("EQuotedString"));
		break;	
	case TWspPrimitiveDecoder::EString:
		typeStr.Copy(_L("EString"));
		break;
	case TWspPrimitiveDecoder::E7BitVal:
		typeStr.Copy(_L("E7BitVal"));
		break;
	}
	return bSuccess;
	}


void CWSPDecodeTest::VarTypesTestL()
	{
	_LIT(KTestText, "WSP Decode: VarTypes");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestVarTypesL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}

void CWSPDecodeTest::TestVarTypesL()
	{
	const TUint8 KToken[] = {0x81};
	const TUint8 KString1[] = {'a', 'b', 'c', 0x00};
	const TUint8 KString2[] = {0x33};
	const TUint8 KLen0[] = {0x00};
	const TUint8 KLen1[] = {0x01};
	const TUint8 KLen2[] = {0x1F};
		
	TBuf8<255> buf;
	buf.Copy(KToken, sizeof(KToken));
	if (!VarType(buf, TWspPrimitiveDecoder::E7BitVal))
		User::Leave(KErrGeneral);

	buf.Copy(KString1, sizeof(KString1));
	if (!VarType(buf, TWspPrimitiveDecoder::EString))
		User::Leave(KErrGeneral);

	buf.Copy(KString2, sizeof(KString2));
	if (!VarType(buf, TWspPrimitiveDecoder::EString))
		User::Leave(KErrGeneral);

	buf.Copy(KLen0, sizeof(KLen0));
	if (!VarType(buf, TWspPrimitiveDecoder::ELengthVal))
		User::Leave(KErrGeneral);

	buf.Copy(KLen1, sizeof(KLen1));
	if (!VarType(buf, TWspPrimitiveDecoder::ELengthVal))
		User::Leave(KErrGeneral);

	buf.Copy(KLen2, sizeof(KLen2));
	if (!VarType(buf, TWspPrimitiveDecoder::ELengthVal))
		User::Leave(KErrGeneral);
	}




//* LenVal */

TBool CWSPDecodeTest::LenVal(TPtrC8 aBuf, TInt aShouldBeVal)
	{
	TBool success  = EFalse;
	TWspPrimitiveDecoder dec(aBuf);	
	TInt len;
	success = dec.LengthVal(len);
	if (success == KErrCorrupt)
		return ETrue;

	return len == aShouldBeVal && !(success < KErrNone);
	}


void CWSPDecodeTest::LengthValsTestL()
	{
	_LIT(KTestText, "WSP Decode: LengthVals");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestLengthValsL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}

void CWSPDecodeTest::TestLengthValsL()
	{
	const TUint8 KLen0[] = {0x00};
	const TUint8 KLen1[] = {0x01};
	const TUint8 KLen26[] = {0x1A};
	const TUint8 KLenUintVAR17680[] = {0x1F, 0x81,0x8A,0x10};
	const TUint8 KLenUintVARCorrupt[] = {0x1F, 0x81,0x8A,0x80};
		
	
	TBuf8<255> buf;
	buf.Copy(KLen0, sizeof(KLen0));
	if (!LenVal(buf, 0))
		User::Leave(KErrGeneral);

	buf.Copy(KLen1, sizeof(KLen1));
	if (!LenVal(buf, 1))
		User::Leave(KErrGeneral);

	buf.Copy(KLen26, sizeof(KLen26));
	if (!LenVal(buf, 0x1A))
		User::Leave(KErrGeneral);

	buf.Copy(KLenUintVAR17680, sizeof(KLenUintVAR17680));
	if (!LenVal(buf, 17680))
		User::Leave(KErrGeneral);
	
	buf.Copy(KLenUintVARCorrupt, sizeof(KLenUintVARCorrupt));
	if (!LenVal(buf, -1))
		User::Leave(KErrGeneral);

	}



//* String */

TBool CWSPDecodeTest::String(TPtrC8 aBuf, TPtrC8 aShouldBe, TInt /*aErr*/)
	{
	TBool success  = EFalse;
	TWspPrimitiveDecoder dec(aBuf);
	
	TPtrC8 str;
	success = dec.String(str);
	if (success < KErrNone)
		return success;


	if (str != aShouldBe)
		success = EFalse;

	// Copy to 16 bits
	return success;
	}



void CWSPDecodeTest::StringTestL()
	{
	_LIT(KTestText, "WSP Decode: String");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestStringL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


void CWSPDecodeTest::TestStringL()
	{
	const TUint8 KStringHello[] = {'H','e','l','l','o',0x00};
	const TUint8 KStringEmpty[] = {0x00};
	const TUint8 KStringNotAString[] = {0x1A};
	const TUint8 KStringCorruptNoZero[] = {'H','e','l','l','o'};
	const TUint8 KStringbye[] = {'b','y','e',0x00};
		
	TBuf8<255> buf;
	
	buf.Copy(KStringHello, sizeof(KStringHello));
	if (!String(buf, _L8("Hello")))
		User::Leave(KErrGeneral);

	buf.Copy(KStringEmpty, sizeof(KStringEmpty));
	if (!String(buf, _L8(""),KErrCorrupt))
		User::Leave(KErrGeneral);

	buf.Copy(KStringNotAString, sizeof(KStringNotAString));
	if (!String(buf, _L8(""), KErrCorrupt))
		User::Leave(KErrGeneral);

	buf.Copy(KStringCorruptNoZero, sizeof(KStringCorruptNoZero));
	if (!String(buf, _L8(""), KErrCorrupt))
		User::Leave(KErrGeneral);
	
	buf.Copy(KStringbye, sizeof(KStringbye));
	if (!String(buf, _L8("bye")))
		User::Leave(KErrGeneral);
	}

//* Tokens */

TBool CWSPDecodeTest::Token(TPtrC8 aBuf, TUint8 aShouldBe, TInt aErr)
	{
	TInt success;
	TWspPrimitiveDecoder dec(aBuf);
	
	TUint8 val;
	success = dec.Val7Bit(val);
	if (success < KErrNone && success == aErr)
		return ETrue;

	return val == aShouldBe;
	}

void CWSPDecodeTest::TokensTestL()
	{
	_LIT(KTestText, "WSP Decode: Tokens");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestTokensL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


void CWSPDecodeTest::TestTokensL()
	{
	const TUint8 KToken0[] = {0x80};
	const TUint8 KToken1[] = {0x81};
	const TUint8 KToken1A[] = {0x9A};
	const TUint8 KToken38[] = {0xB8};
	const TUint8 KTokenCorruptLen[] = {0x01};
	TBuf8<255> buf;
	
	buf.Copy(KToken0, sizeof(KToken0));
	if (!Token(buf, 0))
		User::Leave(KErrGeneral);

	buf.Copy(KToken1, sizeof(KToken1));
	if (!Token(buf, 1))
		User::Leave(KErrGeneral);

	buf.Copy(KToken1A, sizeof(KToken1A));
	if (!Token(buf, 0x1A))
		User::Leave(KErrGeneral);

	buf.Copy(KToken38, sizeof(KToken38));
	if (!Token(buf, 0x38))
		User::Leave(KErrGeneral);
	
	buf.Copy(KTokenCorruptLen, sizeof(KTokenCorruptLen));
	if (!Token(buf, 1))
		User::Leave(KErrGeneral);
	}


//* Integers */

TBool CWSPDecodeTest::Integer(TPtrC8 aBuf, TUint32 aShouldBe, TInt aErr)
	{
	TInt success;
	TWspPrimitiveDecoder dec(aBuf);
	TUint32 val;
	success = dec.Integer(val);
	if (success < KErrNone && success == aErr)
		return success;
	return val == aShouldBe;
	}



void CWSPDecodeTest::IntegersTestL()
	{
	_LIT(KTestText, "WSP Decode: Integers");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestIntegersL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


void CWSPDecodeTest::TestIntegersL()
	{
	const TUint8 KInt0[] = {0x80};
	const TUint8 KInt1[] = {0x81};
	const TUint8 KIntA[] = {0x8A};
	const TUint8 KInt38[] = {0xB8};
	const TUint8 KIntLong4321[] = {0x02, 0x43,0x21};
	const TUint8 KIntLongABCDEF[] = {0x03, 0xAB,0xCD,0xEF};
	const TUint8 KIntCorrupt[] = {0x01};
	TBuf8<255> buf;
	
	buf.Copy(KInt0, sizeof(KInt0));
	if (!Integer(buf, 0))
		User::Leave(KErrGeneral);

	buf.Copy(KInt1, sizeof(KIntA));
	if (!Integer(buf, 1))
		User::Leave(KErrGeneral);

	buf.Copy(KIntA, sizeof(KIntA));
	if (!Integer(buf, 0x0A))
		User::Leave(KErrGeneral);

	buf.Copy(KInt38, sizeof(KInt38));
	if (!Integer(buf, 0x38))
		User::Leave(KErrGeneral);
	
	buf.Copy(KIntLong4321, sizeof(KIntLong4321));
	if (!Integer(buf, 0x4321))
		User::Leave(KErrGeneral);

	buf.Copy(KIntLongABCDEF, sizeof(KIntLongABCDEF));
	if (!Integer(buf, 0xABCDEF))
		User::Leave(KErrGeneral);

	buf.Copy(KIntCorrupt, sizeof(KIntCorrupt));
	if (!Integer(buf,0, KErrCorrupt))
		User::Leave(KErrGeneral);
	}

//* LongInt */

TBool CWSPDecodeTest::LongInt(TPtrC8 aBuf, TUint32 aShouldBe, TInt aErr)
	{
	TInt success;
	TWspPrimitiveDecoder dec(aBuf);
	
	TUint32 val;
	success = dec.LongInt(val);
	if (success < KErrNone && success == aErr)
		return ETrue;
	return val == aShouldBe;
	}



void CWSPDecodeTest::LongIntTestL()
	{
	_LIT(KTestText, "WSP Decode: LongInt");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestLongIntL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}



void CWSPDecodeTest::TestLongIntL()
	{
	const TUint8 KIntLong0[] = {0x01, 0x00};
	const TUint8 KIntLong4321[] = {0x02, 0x43,0x21};
	const TUint8 KIntLongABCDEF[] = {0x03, 0xAB,0xCD,0xEF};
	const TUint8 KIntLongABCDEF112233[] = {0x06, 0xAB,0xCD,0xEF, 0x11,0x22,0x33};
	const TUint8 KIntTooLong110009ABCDEF112233[] = {0x0A, 0x11,0x00, 0x09, 0xAB,0xCD,0xEF, 0x11,0x22,0x33};
	const TUint8 KIntCorrupt[] = {0x01};
	TBuf8<255> buf;
	buf.Copy(KIntLong0, sizeof(KIntLong0));
	if (!LongInt(buf, 0))
		User::Leave(KErrGeneral);

	buf.Copy(KIntLong4321, sizeof(KIntLong4321));
	if (!LongInt(buf, 0x4321))
		User::Leave(KErrGeneral);

	buf.Copy(KIntLongABCDEF, sizeof(KIntLongABCDEF));
	if (!LongInt(buf, 0xABCDEF))
		User::Leave(KErrGeneral);

	buf.Copy(KIntLongABCDEF112233, sizeof(KIntLongABCDEF112233));
	if (!LongInt(buf, 0xEF112233))
		User::Leave(KErrGeneral);
	
	buf.Copy(KIntTooLong110009ABCDEF112233, sizeof(KIntTooLong110009ABCDEF112233));
	if (!LongInt(buf, 0x09ABCDEF, KErrCorrupt))
		User::Leave(KErrGeneral);

	buf.Copy(KIntCorrupt, sizeof(KIntCorrupt));
	if (!LongInt(buf, 0, KErrCorrupt))
		User::Leave(KErrGeneral);
	}



//* UintVar */

TBool CWSPDecodeTest::UintVar(TPtrC8 aBuf, TUint32 aShouldBe, TInt aErr)
	{
	TInt success;
	TWspPrimitiveDecoder dec(aBuf);
	TUint32 val;
	success = dec.UintVar(val);
	if (success < KErrNone && success == aErr)
		return ETrue;
	return val == aShouldBe;
	}


void CWSPDecodeTest::UintVarsTestL()
	{
	_LIT(KTestText, "WSP Decode: UintVars");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestUintVarsL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


void CWSPDecodeTest::TestUintVarsL()
	{
	const TUint8 KUint0[] = {0x00};
	const TUint8 KUint1[] = {0x01};
	const TUint8 KUintA[] = {0x80,0x0A};
	const TUint8 KUintFA[] = {0x81,0x7A};
	const TUint8 KLenUintVAR4510[] = {0x81, 0x8A, 0x10};
	const TUint8 KLenUintVARCorrupt[] = {0x8F, 0x81,0x8A,0x80,0x88};
	const TUint8 KIntLongACorrupt[] = {0xF3, 0xAB,0xCD,0xEF};
		
	
	TBuf8<255> buf;
	
	buf.Copy(KUint0, sizeof(KUint0));
	if (!UintVar(buf, 0))
		User::Leave(KErrGeneral);

	buf.Copy(KUint1, sizeof(KUint1));
	if (!UintVar(buf, 1))
		User::Leave(KErrGeneral);

	buf.Copy(KUintA, sizeof(KUintA));
	if (!UintVar(buf, 0x0A))
		User::Leave(KErrGeneral);

	buf.Copy(KUintFA, sizeof(KUintFA));
	if (!UintVar(buf, 0xFA))
		User::Leave(KErrGeneral);
	
	buf.Copy(KLenUintVAR4510, sizeof(KLenUintVAR4510));
	if (!UintVar(buf, 0x4510))
		User::Leave(KErrGeneral);

	buf.Copy(KLenUintVARCorrupt, sizeof(KLenUintVARCorrupt));
	if (!UintVar(buf, 0, KErrCorrupt))
		User::Leave(KErrGeneral);

	buf.Copy(KIntLongACorrupt, sizeof(KIntLongACorrupt));
	if (!UintVar(buf, 0, KErrCorrupt))
		User::Leave(KErrGeneral);
	}




//* VersionL */

TBool CWSPDecodeTest::VersionL(RStringPool aPool, TPtrC8 aBuf, TPtrC8 aShouldBe, TInt aErr)
	{
	TInt success;
	TWspPrimitiveDecoder dec(aBuf);
	TBool retVal = EFalse;
	RStringF val;
	success = dec.VersionL(aPool,val);
	CleanupClosePushL(val);
	if (success < KErrNone && success == aErr)
		retVal = ETrue;
	else
		{
		RStringF should = aPool.OpenFStringL(aShouldBe);
		retVal = val == should;
		should.Close();
		}
	CleanupStack::PopAndDestroy(&val);
	return retVal;
	}

void CWSPDecodeTest::VersionTestL()
	{
	_LIT(KTestText, "WSP Decode: Version");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestVersionL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}

void CWSPDecodeTest::TestVersionL()
	{
	RStringPool pool; 
	pool.OpenL();
	CleanupClosePushL(pool);
	const TUint8 KVersion1[] = {0x9F};
	const TUint8 KVersion11[] = {0x91};
	const TUint8 KVersion610[] = {0xEA};
	const TUint8 KVersion73[] = {0xF3};
	const TUint8 KVersion00[] = {0x80};
	const TUint8 KVersionStr[] = {'1','.','9','9',0x00};
	TBuf8<255> buf;
	
	buf.Copy(KVersion1, sizeof(KVersion1));
	if (!VersionL(pool, buf, _L8("1")))
		User::Leave(KErrGeneral);

	buf.Copy(KVersion11, sizeof(KVersion11));
	if (!VersionL(pool, buf, _L8("1.1")))
		User::Leave(KErrGeneral);

	buf.Copy(KVersion610, sizeof(KVersion610));
	if (!VersionL(pool, buf, _L8("6.10")))
		User::Leave(KErrGeneral);

	buf.Copy(KVersion73, sizeof(KVersion73));
	if (!VersionL(pool, buf, _L8("7.3")))
		User::Leave(KErrGeneral);

	buf.Copy(KVersion00, sizeof(KVersion00));
	if (!VersionL(pool, buf, _L8("0.0")))
		User::Leave(KErrGeneral);
	
	buf.Copy(KVersionStr, sizeof(KVersionStr));
	if (!VersionL(pool, buf, _L8("1.99")))
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(&pool);
	}


//* Date */

TBool CWSPDecodeTest::Date(TPtrC8 aBuf, TTime aShouldBe, TInt aErr)
	{
	TInt success;
	TWspPrimitiveDecoder dec(aBuf);
	TDateTime val;
	success = dec.Date(val);
	if (success < KErrNone && success == aErr)
		return ETrue;

	TTime time(val);
	return time == aShouldBe;
	}


void CWSPDecodeTest::DateTestL()
	{
	_LIT(KTestText, "WSP Decode: Date");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestDateL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


void CWSPDecodeTest::TestDateL()
	{
	// 1 long, value 0
	const TUint8 KDate0[] = {0x01, 0x00};
	const TUint8 KDateAprl132001[] = {0x04,0x3A,0xD6,0x41,0x80};
	const TUint8 KDateDec132003[] = {0x04,0x3F,0xE2,0x8D,0x84};
	TBuf8<255> buf;
	TDateTime dt(1970,EJanuary,0,0,0,0,0);
	TTime time1(dt);
	buf.Copy(KDate0, sizeof(KDate0));
	if (!Date(buf, time1))
		User::Leave(KErrGeneral);

	TTime time2(dt);
	time2 += TTimeIntervalSeconds(0x3AD64180);
	buf.Copy(KDateAprl132001, sizeof(KDateAprl132001));
	if (!Date(buf, time2))
		User::Leave(KErrGeneral);

	TTime time3(dt);
	time3 += TTimeIntervalSeconds(0x3FE28D84);
	buf.Copy(KDateDec132003, sizeof(KDateDec132003));
	if (!Date(buf, time3))
		User::Leave(KErrGeneral);
	}


TInt CWSPDecodeTest::SegmenterCommonL(RStringPool aPool, TPtrC8 aHdr)
	{
	TInt err;
	TWspField fld;
	
	// Pre-condition to create WSP header segment: Open the string pool 
	// with the string table. Else program will raise a panic stringtable:4 
	// ( EStringTableNotFound )
	aPool.OpenL ( WSPDummy::Table );
	TWspHeaderSegmenter segger(aPool, WSPDummy::Table, aHdr);

	while ((err = segger.NextL(fld)) == KErrNone)
		{
		fld.iHdrName.Close();
		}
	return err;
	}

TBool CWSPDecodeTest::SegmenterL(RStringPool aPool, TPtrC8 aHdr)
	{
	TInt err = SegmenterCommonL(aPool,aHdr);
	if (err != KErrNotFound)
		return EFalse;
	return ETrue;
	}

TBool CWSPDecodeTest::SegmenterExpectCorruptL(RStringPool aPool, TPtrC8 aHdr)
	{
	TInt err = SegmenterCommonL(aPool,aHdr);
	if (err != KErrCorrupt)
		return EFalse;
	return ETrue;
	}

void CWSPDecodeTest::SegmenterTestL()
	{
	_LIT(KTestText, "WSP Decode: Segmenter");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestSegmenterL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


void CWSPDecodeTest::TestSegmenterL()
	{
	TBuf8<255> buf;
	RStringPool pool;
	pool.OpenL();
	CleanupClosePushL(pool);

	// Test Headers
	const TUint8 KAcceptAppVndWMLC[] = {0x80, 0x94};
	const TUint8 KAcceptLanEngQ07[] = {0x82, 0x02, 0x99, 0x47};
	const TUint8 KAcceptLanENSV[] = {0x82, 0x02, 0x83, 0xF0};
	const TUint8 KDateThu23Apr1998134127GMT[] = {0x92, 0x04, 0x35, 0x3f, 0x45, 0x11};
	const TUint8 KContentRangeBytes0499[] = {0x90, 0x03, 0x00, 0x88, 0x01};
	const TUint8 KAcceptRangeNewRangeUnit[] = {0x84, 'n','e','w','-','r','a','n','g','e','-','u','n','i','t',0x00};
	const TUint8 KXNewHdrFoo[] = {'X','-','n','e','w','-','H','e','a','d','e','r',0x00,'F','o','o',0x00};
	const TUint8 KTwo[] = {0x80, 0x94, 0x81, 0x93};

	const TUint8 KCorrupt1[] = {0x82};
	buf.Copy(KCorrupt1, sizeof(KCorrupt1));
	if (!SegmenterExpectCorruptL(pool, buf))
		User::Leave(KErrGeneral);

	const TUint8 KCorrupt2[] = {0x82, 0x02};
	buf.Copy(KCorrupt2, sizeof(KCorrupt2));
	if (!SegmenterExpectCorruptL(pool, buf))
		User::Leave(KErrGeneral);

	buf.Copy(KAcceptAppVndWMLC, sizeof(KAcceptAppVndWMLC));
	if (!SegmenterL(pool, buf))
		User::Leave(KErrGeneral);

	buf.Copy(KAcceptLanEngQ07, sizeof(KAcceptLanEngQ07));
	if (!SegmenterL(pool, buf))
		User::Leave(KErrGeneral);

	buf.Copy(KAcceptLanENSV, sizeof(KAcceptLanENSV));
	if (!SegmenterL(pool, buf))
		User::Leave(KErrGeneral);

	buf.Copy(KDateThu23Apr1998134127GMT, sizeof(KDateThu23Apr1998134127GMT));
	if (!SegmenterL(pool, buf))
		User::Leave(KErrGeneral);

	buf.Copy(KContentRangeBytes0499, sizeof(KContentRangeBytes0499));
	if (!SegmenterL(pool, buf))
		User::Leave(KErrGeneral);

	buf.Copy(KAcceptRangeNewRangeUnit, sizeof(KAcceptRangeNewRangeUnit));
	if (!SegmenterL(pool, buf))
		User::Leave(KErrGeneral);
	
	buf.Copy(KXNewHdrFoo, sizeof(KXNewHdrFoo));
	if (!SegmenterL(pool, buf))
		User::Leave(KErrGeneral);

	buf.Copy(KTwo, sizeof(KTwo));
	if (!SegmenterL(pool, buf))
		User::Leave(KErrGeneral);

	CleanupStack::PopAndDestroy(&pool);
	}

/*

void CWSPDecodeTest::XXXTestL()
	{
	_LIT(KTestText, "WSP Decode: XXX");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestXXXL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


*/
