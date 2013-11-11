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
* Implements default base classes for captestframework
*
*/




/**
 @file
*/

#ifndef CAPTESTUTILITY_H__
#define CAPTESTUTILITY_H__

#include <e32base.h>
#include <s32file.h> 
#include "captestframework.h"

// CDefaultCapabilityTest
class CDefaultCapabilityTest : public CBase, public MCapabilityTest
	{
public:
	IMPORT_C const TDesC& Name() const;
	IMPORT_C const TDesC& SubName() const;
	
	IMPORT_C TCapabilitySet CapabilitiesRequired() const;
	IMPORT_C TUid VidRequired() const;
	IMPORT_C TUid SidRequired() const;

	IMPORT_C ~CDefaultCapabilityTest();
	
protected:
	IMPORT_C void SetNameL(const TDesC& aName);	
	void SetExpectPermissionDenied(TBool aExpectPermissionDenied = ETrue);	

	IMPORT_C void SetCapabilityRequired(const TCapability& aCapability);
	IMPORT_C void SetSidRequired(const TUid& aSid);
	IMPORT_C void SetVidRequired(const TUid& aVid);
	
	IMPORT_C TInt CheckFailL(TInt aResult, const TDesC& aTestName);
	IMPORT_C void RunTestL(RFile& aLogFile);

	void SetFail() { iFail = ETrue;}
	virtual void RunTestL() =0;
private:
	HBufC* iTestName;
	HBufC* iTestSubName;
	
	RFile* iLogFile;
	RFileWriteStream* iLogWriteStream;
	TBool iExpectPermissionDenied;
	TCapabilitySet iCapabilitySet;
	TUid iVidRequired;
	TUid iSidRequired;	
	TBool iFail;
	};

// CDefaultCapabilityTestFactory
class CDefaultCapabilityTestFactory : public CBase, public MCapabilityTestFactory

	{
public:
	IMPORT_C virtual TInt NumberOfTests();
	IMPORT_C virtual MCapabilityTest* Test(TInt aTestNumber);
	
	IMPORT_C void AddTestL(CDefaultCapabilityTest* aTest);

	IMPORT_C ~CDefaultCapabilityTestFactory();
	
private:
	RPointerArray<CDefaultCapabilityTest> iTests;
	};

// inline functions from CSwiCapabilityTest

inline void CDefaultCapabilityTest::SetExpectPermissionDenied(TBool aExpectPermissionDenied)
	{
	iExpectPermissionDenied = aExpectPermissionDenied;
	}

#endif //#ifndef CAPTESTUTILITY_H__
