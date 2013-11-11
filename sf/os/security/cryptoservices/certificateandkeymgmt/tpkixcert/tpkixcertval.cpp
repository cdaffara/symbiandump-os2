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

#include "tpkixcertval.h"
#include "tcertutils.h"
#include "t_input.h"

//tags for elements in specification file
_LIT(KRootStart, "<root>");
_LIT(KRootEnd, "</root>");
_LIT(KCertificateStart, "<certificate>");
_LIT(KCertificateEnd, "</certificate>");
_LIT(KLabelStart, "<label>");
_LIT(KLabelEnd, "</label>");
_LIT(KEEStart, "<ee>");
_LIT(KEEEnd, "</ee>");
_LIT(KInterStart, "<inter>");
_LIT(KInterEnd, "</inter>");
_LIT(KIPoliciesStart, "<ipolicies>");
_LIT(KIPoliciesEnd, "</ipolicies>");
_LIT(KIPolicyStart, "<ipolicy>");
_LIT(KIPolicyEnd, "</ipolicy>");
_LIT(KOResultStart, "<oresult>");
_LIT(KOResultEnd, "</oresult>");
_LIT(KOPoliciesStart, "<opolicies>");
_LIT(KOPoliciesEnd, "</opolicies>");
_LIT(KOPolicyStart, "<opolicy>");
_LIT(KOPolicyEnd, "</opolicy>");

//const TUint KUnicodeMarker = 0xfeff;

//**Cert Chain**//
CTestChain* CTestChain::NewL(const TDesC& aBuf)
	{
	CTestChain* self = CTestChain::NewLC(aBuf);
	CleanupStack::Pop();
	return self;
	}

CTestChain* CTestChain::NewLC(const TDesC& aBuf)
	{
	CTestChain* self = new(ELeave) CTestChain;
	CleanupStack::PushL(self);
	self->ConstructL(aBuf);
	return self;
	}

CTestChain::~CTestChain()
	{
	delete iIntermediateCertsFileName;
	delete iIntermediateCertsLabel;
	}

CTestChain::CTestChain()
	{
	}

void CTestChain::ConstructL(const TDesC& aBuf)
	{
	TInt pos = 0;
	TInt err = KErrNone;
	TPtrC rootCert(Input::ParseElement(aBuf, KRootStart, KRootEnd, pos, err));
	TInt dummyPos = 0;
	iRootCertFileName = Input::ParseElement(rootCert, KCertificateStart, 
		KCertificateEnd, dummyPos, err);
	iRootCertLabel = Input::ParseElement(rootCert, KLabelStart,
		KLabelEnd, dummyPos, err);
	if (iRootCertLabel == KNullDesC)
		{
		User::Leave(KErrNotFound);
		}

	TPtrC EECert(Input::ParseElement(aBuf, KEEStart, KEEEnd, pos));
	dummyPos = 0;
	iEECertFileName = Input::ParseElement(EECert, KCertificateStart,
		KCertificateEnd, dummyPos);
	iEECertLabel = Input::ParseElement(EECert, KLabelStart,
		KLabelEnd, dummyPos);

	iIntermediateCertsFileName = new(ELeave) CDesCArrayFlat(1);
	iIntermediateCertsLabel = new(ELeave) CDesCArrayFlat(1);
	while(AddInter(aBuf, pos))
		{
		}
	}

TBool CTestChain::AddInter(const TDesC& aBuf, TInt& aPos)
	{
	TPtrC interBuf = Input::ParseElement(aBuf, KInterStart, KInterEnd, aPos);
	TInt dummyPos = 0;
	if (interBuf != KNullDesC)
		{
		iIntermediateCertsFileName->AppendL(Input::ParseElement(interBuf, 
			KCertificateStart, KCertificateEnd, dummyPos));
		iIntermediateCertsLabel->AppendL(Input::ParseElement(interBuf,
			KLabelStart, KLabelEnd, dummyPos));
		return ETrue;
		}
	return EFalse;
	}

//**Policy Input/Output**//
CTestParameters* CTestParameters::NewL(const TDesC& aBuf)
	{
	CTestParameters* self = CTestParameters::NewLC(aBuf);
	CleanupStack::Pop();
	return self;
	}

CTestParameters* CTestParameters::NewLC(const TDesC& aBuf)
	{
	CTestParameters* self = new(ELeave) CTestParameters;
	CleanupStack::PushL(self);
	self->ConstructL(aBuf);
	return self;
	}

CTestParameters::~CTestParameters()
	{
	delete iWarnings;
	delete iPolicyInput;
	delete iExpectedPolicyOutput;
	}

CTestParameters::CTestParameters()
	:iIPoliciesSet(ETrue), iOPoliciesSet(ETrue), iOWarningsSet(ETrue)
	{
	}

void CTestParameters::ConstructL(const TDesC& aBuf)
	{
	iWarnings = new(ELeave)CArrayFixFlat<TValidationStatus> (1);
	iPolicyInput = new(ELeave) CDesCArrayFlat (1);
	iExpectedPolicyOutput = new(ELeave) CDesCArrayFlat (1);

	TInt pos = 0;
	TInt err = KErrNone;
	TPtrC iIPolicyBuf = Input::ParseElement(aBuf, KIPoliciesStart, KIPoliciesEnd, pos, err);
	if (err != KErrNone)
		{
		iIPoliciesSet = EFalse;//input policies are optional
		}
	pos = 0;
	while(AddPolicy(iIPolicyBuf, KIPolicyStart, KIPolicyEnd,pos, *iPolicyInput))
		{
		}	
	iError = Input::ParseElement(aBuf, KOResultStart, KOResultEnd, pos);
	TPtrC iOPolicyBuf = Input::ParseElement(aBuf, KOPoliciesStart, KOPoliciesEnd, pos, err);
	if (err != KErrNone)
		{
		iOPoliciesSet = EFalse;//output policies are optional
		}
	pos = 0;
	while(AddPolicy(iOPolicyBuf, KOPolicyStart, KOPolicyEnd, pos, *iExpectedPolicyOutput))
		{
		}	
	}

TBool CTestParameters::AddPolicy(const TDesC& aBuf, const TDesC& aStart, const TDesC& aEnd, TInt& aPos, CDesCArray& aPolicySet)
	{
	TPtrC policyBuf = Input::ParseElement(aBuf, aStart, aEnd, aPos);
	if (policyBuf != KNullDesC)
		{
		aPolicySet.AppendL(policyBuf);
		return ETrue;
		}
	return EFalse;
	}
