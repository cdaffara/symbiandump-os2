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
* Interface definition for UPS policy evaluator ECOM plug-in.
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef POLICYEVALUATOR_H
#define POLICYEVALUATOR_H

#include <e32base.h>
#include <e32cmn.h>
#include <ups/ups.hrh>
#include <ups/cliententity.h>
#include <ups/fingerprint.h>
#include <ups/policy.h>
#include <ups/policyevaluator.h>
#include <ups/promptrequest.h>


namespace UserPromptService
	{
	class CClientEntity;
	class CFingerprint;
	class CPolicy;
	class CPromptRequest;
	class CDecisionRecord;
		
	/**
	Abstract base class for a policy evaluator ECOM plug-in.
	Policy evaluators are primarily	responsible for generating the fingerprints
	used to locate previous decision records related to the current request.
	*/
	class CPolicyEvaluator : public CActive
		{
	public:
		IMPORT_C static CPolicyEvaluator* NewL(const TUid& aPolicyEvaluatormplementationId);
		
		/**
		 Asynchronously generates an ordered list the fingerprints used to locate 
		 decision records related to the request.\n
		 A fingerprint is typically a hash created from the Destination 
		 and OpaqueData values in the request structure e.g. Hash(phone number).
		 
		 - Fingerprints should be ordered most-specific to least-specific.		 
		 - Fingerprints are not required to be a hash or a fixed length.		 
		 - The policy evaluator may also specify the entity within the client process to which
		 a decision applies e.g. the id of the script or midlet.		  
		 - The policy evaluator may also generate an opaque data structure for use by the 
		 dialog creator object. 

		 @param aRequest				The parameters supplied by the system server.
		 @param aPolicy					The policy being evaluated.
		 @param aFingerprints			An ordered set of fingerprints to match against decision records.
										This is only required if the result is ECheckDatabase\n
										(OUT parameter)
		 @param aClientEntity			Optionally, identifies an entity within the client process e.g. a script.
										This is only used if the result is ECheckDatabase.\n
										Ownership is NOT transferred to the caller.\n
										(OUT parameter)
		 @param aDialogCreatorParams	Optional opaque data to pass to the dialog creator if a prompt is displayed.
										Ownership is NOT transferred to the caller.\n
										(OUT parameter)
		 @param aStatus					The request object to complete once the fingerprints have been generated.
		 */
		virtual void GenerateFingerprints(
			const CPromptRequest& aRequest, const CPolicy& aPolicy,
			RPointerArray<CFingerprint>& aFingerprints, const CClientEntity*& aClientEntity, 
			const TAny*& aDialogCreatorParams,
			TRequestStatus& aStatus) = 0;
			
		IMPORT_C virtual TBool ForcePromptL(const CDecisionRecord& aDecision, TUint& aNewEvaluatorInfo);
		IMPORT_C TInt GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1);		
		IMPORT_C ~CPolicyEvaluator();
	protected:
		IMPORT_C CPolicyEvaluator();
		IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	private:
		TAny* iReserved;
		TUid iDtor_ID_Key;		// Required by ECOM
		};
	}

#endif // POLICYEVALUATOR_H
