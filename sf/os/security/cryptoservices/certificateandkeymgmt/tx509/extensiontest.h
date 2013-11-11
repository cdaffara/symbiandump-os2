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
* CExtensionTest class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __EXTENSIONTEST_H
#define __EXTENSIONTEST_H

#include <x509certext.h>

#include "t_testaction.h"
#include "tcertwriter.h"

class CExtensionTest : public CTestAction
	{
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	void TestValidation(TRequestStatus& aStatus);
	~CExtensionTest();

	void PerformAction(TRequestStatus& aStatus);
	void DoReportAction();
	void DoCheckResult(TInt aError);

private:
	CExtensionTest(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);
	
	// compares arrays of things returning true if they are equal
	TBool IsEqual(const RArray<TInt>& aArray1, const RArray<TInt>& aArray2);
	TBool IsEqual(const RPointerArray<HBufC>& aArray1, const RPointerArray<HBufC>& aArray2);
	
	// return true if the extension data matches the expected value
	void CheckExtensionsL(const CX509Certificate& cert, TBool& match, TBool& result);
	TBool CheckDeviceIdListL(const CX509Certificate& cert);
	TBool CheckSidListL(const CX509Certificate& cert);
	TBool CheckVidListL(const CX509Certificate& cert);
	TBool CheckCapabilitiesL(const CX509Certificate& cert);
	
	// populate expected value data structures from scritps
	void BuildIntList(RArray<TInt>& aInts, const TDesC& aBuf, const TDesC& aTag);
	void BuildStringListL(RPointerArray<HBufC>& aStrings, const TDesC& aBuf, const TDesC& aTag);
	void BuildCapabilitySet(TCapabilitySet& aCapabilitySet, const TDesC& aBuf);
	
	void PrintLine(const TDesC& aText);
	void Print(const TDesC& aText);

private:
	RFs& iFs;		
	TFileName iCertFileName;
		
	// Whether the input data is expeceted to match the certificate extension.
	TBool iExpectedMatch;
	
	// Whether the parsing of extension is expected to fail.
	TBool iExpectedCorrupt;	
	
	// Whether the constraints are expected within the certificate
	TBool iDeviceIdsPresent;
	TBool iSidsPresent;
	TBool iVidsPresent;
	TBool iCapabilitiesPresent;
	
	// The expected values associated with the extensions
	RPointerArray<HBufC> iExpectedDeviceIdList;
	RArray<TInt> iExpectedSidList;
	RArray<TInt> iExpectedVidList;
	TCapabilitySet iExpectedCapabilities;
	};

#endif
