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
* sisregistry - client registry session interface implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include <s32mem.h>
#include <f32file.h>
#include <caf/caf.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <caf/cafplatform.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#include "clientserver.h"
#include "rtamanager.h"


using namespace ReferenceTestAgent;
using namespace ContentAccess;

EXPORT_C RRtaManager::RRtaManager()
	{
	
	}

EXPORT_C TInt RRtaManager::Connect()
	{
	TInt err = KErrNone;
	err = RRtaClient::Connect();
	if(err == KErrNone)
		{
		// Don't really care whether client has DRM capability at this point
		err = SendReceive(ENoEnforceCreateManagerMessageHandler, TIpcArgs());
		if(err != KErrNone)
			{
			Close();
			}
		}
	return err;
	}

EXPORT_C TInt RRtaManager::DeleteFile(const TDesC& aFileName) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerDeleteFile, TIpcArgs(&aFileName));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerDeleteFile, TIpcArgs(&aFileName));
		}
	return err;
	}

EXPORT_C TInt RRtaManager::CopyFile(const TDesC& aSource, const TDesC& aDestination) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerCopyFile, TIpcArgs(&aSource, &aDestination));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerCopyFile, TIpcArgs(&aSource, &aDestination));
		}
	return err;
	}

EXPORT_C TInt RRtaManager::CopyFile(RFile& aSource, const TDesC& aDestination) const
	{
	TInt err = KErrNone;
	TIpcArgs ipcArgs;

	err = aSource.TransferToServer(ipcArgs,0,1);
	if (err==KErrNone)
		{
		ipcArgs.Set(2,&aDestination);
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(EManagerCopyFileHandle, ipcArgs);
			}
		else
			{
			err = SendReceive(ENoEnforceManagerCopyFileHandle, ipcArgs);
			}
		}
	return err;
	}

EXPORT_C TInt RRtaManager::RenameFile(const TDesC& aOldFileName, const TDesC& aNewFileName) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerRenameFile, TIpcArgs(&aOldFileName, &aNewFileName));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerRenameFile, TIpcArgs(&aOldFileName, &aNewFileName));
		}
	return err;
	}

EXPORT_C TInt RRtaManager::MkDir(const TDesC& aPath) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerMkDir, TIpcArgs(&aPath));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerMkDir, TIpcArgs(&aPath));
		}
	return err;
	}
	
	
EXPORT_C TInt RRtaManager::MkDirAll(const TDesC& aPath) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerMkDirAll, TIpcArgs(&aPath));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerMkDirAll, TIpcArgs(&aPath));
		}
	return err;
	}
	

EXPORT_C TInt RRtaManager::RenameDir(const TDesC& aOldPath, const TDesC& aNewPath) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerRenameDir, TIpcArgs(&aOldPath, &aNewPath));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerRenameDir, TIpcArgs(&aOldPath, &aNewPath));
		}
	return err;
	}

EXPORT_C TInt RRtaManager::RmDir(const TDesC& aPath) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerRemoveDir, TIpcArgs(&aPath));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerRemoveDir, TIpcArgs(&aPath));
		}
	return err;
	}

EXPORT_C TInt RRtaManager::GetDir(const TDesC& aPath, TUint aEntryMask, TUint aEntrySortKey, CDir*& aEntryList) const
	{
	TRAPD( err, GetDirL(aPath, aEntryMask, aEntrySortKey, aEntryList) );
	return err;
	}

void RRtaManager::GetDirL(const TDesC& aPath, TUint aEntryMask, TUint aEntrySortKey, CDir*& aEntryList) const
	{
	TInt err = KErrOverflow;
	TPckg<TUint> entryMaskPckg(aEntryMask);
	TPckg<TUint> sortKeyPckg(aEntrySortKey);
	
	// start off with an arbitary size receive buffer
	TInt length = 50 * sizeof(TEntry);
	TPckg <TInt> lengthPckg(length);
	

	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		TPtr8 transferPtr = transferBuffer->Des();
	
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(EManagerGetDir1, TIpcArgs(&aPath, &entryMaskPckg, &sortKeyPckg, &transferPtr));
			}
		else
			{
			err = SendReceive(ENoEnforceManagerGetDir1, TIpcArgs(&aPath, &entryMaskPckg, &sortKeyPckg, &transferPtr));
			}


		if(err == KErrOverflow)
			{
			// Find out the length required to receive the directory list
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream entryListStream(transferPtr);
			CleanupClosePushL(entryListStream);
			aEntryList = ContentAccess::CDirStreamable::NewL(entryListStream);
			CleanupStack::PopAndDestroy(&entryListStream);
			}
		CleanupStack::PopAndDestroy(transferBuffer);	
		}
	}

EXPORT_C TInt RRtaManager::GetDir(const TDesC& aPath, TUint aEntryMask, TUint aEntrySortKey, CDir*& aEntryList, CDir*& aFileList) const
	{
	TRAPD(err, GetDirL(aPath, aEntryMask, aEntrySortKey, aEntryList, aFileList));
	return err;
	}

void RRtaManager::GetDirL(const TDesC& aPath, TUint aEntryMask, TUint aEntrySortKey, CDir*& aEntryList, CDir*& aFileList) const
	{
	TInt err = KErrOverflow;
	TPckg<TUint> entryMaskPckg(aEntryMask);
	TPckg<TUint> sortKeyPckg(aEntrySortKey);
	
	// start off with an arbitary size receive buffer
	TInt length = 50 * sizeof(TEntry);
	TPckg <TInt> lengthPckg(length);

	TInt lengthA = length;
	TInt lengthB = length;
		
	// Pack the entry mask and sort key into the one descriptor
	// since we can only pass across four arguements
	HBufC8* doublePackage = HBufC8::NewLC(2* sortKeyPckg.MaxLength());
	doublePackage->Des().Copy(entryMaskPckg);
	doublePackage->Des().Append(sortKeyPckg);	
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBufferA = HBufC8::NewLC(lengthA);
		TPtr8 transferPtrA = transferBufferA->Des();

		HBufC8* transferBufferB = HBufC8::NewLC(lengthB);	
		TPtr8 transferPtrB = transferBufferB->Des();

		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(EManagerGetDir2, TIpcArgs(&aPath, doublePackage, &transferPtrA, &transferPtrB));
			}
		else
			{
			err = SendReceive(ENoEnforceManagerGetDir2, TIpcArgs(&aPath, doublePackage, &transferPtrA, &transferPtrB));
			}


		if(err == KErrOverflow)
			{
			// If B has something in it then A must have succeeded
			// else it must have been A that overflowed
			if(transferPtrB.Length() > 0)
				{
				// Find out the length required to receive the directory list
				lengthPckg.Copy(transferPtrB.Left(lengthPckg.MaxLength()));		
				lengthB = length;
				}
			else
				{
				// Find out the length required to receive the directory list
				lengthPckg.Copy(transferPtrA.Left(lengthPckg.MaxLength()));						
				lengthA = length;
				}
				
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream entryListStream(transferPtrA);
			CleanupClosePushL(entryListStream);
			aEntryList = ContentAccess::CDirStreamable::NewL(entryListStream);
			CleanupStack::PopAndDestroy(&entryListStream);
			
			// put aEntryList on the cleanupstack in case subsequent operations fail
			CleanupStack::PushL(aEntryList);

			// read in the attribute values from the buffer
			entryListStream.Open(transferPtrB);
			CleanupClosePushL(entryListStream);
			aFileList = ContentAccess::CDirStreamable::NewL(entryListStream);
			CleanupStack::PopAndDestroy(&entryListStream);
			
			// Finished so can take aEntryList off the cleanupstack
			CleanupStack::Pop(aEntryList);
			}
		CleanupStack::PopAndDestroy(2, transferBufferA);	// TransferBufferA, TransferBufferB
		}
	CleanupStack::PopAndDestroy(doublePackage);
	}

EXPORT_C TInt RRtaManager::GetDir(const TDesC& aPath, TUidType aEntryUid, TUint aEntrySortKey, CDir*& aFileList) const
	{
	TRAPD(err, GetDirL(aPath, aEntryUid, aEntrySortKey, aFileList));
	return err;
	}

void RRtaManager::GetDirL(const TDesC& aPath, TUidType aEntryUid, TUint aEntrySortKey, CDir*& aFileList) const
	{
	TInt err = KErrOverflow;
	TPckg<TUidType> entryUidPckg(aEntryUid);
	TPckg<TUint> sortKeyPckg(aEntrySortKey);
	
	// start off with an arbitary size receive buffer
	TInt length = 50 * sizeof(TEntry);
	TPckg <TInt> lengthPckg(length);
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		TPtr8 transferPtr = transferBuffer->Des();
	
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(EManagerGetDir1, TIpcArgs(&aPath, &entryUidPckg, &sortKeyPckg, &transferPtr));
			}
		else
			{
			err = SendReceive(ENoEnforceManagerGetDir1, TIpcArgs(&aPath, &entryUidPckg, &sortKeyPckg, &transferPtr));
			}


		if(err == KErrOverflow)
			{
			// Find out the length required to receive the directory list
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream entryListStream(transferPtr);
			CleanupClosePushL(entryListStream);
			aFileList = ContentAccess::CDirStreamable::NewL(entryListStream);
			CleanupStack::PopAndDestroy(&entryListStream);
			}
		CleanupStack::PopAndDestroy(transferBuffer);	
		}
	}


EXPORT_C TInt RRtaManager::GetAttribute (const ContentAccess::TVirtualPathPtr& aVirtualPath, TInt aAttribute, TInt &aValue) const
	{
	TInt result = KErrNone;
	TAttributePckg pckgObject;
	pckgObject.iAttribute = aAttribute;
	pckgObject.iValue = aValue;
	TPckg<TAttributePckg> pckgData(pckgObject); 

	result = SendReceive(EManagerGetAttribute, TIpcArgs(&aVirtualPath.URI(), &aVirtualPath.UniqueId(), &pckgData));
	aValue = pckgObject.iValue;
	return result;
	}

EXPORT_C TInt RRtaManager::GetAttribute(const TInt aAttribute, TInt& aValue, RFile& aFile, const TDesC& aUniqueId) const
	{
	TInt result = KErrNone;
	TIpcArgs ipcArgs;

	result = aFile.TransferToServer(ipcArgs,0,1);	
	if(result == KErrNone)
		{
		TAttributePckg pckgObject;
		pckgObject.iAttribute = aAttribute;
		pckgObject.iValue = aValue;
		
		TPckg<TAttributePckg> pckgData(pckgObject); 
		ipcArgs.Set(2, &pckgData);
		ipcArgs.Set(3, &aUniqueId);
		
		result = SendReceive(EManagerGetAttributeByFileHandle, ipcArgs);
		aValue = pckgObject.iValue;
		}
	
	return result; 	
	}
	
EXPORT_C TInt RRtaManager::GetAttributeSet (const ContentAccess::TVirtualPathPtr& aVirtualPath, RAttributeSet &aAttributeSet) const
	{
	TRAPD(err, GetAttributeSetL (aVirtualPath, aAttributeSet));
	return err;
	}

void RRtaManager::GetAttributeSetL (const ContentAccess::TVirtualPathPtr& aVirtualPath, RAttributeSet &aAttributeSet) const
	{
	AttributeSetL(EPathBasedRetrieval, &aVirtualPath, &aAttributeSet);
	}

EXPORT_C TInt RRtaManager::GetAttributeSet(ContentAccess::RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId) const
	{
	TRAPD(err, GetAttributeSetL (aAttributeSet, aFile, aUniqueId));
	return err;
	}

void RRtaManager::GetAttributeSetL (ContentAccess::RAttributeSet& aAttributeSet, RFile& aFile, const TDesC& aUniqueId) const
	{
	AttributeSetL(EFileHandleBasedRetrieval, NULL, &aAttributeSet, &aFile, &aUniqueId);
	}

void RRtaManager::AttributeSetL(const TRetrievalType& aRetrievalType, const ContentAccess::TVirtualPathPtr* aVirtualPath,...) const
	{
	VA_LIST list;
    VA_START(list, aVirtualPath);

    ContentAccess::RAttributeSet* attributeSet  = VA_ARG(list, ContentAccess::RAttributeSet*);
    
    TInt err = KErrOverflow;
	TInt length = 0;
	TPckg <TInt> lengthPckg(length);
	
	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	attributeSet->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	
	TPtr8 bufPtr = buf->Ptr(0);
	// allocate arbitary receive buffer twice the size of the source buffer
	length = bufPtr.Length() * 2;
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		transferBuffer->Des().Copy(bufPtr);
		TPtr8 transferPtr = transferBuffer->Des();
		
		if(aRetrievalType == EPathBasedRetrieval)
			{
			// attempt to retrieve the attributes
			err = SendReceive(EManagerGetAttributeSet, TIpcArgs(&(aVirtualPath->URI()), &(aVirtualPath->UniqueId()), &transferPtr));	
			}
		else if(aRetrievalType == EFileHandleBasedRetrieval)
			{
			RFile* file  = VA_ARG(list, RFile*);
			const TDesC* uniqueId  = VA_ARG(list, const TDesC*);
			
			// attempt to retrieve the attributes using a File Handle
			TIpcArgs ipcArgs;
			err = file->TransferToServer(ipcArgs,0,1);	
			if(err == KErrNone)
				{
				ipcArgs.Set(2, &transferPtr);
				ipcArgs.Set(3, uniqueId);
				err = SendReceive(EManagerGetAttributeSetByFileHandle, ipcArgs);
				}
			}
		if(err == KErrOverflow)
			{
			// Find out the length required to receive the resulting attribute set
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream readStream(transferPtr);
			CleanupClosePushL(readStream);
			attributeSet->InternalizeL(readStream);
			CleanupStack::PopAndDestroy(&readStream);
			}
		else
			{
			// if any other system wide error then leave
			User::LeaveIfError(err);
			}	
		CleanupStack::PopAndDestroy(transferBuffer);	
		}
		
	CleanupStack::PopAndDestroy(buf); 
	VA_END(list); 
 	}

EXPORT_C TInt RRtaManager::GetStringAttribute (const ContentAccess::TVirtualPathPtr& aVirtualPath, TInt aAttribute, TDes &aValue) const
	{
	TInt result = KErrNone;
	TStringAttributePckg pckgObject;
	pckgObject.iAttribute = aAttribute;
	pckgObject.iValue = aValue;
	TPckg<TStringAttributePckg> pckgData(pckgObject); 

	result = SendReceive(EManagerGetStringAttribute, TIpcArgs(&aVirtualPath.URI(), &aVirtualPath.UniqueId(), &pckgData));
	aValue = pckgObject.iValue;
	return result;
	}
	
EXPORT_C TInt RRtaManager::GetStringAttribute(const TInt aAttribute, TDes& aValue, RFile& aFile, const TDesC& aUniqueId) const
	{
	TInt result = KErrNone;
	TIpcArgs ipcArgs;

	result = aFile.TransferToServer(ipcArgs,0,1);	
	if(result == KErrNone)
		{
		TStringAttributePckg pckgObject;
		pckgObject.iAttribute = aAttribute;
		pckgObject.iValue = aValue;
		
		TPckg<TStringAttributePckg> pckgData(pckgObject); 
		ipcArgs.Set(2, &pckgData);
		ipcArgs.Set(3, &aUniqueId);
		
		result = SendReceive(EManagerGetStringAttributeByFileHandle, ipcArgs);
		aValue = pckgObject.iValue;
		}
		
	return result; 	
	}
	
EXPORT_C TInt RRtaManager::GetStringAttributeSet (const ContentAccess::TVirtualPathPtr& aVirtualPath, RStringAttributeSet &aStringAttributeSet) const
	{
	TRAPD(err, GetStringAttributeSetL (aVirtualPath, aStringAttributeSet));
	return err;
	}

void RRtaManager::GetStringAttributeSetL (const ContentAccess::TVirtualPathPtr& aVirtualPath, RStringAttributeSet &aStringAttributeSet) const
	{
	StringAttributeSetL(EPathBasedRetrieval, &aVirtualPath, &aStringAttributeSet);
	}

EXPORT_C TInt RRtaManager::GetStringAttributeSet(ContentAccess::RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId) const
	{
	TRAPD(err, GetStringAttributeSetL(aStringAttributeSet, aFile, aUniqueId));
	return err;
	}

void RRtaManager::GetStringAttributeSetL(ContentAccess::RStringAttributeSet& aStringAttributeSet, RFile& aFile, const TDesC& aUniqueId) const
	{
	StringAttributeSetL(EFileHandleBasedRetrieval, NULL, &aStringAttributeSet, &aFile, &aUniqueId);
	}
	
void RRtaManager::StringAttributeSetL(const TRetrievalType& aRetrievalType,const ContentAccess::TVirtualPathPtr* aVirtualPath, ...) const
	{
	VA_LIST list;
    VA_START(list,aVirtualPath);
    
   	ContentAccess::RStringAttributeSet* stringAttributeSet  = VA_ARG(list, ContentAccess::RStringAttributeSet*);
	
	TInt err = KErrOverflow;
	TInt length = 0;
	TPckg <TInt> lengthPckg(length);
	
	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	stringAttributeSet->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	
	TPtr8 bufPtr = buf->Ptr(0);
	// allocate arbitary receive buffer much larger than the size of the source buffer
	length = bufPtr.Length() * 15;
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		transferBuffer->Des().Copy(bufPtr);
		TPtr8 transferPtr = transferBuffer->Des();
	
		if(aRetrievalType == EPathBasedRetrieval)
			{
			// attempt to retrieve the string attributes set
			err = SendReceive(EManagerGetStringAttributeSet, TIpcArgs(&(aVirtualPath->URI()), &(aVirtualPath->UniqueId()), &transferPtr));	
			}
		else if(aRetrievalType == EFileHandleBasedRetrieval)
			{
			RFile* file  = VA_ARG( list, RFile*);
			const TDesC* uniqueId  = VA_ARG( list, const TDesC*);
			
			// attempt to retrieve the string attributes set using a File Handle
			TIpcArgs ipcArgs;
			err = file->TransferToServer(ipcArgs,0,1);	
			if(err == KErrNone)
				{
				ipcArgs.Set(2, &transferPtr);
				ipcArgs.Set(3, uniqueId);
				err = SendReceive(EManagerGetStringAttributeSetByFileHandle, ipcArgs);
				}
			}
		if(err == KErrOverflow)
			{
			// Find out the length required to receive the resulting attribute set
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream readStream(transferPtr);
			CleanupClosePushL(readStream);
			stringAttributeSet->InternalizeL(readStream);
			CleanupStack::PopAndDestroy(&readStream);
			}
		else
			{
			// if any other system wide error then leave
			User::LeaveIfError(err);
			}		
		CleanupStack::PopAndDestroy(transferBuffer);	
		}
		
	CleanupStack::PopAndDestroy(buf);
	VA_END(list);
	}

EXPORT_C void RRtaManager::NotifyStatusChangeL(const TDesC& aURI, ContentAccess::TEventMask aMask) const
	{
	TPckg <TEventMask> eventMaskPckg(aMask);
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EManagerNotifyStatusChange, TIpcArgs(&aURI, &eventMaskPckg)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceManagerNotifyStatusChange, TIpcArgs(&aURI, &eventMaskPckg)));
		}
	}

EXPORT_C TInt RRtaManager::CancelNotifyStatusChange(const TDesC& aURI) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerCancelNotifyStatusChange, TIpcArgs(&aURI));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerCancelNotifyStatusChange, TIpcArgs(&aURI));
		}
	return err;
	}

EXPORT_C TInt RRtaManager::SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue) const
	{
	TInt err = KErrNone;
	TPckg <TInt> valuePckg(aValue);
	TPckg <TAgentProperty> agentPropertyPckg(aProperty);
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerSetProperty, TIpcArgs(&agentPropertyPckg, &valuePckg));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerSetProperty, TIpcArgs(&agentPropertyPckg, &valuePckg));
		}
	return err;
	}

EXPORT_C void RRtaManager::DisplayInfoL(ContentAccess::TDisplayInfo aInfo, const ContentAccess::TVirtualPathPtr& aVirtualPath) const
	{
	TPckg <TDisplayInfo> displayInfoPckg(aInfo);
	TPckg <TVirtualPathPtr> virtualPathPckg(aVirtualPath);
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EManagerDisplayInfo, TIpcArgs(&aInfo, &aVirtualPath)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceManagerDisplayInfo, TIpcArgs(&aInfo, &aVirtualPath)));
		}
	}

EXPORT_C void RRtaManager::DisplayInfoL(ContentAccess::TDisplayInfo aInfo, RFile& aFile, const TDesC& aUniqueId) const
	{
	TPckg <TDisplayInfo> displayInfoPckg(aInfo);
	TPckg <RFile> filePckg(aFile);
	
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EManagerDisplayInfo1, TIpcArgs(&aInfo, &aFile, &aUniqueId)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceManagerDisplayInfo1, TIpcArgs(&aInfo, &aFile, &aUniqueId)));
		}
	}

EXPORT_C TInt RRtaManager::AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer) const
	{
	TInt err = KErrNone;
	TPckg <TInt> commandPckg(aCommand);	
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EManagerAgentSpecificCommand, TIpcArgs(&commandPckg, &aInputBuffer, &aOutputBuffer));
		}
	else
		{
		err = SendReceive(ENoEnforceManagerAgentSpecificCommand, TIpcArgs(&commandPckg, &aInputBuffer, &aOutputBuffer));
		}
	return err;
	}

EXPORT_C void RRtaManager::DisplayManagementInfoL() const
	{	
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EManagerDisplayManagementInfo, TIpcArgs()));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceManagerDisplayManagementInfo, TIpcArgs()));
		}
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
      
EXPORT_C TInt RRtaManager::GetAttribute(const TDesC8& aHeaderData, TInt aAttribute, TInt& aValue) const     
    {     
    TPckg<TInt> valuePkg(aValue);     
    TInt result = SendReceive(EWMDRMGetAttribute, TIpcArgs(&aHeaderData, aAttribute, &valuePkg));     
    return result;     
    }     
         
EXPORT_C TInt RRtaManager::GetAttributeSet(const TDesC8& aHeaderData, ContentAccess::RAttributeSet& aAttributeSet) const     
    {     
    TRAPD(err, GetAttributeSetL (aHeaderData, aAttributeSet));     
    return err;     
    }     
         
EXPORT_C TInt RRtaManager::GetStringAttribute(const TDesC8& aHeaderData, TInt aAttribute, TDes& aValue) const     
    {     
    TInt result = SendReceive(EWMDRMGetStringAttribute, TIpcArgs(&aHeaderData, aAttribute, &aValue));     
    return result;     
    }     
      
EXPORT_C TInt RRtaManager::GetStringAttributeSet(const TDesC8& aHeaderData, ContentAccess::RStringAttributeSet& aStringAttributeSet) const     
    {     
    TRAPD(err, GetStringAttributeSetL(aHeaderData, aStringAttributeSet));     
    return err;     
    }     
         
void RRtaManager::GetAttributeSetL(const TDesC8& aHeaderData, ContentAccess::RAttributeSet& aAttributeSet) const     
    {     
    // dynamic buffer since we don't know in advance the size required     
    CBufFlat* buffer = CBufFlat::NewL(KDefaultBufferSize);     
    CleanupStack::PushL(buffer);     
         
    RBufWriteStream stream(*buffer);     
    CleanupClosePushL(stream);     
         
    // write the directory to the stream     
    aAttributeSet.ExternalizeL(stream);     
    CleanupStack::PopAndDestroy(&stream);     
         
    // Create an HBufC8 from the stream buf's length, and copy      
    // the stream buffer into this descriptor     
    HBufC8* transferBuffer = HBufC8::NewLC(buffer->Size() * 15);     
    TPtr8 ptr = buffer->Ptr(0);     
    transferBuffer->Des().Copy(ptr);     
    TPtr8 transferPtr = transferBuffer->Des();     
         
    User::LeaveIfError(SendReceive(EWMDRMGetAttributeSet, TIpcArgs(&aHeaderData, &transferPtr)));     
      
    // read in the attribute values from the buffer     
    RDesReadStream readStream(transferPtr);     
    CleanupClosePushL(readStream);     
    aAttributeSet.InternalizeL(readStream);     
    CleanupStack::PopAndDestroy(&readStream);     
         
    CleanupStack::PopAndDestroy(2, buffer);     
    }     
         
void RRtaManager::GetStringAttributeSetL(const TDesC8& aHeaderData, ContentAccess::RStringAttributeSet& aStringAttributeSet) const     
    {     
    // dynamic buffer since we don't know in advance the size required     
    CBufFlat* buffer = CBufFlat::NewL(KDefaultBufferSize);     
    CleanupStack::PushL(buffer);     
         
    RBufWriteStream stream(*buffer);     
    CleanupClosePushL(stream);     
         
    // write the directory to the stream     
    aStringAttributeSet.ExternalizeL(stream);     
    CleanupStack::PopAndDestroy(&stream);     
         
    // Create an HBufC8 from the stream buf's length, and copy      
    // the stream buffer into this descriptor     
    HBufC8* transferBuffer = HBufC8::NewLC(buffer->Size()*15);     
    TPtr8 ptr = buffer->Ptr(0);     
    transferBuffer->Des().Copy(ptr);     
    TPtr8 transferPtr = transferBuffer->Des();     
         
    User::LeaveIfError(SendReceive(EWMDRMGetStringAttributeSet, TIpcArgs(&aHeaderData, &transferPtr)));     
         
    // read in the attribute values from the buffer     
    RDesReadStream readStream(transferPtr);     
    CleanupClosePushL(readStream);     
    aStringAttributeSet.InternalizeL(readStream);     
    CleanupStack::PopAndDestroy(&readStream);     
                     
    CleanupStack::PopAndDestroy(2, buffer);     
    }     
         
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

