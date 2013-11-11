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

#include "CDelimitedPathSegmentTest.h"

#include <e32base.h>
#include <delimitedpathsegment16.h>
#include <delimitedpathsegment8.h>

#include "TDelimitedDataCommon.h"
#include "DelimitedPathSegmentTests.h"

// Constants
//
_LIT(KTitle16Bit, "%S (16-Bit)");
_LIT(KTitle8Bit, "%S (8-Bit)");

CDelimitedPathSegmentTest* CDelimitedPathSegmentTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CDelimitedPathSegmentTest* self = new (ELeave) CDelimitedPathSegmentTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CDelimitedPathSegmentTest* CDelimitedPathSegmentTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CDelimitedPathSegmentTest* self = CDelimitedPathSegmentTest::NewLC(aTestHarness);
	CleanupStack::Pop(self);
	return self;
	}

CDelimitedPathSegmentTest::CDelimitedPathSegmentTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CDelimitedPathSegmentTest::ConstructL()
	{
	}

CDelimitedPathSegmentTest::~CDelimitedPathSegmentTest()
	{
	}

void CDelimitedPathSegmentTest::DoTestsL()
	{
//
//
// Delimited PathSegment Tests
//
//

	TestDelimitedPathSegmentExtractionL(KPathSegmentParse0, &KPathSegmentParse0Seg0(), &KPathSegmentParse0Seg1(), 
										&KPathSegmentParse0Seg2(), NULL);
	TestDelimitedPathSegmentExtractionL(KPathSegmentParse1, &KPathSegmentParse1Seg0(), &KPathSegmentParse1Seg1(), &KPathSegmentParse1Seg2(), 
								&KPathSegmentParse1Seg3(), NULL);
	TestDelimitedPathSegmentExtractionL(KPathSegmentParse2, &KPathSegmentParse2Seg0(), &KPathSegmentParse2Seg1(), &KPathSegmentParse2Seg2(), 
							   &KPathSegmentParse2Seg3(), &KPathSegmentParse2Seg4(), &KPathSegmentParse2Seg5(), &KPathSegmentParse2Seg6(), 
							   &KPathSegmentParse2Seg7(), NULL);

	TestDelimitedPathSegmentReverseExtractionL(KPathSegmentParse0, &KPathSegmentParse0Seg2(),
							   &KPathSegmentParse0Seg1(), &KPathSegmentParse0Seg0(), NULL);
	TestDelimitedPathSegmentReverseExtractionL(KPathSegmentParse1, &KPathSegmentParse1Seg3(), &KPathSegmentParse1Seg2(), 
								&KPathSegmentParse1Seg1(), &KPathSegmentParse1Seg0(), NULL);
	TestDelimitedPathSegmentReverseExtractionL(KPathSegmentParse2, &KPathSegmentParse2Seg7(), &KPathSegmentParse2Seg6(), 
								&KPathSegmentParse2Seg5(), &KPathSegmentParse2Seg4(), &KPathSegmentParse2Seg3(), 
								&KPathSegmentParse2Seg2(), &KPathSegmentParse2Seg1(), &KPathSegmentParse2Seg0(), NULL);

	TestDelimitedPathSegmentDelimiterPresenceL(KPathSegment_DelimPresence0, KPathSegment_FrontDelim0, KPathSegment_BackDelim0);
	TestDelimitedPathSegmentDelimiterPresenceL(KPathSegment_DelimPresence1, KPathSegment_FrontDelim1, KPathSegment_BackDelim1);
	TestDelimitedPathSegmentDelimiterPresenceL(KPathSegment_DelimPresence2, KPathSegment_FrontDelim2, KPathSegment_BackDelim2);
	TestDelimitedPathSegmentDelimiterPresenceL(KPathSegment_DelimPresence3, KPathSegment_FrontDelim3, KPathSegment_BackDelim3);

	TestDelimitedPathSegmentRemainderL(KPathSegment_Rem_Forwards0_0, &KPathSegment_Rem_Forwards0_0(), &KPathSegment_Rem_Forwards0_1(), 
								&KPathSegment_Rem_Forwards0_2(), &KPathSegment_Rem_Forwards0_3(), NULL);

	TestDelimitedPathSegmentRemainderReverseL(KPathSegment_Rem_Backwards0_0, &KPathSegment_Rem_Backwards0_0(), &KPathSegment_Rem_Backwards0_1(), 
								&KPathSegment_Rem_Backwards0_2(), &KPathSegment_Rem_Backwards0_3(), NULL);

	TestDelimitedPathSegmentDesL(KPathSegment_Des0);

	TestDelimitedPathSegmentPushFrontL(KPathSegment_PushPopFront0_0, &KPathSegment_Push0_a(), &KPathSegment_PushPopFront0_1(), 
								&KPathSegment_Push0_b(), &KPathSegment_PushPopFront0_2(), 
								&KPathSegment_Push0_c(), &KPathSegment_PushPopFront0_3(), 
								&KPathSegment_Push0_d(), &KPathSegment_PushPopFront0_4(), 
								&KPathSegment_Push0_e(), &KPathSegment_PushPopFront0_5(), NULL, NULL);

	TestDelimitedPathSegmentPushBackL(KPathSegment_PushPopBack0_0, &KPathSegment_Push0_a(), &KPathSegment_PushPopBack0_1(), 
								&KPathSegment_Push0_b(), &KPathSegment_PushPopBack0_2(), 
								&KPathSegment_Push0_c(), &KPathSegment_PushPopBack0_3(), 
								&KPathSegment_Push0_d(), &KPathSegment_PushPopBack0_4(), 
								&KPathSegment_Push0_e(), &KPathSegment_PushPopBack0_5(), NULL, NULL);

	TestDelimitedPathSegmentPopFrontL(KPathSegment_PushPopFront0_5, &KPathSegment_PushPopFront0_4(), &KPathSegment_PushPopFront0_3(), 
								&KPathSegment_PushPopFront0_2(), &KPathSegment_PushPopFront0_1(), &KPathSegment_PushPopFront0_0(), NULL);

	TestDelimitedPathSegmentPopBackL(KPathSegment_PushPopBack0_5, &KPathSegment_PushPopBack0_4(), &KPathSegment_PushPopBack0_3(), 
								&KPathSegment_PushPopBack0_2(), &KPathSegment_PushPopBack0_1(), &KPathSegment_PushPopBack0_0(), NULL);

	TestDelimitedPathSegmentAddAndTrimFrontAndBackDelimiterL(KPathSegment_AddTrimDelim0);

	TestDelimitedPathSegmentInsertAndParseL(KPathSegmentInsertForwardsPos0, KPathSegment_InsertForwards0_0, 
									&KPathSegment_Insert0_a(), &KPathSegment_InsertForwards0_1(), 
									&KPathSegment_Insert0_b(), &KPathSegment_InsertForwards0_2(), 
									&KPathSegment_Insert0_c(), &KPathSegment_InsertForwards0_3(), 
									&KPathSegment_Insert0_d(), &KPathSegment_InsertForwards0_4(), 
									&KPathSegment_Insert0_e(), &KPathSegment_InsertForwards0_5(), NULL, NULL);

	TestDelimitedPathSegmentRemoveAndParseL(KPathSegmentRemoveForwardsPos0, KPathSegment_RemoveForwards0_0, 
									&KPathSegment_RemoveForwards0_1(), &KPathSegment_RemoveForwards0_2(), 
									&KPathSegment_RemoveForwards0_3(), &KPathSegment_RemoveForwards0_4(), 
									&KPathSegment_RemoveForwards0_5(), NULL);

	TestDelimitedPathSegmentInsertAndParseReverseL(KPathSegmentInsertBackwardsPos0, KPathSegment_InsertBackwards0_0, 
											&KPathSegment_Insert0_a(), &KPathSegment_InsertBackwards0_1(), 
											&KPathSegment_Insert0_b(), &KPathSegment_InsertBackwards0_2(), 
											&KPathSegment_Insert0_c(), &KPathSegment_InsertBackwards0_3(), 
											&KPathSegment_Insert0_d(), &KPathSegment_InsertBackwards0_4(), 
											&KPathSegment_Insert0_e(), &KPathSegment_InsertBackwards0_5(), NULL, NULL);

	TestDelimitedPathSegmentRemoveAndParseReverseL(KPathSegmentRemoveBackwardsPos0, KPathSegment_RemoveBackwards0_0, 
											&KPathSegment_RemoveBackwards0_1(), &KPathSegment_RemoveBackwards0_2(), 
											&KPathSegment_RemoveBackwards0_3(), &KPathSegment_RemoveBackwards0_4(), 
											&KPathSegment_RemoveBackwards0_5(), NULL);

	TestDelimitedPathSegmentInsertEscapeAndParseL(KPathSegmentInsertEscapeForwardsPos0, KPathSegment_InsertEscapeForwards0_0, 
									&KPathSegment_InsertEscape0_a(), &KPathSegment_InsertEscapeForwards0_1(), 
									&KPathSegment_InsertEscape0_b(), &KPathSegment_InsertEscapeForwards0_2(), 
									&KPathSegment_InsertEscape0_c(), &KPathSegment_InsertEscapeForwards0_3(), 
									&KPathSegment_InsertEscape0_d(), &KPathSegment_InsertEscapeForwards0_4(), 
									&KPathSegment_InsertEscape0_e(), &KPathSegment_InsertEscapeForwards0_5(), NULL, NULL);

	TestDelimitedPathSegmentInsertEscapeAndParseReverseL(KPathSegmentInsertEscapeBackwardsPos0, KPathSegment_InsertEscapeBackwards0_0, 
											&KPathSegment_InsertEscape0_a(), &KPathSegment_InsertEscapeBackwards0_1(), 
											&KPathSegment_InsertEscape0_b(), &KPathSegment_InsertEscapeBackwards0_2(), 
											&KPathSegment_InsertEscape0_c(), &KPathSegment_InsertEscapeBackwards0_3(), 
											&KPathSegment_InsertEscape0_d(), &KPathSegment_InsertEscapeBackwards0_4(), 
											&KPathSegment_InsertEscape0_e(), &KPathSegment_InsertEscapeBackwards0_5(), NULL, NULL);

	TestDelimitedPathSegmentPushAndEscapeFrontL(KPathSegment_PushEscapeFront0_0, &KPathSegment_PushEscape0_a(), &KPathSegment_PushEscapeFront0_1(), 
										&KPathSegment_PushEscape0_b(), &KPathSegment_PushEscapeFront0_2(), 
										&KPathSegment_PushEscape0_c(), &KPathSegment_PushEscapeFront0_3(), 
										&KPathSegment_PushEscape0_d(), &KPathSegment_PushEscapeFront0_4(), 
										&KPathSegment_PushEscape0_e(), &KPathSegment_PushEscapeFront0_5(), NULL, NULL);

	TestDelimitedPathSegmentPushAndEscapeBackL(KPathSegment_PushEscapeBack0_0, &KPathSegment_PushEscape0_a(), &KPathSegment_PushEscapeBack0_1(), 
										&KPathSegment_PushEscape0_b(), &KPathSegment_PushEscapeBack0_2(), 
										&KPathSegment_PushEscape0_c(), &KPathSegment_PushEscapeBack0_3(), 
										&KPathSegment_PushEscape0_d(), &KPathSegment_PushEscapeBack0_4(), 
										&KPathSegment_PushEscape0_e(), &KPathSegment_PushEscapeBack0_5(), NULL, NULL);
	}

//
//
//	Delimited PathSegment tests
//
//

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentExtractionL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataExtractionL(_L("Test TDelimitedPathSegmentParser Parse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentReverseExtractionL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataReverseExtractionL(_L("Test TDelimitedPathSegmentParser ParseReverse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentDelimiterPresenceL(const TDesC& aPath, TBool aFrontDelim, TBool aBackDelim) const
	{
	TestDelimitedDataDelimiterPresenceL(_L("Test TDelimitedPathSegmentParser Delimiter Presence"), iTestHarness, aPath, aFrontDelim, aBackDelim);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentRemainderL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataRemainderL(_L("Test TDelimitedPathSegmentParser Remainder, Parse"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentRemainderReverseL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataRemainderReverseL(_L("Test TDelimitedPathSegmentParser Remainder, ParseReverse"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentDesL(const TDesC& aPath) const
	{
	TestDelimitedDataDesL(_L("Test TDelimitedPathSegmentParser Des"), iTestHarness, aPath);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentPushFrontL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPushFrontL(_L("Test CDelimitedPathSegment PushFront"), iTestHarness, path, list1, list2);
	VA_END(list1);
	VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentPushBackL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPushBackL(_L("Test CDelimitedPathSegment PushBack"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentPopFrontL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPopFrontL(_L("Test CDelimitedPathSegment PopFront"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentPopBackL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPopBackL(_L("Test CDelimitedPathSegment PopBack"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentAddAndTrimFrontAndBackDelimiterL(const TDesC& aPath) const
	{
	TestDelimitedDataAddAndTrimFrontAndBackDelimiterL(_L("Test CDelimitedPathSegment Add and Trim Front and Back delimiter"), iTestHarness, aPath);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentInsertAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataInsertAndParseL(_L("Test CDelimitedPathSegment InsertCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentRemoveAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataRemoveAndParseL(_L("Test CDelimitedPathSegment RemoveCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentInsertAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataInsertAndParseReverseL(_L("Test CDelimitedPathSegment InsertCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentRemoveAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataRemoveAndParseReverseL(_L("Test CDelimitedPathSegment RemoveCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentInsertEscapeAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataInsertEscapeAndParseL(_L("Test CDelimitedPathSegment InsertAndEscapeCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentInsertEscapeAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataInsertEscapeAndParseReverseL(_L("Test CDelimitedPathSegment InsertAndEscapeCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentPushAndEscapeFrontL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPushAndEscapeFrontL(_L("Test CDelimitedPathSegment PushAndEscapeFront"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathSegmentTest::TestDelimitedPathSegmentPushAndEscapeBackL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPushAndEscapeBackL(_L("Test CDelimitedPathSegment PushAndEscapeBack"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}
//
//
//	Implementation of LOCAL functions - test funcions
//
//

void TestDelimitedDataExtractionL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										  const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	TDelimitedPathSegmentParser16 parser16Bit;
	parser16Bit.Parse(aData);

	TInt error = DoDelimitedDataParsingL<HBufC16, TPtrC16>(parser16Bit, aList1);

	CleanupStack::PopAndDestroy(title16Bit);	// title16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	TDelimitedPathSegmentParser8 parser8Bit;
	parser8Bit.Parse(data8Bit);

	error = DoDelimitedDataParsingL<HBufC8, TPtrC8>(parser8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(2, dataBuf);	// dataBuf, title8Bit
	}

void TestDelimitedDataReverseExtractionL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	TDelimitedPathSegmentParser16 parser16Bit;
	parser16Bit.ParseReverse(aData);

	TInt error = DoDelimitedDataParsingL<HBufC16, TPtrC16>(parser16Bit, aList1);

	CleanupStack::PopAndDestroy(title16Bit);	// title16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	TDelimitedPathSegmentParser8 parser8Bit;
	parser8Bit.ParseReverse(data8Bit);

	error = DoDelimitedDataParsingL<HBufC8, TPtrC8>(parser8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(2, dataBuf);	// dataBuf, title8Bit
	}

void TestDelimitedDataDelimiterPresenceL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												 const TDesC& aData, TBool aFrontDelim, TBool aBackDelim)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object 
	TDelimitedPathSegmentParser16 parser16Bit;
	parser16Bit.Parse(aData);
	TInt error = DoDelimiterPresenceTest(parser16Bit, aFrontDelim, aBackDelim);

	CleanupStack::PopAndDestroy(title16Bit);	// title16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object 
	TDelimitedPathSegmentParser8 parser8Bit;
	parser8Bit.Parse(data8Bit);
	error = DoDelimiterPresenceTest(parser8Bit, aFrontDelim, aBackDelim);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(2, dataBuf);	// dataBuf, title8Bit
	}

void TestDelimitedDataRemainderL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimiteddataParser object and do parsing
	TDelimitedPathSegmentParser16 parser16Bit;
	parser16Bit.Parse(aData);

	TInt error = DoDelimitedDataRemainderTestL<HBufC16, TPtrC16>(parser16Bit, aList1);

	CleanupStack::PopAndDestroy(title16Bit);	// title16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimiteddataParser object and do parsing
	TDelimitedPathSegmentParser8 parser8Bit;
	parser8Bit.Parse(data8Bit);

	error = DoDelimitedDataRemainderTestL<HBufC8, TPtrC8>(parser8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(2, dataBuf);	// dataBuf, title8Bit
	}

void TestDelimitedDataRemainderReverseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	TDelimitedPathSegmentParser16 parser16Bit;
	parser16Bit.ParseReverse(aData);

	TInt error = DoDelimitedDataRemainderTestL<HBufC16, TPtrC16>(parser16Bit, aList1);

	CleanupStack::PopAndDestroy(title16Bit);	// title16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	TDelimitedPathSegmentParser8 parser8Bit;
	parser8Bit.ParseReverse(data8Bit);

	error = DoDelimitedDataRemainderTestL<HBufC8, TPtrC8>(parser8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(2, dataBuf);	// dataBuf, title8Bit
	}

void TestDelimitedDataDesL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, const TDesC& aData)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	TDelimitedPathSegmentParser16 parser16Bit;
	parser16Bit.Parse(aData);

	TInt error = DoDelimitedDataDes(parser16Bit, aData);

	CleanupStack::PopAndDestroy(title16Bit);	// title16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 data8Bit = dataBuf->Des();
	data8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	TDelimitedPathSegmentParser8 parser8Bit;
	parser8Bit.Parse(data8Bit);

	error = DoDelimitedDataDes(parser8Bit, data8Bit);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(2, dataBuf);	// dataBuf, title8Bit
	}

void TestDelimitedDataPushFrontL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	TInt error = DoDelimitedDataPushFrontL<HBufC16>(data16Bit, aList1);

	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	error = DoDelimitedDataPushFrontL<HBufC8>(data8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataPushAndEscapeFrontL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												  const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	TInt error = DoDelimitedDataPushAndEscapeFrontL<HBufC16>(data16Bit, aList1);

	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	error = DoDelimitedDataPushAndEscapeFrontL<HBufC8>(data8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataPushBackL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	TInt error = DoDelimitedDataPushBackL<HBufC16>(data16Bit, aList1);

	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	error = DoDelimitedDataPushBackL<HBufC8>(data8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataPushAndEscapeBackL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	TInt error = DoDelimitedDataPushAndEscapeBackL<HBufC16>(data16Bit, aList1);

	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	error = DoDelimitedDataPushAndEscapeBackL<HBufC8>(data8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataPopFrontL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	TInt error = DoDelimitedDataPopFrontL<HBufC16>(data16Bit, aList1);

	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	error = DoDelimitedDataPopFrontL<HBufC8>(data8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataPopBackL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
									   const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	TInt error = DoDelimitedDataPopBackL<HBufC16>(data16Bit, aList1);

	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	error = DoDelimitedDataPopBackL<HBufC8>(data8Bit, aList2);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataAddAndTrimFrontAndBackDelimiterL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, const TDesC& aData)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	TInt error = DoDelimitedDataAddAndTrimFrontAndBackDelimiterL(data16Bit);

	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	error = DoDelimitedDataAddAndTrimFrontAndBackDelimiterL(data8Bit);

	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataInsertAndParseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
											  const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	// Do initial parse and get to position
	data16Bit->Parse();
	TInt error = DoDelimitedDataParseToPosition(data16Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataInsertL<HBufC16, TPtrC16>(data16Bit, aList1);
		}
	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	// Do initial parse and get to position
	data8Bit->Parse();
	error = DoDelimitedDataParseToPosition(data8Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataInsertL<HBufC8, TPtrC8>(data8Bit, aList2);
		}
	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataRemoveAndParseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
											  const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	// Do initial parse and get to position
	data16Bit->Parse();
	TInt error = DoDelimitedDataParseToPosition(data16Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataRemoveL<HBufC16, TPtrC16>(data16Bit, aList1);
		}
	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	// Do initial parse and get to position
	data8Bit->Parse();
	error = DoDelimitedDataParseToPosition(data8Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataRemoveL<HBufC8, TPtrC8>(data8Bit, aList2);
		}
	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataInsertAndParseReverseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
													 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	// Do initial parse and get to position
	data16Bit->ParseReverse();
	TInt error = DoDelimitedDataParseToPosition(data16Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataInsertL<HBufC16, TPtrC16>(data16Bit, aList1);
		}
	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	// Do initial parse and get to position
	data8Bit->ParseReverse();
	error = DoDelimitedDataParseToPosition(data8Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataInsertL<HBufC8, TPtrC8>(data8Bit, aList2);
		}
	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataRemoveAndParseReverseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
													 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	// Do initial parse and get to position
	data16Bit->ParseReverse();
	TInt error = DoDelimitedDataParseToPosition(data16Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataRemoveL<HBufC16, TPtrC16>(data16Bit, aList1);
		}
	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	// Do initial parse and get to position
	data8Bit->ParseReverse();
	error = DoDelimitedDataParseToPosition(data8Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataRemoveL<HBufC8, TPtrC8>(data8Bit, aList2);
		}
	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataInsertEscapeAndParseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
													const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	// Do initial parse and get to position
	data16Bit->Parse();
	TInt error = DoDelimitedDataParseToPosition(data16Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataInsertAndEscapeL<HBufC16, TPtrC16>(data16Bit, aList1);
		}
	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	// Do initial parse and get to position
	data8Bit->Parse();
	error = DoDelimitedDataParseToPosition(data8Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataInsertAndEscapeL<HBufC8, TPtrC8>(data8Bit, aList2);
		}
	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

void TestDelimitedDataInsertEscapeAndParseReverseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
														   const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos)
	{
	// Create title
	HBufC* title16Bit = HBufC::NewLC(aTestTitle.Length() + KTitle16Bit().Length());
	title16Bit->Des().Format(KTitle16Bit, &aTestTitle);
	aTestHarness->StartTestL(*title16Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment16* data16Bit = CDelimitedPathSegment16::NewL(aData);	// Also excersies NewLC()
	CleanupStack::PushL(data16Bit);

	// Do initial parse and get to position
	data16Bit->ParseReverse();
	TInt error = DoDelimitedDataParseToPosition(data16Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataInsertAndEscapeL<HBufC16, TPtrC16>(data16Bit, aList1);
		}
	CleanupStack::PopAndDestroy(2, title16Bit);	// title16Bit, data16Bit
	aTestHarness->EndTest(error);

	// Make 8-bit copy
	HBufC8* dataBuf = HBufC8::NewLC(aData.Length());
	TPtr8 initData8Bit = dataBuf->Des();
	initData8Bit.Copy(aData);

	// Create title
	HBufC* title8Bit = HBufC::NewLC(aTestTitle.Length() + KTitle8Bit().Length());
	title8Bit->Des().Format(KTitle8Bit, &aTestTitle);
	aTestHarness->StartTestL(*title8Bit);

	// Create TDelimitedPathSegmentParser object and do parsing
	CDelimitedPathSegment8* data8Bit = CDelimitedPathSegment8::NewL(initData8Bit);	// Also excersies NewLC()
	CleanupStack::PushL(data8Bit);

	// Do initial parse and get to position
	data8Bit->ParseReverse();
	error = DoDelimitedDataParseToPosition(data8Bit, aStartPos);
	if( error == KErrNone )
		{
		error = DoDelimitedDataInsertAndEscapeL<HBufC8, TPtrC8>(data8Bit, aList2);
		}
	aTestHarness->EndTest(error);
	CleanupStack::PopAndDestroy(3, dataBuf);	// dataBuf, title8Bit, data8Bit
	}

//
//
//	Implementation of LOCAL functions - helper functions
//
//

#include "TDelimitedDataCommon.cpp"

