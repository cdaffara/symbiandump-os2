/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* The filetokens certstore client interface.
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CFSCERTSTORECLIENT_H__
#define __CFSCERTSTORECLIENT_H__

#include "fsmarshaller.h"
#include "fsdatatypes.h"
#include <mctwritablecertstore.h>
#include "CFSClient.h"

/**
 * The filetokens certstore client interface.
 * 
 * Implements the MCTWritableCertStore interface, with inheritance from
 * CFSClient.  Instances of this class are created by CClientInterfaceFactory.
 * Marshals client data for server requests and unpacks returned parameter data
 * across IPC boundary.
 *
 * Note that although this class is derived from CActive (via CFSClient) is does
 * not act an active object, or use CActive functionality.
 */
NONSHARABLE_CLASS(CFSCertStoreClient) : public CFSClient, public MCTWritableCertStore
{
public:
	static MCTTokenInterface* NewWritableInterfaceL(MCTToken& aToken, RFileStoreClientSession& aClient);
	static MCTTokenInterface* NewReadableInterfaceL(MCTToken& aToken, RFileStoreClientSession& aClient);
public:
	virtual ~CFSCertStoreClient();
public:	
	// From MCTTokenInterface
	virtual MCTToken& Token();
	virtual void DoRelease();
public:
	// From MCTCertStore
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
	virtual void Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert,
							TRequestStatus& aStatus);
	virtual void CancelRetrieve();

	// From MCTWritableCertStore
	virtual void Add(const TDesC& aLabel, TCertificateFormat aFormat,
						TCertificateOwnerType aCertificateOwnerType, 
						const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
						const TDesC8& aCert, TRequestStatus& aStatus);
	virtual void Add(const TDesC& aLabel, TCertificateFormat aFormat,
						TCertificateOwnerType aCertificateOwnerType, 
						const TKeyIdentifier* aSubjectKeyId, const TKeyIdentifier* aIssuerKeyId,
						const TDesC8& aCert, const TBool aDeletable,
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
	
protected:
	// From CActive
	virtual void RunL();
	
private:
	CFSCertStoreClient(TInt aUID, MCTToken& aToken, RFileStoreClientSession& aClient);
	void ConstructL();

	void DoListL(RMPointerArray<CCTCertInfo>& aCerts, const CCertAttributeFilter& aFilter);
	void DoGetCertL(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle);
	void DoApplicationsL(const CCTCertInfo& aCertInfo, RArray<TUid>& aApplications);
	TInt DoRetrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert);
};

#endif	//	__CFSCERTSTORECLIENT_H__
