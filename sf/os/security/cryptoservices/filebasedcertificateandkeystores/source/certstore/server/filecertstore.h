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
* CFSCertStoreServer class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
#ifndef __FILECERTSTORE_H__
#define __FILECERTSTORE_H__

#include <s32file.h>
#include <e32std.h>
#include <securitydefs.h>

class CCertInfo;
class CCertStoreSession;
class CCertStoreConduit;
class CCertStoreEntry;
class CCertStoreEntryList;
class CCertAttributeFilter;
struct TAddCertDataStruct;

/**
 * This class implements a certificate store which is stored on a file on the
 * device.  NewL calls the constructor and ConstructL. The constructor does
 * nothing except initializing to 0. ConstructL calls OpenStoreL, then RestoreL
 * and then sets the values of iStoreInfo.  OpenStoreL opens the store and
 * copies it from the ROM if necessary.  RestoreL()
 */

class CFSCertStoreServer : public CBase
	{
public:
	static CFSCertStoreServer* NewL();
	virtual ~CFSCertStoreServer();

	CCertStoreSession* CreateSessionL();

public:
	// For MCTCertStore

	/// List certs, returned pointers are owned by this class
	void ListL(const CCertAttributeFilter& aFilter, RPointerArray<CCertInfo>& aCertsOut) const;

	/// Get cert info, returned pointer owned by this class, leaves if not found
	const CCertInfo& GetCertL(TInt aHandle) const;

	const RArray<TUid>& ApplicationsL(TInt aHandle) const;	
	TBool IsApplicableL(TInt aHandle, TUid aApplication) const;
	TBool TrustedL(TInt aHandle) const;
	HBufC8* RetrieveLC(TInt aHandle, const RMessage2& aMessage) const;
	
	// For MCTWritableCertStore
	void AddL(const TAddCertDataStruct& aInfo,
			  const TDesC8& aCert,
			  const RMessage2& aMessage);
	void RemoveL(TInt aHandle, const RMessage2& aMessage);
	void SetApplicabilityL(TInt aHandle,
						   const RArray<TUid>& aApplications,
						   const RMessage2& aMessage);
	void SetTrustL(TInt aHandle,
				   TBool aTrusted,
				   const RMessage2& aMessage);

 private:
	CFSCertStoreServer();
	void ConstructL();

	// Security policy
	const TSecurityPolicy& AddRemovePolicy(TCertificateOwnerType aOwnerType) const;
	const TSecurityPolicy& RetrievePolicy(TCertificateOwnerType aOwnerType) const;
	const TSecurityPolicy& WriteTrustSettingsPolicy() const;

	TBool CertEntryMatchesFilter(const CCertAttributeFilter& aFilter,
								 const CCertStoreEntry& aEntry) const;

	void DoAddL(const CCertInfo& aCertInfo, const TDesC8& aCertData);
	TStreamId WriteCertDataStreamL(const TDesC8& aData);
	
	/// Atomically update the cert list and write it to disk.
	void ReplaceCertEntryL(TInt aIndex, CCertStoreEntry* aNewEntry);

	/// Write the cert entry list to the store
	void UpdateStoreL();

	void CompactStoreL();

	static void RevertStore(TAny* aStore);			//	Cleanupitem

	/// Open the store and load the index from a file
	void RestoreL(const TDesC& aFilename);
	/// Open the store file, copying from ROM or creating it first if necessary
	void OpenStoreL();
	/// Opens a Composite store present under ROM drive.
	void OpenCompositeStoreL(const TDesC& aFilename);
	/// Create an empty store file, and leave it closed
	void CreateStoreFileL(const TDesC& aFile);
	void DoCreateStoreFileL(const TDesC& aFile);
	void AggregateStoreFileL(const TDesC& aFile);
	void MergeCertificateEntryListL(const CPermanentFileStore& store, const CCertStoreEntryList& aSourceList);

	
private:
	/// Conduit for marshalling/unmarshalling client communications
	CCertStoreConduit* iConduit;
	
	RFs iFs;
	CPermanentFileStore* iStore;

	/// The list of certificates contained in the store.
	CCertStoreEntryList* iEntryList;

	/// The store stream containing the list of certs
	TStreamId iStreamId;
	
	/// Used to distinguish variant specific feature from the default. 
	TUint8 iPatchableConst;
	};

#endif
