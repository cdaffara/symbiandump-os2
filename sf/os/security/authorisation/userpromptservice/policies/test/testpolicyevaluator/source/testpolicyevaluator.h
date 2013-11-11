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
* refpolicyevaluator.h
*
*/


/**
 @file
 @internalTechnology 
 @test 
*/

#ifndef TESTPOLICYEVALUATOR_H
#define TESTPOLICYEVALUATOR_H

#include <e32base.h>
#include <e32cmn.h>
#include <hash.h>
#include <ups/policyevaluator.h>

using namespace UserPromptService;

/**
Test/Reference implementation of a policy evaluator.
*/
class CTestPolicyEvaluator : public CPolicyEvaluator 
	{
public:
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
	void SetClientEntityL();	

	const CPromptRequest* iRequest;
	const CPolicy* iPolicy;
	RPointerArray<CFingerprint>* iFingerprints;
	CClientEntity* iClientEntity;
	const CClientEntity** iClientEntityPtr;	///< UPS server's pointer to client entity
	
	TRequestStatus* iClientStatus;	///< Request object to complete after generating fingerprints
	CMessageDigest* iDigest;		///< Message digest used to create fingerprint
	};

#endif // TESTPOLICYEVALUATOR_H

