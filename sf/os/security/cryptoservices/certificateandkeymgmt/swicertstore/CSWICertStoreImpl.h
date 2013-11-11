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
* CSWICertStoreImpl class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSWICERTSTOREIMPL_H__
#define __CSWICERTSTOREIMPL_H__

#include <e32std.h>
#include <e32base.h>
#include <s32file.h>
#include <ct/rmpointerarray.h>
#include <e32property.h>

class MCTToken;
class CCTCertInfo;
class CSWICertStoreEntryList;
class CCertAttributeFilter;
class TCTTokenObjectHandle;
class CSWICertStoreEntry;
class TCertMetaInfo;

/**
 * This class implements the guts of the SWI cert store.  It is mainly a cut
 * down version of the filetokens cert store server.
 */
NONSHARABLE_CLASS(CSWICertStoreImpl) : public CActive
	{
public:
	static CSWICertStoreImpl* NewL(MCTToken& aToken, RFs& aFs);
	virtual ~CSWICertStoreImpl();

public:
	// For MCTCertStore
	void ListL(RMPointerArray<CCTCertInfo>& aCerts,
			   const CCertAttributeFilter& aFilter);
	CCTCertInfo* GetCertL(const TCTTokenObjectHandle& aHandle);		
	void ApplicationsL(const TCTTokenObjectHandle& aHandle, RArray<TUid>& aAplications);
	TBool IsApplicableL(const TCTTokenObjectHandle& aHandle, TUid aApplication);
	TBool TrustedL(const TCTTokenObjectHandle& aHandle);
	void RetrieveL(const TCTTokenObjectHandle& aHandle, TDes8& aEncodedCert);

	// Addtional meta-data accessors
	const TCertMetaInfo& CertMetaInfoL(const TCTTokenObjectHandle& aHandle) const;	
	
private:
	CSWICertStoreImpl(MCTToken& aToken, RFs& aFs);
	void ConstructL();

	// Open the Writable certstore and setup the iCEntryList
	void SetupWritableCertStoreL();

	// Open the ROM certstore and setup the iZEntryList
	void SetupROMCertStoreL();
	void SetupCompositeROMCertStoreL();
	static void FilterCertificateListL(RMPointerArray<CCTCertInfo>& aCerts,
			   const CCertAttributeFilter& aFilter, const CSWICertStoreEntryList& aEntryList);
	static void AddIfMatchesFilterL(RMPointerArray<CCTCertInfo>& aCerts,
			   const CCertAttributeFilter& aFilter, const CSWICertStoreEntry& aEntry);
	const CSWICertStoreEntry* GetCSWICertStoreEntryL(const TCTTokenObjectHandle& aHandle, TBool& aCEntryHandle) const;
	void MergeCertificateEntryListsL(const CSWICertStoreEntryList& aSourceList, TBool aIsZEntryList = EFalse );
	
protected:	//	From CActive
	void DoCancel();
	void RunL();

private:
	/// Cert store token
	MCTToken& iToken;
	
	/// File server session
	RFs& iFs;
	
	/// Persistent store containg the certs
	CPermanentFileStore* iCStore;
	CPermanentFileStore* iZStore;

	/// The list of certificates contained in the store.
	CSWICertStoreEntryList* iCEntryList;
	CSWICertStoreEntryList* iZEntryList;
	
	TInt iCertIndex;
	RProperty iSwicertstoreProperty;
	RPointerArray<CPermanentFileStore> iZArrayOfStores;
	/// Used to distinguish variant specific feature from the default. 
	TUint8 iPatchableConst;
	};
	
#endif
