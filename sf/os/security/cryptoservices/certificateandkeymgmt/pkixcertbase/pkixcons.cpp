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


#include "pkixCons.h"

//PKIX constraint
//only function is remove
TPKIXConstraint::TPKIXConstraint(	CPKIXValidationState& aState, 
									CPKIXValidationResultBase& aResult)
	:iState(aState), iResult(aResult)
	{
	}

void TPKIXConstraint::Remove(CArrayPtrFlat<CX509CertExtension>& aCriticalExtensions, const TDesC& aOID)
	{
	TInt count = aCriticalExtensions.Count();
	for (TInt i = 0; i < count; i++)
		{
		CX509CertExtension* ext = aCriticalExtensions.At(i);
		if (ext->Id() == aOID)
			{
			aCriticalExtensions.Delete(i);
			break;
			}
		}
	}

//policy constraint
//public functions
TPKIXPolicyConstraint::TPKIXPolicyConstraint(	CPKIXValidationState& aState, 
												CPKIXValidationResultBase& aResult)
	:TPKIXConstraint(aState, aResult)
	{
	}

void TPKIXPolicyConstraint::CleanupPolicyInfoArray(TAny* aPolicies)
	{
	CArrayPtrFlat<CX509CertPolicyInfo>* array = REINTERPRET_CAST(CArrayPtrFlat<CX509CertPolicyInfo>*, aPolicies);
	array->ResetAndDestroy();
	delete array;
	}

void TPKIXPolicyConstraint::CheckCertPoliciesL(const CX509Certificate& aCert)
	{
	const CX509CertExtension* ext =  aCert.Extension(KCertPolicies);
	CX509CertPoliciesExt* policyExt = NULL;
	if (ext)
		{
		policyExt = CX509CertPoliciesExt::NewLC(ext->Data());
		}
	if (iState.iPos > iState.iPolicyRequired)
		{
		if (!(policyExt))
			{
			iResult.SetErrorAndLeaveL(ERequiredPolicyNotFound, iState.iPos);
			}
		const CArrayPtrFlat<CX509CertPolicyInfo>& policies = policyExt->Policies();
		if ((iState.iUserPolicies->Count() == 0) || (PolicyIsPresentL(policies, *iState.iUserPolicies)))
			{
			}
		else
			{
			iResult.SetErrorAndLeaveL(ERequiredPolicyNotFound, iState.iPos);
			}
		}
	if (!policyExt)
		{
		if (!iState.iAnyAuthorityPolicy)
			{
			iState.iAuthorityConstrainedPolicies->ResetAndDestroy();//AP becomes NULL 
			}
		}
	else
		{
		IntersectCertPoliciesL(*policyExt);
		if (ext->Critical())
			{
			TInt count = iState.iAuthorityConstrainedPolicies->Count();
			for (TInt i = 0; i < count; i++)
				{
				const CX509CertPolicyInfo* policy = iState.iAuthorityConstrainedPolicies->At(i);
				if (policy->Qualifiers().Count() > 0)
					{
					iResult.AppendWarningL(TValidationStatus(ECriticalCertPoliciesWithQualifiers, i));
					break;
					}
				}
			Remove(*(iState.iCriticalExts), KCertPolicies);
			}
		CleanupStack::PopAndDestroy();//policyExt
		}		
	}

void TPKIXPolicyConstraint::IntersectCertPoliciesL(const CX509CertPoliciesExt& aPolicyExt)
	{
	//1 intersect AP and CP, assign result to newAP
	CArrayPtrFlat<CX509CertPolicyInfo>* newAP;
	TInt certPolicyCount = aPolicyExt.Policies().Count();
	if (iState.iAnyAuthorityPolicy)
		{
		newAP = new(ELeave) CArrayPtrFlat<CX509CertPolicyInfo> (1);
		TCleanupItem cleanupPolicies(CleanupPolicyInfoArray, newAP);
		CleanupStack::PushL(cleanupPolicies);
		for (TInt i = 0; i < certPolicyCount; i++)
			{
			CX509CertPolicyInfo* info = CX509CertPolicyInfo::NewLC(*(aPolicyExt.Policies().At(i)));
			newAP->AppendL(info);
			CleanupStack::Pop();
			}
		iState.iAnyAuthorityPolicy = EFalse;
		}
	else
		{			
		newAP = IntersectionLC(aPolicyExt.Policies(), *(iState.iAuthorityConstrainedPolicies));
		}

	TInt mappedCount = iState.iMappedPolicies->Count(); 
	for (TInt i = 0; i < mappedCount; i++)
		{
		CX509PolicyMapping* mapping = iState.iMappedPolicies->At(i);
		TInt apCount = iState.iAuthorityConstrainedPolicies->Count();
	//2 for each mapping in MP, if issuer is in AP and subject is in CP, add subject to newAP
		for (TInt j = 0; j < apCount; j++)
			{
			CX509CertPolicyInfo* aCP = iState.iAuthorityConstrainedPolicies->At(j);
			if (aCP->Id() == mapping->IssuerPolicy())
				{
				for (TInt k = 0; k < certPolicyCount; k++)
					{
					CX509CertPolicyInfo* cp = aPolicyExt.Policies().At(k);
					if (mapping->SubjectPolicy() == cp->Id())
						{
						CX509CertPolicyInfo* newPolicy = CX509CertPolicyInfo::NewLC(*cp);
						newAP->AppendL(newPolicy);
						CleanupStack::Pop();
						}
					}
				}
			}
		}
	//new acceptable policies  = intersection
	iState.iAuthorityConstrainedPolicies->ResetAndDestroy(); 
	delete iState.iAuthorityConstrainedPolicies;
	iState.iAuthorityConstrainedPolicies = newAP;
	CleanupStack::Pop();//newAP
	}

void TPKIXPolicyConstraint::UpdatePolicyConstraintsL(const CX509Certificate& aCert)
	{
	//get mapping ext
	const CX509CertExtension* ext = aCert.Extension(KPolicyMapping);
	if ((iState.iPos <= iState.iPolicyMapping) && (ext))
		{
		CX509PolicyMappingExt* policyMappingExt = CX509PolicyMappingExt::NewLC(ext->Data());
		const CArrayPtrFlat<CX509PolicyMapping>& mappings = policyMappingExt->Mappings();
		//for each policy mapping
		TInt countM = mappings.Count();
		for (TInt i = 0; i < countM; i++)
			{
			CX509PolicyMapping* mapping = mappings.At(i);
			CX509PolicyMapping* newMapping = CX509PolicyMapping::NewLC(*mapping);
			iState.iMappedPolicies->AppendL(newMapping);
			CleanupStack::Pop();
			TInt uCount = iState.iUserPolicies->Count();
			for (TInt j = 0; j < uCount; j++)
				{
				HBufC* userPolicy = iState.iUserPolicies->At(j);
				if (newMapping->IssuerPolicy() == *userPolicy)
					{
					HBufC* newUP = newMapping->SubjectPolicy().AllocL();
					CleanupStack::PushL(newUP);
					iState.iUserPolicies->AppendL(newUP);
					CleanupStack::Pop();
					break;
					}
				}
			}
		CleanupStack::PopAndDestroy();//mapping ext
		}
	iState.iPolicyMapping --;
	iState.iPolicyRequired --;
	//get constraints 
	ext = aCert.Extension(KPolicyConstraints);
	if ( ext )
		{
		CX509PolicyConstraintsExt* policyConstraintsExt = CX509PolicyConstraintsExt::NewLC(ext->Data());
		UpdateConstraint(policyConstraintsExt->InhibitPolicyMapping(), iState.iPolicyMapping);
		UpdateConstraint(policyConstraintsExt->ExplicitPolicyRequired(), iState.iPolicyRequired);
		CleanupStack::PopAndDestroy();//constraint ext
		//remove it from the 'critical list'
		if (ext->Critical())
			{
			Remove(*(iState.iCriticalExts), KPolicyConstraints);
			}	
		}
	}

//private functions
TBool TPKIXPolicyConstraint::PolicyIsPresentL(	const CArrayPtrFlat<CX509CertPolicyInfo>& aPolicies,
												const CArrayPtr<HBufC>& aAcceptablePolicies)
	{
	TInt certCount = aPolicies.Count();
	TInt chainCount = aAcceptablePolicies.Count();
	for (TInt i = 0; i < certCount; i++)
		{
		CX509CertPolicyInfo* certPolicy = aPolicies.At(i);
		for (TInt j = 0; j < chainCount; j++)
			{
			HBufC* chainPolicy = aAcceptablePolicies.At(j);
			if (certPolicy->Id() == chainPolicy->Des())
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

void TPKIXPolicyConstraint::UpdateConstraint(const TX509PolicyConstraint& aConstraint, TInt& aCountdown)
	{
	if (aConstraint.iRequired)
		{
		if (aConstraint.iCountdown < aCountdown)
			aCountdown = aConstraint.iCountdown;
		}
	}

void TPKIXPolicyConstraint::FinishPolicyCheckL()
	{
	if (iState.iUserConstrainedPolicies)
		{
		TBool passed = EFalse;
		if (!(iState.iAnyAuthorityPolicy))
			{//policy from user policies must be in authority policy set
			if ((PolicyIsPresentL(*(iState.iAuthorityConstrainedPolicies), *(iState.iUserPolicies))))
				{
				passed = ETrue;
				}
			}		
		if (!passed)
			{
			iResult.SetErrorAndLeaveL(ERequiredPolicyNotFound, iState.iPos);
			}
		}
	}

CArrayPtrFlat<CX509CertPolicyInfo>* TPKIXPolicyConstraint::IntersectionLC(
									const CArrayPtrFlat<CX509CertPolicyInfo>& aFirst,
									const CArrayPtrFlat<CX509CertPolicyInfo>& aSecond)
	//constructs an array of certificate policy objects, 
	//populating it with policies that occur in both of the array parameters
	{
	CArrayPtrFlat<CX509CertPolicyInfo>* inter = new(ELeave) CArrayPtrFlat<CX509CertPolicyInfo> (1);
	TCleanupItem cleanupPolicies(CleanupPolicyInfoArray, inter);
	CleanupStack::PushL(cleanupPolicies);
	TInt count1 = aFirst.Count();
	TInt count2 = aSecond.Count();
	for (TInt i = 0; i < count1; i++)
		{
		CX509CertPolicyInfo* policy1 = aFirst.At(i);
		for (TInt j = 0; j < count2; j++)
			{
			CX509CertPolicyInfo* policy2 = aSecond.At(j);
			if (policy1->Id() == policy2->Id())
				{
				CX509CertPolicyInfo* info = CX509CertPolicyInfo::NewLC(*policy1);
				inter->AppendL(info);
				CleanupStack::Pop();
				}
			}
		}
	return inter;
	}

//name constraint
//public functions
TPKIXNameConstraint::TPKIXNameConstraint(	CPKIXValidationState& aState, 
											CPKIXValidationResultBase& aResult)
	:TPKIXConstraint(aState, aResult)
	{
	}

void TPKIXNameConstraint::CheckNameConstraintsL(const CX509Certificate& aCert) 
	{
	//*do the subject name
	if (NameIsPresentL(aCert.SubjectName(), *(iState.iExcludedDNSubtrees)))
		{
		iResult.SetErrorAndLeaveL(ENameIsExcluded, iState.iPos);
		}
	TInt pCount = iState.iPermittedDNSubtrees->Count();
	if ((pCount > 0) && (!(NameIsPresentL(aCert.SubjectName(), *(iState.iPermittedDNSubtrees)))))
		{
		iResult.SetErrorAndLeaveL(ENameNotPermitted, iState.iPos);
		}
	//*do the alt name
	const CX509CertExtension* ext = aCert.Extension(KSubjectAltName);
	if (ext)
		{
		CX509AltNameExt* altNameExt = CX509AltNameExt::NewLC(ext->Data());
		const CArrayPtrFlat<CX509GeneralName>& altName = altNameExt->AltName();
		TInt count = altName.Count();
		for (TInt i = 0; i < count; i++)
			{
			const CX509GeneralName* gN = altName.At(i);
			switch (gN->Tag())
				{
				case EX509DirectoryName://X500DN
					{
					const CX500DistinguishedName* dN = CX500DistinguishedName::NewLC(gN->Data());
					if (NameIsPresentL(*dN, *(iState.iExcludedDNSubtrees)))
						{
						iResult.SetErrorAndLeaveL(ENameIsExcluded, iState.iPos);
						}
					if ((pCount > 0) && (!(NameIsPresentL(*dN, *(iState.iPermittedDNSubtrees)))))
						{
						iResult.SetErrorAndLeaveL(ENameNotPermitted, iState.iPos);
						}
					CleanupStack::PopAndDestroy();
					}			
					break;
				case EX509RFC822Name://IA5String
					{
					const CX509RFC822Name* name = CX509RFC822Name::NewLC(gN->Data());
					if (NameIsPresent(*name, *(iState.iExcludedRFC822Subtrees)))
						{
						iResult.SetErrorAndLeaveL(ENameIsExcluded, iState.iPos);
						}
					if ((iState.iPermittedRFC822Subtrees->Count() > 0) && (!(NameIsPresent(*name, *(iState.iPermittedRFC822Subtrees)))))
						{
						iResult.SetErrorAndLeaveL(ENameNotPermitted, iState.iPos);
						}
					CleanupStack::PopAndDestroy();
					}
					break;
				case EX509URI://IA5String
					{
					const CX509IPBasedURI* name = CX509IPBasedURI::NewLC(gN->Data());
					const CX509DNSName& domain = name->Host();
					if (NameIsPresent(domain, *(iState.iExcludedDNSNameSubtrees)))
						{
						iResult.SetErrorAndLeaveL(ENameIsExcluded, iState.iPos);
						}
					if ((iState.iPermittedDNSNameSubtrees->Count() > 0) && (!(NameIsPresent(domain, *(iState.iPermittedDNSNameSubtrees)))))
						{
						iResult.SetErrorAndLeaveL(ENameNotPermitted, iState.iPos);
						}
					CleanupStack::PopAndDestroy();
					}
					break;
				case EX509DNSName://IA5String
					{
					const CX509DNSName* name = CX509DNSName::NewLC(gN->Data());
					if (NameIsPresent(*name, *(iState.iExcludedDNSNameSubtrees)))
						{
						iResult.SetErrorAndLeaveL(ENameIsExcluded, iState.iPos);
						}
					if ((iState.iPermittedDNSNameSubtrees->Count() > 0) && (!(NameIsPresent(*name, *(iState.iPermittedDNSNameSubtrees)))))
						{
						iResult.SetErrorAndLeaveL(ENameNotPermitted, iState.iPos);
						}
					CleanupStack::PopAndDestroy();
					}
					break;
				case EX509IPAddress://octet string
					{
					const CX509IPAddress* name = CX509IPAddress::NewLC(gN->Data());
					if (NameIsPresent(*name, *(iState.iExcludedIPAddressSubtrees)))
						{
						iResult.SetErrorAndLeaveL(ENameIsExcluded, iState.iPos);
						}
					if ((iState.iPermittedIPAddressSubtrees->Count() > 0) && (!(NameIsPresent(*name, *(iState.iPermittedIPAddressSubtrees)))))
						{
						iResult.SetErrorAndLeaveL(ENameNotPermitted, iState.iPos);
						}
					CleanupStack::PopAndDestroy();
					}
					break;
				}
			}//end of for loop
		//we've handled this now, so can remove it from the critical list
		Remove(*(iState.iCriticalExts), KSubjectAltName);
		CleanupStack::PopAndDestroy();//altNameExt
		}//end of if(ext)
	}

void TPKIXNameConstraint::UpdateNameConstraintsL(const CX509Certificate& aCert)
	{
	const CX509CertExtension* ext = aCert.Extension(KNameConstraints);
	if (ext)
		{
		CX509NameConstraintsExt* nameCons = CX509NameConstraintsExt::NewLC(ext->Data());
		const CArrayPtrFlat<CX509GeneralSubtree>& excSubtrees = nameCons->ExcludedSubtrees();
		TInt count = excSubtrees.Count();
		for (TInt i = 0; i < count; i++)
			{
			const CX509GeneralSubtree* subtree = excSubtrees.At(i);
			const CX509GeneralName& gN = subtree->Name();
			switch (gN.Tag())
				{
				case EX509DirectoryName://X500DN
					{
					CX500DistinguishedName* name = CX500DistinguishedName::NewLC(gN.Data());
					iState.iExcludedDNSubtrees->AppendL(name);
					CleanupStack::Pop();
					}			
					break;
				case EX509RFC822Name://IA5String
					{
					CX509RFC822Name* name = CX509RFC822Name::NewLC(gN.Data());
					iState.iExcludedRFC822Subtrees->AppendL(name);
					CleanupStack::Pop();
					}
					break;
				case EX509URI://IA5String
					{
					CX509IPBasedURI* name = CX509IPBasedURI::NewLC(gN.Data());
					CX509DNSName* domain = CX509DNSName::NewLC(name->Host());
					iState.iExcludedDNSNameSubtrees->AppendL(domain);
					CleanupStack::Pop();
					CleanupStack::PopAndDestroy();
					}
					break;
				case EX509DNSName://IA5String
					{
					CX509DNSName* name = CX509DNSName::NewLC(gN.Data());
					iState.iExcludedDNSNameSubtrees->AppendL(name);
					CleanupStack::Pop();
					}
					break;
				case EX509IPAddress://octet string
					{
					CX509IPSubnetMask* name = CX509IPSubnetMask::NewLC(gN.Data());
					iState.iExcludedIPAddressSubtrees->AppendL(name);
					CleanupStack::Pop();
					}	
					break;
				default:
					{
					User::Leave(KErrNotSupported);
					}
					break;
				}
			}//end of for loop
		const CArrayPtrFlat<CX509GeneralSubtree>& perSubtrees = nameCons->PermittedSubtrees();
		count = perSubtrees.Count();
		for (TInt j = 0; j < count; j++)
			{
			const CX509GeneralSubtree* subtree = perSubtrees.At(j);
			const CX509GeneralName& gN = subtree->Name();
			switch (gN.Tag())
				{
				case EX509DirectoryName://X500DN
					{
					CX500DistinguishedName* name = CX500DistinguishedName::NewLC(gN.Data());
					iState.iPermittedDNSubtrees->AppendL(name);
					CleanupStack::Pop();
					}			
					break;
				case EX509RFC822Name://IA5String
					{
					CX509RFC822Name* name = CX509RFC822Name::NewLC(gN.Data());
					iState.iPermittedRFC822Subtrees->AppendL(name);
					CleanupStack::Pop();
					}
					break;
				case EX509URI://IA5String
					{
					CX509IPBasedURI* name = CX509IPBasedURI::NewLC(gN.Data());
					CX509DNSName* domain = CX509DNSName::NewLC(name->Host());
					iState.iPermittedDNSNameSubtrees->AppendL(domain);
					CleanupStack::Pop();
					CleanupStack::PopAndDestroy();
					}
					break;
				case EX509DNSName://IA5String
					{
					CX509DNSName* name = CX509DNSName::NewLC(gN.Data());
					iState.iPermittedDNSNameSubtrees->AppendL(name);
					CleanupStack::Pop();
					}
					break;
				case EX509IPAddress://octet string
					{
					CX509IPSubnetMask* name = CX509IPSubnetMask::NewLC(gN.Data());
					iState.iPermittedIPAddressSubtrees->AppendL(name);
					CleanupStack::Pop();
					}
					break;
				default:
					{
					User::Leave(KErrNotSupported);
					}
					break;
				}
			}//end of for loop
		CleanupStack::PopAndDestroy();//nameConsExt
		//we've handled this now, so can remove it from the critical list
		Remove(*(iState.iCriticalExts), KNameConstraints);
		}//end of if(ext)
	}


//private functions
TBool TPKIXNameConstraint::NameIsPresentL(	const CX500DistinguishedName& aSubject,
											const CArrayPtrFlat<CX500DistinguishedName>& aSubtrees)
	{
	TInt count = aSubtrees.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX500DistinguishedName* excluded = aSubtrees.At(i);
		if (aSubject.IsWithinSubtreeL(*excluded))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TBool TPKIXNameConstraint::NameIsPresent(	const CX509DomainName& aSubject,
											const CArrayPtrFlat<CX509DomainName>& aSubtrees)
	{
	TInt count = aSubtrees.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509DomainName* excluded = aSubtrees.At(i);
		if (aSubject.IsWithinSubtree(*excluded))
			{
			return ETrue;
			}
		}
	return EFalse;
	}	

TBool TPKIXNameConstraint::NameIsPresent(	const CX509IPAddress& aSubject,
											const CArrayPtrFlat<CX509IPSubnetMask>& aSubtrees)
	{
	TInt count = aSubtrees.Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX509IPSubnetMask* excluded = aSubtrees.At(i);
		if (aSubject.IsWithinSubtree(*excluded))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

//basic constraint
TPKIXBasicConstraint::TPKIXBasicConstraint(	CPKIXValidationState& aState, 
											CPKIXValidationResultBase& aResult)
	:TPKIXConstraint(aState, aResult)
	{
	}

void TPKIXBasicConstraint::CheckCertSubjectTypeL(const CX509Certificate& aCert)
	{
	TBool markedAsCA = EFalse;
	TBool actsAsCA = iState.iPos > 0;
	const CX509CertExtension* ext = aCert.Extension(KBasicConstraints);
	if (ext)
		{
		CX509BasicConstraintsExt* basic = CX509BasicConstraintsExt::NewLC(ext->Data());
		markedAsCA = basic->IsCA();
		CleanupStack::PopAndDestroy();
		}
	if (actsAsCA && (!markedAsCA))
		{
		iResult.SetErrorAndLeaveL(ENotCACert, iState.iPos);
		}
	}

void TPKIXBasicConstraint::UpdatePathLengthConstraintsL(const CX509Certificate& aCert)
	{
	const CX509CertExtension* ext = aCert.Extension(KBasicConstraints);
	if (ext)
		{
		CX509BasicConstraintsExt* basic = CX509BasicConstraintsExt::NewLC(ext->Data());
		TInt pathLength = basic->MaxChainLength();
		if (pathLength < 0)
			{
			iResult.SetErrorAndLeaveL(ENegativePathLengthSpecified, iState.iPos);
			}
		if (iState.iPos > pathLength)
			{
			iState.iMaxPathLength = pathLength + 1;
			}
		Remove(*(iState.iCriticalExts), KBasicConstraints);
		CleanupStack::PopAndDestroy();//basic
		}
	}

//key usage constraint
TPKIXKeyUsageConstraint::TPKIXKeyUsageConstraint(	CPKIXValidationState& aState, 
											CPKIXValidationResultBase& aResult)
	:TPKIXConstraint(aState, aResult)
	{
	}

void TPKIXKeyUsageConstraint::CheckKeyUsageL(const CX509Certificate& aCert)
	{
	//if key usage is critical and this is a CA cert, the keyCertSign bit must be set
	const CX509CertExtension* ext = aCert.Extension(KKeyUsage);
	if (ext)
			{
			CX509KeyUsageExt* keyUsage = CX509KeyUsageExt::NewLC(ext->Data());
			if ( (iState.iPos > 0) && (!(keyUsage->IsSet(EX509KeyCertSign))))
				{
				iResult.SetErrorAndLeaveL(EBadKeyUsage, iState.iPos);
				}
			CleanupStack::PopAndDestroy();
			//we've processed this critical ext, so remove it
			Remove(*(iState.iCriticalExts), KKeyUsage);
			}
	}
