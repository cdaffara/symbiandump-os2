/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* crypto signature API implementation
* crypto signature API implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptosignatureapi.h>
#include <cryptospi/signerplugin.h>
#include <cryptospi/verifierplugin.h>
#include "legacyselector.h"

using namespace CryptoSpi;


//
// Implementation of signature base class 
//
CSignatureBase::CSignatureBase(MSignatureBase* aSignatureProcessor, TInt aHandle)
: CCryptoBase(aSignatureProcessor, aHandle)
	{	
	}

CSignatureBase::~CSignatureBase()
	{
	}

EXPORT_C void CSignatureBase::SetPaddingModeL(TUid aPaddingMode)
	{
	MSignatureBase* ptr=static_cast<MSignatureBase*>(iPlugin);
	ptr->SetPaddingModeL(aPaddingMode);
	}

EXPORT_C void CSignatureBase::SetKeyL(const CKey& aPrivateKey)
	{
	MSignatureBase* ptr=static_cast<MSignatureBase*>(iPlugin);
	ptr->SetKeyL(aPrivateKey);	
	}

EXPORT_C TInt CSignatureBase::GetMaximumInputLengthL() const
	{
	MSignatureBase* ptr=static_cast<MSignatureBase*>(iPlugin);
	return ptr->GetMaximumInputLengthL();		
	}

EXPORT_C TInt CSignatureBase::GetMaximumOutputLengthL() const
	{
	MSignatureBase* ptr=static_cast<MSignatureBase*>(iPlugin);
	return ptr->GetMaximumOutputLengthL();			
	}

//
// Implementation of Signer
//
CSigner* CSigner::NewL(MSigner* aSigner, TInt aHandle)
	{
	CSigner* self=new(ELeave) CSigner(aSigner, aHandle);
	return self;									
	}

CSigner::CSigner(MSigner* aSigner, TInt aHandle) : CSignatureBase(aSigner, aHandle)
	{
	}

EXPORT_C CSigner::~CSigner()
	{
	}

EXPORT_C void CSigner::SignL(const TDesC8& aInput, CCryptoParams& aSignature)
	{
	MSigner* ptr=static_cast<MSigner*>(iPlugin);
	ptr->SignL(aInput, aSignature);	
	}


//
// Implementation of Verifier
//
CVerifier* CVerifier::NewL(MVerifier* aVerifier, TInt aHandle)
	{
	CVerifier* self=new(ELeave) CVerifier(aVerifier, aHandle);
	return self;				
	}

CVerifier::CVerifier(MVerifier* aVerifier, TInt aHandle)
: CSignatureBase(aVerifier, aHandle)
	{
	}
	
EXPORT_C CVerifier::~CVerifier()
	{
	}

EXPORT_C void CVerifier::VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult)
	{
	MVerifier* ptr=static_cast<MVerifier*>(iPlugin);
	ptr->VerifyL(aInput, aSignature, aVerificationResult);
	}

EXPORT_C void CVerifier::InverseSignL(HBufC8*& aOutput, const CCryptoParams& aSignature)
	{
	MVerifier* ptr=static_cast<MVerifier*>(iPlugin);
	ptr->InverseSignL(aOutput, aSignature);
	}

//
// Implementation of Signer and Verifier Factory
//
EXPORT_C void CSignatureFactory::CreateSignerL(CSigner*& aSigner,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateSignerL(aSigner, aAlgorithmUid, aKey, aPaddingMode, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateSignerL(aSigner, aAlgorithmUid, aKey, aPaddingMode, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector	
		}
	}

EXPORT_C void CSignatureFactory::CreateVerifierL(CVerifier*& aVerifier,
												TUid aAlgorithmUid,
												const CKey& aKey,
												TUid aPaddingMode,
												const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateVerifierL(aVerifier, aAlgorithmUid, aKey, aPaddingMode, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateVerifierL(aVerifier, aAlgorithmUid, aKey, aPaddingMode, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector
		}
	}

//
// Implementation of Asynchronous Signer
// (async methods not implemented, so no coverage)
//

#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

CAsyncSigner* CAsyncSigner::NewL(MAsyncSigner* aSigner, TInt aHandle)
	{
	CAsyncSigner* self=new(ELeave) CAsyncSigner(aSigner, aHandle);
	return self;		
	}

CAsyncSigner::CAsyncSigner(MAsyncSigner* aSigner, TInt aHandle)
: CSignatureBase(aSigner, aHandle)
	{
	}

EXPORT_C CAsyncSigner::~CAsyncSigner()
	{
	}

EXPORT_C void CAsyncSigner::SignL(const TDesC8& aInput, CCryptoParams& aSignature, TRequestStatus& aRequestStatus)
	{
	MAsyncSigner* ptr=static_cast<MAsyncSigner*>(iPlugin);
	ptr->SignL(aInput, aSignature, aRequestStatus);
	}

EXPORT_C void CAsyncSigner::Cancel()
	{
	MAsyncSigner* ptr=static_cast<MAsyncSigner*>(iPlugin);
	ptr->Cancel();
	}

//
// Implementation of Asynchronous Verifier
//
CAsyncVerifier* CAsyncVerifier::NewL(MAsyncVerifier* aVerifier, TInt aHandle)
	{
	CAsyncVerifier* self=new(ELeave) CAsyncVerifier(aVerifier, aHandle);
	return self;
	}
	
CAsyncVerifier::CAsyncVerifier(MAsyncVerifier* aVerifier, TInt aHandle)
: CSignatureBase(aVerifier, aHandle)
	{
	}

EXPORT_C CAsyncVerifier::~CAsyncVerifier()
	{		
	}

EXPORT_C void CAsyncVerifier::VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult, TRequestStatus& aRequestStatus)
	{
	MAsyncVerifier* ptr=static_cast<MAsyncVerifier*>(iPlugin);
	ptr->VerifyL(aInput, aSignature, aVerificationResult, aRequestStatus);
	}

EXPORT_C void CAsyncVerifier::InverseSignL(HBufC8*& aOutput, const CCryptoParams& aSignature, TRequestStatus& aRequestStatus)
	{
	MAsyncVerifier* ptr=static_cast<MAsyncVerifier*>(iPlugin);
	ptr->InverseSignL(aOutput, aSignature, aRequestStatus);
	}

EXPORT_C void CAsyncVerifier::Cancel()
	{
	MAsyncVerifier* ptr=static_cast<MAsyncVerifier*>(iPlugin);
	ptr->Cancel();
	}

//
// Implementation of Signer and Verifier Factory
//
EXPORT_C void CSignatureFactory::CreateAsyncSignerL(CAsyncSigner*& aSigner,
												TUid aAlgorithmUid,
												const CKey& aKey,
												TUid aPaddingMode,
												const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncSignerL(aSigner, aAlgorithmUid, aKey, aPaddingMode, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncSignerL(aSigner, aAlgorithmUid, aKey, aPaddingMode, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //selector	
		}
	}

EXPORT_C void CSignatureFactory::CreateAsyncVerifierL(CAsyncVerifier*& aVerifier,
													TUid aAlgorithmUid,
													const CKey& aKey,
													TUid aPaddingMode,
													const CCryptoParams* aAlgorithmParams)
	{
	MPluginSelector* selector=reinterpret_cast<MPluginSelector *>(Dll::Tls());
	if (selector)
		{
		selector->CreateAsyncVerifierL(aVerifier, aAlgorithmUid, aKey, aPaddingMode, aAlgorithmParams);
		}
	else
		{
		CLegacySelector* legacySelector=CLegacySelector::NewLC();
		legacySelector->CreateAsyncVerifierL(aVerifier, aAlgorithmUid, aKey, aPaddingMode, aAlgorithmParams);
		CleanupStack::PopAndDestroy(legacySelector); //legacySelector
		}

	}


