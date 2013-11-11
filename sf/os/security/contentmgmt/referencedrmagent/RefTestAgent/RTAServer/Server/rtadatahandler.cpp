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

#include "rtadatahandler.h"
#include "RTAsession.h"
#include "RTAserver.h"
#include "rightsobject.h"
#include "rtaArchive.h"
#include "drmfilecontent.h"
#include "drmfiles.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;

CRtaDataHandler* CRtaDataHandler::NewL(const CSession2& aSession)
	{
	return new (ELeave) CRtaDataHandler(aSession);
	}

CRtaDataHandler::CRtaDataHandler(const CSession2& aSession) : CRtaMessageHandler(aSession)
	{
	}
	
CRtaDataHandler::~CRtaDataHandler()
	{
	delete iArchive;
	iFile.Close();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT          
    delete iWmdrmContentObject;     
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
	}

void CRtaDataHandler::ServiceL(const RMessage2& aMessage)
//
// Handle a client request.
	{
	switch (aMessage.Function())
		{	
	// RRtaData functions
	case EOpenFile:
	case ENoEnforceOpenFile:
		OpenFileL(aMessage);
		break;
	case EOpenFileHandle:
	case ENoEnforceOpenFileHandle:
		OpenFileHandleL(aMessage);
		break;
	case EEvaluateIntent:
	case ENoEnforceEvaluateIntent:
		EvaluateIntentL(aMessage);
		break;
	case EExecuteIntent:
	case ENoEnforceExecuteIntent:
		ExecuteIntentL(aMessage);
		break;
	case EReadData:
	case ENoEnforceReadData:
		ReadDataL(aMessage);
		break;
	case EReadOffsetData:
	case ENoEnforceReadOffsetData:
		ReadOffsetDataL(aMessage);
		break;
	case ESeek:
	case ENoEnforceSeek:
		SeekL(aMessage);
		break;
	case EDataSize:
	case ENoEnforceDataSize:
		DataSizeL(aMessage);
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
	case EDataSetProperty:
	case ENoEnforceDataSetProperty:
		SetPropertyL(aMessage);
		break;
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
    case EReadWMDRMHeaderData:     
    case ENoEnforceReadWMDRMHeaderData:     
        ReadWMDRMHeaderL(aMessage);     
        break;     
         
    case EReadWMDRMDataPacket:     
    case ENoEnforceReadWMDRMDataPacket:     
        DecryptWMDRMDataPacketL(aMessage);     
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


void CRtaDataHandler::OpenFileL(const RMessage2& aMessage)
	{
	TUint mode;
	TPckg <TUint> fileModePckg(mode);
	
	HBufC* fileName = ReadDesC16LC(aMessage, 0);
	HBufC* uniqueId= ReadDesC16LC(aMessage, 1);
	aMessage.ReadL(2, fileModePckg);

	User::LeaveIfError(Server().CheckPermittedFileName(*fileName));
	User::LeaveIfError(iFile.Open(Server().Fs(), *fileName, mode));
	
	// create a DRM archive based upon the file handle 
	iArchive = CRefTestAgentArchive::NewL(iFile);
	iContentObject = &iArchive->DrmFilesL().FindL(*uniqueId);
	
	CleanupStack::PopAndDestroy(2, fileName); // fileName, UniqueId
	}
	
void CRtaDataHandler::OpenFileHandleL(const RMessage2& aMessage)
	{
	User::LeaveIfError(iFile.AdoptFromClient(aMessage, 0, 1));
	HBufC* uniqueId = ReadDesC16LC(aMessage, 2);	
	
	// create a DRM archive based upon the file handle 
	iArchive = CRefTestAgentArchive::NewL(iFile);
	iContentObject = &iArchive->DrmFilesL().FindL(*uniqueId);
	
	CleanupStack::PopAndDestroy(uniqueId); // file, UniqueId
	}

void CRtaDataHandler::EvaluateIntentL(const RMessage2& aMessage)
	{
	TIntent intent;
	TPckg <TIntent> intentPckg(intent);
	aMessage.ReadL(0, intentPckg);
	
	Server().EvaluateIntentL(iContentObject->Cid(), intent);	
	}

void CRtaDataHandler::ExecuteIntentL(const RMessage2& aMessage)
	{
	TIntent intent;
	TPckg <TIntent> intentPckg(intent);
	aMessage.ReadL(0, intentPckg);

	Server().ExecuteIntentL(iContentObject->Cid(), intent);
	}

void CRtaDataHandler::ReadDataL(const RMessage2& aMessage)
	{
	TInt length = 0;
	TPckg <TInt> lengthPckg(length);
	
	HBufC8* buffer = ReadDes8LC(aMessage, 0);
	aMessage.ReadL(1, lengthPckg);

	TPtr8 bufPtr = buffer->Des();	
	User::LeaveIfError(iContentObject->Read(bufPtr, length));
	
	WriteL(aMessage, 0, *buffer);
	
	CleanupStack::PopAndDestroy(buffer);
	}

void CRtaDataHandler::ReadOffsetDataL(const RMessage2& aMessage)
	{
	TInt length = 0;
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TInt64 pos = 0;
	TPckg <TInt64> posPckg(pos);
#else
	TInt pos = 0;
	TPckg <TInt> posPckg(pos);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TPckg <TInt> lengthPckg(length);
	
	aMessage.ReadL(0, posPckg);
	HBufC8* buffer = ReadDes8LC(aMessage, 1);
	aMessage.ReadL(2, lengthPckg);

	TPtr8 bufPtr = buffer->Des();	
	User::LeaveIfError(iContentObject->Read(pos, bufPtr, length));
	
	WriteL(aMessage, 1, *buffer);
	
	CleanupStack::PopAndDestroy(buffer);
	}

	
void CRtaDataHandler::SeekL(const RMessage2& aMessage)
	{
	TSeek mode;
	TPckg <TSeek> modePckg(mode);
	aMessage.ReadL(0, modePckg);
	
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TInt64 pos = 0;
	TPckg <TInt64> posPckg(pos);
#else
	TInt pos = 0;
	TPckg <TInt> posPckg(pos);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API	
	aMessage.ReadL(1, posPckg);
	
	User::LeaveIfError(iContentObject->Seek(mode, pos));
	
	WriteL(aMessage, 1, posPckg);
	}

void CRtaDataHandler::DataSizeL(const RMessage2& aMessage)
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TInt64 size = 0;
	TPckg <TInt64> sizePckg(size);
#else
	TInt size = 0;
	TPckg <TInt> sizePckg(size);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	aMessage.ReadL(0, sizePckg);
	
	User::LeaveIfError(iContentObject->Size(size));
	
	WriteL(aMessage, 0, sizePckg);
	}

void CRtaDataHandler::GetAttributeL(const RMessage2& aMessage)
	{
	TInt attribute;
	TPckg <TInt> attributePckg(attribute);

	TInt value;
	TPckg <TInt> valuePckg(value);			

	aMessage.ReadL(0, attributePckg);
	
	iContentObject->GetAttribute(attribute, value);
	
	WriteL(aMessage, 1, valuePckg);
	}
	
void CRtaDataHandler::GetAttributeSetL(const RMessage2& aMessage)
	{
	RAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	HBufC8* value = ReadDes8LC(aMessage, 0);
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
	WriteL(aMessage, 0, bufPtr);
		
	CleanupStack::PopAndDestroy(2, &attributeSet); // buf, attributeSet
	}
	
void CRtaDataHandler::GetStringAttributeL(const RMessage2& aMessage)
	{
	TInt attribute;
	TPckg <TInt> attributePckg(attribute);
	aMessage.ReadL(0, attributePckg);
	
	HBufC* value = ReadDes16LC(aMessage, 1);
	TPtr valuePtr = value->Des();
	
	User::LeaveIfError(iContentObject->GetStringAttribute(attribute, valuePtr));
	
	WriteL(aMessage, 1, valuePtr);
	
	CleanupStack::PopAndDestroy(value);
	}
	
void CRtaDataHandler::GetStringAttributeSetL(const RMessage2& aMessage)
	{
	RStringAttributeSet attributeSet;
	CleanupClosePushL(attributeSet);
	
	HBufC8* value = ReadDes8LC(aMessage, 0);
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
	WriteL(aMessage, 0, bufPtr);
		
	CleanupStack::PopAndDestroy(2, &attributeSet); // buf, attributeSet
	}

void CRtaDataHandler::SetPropertyL(const RMessage2& /*aMessage*/)
	{	
	User::Leave(KErrCANotSupported);
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
      
void CRtaDataHandler::ReadWMDRMHeaderL(const RMessage2& aMessage)     
    {     
    HBufC8* headerData = ReadDesC8LC(aMessage, 0);       
    iWmdrmContentObject = CWmdrmContentParser::NewL(*headerData);     
         
    CleanupStack::PopAndDestroy(headerData);     
    }     
         
void CRtaDataHandler::DecryptWMDRMDataPacketL(const RMessage2& aMessage)     
    {     
    HBufC8* inputPacket = ReadDesC8LC(aMessage, 0);     
         
    HBufC8* decryptedPacket = ReadDes8LC(aMessage, 1);     
    TPtr8 decryptedPacketPtr = decryptedPacket->Des();     
         
    decryptedPacketPtr.Copy(*inputPacket);     
         
    WriteL(aMessage, 1, decryptedPacketPtr);     
         
    CleanupStack::PopAndDestroy(2, inputPacket);     
    }     
         
void CRtaDataHandler::GetWMDRMAttributeL(const RMessage2& aMessage)     
    {     
    TInt attribute = aMessage.Int0();     
    TInt value;     
    TPckg <TInt> valuePckg(value);               
    User::LeaveIfError(iWmdrmContentObject->GetAttribute(attribute, value));     
         
    WriteL(aMessage, 1, valuePckg);     
    }     
         
void CRtaDataHandler::GetWMDRMStringAttributeL(const RMessage2& aMessage)     
    {     
    TInt attribute = aMessage.Int0();     
    HBufC* value = ReadDes16LC(aMessage, 1);     
    TPtr valuePtr = value->Des();     
         
    User::LeaveIfError(iWmdrmContentObject->GetStringAttribute(attribute, valuePtr));     
    WriteL(aMessage, 1, valuePtr);     
         
    CleanupStack::PopAndDestroy(value);     
    }     
         
void CRtaDataHandler::GetWMDRMAttributeSetL(const RMessage2& aMessage)     
    {     
    RAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
         
    HBufC8* value = ReadDes8LC(aMessage, 0);     
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
    WriteL(aMessage, 0, bufPtr);     
             
    CleanupStack::PopAndDestroy(2, &attributeSet); // buf, attributeSet     
    }     
         
void CRtaDataHandler::GetWMDRMStringAttributeSetL(const RMessage2& aMessage)     
    {     
    RStringAttributeSet attributeSet;     
    CleanupClosePushL(attributeSet);     
         
    HBufC8* value = ReadDes8LC(aMessage, 0);     
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
    WriteL(aMessage, 0, bufPtr);     
             
    CleanupStack::PopAndDestroy(2, &attributeSet); // buf, attributeSet     
    }        
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 
