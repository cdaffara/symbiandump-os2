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


#include "policycache.h"
#include "policylist.h"
#include "policyreader.h"
#include "promptrequest.h"
#include "serviceconfig.h"
#include <f32file.h>
#include <ups/upserr.h>
#include "upslog.h"

using namespace UserPromptService;

EXPORT_C CPolicyCache* CPolicyCache::NewL(RFs& aFs, const TDesC& aPolicyPath)
/**
Creates a new policy cache.
@param aFs			The file server session used to load policy files.\n
					Ownership is not transferred.
@param aPolicyPath	The path of the policy files (minus the drive). It must be
					in the following form "\private\sid\policies\"
@return				A pointer to the new policy cache object.
*/
	{
	CPolicyCache* self = new(ELeave) CPolicyCache(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aPolicyPath);
	CleanupStack::Pop(self);
	return self;
	}

CPolicyCache::CPolicyCache(RFs& aFs) 
/**
Constructor
@param aFs	The file server session used to load policy files.
*/
	: iFs(aFs)
	{	
	}

CPolicyCache::~CPolicyCache()
/**
Destructor
*/
	{
	iPolicyLists.ResetAndDestroy();
	delete iPolicyPath;
	}

void CPolicyCache::ConstructL(const TDesC& aPolicyPath)
/**
Second phase constructor, resolves the system drive.
@param aPolicyPath	The directory where the policy files are stored.
*/
	{
	iPolicyPath = aPolicyPath.AllocL();
	iSystemDriveChar = iFs.GetSystemDriveChar();
	}

EXPORT_C const CPolicy* CPolicyCache::MatchL(const CPromptRequest& aRequest)
/**
Finds the first policy that matches the request.

@param	aRequest	The request data from the system server.
@return				A copy of the policy object to apply to the request. If a policy
					file exists but no policy is defined then a default policy object
					is returned that current request to be accepted or denied.
					
@leave KErrUpsMissingPolicyFile	If no policy file is found for the requested server sid and service id.								
@leave KErrUpsBadPolicyFile		If an error occured whilst parsing a policy file.
*/
	{
	CPolicyList::TId id(aRequest.ServerSid(), aRequest.ServiceId());		
	const CPolicyList* list = PolicyList(id);
	
	if (!list)
		{
		// Policy list not found so try and load it. This
		// leaves if a UPS policy file is not found.
		list = LoadPolicyListL(id);				
		}
		
	ASSERT(list);	
	return list->Match(aRequest);
	}

const CPolicyList* CPolicyCache::PolicyList(const CPolicyList::TId& aId) const
/**
Gets the specified policy list if it is loaded.
@param	The ID of the policy list.
@return A pointer to the policy list if it is loaded; otherwise, is returned.
*/
	{
	TInt count = iPolicyLists.Count();
	for (TInt i = 0; i < count; ++i) // Check if policy list is in the cache
		{
		if (iPolicyLists[i]->Id() == aId)
			{
			return iPolicyLists[i];			
			}
		}
	return 0;
	}

CPolicyList* CPolicyCache::LoadPolicyListL(const CPolicyList::TId& aId)
/**
Loads a list of policies from the policy file for the specified system server and service.
@param	aId	A tuple of the system server SID and service UID that identifies a policy file.

@return		The list of policies, or N
@leave		KErrUpsBadPolicyFile if an error occured whilst parsing a policy file.
			KErrUpsMissingPolicyFile if the policy file was not found on the Z or the system drive.
*/
	{
	RBuf fn;
	fn.CreateL(KMaxFileName);
	CleanupClosePushL(fn);
	
	LocatePolicyFileL(fn, aId);		
	
	CPolicyList* l(0);
	TRAPD(err, l = ParseListL(aId, fn));			
	if (err != KErrNone && err != KErrNoMemory)
		{
		DEBUG_PRINTF2(_L("Unable to parse %S"), &fn);
		err = KErrUpsBadPolicyFile;
		
		if (fn[0] == 'Z')
			{			
			DEBUG_PRINTF2(_L("Unable to parse %S"), &fn);
			User::Panic(KUpsPoliciesPanicCat, EUpsPoliciesCorruptRomPolicy);			
			}
		else {
			// If we failed to read the policy file from the system 
			// drive then try the Z drive.
			fn[0] = 'Z';
			TRAP(err, l = ParseListL(aId, fn));
			if (err == KErrNone)
				{
				DEBUG_PRINTF2(_L("Ignoring corrupt policy file on system drive. Loading %S"), &fn);
				}
			else if (err != KErrNotFound && err != KErrNoMemory)
				{
				DEBUG_PRINTF2(_L("Unable to parse %S"), &fn);
				User::Panic(KUpsPoliciesPanicCat, EUpsPoliciesCorruptRomPolicy);
				}			
			}
		}
	User::LeaveIfError(err);
	
	CleanupStack::PushL(l);
	iPolicyLists.AppendL(l);
	CleanupStack::Pop(l);		
	CleanupStack::PopAndDestroy(&fn);
	return l;
	}

void CPolicyCache::LocatePolicyFileL(TDes& aPolicyFileName, const CPolicyList::TId& aId)
/**
Determines whether the policy file should be loaded from the Z drive 
or the system drive and returns the filename.

@param aPolicyFileName	Descriptor to populate with the filename.
@param aId				The id of the policy file to load.
						
@leave KErrUpsMissingPolicyFile The policy file was not found on either the Z drive
								or the system drive.
*/	
	{
	_LIT(KDriveSpec, "!:");
		
	aPolicyFileName.Zero();
	aPolicyFileName.Append(KDriveSpec);
	aPolicyFileName.Append(*iPolicyPath);
	aId.AppendNameToPath(aPolicyFileName);
	
	// System drive eclipses Z drive	
	aPolicyFileName[0] = iSystemDriveChar;
	if (! FileExistsL(aPolicyFileName))
		{
		aPolicyFileName[0] = 'Z';
		if (! FileExistsL(aPolicyFileName))
			{
			DEBUG_PRINTF3(_L8("No policy file for system server sid = 0x%08x, service id = 0x%08x"),
				aId.iServerSid.iId, aId.iServiceId.iUid);
			User::Leave(KErrUpsMissingPolicyFile);	
			}
		}
	}

CPolicyList* CPolicyCache::ParseListL(const CPolicyList::TId& aId, const TDesC& aPolicyFileName)
/**
Parses a policy file and constructs a policy list object.

@param	aId				The policy file id that will be used to locate the policy list within
						the policy cache.
@param	aPolicyFileName	The absolute path of the policy file to parse.
@return The new policy list object.
*/
	{
	CPolicyReader* r = CPolicyReader::NewLC(iFs, aPolicyFileName);
	CPolicyList* l = CPolicyList::NewL(aId, *r);	
	CleanupStack::PopAndDestroy(r);
	return l;
	}

EXPORT_C void CPolicyCache::ServiceConfigL(const TSecureId& aServerSid, RArray<TServiceConfig>& aConfigs)
/**
Loads the service configuration data from the policy files for every service provided
by a given system server.
@param	aServerSid	The secure id of the system server.
@param	aConfigs	The RArray to populate with the configuration data.
*/
	{				
	RArray<TUint> services;
	CleanupClosePushL(services);
	
	DEBUG_PRINTF2(_L8("Loading service configuration for system server 0x%08x"), aServerSid.iId);
	
	FindServicesL(aServerSid, services);
	TInt numServices = services.Count();
	
	// Load the services
	for (TInt i = 0; i < numServices; ++i)
		{
		CPolicyList::TId id(aServerSid, TUid::Uid(services[i]));		
		const CPolicyList* list = PolicyList(id);
		if (list)
			{
			// Use in-memory service configuration
			aConfigs.AppendL(list->ServiceConfig());
			}
		else 
			{
			// Load policy file into cache
			CPolicyList* loadedList = LoadPolicyListL(id);
			aConfigs.AppendL(loadedList->ServiceConfig());
			}
		}						
	CleanupStack::PopAndDestroy(&services);
	}

void CPolicyCache::FindServicesL(const TSecureId& aServerSid, RArray<TUint>& aServices)
/**
Finds all of the policy files on Z drive and C drive for this system server		
A wildcard string is used to filter out policy files for other system servers.

@param aServerSid	The secure id of the system server.
@param aServices	The array of service ids to populate.
*/
	{
	_LIT(KFilenameTemplate, "ups_%08x_????????.rsc");
	TBuf<32> pattern;
	pattern.AppendFormat(KFilenameTemplate, aServerSid.iId);
		
	RBuf fn;	// Store current file/directory name.
	fn.CreateL(KMaxFileName);
	CleanupClosePushL(fn);	
	for (TInt i = 0; i < 2; ++i)	
		{
		TUint driveChar = (i == 0) ? TChar('Z') : iSystemDriveChar;
		
		fn.Zero();
		_LIT(KDirFormat, "%c:%S");
		fn.AppendFormat(KDirFormat, driveChar, &*iPolicyPath);
		
		CDir* dir(0);	
		TInt err = iFs.GetDir(fn, KEntryAttNormal, ESortNone, dir);
		if (err == KErrPathNotFound) 
			{
			continue;
			}
		User::LeaveIfError(err);
		CleanupStack::PushL(dir);
		
		// Find the services
		TInt count = dir->Count();
		for (TInt j = 0; j < count; ++j)
			{
			const TEntry& e((*dir)[j]);
			if (e.iName.MatchF(pattern) >= 0)
				{
				fn.Zero();
				_LIT(KFileFormat, "%C:%S%S");
				fn.AppendFormat(KFileFormat, driveChar, iPolicyPath, &e.iName);
								
				CPolicyList::TId policyListId;
				TRAP(err, CPolicyList::TId::IdL(fn, policyListId))
				if (err == KErrNoMemory)
					{
					// Do not let OOM cause us to ignore policy files
					User::Leave(err);
					}
				if (err == KErrNone)
					{
					// Ensure there are no duplicate service configs due to eclisped policy files.
					err = aServices.InsertInOrder(static_cast<TUint>(policyListId.iServiceId.iUid));
					if (err != KErrAlreadyExists)
						{
						User::LeaveIfError(err);
						}
					}
				else
					{
					DEBUG_PRINTF2(_L("%S does not match UPS policy filename rules"), &fn);
					}
				}
			// Policy file is for a different system server
			}		
		CleanupStack::PopAndDestroy(dir);
		}	
	CleanupStack::PopAndDestroy(&fn);
	}

TBool CPolicyCache::FileExistsL(const TDesC& aFileName)
/**
Checks whether a file exists.
- If the FileName corresponds to a directory then EFalse is returned.

@param	aFileName	The absolute path of the file to check.
@return ETrue, if aFileName exists; otherwise, EFalse is returned.
*/
	{
	TBool exists = EFalse;
	TEntry* e = new(ELeave) TEntry();		
	TInt err = iFs.Entry(aFileName, *e);
	if (err == KErrNone && ! e->IsDir())
		{
		exists = ETrue;			
		}
	delete e;
	return exists;
	}

