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
 @test
*/

#ifndef TUPSPOLICYEVALUATOR_H
#define TUPSPOLICYEVALUATOR_H

#include <e32base.h>
#include <e32cmn.h>
#include <e32property.h>
#include <hash.h>
#include <ups/policyevaluator.h>
#include "tupsproperty.h"

using namespace UserPromptService;

/**
Test/Reference implementation of a policy evaluator.
*/
class CTestPolicyEvaluator : public CPolicyEvaluator 
	{
public:
	typedef enum
		{
		EIdle,
		EInitialisation,
		EHoldEvaluator,
		EGenerateFingerprints	
		} TState;

	static CPolicyEvaluator* CreatePolicyEvaluatorL();

	// From UserPromptService::CPolicyEvaluator
	void GenerateFingerprints(const CPromptRequest& aRequest, const CPolicy& aPolicy, 
		RPointerArray<CFingerprint>& aFingerprints, const CClientEntity*& aClientEntity, 
		const TAny*& aDialogCreatorParams, 
		TRequestStatus& aStatus);
		
	TBool ForcePromptL(const CDecisionRecord& aDecision, TUint& aNewEvaluatorInfo);
		
protected:
	// From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	~CTestPolicyEvaluator();
private:		
	CTestPolicyEvaluator();	
	void ConstructL();
	void DoInitialisationL();
	void DoGenerateFingerprintsL();
	void SetClientEntityL();
	
	TState iState;								///< Current state	

	const CPromptRequest* iRequest;
	const CPolicy* iPolicy;
	RPointerArray<CFingerprint>* iFingerprints;
	CClientEntity* iClientEntity;
	const CClientEntity** iClientEntityPtr;	///< UPS server's pointer to client entity

	TRequestStatus* iClientStatus;			///< Request object to complete after generating fingerprints
	CMessageDigest* iDigest; 				///< Message digest used to create fingerprint
	
	RProperty iCheckIfStillNeedtoHold;		///< Determines whether th
	CUpsProperty* iUpsProperty;				///< Allows communication of paramters with Client via Publish & Subscribe
	};

#endif // TUPSPOLICYEVALUATOR_H

