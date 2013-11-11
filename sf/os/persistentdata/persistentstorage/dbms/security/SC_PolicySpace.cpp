// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CPolicySpace class
// 
//

#include <s32file.h>
#include "D32Strings.h"
#include "D32Map.h"
#include "SC_Policy.h"
#include "SC_StrmIn.h"
#include "SC_PassAllPolicy.h"
#include "SC_DomainLoader.h"
#include "SC_PolicySpace.h"

namespace DBSC
{

/**
*/
inline CPolicySpace::CPolicySpace() :
	iPDCollection(TLinearOrder< TPair<TInt, CPolicyDomain*> > (&Compare<TInt, CPolicyDomain*>))
	{
	}

/**
Standard phase-one CPolicySpace factory method
@param aFs A reference to a file server session object
@param aPrivatePath A reference to the DBMS server private directory.
@return A pointer to just created CPolicySpace instance.
@leave System-wide error codes, including KErrNoMemory
*/
CPolicySpace* CPolicySpace::NewL(RFs& aFs, const TDesC& aPrivatePath)
	{
	CPolicySpace* self = new (ELeave) CPolicySpace;
	CleanupReleasePushL(*self);
	self->ConstructL(aFs, aPrivatePath);
	CleanupStack::Pop(self);
	return self;
	}

/**
Standard phase-two CPolicySpace construction method
@param aFs A reference to a file server session object
@param aPrivatePath A reference to the DBMS server private directory.
@leave System-wide error codes, including KErrNoMemory
*/
void CPolicySpace::ConstructL(RFs& aFs, const TDesC& aPrivatePath)
	{
	iPassAllDbPolicy = new (ELeave) CPassAllPolicy(EPOTDatabase);
	iPassAllTblPolicy = new (ELeave) CPassAllPolicy(EPOTTable);
	TBuf<KMaxPath>* policyDir = new (ELeave) TFileName;
	CleanupStack::PushL(policyDir);
	policyDir->Copy(aPrivatePath);
	policyDir->Append(KSecurityPolicyDir);
	TRAPD(err, LoadPolicyDomainsL(aFs, *policyDir));
	if(err != KErrNone && err != KErrNotFound) //it's not an error, if there are no policy files
		{
		__LEAVE(err);
		}
	CleanupStack::PopAndDestroy(policyDir);
	}

/**
The method creates a policy domain object for each binary policy file found in the system.
The created objects will be added to iPDCollection collection of policy domain objects.
@param aFs A reference to a file server session object
@param aPrivatePath A reference to the DBMS server private directory.
@leave System-wide error codes, including KErrNoMemory
*/
void CPolicySpace::LoadPolicyDomainsL(RFs& aFs, const TDesC& aPolicyDir)
	{
	CPolicyDomainLoader* loader = CPolicyDomainLoader::NewLC(aFs, aPolicyDir, iPDCollection);
	loader->RunL();
	CleanupStack::PopAndDestroy(loader);
	}

/**
The method returns the related with aDomainUid parameter CPolicyDomain object.
@param aDomainUid Domain UID
@return A pointer to the related with aDomainUid parameter CPolicyDomain object.
*/
CPolicyDomain* CPolicySpace::PolicyDomain(TUid aDomainUid) const
	{
	__ASSERT(aDomainUid != KNullUid);
	CPolicyDomain* domain = NULL;
	if(iPDCollection.Find(aDomainUid.iUid, domain) == KErrNone)
		{
		__ASSERT(domain);
		__ASSERT(domain->Uid() == aDomainUid);
		}
	return domain;
	}

/**
*/
CPolicySpace::~CPolicySpace()
	{
	TMapIterator<TInt, CPolicyDomain*> it(iPDCollection);
	TPair<TInt, CPolicyDomain*> pair;
	while(it.Next(pair))
		{
		delete pair.iData;
		}
	iPDCollection.Close();
	delete iPassAllTblPolicy;
	delete iPassAllDbPolicy;
	}

/**
Implements MPolicySpace::Release().
Use this method when want to destroy particular CPolicySpace object.
*/
void CPolicySpace::Release()
	{
	delete this;
	}

/**
Implements MPolicySpace::DbPolicyL().
@param aDbPolicyRequest Request params: request type (secure/non-secure) and domain UID
@return A const pointer to the related with the request UID policy object.
*/
const MPolicy* CPolicySpace::DbPolicyL(const TDbPolicyRequest& aDbPolicyRequest) const
	{
	const MPolicy* policy = NULL;
	if(aDbPolicyRequest.iAccessType == EATNonSecure)
		{
		policy = iPassAllDbPolicy;
		}
	else//Secure shared database access
		{
		__ASSERT(aDbPolicyRequest.iUid != KNullUid);
		CPolicyDomain* domain = PolicyDomain(aDbPolicyRequest.iUid);
		if(domain)
			{
			policy = domain->DbPolicy();
			}
		}
	if(aDbPolicyRequest.iAccessType == EATSecure && !policy)
		{//there is no security policy associated with the supplied uid.
		__LEAVE(KErrArgument);
		}
	__ASSERT(policy);
	return policy;
	}

/**
Implements MPolicySpace::TblPolicyL().
@param aDbPolicyRequest Request params: request type (secure/non-secure) and domain UID
@param aTblName Database table name
@return A const pointer to the related with the request table policy object.
*/
const MPolicy* CPolicySpace::TblPolicyL(const TDbPolicyRequest& aDbPolicyRequest, 
										const TDesC& aTblName) const
	{
	const MPolicy* policy = NULL;
	if(aDbPolicyRequest.iAccessType == EATNonSecure)
		{
		policy = iPassAllTblPolicy;
		}
	else//Secure shared database access
		{
		CPolicyDomain* domain = PolicyDomain(aDbPolicyRequest.iUid);
		if(domain)
			{
			policy = domain->TblPolicy(aTblName);
			if(!policy)
				{
				policy = domain->DbPolicy();
				}
			}
		}
	if(aDbPolicyRequest.iAccessType == EATSecure && !policy)
		{//there is no security policy associated with the supplied uid.
		__LEAVE(KErrArgument);
		}
	__ASSERT(policy);
	return policy;
	}

/**
Implements MPolicySpace::BackupSIDL().
Returns backup&restore SID for the databases, the access to which is controlled by the
security policy, identified by aDbUid parameter.
@param aDbUid Domain UID
@return Backup&restore SID for the supplied domain UID
@leave KErrArgument if there is no security policy domain for the supplied UID.
*/
TSecureId CPolicySpace::BackupSIDL(TUid aDbUid) const
	{
	CPolicyDomain* domain = PolicyDomain(aDbUid);
	if(!domain)
		{
		__LEAVE(KErrArgument);
		}
	return domain->BackupSID();
	}
	
} //end of - namespace DBSC
