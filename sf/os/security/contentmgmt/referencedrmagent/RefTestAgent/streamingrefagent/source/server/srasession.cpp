/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements CSraSession which is server-side session object.
* See class and function definitions for more detail.
*
*/


#include "sraserver.h"
#include "srarightsobject.h"
#include "sraprocessor.h"
#include "srautils.h"
#include <s32file.h>
#include "sdpconnectionfield.h"

CSraSession::CSraSession()
//constructor
	{
	//empty
	}


CSraSession::~CSraSession()
//destructor
	{
	CSraServer* server = GetServerHandle();
	server->RemoveSession();
		
	delete iKeyStreamSink;
	delete iRo;
	delete iSdp;
	delete iSdpDoc;
	delete iProcessor;
	iMutex.Close();
	}

CSraServer* CSraSession::GetServerHandle()
/**
 	@return A pointer to the reference agent server object.
 */
	{
	CSraServer* s = static_cast<CSraServer*>(const_cast<CServer2*>(Server()));
	ASSERT(s);
	return s;
	}

CSraSession* CSraSession::NewL()
/**
 	Create a new streaming reference agent session object.
 	@return Pointer to a fully constructed CSraSession object. Ownership is passed to the caller.
 */
	{
	CSraSession* self = new(ELeave) CSraSession();
	return self;
	}

void CSraSession::CreateL()
/**
 	Override CSession2::CreateL function by incrementing the session counter
 	of the server object. This function is called by the CServer2 framework.
 	
 	@see CSession2::CreateL
 */
	{
	CSraServer* server = GetServerHandle();
	if(!server)
		{
		User::Leave(KErrBadHandle);
		}
	server->AddSession();
	}

void CSraSession::ServiceL(const RMessage2& aMessage)
/**
 	Override CSession2::ServiceL by handling the servicing of a client request.
	
	@param	aMessage	Standard server-side handle to message.
	@see CSession2::ServiceL
 */
	{
	TSraFunction f = static_cast<TSraFunction>(aMessage.Function());
	
	switch(f)
		{
		case EGetAttribute:
			GetAttributeL(aMessage);
			break;
		case EGetStringAttributeSize:
			GetStringAttributeSizeL(aMessage);
			break;
		case EGetStringAttributeData:
			GetStringAttributeDataL(aMessage);
			break;
		case EGetPostDeliveryRights:
			GetPostDeliveryRightsL(aMessage);
			break;
		case ESetKeyStreamSink:
			SetKeyStreamSinkL(aMessage);
			break;
		case ESetSdpKeyStream:
			SetSdpMediaFieldL(aMessage);
			break;
		case ESetSdpDocument:
			SetSdpDocumentL(aMessage);
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	}

void CSraSession::ServiceError(const RMessage2& aMessage, TInt aError)
/**
 	Override CSession2::ServiceError by handling the situation when the ServiceL leaves.

	Panick the client if the leave is because of a bad descriptor.	 
	Otherwise, complete the request with the error code.

	@param	aMessage		The message which caused leave to occur.
	@param	aError			The leave code.	 This is a Symbian OS error code.
	@see CSession2::ServiceError
 */
	{
	if (aError == KErrBadDescriptor)
		PanicClient(aMessage, ESraClientBadDesc);
	else
		aMessage.Complete(aError);
	}


void CSraSession::GetAttributeL(const RMessage2& aMessage)
/**
 	Returns the value of an attribute requested by the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TAttribute attribute;
	TPckg<TAttribute> package(attribute);
	aMessage.ReadL(0, package);
	
	TBool value = EFalse;
	
	switch(attribute)
		{
		case EIsProgramProtected:
			value = iRo->AccessRights() & EProgramProtected;
			break;
		case EIsServiceProtected:
			value = iRo->AccessRights() & EServiceProtected;
			break;
		case ECanExport:
			value = iRo->AccessRights() & ECanExportContent;
			break;
		case EMustProtectIfRecording:
			value = iRo->AccessRights() & EMustProtectContentIfRecording;
			break;
		case ECanPlay:
			value = iRo->AccessRights() & ECanPlayContent;
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	
	TPckg<TBool> packageVal(value);
	aMessage.WriteL(1, packageVal);
	aMessage.Complete(KErrNone);
	}


void CSraSession::GetStringAttributeSizeL(const RMessage2& aMessage)
/**
 	Returns the size of the string attribute requested by the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TStringAttribute strAttribute;
	TPckg<TStringAttribute> package(strAttribute);
	aMessage.ReadL(0, package);
	
	TInt size = 0;
	
	switch(strAttribute)
		{
		case ERightsIssuerURI:
			size = iRo->RightsIssuer()->Des().Size();
			break;
		case EContentID:
			size = iRo->ContentId()->Des().Size();
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	
	TPckg<TInt> packageSize(size);
	aMessage.WriteL(1, packageSize);
	aMessage.Complete(KErrNone);
	}


void CSraSession::GetStringAttributeDataL(const RMessage2& aMessage)
/**
 	Returns the value of a string attribute requested by the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TStringAttribute strAttribute;
	TPckg<TStringAttribute> package(strAttribute);
	aMessage.ReadL(0, package);
	
	HBufC8* value(0);
	
	switch(strAttribute)
		{
	case ERightsIssuerURI:
		value = iRo->RightsIssuer();
		break;
	case EContentID:
		value = iRo->ContentId();
		break;
	default:
		User::Leave(KErrNotSupported);
		}
	
	TPtr8 ptr(value->Des());
	aMessage.WriteL(1, ptr);
	aMessage.Complete(KErrNone);
	}


void CSraSession::GetPostDeliveryRightsL(const RMessage2& aMessage)
/**
 	Returns a file handle to the requested post-acqusition rights object, its content id, 
 	the post-acquisition rights' mime type and the post-acquisition content's mime type.
 	
 	@param	aMessage	Standard server-side handle to message.
 */ 
	{
	_LIT(KPostAcqRoPath, "C:\\private\\10285A1C\\postacquisition.dr");
	
	// In this reference agent, it is assumed that a post-acquisition rights object (KPostAcqRoPath)
	// exists in the private folder of the agent.
	// In real-life standards, the post-acquisition rights object would be generated
	// from the rights object which is used to decode key stream.

	// Open the post-acqusition rights object file
	RFile f;
	User::LeaveIfError(f.Open(GetServerHandle()->iFs, KPostAcqRoPath(), EFileRead | EFileShareAny));
	CleanupClosePushL(f);
	
	// Send the content id of the post-acquisition rights object
	// In a real agent, this content id is extracted from the pos-acquisition RO.
	aMessage.WriteL(1, KPostAcquisitionCid());
	// Send the mime type of the post-acquisition rights object. This mime type is specific to
	// the reference agent of the streaming CAF and used to import post-acquisition rights object 
	// into the reference agent of the regular CAF by the client.
	// A real streaming agent shall use a rights mime type recognized by an existing CAF agent
	// to import post-acquisition rights objects.
	aMessage.WriteL(2, KPostAcquisitionRoMimeType());
	// Send the mime type of the post-acquisition content. This mime type is specific to
	// the reference agent of the streaming CAF and used to import protected streamed content 
	// into the reference agent of the regular CAF by the client.
	// A real streaming agent shall use a content mime type recognized by an existing CAF agent
	// to import a protected streamed content. 
	aMessage.WriteL(3, KPostAcquisitionContentMimeType());
	
	// Pass the file	handle to the client
	User::LeaveIfError(f.TransferToClient(aMessage, 0));
	
	// The message should have been completed
	ASSERT(aMessage.IsNull());
	
	// Close the post-acquisition rights object file.
	CleanupStack::PopAndDestroy(&f);
	}


void CSraSession::SetKeyStreamSinkL(const RMessage2& aMessage)
/**
 	Obtain a key stream sink from the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TInt len = aMessage.GetDesMaxLengthL(0);
	HBufC8* desc = HBufC8::NewLC(len);
	TPtr8 ptr(desc->Des());
	aMessage.ReadL(0,ptr);
	
	delete iKeyStreamSink;
	iKeyStreamSink = NULL;
	
	// It is not possible, by using standard object transferring methods in client-server communication,
	// to create a virtual base class on the server-side without losing its type-specific information.
	// Since CKeyStreamSink is a virtual interface and is passed from client to server, special care
	// must be applied so that type-specific information won't be lost after the transfer. 
	// CKeyStreamSink class provides such functionality but serializing the type identifier and 
	// using it on the server-side to generate the correct instance of CKeyStreamSink.
	iKeyStreamSink = CKeyStreamSink::InternalizeLC(ptr); 
	
	// Encryption and authentication algorithm of the key stream sink must be set
	// by the agent when it receives the key stream sink from the client. Because 
	// these depend on the streaming standard, the client may not know the values,
    // and they will be available only at the server.
	iKeyStreamSink->SetEncryptionAlgorithmL(EAES_128_CBC);
	iKeyStreamSink->SetAuthenticationAlgorithmL(EHMAC_SHA1);
	
	CleanupStack::Pop(iKeyStreamSink);
	CleanupStack::PopAndDestroy(desc);
	aMessage.Complete(KErrNone);
	}


void CSraSession::SetSdpMediaFieldL(const RMessage2& aMessage)
/**
 	Sets the SDP key stream field coming from the client.
 	
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TInt len = aMessage.GetDesMaxLengthL(0);
	HBufC8* des = HBufC8::NewLC(len);
	TPtr8 ptr(des->Des());
	aMessage.ReadL(0,ptr);
	
	//Delete previous SDP and rights objects
	delete iSdp;
	iSdp = NULL;
	delete iRo;
	iRo = NULL;
	
	DoSetSdpMediaFieldL(GetServerHandle()->iFs, iSdp, iRo, ptr, KSraPrivateFolder());
	CleanupStack::PopAndDestroy(des);

	// Leave if no RO has been found.
	// Please note that the reference to RO may arrive in the key stream in DVB-H and 
	// other real-life standards. In that case, the presence of a matching RO won't be validated here.
	if(!iRo)
		{
		User::Leave(KErrCANoRights);
		}
	StartProcessorL();
	aMessage.Complete(KErrNone);
	}


void CSraSession::SetSdpDocumentL(const RMessage2& aMessage)
/**
 	Sets the SDP document object coming from the client.
 	
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TInt len = aMessage.GetDesMaxLengthL(0);
	HBufC8* des = HBufC8::NewLC(len);
	TPtr8 ptr(des->Des());
	aMessage.ReadL(0,ptr);
	
	//Delete previous SDPDoc
	delete iSdpDoc;
	iSdpDoc = NULL;
	
	// Decode the SDP document object from the encoded data
	iSdpDoc = CSdpDocument::DecodeL(*des);
	CleanupStack::PopAndDestroy(des);
	
	aMessage.Complete(KErrNone);
	}


void CSraSession::StartProcessorL()
/**
 	Start the key processor which creates key associations from the received key information
 	and then process that by using the key stream sink.
 */
	{
	// Mutex is used to synchronise the agent with the test harness.
	// A real-life agent should not need to use a mutex object.
	User::LeaveIfError(iMutex.CreateGlobal(KSraMutexName));
	// Mutex has been created and now it is acquired.
	iMutex.Wait();
	
	//Obtain the IP address which is listened by the processor and convert it into 16-bit
	CSdpConnectionField* sdpConn(0);
	
	if(iSdp->ConnectionFields().Count() == 1)
		{
		//There is a connection field at media field level. Use it.
		sdpConn = iSdp->ConnectionFields()[0];
		}
	else
		{
		// No connection field at media level. Use the one at session level.
		sdpConn = iSdpDoc->ConnectionField();
		}
	
	HBufC* addr16 = HBufC::NewLC(sdpConn->Address().Length());
	TPtr ptr16(addr16->Des());
	ptr16.Copy(sdpConn->Address());
	
	// Create a key processor
	// The IP address and port of the connection field are sent to the processor
	// which listens to the encrypted key stream. 
	// iProcessor = CSraProcessor::NewL(*this, *addr16, iSdp->Port());
	
	// However, here, the default IP address (0.0.0.0) will be used to listen to. 
	// Because in the reference agent test, the key stream is sent by another process (test harness) on the device.
	iProcessor = CSraProcessor::NewL(*this, _L("0.0.0.0"), iSdp->Port());
	CleanupStack::PopAndDestroy(addr16);
	
	// Launch the processor
	iProcessor->StartProcessing();
	}
