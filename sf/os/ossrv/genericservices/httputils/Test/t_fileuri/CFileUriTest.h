// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CFILEURITEST_H__
#define __CFILEURITEST_H__

// System includes
//
#include <e32base.h>
#include <uri8.h>
#include <uri16.h>

// Local includes
//
#include "IpuTestUtils.h"
#include "ctestbase.h"

// CFileUriTest - test class for the Uri family of classes for the file URI implementation
//
class CFileUriTest : public CTestBase
	{
public:
	// Static factory c'tor. Leaves pointer to created object on the cleanup stack.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CFileUriTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CFileUriTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CFileUriTest();

	// Runs the defined tests.
	//
	void DoTestsL();

private:	// Methods

	// Default c'tor.
	//
	CFileUriTest(CIpuTestHarness* aTestHarness);

	// Non-trivial c'tor. Second part of 2-phase construction - does all allocation.
	//
	void ConstructL();


//
//
//	File Uri Tests
//
//
	void TestFileUriCreationL(const TDesC16& aFullFileName, const TDesC16& aExpectedUri, TUint aFlags) const;
	void TestPrivateFileUriCreationL(const TDesC16& aRelativeFileName, const TDriveNumber aDrive, const TDesC16& aExpectedUri, TUint aFlags) const;
	void TestFileUriComponentExtractionL(const TDesC16& aFileName, const TDesC16& aPath, const TDesC16& aExpectedFileName, const TDesC16& aExpectedDrive, TUint aFlags) const;
	void TestPrivateFileUriComponentExtractionL(const TDesC16& aFileName, const TDriveNumber aDrive, const TDesC16& aPath, const TDesC16& aExpectedFileName, TUint aFlags) const;
	void TestFileUriCreationWithOldAPIAndComponentExtractionL(const TDesC16& aFileName, const TDesC16& aPath, const TDesC16& aExpectedFileName) const;
	void TestFileUriNameExtractionL(const TDesC& aUri, const TDesC& aFilename, const TDesC& aPath);
	void TestFileUriNameExtractionL(const TDesC8& aUri, const TDesC& aFilename, const TDesC& aPath);

private:	// Attributes

	// Test harness
	//
	CIpuTestHarness*	iTestHarness;
	};

#endif	// __CFILEURITEST_H__
