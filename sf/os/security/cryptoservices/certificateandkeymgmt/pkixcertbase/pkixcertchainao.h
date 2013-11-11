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
* CPKIXCertChainAO class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __PKIXCERTCHAINAO_H__
#define __PKIXCERTCHAINAO_H__

#include <e32base.h>
#include "pkixcerts.h"
#include "pkixchainbuilder.h"
#include "pkixcertstate.h"
#include <x509cert.h>
#include <x509certext.h>
#include <x509gn.h>
#include <pkixcertchain.h>

class CPKIXCertChainBase;

/**
 * This class handles the asynchronous part of the pkix chain validation. It
 * is an active object which handles the asynchronous certificate store operations.
 */
class CPKIXCertChainAO : public CActive
{
public:
	static CPKIXCertChainAO* NewL(MCertStore& aCertStore, CPKIXCertChainBase &aPKIXCertChain,
		const RPointerArray<CX509Certificate>& aRootCerts);
	static CPKIXCertChainAO* NewL(MCertStore& aCertStore, CPKIXCertChainBase &aPKIXCertChain, 
		const TUid aClient);
	virtual ~CPKIXCertChainAO();

private:
	CPKIXCertChainAO(MCertStore& aCertStore, CPKIXCertChainBase &aPKIXCertChain);
	CPKIXCertChainAO(MCertStore& aCertStore, CPKIXCertChainBase &aPKIXCertChain, const TUid aClient);
	void ConstructL(const RPointerArray<CX509Certificate>& aRootCerts);

public:
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	void HandleEStoreManagerInitializationL();
	void HandleEStoreManagerInitializedL();
	void HandleEAddRootsL();
	void HandleERootsInitializedL();
	void HandleEBuildChainStartL();
	void HandleEBuildChainAddCandidateEndL();
	void HandleEBuildChainCertsFromStoreBeginL();
	void HandleEBuildChainCertsFromStoreEndL();
	void HandleEAddCandidateIntermediateCertsEndL();
	void HandleEValidateEndL();

	// Request functions
public:
	void ValidateL(CPKIXValidationResultBase& aValidationResult, const TTime& aValidationTime,							
		const CArrayPtr<HBufC>* aInitialPolicies, TRequestStatus& aStatus);
	void CancelValidate();

private:
	void InitParamsL();
	void SetParamsL(CX509Certificate& aCert, const TPtrC8& aEncodedParams);
	void DoValidateL(CPKIXValidationResultBase& aValidationResult, const TTime& aValidationTime,
		const CArrayPtr<HBufC>* aInitialPolicies);
	void ProcessCertsL(CPKIXValidationState& aState, CPKIXValidationResultBase& aResult) const;
	void CriticalExtsL(CPKIXValidationState& aState, const CX509Certificate& aCert) const;
	void CheckSignatureAndNameL(const CX509Certificate& aCert, CPKIXValidationState& aState, 
								CPKIXValidationResultBase& aResult) const;
	void CheckCriticalExtsL(CPKIXValidationState& aState, 
		CPKIXValidationResultBase& aResult) const;

	/**
	 * The states used to know what to do in the RunL function.
	 */
	enum TState
	{
		EAddRoots,
		ERootsInitialized,
		EBuildChainStart,
		EBuildChainAddCandidateEnd,
		EBuildChainCertsFromStoreBegin,
		EBuildChainCertsFromStoreEnd,
		EAddCandidateIntermediateCertsEnd,
		EValidateEnd
	};

private:
	MCertStore* iCertStore;
	// a reference to the object which owns this instance
	CPKIXCertChainBase &iPKIXCertChain;
	TUid iClient;
	TState iState;

	/**
	 * <P>Ownership of this object is peculiar.</P>
	 * <P>We need this to be a member because it is created and initialized in
	 * the EAddRoots handler but is only added to iRoots in the ERootsInitialized
	 * handler. iRoots takes ownesrhip of it, so it is set to 0 afterwards.</P>
	 * <P>The only case where the destructor will have to delete it, is when a leave
	 * occurs between the EAddRoots and ERootsInitialized states.</P>
	 */
	CPKIXCertsFromStore* iCertsFromStoreRoots;

	TRequestStatus* iOriginalRequestStatus;

	/**
	 * The roots that we trust.
	 * This can be given 
	 * <UL>
	 * <LI>explicitly : the user gives a set of root certificates at construction time.</LI>
	 * <LI>implicitly : the user gives the uid of the application and the validation code
	 * retrieves the root certificates trusted for this application from the certificate
	 * store.</LI>
	 * </UL>
	 */
	CPKIXChainBuilder* iRoots;

	CPKIXChainBuilder* iBuilder;

	/**
	 * To store the parameters passed to CPKIXCertChainBase::Validate
	 */ 
	CPKIXValidationResultBase* iValidationResult;
	/**
	 * To store the parameters passed to CPKIXCertChainBase::Validate
	 */ 
	TTime iValidationTime;					 
	/**
	 * To store the parameters passed to CPKIXCertChainBase::Validate
	 */ 
	const CArrayPtr<HBufC>* iInitialPolicies;

	TBool iAddIssuerResult;

	TInt iNumberOfAddedCertificates;
};

#endif
