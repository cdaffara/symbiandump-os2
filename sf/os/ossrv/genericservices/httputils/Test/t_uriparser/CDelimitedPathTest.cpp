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

#include "CDelimitedPathTest.h"

#include <e32base.h>
#include <delimitedpath16.h>
#include <delimitedpath8.h>

#include "TDelimitedDataCommon.h"
#include "DelimitedPathTests.h"

// Constants
//
_LIT(KTitle16Bit, "%S (16-Bit)");
_LIT(KTitle8Bit, "%S (8-Bit)");

CDelimitedPathTest* CDelimitedPathTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CDelimitedPathTest* self = new (ELeave) CDelimitedPathTest(aTestHarness);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CDelimitedPathTest* CDelimitedPathTest::NewL(CIpuTestHarness* aTestHarness)
	{
	CDelimitedPathTest* self = CDelimitedPathTest::NewLC(aTestHarness);
	CleanupStack::Pop();	// self
	return self;
	}

CDelimitedPathTest::CDelimitedPathTest(CIpuTestHarness* aTestHarness)
: iTestHarness(aTestHarness)
	{
	}

void CDelimitedPathTest::ConstructL()
	{
	}

CDelimitedPathTest::~CDelimitedPathTest()
	{
	}

void CDelimitedPathTest::DoTestsL()
	{
//
//
// Delimited Path Tests
//
//

	TestDelimitedPathExtractionL(KPathParse0, &KPathParse0Seg0(), &KPathParse0Seg1(), &KPathParse0Seg2(),
							   &KPathParse0Seg3(), &KPathParse0Seg4(), NULL);
	TestDelimitedPathExtractionL(KPathParse1, &KPathParse1Seg0(), &KPathParse1Seg1(), NULL);
	TestDelimitedPathExtractionL(KPathParse2, &KPathParse2Seg0(), &KPathParse2Seg1(), &KPathParse2Seg2(), 
							   &KPathParse2Seg3(), &KPathParse2Seg4(), &KPathParse2Seg5(), &KPathParse2Seg6(), NULL);

	TestDelimitedPathReverseExtractionL(KPathParse0, &KPathParse0Seg4(), &KPathParse0Seg3(), &KPathParse0Seg2(),
							   &KPathParse0Seg1(), &KPathParse0Seg0(), NULL);
	TestDelimitedPathReverseExtractionL(KPathParse1, &KPathParse1Seg1(), &KPathParse1Seg0(), NULL);
	TestDelimitedPathReverseExtractionL(KPathParse2, &KPathParse2Seg6(), &KPathParse2Seg5(), &KPathParse2Seg4(), 
							   &KPathParse2Seg3(), &KPathParse2Seg2(), &KPathParse2Seg1(), &KPathParse2Seg0(), NULL);

	TestDelimitedPathDelimiterPresenceL(KPath_DelimPresence0, KPath_FrontDelim0, KPath_BackDelim0);
	TestDelimitedPathDelimiterPresenceL(KPath_DelimPresence1, KPath_FrontDelim1, KPath_BackDelim1);
	TestDelimitedPathDelimiterPresenceL(KPath_DelimPresence2, KPath_FrontDelim2, KPath_BackDelim2);
	TestDelimitedPathDelimiterPresenceL(KPath_DelimPresence3, KPath_FrontDelim3, KPath_BackDelim3);

	TestDelimitedPathRemainderL(KPath_Rem_Forwards0_0, &KPath_Rem_Forwards0_0(), &KPath_Rem_Forwards0_1(), 
								&KPath_Rem_Forwards0_2(), &KPath_Rem_Forwards0_3(), &KPath_Rem_Forwards0_4(), NULL);
	TestDelimitedPathRemainderL(KPath_Rem_Forwards1_0, &KPath_Rem_Forwards1_0(), &KPath_Rem_Forwards1_1(), 
								&KPath_Rem_Forwards1_2(), &KPath_Rem_Forwards1_3(), NULL);
	TestDelimitedPathRemainderL(KPath_Rem_Forwards2_0, &KPath_Rem_Forwards2_0(), &KPath_Rem_Forwards2_1(), 
								&KPath_Rem_Forwards2_2(), &KPath_Rem_Forwards2_3(), NULL);

	TestDelimitedPathRemainderReverseL(KPath_Rem_Backwards0_0, &KPath_Rem_Backwards0_0(), &KPath_Rem_Backwards0_1(), 
								&KPath_Rem_Backwards0_2(), &KPath_Rem_Backwards0_3(), &KPath_Rem_Backwards0_4(), NULL);
	TestDelimitedPathRemainderReverseL(KPath_Rem_Backwards1_0, &KPath_Rem_Backwards1_0(), &KPath_Rem_Backwards1_1(), 
								&KPath_Rem_Backwards1_2(), &KPath_Rem_Backwards1_3(), NULL);
	TestDelimitedPathRemainderReverseL(KPath_Rem_Backwards2_0, &KPath_Rem_Backwards2_0(), &KPath_Rem_Backwards2_1(), 
								&KPath_Rem_Backwards2_2(), &KPath_Rem_Backwards2_3(), NULL);

	TestDelimitedPathDesL(KPath_Des0);

	TestDelimitedPathPushFrontL(KPath_PushPopFront0_0, &KPath_Push0_a(), &KPath_PushPopFront0_1(), 
								&KPath_Push0_b(), &KPath_PushPopFront0_2(), 
								&KPath_Push0_c(), &KPath_PushPopFront0_3(), 
								&KPath_Push0_d(), &KPath_PushPopFront0_4(), 
								&KPath_Push0_e(), &KPath_PushPopFront0_5(), NULL, NULL);

	TestDelimitedPathPushBackL(KPath_PushPopBack0_0, &KPath_Push0_a(), &KPath_PushPopBack0_1(), 
								&KPath_Push0_b(), &KPath_PushPopBack0_2(), 
								&KPath_Push0_c(), &KPath_PushPopBack0_3(), 
								&KPath_Push0_d(), &KPath_PushPopBack0_4(), 
								&KPath_Push0_e(), &KPath_PushPopBack0_5(), NULL, NULL);

	TestDelimitedPathPopFrontL(KPath_PushPopFront0_5, &KPath_PushPopFront0_4(), &KPath_PushPopFront0_3(), 
								&KPath_PushPopFront0_2(), &KPath_PushPopFront0_1(), &KPath_PushPopFront0_0(), NULL);

	TestDelimitedPathPopBackL(KPath_PushPopBack0_5, &KPath_PushPopBack0_4(), &KPath_PushPopBack0_3(), 
								&KPath_PushPopBack0_2(), &KPath_PushPopBack0_1(), &KPath_PushPopBack0_0(), NULL);

	TestDelimitedPathAddAndTrimFrontAndBackDelimiterL(KPath_AddTrimDelim0);

	TestDelimitedPathInsertAndParseL(KPathInsertForwardsPos0, KPath_InsertForwards0_0, 
									&KPath_Insert0_a(), &KPath_InsertForwards0_1(), 
									&KPath_Insert0_b(), &KPath_InsertForwards0_2(), 
									&KPath_Insert0_c(), &KPath_InsertForwards0_3(), 
									&KPath_Insert0_d(), &KPath_InsertForwards0_4(), 
									&KPath_Insert0_e(), &KPath_InsertForwards0_5(), NULL, NULL);

	TestDelimitedPathRemoveAndParseL(KPathRemoveForwardsPos0, KPath_RemoveForwards0_0, 
									&KPath_RemoveForwards0_1(), &KPath_RemoveForwards0_2(), 
									&KPath_RemoveForwards0_3(), &KPath_RemoveForwards0_4(), 
									&KPath_RemoveForwards0_5(), NULL);

	TestDelimitedPathInsertAndParseReverseL(KPathInsertBackwardsPos0, KPath_InsertBackwards0_0, 
											&KPath_Insert0_a(), &KPath_InsertBackwards0_1(), 
											&KPath_Insert0_b(), &KPath_InsertBackwards0_2(), 
											&KPath_Insert0_c(), &KPath_InsertBackwards0_3(), 
											&KPath_Insert0_d(), &KPath_InsertBackwards0_4(), 
											&KPath_Insert0_e(), &KPath_InsertBackwards0_5(), NULL, NULL);

	TestDelimitedPathRemoveAndParseReverseL(KPathRemoveBackwardsPos0, KPath_RemoveBackwards0_0, 
											&KPath_RemoveBackwards0_1(), &KPath_RemoveBackwards0_2(), 
											&KPath_RemoveBackwards0_3(), &KPath_RemoveBackwards0_4(), 
											&KPath_RemoveBackwards0_5(), NULL);

	TestDelimitedPathInsertEscapeAndParseL(KPathInsertEscapeForwardsPos0, KPath_InsertEscapeForwards0_0, 
									&KPath_InsertEscape0_a(), &KPath_InsertEscapeForwards0_1(), 
									&KPath_InsertEscape0_b(), &KPath_InsertEscapeForwards0_2(), 
									&KPath_InsertEscape0_c(), &KPath_InsertEscapeForwards0_3(), 
									&KPath_InsertEscape0_d(), &KPath_InsertEscapeForwards0_4(), 
									&KPath_InsertEscape0_e(), &KPath_InsertEscapeForwards0_5(), NULL, NULL);

	TestDelimitedPathInsertEscapeAndParseReverseL(KPathInsertEscapeBackwardsPos0, KPath_InsertEscapeBackwards0_0, 
											&KPath_InsertEscape0_a(), &KPath_InsertEscapeBackwards0_1(), 
											&KPath_InsertEscape0_b(), &KPath_InsertEscapeBackwards0_2(), 
											&KPath_InsertEscape0_c(), &KPath_InsertEscapeBackwards0_3(), 
											&KPath_InsertEscape0_d(), &KPath_InsertEscapeBackwards0_4(), 
											&KPath_InsertEscape0_e(), &KPath_InsertEscapeBackwards0_5(), NULL, NULL);

	TestDelimitedPathPushAndEscapeFrontL(KPath_PushEscapeFront0_0, &KPath_PushEscape0_a(), &KPath_PushEscapeFront0_1(), 
										&KPath_PushEscape0_b(), &KPath_PushEscapeFront0_2(), 
										&KPath_PushEscape0_c(), &KPath_PushEscapeFront0_3(), 
										&KPath_PushEscape0_d(), &KPath_PushEscapeFront0_4(), 
										&KPath_PushEscape0_e(), &KPath_PushEscapeFront0_5(), NULL, NULL);

	TestDelimitedPathPushAndEscapeBackL(KPath_PushEscapeBack0_0, &KPath_PushEscape0_a(), &KPath_PushEscapeBack0_1(), 
										&KPath_PushEscape0_b(), &KPath_PushEscapeBack0_2(), 
										&KPath_PushEscape0_c(), &KPath_PushEscapeBack0_3(), 
										&KPath_PushEscape0_d(), &KPath_PushEscapeBack0_4(), 
										&KPath_PushEscape0_e(), &KPath_PushEscapeBack0_5(), NULL, NULL);

//
//
// Delimited PathSegment Tests
//
//
/*
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
*/	}

//
//
//	Delimited Path tests
//
//

void CDelimitedPathTest::TestDelimitedPathExtractionL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataExtractionL(_L("Test TDelimitedPathParser Parse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathReverseExtractionL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataReverseExtractionL(_L("Test TDelimitedPathParser ParseReverse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathDelimiterPresenceL(const TDesC& aPath, TBool aFrontDelim, TBool aBackDelim) const
	{
	TestDelimitedDataDelimiterPresenceL(_L("Test TDelimitedPathParser Delimiter Presence"), iTestHarness, aPath, aFrontDelim, aBackDelim);
	}

void CDelimitedPathTest::TestDelimitedPathRemainderL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataRemainderL(_L("Test TDelimitedPathParser Remainder, Parse"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathRemainderReverseL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataRemainderReverseL(_L("Test TDelimitedPathParser Remainder, ParseReverse"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathDesL(const TDesC& aPath) const
	{
	TestDelimitedDataDesL(_L("Test TDelimitedPathParser Des"), iTestHarness, aPath);
	}

void CDelimitedPathTest::TestDelimitedPathPushFrontL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPushFrontL(_L("Test CDelimitedPath PushFront"), iTestHarness, path, list1, list2);
	VA_END(list1);
	VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathPushBackL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPushBackL(_L("Test CDelimitedPath PushBack"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathPopFrontL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPopFrontL(_L("Test CDelimitedPath PopFront"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathPopBackL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPopBackL(_L("Test CDelimitedPath PopBack"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathAddAndTrimFrontAndBackDelimiterL(const TDesC& aPath) const
	{
	TestDelimitedDataAddAndTrimFrontAndBackDelimiterL(_L("Test CDelimitedPath Add and Trim Front and Back delimiter"), iTestHarness, aPath);
	}

void CDelimitedPathTest::TestDelimitedPathInsertAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataInsertAndParseL(_L("Test CDelimitedPath InsertCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathRemoveAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataRemoveAndParseL(_L("Test CDelimitedPath RemoveCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathInsertAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataInsertAndParseReverseL(_L("Test CDelimitedPath InsertCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathRemoveAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataRemoveAndParseReverseL(_L("Test CDelimitedPath RemoveCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathInsertEscapeAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataInsertEscapeAndParseL(_L("Test CDelimitedPath InsertAndEscapeCurrent, Parse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathInsertEscapeAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataInsertEscapeAndParseReverseL(_L("Test CDelimitedPath InsertAndEscapeCurrent, ParseReverse"), iTestHarness, path, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathPushAndEscapeFrontL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPushAndEscapeFrontL(_L("Test CDelimitedPath PushAndEscapeFront"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathPushAndEscapeBackL(TRefByValue<const TDesC> aPath, ...) const
	{
	VA_LIST list1; VA_START(list1, aPath);
	VA_LIST list2; VA_START(list2, aPath);
	const TDesC& path = aPath;

	TestDelimitedDataPushAndEscapeBackL(_L("Test CDelimitedPath PushAndEscapeBack"), iTestHarness, path, list1, list2);
	VA_END(list1); VA_END(list2);
	}
/*
//
//
//	Delimited PathSegment tests
//
//

void CDelimitedPathTest::TestDelimitedPathSegmentExtractionL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataExtractionL(TDelimitedPathSegmentParser16(), TDelimitedPathSegmentParser8(),
		_L("Test TDelimitedPathSegmentParser Parse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentReverseExtractionL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataReverseExtractionL<TDelimitedPathSegmentParser16, TDelimitedPathSegmentParser8>(
		_L("Test TDelimitedPathSegmentParser ParseReverse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentDelimiterPresenceL(const TDesC& aPathSegment, TBool aFrontDelim, TBool aBackDelim) const
	{
	TestDelimitedDataDelimiterPresenceL<TDelimitedPathSegmentParser16, TDelimitedPathSegmentParser8>(
		_L("Test TDelimitedPathSegmentParser Delimiter Presence"), iTestHarness, aPathSegment, aFrontDelim, aBackDelim);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentRemainderL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataRemainderL<TDelimitedPathSegmentParser16, TDelimitedPathSegmentParser8>(
		_L("Test TDelimitedPathSegmentParser Remainder, Parse"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentRemainderReverseL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataRemainderReverseL<TDelimitedPathSegmentParser16, TDelimitedPathSegmentParser8>(
		_L("Test TDelimitedPathSegmentParser Remainder, ParseReverse"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentDesL(const TDesC& aPathSegment) const
	{
	TestDelimitedDataDesL<TDelimitedPathSegmentParser16, TDelimitedPathSegmentParser8>(_L("Test TDelimitedPathSegment Des"), iTestHarness, aPathSegment);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentPushFrontL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	CDelimitedPath16* path16Bit = NULL; CDelimitedPath8* path8Bit = NULL;
	TestDelimitedDataPushFrontL(path16Bit, path8Bit,
		_L("Test TDelimitedPathSegmentParser PushFront"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1);
	VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentPushBackL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataPushBackL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser PushBack"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentPopFrontL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataPopFrontL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser PopFront"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentPopBackL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataPopBackL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser PopBack"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentAddAndTrimFrontAndBackDelimiterL(const TDesC& aPathSegment) const
	{
	TestDelimitedDataAddAndTrimFrontAndBackDelimiterL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser Add and Trim Front and Back delimiter"), iTestHarness, aPathSegment);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentInsertAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataInsertAndParseL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser InsertCurrent, Parse"), iTestHarness, pathSegment, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentRemoveAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataRemoveAndParseL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser RemoveCurrent, Parse"), iTestHarness, pathSegment, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentInsertAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataInsertAndParseReverseL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser InsertCurrent, ParseReverse"), iTestHarness, pathSegment, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentRemoveAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataRemoveAndParseReverseL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser RemoveCurrent, ParseReverse"), iTestHarness, pathSegment, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentInsertEscapeAndParseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataInsertEscapeAndParseL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser InsertAndEscapeCurrent, Parse"), iTestHarness, pathSegment, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentInsertEscapeAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataInsertEscapeAndParseReverseL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser InsertAndEscapeCurrent, ParseReverse"), iTestHarness, pathSegment, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentPushAndEscapeFrontL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataPushAndEscapeFrontL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser PushAndEscapeFront"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedPathSegmentPushAndEscapeBackL(TRefByValue<const TDesC> aPathSegment, ...) const
	{
	VA_LIST list1; VA_START(list1, aPathSegment);
	VA_LIST list2; VA_START(list2, aPathSegment);
	const TDesC& pathSegment = aPathSegment;

	TestDelimitedDataPushAndEscapeBackL<CDelimitedPathSegment16, CDelimitedPathSegment8>(
		_L("Test TDelimitedPathSegmentParser PushAndEscapeBack"), iTestHarness, pathSegment, list1, list2);
	VA_END(list1); VA_END(list2);
	}

//
//
//	Delimited Query tests
//
//
void CDelimitedPathTest::TestDelimitedQueryExtractionL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataExtractionL<TDelimitedQueryParser16, TDelimitedQueryParser8>(
		_L("Test TDelimitedQueryParser Parse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryReverseExtractionL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataReverseExtractionL<TDelimitedQueryParser16, TDelimitedQueryParser8>(
		_L("Test TDelimitedQueryParser ParseReverse : GetNext, Dec, Inc, Peek and Eos"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryDelimiterPresenceL(const TDesC& aQuery, TBool aFrontDelim, TBool aBackDelim) const
	{
	TestDelimitedDataDelimiterPresenceL<TDelimitedQueryParser16, TDelimitedQueryParser8>(
		_L("Test TDelimitedQueryParser Delimiter Presence"), iTestHarness, aQuery, aFrontDelim, aBackDelim);
	}

void CDelimitedPathTest::TestDelimitedQueryRemainderL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataRemainderL<TDelimitedQueryParser16, TDelimitedQueryParser8>(
		_L("Test TDelimitedQueryParser Remainder, Parse"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryRemainderReverseL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataRemainderReverseL<TDelimitedQueryParser16, TDelimitedQueryParser8>(
		_L("Test TDelimitedQueryParser Remainder, ParseReverse"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryDesL(const TDesC& aQuery) const
	{
	TestDelimitedDataDesL<TDelimitedQueryParser16, TDelimitedQueryParser8>(_L("Test TDelimitedQuery Des"), iTestHarness, aQuery);
	}

void CDelimitedPathTest::TestDelimitedQueryPushFrontL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataPushFrontL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser PushFront"), iTestHarness, query, list1, list2);
	VA_END(list1);
	VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryPushBackL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataPushBackL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser PushBack"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryPopFrontL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataPopFrontL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser PopFront"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryPopBackL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataPopBackL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser PopBack"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryAddAndTrimFrontAndBackDelimiterL(const TDesC& aQuery) const
	{
	TestDelimitedDataAddAndTrimFrontAndBackDelimiterL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser Add and Trim Front and Back delimiter"), iTestHarness, aQuery);
	}

void CDelimitedPathTest::TestDelimitedQueryInsertAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataInsertAndParseL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser InsertCurrent, Parse"), iTestHarness, query, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryRemoveAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataRemoveAndParseL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser RemoveCurrent, Parse"), iTestHarness, query, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryInsertAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataInsertAndParseReverseL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser InsertCurrent, ParseReverse"), iTestHarness, query, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryRemoveAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataRemoveAndParseReverseL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser RemoveCurrent, ParseReverse"), iTestHarness, query, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryInsertEscapeAndParseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataInsertEscapeAndParseL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser InsertAndEscapeCurrent, Parse"), iTestHarness, query, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryInsertEscapeAndParseReverseL(TInt aStartPos, TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataInsertEscapeAndParseReverseL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser InsertAndEscapeCurrent, ParseReverse"), iTestHarness, query, list1, list2, aStartPos);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryPushAndEscapeFrontL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataPushAndEscapeFrontL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser PushAndEscapeFront"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}

void CDelimitedPathTest::TestDelimitedQueryPushAndEscapeBackL(TRefByValue<const TDesC> aQuery, ...) const
	{
	VA_LIST list1; VA_START(list1, aQuery);
	VA_LIST list2; VA_START(list2, aQuery);
	const TDesC& query = aQuery;

	TestDelimitedDataPushAndEscapeBackL<CDelimitedQuery16, CDelimitedQuery8>(
		_L("Test TDelimitedQueryParser PushAndEscapeBack"), iTestHarness, query, list1, list2);
	VA_END(list1); VA_END(list2);
	}
*/
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

	// Create TDelimitedPathParser object and do parsing
	TDelimitedPathParser16 parser16Bit;
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

	// Create TDelimitedPathParser object and do parsing
	TDelimitedPathParser8 parser8Bit;
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

	// Create TDelimitedPathParser object and do parsing
	TDelimitedPathParser16 parser16Bit;
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

	// Create TDelimitedPathParser object and do parsing
	TDelimitedPathParser8 parser8Bit;
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

	// Create TDelimitedPathParser object 
	TDelimitedPathParser16 parser16Bit;
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

	// Create TDelimitedPathParser object 
	TDelimitedPathParser8 parser8Bit;
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
	TDelimitedPathParser16 parser16Bit;
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
	TDelimitedPathParser8 parser8Bit;
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

	// Create TDelimitedPathParser object and do parsing
	TDelimitedPathParser16 parser16Bit;
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

	// Create TDelimitedPathParser object and do parsing
	TDelimitedPathParser8 parser8Bit;
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

	// Create TDelimitedPathParser object and do parsing
	TDelimitedPathParser16 parser16Bit;
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

	// Create TDelimitedPathParser object and do parsing
	TDelimitedPathParser8 parser8Bit;
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath16* data16Bit = CDelimitedPath16::NewL(aData);	// Also excersies NewLC()
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

	// Create TDelimitedPathParser object and do parsing
	CDelimitedPath8* data8Bit = CDelimitedPath8::NewL(initData8Bit);	// Also excersies NewLC()
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
