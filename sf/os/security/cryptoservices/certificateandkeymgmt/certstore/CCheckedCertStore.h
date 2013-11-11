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
* CCheckedCertStore class implementation
*
*/


/**
 @file 
 @internalComponent 
*/
 
#ifndef __CCHECKEDCERTSTORE_H__
#define __CCHECKEDCERTSTORE_H__

#include <e32std.h>
#include <ct.h>
#include <e32property.h>
#include <mctwritablecertstore.h>
#include "unifiedkeystore.h"

NONSHARABLE_CLASS(CCheckedCertStore) : public CActive, public MCTWritableCertStore
{
public:
 	static CCheckedCertStore* NewCheckedCertStoreL(MCTTokenInterface*, RProperty&);
 	static CCheckedCertStore* NewCheckedWritableCertStoreL(MCTTokenInterface*, RProperty&);
public:
	void ConstructL();
public:	//	MCTTokenInterface
	virtual MCTToken& Token();

public:	//	MCTCertStore
	virtual void List(RMPointerArray<CCTCertInfo>& aCerts, const CCertAttributeFilter& aFilter,
						TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void GetCert(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle, 
						TRequestStatus& aStatus);
	virtual void CancelGetCert();
	virtual void Applications(const CCTCertInfo& aCertInfo, RArray<TUid>& aApplications,
						TRequestStatus& aStatus);
	virtual void CancelApplications();
	virtual void IsApplicable(const CCTCertInfo& aCertInfo, TUid aApplication, 
						TBool& aIsApplicable, TRequestStatus& aStatus);
	virtual void CancelIsApplicable();
	virtual void Trusted(const CCTCertInfo& aCertInfo, TBool& aTrusted, 
						TRequestStatus& aStatus);
	virtual void CancelTrusted();
	virtual void Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert, 
						TRequestStatus& aStatus);
	virtual void CancelRetrieve();

	
public:	//	MCTWritableCertStore

	virtual void Add( const TDesC& aLabel, TCertificateFormat aFormat,
						TCertificateOwnerType aCertificateOwnerType, 
						const TKeyIdentifier* aSubjectKeyId,
						const TKeyIdentifier* aIssuerKeyId,
						const TDesC8& aCert, TRequestStatus& aStatus);
	
	// new Add(.., TBool aDeletable, ..) method from MCTWritableCertStore
	virtual void Add( const TDesC& aLabel, 
						TCertificateFormat aFormat,
						TCertificateOwnerType aCertificateOwnerType, 
						const TKeyIdentifier* aSubjectKeyId,
						const TKeyIdentifier* aIssuerKeyId,
						const TDesC8& aCert, 
						const TBool aDeletable,
						TRequestStatus& aStatus);
												
	virtual void CancelAdd();
	virtual void Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus);
	virtual void CancelRemove();
	virtual void SetApplicability(const CCTCertInfo& aCertInfo, 
						const RArray<TUid>& aApplications, TRequestStatus &aStatus);
	virtual void CancelSetApplicability();
	virtual void SetTrust(const CCTCertInfo& aCertInfo, TBool aTrusted, 
						TRequestStatus& aStatus);
	virtual void CancelSetTrust();
protected:	//	From CActive
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
private:
	enum TState
	{
		EIdleState = 0,
		EList,
		EAdd,					// attempts using new Add() with aDeletable param
		EInitKeyStoreForAdd,
		EInitKeyStoreForList,
		EGetKeyInfosForAdd,
		EGetKeyInfosForList,
		EOldAdd,				// uses original Add() w/o aDeletable param
								// if the new Add is not supported
		ERemove,
		ESetApplicability,
		ESetTrust
	};
private:
 	CCheckedCertStore(MCTCertStore&, RProperty&);
 	CCheckedCertStore(MCTWritableCertStore&, RProperty&);
	virtual ~CCheckedCertStore();
private:
    void Complete(TInt aError);
	void Cleanup();
	void InitialiseKeyStoreL(TState aNextState);
	void BuildCheckedCertificateListL();
	void ComputeAndCheckSubjectKeyIdL();
	
	void DoAddL( const TDesC& aLabel, 
    				TCertificateFormat aFormat,
                	TCertificateOwnerType aCertificateOwnerType, 
                	const TKeyIdentifier* aSubjectKeyId,
                	const TKeyIdentifier* aIssuerKeyId,
                	const TDesC8& aCert,
                	const TBool aDeletable,
                	TRequestStatus& aStatus
                	);

	void CancelOutstandingRequest();

private:  // Generic
    
    /// The certificate store we wrap, owned by us
	MCTCertStore& iCertStore;
    /// Pointer to the same object as iCertStore if it's writable, otherwise NULL
	MCTWritableCertStore* iWritableCertStore;
    /// File server session, used by unified key store
	RFs iFs;
    /// Unified keystore, created and destroyed on demand
	CUnifiedKeyStore* iUnifiedKeyStore;
    /// Current state of the active object
	TState iState;
    /// Request status of client request
	TRequestStatus* iCallerStatus;
    /// Key filter used for listing and adding certs
	TCTKeyAttributeFilter iKeyFilter;
    /// List of key infos returned by keystore
	RMPointerArray<CCTKeyInfo> iKeyInfos;

    //	For Add requests

    /// Format of cert to add
	TCertificateFormat iFormat;
    /// Owner type of cert to add
	TCertificateOwnerType iCertificateOwnerType;
    /// Subject key id of cert to add, not owned by us
	const TKeyIdentifier* iSubjectKeyId;
    /// Issuer key id of cert to add, not owned by us
	const TKeyIdentifier* iIssuerKeyId;
	//  Deletable flag of cert to add	
	TBool iDeletable;


    /// Holds the subject key id extracted from the cert - used if the caller
    /// doesn't supply it
	TKeyIdentifier iComputedSubjectKeyId;
    /// Label of the cert to add, owned by us
	HBufC* iCertLabel;
    /// Certificate data to add, owned by us
	HBufC8* iCertificate;

    // For List requests
    
    /// Pointer to client's cert list, used to return certs, not owned by us
	RMPointerArray<CCTCertInfo>* iCallerCerts;
    /// Pointer to client's cert filter, not owned by us
	const CCertAttributeFilter* iCallerFilter;
    // Reference for the certstore change notification property.
	RProperty& iPSCertstoreChangePropertyRef;

};

#endif	//	__CCHECKEDCERTSTORE_H__
