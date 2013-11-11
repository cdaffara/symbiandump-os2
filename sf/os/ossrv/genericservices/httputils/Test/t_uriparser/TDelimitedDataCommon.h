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

#ifndef __TDELIMITEDDATACOMMON_H__
#define __TDELIMITEDDATACOMMON_H__

// System includes
//
#include <e32base.h>

//
//
//	Test functions
//
//

LOCAL_C void TestDelimitedDataExtractionL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										  const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataReverseExtractionL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataDelimiterPresenceL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												 const TDesC& aData, TBool aFrontDelim, TBool aBackDelim);

LOCAL_C void TestDelimitedDataRemainderL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataRemainderReverseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataDesL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, const TDesC& aData);

LOCAL_C void TestDelimitedDataPushFrontL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataPushAndEscapeFrontL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												  const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataPushBackL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataPushAndEscapeBackL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
												 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataPopFrontL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
										const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataPopBackL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
									   const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2);

LOCAL_C void TestDelimitedDataAddAndTrimFrontAndBackDelimiterL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, const TDesC& aData);

LOCAL_C void TestDelimitedDataInsertAndParseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
											  const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos);

LOCAL_C void TestDelimitedDataRemoveAndParseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
											  const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos);

LOCAL_C void TestDelimitedDataInsertAndParseReverseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
													 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos);

LOCAL_C void TestDelimitedDataRemoveAndParseReverseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
													 const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos);

LOCAL_C void TestDelimitedDataInsertEscapeAndParseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
													const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos);

LOCAL_C void TestDelimitedDataInsertEscapeAndParseReverseL(const TDesC& aTestTitle, CIpuTestHarness* aTestHarness, 
														   const TDesC& aData, VA_LIST& aList1, VA_LIST& aList2, TInt aStartPos);

//
//
//	Templated helper functions
//
//

template<class HBufCType, class TPtrCType, class TDelimitedParserType>
TInt DoDelimitedDataParsingL(const TDelimitedParserType& aParser, VA_LIST& aSegments);

template<class HBufCType>
HBufCType* GetNextArgLC(VA_LIST& aList);

template<class TDelimitedParserType>
TInt DoDelimiterPresenceTest(const TDelimitedParserType& aParser, TBool aFrontDelim, TBool aBackDelim);

template<class HBufCType, class TPtrCType, class TDelimitedParserType>
TInt DoDelimitedDataRemainderTestL(const TDelimitedParserType& aParser, VA_LIST& aRemainders);

template<class TDelimitedParserType, class TDesCType>
TInt DoDelimitedDataDes(const TDelimitedParserType& aParser, const TDesCType& aDelimitedData);

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPushFrontL(CDelimitedDataType* aParser, VA_LIST& aData);

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPushBackL(CDelimitedDataType* aParser, VA_LIST& aData);

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPopFrontL(CDelimitedDataType* aParser, VA_LIST& aData);

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPopBackL(CDelimitedDataType* aParser, VA_LIST& aData);

template<class CDelimitedDataType>
TInt DoDelimitedDataAddAndTrimFrontAndBackDelimiterL(CDelimitedDataType* aParser);

template<class CDelimitedDataType>
TInt DoDelimitedDataParseToPosition(CDelimitedDataType* aParser, TInt aStartPos);

template<class HBufCType, class TPtrCType, class CDelimitedDataType>
TInt DoDelimitedDataInsertL(CDelimitedDataType* aParser, VA_LIST& aData);

template<class HBufCType, class TPtrCType, class CDelimitedDataType>
TInt DoDelimitedDataRemoveL(CDelimitedDataType* aParser, VA_LIST& aData);

template<class HBufCType, class TPtrCType, class CDelimitedDataType>
TInt DoDelimitedDataInsertAndEscapeL(CDelimitedDataType* aParser, VA_LIST& aData);

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPushAndEscapeFrontL(CDelimitedDataType* aParser, VA_LIST& aData);

template<class HBufCType, class CDelimitedDataType>
TInt DoDelimitedDataPushAndEscapeBackL(CDelimitedDataType* aParser, VA_LIST& aData);

#endif	// __TDELIMITEDDATACOMMON_H__
