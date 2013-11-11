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
* CPKIXValidationState class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#if !defined (__PKIXCERTSTATE_H__)
#define __PKIXCERTSTATE_H__

#include <e32std.h>
#include <x509cert.h>
#include <x509certchain.h>
#include <x509certext.h>
#include <x509gn.h>

/*this class has no brains at all. all it does it act as a container for
state information needed when processing a certificate chain according to 
the PKIX algorithm. it gets passed around among the classes which use it. 
the classes which use it are all friends 'cos that seems neater than making 
all its data public or including 500 accessors*/

class CPKIXCertChainAO;

class CPKIXValidationState : public CBase
	{
	friend class CPKIXCertChainAO;
	friend class TPKIXPolicyConstraint;
	friend class TPKIXNameConstraint;
	friend class TPKIXBasicConstraint;
	friend class TPKIXKeyUsageConstraint;
public:
	static CPKIXValidationState* NewL(	const TTime aValidationTime, TInt aChainLength,
										const CArrayPtr<HBufC>* aInitialPolicies);
	static CPKIXValidationState* NewLC(	const TTime aValidationTime, TInt aChainLength,
										const CArrayPtr<HBufC>* aInitialPolicies);
	~CPKIXValidationState();
private:
	CPKIXValidationState(	const TTime aValidationTime, TInt aChainLength,
							const CArrayPtr<HBufC>* aInitialPolicies);
	void ConstructL();
private:
	const TTime iValidationTime;
	TInt iPolicyMapping;
	TInt iPolicyRequired;
	TInt iMaxPathLength;
	TInt iPos;

//	TBool iAnyUserPolicy;
	TBool iAnyAuthorityPolicy;
	
	const CArrayPtr<HBufC>* iUserConstrainedPolicies;
	CArrayPtrFlat<CX509CertPolicyInfo>* iAuthorityConstrainedPolicies;
	
	CArrayPtrFlat<HBufC>* iUserPolicies;	
	CArrayPtrFlat<CX509PolicyMapping>* iMappedPolicies;
	CArrayPtrFlat<CX500DistinguishedName>* iExcludedDNSubtrees;
	CArrayPtrFlat<CX500DistinguishedName>* iPermittedDNSubtrees;
	//these are all just to deal with name constraints on alt names
	//can you believe it?
	CArrayPtrFlat<CX509DomainName>* iExcludedRFC822Subtrees;
	CArrayPtrFlat<CX509DomainName>* iPermittedRFC822Subtrees;
	CArrayPtrFlat<CX509DomainName>* iExcludedDNSNameSubtrees;
	CArrayPtrFlat<CX509DomainName>* iPermittedDNSNameSubtrees;
	CArrayPtrFlat<CX509IPSubnetMask>* iExcludedIPAddressSubtrees;
	CArrayPtrFlat<CX509IPSubnetMask>* iPermittedIPAddressSubtrees;
	CArrayPtrFlat<CX509CertExtension>* iCriticalExts;
	};

#endif
