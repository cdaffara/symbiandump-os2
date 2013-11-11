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


/**
 @file
 @internalComponent
 @released 
*/

#ifndef POLICYLIST_H
#define POLICYLIST_H

#include <e32base.h>
#include <e32cmn.h>
#include <ups/upstypes.h>
#include "serviceconfig.h"

namespace UserPromptService
	{			
	// Functions are only exported to support unit tests.
	// Not intended for use outside UPS policy library.
	class CPolicy;
	class CPolicyReader;
	class CPromptRequest;
		
	NONSHARABLE_CLASS(CPolicyList) : public CBase
		{
	public:		
	
		/**
		Converts between UPS policy filenames and system server secure ids & service ids.
		*/
		NONSHARABLE_CLASS(TId)
			{
		public:
			IMPORT_C TId();
			IMPORT_C TId(const TSecureId& iServerSid, const TServiceId& iServiceId);
			IMPORT_C static void IdL(const TDesC& aPolicyFile, TId& aId);
			IMPORT_C void AppendNameToPath(TDes& aFileName) const;
			IMPORT_C TBool operator==(const TId& aId) const;
		public:
			TSecureId iServerSid;
			TServiceId iServiceId;
			};			
			
		IMPORT_C static CPolicyList* NewL(const TId& aId, CPolicyReader& aReader);
		IMPORT_C static CPolicyList* NewLC(const TId& aId, CPolicyReader& aReader);		
		IMPORT_C const CPolicy* Match(const CPromptRequest& aRequest) const;
		IMPORT_C const TId& Id() const;
		IMPORT_C const TServiceConfig& ServiceConfig() const; 
		
		~CPolicyList();
	private:
		CPolicyList(const TId& aId);
		void ConstructL(CPolicyReader& aReader);
		
		CPolicy* iDefaultPolicy;			///< The policy to use if no other policy matches.
		RPointerArray<CPolicy> iPolicies;	///< An ordered list of policies read from the policy file.
		/** Associates the policy list with a system server secure id and a service id.	*/
		TId iId;
		TServiceConfig iServiceConfig;		///< The policy header
		};
	}

#endif // POLICYLIST_H
