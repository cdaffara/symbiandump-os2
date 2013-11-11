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


#include "pkixcertstate.h"

//********************************************************************************//
CPKIXValidationState* CPKIXValidationState::NewL(	const TTime aValidationTime, TInt aChainLength,
													const CArrayPtr<HBufC>* aInitialPolicies)
	{
	CPKIXValidationState* s = CPKIXValidationState::NewLC(aValidationTime, aChainLength, aInitialPolicies);
	CleanupStack::Pop();
	return s;
	}

CPKIXValidationState* CPKIXValidationState::NewLC(	const TTime aValidationTime, TInt aChainLength,
													const CArrayPtr<HBufC>* aInitialPolicies)
	{
	CPKIXValidationState* s = new(ELeave) CPKIXValidationState(aValidationTime, aChainLength, aInitialPolicies);
	CleanupStack::PushL(s);
	s->ConstructL();
	return s;
	}

CPKIXValidationState::~CPKIXValidationState()
	{
	if (iAuthorityConstrainedPolicies != NULL)
		{
		iAuthorityConstrainedPolicies->ResetAndDestroy();
		}
	delete iAuthorityConstrainedPolicies;

	if (iUserPolicies != NULL)
		{
		iUserPolicies->ResetAndDestroy();
		}
	delete iUserPolicies;

	if (iMappedPolicies != NULL)
		{
		iMappedPolicies->ResetAndDestroy();
		}
	delete iMappedPolicies;

	if (iExcludedDNSubtrees != NULL)
		{
		iExcludedDNSubtrees->ResetAndDestroy();
		}
	delete iExcludedDNSubtrees;

	if (iPermittedDNSubtrees != NULL)
		{
		iPermittedDNSubtrees->ResetAndDestroy();
		}
	delete iPermittedDNSubtrees;

	if (iExcludedRFC822Subtrees != NULL)
		{
		iExcludedRFC822Subtrees->ResetAndDestroy();
		}
	delete iExcludedRFC822Subtrees;

	if (iPermittedRFC822Subtrees != NULL)
		{
		iPermittedRFC822Subtrees->ResetAndDestroy();
		}
	delete iPermittedRFC822Subtrees;

	if (iExcludedDNSNameSubtrees != NULL)
		{
		iExcludedDNSNameSubtrees->ResetAndDestroy();
		}
	delete iExcludedDNSNameSubtrees;

	if (iPermittedDNSNameSubtrees != NULL)
		{
		iPermittedDNSNameSubtrees->ResetAndDestroy();
		}
	delete iPermittedDNSNameSubtrees;

	if (iExcludedIPAddressSubtrees != NULL)
		{
		iExcludedIPAddressSubtrees->ResetAndDestroy();
		}
	delete iExcludedIPAddressSubtrees;

	if (iPermittedIPAddressSubtrees != NULL)
		{
		iPermittedIPAddressSubtrees->ResetAndDestroy();
		}
	delete iPermittedIPAddressSubtrees;
	delete iCriticalExts;
	}

CPKIXValidationState::CPKIXValidationState(	const TTime aValidationTime, TInt aChainLength, 
											const CArrayPtr<HBufC>* aInitialPolicies)
	:iValidationTime(aValidationTime), iPolicyMapping(aChainLength), iPolicyRequired(aChainLength),
	iMaxPathLength(aChainLength), iPos(aChainLength-1), iAnyAuthorityPolicy(ETrue)
	{
	// iUserConstrainedPolicies only set if we have any
	if (aInitialPolicies && aInitialPolicies->Count() != 0)
		{
		iUserConstrainedPolicies = aInitialPolicies;
		}
	}

void CPKIXValidationState::ConstructL()
	{
	iAuthorityConstrainedPolicies = new(ELeave) CArrayPtrFlat<CX509CertPolicyInfo> (1);
	iUserPolicies = new(ELeave) CArrayPtrFlat<HBufC> (1);
	iMappedPolicies = new(ELeave) CArrayPtrFlat<CX509PolicyMapping> (1);
	iExcludedDNSubtrees = new(ELeave) CArrayPtrFlat<CX500DistinguishedName> (1);
	iPermittedDNSubtrees = new(ELeave) CArrayPtrFlat<CX500DistinguishedName> (1);
	iExcludedRFC822Subtrees = new(ELeave) CArrayPtrFlat<CX509DomainName> (1);
	iPermittedRFC822Subtrees = new(ELeave) CArrayPtrFlat<CX509DomainName> (1);
	iExcludedDNSNameSubtrees = new(ELeave) CArrayPtrFlat<CX509DomainName> (1);
	iPermittedDNSNameSubtrees = new(ELeave) CArrayPtrFlat<CX509DomainName> (1);
	iExcludedIPAddressSubtrees = new(ELeave) CArrayPtrFlat<CX509IPSubnetMask> (1);
	iPermittedIPAddressSubtrees = new(ELeave) CArrayPtrFlat<CX509IPSubnetMask> (1);
	iCriticalExts = new(ELeave) CArrayPtrFlat<CX509CertExtension> (1);
	//copy the user-supplied policies into userpolicies
	if (iUserConstrainedPolicies)
		{
		TInt userPolicyCount = iUserConstrainedPolicies->Count();
		for (TInt i = 0; i < userPolicyCount; i++)
			{
			HBufC* policy = iUserConstrainedPolicies->At(i)->AllocL();
			CleanupStack::PushL(policy);
			iUserPolicies->AppendL(policy);
			CleanupStack::Pop();
			}
		}
	}


