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


#include <pkixcertchain.h>
#include <unifiedcertstore.h>
#include <x509certext.h>
#include "pkixCertChainHelper.h"

//**********************************************************************************//
EXPORT_C CPKIXCertChain* CPKIXCertChain::NewL(RFs& aFs,
											  const TPtrC8& aEncodedCerts, 
											  const TUid aClient)
	{
	CPKIXCertChain* self = CPKIXCertChain::NewLC(aFs, aEncodedCerts, aClient);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPKIXCertChain* CPKIXCertChain::NewLC(RFs& aFs,
											   const TPtrC8& aEncodedCerts,
											   const TUid aClient)
	{
	CPKIXCertChain* self = new(ELeave) CPKIXCertChain();
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aEncodedCerts, aClient);
	return self;
	}

EXPORT_C CPKIXCertChain* CPKIXCertChain::NewL(RFs& aFs, 
											  const TPtrC8& aEncodedCerts,
											  const RPointerArray<CX509Certificate>& aRootCerts)
	{
	CPKIXCertChain* self = CPKIXCertChain::NewLC(aFs, aEncodedCerts, aRootCerts);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPKIXCertChain* CPKIXCertChain::NewLC(RFs& aFs, 
											   const TPtrC8& aEncodedCerts,
											   const RPointerArray<CX509Certificate>& aRootCerts)
	{
	CPKIXCertChain* self = new(ELeave) CPKIXCertChain();
	CleanupStack::PushL(self);
	self->ConstructL(aFs, aEncodedCerts, aRootCerts);
	return self;
	}

CPKIXCertChain::CPKIXCertChain() :
	CPKIXCertChainBase()
	{
	}

void CPKIXCertChain::ConstructL(RFs& aFs, const TPtrC8& aEncodedCerts, TUid aClient)
	{
	iHelper = CPKIXCertChainHelper::NewL(aFs);
	CPKIXCertChainBase::ConstructL(iHelper->CertStore(), aEncodedCerts, aClient);
	}

void CPKIXCertChain::ConstructL(RFs& aFs, const TPtrC8& aEncodedCerts,
								const RPointerArray<CX509Certificate>& aRootCerts)
	{
	iHelper = CPKIXCertChainHelper::NewL(aFs);
	CPKIXCertChainBase::ConstructL(iHelper->CertStore(), aEncodedCerts, aRootCerts);
	}

EXPORT_C CPKIXCertChain::~CPKIXCertChain()
	{
	delete iHelper;
	}

EXPORT_C void CPKIXCertChain::ValidateL(CPKIXValidationResult& aValidationResult,
										const TTime& aValidationTime,
										TRequestStatus& aStatus)
	{
	// L in function name is to preserve BC - this can't leave
	iHelper->Validate(*this, aValidationResult, aValidationTime, aStatus);
	}

EXPORT_C void CPKIXCertChain::ValidateL(CPKIXValidationResult& aValidationResult,
										const TTime& aValidationTime,								 
										const CArrayPtr<HBufC>& aInitialPolicies,
										TRequestStatus& aStatus)
	{
	// L in function name is to preserve BC - this can't leave
	iHelper->Validate(*this, aValidationResult, aValidationTime, aInitialPolicies, aStatus);
	}

EXPORT_C void CPKIXCertChain::CancelValidate()
	{
	iHelper->CancelValidate();
	}

EXPORT_C TBool CPKIXCertChain::ChainHasRoot() const
	{
	// This method is necessary to preserve BC
	return CPKIXCertChainBase::ChainHasRoot();
	}

EXPORT_C void CPKIXCertChain::AddCertL(const TPtrC8& aEncodedCerts)
	{
	// This method is necessary to preserve BC
	CPKIXCertChainBase::AddCertL(aEncodedCerts);
	}

EXPORT_C const RPointerArray<TDesC>& CPKIXCertChain::SupportedCriticalExtensions() const
	{
	return CPKIXCertChainBase::SupportedCriticalExtensions();
	}

EXPORT_C void CPKIXCertChain::AddSupportedCriticalExtensionsL(const RPointerArray<TDesC>& aCriticalExtOids)
	{
	CPKIXCertChainBase::AddSupportedCriticalExtensionsL(aCriticalExtOids);
	}

EXPORT_C void CPKIXCertChain::RemoveSupportedCriticalExtensions(const RPointerArray<TDesC>& aCriticalExtOids)
	{
	CPKIXCertChainBase::RemoveSupportedCriticalExtensions(aCriticalExtOids);
	}

EXPORT_C void CPKIXCertChain::SetSupportedCriticalExtensionsL(const RPointerArray<TDesC>& aCriticalExtOids)
	{
	CPKIXCertChainBase::SetSupportedCriticalExtensionsL(aCriticalExtOids);
	}

EXPORT_C void CPKIXCertChain::ResetSupportedCriticalExtsToDefaultL()
	{
	CPKIXCertChainBase::ResetSupportedCriticalExtsToDefaultL();
	}
	
EXPORT_C void CPKIXCertChain::SetValidityPeriodCheckFatal(TBool aIsFatal)
	{
	CPKIXCertChainBase::SetValidityPeriodCheckFatal(aIsFatal);
	}
