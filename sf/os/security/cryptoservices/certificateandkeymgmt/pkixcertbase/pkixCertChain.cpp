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
#include "pkixcertchainao.h"
#include "pkixCons.h"
#include "pkixcertstate.h"
#include "pkixcerts.h"

//**********************************************************************************//
EXPORT_C CPKIXCertChainBase* CPKIXCertChainBase::NewL(MCertStore& aCertStore,
													  const TPtrC8& aEncodedCerts, 
													  const TUid aClient)
	{
	CPKIXCertChainBase* self = CPKIXCertChainBase::NewLC(aCertStore, aEncodedCerts, aClient);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPKIXCertChainBase* CPKIXCertChainBase::NewLC(MCertStore& aCertStore,
													   const TPtrC8& aEncodedCerts,
													   const TUid aClient)
	{
	CPKIXCertChainBase* self = new(ELeave) CPKIXCertChainBase();
	CleanupStack::PushL(self);
	self->ConstructL(aCertStore, aEncodedCerts, aClient);
	return self;
	}

EXPORT_C CPKIXCertChainBase* CPKIXCertChainBase::NewL(MCertStore& aCertStore,
													  const TPtrC8& aEncodedCerts,
													  const RPointerArray<CX509Certificate>& aRootCerts)
	{
	CPKIXCertChainBase* self = CPKIXCertChainBase::NewLC(aCertStore, aEncodedCerts, aRootCerts);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPKIXCertChainBase* CPKIXCertChainBase::NewLC(MCertStore& aCertStore,
													   const TPtrC8& aEncodedCerts,
													   const RPointerArray<CX509Certificate>& aRootCerts)
	{
	CPKIXCertChainBase* self = new(ELeave) CPKIXCertChainBase();
	CleanupStack::PushL(self);
	self->ConstructL(aCertStore, aEncodedCerts, aRootCerts);
	return self;
	}

EXPORT_C CPKIXCertChainBase::~CPKIXCertChainBase()
	{
	iIntermediateCerts.ResetAndDestroy();
	iIntermediateCerts.Close();
	iSupportedCriticalExts.Close();
	
	delete iActiveObject;
	}

//end of ctors & dtor

EXPORT_C void CPKIXCertChainBase::ValidateL(CPKIXValidationResultBase& aValidationResult,
										const TTime& aValidationTime,
										TRequestStatus& aStatus)
	{
	iActiveObject->ValidateL(aValidationResult, aValidationTime, NULL, aStatus);
	}

EXPORT_C void CPKIXCertChainBase::ValidateL(CPKIXValidationResultBase& aValidationResult,
										const TTime& aValidationTime,								 
										const CArrayPtr<HBufC>& aInitialPolicies,
										TRequestStatus& aStatus)
	{
	iActiveObject->ValidateL(aValidationResult, aValidationTime, &aInitialPolicies, aStatus);
	}

EXPORT_C void CPKIXCertChainBase::CancelValidate()
	{
	iActiveObject->CancelValidate();
	}

EXPORT_C TBool CPKIXCertChainBase::ChainHasRoot() const
	{
	return iChainHasRoot;
	}

EXPORT_C void CPKIXCertChainBase::AddCertL(const TPtrC8& aEncodedCerts)
	{
	AddIntermediateCertsL(aEncodedCerts);
	}

EXPORT_C const RPointerArray<TDesC>& CPKIXCertChainBase::SupportedCriticalExtensions() const
	{
	return iSupportedCriticalExts;
	}

EXPORT_C void CPKIXCertChainBase::AddSupportedCriticalExtensionsL(const RPointerArray<TDesC>& aCriticalExtOids)
	{
	TBool notPresent;
	TInt count = aCriticalExtOids.Count();
	for (TInt x=0; x < count; ++x)
		{
		notPresent = ETrue;
		for (TInt y=0; y < iSupportedCriticalExts.Count(); ++y)
			{
			if (*aCriticalExtOids[x] == *iSupportedCriticalExts[y])
				{
				notPresent = EFalse;
				break;
				}
			}
		if (notPresent)
			{
			iSupportedCriticalExts.AppendL(aCriticalExtOids[x]);
			}
		}
	}

EXPORT_C void CPKIXCertChainBase::RemoveSupportedCriticalExtensions(const RPointerArray<TDesC>& aCriticalExtOids)
	{
	TInt count = iSupportedCriticalExts.Count();
	TInt newCount = aCriticalExtOids.Count();
	if (count > 0)
		{
		for (TInt x=count - 1; x >= 0; --x)
			{
			for (TInt y=0; y < newCount; ++y)
				{
				if (*aCriticalExtOids[y] == *iSupportedCriticalExts[x])
					{
					iSupportedCriticalExts.Remove(x);
					break;
					}
				}
			}
		}
	}

EXPORT_C void CPKIXCertChainBase::SetSupportedCriticalExtensionsL(const RPointerArray<TDesC>& aCriticalExtOids)
	{
	iSupportedCriticalExts.Reset();
	AddSupportedCriticalExtensionsL(aCriticalExtOids);
	}

EXPORT_C void CPKIXCertChainBase::ResetSupportedCriticalExtsToDefaultL()
	{
	iSupportedCriticalExts.Reset();
	// standard X.509 extensions
	iSupportedCriticalExts.AppendL(&KExtendedKeyUsage);
	iSupportedCriticalExts.AppendL(&KPolicyMapping);		// RFC - MUST be non-critical
	iSupportedCriticalExts.AppendL(&KSubjectAltName);
	iSupportedCriticalExts.AppendL(&KKeyUsage);	
	iSupportedCriticalExts.AppendL(&KBasicConstraints);
	iSupportedCriticalExts.AppendL(&KNameConstraints);
	iSupportedCriticalExts.AppendL(&KPolicyConstraints);
	iSupportedCriticalExts.AppendL(&KCertPolicies);	
	iSupportedCriticalExts.AppendL(&KInhibitAnyPolicy);
	// Symbian critical extensions
	iSupportedCriticalExts.AppendL(&KDeviceIdListConstraint);
	iSupportedCriticalExts.AppendL(&KSidListConstraint);
	iSupportedCriticalExts.AppendL(&KVidListConstraint);
	iSupportedCriticalExts.AppendL(&KCapabilitiesConstraint);
	}


EXPORT_C void CPKIXCertChainBase::SetValidityPeriodCheckFatal(TBool aIsFatal)
	{
	iDateTimeCheckFatal = aIsFatal;
	}


EXPORT_C TBool CPKIXCertChainBase::ValidityPeriodCheckFatal() const
	{
	return iDateTimeCheckFatal;
	}

//private functions
//************************************************************************//

EXPORT_C CPKIXCertChainBase::CPKIXCertChainBase()
	: iChainHasRoot(EFalse), iDateTimeCheckFatal(ETrue)
	{
	}

EXPORT_C void CPKIXCertChainBase::ConstructL(MCertStore& aCertStore, const TPtrC8& aEncodedCerts, 
											 TUid aClient)
	{
	iActiveObject = CPKIXCertChainAO::NewL(aCertStore, *this, aClient);
	DoConstructL(aEncodedCerts);
	}

/**
 * Second-phase constructor
 * This constructor takes a set of root certificates we trust. We don't take into account 
 * the certificates in the certificate store because we are not interested in the
 * trust model of that store (where each certificates comes with a set of uid of the 
 * applications that trust this certificate)
 * this is consistent with the fact that in FindIssuer, we only look for non-root
 * certs in the store
 */
EXPORT_C void CPKIXCertChainBase::ConstructL(MCertStore& aCertStore, const TPtrC8& aEncodedCerts,
											 const RPointerArray<CX509Certificate>& aRootCerts)
	{
	iActiveObject = CPKIXCertChainAO::NewL(aCertStore, *this, aRootCerts);
	DoConstructL(aEncodedCerts);
	}

void CPKIXCertChainBase::DoConstructL(const TPtrC8& aEncodedCerts)
	{
	iChain = new(ELeave) CArrayPtrFlat<CX509Certificate> (1);

	TInt pos = 0;
	CX509Certificate* eeCert = CX509Certificate::NewLC(aEncodedCerts, pos);
	iChain->AppendL(eeCert);
	CleanupStack::Pop(eeCert);
	AddIntermediateCertsL(aEncodedCerts);
	ResetSupportedCriticalExtsToDefaultL();
	}

void CPKIXCertChainBase::AddIntermediateCertsL(const TPtrC8& aEncodedCerts)
	{
//decode aEncodedCerts, and add any that aren't self-signed
	TInt pos = 0;
	TInt end = aEncodedCerts.Length();
	while (pos < end)
		{
		CX509Certificate* decoded = CX509Certificate::NewLC(aEncodedCerts, pos);
		if (decoded->IsSelfSignedL())
			{
			// Then it's no use to us because it cannot be part of a chain with a
			// root certificate we trust.
			CleanupStack::PopAndDestroy(decoded);
			}
		else
			{
			User::LeaveIfError(iIntermediateCerts.Append(decoded));
			CleanupStack::Pop(decoded);
			}
		}
	}

void CPKIXCertChainBase::RemoveLastCerts(TInt aNumberOfCertsToRemove)
	{
	__ASSERT_DEBUG(iChain->Count() >= aNumberOfCertsToRemove,
		User::Panic(_L("CPKIXCertChain"), 1));

	// We don't have to change i because it is the count of the array that decreases
	for (TInt i = iChain->Count() - aNumberOfCertsToRemove; i < iChain->Count(); )
		{
		delete (*iChain)[i];
		iChain->Delete(i);
		}
	}

CArrayPtrFlat<CX509Certificate>& CPKIXCertChainBase::Chain()
	{
	__ASSERT_ALWAYS(iChain, User::Panic(_L("CPKICCertChainBase"), 1));
	return *iChain;
	}

const RPointerArray<CX509Certificate>& CPKIXCertChainBase::IntermediateCerts()
	{
	return iIntermediateCerts;
	}

TBool CPKIXCertChainBase::ChainHasRoot()
	{
	return iChainHasRoot;
	}

void CPKIXCertChainBase::SetChainHasRoot(TBool aHasRoot)
	{
	iChainHasRoot = aHasRoot;
	}
