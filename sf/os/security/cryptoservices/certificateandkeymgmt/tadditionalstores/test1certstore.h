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
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __TEST1CERTSTORE_H__
#define __TEST1CERTSTORE_H__

#include <s32file.h>
#include <e32std.h>
#include <ct.h>
#include <mctwritablecertstore.h>
#include <unifiedkeystore.h>

#include "test1certstoretoken.h"

#define CACERTS2 0

class CFileCertStoreMapping;
class CFileCertStoreMappings;

/**
 * This class implements a certificate store which is stored on a file on the device.
 *
 * CTest1CertStore initialization.
 * NewL and NewLC call the constructor and ConstructL. The constructor does nothing
 * except initializing to 0. ConstructL connects to fs, then calls
 * OpenStoreL, then RestoreL and then sets the values of iStoreInfo.
 * OpenStoreL opens the store ad copies it from the ROM if necessary.
 * RestoreL() 
 */
class CTest1CertStore : public CActive, public MCTWritableCertStore
	{
public:
	static CTest1CertStore* NewL(RFs& aFs, CTest1CertStoreToken& aToken, 
		const TDesC& aFileName,	TFileMode aMode);

public:	// from MCTTokenInterface
	virtual MCTToken& Token();
protected:	// from MCTTokenInterface
	virtual void DoRelease();


public:	// from MCTCertStore
	virtual void List(RMPointerArray<CCTCertInfo>& aCerts, 
		const CCertAttributeFilter& aFilter, TRequestStatus& aStatus);
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

	// this could be asynch or sync depending on state of compile flag CERTSTORE_SOFTWARE_ASYNCH
	virtual void Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert,
		TRequestStatus& aStatus);
	virtual void CancelRetrieve();
	virtual void Capabilities(const CCTCertInfo& aCertInfo, TCapabilitySet& aCapbilitiesOut,
							  TRequestStatus& aStatus);
	virtual void CancelCapabilities();
	virtual void IsMandatory(const CCTCertInfo& aCertInfo, TBool& aMandatoryOut,
							 TRequestStatus& aStatus);
	virtual void CancelIsMandatory();

public:	// from MCTWritableCertStore
	virtual void Add(const TDesC& aLabel, TCertificateFormat aFormat,
		TCertificateOwnerType aCertificateOwnerType, 
		const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
		const TDesC8& aCert, TRequestStatus& aStatus);
	virtual void CancelAdd();
	virtual void Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus);
	virtual void CancelRemove();
	virtual void SetApplicability(const CCTCertInfo& aCertInfo, 
		const RArray<TUid>& aApplications, TRequestStatus &aStatus);
	virtual void CancelSetApplicability();
	virtual void SetTrust(const CCTCertInfo& aCertInfo, TBool aTrusted, 
		TRequestStatus& aStatus);
	virtual void CancelSetTrust();
	virtual void SetCapabilities(const CCTCertInfo& aCertInfo, const TCapabilitySet& aCapabilities, 
								 TRequestStatus& aStatus);
	virtual void CancelSetCapabilities();
	virtual void SetMandatory(const CCTCertInfo& aCertInfo, TBool aMandatory,
							  TRequestStatus& aStatus);
	virtual void CancelSetMandatory();

private:
	virtual ~CTest1CertStore();
	void DoSetApplicabilityL(const CCTCertInfo& aCertInfo, 
		const RArray<TUid>& aApplications);
	void DoSetTrustL(const CCTCertInfo& aCertInfo, 
		TBool aTrusted);
	CTest1CertStore(CTest1CertStoreToken& aToken, RFs& aFs);
	void ConstructL(const TDesC& aFileName, TFileMode aMode);
	static void RevertStore(TAny* aStore);
	static void DeleteFile(TAny* aThis);
	void RestoreL();
	CPermanentFileStore* OpenStoreL(const TDesC& aFileName, TFileMode aMode);
	CPermanentFileStore* OpenStoreLC(const TDesC& aFileName, TFileMode aMode);


	//
//	void AddCACertL(const CCertificate& aCert, const TDesC& aLabel, TCertificateFormat aFormat);
//	void AddUserCertL(const CCertificate& aCert, const TDesC& aLabel, TCertificateFormat aFormat, 
//		const TKeyIdentifier& aIssuerKeyHash, const TKeyIdentifier& aSubjectKeyHash);
	void AddUserCertURLL(const TDesC8& aCert, const TDesC& aLabel,
		const TKeyIdentifier& aIssuerKeyHash, const TKeyIdentifier& aSubjectKeyHash);
//	void AddCertL(CCTCertInfo* aCertInfo, const CCertificate& aCert, CFileCertStoreMappings& aMappings);
	void AddCertL(CCTCertInfo* aCertInfo, const TDesC8& aCert, CFileCertStoreMappings& aMappings);
	void DoRemoveL(const CCTCertInfo& aCertInfo);
	void DoLoadL(TDes8& aCertificate, CFileCertStoreMapping& aMapping) const;
	void DoGetUserCertsL(RPointerArray<CCTCertInfo>& aCertInfos,
		const CCertAttributeFilter& aFilter) const;
	void UpdateStoreL();
	void DoAddL(const TDesC& aLabel, TCertificateFormat aFormat,
		TCertificateOwnerType aCertificateOwnerType, const TKeyIdentifier* aSubjectKeyId,
		const TKeyIdentifier* aIssuerKeyId,const TDesC8& aCert);

	// perform the retrieval synchronously
	void RetrieveNow(const CCTCertInfo& aCertInfo,TDes8& aCertificate,TRequestStatus& aStatus);

private:
	void RunL();
	void DoCancel();

private:
	enum TState
		{
		EGetKeyInfos,
		EList,
		ERetrieve
		};

	//
private:
	CTest1CertStoreToken& iToken;

	RFile iFile;
	RFs& iFs;
	CPermanentFileStore* iStore;

	/**
	 * The list of certificates contained in the store.
	 */
	CFileCertStoreMappings* iCerts;

	TState iState;
	TRequestStatus* iOriginalRequestStatus;
	RPointerArray<CCTCertInfo>* iCertInfos;
	const CCertAttributeFilter* iFilter;
	TCTKeyAttributeFilter iKeyFilter;
	RMPointerArray<CCTKeyInfo> iKeyInfos;
	CUnifiedKeyStore* iUnifiedKeyStore;
	
	// used for asynchronous certificate retrieval
	const CCTCertInfo* iAsynchCertInfo;
	TDes8* iAsynchCertificate;
	};

#endif
