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
// Implementation of test CConvertToRfc1123FormTest.
// 
//


#include "cconverttorfc1123formtest.h"
#include "inetprottextutils.h"
#include <tinternetdate.h>
#include "t_internetdatetestcases.h"



CConvertToRfc1123FormTest* CConvertToRfc1123FormTest::NewLC(CIpuTestHarness* aTestHarness)
	{
	CConvertToRfc1123FormTest* self = new(ELeave)CConvertToRfc1123FormTest(aTestHarness);
	CleanupStack::PushL(self);
	return self;
	}


CConvertToRfc1123FormTest::CConvertToRfc1123FormTest(CIpuTestHarness* aTestHarness):
	iTestHarness(aTestHarness)
	{}

CConvertToRfc1123FormTest::~CConvertToRfc1123FormTest()
	{}

void CConvertToRfc1123FormTest::DoTestsL()
	{
	TInt ii;
	
	for (ii=0; ii< KInvalidTimeNumTests; ++ii)
		{
		TInternetDate date;
		TRAPD( error, date.SetDateL(KInvalidTimeTestCases[ii]));
		if (error && error != KErrCorrupt)
			{
			User::LeaveIfError(error);
			}
		}
	
	for (ii=0; ii < KNumberDEF126191TestCases; ++ii)
		{
		TInternetDate internetDate;
		_LIT(KTest, "Test for fix DEF126191 - Case: ");
		TInternetDateOffsetTestCase tc = KDEF126191TestCases[ii];
		TBuf<512> testName;
		testName.Append(KTest);
		TPtrC8 testDate( tc.iTextInput );
		TBuf<150> bufTestDate;
		bufTestDate.Copy(testDate);
		testName.Append(bufTestDate);
		_LIT(KTestExpected, "\n \t \t \t \t And Expected Result is: ");
		testName.Append(KTestExpected);
		TPtrC8 expectedTestDate( tc.iExpectedOutput );
		TBuf<150> bufExpectedTestDate;
		bufExpectedTestDate.Copy(expectedTestDate);
		testName.Append(bufExpectedTestDate);
		
		iTestHarness->StartTestL(testName);
		TInt err = 0;
		internetDate.SetDateL(tc.iTextInput);
		TDateTime parsedDate = internetDate.DateTime();
		TInternetDate ConvertparsedDate(parsedDate);
		HBufC8* textDate = ConvertparsedDate.InternetDateTimeL(TInternetDate::ERfc1123Format); 
		CleanupStack::PushL(textDate);

		TBuf8 <50> KExpcted;
		KExpcted.Copy(tc.iExpectedOutput);
		if(textDate->CompareF(KExpcted) != 0)
			{
			err = KErrBadDescriptor;
			}
		CleanupStack::PopAndDestroy(textDate);
		iTestHarness->EndTest(err);
		}
		
	for (ii=0; ii< KNumberRfc1123GMTTzTestCases; ++ii)
		{
		TInternetDateTestCase tc = KRfc1123GMTTzTestCases[ii];
		TDateTime time(tc.iYear, tc.iMonth, tc.iDay, tc.iHour, tc.iMin, tc.iSecond, 0);
		TPtrC8 expected(tc.iTextFormat);
		ConvertTestL(time, expected);
		}

	for (ii=0; ii< KNumberINC090613TestCases; ++ii)
		{
		TInternetDate date;
		_LIT(KTest, "Test for fix INC090613 - Case: ");
		TBuf<256> testname;
		testname.Append(KTest);
		TPtrC8 testdate(KINC090613TestCases[ii]);
		TBuf<150> buftestdate;
		buftestdate.Copy(testdate);
		testname.Append(buftestdate);
				
		
		iTestHarness->StartTestL(testname);
		TRAPD(err,date.SetDateL(KINC090613TestCases[ii]));
		iTestHarness->EndTest(err);
		User::LeaveIfError(err);
		}
	for (ii=0; ii< KNumberINC107382TestCases; ++ii)
		{
		TInternetDate date;
		_LIT(KTest, "Test for fix INC107382 - Case: ");
		TBuf<256> testname;
		testname.Append(KTest);
		TPtrC8 testdate(KINC107382TestCases[ii]);
		TBuf<150> buftestdate;
		buftestdate.Copy(testdate);
		testname.Append(buftestdate);
				
		
		iTestHarness->StartTestL(testname);
		TRAPD(err,date.SetDateL(KINC107382TestCases[ii]));
		iTestHarness->EndTest(err);
		User::LeaveIfError(err);
		}
	
	for (ii=0; ii< KInvalidDescriptorToIntTests; ++ii)
		{
		_LIT(KTest, "Test for fix DEF138868 - Case: ");
		TBuf<256> testname;
		testname.Append(KTest);
		TPtrC8 testInput(KInvalidDesToIntTestCases[ii]);
		TBuf<150> buftestInput;
		buftestInput.Copy(testInput);
		testname.Append(buftestInput);
		
		iTestHarness->StartTestL(testname);
		TInt err=0;
		InetProtTextUtils::ConvertDescriptorToInt(KInvalidDesToIntTestCases[ii], err);
		iTestHarness->EndTest(err);
		User::LeaveIfError(err);
		}
		
	}
	

void CConvertToRfc1123FormTest::ConvertTestL(TDateTime aDateTime, const TDesC8& aExpectedResult)
	{
	_LIT(KStartText, "TDateTime->Rfc1123 Expected: ");
	TBuf<284> testName;
	testName.Append(KStartText);
	TBuf <256> expected;
	expected.Copy(aExpectedResult);
	testName.Append(expected);

	iTestHarness->StartTestL(testName);
	TRAPD(error, ConvertSingleTestL(aDateTime, aExpectedResult));
	iTestHarness->EndTest(error);
	User::LeaveIfError(error);
	}

void CConvertToRfc1123FormTest::ConvertSingleTestL(TDateTime aDateTime, const TDesC8& aExpectedResult)
	{
	TInternetDate internetDate(aDateTime);
	HBufC8* textDate = internetDate.InternetDateTimeL(TInternetDate::ERfc1123Format);
	CleanupStack::PushL(textDate);
	if (aExpectedResult.CompareF(*textDate) != 0)
		User::Leave(KErrCorrupt);

	CleanupStack::PopAndDestroy(textDate);
	}
