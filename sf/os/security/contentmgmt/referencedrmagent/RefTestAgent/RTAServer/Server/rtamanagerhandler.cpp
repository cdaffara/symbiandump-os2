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
* SisRegistry - server session implementation
*
*/


/**
 @file
 @test
 @internalComponent
*/
#include <s32mem.h>
#include <f32file.h>
#include <s32file.h>
#include <caf/virtualpath.h>

#include "rtamanagerhandler.h"
#include "RTAsession.h"
#include "RTAserver.h"
#include "rightsobject.h"
#include "rtaArchive.h"
#include "drmfilecontent.h"
#include "drmfiles.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;

//This structure is used to pass empty lists to the GetDir method. 	
class CDirectoryStruct : public CBase
	{
public:
	~CDirectoryStruct();
public:
	CDir* entryList;
	CDir* dirList;
	};

CDirectoryStruct::~CDirectoryStruct()
	{
	delete entryList;
	delete dirList;
	}

CRtaManagerHandler* CRtaManagerHandler::NewL(const CSession2& aSession)
	{
	CRtaManagerHandler* self = new (ELeave) CRtaManagerHandler(aSession);
	return self;
	}
	
CRtaManagerHandler::CRtaManagerHandler(const CSession2& aSession) : CRtaMessageHandler(aSession)
	{
	}
	
CRtaManagerHandler::~CRtaManagerHandler()
	{
	delete iArchive;
	iFile.Close();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
    delete iWmdrmContentObject;     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
	}

void CRtaManagerHandler::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
	{

	switch (aMessage.Function())
		{	
	// RRtaManager functions
	case EManagerDeleteFile:
	case ENoEnforceManagerDeleteFile:
		DeleteFileL(aMessage);
		break;
	case EManagerCopyFile:
	case ENoEnforceManagerCopyFile:
		CopyFileL(aMessage);
		break;
	case EManagerCopyFileHandle:
	case ENoEnforceManagerCopyFileHandle:
		CopyFileHandleL(aMessage);
		break;
	case EManagerRenameFile:
	case ENoEnforceManagerRenameFile:
		RenameFileL(aMessage);
		break;
	case EManagerMkDir:
	case ENoEnforceManagerMkDir:
		MkDirL(aMessage);
		break;
	case EManagerMkDirAll:
	case ENoEnforceManagerMkDirAll:
		MkDirAllL(aMessage);
		break;
	case EManagerRenameDir:
	case ENoEnforceManagerRenameDir:
		DeleteFileL(aMessage);
		break;
	case EManagerRemoveDir:
	case ENoEnforceManagerRemoveDir:
		RmDirL(aMessage);
		break;
	case EManagerGetDir1:
	case ENoEnforceManagerGetDir1:
		GetDir1L(aMessage);
		break;
	case EManagerGetDir2:
	case ENoEnforceManagerGetDir2:
		GetDir2L(aMessage);
		break;
	case EManagerGetDir3:
	case ENoEnforceManagerGetDir3:
		GetDir3L(aMessage);
		break;	
	case EManagerGetAttribute:
	case EManagerGetAttributeByFileHandle:	
	case ENoEnforceManagerGetAttribute:
		GetAttributeL(aMessage);
		break;
	case EManagerGetAttributeSet:
	case EManagerGetAttributeSetByFileHandle:
	case ENoEnforceManagerGetAttributeSet:
		GetAttributeSetL(aMessage);
		break;
	case EManagerGetStringAttribute:
	case EManagerGetStringAttributeByFileHandle:
	case ENoEnforceManagerGetStringAttribute:
		GetStringAttributeL(aMessage);
		break;
	case EManagerGetStringAttributeSet:
	case EManagerGetStringAttributeSetByFileHandle:
	case ENoEnforceManagerGetStringAttributeSet:
		GetStringAttributeSetL(aMessage);
		break;
	case EManagerNotifyStatusChange:
	case ENoEnforceManagerNotifyStatusChange:
		NotifyStatusChangeL(aMessage);
		break;
	case EManagerCancelNotifyStatusChange:
	case ENoEnforceManagerCancelNotifyStatusChange:
		CancelNotifyStatusChangeL(aMessage);
		break;	
	case EManagerAgentSpecificCommand:
	case ENoEnforceManagerAgentSpecificCommand:
		AgentSpecificCommandL(aMessage);
		break;
	case EManagerSetProperty:
	case ENoEnforceManagerSetProperty:
		SetPropertyL(aMessage);
		break;
	case EManagerDisplayInfo:
	case ENoEnforceManagerDisplayInfo:
		DisplayInfoL(aMessage);
		break;
	case EManagerDisplayInfo1:
	case ENoEnforceManagerDisplayInfo1:
		DisplayInfo1L(aMessage);
		break;
	case EManagerDisplayManagementInfo:
	case ENoEnforceManagerDisplayManagementInfo:
		DisplayManagementInfoL(aMessage);
		break;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    case EWMDRMGetAttribute:     
    case ENoEnforceWMDRMGetAttribute:     
        GetWMDRMAttributeL(aMessage);     
        break;     
             
    case EWMDRMGetAttributeSet:     
    case ENoEnforceWMDRMGetAttributeSet:     
        GetWMDRMAttributeSetL(aMessage);     
        break;     
         
    case EWMDRMGetStringAttribute:     
    case ENoEnforceWMDRMGetStringAttribute:     
        GetWMDRMStringAttributeL(aMessage);     
        break;     
             
    case EWMDRMGetStringAttributeSet:     
    case ENoEnforceWMDRMGetStringAttributeSet:     
        GetWMDRMStringAttributeSetL(aMessage);     
        break;     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
	default:
		CRtaSession::PanicClient(aMessage,EPanicIllegalFunction);
		break;
		}
	}

void CRtaManagerHandler::DeleteFileL(const RMessage2& aMessage)
	{
	_LIT(KSystemDrive, "C:"); // not always c but close enough for test/ref
	const TInt KPrivatePathLen = 32;
	TBuf<KPrivatePathLen> myDataCage;
	User::LeaveIfError(Server().Fs().PrivatePath(myDataCage));
	myDataCage.Insert(0, KSystemDrive); 
	
	HBufC16* buffer = ReadDesC16LC(aMessage,0);

	if (buffer->FindF(myDataCage) == KErrNotFound)
		{
		User::Leave(KErrArgument);
		}
	User::LeaveIfError(Server().Fs().Delete(*buffer));
	CleanupStack::PopAndDestroy(buffer); 
	}

void CRtaManagerHandler::CopyFileL(const RMessage2& aMessage)
	{
	// Read source and destination
	HBufC16* source = ReadDesC16LC(aMessage,0);
	HBufC16* destination= ReadDesC16LC(aMessage,1);
	
	// Attempt to copy the files
	CFileMan* fileMan = CFileMan::NewL(Server().Fs());
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Copy(*source, *destination, EFalse));
	CleanupStack::PopAndDestroy(3, source); // fileman, destination, source
	}

void CRtaManagerHandler::CopyFileHandleL(const RMessage2& aMessage)
	{
	RFile file;

	// Read source and destination
	User::LeaveIfError(file.AdoptFromClient(aMessage,0,1));
	CleanupClosePushL(file);
	HBufC16* destination= ReadDesC16LC(aMessage,2);

	// Attempt to copy the files
	CFileMan* fileMan = CFileMan::NewL(Server().Fs());
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Copy(file, *destination));
	CleanupStack::PopAndDestroy(3); // fileman, destination, file
	}
	
void CRtaManagerHandler::RenameFileL(const RMessage2& aMessage)
	{
	// Read source and destination
	HBufC16* source = ReadDesC16LC(aMessage,0);
	HBufC16* destination= ReadDesC16LC(aMessage,1);
	
	// Attempt to rename the file
	User::LeaveIfError(Server().Fs().Rename(*source, *destination));
	CleanupStack::PopAndDestroy(2, source); // destination, source
	}

void CRtaManagerHandler::MkDirL(const RMessage2& aMessage)
	{
	// Read path
	HBufC16* path = ReadDesC16LC(aMessage,0);
	
	// Attempt to create the directory
	User::LeaveIfError(Server().Fs().MkDir(*path));
	CleanupStack::PopAndDestroy(path);
	}

void CRtaManagerHandler::MkDirAllL(const RMessage2& aMessage)
	{
	// Read path
	HBufC16* path = ReadDesC16LC(aMessage,0);
	
	// Attempt to create the directory
	User::LeaveIfError(Server().Fs().MkDirAll(*path));
	CleanupStack::PopAndDestroy(path);
	}
	
void CRtaManagerHandler::RenameDirL(const RMessage2& aMessage)
	{
	// Read oldPath, newPath
	HBufC16* oldPath = ReadDesC16LC(aMessage,0);
	HBufC16* newPath = ReadDesC16LC(aMessage,1);
	
	// Attempt to rename the directory
	User::LeaveIfError(Server().Fs().Rename(*oldPath, *newPath));
	CleanupStack::PopAndDestroy(2, oldPath); // newPath, oldPath
	}
	
void CRtaManagerHandler::RmDirL(const RMessage2& aMessage)
	{
	// Read path
	HBufC16* path = ReadDesC16LC(aMessage,0);
	
	// Attempt to remove the directory
	User::LeaveIfError(Server().Fs().RmDir(*path));
	CleanupStack::PopAndDestroy(path);
	}
	
void CRtaManagerHandler::GetDir1L(const RMessage2& aMessage)
	{
	// read path
	HBufC* name = ReadDesC16LC(aMessage,0);
	
	// read entry attribute mask
	TUint entryAttMask;
	TPckg<TUint> entryAttMaskPckg(entryAttMask);
	aMessage.Read(1, entryAttMaskPckg);
	
	// read entry sort key
	TUint entrySortKey;
	TPckg<TUint> entrySortKeyPckg(entrySortKey);
	aMessage.Read(2, entrySortKeyPckg);

	// Get Dir	
	CDir* entryList = NULL;
	User::LeaveIfError(Server().Fs().GetDir(*name, entryAttMask, entrySortKey, entryList));
	CleanupStack::PushL(entryList);
	
	// Write results to the client
	WriteCDirToMessageL(aMessage, 3, *entryList);
	
	CleanupStack::PopAndDestroy(2, name); // entryList, name
	}
	
void CRtaManagerHandler::GetDir2L(const RMessage2& aMessage)
	{
		// read path
	HBufC* name = ReadDesC16LC(aMessage,0);
	
	// read entry attribute mask
	TUint entryAttMask;
	TPckg<TUint> entryAttMaskPckg(entryAttMask);
	aMessage.Read(1, entryAttMaskPckg);
	
	// read entry sort key, packaged beside entryAttMask in the same parameter
	TUint entrySortKey;
	TPckg<TUint> entrySortKeyPckg(entrySortKey);
	aMessage.Read(1, entrySortKeyPckg, entryAttMaskPckg.MaxLength());
	
	CDirectoryStruct* dirstruct = new (ELeave) CDirectoryStruct;
	CleanupStack::PushL(dirstruct);

	User::LeaveIfError(Server().Fs().GetDir(*name, entryAttMask, entrySortKey, dirstruct->entryList, dirstruct->dirList));
	
	// copy data across into CDirStreamable
	WriteCDirToMessageL(aMessage, 2, *dirstruct->entryList);
	WriteCDirToMessageL(aMessage, 3, *dirstruct->dirList);
		
	CleanupStack::PopAndDestroy(2, name); // dirstruct, name
	}

void CRtaManagerHandler::GetDir3L(const RMessage2& aMessage)
	{
		// read path
	HBufC* name = ReadDesC16LC(aMessage,0);
	
	// read entry Uid
	TUidType entryUid;
	TPckg<TUidType> entryUidPckg(entryUid);
	aMessage.Read(1, entryUidPckg);
	
	// read entry sort key, packaged beside entryAttMask in the same parameter
	TUint entrySortKey;
	TPckg<TUint> entrySortKeyPckg(entrySortKey);
	aMessage.Read(2, entrySortKeyPckg);
	
	CDir* fileList = NULL;
	User::LeaveIfError(Server().Fs().GetDir(*name, entryUid, entrySortKey, fileList));
	
	// copy data across into CDirStreamable
	CleanupStack::PushL(fileList);
	WriteCDirToMessageL(aMessage, 3, *fileList);
		
	CleanupStack::PopAndDestroy(2, name); // fileList, name
	}

void CRtaManagerHandler::GetAttributeL(const RMessage2& aMessage)
	{
	// open the content object specified in parameter 0,1 either using virtual path or file handle
	OpenContentObjectL(aMessage, 0, 1);
	
	TAttributePckg pckgObject;
	TPckg <TAttributePckg> pckgData(pckgObject); 
	aMessage.ReadL(2, pckgData);

	User::LeaveIfError(iContentObject->GetAttribute(pckgData().iAttribute, pckgData().iValue));
	WriteL(aMessage, 2, pckgData);
	}
	
void CRtaManagerHandler::GetAttributeSetL(const RMessage2& aMessage)
	{
	// open the content object specified in parameter 0,1 either using virtual path or file handle
	OpenContentObjectL(aMessage, 0, 1);

	RAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	HBufC8* value = ReadDes8LC(aMessage, 2);
	TPtr8 valuePtr = value->Des();
	RDesReadStream readStream(valuePtr);
	attributeSet.InternalizeL(readStream);
	CleanupStack::PopAndDestroy(value);
	
	iContentObject->GetAttributeSet(attributeSet);

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	// create write stream
	RBufWriteStream writeStream(*buf);
	// write the directory to the stream
	attributeSet.ExternalizeL(writeStream);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(aMessage, 2, bufPtr);
		
	CleanupStack::PopAndDestroy(2, &attributeSet); // buf, attributeSet
	}
	
void CRtaManagerHandler::GetStringAttributeL(const RMessage2& aMessage)
	{
	// open the content object specified in parameter 0,1 either using virtual path or file handle
	OpenContentObjectL(aMessage, 0, 1);

	TStringAttributePckg pckgObject;
	TPckg<TStringAttributePckg> pckgData(pckgObject); 
	aMessage.ReadL(2, pckgData);

	User::LeaveIfError(iContentObject->GetStringAttribute(pckgData().iAttribute, pckgData().iValue));
	WriteL(aMessage, 2, pckgData);
	}
	
void CRtaManagerHandler::GetStringAttributeSetL(const RMessage2& aMessage)
	{
	// open the content object specified in parameter 0,1 either using virtual path or file handle
	OpenContentObjectL(aMessage, 0, 1);

	RStringAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	HBufC8* value = ReadDes8LC(aMessage, 2);
	TPtr8 valuePtr = value->Des();
	RDesReadStream readStream(valuePtr);
	attributeSet.InternalizeL(readStream);
	CleanupStack::PopAndDestroy(value);
	
	iContentObject->GetStringAttributeSet(attributeSet);

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	// create write stream
	RBufWriteStream writeStream(*buf);
	// write the directory to the stream
	attributeSet.ExternalizeL(writeStream);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(aMessage, 2, bufPtr);
		
	CleanupStack::PopAndDestroy(2, &attributeSet); // buf, attributeSet
	}
	
void CRtaManagerHandler::NotifyStatusChangeL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	// Server().NotifyStatusChange(aVirtualPath)
	}
	
void CRtaManagerHandler::CancelNotifyStatusChangeL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	// Server().CancelNotifyStatusChange(aVirtualPath)
	}

void CRtaManagerHandler::WriteCDirToMessageL(const RMessage2& aMessage, TInt aParam, CDir& aDir)
	{
	// Create a CDirStreamable from the directory
	CDirStreamable* dirStreamable = CDirStreamable::NewL(aDir);
	CleanupStack::PushL(dirStreamable);
	
	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	// create write stream
	RBufWriteStream writeStream(*buf);
	// write the directory to the stream
	dirStreamable->ExternalizeL(writeStream);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(aMessage, aParam, bufPtr);
	
	CleanupStack::PopAndDestroy(2, dirStreamable); // buf, dirStreamable
	}


void CRtaManagerHandler::OpenContentObjectL(const RMessage2& aMessage, TInt aUriOrFileSession, TInt aUniqueIdOrFileHandle)
	{
	// close any existing content object
	delete iArchive;
	iArchive = NULL;
	iFile.Close();
	
	switch(aMessage.Function())
		{
		case EManagerGetAttribute:
		case EManagerGetAttributeSet:
		case EManagerGetStringAttribute:
		case EManagerGetStringAttributeSet:
			{
			_LIT(KPipe,"|" );
			HBufC* uri = ReadDesC16LC(aMessage, aUriOrFileSession);

			//If uri concatenated with uid remove uid.
			TInt positionFound = uri->Find(KPipe);
			if(positionFound!= KErrNotFound)
				{
				*uri = uri->Left(positionFound); 	
				}
			User::LeaveIfError(iFile.Open(Server().Fs(),*uri, EFileShareReadersOnly | EFileRead | EFileStream));
			CleanupStack::PopAndDestroy(uri); // uri	
			break;
			}
			
			
		case EManagerGetAttributeByFileHandle:
		case EManagerGetAttributeSetByFileHandle:
		case EManagerGetStringAttributeByFileHandle:
		case EManagerGetStringAttributeSetByFileHandle:
			{
			User::LeaveIfError(iFile.AdoptFromClient(aMessage, aUriOrFileSession, aUniqueIdOrFileHandle));
			aUniqueIdOrFileHandle =3; // position of the uniqueId in message.
			break;
			}
		}
		
	iArchive = CRefTestAgentArchive::NewL(iFile);
	HBufC* uniqueId = ReadDesC16LC(aMessage, aUniqueIdOrFileHandle);	
	iContentObject = &iArchive->DrmFilesL().FindL(*uniqueId);
	
	CleanupStack::PopAndDestroy(uniqueId); // uniqueId
	}

void CRtaManagerHandler::SetPropertyL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaManagerHandler::DisplayInfoL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaManagerHandler::DisplayInfo1L(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaManagerHandler::AgentSpecificCommandL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaManagerHandler::AgentSpecificCommandAsyncL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaManagerHandler::DisplayManagementInfoL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
      
void CRtaManagerHandler::OpenWmdrmContentObjectL(const RMessage2& aMessage, TInt aHeaderParameter)     
    {     
    HBufC8* headerData = ReadDesC8LC(aMessage, aHeaderParameter);        
    iWmdrmContentObject = CWmdrmContentParser::NewL(*headerData);     
         
    CleanupStack::PopAndDestroy(headerData);     
    }     
      
void CRtaManagerHandler::GetWMDRMAttributeL(const RMessage2& aMessage)     
    {     
    OpenWmdrmContentObjectL(aMessage, 0);     
         
    TInt attribute = aMessage.Int1();     
    TInt value;     
    TPckg<TInt> valuePckg(value);     
      
    User::LeaveIfError(iWmdrmContentObject->GetAttribute(attribute, value));     
    WriteL(aMessage, 2, valuePckg);     
    }     
         
void CRtaManagerHandler::GetWMDRMStringAttributeL(const RMessage2& aMessage)     
    {     
    OpenWmdrmContentObjectL(aMessage, 0);     
      
    TInt attribute = aMessage.Int1();     
    HBufC* value = ReadDes16LC(aMessage, 2);     
    TPtr valuePtr = value->Des();     
      
    User::LeaveIfError(iWmdrmContentObject->GetStringAttribute(attribute, valuePtr));     
    WriteL(aMessage, 2, valuePtr);     
         
    CleanupStack::PopAndDestroy(value);     
    }     
         
void CRtaManagerHandler::GetWMDRMAttributeSetL(const RMessage2& aMessage)     
    {     
    OpenWmdrmContentObjectL(aMessage, 0);     
         
    RAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
         
    HBufC8* value = ReadDes8LC(aMessage, 1);     
    TPtr8 valuePtr = value->Des();     
    RDesReadStream readStream(valuePtr);     
    CleanupClosePushL(readStream);     
         
    attributeSet.InternalizeL(readStream);     
    CleanupStack::PopAndDestroy(2, value);     
         
    iWmdrmContentObject->GetAttributeSet(attributeSet);     
      
    // Write the object out to a buffer, send to client     
    CBufFlat* buf = CBufFlat::NewL(50);     
    CleanupStack::PushL(buf);     
    // create write stream     
    RBufWriteStream writeStream(*buf);     
    CleanupClosePushL(writeStream);     
         
    // write the directory to the stream     
    attributeSet.ExternalizeL(writeStream);     
    CleanupStack::PopAndDestroy(&writeStream);     
         
    TPtr8 bufPtr = buf->Ptr(0);     
    WriteL(aMessage, 1, bufPtr);     
             
    CleanupStack::PopAndDestroy(2, &attributeSet); // buf, attributeSet     
    }     
         
void CRtaManagerHandler::GetWMDRMStringAttributeSetL(const RMessage2& aMessage)     
    {     
    OpenWmdrmContentObjectL(aMessage, 0);     
         
    RStringAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
         
    HBufC8* value = ReadDes8LC(aMessage, 1);     
    TPtr8 valuePtr = value->Des();     
    RDesReadStream readStream(valuePtr);     
    CleanupClosePushL(readStream);     
         
    attributeSet.InternalizeL(readStream);     
    CleanupStack::PopAndDestroy(2, value);     
         
    iWmdrmContentObject->GetStringAttributeSetL(attributeSet);     
      
    // Write the object out to a buffer, send to client     
    CBufFlat* buf = CBufFlat::NewL(50);     
    CleanupStack::PushL(buf);     
    // create write stream     
    RBufWriteStream writeStream(*buf);     
    CleanupClosePushL(writeStream);     
         
    // write the directory to the stream     
    attributeSet.ExternalizeL(writeStream);     
    CleanupStack::PopAndDestroy(&writeStream);     
         
    TPtr8 bufPtr = buf->Ptr(0);     
    WriteL(aMessage, 1, bufPtr);     
             
    CleanupStack::PopAndDestroy(2, &attributeSet); // buf, attributeSet     
    }     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
