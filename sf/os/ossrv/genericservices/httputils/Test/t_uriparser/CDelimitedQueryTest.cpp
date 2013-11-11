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

#include "CDelimitedQueryTest.h"

#include <e32base.h>
#include <delimitedquery16.h>
#include <delimitedquery8.h>

#include "TDelimitedDataCommon.h"
#include "DelimitedQueryTests.h"

// Constants
//
_LIT(KTitle16Bit, "%S (16-Bit)");
_LIT(KTitle8Bit, "%S (8-Bit)");

CDelimitedQueryTest* CDelimitedQueryTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CDelimitedQueryTest* self = new (ELeave) CDelimitedQueryTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CDelimitedQueryTest* CDelimitedQueryTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CDelimitedQueryTest* self = CDelimitedQueryTest::NewLC(aTestHarness);
	CleanupStack::Pop(self);
	return self;
	}

CDelimitedQueryTest::CDelimitedQueryTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CDelimitedQueryTest::ConstructL()
	{
	}

CDelimitedQueryTest::~CDelimitedQueryTest()
	{
	}

void CDelimitedQueryTest::DoTestsL()
	{
//
//
// Delimited Query Tests
//
//

	TestDelimitedQueryExtractionL(KQueryParse0, &KQueryParse0Seg0(), &KQueryParse0Seg1(), &KQueryParse0Seg2(),
							   &KQueryParse0Seg3(), NULL);
	TestDelimitedQueryExtractionL(KQueryParse1, &KQueryParse1Seg0(), &KQueryParse1Seg1(), NULL);
	TestDelimitedQueryExtractionL(KQueryParse2, &KQueryParse2Seg0(), &KQueryParse2Seg1(), &KQueryParse2Seg2(), 
							   &KQueryParse2Seg3(), &KQueryParse2Seg4(), &KQueryParse2Seg5(), NULL);

	TestDelimitedQueryReverseExtractionL(KQueryParse0, &KQueryParse0Seg3(), &KQueryParse0Seg2(),
							   &KQueryParse0Seg1(), &KQueryParse0Seg0(), NULL);
	TestDelimitedQueryReverseExtractionL(KQueryParse1, &KQueryParse1Seg1(), &KQueryParse1Seg0(), NULL);
	TestDelimitedQueryReverseExtractionL(KQueryParse2, &KQueryParse2Seg5(), &KQueryParse2Seg4(), 
							   &KQueryParse2Seg3(), &KQueryParse2Seg2(), &KQueryParse2Seg1(), &KQueryParse2Seg0(), NULL);

	TestDelimitedQueryDelimiterPresenceL(KQuery_DelimPresence0, KQuery_FrontDelim0, KQuery_BackDelim0);
	TestDelimitedQueryDelimiterPresenceL(KQuery_DelimPresence1, KQuery_FrontDelim1, KQuery_BackDelim1);
	TestDelimitedQueryDelimiterPresenceL(KQuery_DelimPresence2, KQuery_FrontDelim2, KQuery_BackDelim2);
	TestDelimitedQueryDelimiterPresenceL(KQuery_DelimPresence3, KQuery_FrontDelim3, KQuery_BackDelim3);

	TestDelimitedQueryRemainderL(KQuery_Rem_Forwards0_0, &KQuery_Rem_Forwards0_0(), &KQuery_Rem_Forwards0_1(), 
								&KQuery_Rem_Forwards0_2(), &KQuery_Rem_Forwards0_3(), NULL);

	TestDelimitedQueryRemainderReverseL(KQuery_Rem_Backwards0_0, &KQuery_Rem_Backwards0_0(), &KQuery_Rem_Backwards0_1(), 
								&KQuery_Rem_Backwards0_2(), &KQuery_Rem_Backwards0_3(), NULL);

	TestDelimitedQueryDesL(KQuery_Des0);

	TestDelimitedQueryPushFrontL(KQuery_PushPopFront0_0, &KQuery_Push0_a(), &KQuery_PushPopFront0_1(), 
								&KQuery_Push0_b(), &KQuery_PushPopFront0_2(), 
								&KQuery_Push0_c(), &KQuery_PushPopFront0_3(), 
								&KQuery_Push0_d(), &KQuery_PushPopFront0_4(), 
								&KQuery_Push0_e(), &KQuery_PushPopFront0_5(), NULL, NULL);

	TestDelimitedQueryPushBackL(KQuery_PushPopBack0_0, &KQuery_Push0_a(), &KQuery_PushPopBack0_1(), 
								&KQuery_Push0_b(), &KQuery_PushPopBack0_2(), 
								&KQuery_Push0_c(), &KQuery_PushPopBack0_3(), 
								&KQuery_Push0_d(), &KQuery_PushPopBack0_4(), 
								&KQuery_Push0_e(), &KQuery_PushPopBack0_5(), NULL, NULL);

	TestDelimitedQueryPopFrontL(KQuery_PushPopFront0_5, &KQuery_PushPopFront0_4(), &KQuery_PushPopFront0_3(), 
								&KQuery_PushPopFront0_2(), &KQuery_PushPopFront0_1(), &KQuery_PushPopFront0_0(), NULL);

	TestDelimitedQueryPopBackL(KQuery_PushPopBack0_5, &KQuery_PushPopBack0_4(), &KQuery_PushPopBack0_3(), 
								&KQuery_PushPopBack0_2(), &KQuery_PushPopBack0_1(), &KQuery_PushPopBack0_0(), NULL);

	TestDelimitedQueryAddAndTrimFrontAndBackDelimiterL(KQuery_AddTrimDelim0);

	TestDelimitedQueryInsertAndParseL(KQueryInsertForwardsPos0, KQuery_InsertForwards0_0, 
									&KQuery_Insert0_a(), &KQuery_InsertForwards0_1(), 
									&KQuery_Insert0_b(), &KQuery_InsertForwards0_2(), 
									&KQuery_Insert0_c(), &KQuery_InsertForwards0_3(), 
									&KQuery_Insert0_d(), &KQuery_InsertForwards0_4(), 
									&KQuery_Insert0_e(), &KQuery_InsertForwards0_5(), NULL, NULL);

	TestDelimitedQueryRemoveAndParseL(KQueryRemoveForwardsPos0, KQuery_RemoveForwards0_0, 
									&KQuery_RemoveForwards0_1(), &KQuery_RemoveForwards0_2(), 
									&KQuery_RemoveForwards0_3(), &KQuery_RemoveForwards0_4(), 
									&KQuery_RemoveForwards0_5(), NULL);

	TestDelimitedQueryInsertAndParseReverseL(KQueryInsertBackwardsPos0, KQuery_InsertBackwards0_0, 
											&KQuery_Insert0_a(), &KQuery_InsertBackwards0_1(), 
											&KQuery_Insert0_b(), &KQuery_InsertBackwards0_2(), 
											&KQuery_Insert0_c(), &KQuery_InsertBackwards0_3(), 
											&KQuery_Insert0_d(), &KQuery_InsertBackwards0_4(), 
											&KQuery_Insert0_e(), &KQuery_InsertBackwards0_5(), NULL, NULL);

	TestDelimitedQueryRemoveAndParseReverseL(KQueryRemoveBackwardsPos0, KQuery_RemoveBackwards0_0, 
											&KQuery_RemoveBackwards0_1(), &KQuery_RemoveBackwards0_2(), 
											&KQuery_RemoveBackwards0_3(), &KQuery_RemoveBackwards0_4(), 
											&KQuery_RemoveBackwards0_5(), NULL);

	TestDelimitedQueryInsertEscapeAndParseL(KQueryInsertEscapeForwardsPos0, KQuery_InsertEscapeForwards0_0, 
									&KQuery_InsertEscape0_a(), &KQuery_InsertEscapeForwards0_1(), 
									&KQuery_InsertEscape0_b(), &KQuery_InsertEscapeForwards0_2(), 
									&KQuery_InsertEscape0_c(), &KQuery_InsertEscapeForwards0_3(), 
									&KQuery_InsertEscape0_d(), &KQuery_InsertEscapeForwards0_4(), 
									&KQuery_InsertEscape0_e(), &KQuery_InsertEscapeForwards0_5(), NULL, NULL);

	TestDelimitedQueryInsertEscapeAndParseReverseL(KQueryInsertEscapeBackwardsPos0, KQuery_InsertEscapeBackwards0_0, 
											&KQuery_InsertEscape0_a(), &KQuery_InsertEscapeBackwards0_1(), 
											&KQuery_InsertEscape0_b(), &KQuery_InsertEscapeBackwards0_2(), 
											&KQuery_InsertEscape0_c(), &KQuery_InsertEscapeBackwards0_3(), 
											&KQuery_InsertEscape0_d(), &KQuery_InsertEscapeBackwards0_4(), 
											&KQuery_InsertEscape0_e(), &KQuery_InsertEscapeBackwards0_5(), NULL, NULL);

	TestDelimitedQueryPushAndEscapeFrontL(KQuery_PushEscapeFront0_0, &KQuery_PushEscape0_a(), &KQuery_PushEscapeFront0_1(), 
										&KQuery_PushEscape0_b(), &KQuery_PushEscapeFront0_2(), 
										&KQuery_PushEscape0_c(), &KQuery_PushEscapeFront0_3(), 
										&KQuery_PushEscape0_d(), &KQuery_PushEscapeFront0_4(), 
										&KQuery_PushEscape0_e(), &KQuery_PushEscapeFront0_5(), NULL, NULL);

	TestDelimitedQueryPushAndEscapeBackL(KQuery_PushEscapeBack0_0, &KQuery_PushEscape0_a(), &KQuery_PushEscapeBack0_1(), 
										&KQuery_PushEscape0_b(), &KQuery_PushEscapeBack0_2(), 
										&KQuery_PushEscape0_c(), &KQuery_PushEscapeBack0_3(), 
										&KQuery_PushEscape0_d(), &KQuery_PushEscapeBack0_4(), 
										&KQuery_PushEscape0_e(), &KQuery_PushEscapeBack0_5(), NULL, NULL);
	}

//
//
//	Delimited Query tests
//
//

void CDelimitedQueryTest::TestDelimitedQueryExtractionL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataExtractionL(_L("Test TDelimitedQueryParser Parse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryReverseExtractionL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataReverseExtractionL(_L("Test TDelimitedQueryParser ParseReverse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryDelimiterPresenceL(const TDesC& aQuery, TBool aFrontDelim, TBool aBackDelim) const
	{
	TestDelimitedDataDelimiterPresenceL(_L("Test TDelimitedQueryParser Delimiter Presence"), iTestHarness, aQuery, aFrontDelim, aBackDelim);
	}

void CDelimitedQueryTest::TestDelimitedQueryRemainderL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataRemainderL(_L("Test TDelimitedQueryParser Remainder, Parse"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryRemainderReverseL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataRemainderReverseL(_L("Test TDelimitedQueryParser Remainder, ParseReverse"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryDesL(const TDesC& aQuery) const
	{
	TestDelimitedDataDesL(_L("Test TDelimitedQueryParser Des"), iTestHarness, aQuery);
	}

void CDelimitedQueryTest::TestDelimitedQueryPushFrontL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataPushFrontL(_L("Test CDelimitedQuery PushFront"), iTestHarness, path, list1, list2);
	VA_END(list1);
	VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryPushBackL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataPushBackL(_L("Test CDelimitedQuery PushBack"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryPopFrontL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataPopFrontL(_L("Test CDelimitedQuery PopFront"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryPopBackL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataPopBackL(_L("Test CDelimitedQuery PopBack"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryAddAndTrimFrontAndBackDelimiterL(const TDesC& aQuery) const
	{
	TestDelimitedDataAddAndTrimFrontAndBackDelimiterL(_L("Test CDelimitedQuery Add and Trim Front and Back delimiter"), iTestHarness, aQuery);
	}

void CDelimitedQueryTest::TestDelimitedQueryInsertAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataInsertAndParseL(_L("Test CDelimitedQuery InsertCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryRemoveAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataRemoveAndParseL(_L("Test CDelimitedQuery RemoveCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryInsertAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataInsertAndParseReverseL(_L("Test CDelimitedQuery InsertCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryRemoveAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataRemoveAndParseReverseL(_L("Test CDelimitedQuery RemoveCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryInsertEscapeAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataInsertEscapeAndParseL(_L("Test CDelimitedQuery InsertAndEscapeCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryInsertEscapeAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataInsertEscapeAndParseReverseL(_L("Test CDelimitedQuery InsertAndEscapeCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryPushAndEscapeFrontL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataPushAndEscapeFrontL(_L("Test CDelimitedQuery PushAndEscapeFront"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedQueryTest::TestDelimitedQueryPushAndEscapeBackL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& path = aQuery;

	TestDelimitedDataPushAndEscapeBackL(_L("Test CDelimitedQuery PushAndEscapeBack"), iTestHarness, path, list1, list2);
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

	// Create TDelimitedQueryParser object and do parsing
	TDelimitedQueryParser16 parser16Bit;
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

	// Create TDelimitedQueryParser object and do parsing
	TDelimitedQueryParser8 parser8Bit;
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

	// Create TDelimitedQueryParser object and do parsing
	TDelimitedQueryParser16 parser16Bit;
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

	// Create TDelimitedQueryParser object and do parsing
	TDelimitedQueryParser8 parser8Bit;
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

	// Create TDelimitedQueryParser object 
	TDelimitedQueryParser16 parser16Bit;
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

	// Create TDelimitedQueryParser object 
	TDelimitedQueryParser8 parser8Bit;
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
	TDelimitedQueryParser16 parser16Bit;
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
	TDelimitedQueryParser8 parser8Bit;
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

	// Create TDelimitedQueryParser object and do parsing
	TDelimitedQueryParser16 parser16Bit;
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

	// Create TDelimitedQueryParser object and do parsing
	TDelimitedQueryParser8 parser8Bit;
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

	// Create TDelimitedQueryParser object and do parsing
	TDelimitedQueryParser16 parser16Bit;
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

	// Create TDelimitedQueryParser object and do parsing
	TDelimitedQueryParser8 parser8Bit;
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery16* data16Bit = CDelimitedQuery16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedQueryParser object and do parsing
	CDelimitedQuery8* data8Bit = CDelimitedQuery8::NewL(initData8Bit);	// Also excersies NewLC()
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
