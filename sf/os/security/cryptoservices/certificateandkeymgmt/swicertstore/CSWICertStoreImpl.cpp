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


#include "CSWICertStoreImpl.h"
#include "CSWICertStoreEntryList.h"
#include "CSWICertStoreEntry.h"
#include "CSWICertStoreTokenType.h"
#include "CSWICertStoreToken.h"
#include "unifiedcertstore.h"
#include "log.h"

#include <certstorepatchdata.h>
#include <ccertattributefilter.h>
#include <f32file.h>
#include <u32hal.h> 
#include <e32svr.h>

_LIT(KSWICertStoreZFilename,"z:\\resource\\swicertstore.dat");
_LIT(KSWIWritableCertStorePath,"!:\\resource\\swicertstore\\dat\\");
_LIT(KSWIROMCertStoreFilenamePattern,"z:\\resource\\swicertstore*.dat");

/////////////////////////////////////////////////////////////////////////////////////////
//CFSCertStoreServer
/////////////////////////////////////////////////////////////////////////////////////////

CSWICertStoreImpl* CSWICertStoreImpl::NewL(MCTToken& aToken, RFs& aFs)
	{
	CSWICertStoreImpl* self = new (ELeave) CSWICertStoreImpl(aToken, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSWICertStoreImpl::CSWICertStoreImpl(MCTToken& aToken, RFs& aFs) :
        CActive(EPriorityStandard), iToken(aToken), iFs(aFs)
	{
	CActiveScheduler::Add(this);
	}

void CSWICertStoreImpl::ConstructL()
	{
	TInt err = iSwicertstoreProperty.Attach(KUnifiedCertStorePropertyCat,
											EUnifiedCertStoreFlag, EOwnerThread);
	
	User::LeaveIfError(err);

	iPatchableConst = KAggregateCertStore;
	
	#ifdef __WINS__
		// For the emulator allow the constant to be patched via epoc.ini
		UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
		(TAny*)"KAggregateCertStore", &iPatchableConst); // read emulator property (if present)
	#endif		
	
	
	// if patchable constant is enabled
	if(iPatchableConst)
		{
		// Setup composite ROM Certstore
		SetupCompositeROMCertStoreL();
		}
	else
		{
		// Setup the ROM CertStore
		SetupROMCertStoreL();		
		}
	
	// Setup the Writable CertStore
	// Bear in mind we need to trap NewL(), OpenLC() etc.
	// Any error occurs, it should still setup the z:\ certstore
	TRAP_IGNORE(SetupWritableCertStoreL());

	// aggregate ROM composite certstore and writable certstore if patchable constant is enabled
	if(iPatchableConst)
		{
		if(iCEntryList)
			{
			// ETrue to indicate that we are passing 'iZEntryList'.
			// This action will remove duplicate entries from 'iZEntryList'.
			MergeCertificateEntryListsL(*iZEntryList, ETrue);	
			}
		}
	// Subscribe to the Property
	iSwicertstoreProperty.Subscribe(iStatus);
	SetActive();
	}

void CSWICertStoreImpl::DoCancel()
	{
	// Cancel outstanding request
    iSwicertstoreProperty.Cancel();
	}

void CSWICertStoreImpl::RunL()
	{
	// Resubscribe before processing new value to prevent missing updates
	iSwicertstoreProperty.Subscribe(iStatus);
	SetActive();

	// Bear in mind we need to trap NewL(), OpenLC() etc.
	TRAP_IGNORE(SetupWritableCertStoreL());
	}

void CSWICertStoreImpl::SetupWritableCertStoreL()
	{
	TInt err = KErrNone;
 	
 	delete iCEntryList;
	iCEntryList = NULL;
	
	// The attached file will be freed as well.
	delete iCStore;
	iCStore = NULL;
		
 	do
 		{
 		// Open the store if it is there
 		RFile file;
 		CDir* entryList = NULL;

		TPath writableSwiCertStorePath(KSWIWritableCertStorePath);
		
		TChar systemDriveChar = RFs::GetSystemDriveChar();

		writableSwiCertStorePath[0] = systemDriveChar;
		
		if (KErrNone == iFs.GetDir (writableSwiCertStorePath, KEntryAttNormal,
		 	ESortByName|EDirsFirst, entryList))
				{
				if (entryList)
		                {
						CleanupStack::PushL(entryList);
				 
						TFileName certstoreFile;
						TInt count = entryList->Count();
			
						if (count > 0)
					            {
								// Get the highest version certstore file
							 	const TEntry& entry = (*entryList)[count - 1];
				
							 	certstoreFile.Append(writableSwiCertStorePath);
							 	certstoreFile.Append(entry.iName);
							 
							 	CleanupStack::PopAndDestroy(1, entryList);
							 	}
						else
					            {
								CleanupStack::PopAndDestroy(1, entryList);

							 	// No certstore in this directory
							 	// Currently there should not have any directory in this path
							 	return;
							 	}

						err = file.Open(iFs, certstoreFile, EFileShareReadersOnly);
						 
						if (err != KErrNone && err != KErrNotFound)
								{
								// Log the error to the system file.
								DEBUG_PRINTF2(_L8("Error Opening SWI writable certstore: %S."), &certstoreFile);
							 	return;
							 	}
						 
						 if (err == KErrNone)
					            {
							 	// File exist
							 	CleanupClosePushL(file);
							 
							 	TRAP_IGNORE(iCStore = CPermanentFileStore::FromL(file));
							 
							 	if (iCStore)
							    		{
								 		CleanupStack::Pop(&file); // now owned by store
							 
										// Read id of cert list stream
								 		TStreamId streamId;
								 		RStoreReadStream stream;
										stream.OpenLC(*iCStore, iCStore->Root());
										stream >> streamId;
										CleanupStack::PopAndDestroy(&stream);
										 
										// Read the certificate list
										RStoreReadStream entryStream;
										entryStream.OpenLC(*iCStore, streamId);
										iCEntryList = CSWICertStoreEntryList::NewL(entryStream, iToken, iCertIndex);
										 
								 		CleanupStack::PopAndDestroy(&entryStream);

								 		// Record the writable certstore index
								 		// The index will then be used to set the tokenId in the
								 		// in handle, so to make the handle unique.
								 		iCertIndex += iCEntryList->Count();
								 		}
						 		else
							         	{
										// The file format might not comply to the CPermanentFileStore format
										CleanupStack::PopAndDestroy(&file);

								 		// Log the error to the system file.
								 		DEBUG_PRINTF2(_L8("Error Opening SWI writable certstore: %S."), &certstoreFile);

								 		return;
							         	}   
					            }
		                }
				}
 		}while (err == KErrNotFound);  // There is a very slight chance that the file has been deleted
	}

void CSWICertStoreImpl::MergeCertificateEntryListsL(const CSWICertStoreEntryList& aSourceList, TBool aIsZEntryList)
	{
	ASSERT(iPatchableConst);
	// if patchable constant is enabled
	
	TInt sourceCount = aSourceList.Count();
	for(TInt i = 0; i < sourceCount; i++)
		{
		if(!aIsZEntryList)
			{
			if (!iZEntryList->LabelExists(aSourceList.GetByIndex(i).CertInfo().Label()))
				{
				// Aggregation: Append this entry in the ROM composite list 
				const CSWICertStoreEntry& entry = aSourceList.GetByIndex(i);
				CSWICertStoreEntry* newEntry = CSWICertStoreEntry::NewL(entry.CertInfo(),
																entry.CertificateApps(),
																entry.Trusted(),
																entry.DataStreamId(),
																entry.CertMetaInfo(), 
																entry.StoreIndex());
				CleanupStack::PushL(newEntry);
				iZEntryList->AppendL(newEntry);
				// iZEntryList has taken ownership
				CleanupStack::Pop(newEntry); 
				}
			// Eclipsing: Higher order store certificates with same labels take precedence over lower order store certificates.
			// therefore the later are not included in the composite certificate entry list.
			// N.B iCertIndex will not give total count of certificates because of Eclipsing mechanism.
			// Higher order store certificates are ones which are aggregated prior to other certificates. 
			}
		else
			{
			// remove duplicate label entries from the 'iZEntryList'
			if (iCEntryList->LabelExists(aSourceList.GetByIndex(i).CertInfo().Label()))
				{
				// Remove entries from iZEntryList having same labels as in iCEntryList.
				iZEntryList->Remove(i);
				}
			}
		}
	}

void CSWICertStoreImpl::SetupCompositeROMCertStoreL()
	{
	ASSERT(iPatchableConst);
	
	// Collect the file present in ROM certstore private directory.
	//The files in this list will be in descending order by name.
	CDir* filenameList = NULL;

	User::LeaveIfError(iFs.GetDir(KSWIROMCertStoreFilenamePattern, KEntryAttNormal, ESortByName|EDescending, filenameList));	
	CleanupStack::PushL(filenameList);
	TInt count = filenameList->Count();
	TStreamId streamId;

	// make full path to the certstore files residing on ROM drive	
	// N.B filenameList will contain filenames in descending order.
	RFile file;
	RStoreReadStream stream;
	
	// create an empty list, this will be the composite certificate entry list
	iZEntryList = CSWICertStoreEntryList::NewL();
	// the 'corruptCount' is used for proper correctly referencing valid certstores   
	TInt corruptCount=0;
	for(TInt i = 0; i < count; i++)
		{
		RBuf romFile;
		romFile.CreateL(KMaxFileName);
		CleanupClosePushL(romFile);
		romFile.Append(_L("z:\\resource\\"));
		romFile.Append(((*filenameList)[i]).iName);
		User::LeaveIfError(file.Open(iFs,romFile,EFileShareReadersOnly));
		CleanupClosePushL(file);
		// open and append the store in array of stores
		CPermanentFileStore* store = NULL;
		// if there is any corrupt certstore present then we will simply ignore its
		// aggregation and proceed with aggregating remaining stores.
		// ownership is transfered to 'iZArrayOfStores' 
		TRAPD(err, store = CPermanentFileStore::FromL(file));
		if(err != KErrNone)
			{
			corruptCount++;
			//cleanup file and romFile.
			CleanupStack::PopAndDestroy(2, &romFile);
			continue;
			}
		iZArrayOfStores.AppendL(store);
		TInt index = i - corruptCount;
		// open the root stream containing the streamId of info stream.
		stream.OpenLC(*(iZArrayOfStores)[index],(iZArrayOfStores[index])->Root());
		stream >> streamId;
		CleanupStack::PopAndDestroy(&stream);

		// re-open the info stream containing the certificates entry list.
		stream.OpenLC(*(iZArrayOfStores)[index],streamId);
		
		// store index for these entries will be 'index'.
		// iCertIndex will be 0 for the first certificate entry list passed.
		CSWICertStoreEntryList* temp_list = CSWICertStoreEntryList::NewL(stream, iToken,iCertIndex,index);
		// increment to have unique certificate Ids allocation.
		iCertIndex += temp_list->Count();
		CleanupStack::PushL(temp_list);
		MergeCertificateEntryListsL(*temp_list);
		//temp_list, stream, file and romFile.
		CleanupStack::PopAndDestroy(4, &romFile);
		} 
	CleanupStack::PopAndDestroy(filenameList);
	}

void CSWICertStoreImpl::SetupROMCertStoreL()
	{
	// Open the store
	RFile file;
	User::LeaveIfError(file.Open(iFs, KSWICertStoreZFilename, EFileShareReadersOnly));
	CleanupClosePushL(file);
	iZStore = CPermanentFileStore::FromL(file);
	CleanupStack::Pop(&file); // now owned by store

	// Read id of cert list stream
	TStreamId streamId;
	RStoreReadStream stream;
	stream.OpenLC(*iZStore, iZStore->Root());
	stream >> streamId;
	CleanupStack::PopAndDestroy(&stream);

	// Read the certificate list
	RStoreReadStream entryStream;
	entryStream.OpenLC(*iZStore, streamId);
	iZEntryList = CSWICertStoreEntryList::NewL(entryStream, iToken);
	CleanupStack::PopAndDestroy(&entryStream);

	// set the valid indexes of writable certstore
	iCertIndex = iZEntryList->Count();
	}

CSWICertStoreImpl::~CSWICertStoreImpl()
	{
	Cancel();
	iSwicertstoreProperty.Close();
	delete iCEntryList;
	delete iZEntryList;
	delete iCStore;
	delete iZStore;
	iZArrayOfStores.ResetAndDestroy();
	}

void CSWICertStoreImpl::ListL(RMPointerArray<CCTCertInfo>& aCerts,
			      const CCertAttributeFilter& aFilter)
	{
	if (iCEntryList)
	        {
			FilterCertificateListL(aCerts, aFilter, *iCEntryList);
			}
	FilterCertificateListL(aCerts, aFilter, *iZEntryList);
	}

CCTCertInfo* CSWICertStoreImpl::GetCertL(const TCTTokenObjectHandle& aHandle)
	{
	TBool isCEntryHandle = EFalse;
	const CSWICertStoreEntry* entry = GetCSWICertStoreEntryL(aHandle, isCEntryHandle);	
	return CCTCertInfo::NewL(entry->CertInfo());
	}

void CSWICertStoreImpl::ApplicationsL(const TCTTokenObjectHandle& aHandle, RArray<TUid>& aApplications)
	{
	TBool isCEntryHandle = EFalse;
	const CSWICertStoreEntry* entry = GetCSWICertStoreEntryL(aHandle, isCEntryHandle);
	
	const RArray<TUid>& apps = entry->CertificateApps();
	TInt count = apps.Count();
	
	for (TInt i = 0 ; i < count ; ++i)
		{
		User::LeaveIfError(aApplications.Append(apps[i]));
		}
	}

TBool CSWICertStoreImpl::IsApplicableL(const TCTTokenObjectHandle& aHandle, TUid aApplication)
	{
	TBool isCEntryHandle = EFalse;
	const CSWICertStoreEntry* entry = GetCSWICertStoreEntryL(aHandle, isCEntryHandle);
	return entry->IsApplicable(aApplication);
	}

TBool CSWICertStoreImpl::TrustedL(const TCTTokenObjectHandle& aHandle)
	{
	TBool isCEntryHandle = EFalse;	
	const CSWICertStoreEntry* entry = GetCSWICertStoreEntryL(aHandle, isCEntryHandle);
	return entry->Trusted();
	}

void CSWICertStoreImpl::RetrieveL(const TCTTokenObjectHandle& aHandle, TDes8& aEncodedCert)
	{
	TBool isCEntryHandle = EFalse;
	const CSWICertStoreEntry* entry = GetCSWICertStoreEntryL(aHandle, isCEntryHandle);
	RStoreReadStream stream;
	
	if (iCEntryList && isCEntryHandle)
		{
		stream.OpenLC(*iCStore, entry->DataStreamId());
		}
	else
	    {
	    // if patchable constant is enabled
	    if (iPatchableConst)
	    	{
	    	stream.OpenLC(*(iZArrayOfStores[entry->StoreIndex()]), entry->DataStreamId());
	    	}
	    else
	    	{
	    	stream.OpenLC(*iZStore, entry->DataStreamId());
	    	}
		}

	TInt size = entry->CertInfo().Size();

	if (aEncodedCert.MaxLength() < size)
			{
			User::Leave(KErrOverflow);
			}

	stream.ReadL(aEncodedCert, size);
	CleanupStack::PopAndDestroy(&stream);
	}

const TCertMetaInfo& CSWICertStoreImpl::CertMetaInfoL(const TCTTokenObjectHandle& aHandle) const
	{
	TBool isCEntryHandle = EFalse;
	const CSWICertStoreEntry* entry = GetCSWICertStoreEntryL(aHandle, isCEntryHandle);
	return entry->CertMetaInfo(); 
	}

const CSWICertStoreEntry* CSWICertStoreImpl::GetCSWICertStoreEntryL(const TCTTokenObjectHandle& aHandle, TBool& aCEntryHandle) const
	{
	if (iCEntryList)
	        {
			const CSWICertStoreEntry* entry = NULL;

			TRAPD (retCode, entry = &(iCEntryList->GetByHandleL(aHandle)));

			if (entry && (retCode == KErrNone))
			        {
			        aCEntryHandle = ETrue;
					return entry;
					}
			}
	return &iZEntryList->GetByHandleL(aHandle);
	}

void CSWICertStoreImpl::AddIfMatchesFilterL(RMPointerArray<CCTCertInfo>& aCerts, const CCertAttributeFilter& aFilter, const CSWICertStoreEntry& aEntry)
	{
	const CCTCertInfo& certInfo = aEntry.CertInfo();
	TBool accept = ETrue;
			  
	if (aFilter.iUidIsSet)
		{
		accept = aEntry.IsApplicable(aFilter.iUid);
		}
	if (aFilter.iFormatIsSet && accept)
		{
		accept = (aFilter.iFormat == certInfo.CertificateFormat());
		}
	if (aFilter.iOwnerTypeIsSet && accept)
		{
		accept = (aFilter.iOwnerType == certInfo.CertificateOwnerType());
		}
	if (aFilter.iSubjectKeyIdIsSet && accept)
		{
		accept = (aFilter.iSubjectKeyId == certInfo.SubjectKeyId());
		}
	if (aFilter.iLabelIsSet && accept)
		{
		accept = (aFilter.iLabel == certInfo.Label());
		}
			  
	if (accept)
		{
		CCTCertInfo* newCertInfo = CCTCertInfo::NewLC(certInfo);
		User::LeaveIfError(aCerts.Append(newCertInfo));
		CleanupStack::Pop(newCertInfo);
		}
	}

void CSWICertStoreImpl::FilterCertificateListL(RMPointerArray<CCTCertInfo>& aCerts,
			   const CCertAttributeFilter& aFilter, const CSWICertStoreEntryList& aEntryList)
	{
	TInt count = aEntryList.Count();
	for (TInt index = 0; index < count; index++)
		{
		const CSWICertStoreEntry& entry = aEntryList.GetByIndex(index);
		AddIfMatchesFilterL(aCerts, aFilter, entry);
		}
	}


