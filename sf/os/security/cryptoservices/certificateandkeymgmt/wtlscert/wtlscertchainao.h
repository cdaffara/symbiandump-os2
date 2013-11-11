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
* CWTLSCertChainAO class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __WTLSCERTCHAINAO_H__
#define __WTLSCERTCHAINAO_H__

#include <e32base.h>
#include <f32file.h>
#include <unifiedcertstore.h>
#include <hash.h>


class CWTLSCertChain;
class CWTLSValidationResult;
class CWTLSCertificate;

/**
 * This class handles the asynchronous part of the wtls chain validation. It
 * is an active object which handles the asynchronous certificate store operations.
 */
class CWTLSCertChainAO : public CActive
{
public:
	static CWTLSCertChainAO* NewL(RFs& aFs, CWTLSCertChain &aWTLSCertChain,
		const CArrayPtr<CWTLSCertificate>& aRootCerts);
	static CWTLSCertChainAO* NewL(RFs& aFs, CWTLSCertChain &aWTLSCertChain, 
		const TUid aClient);
	virtual ~CWTLSCertChainAO();
	virtual TInt RunError(TInt aError);

private:
	CWTLSCertChainAO(RFs& aFs, CWTLSCertChain &aWTLSCertChain);
	CWTLSCertChainAO(RFs& aFs, CWTLSCertChain &aWTLSCertChain, const TUid aClient);
	void ConstructL(const CArrayPtr<CWTLSCertificate>& aRootCerts);

public:
	void RunL();
	void DoCancel();

private:
	/**
	 * This function creates a CCertStoreManager,
	 * calls CCertStoreManager initialise and sets the state
	 * to EStoreManagerInitialized
	 */
	void HandleEStoreManagerInitializationL();
	void HandleEStoreManagerInitializedL();
	void HandleEGetCertHashesL();
	void HandleEPruneListL();
	void HandleEPruneListDoneL();
	void HandleECheckTCAL();
	void HandleEIsChainSelfSignedL();
	void HandleERetrieveRootsL();
	void HandleEAddRootToListL();
	void HandleEFindRootL();
	void HandleEValidateEndL();

	// Request functions
public:
	void Validate(CWTLSValidationResult& aValidationResult, const TTime& aValidationTime,							
		TRequestStatus& aStatus);

private:
	TBool CheckSignatureAndNameL(const CWTLSCertificate& aCert,
		CWTLSValidationResult& aResult, TInt aPos) const;
	TBool CheckValidityPeriod(const CWTLSCertificate& aCert,
		CWTLSValidationResult& aResult,	const TTime aTime, TInt aPos) const;
	HBufC8& GeneratePublicKeyHashL(const CWTLSCertificate& aCert) const;

	enum TState
	{
		EStoreManagerInitialization = 0,
		EStoreManagerInitialized,
		EGetCertHashes,
		EPruneList,
		EPruneListDone,
		ECheckTCA,
		EIsChainSelfSigned,
		ERetrieveRoots,
		EAddRootToList,
		EFindRoot,
		EValidateEnd
	};

private:
	RFs& iFs;
	CUnifiedCertStore* iCertStoreManager;
	/**
	 * List of the subject hashes from the cert store
	 * The elements remained owned by the CCTCertInfo
	 */
	RPointerArray< TBuf8<20> > iRootSubjectStoreHashList;
	/**
	 * List of the subject hashes from the passed in certs
	 * We own this object and all its elements
	 */
	RPointerArray< HBufC8 > iRootSubjectClientHashList;
	CCertAttributeFilter* iFilter;
	/**
	 * Stores info on the certs in the cert store
	 * We own this object and all its elements
	 */
	RMPointerArray<CCTCertInfo> iCertInfos;
	/**
	 * Indicates whether iWTLSCertChain.iChain was pruned or not
	 */
	TBool iPruned;
	/**
	 * The index within iChain that the chain was pruned
	 */
	TInt iPrunedChainLength;

	CWTLSCertChain& iWTLSCertChain;
	TUid iClient;
	TPtr8 iEncodedCert;
	HBufC8* iEncodedCertTemp;

	TState iState;

	TRequestStatus* iOriginalRequestStatus;

	/**
	 * Roots passed in from the client that we trust
	 * We own this object and all its elements
	 */
	RPointerArray<CWTLSCertificate> iRootsFromClient;
	/**
	 * Roots from the specified cert store that we trust.
	 * We own this object and all its elements
	 */
	RPointerArray<CWTLSCertificate> iRootsFromStore;

	/**
	 * A counter used to carry counter information between AO states
	 */
	TInt iIndex;

	/**
	 * Indicates if a root cert has been found for this particular chain
	 */
	TBool iFoundRoot;

	/**
	 * To store the parameters passed to CWTLSCertChain::Validate
	 */ 
	CWTLSValidationResult* iValidationResult;
	/**
	 * To store the parameters passed to CWTLSCertChain::Validate
	 */ 
	const TTime* iValidationTime;					 
};

#include <wtlscertchain.h>

#endif
