/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/




/**
 @file
*/

#include "captestutility.h"

// CDefaultCapabilityTest

EXPORT_C CDefaultCapabilityTest::~CDefaultCapabilityTest()
	{
	delete iTestName;

	if (iLogWriteStream)
		{
		iLogWriteStream->WriteInt32L(EFileEnd); // End marker
		iLogWriteStream->Close();
		delete iLogWriteStream;
		iLogWriteStream=0;
		}
	}
	
EXPORT_C void CDefaultCapabilityTest::SetNameL(const TDesC& aName)
	{
	delete iTestName;
	iTestName=0;
	iTestName=HBufC::NewL(aName.Length());
	TPtr testNamePtr=iTestName->Des();
	testNamePtr.Copy(aName);
	}

EXPORT_C TInt CDefaultCapabilityTest::CheckFailL(TInt aResult, const TDesC& aTestName)
	{
	_LIT(KColourRed, "<font color=FF0000>");
	_LIT(KColourGreen, "<font color=00AF00>");
	_LIT(KFontEndColour, "</font>");
	TBuf<1024> buf;
	
	if (iExpectPermissionDenied && aResult==KErrPermissionDenied)
		{
		buf.Format(_L("%S%S: Permission Denied,    Err=%d%S"), &KColourGreen, &aTestName, aResult, &KFontEndColour);
		iLogWriteStream->WriteInt32L(ETestPassed);
		*iLogWriteStream << buf;
		return aResult;
		}
	else if (!iExpectPermissionDenied && aResult!=KErrPermissionDenied)
		{
		buf.Format(_L("%S%S: Succeeded,    Err=%d%S"), &KColourGreen, &aTestName, aResult, &KFontEndColour);
		iLogWriteStream->WriteInt32L(ETestPassed);
		*iLogWriteStream << buf;
		return aResult;
		}

	// Test failed, do something with error.
	if (aResult == KErrPermissionDenied)
		{
		buf.Format(_L("%S%S: Permission denied! (Unexpected),    Err=%d%S"), &KColourRed, &aTestName, aResult, &KFontEndColour);
		iLogWriteStream->WriteInt32L(ETestFailed);
		*iLogWriteStream << buf;
		}
	else
		{
		buf.Format(_L("%S%S: Succeeded! (Unexpected),    Err=%d%S"), &KColourRed, &aTestName, aResult, &KFontEndColour);
		iLogWriteStream->WriteInt32L(ETestFailed);
		*iLogWriteStream << buf;
		}
	
	SetFail();
	return aResult;
	}

EXPORT_C void CDefaultCapabilityTest::RunTestL(RFile& aLogFile)
	{
	// Setup logging
	iLogFile=&aLogFile;
	
	if (iLogWriteStream)
		{
		iLogWriteStream->Close();
		delete iLogWriteStream;
		iLogWriteStream=0;
		}
	iLogWriteStream=new(ELeave) RFileWriteStream(aLogFile, 0);
	
	// run tests
	RunTestL();

	if (!iFail)
		{
		iLogWriteStream->WriteInt32L(ETestsEnded); // Test ended ok
		}
	}

EXPORT_C const TDesC& CDefaultCapabilityTest::Name() const
	{
	if (iTestName)
		{
		return *iTestName;
		}
	return KNullDesC;
	}

EXPORT_C const TDesC& CDefaultCapabilityTest::SubName() const
	{
	if (iTestSubName)
		{
		return *iTestSubName;
		}
	return KNullDesC;
	}

EXPORT_C TCapabilitySet CDefaultCapabilityTest::CapabilitiesRequired() const
	{
	return iCapabilitySet;
	}

EXPORT_C TUid CDefaultCapabilityTest::VidRequired() const
	{
	return iVidRequired;
	}
	
EXPORT_C TUid CDefaultCapabilityTest::SidRequired() const
	{
	return iSidRequired;
	}

EXPORT_C void CDefaultCapabilityTest::SetCapabilityRequired(const TCapability& aCapability)
	{
	iCapabilitySet.AddCapability(aCapability);
	}

EXPORT_C void CDefaultCapabilityTest::SetSidRequired(const TUid& aSid)
	{
	iSidRequired=aSid;
	}
	
EXPORT_C void CDefaultCapabilityTest::SetVidRequired(const TUid& aVid)
	{
	iVidRequired=aVid;
	}

// CDefaultCapabilityTestFactory
EXPORT_C MCapabilityTest* CDefaultCapabilityTestFactory::Test(TInt aTestNumber)
	{
	return iTests[aTestNumber];
	}

EXPORT_C TInt CDefaultCapabilityTestFactory::NumberOfTests()
	{
	return iTests.Count();
	}
	
EXPORT_C void CDefaultCapabilityTestFactory::AddTestL(CDefaultCapabilityTest* aTest)
	{
	iTests.AppendL(aTest);
	}


EXPORT_C CDefaultCapabilityTestFactory::~CDefaultCapabilityTestFactory()
	{
	for (TInt i=0; i< iTests.Count(); ++i)
		{
		delete iTests[i];
		}
	iTests.Reset();
	}

