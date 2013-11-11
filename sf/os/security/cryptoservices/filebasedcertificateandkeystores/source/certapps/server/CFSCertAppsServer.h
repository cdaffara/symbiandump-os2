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
* CFSCertAppsServer class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CFSCERTAPPSSERVER_H__
#define __CFSCERTAPPSSERVER_H__

#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <mctcertapps.h>

class CFSCertAppsToken;
class CCertAppsSession;
class CCertAppsConduit;

class CFSCertAppsServer : public CBase
	{
public:
	static CFSCertAppsServer* NewL();
	virtual ~CFSCertAppsServer();
	CCertAppsSession* CreateSessionL();

public: 
	void AddL(const TCertificateAppInfo& aClient);
	void RemoveL(const TUid& aUid);
	TInt ApplicationCountL() const;
	void ApplicationsL(RArray<TCertificateAppInfo>& aAppArray) const;
	void ApplicationL(const TUid& aUid, TCertificateAppInfo& aInfo) const;

private:
	CFSCertAppsServer();
	void ConstructL();

	/// Finds an application and returns NULL if not found - if aIndex is not
	/// null then index is copied there
	const TCertificateAppInfo* FindApplication(const TUid& aUid, TInt* aIndex = NULL) const;

	/// Configures the object from the file store. Will create a new store
	/// if one does not exist
	void OpenStoreL();
	/// Opens a Composite store present under ROM drive.
	void OpenCompositeStoreL(const TDesC& aFilename);
	/// Attempt to load the cert apps from the given file
	void ReadStoreContentsL(const TDesC& aFilename);
	/// Create a new empty store
	void CreateStoreL(const TDesC& aFilename);
	/// aggregate the store files
	void AggregateStoreFileL(const TDesC& aFile);
	/// aggregate certificate client entries.
	void MergeCertificateEntryListL(const RArray<TCertificateAppInfo>& aSourceList);	
	/// finds if an uid exists in the entry list 
	TBool FindUid(const TUid& aUid);
	// TCleanup items used when creating a store or replacing a stream
	static void DeleteStoreFile(TAny* aThis);
	static void RevertStore(TAny* aStore);

	/// Replaces the store - called by both Add and Remove.
	/// Leaves if failed. ExcludedIndex is the index into the 
	/// array to be excluded. -1 if none are excluded
	void ReplaceAndCommitL(TInt aExcludedIndex = -1);

	/// Externalizes the array into a stream - commits it if successful
	void WriteClientArrayL(RWriteStream& stream, TInt aExcludedIndex = -1) const;

private:
	/// The conduit object used for marshalling/unmarshalling client communications
	CCertAppsConduit* iConduit;
	RFs iFs;
	/// The store where the client are stored
	CPermanentFileStore* iStore;
	TStreamId iId;	
	/// A cache of the contents of the store
	RArray<TCertificateAppInfo> iClients;
	/// Used to distinguish variant specific feature from the default. 
	TUint8 iPatchableConst;
	};

#endif	//	__CFSCERTAPPSSERVER_H__
