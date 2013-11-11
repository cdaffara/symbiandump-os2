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
* Class declaration for the User Prompt Service policy cache.
*
*/


/**
 @file
 @internalComponent
 @released 
*/

#ifndef POLICYCACHE_H
#define POLICYCACHE_H

#include <e32base.h>
#include <e32cmn.h>

#include <f32file.h>
#include "policylist.h"
#include "serviceconfig.h"

namespace UserPromptService
	{
	class CPolicy;
	class CPromptRequest;
	
	/**
	Maintains an in memory cache of User Prompt Service policies.	
	- Policy files are loaded lazily when MatchL is called..
	*/	
	NONSHARABLE_CLASS(CPolicyCache) : public CBase
		{
	public:
		IMPORT_C static CPolicyCache* NewL(RFs& aFs, const TDesC& aPolicyPath);
		IMPORT_C const CPolicy* MatchL(const CPromptRequest& aRequest);
		IMPORT_C void ServiceConfigL(const TSecureId& aServerSid, RArray<TServiceConfig>& aConfig);
		
		~CPolicyCache();
	private:
		CPolicyCache(RFs& aFs);
		void ConstructL(const TDesC& aPolicyPath);
		CPolicyList* ParseListL(const CPolicyList::TId& aId, const TDesC& aPolicyFileName);
		CPolicyList* LoadPolicyListL(const CPolicyList::TId& aId);
		void LocatePolicyFileL(TDes& aPolicyFileName, const CPolicyList::TId& aId);
		TBool FileExistsL(const TDesC& aFileName);
		
		const CPolicyList* PolicyList(const CPolicyList::TId& aId) const;
		void FindServicesL(const TSecureId& aServerSid, RArray<TUint>& aServices);

		RPointerArray<CPolicyList> iPolicyLists;
		HBufC* iPolicyPath;
		RFs iFs;
		TChar iSystemDriveChar;
		};
	}

#endif // POLICYCACHE_H
