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
// Class CConvertToRfc1123FormTest used to test parsing of TDateTime into RFC 1123
// date/time formats
// 
//

#ifndef		__CCONVERTTORFC1123FORMTEST_H__
#define		__CCONVERTTORFC1123FORMTEST_H__

#include <e32base.h>

#include "IpuTestUtils.h"
#include "ctestbase.h"

class CConvertToRfc1123FormTest: public CTestBase
	{
public:
	static CConvertToRfc1123FormTest* NewLC(CIpuTestHarness* aTestHarness);
	~CConvertToRfc1123FormTest();
	void DoTestsL();

private:
	CConvertToRfc1123FormTest(CIpuTestHarness* aTestHarness);
	void ConvertTestL(TDateTime aDateTime, const TDesC8& aExpectedResult);
	void ConvertSingleTestL(TDateTime aDateTime, const TDesC8& aExpectedResult);

private:
	CIpuTestHarness* iTestHarness;
	};



#endif //	__CCONVERTTORFC1123FORMTEST_H__
