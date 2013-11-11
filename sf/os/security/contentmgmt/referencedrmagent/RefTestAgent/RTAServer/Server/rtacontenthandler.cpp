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

#include "rtacontenthandler.h"
#include "RTAsession.h"
#include "RTAserver.h"
#include "rightsobject.h"
#include "rtaArchive.h"
#include "drmfilecontent.h"
#include "drmfilecontainer.h"
#include "drmfiles.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;

CRtaContentHandler* CRtaContentHandler::NewL(const CSession2& aSession)
	{
	return new (ELeave) CRtaContentHandler(aSession);
	}

CRtaContentHandler::CRtaContentHandler(const CSession2& aSession) : CRtaMessageHandler(aSession)
	{
	}
	
CRtaContentHandler::~CRtaContentHandler()
	{
	delete iArchive;
	iFile.Close();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    delete iWmdrmContentObject;     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
	}

void CRtaContentHandler::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
	{
	switch (aMessage.Function())
		{	
	// RRtaContent functions
	case EOpenFile:
	case ENoEnforceOpenFile:
		OpenFileL(aMessage);
		break;
	case EOpenFileHandle:
	case ENoEnforceOpenFileHandle:
		OpenFileHandleL(aMessage);
		break;
	case EOpenContainer:
	case ENoEnforceOpenContainer:
		OpenContainerL(aMessage);
		break;
	case ECloseContainer:
	case ENoEnforceCloseContainer:
		CloseContainerL(aMessage);
		break;
	case ESearch:
	case ENoEnforceSearch:
		SearchL(aMessage);
		break;
	case EGetEmbeddedObjects1:
	case ENoEnforceGetEmbeddedObjects1:
		GetEmbeddedObjects1L(aMessage);		
		break;
	case EGetEmbeddedObjects2:
	case ENoEnforceGetEmbeddedObjects2:
		GetEmbeddedObjects2L(aMessage);		
		break;
	case EGetAttribute:
		GetAttributeL(aMessage);
		break;
	case EGetAttributeSet:
		GetAttributeSetL(aMessage);
		break;
	case EGetStringAttribute:
		GetStringAttributeL(aMessage);
		break;
	case EGetStringAttributeSet:
		GetStringAttributeSetL(aMessage);
		break;
	case EContentAgentSpecificCommand:
	case ENoEnforceContentAgentSpecificCommand:
		AgentSpecificCommandL(aMessage);
		break;
	case EContentNotifyStatusChange:
	case ENoEnforceContentNotifyStatusChange:
		NotifyStatusChangeL(aMessage);
		break;
	case EContentCancelNotifyStatusChange:
	case ENoEnforceContentCancelNotifyStatusChange:
		CancelNotifyStatusChangeL(aMessage);
		break;
	case EContentRequestRights:
	case ENoEnforceContentRequestRights:
		RequestRightsL(aMessage);
		break;
	case EContentCancelRequestRights:
	case ENoEnforceContentCancelRequestRights:
		CancelRequestRightsL(aMessage);
		break;
	case EContentDisplayInfo:
	case ENoEnforceContentDisplayInfo:
		DisplayInfoL(aMessage);
		break;
	case EContentSetProperty:
	case ENoEnforceContentSetProperty:
		SetPropertyL(aMessage);
		break;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    case EReadWMDRMHeaderData:     
    case ENoEnforceReadWMDRMHeaderData:     
        ReadWMDRMHeaderL(aMessage);     
        break;     
         
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


void CRtaContentHandler::OpenFileL(const RMessage2& aMessage)
	{
	TUint mode;
	TPckg <TUint> fileModePckg(mode);
	
	HBufC* fileName = ReadDesC16LC(aMessage, 0);
	aMessage.ReadL(1, fileModePckg);
	
	User::LeaveIfError(Server().CheckPermittedFileName(*fileName));
	User::LeaveIfError(iFile.Open(Server().Fs(), *fileName, mode));
	
	// create a DRM archive based upon the file handle 
	iArchive = CRefTestAgentArchive::NewL(iFile);
	CleanupStack::PopAndDestroy(fileName); 
	}
	
void CRtaContentHandler::OpenFileHandleL(const RMessage2& aMessage)
	{
	User::LeaveIfError(iFile.AdoptFromClient(aMessage, 0, 1));
	
	// create a DRM archive based upon the file handle 
	iArchive = CRefTestAgentArchive::NewL(iFile);
	}


void CRtaContentHandler::OpenContainerL(const RMessage2& aMessage)
	{
	HBufC* uniqueId = ReadDesC16LC(aMessage, 0);

	User::LeaveIfError(iArchive->DrmFilesL().OpenContainer(*uniqueId));
	
	CleanupStack::PopAndDestroy(uniqueId);
	}

void CRtaContentHandler::CloseContainerL(const RMessage2& /*aMessage*/)
	{
	User::LeaveIfError(iArchive->DrmFilesL().CloseContainer());
	}

void CRtaContentHandler::GetEmbeddedObjects1L(const RMessage2& aMessage)
	{
	RStreamablePtrArray<CEmbeddedObject> array;
	CleanupClosePushL(array);
	
	iArchive->DrmFilesL().CurrentContainer().GetEmbeddedObjectsL(array);

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	// create write stream
	RBufWriteStream writeStream(*buf);
	// write the directory to the stream
	array.ExternalizeL(writeStream);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(aMessage, 0, bufPtr);
	
	CleanupStack::PopAndDestroy(2, &array); // buf, array
	}

void CRtaContentHandler::GetEmbeddedObjects2L(const RMessage2& aMessage)
	{
	RStreamablePtrArray<CEmbeddedObject> array;
	CleanupClosePushL(array);
	
	TEmbeddedType type;
	TPckg<TEmbeddedType> typePckg(type);
	
	aMessage.Read(1, typePckg);
	
	iArchive->DrmFilesL().CurrentContainer().GetEmbeddedObjectsL(array, type);

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	// create write stream
	RBufWriteStream writeStream(*buf);
	// write the directory to the stream
	array.ExternalizeL(writeStream);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(aMessage, 0, bufPtr);
	
	CleanupStack::PopAndDestroy(2, &array); // buf, array
	}

void CRtaContentHandler::SearchL(const RMessage2& aMessage)
	{
	RStreamablePtrArray<CEmbeddedObject> array;
	CleanupClosePushL(array);
	
	HBufC8* mimeType = ReadDesC8LC(aMessage, 1);
	TBool recursive;
	TPckg<TBool> recursivePckg(recursive);
	
	aMessage.Read(2, recursivePckg);
	
	User::LeaveIfError(iArchive->DrmFilesL().CurrentContainer().Search(array, *mimeType, recursive));

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	// create write stream
	RBufWriteStream writeStream(*buf);
	// write the directory to the stream
	array.ExternalizeL(writeStream);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(aMessage, 0, bufPtr);
	
	CleanupStack::PopAndDestroy(3, &array); // buf, mimeType, array
	}

void CRtaContentHandler::GetAttributeL(const RMessage2& aMessage)
	{
	TInt attribute;
	TPckg <TInt> attributePckg(attribute);

	TInt value;
	TPckg <TInt> valuePckg(value);			

	HBufC* uniqueId = ReadDesC16LC(aMessage, 0);

	aMessage.ReadL(1, attributePckg);
	
	iArchive->DrmFilesL().FindL(*uniqueId).GetAttribute(attribute, value);
	
	WriteL(aMessage, 2, valuePckg);
	
	CleanupStack::PopAndDestroy(uniqueId);
	}
	
void CRtaContentHandler::GetAttributeSetL(const RMessage2& aMessage)
	{
	RAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	HBufC* uniqueId = ReadDesC16LC(aMessage, 0);
	
	HBufC8* value = ReadDes8LC(aMessage, 1);
	TPtr8 valuePtr = value->Des();
	RDesReadStream readStream(valuePtr);
	attributeSet.InternalizeL(readStream);
	CleanupStack::PopAndDestroy(value);
	
	iArchive->DrmFilesL().FindL(*uniqueId).GetAttributeSet(attributeSet);

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	// create write stream
	RBufWriteStream writeStream(*buf);
	// write the directory to the stream
	attributeSet.ExternalizeL(writeStream);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(aMessage, 1, bufPtr);
		
	CleanupStack::PopAndDestroy(3, &attributeSet); // buf, attributeSet, uniqueId
	}
	
void CRtaContentHandler::GetStringAttributeL(const RMessage2& aMessage)
	{
	TInt attribute;
	TPckg <TInt> attributePckg(attribute);
	
	HBufC* uniqueId = ReadDesC16LC(aMessage, 0);
	
	aMessage.ReadL(1, attributePckg);
	
	HBufC* value = ReadDes16LC(aMessage, 2);
	TPtr valuePtr = value->Des();
	
	User::LeaveIfError(iArchive->DrmFilesL().FindL(*uniqueId).GetStringAttribute(attribute, valuePtr));
	
	WriteL(aMessage, 2, valuePtr);
	
	CleanupStack::PopAndDestroy(2, uniqueId); // value, uniqueId
	}
	
void CRtaContentHandler::GetStringAttributeSetL(const RMessage2& aMessage)
	{
	RStringAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	HBufC* uniqueId = ReadDesC16LC(aMessage, 0);
	
	HBufC8* value = ReadDes8LC(aMessage, 1);
	TPtr8 valuePtr = value->Des();
	RDesReadStream readStream(valuePtr);
	attributeSet.InternalizeL(readStream);
	CleanupStack::PopAndDestroy(value);
	
	iArchive->DrmFilesL().FindL(*uniqueId).GetStringAttributeSet(attributeSet);

	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	// create write stream
	RBufWriteStream writeStream(*buf);
	// write the directory to the stream
	attributeSet.ExternalizeL(writeStream);
	TPtr8 bufPtr = buf->Ptr(0);
	WriteL(aMessage, 1, bufPtr);
		
	CleanupStack::PopAndDestroy(3, &attributeSet); // buf, attributeSet, uniqueid
	}
	
void CRtaContentHandler::AgentSpecificCommandL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaContentHandler::AgentSpecificCommandAsyncL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaContentHandler::NotifyStatusChangeL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaContentHandler::CancelNotifyStatusChangeL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaContentHandler::RequestRightsL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaContentHandler::CancelRequestRightsL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaContentHandler::DisplayInfoL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

void CRtaContentHandler::SetPropertyL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrCANotSupported);
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
      
void CRtaContentHandler::ReadWMDRMHeaderL(const RMessage2& aMessage)     
    {     
    HBufC8* headerData = ReadDesC8LC(aMessage, 0);       
    iWmdrmContentObject = CWmdrmContentParser::NewL(*headerData);     
         
    CleanupStack::PopAndDestroy(headerData);     
    }     
         
void CRtaContentHandler::GetWMDRMAttributeL(const RMessage2& aMessage)     
    {     
    TInt attribute = aMessage.Int0();     
    TInt value;     
    TPckg<TInt> valuePkg(value);     
      
    User::LeaveIfError(iWmdrmContentObject->GetAttribute(attribute, value));     
    WriteL(aMessage, 1, valuePkg);     
    }     
         
void CRtaContentHandler::GetWMDRMStringAttributeL(const RMessage2& aMessage)     
    {     
    TInt attribute = aMessage.Int0();     
    HBufC* value = ReadDes16LC(aMessage, 1);     
    TPtr valuePtr = value->Des();     
      
    User::LeaveIfError(iWmdrmContentObject->GetStringAttribute(attribute, valuePtr));     
    WriteL(aMessage, 1, valuePtr);     
         
    CleanupStack::PopAndDestroy(value);     
    }     
         
void CRtaContentHandler::GetWMDRMAttributeSetL(const RMessage2& aMessage)     
    {     
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
         
void CRtaContentHandler::GetWMDRMStringAttributeSetL(const RMessage2& aMessage)     
    {     
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

