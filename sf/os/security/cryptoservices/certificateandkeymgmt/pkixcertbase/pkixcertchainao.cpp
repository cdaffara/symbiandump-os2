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


#include "pkixcertchainao.h"
#include "pkixCons.h"
#include <x509keys.h>
#include <pkixcertchain.h>
#include "x509constraintext.h"

CPKIXCertChainAO* CPKIXCertChainAO::NewL(MCertStore& aCertStore,
										 CPKIXCertChainBase &aPKIXCertChain,
										 const RPointerArray<CX509Certificate>& aRootCerts)
	{
	CPKIXCertChainAO* self = new(ELeave) CPKIXCertChainAO(aCertStore, aPKIXCertChain);
	CleanupStack::PushL(self);
	self->ConstructL(aRootCerts);
	CleanupStack::Pop(self);
	return self;
	}

CPKIXCertChainAO* CPKIXCertChainAO::NewL(MCertStore& aCertStore, 
										 CPKIXCertChainBase &aPKIXCertChain,
										 const TUid aClient)
	{
	return new(ELeave) CPKIXCertChainAO(aCertStore, aPKIXCertChain, aClient);
	}

CPKIXCertChainAO::~CPKIXCertChainAO()
	{
	Cancel();

	delete iRoots;
	delete iBuilder;
	delete iCertsFromStoreRoots;
	}

CPKIXCertChainAO::CPKIXCertChainAO(MCertStore& aCertStore,
								   CPKIXCertChainBase &aPKIXCertChain)
	: CActive(EPriorityNormal), iCertStore(&aCertStore), iPKIXCertChain(aPKIXCertChain)
	{
	CActiveScheduler::Add(this);
	}

CPKIXCertChainAO::CPKIXCertChainAO(MCertStore& aCertStore, 
								   CPKIXCertChainBase &aPKIXCertChain,
								   const TUid aClient)
	: CActive(EPriorityNormal), iCertStore(&aCertStore),
	  iPKIXCertChain(aPKIXCertChain), iClient(aClient)
	{
	CActiveScheduler::Add(this);
	}

void CPKIXCertChainAO::ConstructL(const RPointerArray<CX509Certificate>& aRootCerts)
	{
	CPKIXCertsFromClient* roots = CPKIXCertsFromClient::NewLC(aRootCerts);
	iRoots = CPKIXChainBuilder::NewL();
	iRoots->AddSourceL(roots);
	CleanupStack::Pop(roots);
	}

void CPKIXCertChainAO::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EAddRoots:
			HandleEAddRootsL();
			break;

		case ERootsInitialized:
			HandleERootsInitializedL();
			break;

		case EBuildChainStart:
			HandleEBuildChainStartL();
			break;

		case EBuildChainAddCandidateEnd:
			HandleEBuildChainAddCandidateEndL();
			break;

		case EBuildChainCertsFromStoreBegin:
			HandleEBuildChainCertsFromStoreBeginL();
			break;

		case EBuildChainCertsFromStoreEnd:
			HandleEBuildChainCertsFromStoreEndL();
			break;

		case EAddCandidateIntermediateCertsEnd:
			HandleEAddCandidateIntermediateCertsEndL();
			break;

		case EValidateEnd:
			HandleEValidateEndL();
			break;

		default:
			User::Panic(_L("CPKIXCertChainAO"), 1);
			break;
		}
	}

TInt CPKIXCertChainAO::RunError(TInt aError)
{
	iPKIXCertChain.RemoveLastCerts(iNumberOfAddedCertificates);
	iNumberOfAddedCertificates = 0;

	delete iRoots;
	iRoots = 0;
		
	delete iBuilder;
	iBuilder = 0;
	delete iCertsFromStoreRoots;
	iCertsFromStoreRoots = 0;

	iValidationResult->RemovePolicies();

	User::RequestComplete(iOriginalRequestStatus, aError);
	return KErrNone;
}

/**
 * Creates a list of all the certificates retrieved from the store based on the filter passed.  
 */

void CPKIXCertChainAO::HandleEAddRootsL()
	{
	__ASSERT_DEBUG(!iCertsFromStoreRoots, User::Panic(_L("CPKICCertChainAO"), 1));
	iCertsFromStoreRoots = CPKIXCertsFromStore::NewL(*iCertStore, iClient);
	iCertsFromStoreRoots->Initialize(iStatus);
	iState = ERootsInitialized;	
	SetActive();
	}

/**
 * Adds the list of certificates retrieved from the store, iRoots (CPKIXChainBuilder)
 * maintains a templatized list of all the certificates in MPKIXCertSource format.
 */

void CPKIXCertChainAO::HandleERootsInitializedL()
	{
	iRoots->AddSourceL(iCertsFromStoreRoots);
	// Ownership has been passed to iRoots
	iCertsFromStoreRoots = 0;
	iState = EBuildChainStart;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CPKIXCertChainAO::HandleEBuildChainStartL()
	{
	if ( false == iPKIXCertChain.ChainHasRoot())
		{
		if (iPKIXCertChain.Chain().Count() == 0)
			{
			iState = EValidateEnd;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		else
			{
			//1) look for an issuer that's a root
			iRoots->AddIssuer(iNumberOfAddedCertificates, iAddIssuerResult, iPKIXCertChain.Chain(), iStatus);
			iState = EBuildChainAddCandidateEnd;
			}
		}
	else
		{
		// This is the correct state as at this point the chain of certificate has been build upto a
		// root certificate.
		iState = EValidateEnd;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	SetActive();
	}

void CPKIXCertChainAO::HandleEBuildChainAddCandidateEndL()
	{
	if (iAddIssuerResult)
		{
		iPKIXCertChain.SetChainHasRoot(ETrue);
		iState = EValidateEnd;
		}
	else
		{
		//2) look for a non-root issuer in intermediate certs
		iBuilder = CPKIXChainBuilder::NewL();
		
		CPKIXCertsFromClient* serverCerts = CPKIXCertsFromClient::NewLC(iPKIXCertChain.IntermediateCerts());
		iBuilder->AddSourceL(serverCerts);
		CleanupStack::Pop(serverCerts);

		iState = EBuildChainCertsFromStoreBegin;
		}

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CPKIXCertChainAO::HandleEBuildChainCertsFromStoreBeginL()
	{
	//3) look for a non-root issuer in the store
	iCertsFromStoreRoots = CPKIXCertsFromStore::NewL(*iCertStore);
	iCertsFromStoreRoots->Initialize(iStatus);
	iState = EBuildChainCertsFromStoreEnd;
	SetActive();
	}

void CPKIXCertChainAO::HandleEBuildChainCertsFromStoreEndL()
	{
	iBuilder->AddSourceL(iCertsFromStoreRoots);
	iCertsFromStoreRoots = 0;

	iBuilder->AddIssuer(iNumberOfAddedCertificates, iAddIssuerResult, iPKIXCertChain.Chain(), iStatus);
	iState = EAddCandidateIntermediateCertsEnd;
	SetActive();
	}

void CPKIXCertChainAO::HandleEAddCandidateIntermediateCertsEndL()
	{
	if (iAddIssuerResult)
		{
		// cert is a pointer to something we don't own
		CX509Certificate* cert = iPKIXCertChain.Chain().At(iPKIXCertChain.Chain().Count() - 1);
		
		/* If the issuer is not a self signed certificate then it cannot be trusted anchor for the chain 
		 * validation process, this means that we restart the certification validation process.
		 */ 
		
		if (!(cert->IsSelfSignedL()))
			{
			iState = EBuildChainStart;	
			}
		else
			{
			iState = EValidateEnd;			
			}
		}
	else
		{
		iState = EValidateEnd;	
		}

	delete iBuilder;
	iBuilder = 0;

	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CPKIXCertChainAO::HandleEValidateEndL()
	{
	InitParamsL();
	
	__ASSERT_DEBUG(iValidationResult, User::Panic(_L("CPKICCertChainAO"), 1));
	DoValidateL(*iValidationResult, iValidationTime, iInitialPolicies);

	User::RequestComplete(iOriginalRequestStatus, KErrNone);
	}

void CPKIXCertChainAO::DoCancel()
	{
	delete iRoots;
	iRoots = 0;

	delete iBuilder;
	iBuilder = 0;

	delete iCertsFromStoreRoots;
	iCertsFromStoreRoots = 0;

	User::RequestComplete(iOriginalRequestStatus, KErrCancel);
	}

void CPKIXCertChainAO::ValidateL(CPKIXValidationResultBase& aValidationResult,
								 const TTime& aValidationTime,								 
								 const CArrayPtr<HBufC>* aInitialPolicies,
								 TRequestStatus& aStatus)
	{
	aValidationResult.Reset();
	iValidationResult = &aValidationResult;
	iValidationTime = aValidationTime;
	iInitialPolicies = aInitialPolicies;
	iOriginalRequestStatus = &aStatus;
	iNumberOfAddedCertificates = 0;

	__ASSERT_ALWAYS(!IsActive(), User::Panic(_L("CPKICCertChainAO"), 1));

	if (!iRoots)
		{
		// If iRoots is 0, it means that the caller gave a uid and that
		// we must retrieve the trusted certificates from the different
		// stores
		iRoots = CPKIXChainBuilder::NewL();
		iState = EAddRoots;
		}
	else
		{
		// The caller gave a set of certificates it trusts,
		// so we don't have to retrieve anything from the stores
		iState = EBuildChainStart;
		}

	aStatus = KRequestPending;
	TRequestStatus *status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CPKIXCertChainAO::CancelValidate()
	{
	Cancel();
	}

void CPKIXCertChainAO::InitParamsL()
/*
this function initialises signing key parameters for the certificates
-only DSA needs these at present
-we get the signing key, from the spki of the issuer
-if it's dsa, we look for params here
	-if we find them we initialise the cert with them
	-otherwise, we look in the issuer's issuer
		-if we don't find them there we give up.
*/
	{
	
	// If the root is DSA signed, set its parameters

	TInt count = iPKIXCertChain.Chain().Count();
	
	CX509Certificate* current = iPKIXCertChain.Chain().At(count-1);
	TAlgorithmId signingAlgorithm = current->SigningAlgorithm().AsymmetricAlgorithm().Algorithm();
	
	if (signingAlgorithm == EDSA)
		{
		
		const CSubjectPublicKeyInfo& key = current->PublicKey();
		SetParamsL(*current, key.EncodedParams());
		
		}
	
	// Also the rest of the chain
	
	for (TInt i = count - 2; i >= 0; i--)
		{
		
		current = iPKIXCertChain.Chain().At(i);
		TAlgorithmId signingAlgorithm = current->SigningAlgorithm().AsymmetricAlgorithm().Algorithm();
		
		if (signingAlgorithm == EDSA)
			{
			
			// Look down the chain for parameters
			
			for (TInt j = i+1; j < count; j++)
				{
				
				CX509Certificate* issuer = iPKIXCertChain.Chain().At(j);
				const CSubjectPublicKeyInfo& key = issuer->PublicKey();
				if (key.EncodedParams() != KNullDesC8 && key.AlgorithmId() == EDSA)
					{
					SetParamsL(*current, key.EncodedParams());
					break;
					}
				
				}
			
			}
		
		}
	}

void CPKIXCertChainAO::SetParamsL(CX509Certificate& aCert, const TPtrC8& aEncodedParams)
	{
	TX509KeyFactory factory;
	CDSAParameters* theDSAParams = factory.DSAParametersL(aEncodedParams);
	CleanupStack::PushL(theDSAParams);
	
	CSigningKeyParameters* params = CSigningKeyParameters::NewLC();
	params->SetDSAParamsL(*theDSAParams);
	
	aCert.SetParametersL(*params);

	CleanupStack::PopAndDestroy(2, theDSAParams);
	}

void CPKIXCertChainAO::DoValidateL(CPKIXValidationResultBase& aValidationResult,
								 const TTime& aValidationTime, 
								 const CArrayPtr<HBufC>* aInitialPolicies)
	{
	if (!iPKIXCertChain.ChainHasRoot())
		{
		aValidationResult.SetError(EChainHasNoRoot, 0);
		}
	else
		{
		CPKIXValidationState* state = CPKIXValidationState::NewLC(aValidationTime, iPKIXCertChain.Chain().Count(), aInitialPolicies);
		TRAPD(err, ProcessCertsL(*state, aValidationResult));
		//a leave here means either:
		//	-a validation error, in which case we've set the error field in result, or
		//	-some other error (e.g. OOM) in which case error is still EChainHasNoRoot
		if ((err != KErrNone) && ((aValidationResult.Error().iReason) == EChainHasNoRoot))
			//then we left with a non-validation-related error, so leave again...
			{
			User::Leave(err);
			}
		CleanupStack::PopAndDestroy(state);
		}
	}

// ProcessCertsL: This function validates a complete certificate 
//                chain. If an error occurs in this function the function
//                SetErrorAndLeaveL must be called. 
// 
// Note Do not use SetErrorAndLeaveL with EChainHasNoRoot (see TRAP code in 
// CPKIXCertChainAO::DoValidateL )             
void CPKIXCertChainAO::ProcessCertsL(CPKIXValidationState& aState,
									 CPKIXValidationResultBase& aResult) const
	{
	TPKIXPolicyConstraint policy(aState, aResult);
	TPKIXNameConstraint name(aState, aResult);
	TPKIXBasicConstraint basic(aState, aResult);
	TPKIXKeyUsageConstraint keyUsage(aState, aResult);
	for (; aState.iPos >= 0; aState.iPos--)
		{
		aState.iMaxPathLength--;
		if (aState.iMaxPathLength < aState.iPos)
			{
			aResult.SetErrorAndLeaveL(EPathTooLong, aState.iPos);
			}
		const CX509Certificate* current = iPKIXCertChain.Chain().At(aState.iPos);
		CCertificateValidationWarnings* certWarnings = CCertificateValidationWarnings::NewLC(aState.iPos);
		aResult.AppendCertificateValidationObjectL(*certWarnings);
		CleanupStack::Pop(certWarnings);
		CriticalExtsL(aState, *current);
		CheckCriticalExtsL(aState, aResult);
		CheckSignatureAndNameL(*current, aState, aResult);
		//!!!!NO!!checks for revocation at this time!!
		
		if (!(current->ValidityPeriod().Valid(aState.iValidationTime)))
			{
			//validity period invalid, now check how to report this
			if (iPKIXCertChain.ValidityPeriodCheckFatal())
				{
				aResult.SetErrorAndLeaveL(EDateOutOfRange, aState.iPos);
				}
			else
				{
				aResult.AppendWarningL(TValidationStatus(EDateOutOfRange, aState.iPos));
				}
			}
		
		policy.CheckCertPoliciesL(*current);
		name.CheckNameConstraintsL(*current);
		keyUsage.CheckKeyUsageL(*current);
		if (aState.iPos < (iPKIXCertChain.Chain().Count() - 1))
			{
			basic.CheckCertSubjectTypeL(*current);
			}
		basic.UpdatePathLengthConstraintsL(*current);
		name.UpdateNameConstraintsL(*current);
		policy.UpdatePolicyConstraintsL(*current);
		aState.iCriticalExts->Reset();
		}
	policy.FinishPolicyCheckL();
	//*copy* all policies from aState.iAuthorityConstrainedPolicies into aResult.iPolicies
	TInt policyCount = aState.iAuthorityConstrainedPolicies->Count();
	for (TInt i = 0; i < policyCount; i ++)
		{
		CX509CertPolicyInfo* policyInfo = CX509CertPolicyInfo::NewLC(*(aState.iAuthorityConstrainedPolicies->At(i)));
		aResult.AppendPolicyL(*policyInfo);
		CleanupStack::Pop(policyInfo);
		}

	aResult.SetError(EValidatedOK, 0);
	}

void CPKIXCertChainAO::CriticalExtsL(CPKIXValidationState& aState, 
									 const CX509Certificate& aCert) const
	{
	const CArrayPtrFlat<CX509CertExtension>& exts = aCert.Extensions();
	TInt count = exts.Count();
	for (TInt i = 0; i < count; i++)
		{
		CX509CertExtension* ext = exts.At(i);
		if (ext->Critical())
			{
			aState.iCriticalExts->AppendL(ext);
			}
		}	 
	}

void CPKIXCertChainAO::CheckSignatureAndNameL(const CX509Certificate& aCert, CPKIXValidationState& aState, 
											CPKIXValidationResultBase& aResult) const
	{
	TInt issuerPos = aState.iPos + 1;
	if (issuerPos == iPKIXCertChain.Chain().Count())
		//then it's the root
		{
		if (aCert.IssuerName().ExactMatchL(aCert.SubjectName()))
			//then it claims to be self signed, sig must verify
			{
			if (!(aCert.VerifySignatureL(aCert.PublicKey().KeyData())))
				{
				aResult.SetErrorAndLeaveL(ESignatureInvalid, aState.iPos);
				}
			}
		else
			//we generate a warning
			{
			aResult.AppendWarningL(TValidationStatus(ERootCertNotSelfSigned, aState.iPos));
			}
		}
	else
		//then it isn't the root: so names must chain & sigs must verify
		{
		const CX509Certificate* issuer = iPKIXCertChain.Chain().At(issuerPos);
		if (!(aCert.IssuerName().ExactMatchL(issuer->SubjectName())))
			{
			aResult.SetErrorAndLeaveL(ENamesDontChain, aState.iPos);
			}
		if (!(aCert.VerifySignatureL(issuer->PublicKey().KeyData())))
			{
			aResult.SetErrorAndLeaveL(ESignatureInvalid, aState.iPos);
			}
		}
	}

void CPKIXCertChainAO::CheckCriticalExtsL(CPKIXValidationState& aState, CPKIXValidationResultBase& aResult) const
	{
	TBool foundUnrecognisedCritExt;
	
	// retrieve the supported list of critical extensions. If a critical extension is found whose OID matches an 
	// element in this set then certificate validation shall treat this as a warning instead of an error.
	const RPointerArray<TDesC>& supportedCritExt = iPKIXCertChain.SupportedCriticalExtensions();
	
	TInt count = aState.iCriticalExts->Count();
	TInt supportedCount = supportedCritExt.Count();
	for (TInt i = 0; i < count; i++)
		{
		foundUnrecognisedCritExt = ETrue;
		const CX509CertExtension* ext = aState.iCriticalExts->At(i);
		const TPtrC& extName = ext->Id();
		
		for (TInt j = 0; j < supportedCount; ++j)
			{
			if (extName == *supportedCritExt[j])
				{
				foundUnrecognisedCritExt = EFalse;
				HBufC* oid = extName.AllocLC();
				aResult.AppendCriticalExtensionWarningL(*oid);
				CleanupStack::Pop(oid);
				break;
				}
			}			
		
		if (extName == KExtendedKeyUsage)
			{
			aResult.AppendWarningL(TValidationStatus(ECriticalExtendedKeyUsage, aState.iPos));
			}
		else if (extName == KPolicyMapping)
			{
			aResult.AppendWarningL(TValidationStatus(ECriticalPolicyMapping, aState.iPos));
			}
		else if (extName == KInhibitAnyPolicy)
			{
			//ignore this in the same way
			}
		else if (extName == KDeviceIdListConstraint)
			{
			aResult.AppendWarningL(TValidationStatus(ECriticalDeviceId, aState.iPos));
			}
		else if(extName == KSidListConstraint)
			{
			aResult.AppendWarningL(TValidationStatus(ECriticalSid, aState.iPos));
			}
		else if(extName == KVidListConstraint)
			{
			aResult.AppendWarningL(TValidationStatus(ECriticalVid, aState.iPos));
			}
		else if(extName == KCapabilitiesConstraint)
			{
			aResult.AppendWarningL(TValidationStatus(ECriticalCapabilities, aState.iPos));
			}
		
		if (foundUnrecognisedCritExt)
			{
			aResult.SetErrorAndLeaveL(EUnrecognizedCriticalExtension, aState.iPos);
			}			
		
		}
	}
