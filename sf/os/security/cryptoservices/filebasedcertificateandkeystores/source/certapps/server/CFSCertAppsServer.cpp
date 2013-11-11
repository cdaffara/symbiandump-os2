/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CFSCertAppsServer.h"
#include "CCertAppsSession.h"
#include "CCertAppsConduit.h"
#include "fstokencliserv.h"
#include "fstokenutil.h"

#include <certstorepatchdata.h>
#include <certificateapps.h>
#include <u32hal.h> 
#include <e32svr.h>

// Filename where all data is stored
_LIT(KCertAppsFilename,"certclients.dat");
_LIT(KCertAppsFile,"certclients*.dat");

CFSCertAppsServer* CFSCertAppsServer::NewL()
	{
	CFSCertAppsServer* self = new (ELeave) CFSCertAppsServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CFSCertAppsServer::CFSCertAppsServer()
	{
	}

void CFSCertAppsServer::ConstructL()
	{
	iConduit = CCertAppsConduit::NewL(*this);
	
	// Connect to the filesystem
	User::LeaveIfError(iFs.Connect());

	iPatchableConst = KAggregateCertStore;
	
	#ifdef __WINS__
		// For the emulator allow the constant to be patched via epoc.ini
		UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
		(TAny*)"KAggregateCertStore", &iPatchableConst); // read emulator property (if present)
	#endif		
	
	// Retrieves the store
	OpenStoreL();
	}

CFSCertAppsServer::~CFSCertAppsServer()
	{
	delete iStore;
 	iFs.Close();
	iClients.Close();
	delete iConduit;
	}

CCertAppsSession* CFSCertAppsServer::CreateSessionL()
	{
	return CCertAppsSession::NewL(*iConduit); 
	}

void CFSCertAppsServer::AddL(const TCertificateAppInfo& aClient)
	{
	// see if application already exists. If so, then leave
	
	if (FindApplication(aClient.Id()))
		{
		User::Leave(KErrAlreadyExists);
		}
	User::LeaveIfError(iClients.Append(aClient));
	TRAPD(err, ReplaceAndCommitL(-1));
	if (err != KErrNone)
		{
		// We must remove the client from iClients if we didn't manage 
		// to add it to the store
		iClients.Remove(iClients.Count() - 1);
		User::Leave(err);
		}
	}

void CFSCertAppsServer::RemoveL(const TUid& aUid)
	{
	// Make sure the application for that uid exists
	TInt i;
	if (!FindApplication(aUid, &i))
		{
		User::Leave(KErrNotFound);
		}

	ReplaceAndCommitL(i);
	
	// We managed to remove it from the store, so we remove it from the
	// iClients array
	iClients.Remove(i);
	}

TInt CFSCertAppsServer::ApplicationCountL() const
	{
	return iClients.Count();
	}

void CFSCertAppsServer::ApplicationsL(RArray<TCertificateAppInfo>& aAppArray) const
	{
	// Make a copy of the array
	TInt count = iClients.Count();

	for (TInt i = 0; i < count; ++i)
		{
		aAppArray.AppendL(iClients[i]);
		}
	}

void CFSCertAppsServer::ApplicationL(const TUid& aUid, TCertificateAppInfo& aInfo) const
	{
	const TCertificateAppInfo* app = FindApplication(aUid);

	// leave if not found
	if (!app)
		{
		User::Leave(KErrNotFound);
		}

	// make a copy and return to client
	aInfo = *app;
	}

const TCertificateAppInfo* CFSCertAppsServer::FindApplication(const TUid& aUid, TInt* aIndex) const
	{
	// This helper function tries to find an application with the given
	// Uid. It returns NULL if not found. aIndex returns the index into
	// the array successful
	const TCertificateAppInfo* retVal = NULL;
	TInt end = iClients.Count();
	for (TInt i = 0; i < end; ++i)
		{
		if (iClients[i].Id() == aUid)
			{
			// check if an index is required to be returned
			if (aIndex)
				{
				*aIndex = i;
				}
			retVal = &iClients[i];
			break;
			}
		}

	return retVal;
	}

void CFSCertAppsServer::AggregateStoreFileL(const TDesC& aFile)
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
	
	// Read the certificate's clients entry list
	stream.OpenLC(*store, streamId);
	TInt count = stream.ReadInt32L();
	RArray<TCertificateAppInfo> entryList;
	for (TInt i = 0 ; i < count ; i++)
		{
		TCertificateAppInfo clientInfo;
		stream >> clientInfo;
		entryList.AppendL(clientInfo);
		}
	CleanupStack::PopAndDestroy(&stream);
	CleanupClosePushL(entryList);

	MergeCertificateEntryListL(entryList);
	// cleanup entryList and store instances.
	CleanupStack::PopAndDestroy(2,store);		
	}

TBool CFSCertAppsServer::FindUid(const TUid& aUid)
	{
	TInt end = iClients.Count();
	for (TInt i = 0; i < end; ++i)
		{
		if (iClients[i].Id() == aUid)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

void CFSCertAppsServer::MergeCertificateEntryListL(const RArray<TCertificateAppInfo>& aSourceList)
	{
	ASSERT(iPatchableConst);

	// if patchable constant is enabled
	TInt sourceCount = aSourceList.Count();
	for(TInt i = 0; i < sourceCount; i++)
		{
		// compare if the uid pre-exists in the composite list.
		if (!FindUid(aSourceList[i].Id()))
			{
			// Aggregation: append this entry to the composite list.
			iClients.AppendL(aSourceList[i]);
			}
		// Eclipsing: Higher order store cert client entries with same UIDs take precedence over lower order  
		// store cert client entries therefore the later are not included in the composite cert client entry list.
		// Higher order store client entries are ones which are aggregated prior to other client entries. 
		}
	}

void CFSCertAppsServer::OpenCompositeStoreL(const TDesC& aFilename)
	{
	ASSERT(iPatchableConst);

	// 1. create a new empty certstore file under system drive with the name 'certclients.dat'. 
	// 2. this will be the composite store and the instances 'iEntryList' and 'iStore' will be initialized with this.
	// 3. make private rom drive path where certstore files are located.
	// 4. collect the certstore file names in a list.
	// 5. make private rom drive path on each file.
	// 6. populate the composite store with certificate client entries present in rom drive certstores.
	
	// create a new empty certstore file 'certclients.dat' under system drive.
	CreateStoreL(aFilename);	
	// restore permanent store on it 
	// this will be the composite store after complete aggregation. 
	ReadStoreContentsL(aFilename);
		
	RBuf romFilename;
	romFilename.CreateL(KMaxFilenameLength);
	CleanupClosePushL(romFilename);
	FileUtils::MakePrivateROMFilenameL(iFs, KCertAppsFile, romFilename);
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
	// write the 'iClients' to the composite store.	
	ReplaceAndCommitL(-1);	
	CleanupStack::PopAndDestroy(filenameList);
	}

// this logic should be handled by a superclass or mixin
void CFSCertAppsServer::OpenStoreL()
	{
	RBuf filename;
	filename.CreateL(KMaxFilenameLength);
	CleanupClosePushL(filename);		
	FileUtils::MakePrivateFilenameL(iFs, KCertAppsFilename, filename);

	// Attempt to open the store
	// need to test opening corrupt store
	TRAPD(err, ReadStoreContentsL(filename));

	if (err == KErrNoMemory || err == KErrInUse)
		{
		User::Leave(err);
		}
	
	if (err != KErrNone)
		{
		// Couldn't open RAM based store, copy from ROM	
		FileUtils::EnsurePathL(iFs, filename);		
	
		if(iPatchableConst)
			{
			OpenCompositeStoreL(filename);
			}
		else
			{
			RBuf romFilename;
			romFilename.CreateL(KMaxFilenameLength);
			CleanupClosePushL(romFilename);			
			FileUtils::MakePrivateROMFilenameL(iFs, KCertAppsFilename, romFilename);
			
			if (FileUtils::ExistsL(iFs, romFilename))
				{
				FileUtils::CopyL(iFs, romFilename, filename);
				}
			else
				{
				CreateStoreL(filename);
				}
			CleanupStack::PopAndDestroy(&romFilename);
			//Retry open, and leave on failure
			ReadStoreContentsL(filename);			
			}
		}
	CleanupStack::PopAndDestroy(&filename);
	ASSERT(iStore);
	}

void CFSCertAppsServer::ReadStoreContentsL(const TDesC& aFilename)
	{
	// Make sure the store is not read-only
	User::LeaveIfError(iFs.SetAtt(aFilename, KEntryAttNormal, KEntryAttReadOnly));

	RFile file;
	User::LeaveIfError(file.Open(iFs, aFilename, EFileRead | EFileWrite));
	CleanupClosePushL(file);
	CPermanentFileStore* store = CPermanentFileStore::FromL(file);
	CleanupStack::Pop(&file);
	CleanupStack::PushL(store);

	// now read the root stream to get the id of our main stream
	RStoreReadStream readStream;
	readStream.OpenLC(*store, store->Root());
	readStream >> iId;	// This can leave
	CleanupStack::PopAndDestroy(&readStream);

	// finally, restore the stream which contains the client arrays.
	// First long is the number of entries, then each entry 
	readStream.OpenLC(*store, iId);
	TInt count = readStream.ReadInt32L();
	for (TInt i = 0; i < count; ++i)
		{
		TCertificateAppInfo clientInfo;
		readStream >> clientInfo;
		User::LeaveIfError(iClients.Append(clientInfo));
		}
	CleanupStack::PopAndDestroy(&readStream);

	ASSERT(!iStore);
	iStore = store;
	CleanupStack::Pop(store);
	}

void CFSCertAppsServer::CreateStoreL(const TDesC& aFilename)
	{
	// If for some reason we can't complete the creation of the store, we want
	// to be sure that we don't leave a half-constructed store on the device
	// as we will then be in trouble when we try to open the store
	TCleanupItem deleteStoreFile(DeleteStoreFile, this);
	CleanupStack::PushL(deleteStoreFile);

	iFs.Delete(aFilename); // ignore errors

	RFile file;
	User::LeaveIfError(file.Create(iFs, aFilename, EFileWrite));
	CleanupClosePushL(file);
	
	CPermanentFileStore* store = CPermanentFileStore::NewL(file);
	CleanupStack::Pop(&file); // now owned by store
	CleanupStack::PushL(store);
	store->SetTypeL(KPermanentFileStoreLayoutUid);

	RStoreWriteStream clientsStream;
	TStreamId id = clientsStream.CreateLC(*store);
	WriteClientArrayL(clientsStream);
	CleanupStack::PopAndDestroy(&clientsStream);

	RStoreWriteStream rootStream;
	TStreamId rootId = rootStream.CreateLC(*store);
	rootStream << id;
	rootStream.CommitL();
	CleanupStack::PopAndDestroy(&rootStream);

	store->SetRootL(rootId);
	store->CommitL();

	CleanupStack::PopAndDestroy(store);
	CleanupStack::Pop(); // deleteStoreFile
	}

void CFSCertAppsServer::RevertStore(TAny* aStore)
	{
	// this is a CleanupItem
	__ASSERT_DEBUG(aStore, PanicServer(EPanicNotInitialised));

	CPermanentFileStore* store = reinterpret_cast<CPermanentFileStore *>(aStore);
	store->Revert();
	}

void CFSCertAppsServer::DeleteStoreFile(TAny *aThis)
	{
	__ASSERT_DEBUG(aThis, PanicServer(EPanicNotInitialised));

	// should call non-static member
	
	CFSCertAppsServer* self = reinterpret_cast<CFSCertAppsServer*>(aThis);

	// Something strange has occurred if we can't get the ramStorePath.
	// Since we can't leave we have to ignore the error
	TFileName ramStorePath;
	TRAPD(err, FileUtils::MakePrivateFilenameL(self->iFs, KCertAppsFilename, ramStorePath));
	if (!err)
		{
		self->iFs.Delete(ramStorePath);
		}
	}

void CFSCertAppsServer::ReplaceAndCommitL(TInt aExcludedIndex)
	{
	TCleanupItem cleanupStore(RevertStore, iStore);
	CleanupStack::PushL(cleanupStore);

	// compact the store 
	iStore->ReclaimL(); // do we need to reclaim
	iStore->CompactL();

	RStoreWriteStream outputStream;
	outputStream.ReplaceLC(*iStore, iId);

	WriteClientArrayL(outputStream, aExcludedIndex);

	CleanupStack::PopAndDestroy(&outputStream);
	iStore->CommitL();
	CleanupStack::Pop(); // cleanupStore
	}

void CFSCertAppsServer::WriteClientArrayL(RWriteStream& stream, TInt aExcludedIndex) const
	{
	// the count of elements to be written is the arraycount - 1 if we exclude 
	// something, otherwise the arraycount
	TInt arrayEnd = iClients.Count();
	TInt count = (aExcludedIndex < 0) ? (arrayEnd) : (arrayEnd - 1);

	stream.WriteInt32L(count);

	for (TInt i = 0; i < arrayEnd; ++i)
		{
		if (i != aExcludedIndex)
			{
			stream << iClients[i];	// This can leave
			}
		}
	stream.CommitL();
	}
