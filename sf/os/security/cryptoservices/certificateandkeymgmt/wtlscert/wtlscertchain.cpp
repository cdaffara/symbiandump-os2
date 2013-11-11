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


#include <wtlscertchain.h>
#include <wtlskeys.h>
#include "wtlscertchainao.h"

//validation status
EXPORT_C TWTLSValidationStatus::TWTLSValidationStatus(const TValidationError aError,
													  const TInt aCert)
	:iReason(aError), iCert(aCert)
	{
	}

//validationresult

EXPORT_C CWTLSValidationResult* CWTLSValidationResult::NewLC()
	{
	CWTLSValidationResult* s = new(ELeave) CWTLSValidationResult;
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

EXPORT_C CWTLSValidationResult* CWTLSValidationResult::NewL()
	{
	CWTLSValidationResult* s = CWTLSValidationResult::NewLC();
	CleanupStack::Pop();
	return s;
	}

EXPORT_C CWTLSValidationResult::~CWTLSValidationResult()
	{
	delete iWarnings;
	}

CWTLSValidationResult::CWTLSValidationResult()
	:iError(EValidatedOK, 0)
	{
	}

void CWTLSValidationResult::ConstructL()
	{
	iWarnings = new(ELeave) CArrayFixFlat<TWTLSValidationStatus> (1);
	}

EXPORT_C const TWTLSValidationStatus CWTLSValidationResult::Error() const
	{
	return iError;
	}

EXPORT_C const CArrayFixFlat<TWTLSValidationStatus>& CWTLSValidationResult::Warnings() const
	{
	return *iWarnings;
	}

void CWTLSValidationResult::Reset()
	{
	iError = TWTLSValidationStatus(EValidatedOK, 0);
	iWarnings->Reset();
	}

void CWTLSValidationResult::SetError(const TValidationError aError, const TInt aCert)
	{
	iError.iReason = aError;
	iError.iCert = aCert;
	}

void CWTLSValidationResult::AppendWarningL(TWTLSValidationStatus aWarning)
	{
	iWarnings->AppendL(aWarning);
	}

		//WTLS cert chain

		//constructors
EXPORT_C CWTLSCertChain* CWTLSCertChain::NewL(RFs& aFs,
											  const TPtrC8& aEncodedCerts,
											  const TUid aClient)
	{
	CWTLSCertChain* self = CWTLSCertChain::NewLC(aFs, aEncodedCerts, aClient);
	CleanupStack::Pop();	//self
	return self;
	}

EXPORT_C CWTLSCertChain* CWTLSCertChain::NewLC(RFs& aFs,
											   const TPtrC8& aEncodedCerts,
											   const TUid aClient)
	{
	CWTLSCertChain* self = new(ELeave) CWTLSCertChain(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aEncodedCerts, aClient);
	return self;
	}

EXPORT_C CWTLSCertChain* CWTLSCertChain::NewL(RFs& aFs, 
											  const TPtrC8& aEncodedCerts, 
											  const CArrayPtr<CWTLSCertificate>& aRootCerts)
	{
	CWTLSCertChain* self = CWTLSCertChain::NewLC(aFs, aEncodedCerts, aRootCerts);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CWTLSCertChain* CWTLSCertChain::NewLC(RFs& aFs, 
											   const TPtrC8& aEncodedCerts, 
											   const CArrayPtr<CWTLSCertificate>& aRootCerts)
	{
	CWTLSCertChain* self = new(ELeave) CWTLSCertChain(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aEncodedCerts, aRootCerts);
	return self;
	}

		//destructor	
EXPORT_C CWTLSCertChain::~CWTLSCertChain()
	{
	if (iChain)
		iChain->ResetAndDestroy();
	delete iChain;
	delete iActiveObject;
	}

		//validation
EXPORT_C void CWTLSCertChain::ValidateL(CWTLSValidationResult& aValidationResult,
										const TTime& aValidationTime,
										TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iActiveObject, User::Panic(_L("CWTLSCertChain"), 1));

	iActiveObject->Validate(aValidationResult, aValidationTime, aStatus);
	}

		//accessors
EXPORT_C TInt CWTLSCertChain::Count() const
	{
	return iChain->Count();
	}

EXPORT_C const CWTLSCertificate& CWTLSCertChain::Cert(TInt aIndex) const
	{
	return *(iChain->At(aIndex));
	}

EXPORT_C TBool CWTLSCertChain::ChainHasRoot() const
	{
	return iChainHasRoot;
	}

EXPORT_C void CWTLSCertChain::AppendCertsL(const TPtrC8& aEncodedCerts)
	{
	for(TInt pos = 0; pos < aEncodedCerts.Size(); ) 
		{
		CWTLSCertificate* eeCert = CWTLSCertificate::NewLC(aEncodedCerts, pos);
		iChain->AppendL(eeCert);
		CleanupStack::Pop(eeCert);
		}
	}

//private functions
CWTLSCertChain::CWTLSCertChain(RFs& aFs)
	: iFs(aFs), iChainHasRoot(EFalse)
	{
	}
	
void CWTLSCertChain::ConstructL(const TPtrC8& aEncodedCerts, const TUid aClient)
	{
	iActiveObject = CWTLSCertChainAO::NewL(iFs, *this, aClient);
	DoConstructL(aEncodedCerts);
	}

void CWTLSCertChain::ConstructL(const TPtrC8& aEncodedCerts, const CArrayPtr<CWTLSCertificate>& aRootCerts)
	{
	iActiveObject = CWTLSCertChainAO::NewL(iFs, *this, aRootCerts);
	DoConstructL(aEncodedCerts);
	}

void CWTLSCertChain::DoConstructL(const TPtrC8& aEncodedCerts)
	{
	iChain = new(ELeave) CArrayPtrFlat<CWTLSCertificate> (5);
	//typical cert chain unlikely to be more than 5
	AppendCertsL(aEncodedCerts);
	}


