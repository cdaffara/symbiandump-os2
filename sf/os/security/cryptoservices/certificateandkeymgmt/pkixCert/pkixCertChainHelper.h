/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CPKIXCertChainHelper class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __PKIXCERTCHAINHELPER_H__
#define __PKIXCERTCHAINHELPER_H__

#include <e32base.h>

class RFs;
class CPKIXCertChainBase;
class CPKIXValidationResult;
class CUnifiedCertStore;

/**
 * An helper object that used to implement the valdiation functionality of the
 * CPKICCertChain interface.  This is done by creating and initialising a
 * unified certificate store, and then calling through to the implementation in
 * CPKIXCertChainBase.
 */
NONSHARABLE_CLASS(CPKIXCertChainHelper) : public CActive
	{
public:
	static CPKIXCertChainHelper* NewL(RFs& aFs);

	virtual ~CPKIXCertChainHelper();

	CUnifiedCertStore& CertStore();
	
	/** Validates the chain. */
	void Validate(CPKIXCertChainBase& aCertChain,
				  CPKIXValidationResult& aValidationResult, 
				  const TTime& aValidationTime,
				  TRequestStatus& aStatus);
	
	/** Validates the chain. */
	void Validate(CPKIXCertChainBase& aCertChain,
				  CPKIXValidationResult& aValidationResult, 
				  const TTime& aValidationTime,
				  const CArrayPtr<HBufC>& aInitialPolicies, 
				  TRequestStatus& aStatus);

	/** Cancels an asynchronous Validate() operation. */
	void CancelValidate();

private:
	CPKIXCertChainHelper();
	void ConstructL(RFs& aFs);
	void StartValidate(CPKIXCertChainBase& aCertChain,
					   CPKIXValidationResult& aValidationResult, 
					   const TTime& aValidationTime,
					   const CArrayPtr<HBufC>* aInitialPolicies, 
					   TRequestStatus& aStatus);
	void InitializeCertStore();
	void ValidateChainL();

private:
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();
	void Complete(TInt aError);
	
private:
	enum TState
		{
		EIdle,
		EInitializeCertStore,
		EValidateChain
		};
	
	CUnifiedCertStore*		iCertStore;
	TBool					iCertStoreInitialized;
	TState					iState;
	
	// Client parameters
	CPKIXCertChainBase*		iCertChain;
	CPKIXValidationResult*	iValidationResult;
	TTime					iValidationTime;
	const CArrayPtr<HBufC>*	iInitialPolicies;
	TRequestStatus*			iClientStatus;
	};

#endif
