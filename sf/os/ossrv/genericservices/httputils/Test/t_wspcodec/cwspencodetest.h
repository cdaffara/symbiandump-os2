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
// Class CWSPEncodeTest
// 
//

#ifndef		__CWSPENCODETEST_H__
#define		__CWSPENCODETEST_H__

#include <e32base.h>

#include "IpuTestUtils.h"
#include "ctestbase.h"

class CWSPEncodeTest: public CTestBase
	{
public:
	static CWSPEncodeTest* NewLC(CIpuTestHarness* aTestHarness);
	~CWSPEncodeTest();
	void DoTestsL();

private:
	CWSPEncodeTest(CIpuTestHarness* aTestHarness);
	void HeaderEncoderTestL();
	void TestHeaderEncoderL();
	void ShortIntTestL();
	TInt TestShortInt();
	void LongIntTestL();
	void TestLongIntL();
	void UintVarTestL();
	void TestUintVarL();
	void TestStringL();
	void StringTestL();
	void TestDateL();
	void DateTestL();

private:
	CIpuTestHarness* iTestHarness;
	};



#endif //	__CWSPENCODETEST_H__
