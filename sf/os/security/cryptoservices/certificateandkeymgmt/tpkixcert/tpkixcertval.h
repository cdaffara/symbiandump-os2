/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/
 
#ifndef __TPKIXCERTVAL_H__
#define __TPKIXCERTVAL_H__

#include <e32std.h>
#include <badesca.h>
#include <x509certchain.h>

class CTestChain : public CBase
	{
public:
	static CTestChain* NewL(const TDesC& aBuf);
	static CTestChain* NewLC(const TDesC& aBuf);
	~CTestChain();
	CTestChain();
	virtual void ConstructL(const TDesC& aBuf);
	TBool AddInter(const TDesC& aBuf, TInt& aPos);
	//
	TBuf<128> iEECertFileName;
	TBuf<128> iEECertLabel;
	TBuf<128> iRootCertFileName;
	TBuf<128> iRootCertLabel;
	CDesCArray* iIntermediateCertsFileName;
	CDesCArray* iIntermediateCertsLabel;
	};

class CTestParameters : public CBase
	{
public:
	static CTestParameters* NewL(const TDesC& aBuf);
	static CTestParameters* NewLC(const TDesC& aBuf);
	~CTestParameters();
	CTestParameters();
	virtual void ConstructL(const TDesC& aBuf);
	TBool AddPolicy(const TDesC& aBuf, const TDesC& aStart, const TDesC& aEnd, TInt& aPos, CDesCArray& aPolicySet);
	//
	TBuf<128> iError;
	CArrayFixFlat<TValidationStatus>* iWarnings;
	CDesCArray* iPolicyInput;
	CDesCArray* iExpectedPolicyOutput;
	TBool iIPoliciesSet;
	TBool iOPoliciesSet;
	TBool iOWarningsSet;
	};

#endif

