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
* TPKIXConstraint and TPKIXPolicyConstraint implementations
*
*/


/**
 @file 
 @internalTechnology
*/
 
#if !defined (__PKIXCONS_H__)
#define __PKIXCONS_H__

#include <e32std.h>
#include <x509cert.h>
#include <x509certchain.h>
#include <x509certext.h>
#include <x509gn.h>
#include <pkixcertchain.h>
#include "pkixcertstate.h"

class TPKIXConstraint
	{
protected:
	TPKIXConstraint(CPKIXValidationState& aState, CPKIXValidationResultBase& aResult);
	void Remove(CArrayPtrFlat<CX509CertExtension>& aCriticalExtensions, const TDesC& aOID);
	CPKIXValidationState& iState;
	CPKIXValidationResultBase& iResult;
	};

/*
Certificate policies are processed in the following way:

Inputs:
------
user-constrained policy set: set of policies deemed acceptable to client of PKIXCert (relying party)

Variables:
---------
UP: user-constrained-policy set: initialised by user, or set to empty (='any-policy', since empty is not a sensible value)
AP: authority-constrained policy set: set of policies deemed acceptable to issuing authority, initialised to 'any-policy'
MP: mapped policy set: set of mapped policies, initialised to empty
explicit policy:	integer indicating position of first certificate in path where explicit policy ID is required.
					intialised to n+1 where n=length of chain
policy mapping:		integer indicating position of last certificate in path where policy mapping is allowed
					intialised to n+1 where n=length of chain

Processing:
----------
For each certificate in chain, where certificate is at i in the chain (root being 1, ee cert being n):

-if explicit policy <= i, a policy ID in cert shall be in UP
-if policy mapping <= i, policy may not be mapped
-if the cert doesn't contain a policy extension, then:
	-if AP = any, AP is unchanged
	-else AP is set to empty
-if the cert contains a policy extension then 
	-AP is set to the intersection of AP and CP
	-any mapped policies occurring in CP are added to AP

-if we can do mapping, then
	-if AP = any then 
		-all the 'subject' policies are added to MP
		-for all mapped policies, if the 'issuer' policy is in UP then the 'subject' policy is added to UP
	-if AP != any then
		-for all mapped policies, 
			-if the issuer policy is in AP then
				-the subject policy is added to MP
				-if the issuer policy is in UP then the subject policy is added to UP

At the end of the chain,
-if UP = any then we pass
-if UP != any then 
	-if AP = any then we fail
	-otherwise we intersect AP and UP; if the result is empty then we fail

It is not entirely clear that this is a faithful implementation of the algorithm described in RFC 2459 
section 6.1, because the spec is a little ambiguous here. Additionally, this section of the spec is 
currently undergoing revision (true as of 12 April 2000). So when it's finished being revised, the code
here should be revised to reflect the spec.

However, for the test vectors that I have at this time this appears to give sensible results.

*/

class TPKIXPolicyConstraint : public TPKIXConstraint
	{
public:
	TPKIXPolicyConstraint(CPKIXValidationState& aState, CPKIXValidationResultBase& aResult);
	void CheckCertPoliciesL(const CX509Certificate& aCert);
	void UpdatePolicyConstraintsL(const CX509Certificate& aCert);
	void FinishPolicyCheckL();
private:
	void IntersectCertPoliciesL(const CX509CertPoliciesExt& aPolicyExt);
	void UpdateConstraint(const TX509PolicyConstraint& aConstraint, TInt& aCountdown);
	CArrayPtrFlat<CX509CertPolicyInfo>* IntersectionLC(	const CArrayPtrFlat<CX509CertPolicyInfo>& aFirst,
														const CArrayPtrFlat<CX509CertPolicyInfo>& aSecond);
	TBool PolicyIsPresentL(	const CArrayPtrFlat<CX509CertPolicyInfo>& aPolicies, 
							const CArrayPtr<HBufC>& aAcceptablePolicies);
	static void CleanupPolicyInfoArray(TAny* aPolicies);
	};

class TPKIXNameConstraint : public TPKIXConstraint
	{
public:
	TPKIXNameConstraint(CPKIXValidationState& aState, CPKIXValidationResultBase& aResult);
	void CheckNameConstraintsL(	const CX509Certificate& aCert);
	void UpdateNameConstraintsL(const CX509Certificate& aCert);
private:
	TBool NameIsPresentL(const CX500DistinguishedName& aSubject,
						const CArrayPtrFlat<CX500DistinguishedName>& aSubtrees);
	TBool NameIsPresent(const CX509DomainName& aSubject,
						const CArrayPtrFlat<CX509DomainName>& aSubtrees);
	TBool NameIsPresent(const CX509IPAddress& aSubject,
						const CArrayPtrFlat<CX509IPSubnetMask>& aSubtrees);
	};

class TPKIXBasicConstraint : public TPKIXConstraint
	{
public:
	TPKIXBasicConstraint(CPKIXValidationState& aState, CPKIXValidationResultBase& aResult);
	void CheckCertSubjectTypeL(const CX509Certificate& aCert);
	void UpdatePathLengthConstraintsL(const CX509Certificate& aCert);
private:
	};

class TPKIXKeyUsageConstraint : public TPKIXConstraint
	{
public:
	TPKIXKeyUsageConstraint(CPKIXValidationState& aState, CPKIXValidationResultBase& aResult);
	void CheckKeyUsageL(const CX509Certificate& aCert);
private:
	};

#endif
