/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SWICERTSTORE_H__
#define __SWICERTSTORE_H__

#include <mctcertstore.h>

class CSWICertStoreImpl;
class CSWICertStoreToken;

/**  Addtional meta-data information about the Certificate */
class TCertMetaInfo
	{
public:
	TCapabilitySet iCapabilities;
	TBool iIsMandatory;
	TBool iIsSystemUpgrade;
	};

// The certificate is marked for mandatory flag
const TUint8 KMandatory = 0x00;
// The certificate is marked for System Upgrade flag
const TUint8 KSystemUpgrade = 0x01;


/**
 * SWI Cert store token type UID.
 */
const TUint KSWICertStoreTokenTypeUid = 0x102042BA;

/**
 * The software install certificate store.
 *
 * To support platform security, software install uses an independant ROM-based
 * certificate store to find root certs.  In addition to the usual meta-data,
 * this also associates a set of capabilities and a mandatory flag with each
 * cert.
 *
 * Although this class supports the MCTCertStore interface, the implementation
 * is synchronous.
 */
NONSHARABLE_CLASS(CSWICertStore) : protected CBase, public MCTCertStore
	{
public:
	/// Create token type, for use by swicertstoreplugin
	IMPORT_C static CCTTokenType* CreateTokenTypeL();

	/// Create the cert store interface directly without going through ecom
	IMPORT_C static CSWICertStore* NewL(RFs& aFs);

	/// Create the cert store interface, called by token's GetInterface() method
	static CSWICertStore* NewL(CSWICertStoreToken& aToken, RFs& aFs);

	// Implementation of MCTTokenInterface
	virtual MCTToken& Token();
	
	// Implementation of MCTCertStore
	virtual void List(RMPointerArray<CCTCertInfo>& aCerts,
					  const CCertAttributeFilter& aFilter, TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void GetCert(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle, 
						 TRequestStatus& aStatus);		
	virtual void CancelGetCert();
	virtual void Applications(const CCTCertInfo& aCertInfo, RArray<TUid>& aAplications,
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

	// Addtional meta-data accessors
	IMPORT_C const TCertMetaInfo& CertMetaInfoL(const CCTCertInfo& aCertInfo);
	
private:
	// Implementation of MCTTokenInterface
	virtual void DoRelease();

private:
	CSWICertStore(CSWICertStoreToken& aToken);
	virtual ~CSWICertStore();
	void ConstructL(RFs& aFs);

private:
	CSWICertStoreToken& iToken;
	CSWICertStoreImpl* iImpl;
	};

#endif
