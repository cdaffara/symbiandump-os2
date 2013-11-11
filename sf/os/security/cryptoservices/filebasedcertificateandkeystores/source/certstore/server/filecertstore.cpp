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


#include "filecertstore.h"
#include "CCertStoreConduit.h"
#include "CCertStoreSession.h"
#include "CCertStoreEntry.h"
#include "CCertStoreEntryList.h"
#include "fstokencliserv.h"
#include "fstokenutil.h"
#include "fsdatatypes.h"

#include <certstorepatchdata.h>
#include <ccertattributefilter.h>
#include <signed.h>
#include <x509cert.h>
#include <x509certext.h>
#include <x509keys.h>
#include <wtlscert.h>
#include <u32hal.h> 
#include <e32svr.h>

_LIT(KCertStoreFilename,"CACerts.dat");
_LIT(KCertStoreFilenamePattern,"cacerts*.dat");

/////////////////////////////////////////////////////////////////////////////////////////
//CFSCertStoreServer
/////////////////////////////////////////////////////////////////////////////////////////

CFSCertStoreServer* CFSCertStoreServer::NewL()
	{
	CFSCertStoreServer* self = new (ELeave) CFSCertStoreServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFSCertStoreServer::CFSCertStoreServer()
	{
	}

void CFSCertStoreServer::ConstructL()
	{
	iConduit = CCertStoreConduit::NewL(*this);
	User::LeaveIfError(iFs.Connect());
	
	iPatchableConst = KAggregateCertStore;
	
	#ifdef __WINS__
		// For the emulator allow the constant to be patched via epoc.ini
		UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
		(TAny*)"KAggregateCertStore", &iPatchableConst); // read emulator property (if present)
	#endif		
	
	OpenStoreL();
	}

CFSCertStoreServer::~CFSCertStoreServer()
	{
	delete iStore;
	delete iEntryList;
	delete iConduit;
	iFs.Close();
	}

CCertStoreSession* CFSCertStoreServer::CreateSessionL()
	{
	return CCertStoreSession::NewL(*iConduit);
	}

// API policing ////////////////////////////////////////////////////////////////

_LIT_SECURITY_POLICY_PASS(KPolicyAlwaysPass);
_LIT_SECURITY_POLICY_FAIL(KPolicyAlwaysFail);
_LIT_SECURITY_POLICY_C1(KPolicyRequireReadUserData, ECapabilityReadUserData);
_LIT_SECURITY_POLICY_C1(KPolicyRequireWriteUserData, ECapabilityWriteUserData);
_LIT_SECURITY_POLICY_C1(KPolicyRequireWriteDeviceData, ECapabilityWriteDeviceData);

const TSecurityPolicy& CFSCertStoreServer::AddRemovePolicy(TCertificateOwnerType aOwnerType) const
	{
	switch (aOwnerType)
		{
		case ECACertificate:
			return KPolicyRequireWriteDeviceData;

		case EUserCertificate:
		case EPeerCertificate:
			return KPolicyRequireWriteUserData;

		default:
			return KPolicyAlwaysFail;
		}
	}

const TSecurityPolicy& CFSCertStoreServer::RetrievePolicy(TCertificateOwnerType aOwnerType) const
	{
	switch (aOwnerType)
		{
		case ECACertificate:
			return KPolicyAlwaysPass;

		case EUserCertificate:
		case EPeerCertificate:
			return KPolicyRequireReadUserData;

		default:
			return KPolicyAlwaysFail;
		}
	}

const TSecurityPolicy& CFSCertStoreServer::WriteTrustSettingsPolicy() const
	{
	return KPolicyRequireWriteDeviceData;
	}


// Read-only interface /////////////////////////////////////////////////////////

void CFSCertStoreServer::ListL(const CCertAttributeFilter& aFilter,
							   RPointerArray<CCertInfo>& aCertsOut) const
	{
	
	// check that if KeyUsage is set, only User certificates are requested
	if (aFilter.iKeyUsage != EX509UsageAll &&
		(!aFilter.iOwnerTypeIsSet || aFilter.iOwnerType != EUserCertificate))
		{
		User::Leave(KErrArgument);
		}
	
	TInt count = iEntryList->Count();
	for (TInt index = 0; index < count; index++)
		{
		const CCertStoreEntry& entry = iEntryList->GetByIndex(index);

		if (CertEntryMatchesFilter(aFilter, entry))
			{
			User::LeaveIfError(aCertsOut.Append(&entry.CertInfo()));
			}
		}
	}

TBool CFSCertStoreServer::CertEntryMatchesFilter(const CCertAttributeFilter& aFilter,
												 const CCertStoreEntry& aEntry) const
	{
	if (aFilter.iUidIsSet && !aEntry.IsApplicable(aFilter.iUid))
		{
		return EFalse;
		}
	
	const CCertInfo& certInfo = aEntry.CertInfo();
	
	if (aFilter.iFormatIsSet && aFilter.iFormat != certInfo.CertificateFormat())
		{
		return EFalse;
		}
	
	if (aFilter.iOwnerTypeIsSet && aFilter.iOwnerType != certInfo.CertificateOwnerType())
		{
		return EFalse;
		}
	
	if (aFilter.iSubjectKeyIdIsSet && aFilter.iSubjectKeyId != certInfo.SubjectKeyId())
		{
		return EFalse;
		}

	if (aFilter.iLabelIsSet && aFilter.iLabel != certInfo.Label())
		{
		return EFalse;
		}

	if (aFilter.iIssuerKeyIdIsSet && aFilter.iIssuerKeyId != certInfo.IssuerKeyId())
		{
		return EFalse;
		}
	
	return ETrue;
	}

const CCertInfo& CFSCertStoreServer::GetCertL(TInt aHandle) const 
	{
	// Leaves if not found
	return iEntryList->GetByHandleL(aHandle).CertInfo();
	}

const RArray<TUid>& CFSCertStoreServer::ApplicationsL(TInt aHandle) const
	{
	return iEntryList->GetByHandleL(aHandle).CertificateApps();
	}

TBool CFSCertStoreServer::IsApplicableL(TInt aHandle, TUid aApplication) const
	{
	return iEntryList->GetByHandleL(aHandle).IsApplicable(aApplication);
	}

TBool CFSCertStoreServer::TrustedL(TInt aHandle) const
	{	
	return iEntryList->GetByHandleL(aHandle).Trusted();
	}

HBufC8* CFSCertStoreServer::RetrieveLC(TInt aHandle, const RMessage2& aMessage) const
	{
	const CCertStoreEntry& entry = iEntryList->GetByHandleL(aHandle);

	// API policing
	if (!RetrievePolicy(entry.CertInfo().CertificateOwnerType()).CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}
	
	TInt size = entry.CertInfo().Size();
	HBufC8* buf = HBufC8::NewMaxLC(size);
	TPtr8 ptr = buf->Des();
	ptr.FillZ();
	
	RStoreReadStream stream;
	stream.OpenLC(*iStore, entry.DataStreamId());
	stream.ReadL(ptr, size);
	CleanupStack::PopAndDestroy(&stream);

	return buf;
	}

// Writable interface //////////////////////////////////////////////////////////

void CFSCertStoreServer::AddL(const TAddCertDataStruct& aInfo,
							  const TDesC8& aCert,
							  const RMessage2& aMessage)
	{
	// Check if a certificate with this name already exists
	
	if (iEntryList->LabelExists(aInfo.iLabel))
		{
		User::Leave(KErrBadName);
		}
	// Check subject key id and cert data are supplied, issuer key id is optional
	if (aInfo.iSubjectKeyId == KNullDesC8 || aCert == KNullDesC8)
		{
		User::Leave(KErrArgument);
		}

	// Create cert entry (this sanity checks the rest of the arguments)
	CCertInfo* certInfo = CCertInfo::NewLC(aInfo.iLabel,
										   aInfo.iFormat, 
										   aInfo.iCertificateOwnerType,
										   aCert.Length(),
										   &aInfo.iSubjectKeyId,
										   &aInfo.iIssuerKeyId,
										   iEntryList->NextFreeHandle(),
										   aInfo.iDeletable);

	// API policing
	if (!AddRemovePolicy(aInfo.iCertificateOwnerType).CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

	CompactStoreL();
	
	TRAPD(err, DoAddL(*certInfo, aCert));
	CleanupStack::PopAndDestroy(certInfo);

	if (err != KErrNone)
		{
		iStore->Revert();
		User::Leave(err);
		}
	}

void CFSCertStoreServer::DoAddL(const CCertInfo& aCertInfo, const TDesC8& aCertData)
	{
	TStreamId dataStreamId = WriteCertDataStreamL(aCertData);

	RArray<TUid> initialApps;
	CleanupClosePushL(initialApps);
	
	CCertStoreEntry* entry = CCertStoreEntry::NewL(aCertInfo,
												   initialApps,
												   EFalse,
												   dataStreamId);
	CleanupStack::PopAndDestroy(&initialApps);
	CleanupStack::PushL(entry);
	
	TInt index = iEntryList->AppendL(entry);
	CleanupStack::Pop(entry); // iEntryList has taken ownership
	
	TRAPD(err, UpdateStoreL());
	if (err != KErrNone)
		{
		iEntryList->Remove(index);
		delete entry;
		User::Leave(err);
		}
	}

TStreamId CFSCertStoreServer::WriteCertDataStreamL(const TDesC8& aData)
	{
	RStoreWriteStream stream;
	TStreamId streamId = stream.CreateLC(*iStore);
	stream.WriteL(aData);
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);
	return streamId;
	}	

void CFSCertStoreServer::RemoveL(TInt aHandle, const RMessage2& aMessage)
	{
	TInt index = iEntryList->IndexForHandle(aHandle);
	User::LeaveIfError(index);

	// API policing
	const CCertStoreEntry& entry = iEntryList->GetByIndex(index);


	if (!AddRemovePolicy(entry.CertInfo().CertificateOwnerType()).CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}
	
	TCleanupItem cleanupStore(RevertStore, iStore);
	CleanupStack::PushL(cleanupStore);	
	
	TStreamId datastreamid = entry.DataStreamId();
	// Data stream needs to be deleted which was written while adding the certificate.
	iStore->DeleteL(datastreamid);

	CCertStoreEntry* oldEntry = iEntryList->Remove(index);
	TRAPD(err, UpdateStoreL());
	if (err == KErrNone)
		{
		delete oldEntry;
		}
	else
		{
		// This will always succeed because we just did a remove
		iEntryList->AppendL(oldEntry);
		User::Leave(err);
		}
	CleanupStack::Pop(); // cleanupStore	
	CompactStoreL();
	}

void CFSCertStoreServer::SetApplicabilityL(TInt aHandle,
										   const RArray<TUid>& aApps,
										   const RMessage2& aMessage)
	{
	TInt index = iEntryList->IndexForHandle(aHandle);
	User::LeaveIfError(index);
	const CCertStoreEntry& entry = iEntryList->GetByIndex(index);

	if (entry.CertInfo().CertificateOwnerType() != ECACertificate)
		{
		User::Leave(KErrArgument);
		}

	// API policing
	if (!WriteTrustSettingsPolicy().CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}	

	CompactStoreL();
	
	CCertStoreEntry* newEntry = CCertStoreEntry::NewL(entry.CertInfo(),
													  aApps,
													  entry.Trusted(),
													  entry.DataStreamId());
	ReplaceCertEntryL(index, newEntry);
	}

void CFSCertStoreServer::SetTrustL(TInt aHandle,
								   TBool aTrusted,
								   const RMessage2& aMessage)
	{
	TInt index = iEntryList->IndexForHandle(aHandle);
	User::LeaveIfError(index);
	const CCertStoreEntry& entry = iEntryList->GetByIndex(index);

	if (entry.CertInfo().CertificateOwnerType() != ECACertificate)
		{
		User::Leave(KErrArgument);
		}

	// API policing
	if (!WriteTrustSettingsPolicy().CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}	

	CompactStoreL();

	CCertStoreEntry* newEntry = CCertStoreEntry::NewL(entry.CertInfo(),
													  entry.CertificateApps(),
													  aTrusted,
													  entry.DataStreamId());
	ReplaceCertEntryL(index, newEntry);
	}

void CFSCertStoreServer::ReplaceCertEntryL(TInt aIndex, CCertStoreEntry* aNewEntry)
	{
	assert(aIndex >= 0 && aNewEntry, EPanicCertStoreReplaceArguments);
	CCertStoreEntry* oldEntry = iEntryList->Replace(aIndex, aNewEntry);
	TRAPD(err, UpdateStoreL());
	if (err == KErrNone)
		{
		delete oldEntry;		
		}
	else
		{
		iStore->Revert();
		iEntryList->Replace(aIndex, oldEntry);
		delete aNewEntry;
		User::Leave(err);
		}
	}

// This is a cleanup item that reverts the store
void CFSCertStoreServer::RevertStore(TAny* aStore)
	{
	CPermanentFileStore* store = reinterpret_cast<CPermanentFileStore*>(aStore);
	
	// We're ignoring the leave code from this becuase there's no way we can
	// handle this sensibly.  This shouldn't be a problem in practice - this
	// will leave if for example the file store is on removable which is
	// unexpectedly remove, and this is never the case for us.
	TRAP_IGNORE(store->RevertL());
	}

void CFSCertStoreServer::UpdateStoreL()
	{
	RStoreWriteStream stream;
	stream.ReplaceLC(*iStore, iStreamId);
	stream << *iEntryList;
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream);
	
	iStore->CommitL();
	}

void CFSCertStoreServer::CompactStoreL()
	{
	iStore->ReclaimL();
	iStore->CompactL();
	iStore->CommitL();
	}

void CFSCertStoreServer::RestoreL(const TDesC& aFilename)
	{
	// Make sure the store is not read-only
	User::LeaveIfError(iFs.SetAtt(aFilename, KEntryAttNormal, KEntryAttReadOnly));

	// Open the store
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFilename, EFileRead | EFileWrite));
	CleanupClosePushL(file);
	CPermanentFileStore* store = CPermanentFileStore::FromL(file);
	CleanupStack::Pop(&file); // now owned by store
	CleanupStack::PushL(store);

	// Read id of cert list stream
	TStreamId caCertEntryStreamId;
	RStoreReadStream stream;
	stream.OpenLC(*store, store->Root());
	stream >> iStreamId;
	CleanupStack::PopAndDestroy(&stream);

	// Read the certificate list
	RStoreReadStream caCertEntryStream;
	caCertEntryStream.OpenLC(*store, iStreamId);
	iEntryList = CCertStoreEntryList::NewL(caCertEntryStream);
	CleanupStack::PopAndDestroy(&caCertEntryStream);

	assert(!iStore, EPanicCertStoreRestoreState);
	iStore = store;
	CleanupStack::Pop(store);
	}

void CFSCertStoreServer::AggregateStoreFileL(const TDesC& aFile)
	{
	ASSERT(iPatchableConst);
	
	// if patchable constant is enabled
	// 1. open read-only permanent file store on each file.
	// 2. open certificate client entry list of each store.
	// 3. aggregate the entries.	
	RFile file;
	User::LeaveIfError(file.Open(iFs, aFile, EFileRead));
	CleanupClosePushL(file);
	CPermanentFileStore* store = CPermanentFileStore::FromL(file);
	// now owned by store
	CleanupStack::Pop(&file); 
	CleanupStack::PushL(store);

	// Read id of cert list stream
	TStreamId streamId;
	RStoreReadStream stream;
	stream.OpenLC(*store, store->Root());
	stream >> streamId;
	CleanupStack::PopAndDestroy(&stream);
	// Read the certificate entry list
	stream.OpenLC(*store, streamId);
	CCertStoreEntryList* entryList = CCertStoreEntryList::NewL(stream);
	CleanupStack::PushL(entryList);
	
	MergeCertificateEntryListL(*store,*entryList);
	// cleanup entrylist, stream and store instances
	CleanupStack::PopAndDestroy(3, store);
	}

void CFSCertStoreServer::MergeCertificateEntryListL(const CPermanentFileStore& aStore, const CCertStoreEntryList& aSourceList)
	{
	ASSERT(iPatchableConst);
	
	// if patchable constant is enabled
	TInt sourceCount = aSourceList.Count();
	for(TInt i = 0; i < sourceCount; i++)
		{
		if (!iEntryList->LabelExists(aSourceList.GetByIndex(i).CertInfo().Label()))
			{
			// Aggregation:	
			// 1. write the certificate data of this new entry to composite store i.e. 'iStore'.
			// 2. append this new entry to composite entry list i.e. 'iEntryList'
		
			const CCertStoreEntry& entry = aSourceList.GetByIndex(i);

			TInt size = entry.CertInfo().Size();
			HBufC8* buf = HBufC8::NewMaxLC(size);
			TPtr8 ptr = buf->Des();
			RStoreReadStream stream;
			stream.OpenLC(aStore, entry.DataStreamId());
			stream.ReadL(ptr, size);
			CleanupStack::PopAndDestroy(&stream);
			TStreamId dataStreamId = WriteCertDataStreamL(*buf);
			CleanupStack::PopAndDestroy(buf);
			
			// create a new entry to be appended to the composite list	
			// the new entry must have unique certificate id in this list.
			const CCertInfo& certInfo = entry.CertInfo();
			CCertInfo* info = CCertInfo::NewLC(
												certInfo.Label(),
												certInfo.CertificateFormat(),
												certInfo.CertificateOwnerType(),
												certInfo.Size(),
												&certInfo.SubjectKeyId(),
												&certInfo.IssuerKeyId(),
												iEntryList->NextFreeHandle(),
												certInfo.IsDeletable()
												);
		
			CCertStoreEntry* newEntry = CCertStoreEntry::NewL(
												*info,
												entry.CertificateApps(),
												entry.Trusted(),
												dataStreamId
												);
			
			CleanupStack::PopAndDestroy(info);
			CleanupStack::PushL(newEntry);
			iEntryList->AppendL(newEntry);
			// iEntryList has taken ownership
			CleanupStack::Pop(newEntry); 
			}
		// Eclipsing: do not add this certificate in composite filecertstore.
		// Higher order store certificates with same labels take precedence over lower order 
		// store certificates therefore the later are not included in the composite filecertstore.
		// Higher order store certificates are ones which are aggregated prior to other certificates. 
		}
	}

void CFSCertStoreServer::OpenCompositeStoreL(const TDesC& aFilename)
	{
	ASSERT(iPatchableConst);
	
	// 1. create a new empty certstore file under system drive with the name 'CAcerts.dat'. 
	// 2. this will be the composite store and the instances 'iEntryList' and 'iStore' will be initialized with this.
	// 3. make private rom drive path where certstore files are located.
	// 4. collect the certstore file names in a list.
	// 5. make private rom drive path on each file.
	// 6. populate the composite store with certificate client entries present in rom drive certstores.
	
	// create a new empty certstore file 'CAcerts.dat' under system drive.
	CreateStoreFileL(aFilename);
	// restore permanent store on it 
	// this will be the composite store after complete aggregation. 
	RestoreL(aFilename);

	RBuf romFilename;
	romFilename.CreateL(KMaxFileName);
	CleanupClosePushL(romFilename);		
	FileUtils::MakePrivateROMFilenameL(iFs, KCertStoreFilenamePattern, romFilename);
	CDir* filenameList = NULL;
	User::LeaveIfError(iFs.GetDir(romFilename, KEntryAttNormal, ESortByName|EDescending, filenameList));	
	CleanupStack::PopAndDestroy(&romFilename);
	CleanupStack::PushL(filenameList);
	TInt count = filenameList->Count();
	
	// aggregate ROM stores iteratively 
	for(TInt index = 0; index < count; index++)		
		{
		RBuf fileName;
		fileName.CreateL(KMaxFileName);
		CleanupClosePushL(fileName);		
		FileUtils::MakePrivateROMFilenameL(iFs, ((*filenameList)[index]).iName, fileName);
		// if there is any corrupt certstore present then we will simply ignore its
		// aggregation and proceed with aggregating remaining stores.
		TRAP_IGNORE(AggregateStoreFileL(fileName));
		CleanupStack::PopAndDestroy(&fileName);	
		}
	// write the 'iEntryList' to the composite store.	
	UpdateStoreL();
	CompactStoreL();
	CleanupStack::PopAndDestroy(filenameList);
	}

void CFSCertStoreServer::OpenStoreL()
	{
	RBuf filename;
	filename.CreateL(KMaxFilenameLength);
	CleanupClosePushL(filename);		
	FileUtils::MakePrivateFilenameL(iFs, KCertStoreFilename, filename);

	// Attempt to open the store
	// need to test opening corrupt store
	TRAPD(err, RestoreL(filename));

	if (err == KErrNoMemory || err == KErrInUse)
		{
		User::Leave(err);
		}
	
	if (err != KErrNone)
		{
		// Couldn't open RAM based store, copy from ROM	
		FileUtils::EnsurePathL(iFs, filename);		
	
		// if patchable constant is enabled
		if(iPatchableConst)
			{
			OpenCompositeStoreL(filename);
			}
		else
			{	
			RBuf romFilename;
			romFilename.CreateL(KMaxFilenameLength);
			CleanupClosePushL(romFilename);		
			FileUtils::MakePrivateROMFilenameL(iFs, KCertStoreFilename, romFilename);

			if (FileUtils::ExistsL(iFs, romFilename))
				{
				FileUtils::CopyL(iFs, romFilename, filename);
				}	
			else
				{
				// if none of the certstore files are present then create an empty one.
				CreateStoreFileL(filename);
				}
			CleanupStack::PopAndDestroy(&romFilename);
			//Retry open, and leave on failure
			RestoreL(filename);
			}
		}
	CleanupStack::PopAndDestroy(&filename);
	assert(iStore, EPanicCertStoreOpenState);
	}

void CFSCertStoreServer::CreateStoreFileL(const TDesC& aFile)
	{
	TRAPD(err, DoCreateStoreFileL(aFile));
	if (err != KErrNone)
		{
		// Attempt to delete file, but don't complain if it fails
		iFs.Delete(aFile);  
		User::Leave(err);
		}
	}

void CFSCertStoreServer::DoCreateStoreFileL(const TDesC& aFile)
	{
 	CPermanentFileStore* fileStore = CPermanentFileStore::ReplaceLC(iFs, aFile, EFileRead | EFileWrite | EFileShareExclusive);
	fileStore->SetTypeL(KPermanentFileStoreLayoutUid);
	
	// Create info stream
	CCertStoreEntryList* emptyCertList = CCertStoreEntryList::NewLC();	
	RStoreWriteStream infoStream;
	TStreamId streamId = infoStream.CreateLC(*fileStore);
	infoStream << *emptyCertList;
	infoStream.CommitL();
	CleanupStack::PopAndDestroy(2, emptyCertList);

	// Create root stream - just contains id of info stream
	RStoreWriteStream rootStream;
	TStreamId rootStreamId = rootStream.CreateLC(*fileStore);
	fileStore->SetRootL(rootStreamId);
	rootStream << streamId;
	rootStream.CommitL();
	CleanupStack::PopAndDestroy(&rootStream);

	fileStore->CommitL();
	CleanupStack::PopAndDestroy(fileStore);
	}
