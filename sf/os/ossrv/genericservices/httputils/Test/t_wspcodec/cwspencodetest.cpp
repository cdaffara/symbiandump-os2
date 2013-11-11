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
// Implementation of test CWSPEncodeTest.
// 
//


#include "cwspencodetest.h"
#include <wspencoder.h>


CWSPEncodeTest* CWSPEncodeTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CWSPEncodeTest* self = new(ELeave)CWSPEncodeTest(aTestHarness);
	CleanupStack::PushL(self);
	return self;
	}


CWSPEncodeTest::CWSPEncodeTest(CIpuTestHarness* aTestHarness):
	iTestHarness(aTestHarness)
	{}

CWSPEncodeTest::~CWSPEncodeTest()
	{}

void CWSPEncodeTest::DoTestsL()
	{
	HeaderEncoderTestL();
	ShortIntTestL();
	LongIntTestL();
	UintVarTestL();
	StringTestL();
	DateTestL();
	}
	

void CWSPEncodeTest::HeaderEncoderTestL()
	{
	_LIT(KTestText, "WSP Encode: Header");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error, TestHeaderEncoderL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}

void CWSPEncodeTest::TestHeaderEncoderL()
	{
	TInt err=0;

	CWspHeaderEncoder* primEncoder = CWspHeaderEncoder::NewLC();

	primEncoder->StartHeaderL(0x27);
	primEncoder->StartValueLengthL();
	primEncoder->AddUintVarL(0xff);
	primEncoder->AddLongIntL(999999);
	primEncoder->AddIntegerL(0x7F);
	primEncoder->EndValueLengthL();
	HBufC8* buf = primEncoder->EndHeaderL();
	CleanupStack::PushL(buf);
	CleanupStack::PopAndDestroy(2); //primEncoder, buf

	User::LeaveIfError(err);
	}


void CWSPEncodeTest::ShortIntTestL()
	{
	_LIT(KTestText, "WSP Encode: ShortInt");
	iTestHarness->StartTestL(KTestText);
	TInt error = TestShortInt();
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


TInt CWSPEncodeTest::TestShortInt()
	{
	TUint8 input;
	TUint8 output;

	// TWspPrimitiveEncoder::ShortInt
	// Check 7 and 8 bit values. If 8bit notinh happens
	input=0; // MIN input value
	output = TWspPrimitiveEncoder::ShortInt(input);
	if (output !=128)
		return KErrGeneral;
		
	input=35; // MAX input value
	output = TWspPrimitiveEncoder::ShortInt(input);
	if (output !=163)
		return KErrGeneral;

	input=0x7F; // MAX input value
	output = TWspPrimitiveEncoder::ShortInt(input);
	if (output !=255)
		return KErrGeneral;

	input=0x80; // invalid value
	output = TWspPrimitiveEncoder::ShortInt(input);
	if (output !=0)
		return KErrGeneral;

	return KErrNone;
	}


void CWSPEncodeTest::LongIntTestL()
	{
	_LIT(KTestText, "WSP Encode: LongInt");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error,TestLongIntL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}

void CWSPEncodeTest::TestLongIntL()
	{
	HBufC8* output=NULL;
	TBuf8<255> buf;
	TUint32 input;

	const TUint8 KZeroValueLongInt[] = {0x01, 0x00};
	const TUint8 K127LongInt[] = {0x01, 0x7F};
	const TUint8 K255LongInt[] = {0x01, 0xFF};
	const TUint8 K256LongInt[] = {0x02, 0x01, 0x00};
	const TUint8 KFFFFLongInt[] = {0x02, 0xFF, 0xFF};
	const TUint8 K10000LongInt[] = {0x03, 0x01, 0x00, 0x00};
	const TUint8 KFFFFFFLongInt[] = {0x03, 0xFF, 0xFF, 0xFF};
	const TUint8 K1000000LongInt[] = {0x04, 0x01, 0x00, 0x00, 0x00};
	const TUint8 KFFFFFFFFLongInt[] = {0x04, 0xFF, 0xFF, 0xFF, 0xFF};

	input=0;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(KZeroValueLongInt, sizeof(KZeroValueLongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=127;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(K127LongInt, sizeof(K127LongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=255;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(K255LongInt, sizeof(K255LongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=256;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(K256LongInt, sizeof(K256LongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0xFFFF;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(KFFFFLongInt, sizeof(KFFFFLongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x10000;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(K10000LongInt, sizeof(K10000LongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0xFFFFFF;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(KFFFFFFLongInt, sizeof(KFFFFFFLongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x1000000;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(K1000000LongInt, sizeof(K1000000LongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0xFFFFFFFF;
	output = TWspPrimitiveEncoder::LongIntL(input);
	CleanupStack::PushL(output);
	buf.Copy(KFFFFFFFFLongInt, sizeof(KFFFFFFFFLongInt));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);
	}


void CWSPEncodeTest::UintVarTestL()
	{
	_LIT(KTestText, "WSP Encode: UintVar");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error,TestUintVarL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}

void CWSPEncodeTest::TestUintVarL()
	{

	HBufC8* output=NULL;
	TBuf8<255> buf;
	TUint32 input;

	const TUint8 KMinValue[] = {0x00};
	const TUint8 KMaxValue[] = {0x8F, 0xFF, 0xFF, 0xFF, 0x7F};

	const TUint8 K7bitBoundaryLower[] = {0x7F};
	const TUint8 K7bitBoundaryUpper[] = {0x81, 0x00};

	const TUint8 K14bitBoundaryLower[] = {0xFF, 0x7F};
	const TUint8 K14bitBoundaryUpper[] = {0x81, 0x80, 0x00};

	const TUint8 K21bitBoundaryLower[] = {0xFF, 0xFF, 0x7F};
	const TUint8 K21bitBoundaryUpper[] = {0x81, 0x80, 0x80, 0x00};
	const TUint8 K28bitBoundaryLower[] = {0xFF, 0xFF, 0xFF, 0x7F};
	const TUint8 K28bitBoundaryUpper[] = {0x81, 0x80, 0x80, 0x80, 0x00};

	input=0;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(KMinValue, sizeof(KMinValue));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0xFFFFFFFF;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(KMaxValue, sizeof(KMaxValue));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x7F;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(K7bitBoundaryLower, sizeof(K7bitBoundaryLower));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x80;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(K7bitBoundaryUpper, sizeof(K7bitBoundaryUpper));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x3FFF;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(K14bitBoundaryLower, sizeof(K14bitBoundaryLower));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x4000;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(K14bitBoundaryUpper, sizeof(K14bitBoundaryUpper));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x1FFFFF;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(K21bitBoundaryLower, sizeof(K21bitBoundaryLower));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x200000;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(K21bitBoundaryUpper, sizeof(K21bitBoundaryUpper));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0xFFFFFFF;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(K28bitBoundaryLower, sizeof(K28bitBoundaryLower));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	input=0x10000000;
	output = TWspPrimitiveEncoder::UintVarL(input);
	CleanupStack::PushL(output);
	buf.Copy(K28bitBoundaryUpper, sizeof(K28bitBoundaryUpper));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	}


void CWSPEncodeTest::StringTestL()
	{
	_LIT(KTestText, "WSP Encode: String");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error,TestStringL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}

void CWSPEncodeTest::TestStringL()
	{
	TBuf8<255> buf;

	// Test 1. A regular string
	const TUint8 KTestStringResult1[] = {'X','-','n','e','w','-','H','e','a','d','e','r','F','o','o',0x00};
	_LIT8(KTestString1,"X-new-HeaderFoo");

	// Test 2. A quoted string
	const TUint8 KTestStringResult2[] = {'\"','X','-','n','e','w','-','H','e','a','d','e','r','F','o','o',0x00};
	_LIT8(KTestString2,"\"X-new-HeaderFoo\"");

	// Test 3. A string beginning with an Upper-Ascii character
	const TUint8 KTestStringResult3[] = {0x7F,0x80,'X','-','n','e','w','-','H','e','a','d','e','r','F','o','o',0x00};
	const TUint8 KTestString3[] = {0x80,'X','-','n','e','w','-','H','e','a','d','e','r','F','o','o'};

	HBufC8* output = TWspPrimitiveEncoder::TextStringL(KTestString1);
	CleanupStack::PushL(output);
	buf.Copy(KTestStringResult1, sizeof(KTestStringResult1));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);


	output = TWspPrimitiveEncoder::TextStringL(KTestString2);
	CleanupStack::PushL(output);
	buf.Copy(KTestStringResult2, sizeof(KTestStringResult2));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);

	buf.Copy(KTestString3, sizeof(KTestString3));
	output = TWspPrimitiveEncoder::TextStringL(buf);
	CleanupStack::PushL(output);
	buf.Copy(KTestStringResult3, sizeof(KTestStringResult3));
	if ((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);
	}


void CWSPEncodeTest::DateTestL()
	{
	_LIT(KTestText, "WSP Encode: Date");
	iTestHarness->StartTestL(KTestText);
	TRAPD(error,TestDateL());
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}


void CWSPEncodeTest::TestDateL()
	{
	TBuf8<255> buf;

	const TUint8 KDate0[] = {0x01, 0x00};

	TDateTime date(1970,EJanuary,0,0,0,0,0);
	
	HBufC8* output = TWspPrimitiveEncoder::DateL(date);
	CleanupStack::PushL(output);
	buf.Copy(KDate0, sizeof(KDate0));
	if((*output).CompareF(buf)!=0)
		User::Leave(KErrGeneral);
	CleanupStack::PopAndDestroy(output);
	}


