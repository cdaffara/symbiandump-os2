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


#include "TestChain.h"
#include "t_input.h"

_LIT(KServerStart, "<server>");
_LIT(KRootStart, "<root>");
_LIT(KValueStart, "<value>");

_LIT(KCertificateStart, "<certificate>");
_LIT(KLabelStart, "<label>");
_LIT(KResultStart, "<oresult>");

//test chain superclass
CTestChain::CTestChain()
	{
	}

CTestChain::~CTestChain()
	{
	delete iServerCerts;
	delete iServerLabels;
	delete iRootCerts;
	delete iRootLabels;
	delete iExtraCerts;
	delete iExtraLabels;
	}

void CTestChain::ConstructL()
	{
	iServerCerts = new(ELeave) CDesCArrayFlat (1);
	iServerLabels = new(ELeave) CDesCArrayFlat (1);
	iRootCerts = new(ELeave) CDesCArrayFlat (1);
	iRootLabels = new(ELeave) CDesCArrayFlat (1);
	iExtraCerts = new(ELeave) CDesCArrayFlat (1);
	iExtraLabels = new(ELeave) CDesCArrayFlat (1);
	}

//test build class		
CTestChainBuild* CTestChainBuild::NewL()
	{
	CTestChainBuild* self = CTestChainBuild::NewLC();
	CleanupStack::Pop();
	return self;
	}

CTestChainBuild* CTestChainBuild::NewLC()
	{
	CTestChainBuild* self = new(ELeave) CTestChainBuild;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestChainBuild::~CTestChainBuild()
	{
	delete iCorrectChain;
	}

//test chain validate class
CTestChainValidate* CTestChainValidate::NewL(void)
	{
	CTestChainValidate* self = CTestChainValidate::NewLC();
	CleanupStack::Pop();
	return self;
	}

CTestChainValidate* CTestChainValidate::NewLC(void)
	{
	CTestChainValidate* self = new(ELeave) CTestChainValidate;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CTestChainValidate::ConstructL(void)
	{
	CTestChain::ConstructL();
	iWarnings = new(ELeave)CArrayFixFlat<TWTLSValidationStatus> (1);
	iDateIssued = 0;
	}

void CTestChainValidate::AddChainL(const TDesC& aValues)
	{
	AddServerL(aValues);
	AddRootL(aValues);
	}

void CTestChainValidate::AddDateIssued(const TDesC& aValues)
	{
	TPtrC iResultBuf = Input::ParseElement(aValues, KValueStart);
	if((iResultBuf == _L("true")) || (iResultBuf == _L("1")))
		iDateIssued = ETrue;
	}
void CTestChainValidate::AddIOL(const TDesC& aValues)
	{
	TPtrC iResultBuf = Input::ParseElement(aValues, KResultStart);
	if (iResultBuf != KNullDesC)
		{
		TValidationError expectedError;

		if(iResultBuf == _L("ValidatedOK"))
			expectedError = EValidatedOK;
		else if(iResultBuf == _L("SignatureInvalid"))
			expectedError = ESignatureInvalid;
		else if(iResultBuf == _L("ChainHasNoRoot"))
			expectedError = EChainHasNoRoot;
		else if(iResultBuf == _L("NamesDontChain"))
			expectedError = ENamesDontChain;
		else if(iResultBuf == _L("NotCACert"))
			expectedError = ENotCACert;
		else 
			return;

		iError = new(ELeave) TWTLSValidationStatus(expectedError, 0);
		}
	}

CTestChainValidate::~CTestChainValidate()
	{
	delete iError;
	delete iWarnings;
	};

TBool CTestChainValidate::AddServerL(const TDesC& aBuf)
	{
	TInt nPos, n;
	TPtrC ioSection(aBuf);
	TPtrC ioBuf;
	if (ioSection != KNullDesC)
		{
		do
			{
			nPos = 0;
			ioBuf.Set(Input::ParseElement(ioSection, KServerStart, nPos));
			if(nPos > 0)
				{
				ioSection.Set(ioSection.Mid(nPos));
				n=0;
				TPtrC iCertBuf = Input::ParseElement(ioBuf, KCertificateStart, n);
				iServerCerts->AppendL(iCertBuf);
				TPtrC iLabelBuf = Input::ParseElement(ioBuf, KLabelStart, n);
				iServerLabels->AppendL(iLabelBuf);
				};
			
			}
		while(nPos > 0);
		return ETrue;
		}
	return EFalse;
	}

TBool CTestChainValidate::AddRootL(const TDesC& aBuf)
	{
	TInt nPos, n;
	TPtrC ioSection(aBuf);
	TPtrC ioBuf;
	if (ioSection != KNullDesC)
		{
		do
			{
			nPos = 0;
			ioBuf.Set(Input::ParseElement(ioSection, KRootStart, nPos));
			if(nPos > 0)
				{
				ioSection.Set(ioSection.Mid(nPos));
				n=0;
				TPtrC iCertBuf = Input::ParseElement(ioBuf, KCertificateStart, n);
				iRootCerts->AppendL(iCertBuf);
				TPtrC iLabelBuf = Input::ParseElement(ioBuf, KLabelStart, n);
				iRootLabels->AppendL(iLabelBuf);
				};
			
			}
		while(nPos > 0);
		return ETrue;
		}
	return EFalse;
	}


