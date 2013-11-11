/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "policylist.h"
#include "policyreader.h"
#include "promptrequest.h"
#include <f32file.h>
#include <ups/upserr.h>
#include "upslog.h"

using namespace UserPromptService;

// CPolicyList::TId ----------------------------------------------------------
EXPORT_C CPolicyList::TId::TId() 
/**
Constructor
*/	
	: iServerSid(), iServiceId(TUid::Null())
	{
	}

EXPORT_C CPolicyList::TId::TId(const TSecureId& aServerSid, const TServiceId& aServiceId)
/**
Constructor
@param	aServerSid	The secure id of the system server
@param	aServiceId	The UID of the service.
*/
	: iServerSid(aServerSid), iServiceId(aServiceId)
	{	
	}

EXPORT_C TBool CPolicyList::TId::operator==(const CPolicyList::TId& aId) const
/**
Tests whether this policy list id is euivalent to another policy list id.

@param	aId	The policy list id object to compare this object against.
@return ETrue if the system server secure id and the service UID match;
		otherwise, EFalse is returned.
*/
	{
	return (iServerSid == aId.iServerSid &&	iServiceId == aId.iServiceId);
	}

EXPORT_C void CPolicyList::TId::IdL(const TDesC& aPolicyFile, CPolicyList::TId& aId)
/**
Determines the policy list id from the name of the policy file.
@param	aPolicyFile	The filename of the policy file.
@param	aId			The id object to populate.

@leave	KErrUpsInvalidPolicyFileName if the policy file is not in the correct format. 
*/
	{
	TParse* p = new (ELeave) TParse();
	CleanupStack::PushL(p);
	TInt err(KErrNone);
	
	if ((err = p->Set(aPolicyFile, NULL, NULL)) == KErrNone)
		{
		TPtrC n = p->NameAndExt();
		_LIT(KPolicyFilePattern, "ups_????????_????????.rsc");
		
		if (n.MatchF(KPolicyFilePattern) != KErrNotFound)
			{
			TUint32 serverSid;
			TLex l(n.Mid(4,8));
			if ((err = l.Val(serverSid, EHex)) == KErrNone)
				{
				TUint32 serviceId;
				l = n.Mid(13,8);
				if ((err = l.Val(serviceId, EHex)) == KErrNone)
					{
					aId.iServerSid = TSecureId(serverSid);
					aId.iServiceId = TServiceId::Uid(serviceId);
					}
				}
			}
		}
		
	if (err != KErrNone)
		{
		User::Leave(KErrUpsInvalidPolicyFileName);
		}
		
	CleanupStack::PopAndDestroy(p);	
	}

EXPORT_C void CPolicyList::TId::AppendNameToPath(TDes& aFileName) const
	{
	_LIT(KPolicyFileNameFormat, "ups_%08x_%08x.rsc");
	aFileName.AppendFormat(KPolicyFileNameFormat, iServerSid.iId, iServiceId.iUid);
	}

// CPolicyList ---------------------------------------------------------------
EXPORT_C CPolicyList* CPolicyList::NewL(const CPolicyList::TId& aId, CPolicyReader& aReader)
/**
Factory method for creating policy list objects.
@param	aId			The id (system server SID and service id) of the UPS policy file.
@param	aReader		The policy file resource parser.
@return A pointer to the new policy list.
*/
	{
	CPolicyList* self = CPolicyList::NewLC(aId, aReader);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPolicyList* CPolicyList::NewLC(const CPolicyList::TId& aId, CPolicyReader& aReader)
/**
Factory method for policy list objects. A pointer to the new policy list object is placed
on the cleanup stack.

@param	aId			The id (system server SID and service id) of the UPS policy file.
@param	aReader		The policy file resource parser.
@return A pointer to the new policy list.
*/
	{
	CPolicyList* self = new(ELeave) CPolicyList(aId);
	CleanupStack::PushL(self);
	self->ConstructL(aReader);
	return self;
	}

CPolicyList::CPolicyList(const CPolicyList::TId& aId) : iId(aId)
/**
Constructor
@param aId	The ID of the UPS policy list file.
*/
	{	
	}

CPolicyList::~CPolicyList()
/**
Destructor
*/
	{
	delete iDefaultPolicy;
	iPolicies.ResetAndDestroy();
	}

void CPolicyList::ConstructL(CPolicyReader& aReader)
/**
Second phase constructor that reads all policies in the policy file.
@param aReader	The policy reader instance.
*/
	{
	const TPolicyHeader& hdr = aReader.Header();
	iServiceConfig.iServiceId = iId.iServiceId.iUid;
	iServiceConfig.iPolicy = hdr.iAuthPolicy;
	iServiceConfig.iMajorVersion = hdr.iMajorVersion;
	iServiceConfig.iMinorVersion = hdr.iMinorVersion;
	
	CPolicy* p(0);	
	while ((p = aReader.NextPolicyL()) != 0)
		{
		CleanupStack::PushL(p);
		iPolicies.AppendL(p);
		CleanupStack::Pop(p);
		}
	
	iDefaultPolicy = aReader.DefaultPolicyL();
	}

EXPORT_C const CPolicy* CPolicyList::Match(const CPromptRequest& aRequest) const
/**
Tests each policy in the list in turn and returns the first matching policy. If no match
is found then a 'default' policy object is returned.

@param	aRequest	The request to match against the policies.
@return				A pointer to the policy object to use for the request.
*/
	{
	TInt n = iPolicies.Count();
	const CPolicy* p(0);
	for (TInt i = 0; i < n; ++i)
		{
		if (iPolicies[i]->Matches(aRequest.ClientSid(), aRequest.Destination(), aRequest.SecurityResult())) 
			{
			p = iPolicies[i];
			DEBUG_PRINTF5(_L8("Using policy %d for client sid = 0x%08x, system server sid = 0x%08x, service id = 0x%08x"),
				i, aRequest.ClientSid().iId, aRequest.ServerSid().iId, aRequest.ServiceId().iUid);					
			break;
			}
		}

	if (! p)
		{
		DEBUG_PRINTF4(_L8("Using default policy for client sid = 0x%08x, system server sid = 0x%08x, service id = 0x%08x"),
			aRequest.ClientSid().iId, aRequest.ServerSid().iId, aRequest.ServiceId().iUid);
		p = iDefaultPolicy;
		}
	return p;
	}
	
EXPORT_C const CPolicyList::TId& CPolicyList::Id() const
/**
Gets the ID that associates a list of policies with a system server SID and service ID.
@return The ID of the policy list.
*/
	{
	return iId;
	}

EXPORT_C const TServiceConfig& CPolicyList::ServiceConfig() const
/**
Gets the service configuration information for this policy file. This is used
by the UPS system-server API to determine whether or not to invoke the UPS.
@return A const reference to the service configuration information.
*/
	{
	return iServiceConfig;
	}
