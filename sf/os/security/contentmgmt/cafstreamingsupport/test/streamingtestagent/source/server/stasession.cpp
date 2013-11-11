// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements CStaSession which is server-side session object.
// 
//

#include "staserver.h"
#include "srautils.h"

using namespace StreamAccess;
using namespace ContentAccess;

CStaSession::CStaSession(CStaServer& aServer)
/**
	Intializes the Streaming Test Agent session object with the server handle.
 */
	:	CScsSession(aServer)
		{
		//empty
		}


CStaSession::~CStaSession()
/**
	Destructor.
 */
	{
	delete iBuffer;	
	delete iKeyStreamSink;
	delete iRo;
	delete iSdp;
	delete iSdpDoc;
	}

	
CStaSession* CStaSession::NewL(CStaServer& aServer)
/**
	Factory function allocates new instance of CStaSession.
	
	@param aServer  Streaming Test Agent server object.
	@return			New, initialized instance of CStaSession 
					which is owned by the caller.
 */
	{
	CStaSession* s = new (ELeave) CStaSession(aServer);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}


TBool CStaSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsSession by handling the supplied message.

	@param	aFunction	Function identifier without SCS code.
	@param	aMessage	Standard server-side handle to message.
	@return 			ETrue, if the service is done successfully. Otherwise, EFalse.
 */
	{
	TStaFunction f = static_cast<TStaFunction>(aFunction);
				
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
			//RMessage2 object is closed by both TransferToClient() and SCS framework.
			//return EFalse to prevent SCS to close the message object.
			return EFalse;
		case ESendKeyStream:
			GetKeyStreamL(aMessage);
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
		case ESetIpSecAssoc:
			SetIpSecAssociationL(aMessage);
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	return ETrue;
	}//End of function DoServiceL


void CStaSession::GetAttributeL(const RMessage2& aMessage)
/**
 	Returns the value of an attribute requested by the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TAttribute attr;
	TPckg<TAttribute> attrBuf(attr);
	aMessage.ReadL(0,attrBuf);
	
	VerifyRoL();
	
	TBool retVal = EFalse;
	
	switch(attr)
		{
		case EIsProgramProtected:
			retVal = iRo->AccessRights() & EProgramProtected;
			break;
		case EIsServiceProtected:
			retVal = iRo->AccessRights() & EServiceProtected;
			break;
		case ECanExport:
			retVal = iRo->AccessRights() & ECanExportContent;
			break;
		case EMustProtectIfRecording:
			retVal = iRo->AccessRights() & EMustProtectContentIfRecording;
			break;
		case ECanPlay:
			retVal = iRo->AccessRights() & ECanPlayContent;
			break;
		default:
			PanicClient(aMessage, KErrCAOutOfRange);
		}
	
	// If the the return value is greater than 0, set the boolean value to ETrue
	if(retVal)
		{
		retVal = ETrue;
		}
	
	TPckg<TBool> retValBuf(retVal);
	aMessage.WriteL(1, retValBuf);
	}


void CStaSession::GetStringAttributeSizeL(const RMessage2& aMessage)
/**
 	Returns the size of the string attribute requested by the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TStringAttribute strAttr;
	TPckg<TStringAttribute> strAttrBuf(strAttr);
	aMessage.ReadL(0,strAttrBuf);
	
	VerifyRoL();
	TInt size = 0;
	
	switch(strAttr)
		{
		case EContentID:
			size = iRo->ContentId()->Des().Size();
			break;
		case ERightsIssuerURI:
			size = iRo->RightsIssuer()->Des().Size();
			break;
		default:
			PanicClient(aMessage, KErrCAOutOfRange);
		}
	
	TPckg<TInt> sizeBuf(size);
	aMessage.WriteL(1, sizeBuf);
	}


void CStaSession::GetStringAttributeDataL(const RMessage2& aMessage)
/**
 	Returns the value of a string attribute requested by the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TStringAttribute strAttr;
	TPckg<TStringAttribute> strAttrBuf(strAttr);
	aMessage.ReadL(0,strAttrBuf);
	
	VerifyRoL();
	HBufC8* str = NULL;
	
	switch(strAttr)
	{
	case EContentID:
		str = iRo->ContentId();
		break;
	case ERightsIssuerURI:
		str = iRo->RightsIssuer();
		break;
	default:
		PanicClient(aMessage, KErrCAOutOfRange);
	}
	TPtr8 ptr(str->Des());
	aMessage.WriteL(1, ptr);
	}


void CStaSession::GetPostDeliveryRightsL(const RMessage2& aMessage)
/**
 	Returns the file handle of the requested post-acqusition rights, its content id, 
 	the post-acquisition rights' mime type and the post-acquisition content's mime type.
 	@param	aMessage	Standard server-side handle to message.
 */ 
	{
	VerifyRoL();
	
	//In this test agent, a pre-defined post-acquisition rights object is used.
	//In real-life standards, the post-acquisition rights object would be generated
	//from the rights object which is used to decode key stream.
	
	//Get the file path of the pre-defined post acqusition rights object
	TFileName fPath(KStaPrivateFolder);
	fPath[0] = Server()->iFs.GetSystemDriveChar();
	//Convert 8-bit to 16-bit
	HBufC* fName = HBufC::NewLC(iRo->ContentId()->Des().Length());
	TPtr ptrName(fName->Des());
	ptrName.Copy(iRo->ContentId()->Des());
	fPath.Append(*fName);
	fPath.Append(KRoFileExtension);
	CleanupStack::PopAndDestroy(fName);
	
	//Open the post-acqusition rights object file
	RFile f;
	User::LeaveIfError(f.Open(Server()->iFs, fPath, EFileRead | EFileShareAny));
	CleanupClosePushL(f);
	
	//Send the content id of the post-acquisition rights object
	aMessage.WriteL(1, KPostAcquisitionCid());
	//Send the mime type of the post-acquisition rights object
	aMessage.WriteL(2, KPostAcquisitionRoMimeType());
	//Send the mime type of the post-acquisition content
	aMessage.WriteL(3, KPostAcquisitionContentMimeType());
	
	//Pass the file	handle to the client
	User::LeaveIfError(f.TransferToClient(aMessage, 0));
	//The message should have been completed
	ASSERT(aMessage.IsNull());
	CleanupStack::PopAndDestroy(&f);
	}


void CStaSession::GetKeyStreamL(const RMessage2& aMessage)
/**
 	Gets an encrypyed short-term key from the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TInt len = aMessage.GetDesMaxLengthL(0);
	HBufC8* shortTermKey = HBufC8::NewLC(len);
	TPtr8 ptr(shortTermKey->Des());
	aMessage.ReadL(0,ptr);
	
	//Check if the long-term key key exists in the short-term key
	TInt ret = shortTermKey->Find(*iRo->Key());
	if(ret == KErrNotFound)
		{
		User::Leave(KErrGeneral);
		}
	
	//Get the plain string (key)
	HBufC8* decryptedKey = HBufC8::NewLC(ret);
	TPtr8 ptrKey(decryptedKey->Des());
	ptrKey.Copy(shortTermKey->Des().Ptr(), ret);
	
	CTestKeyAssociation* decryptedAssoc = CTestKeyAssociation::NewL(ptrKey);
	CleanupStack::PushL(decryptedAssoc);
	
	iKeyStreamSink->ProcessNewKeyAssociationL(*decryptedAssoc);
	
	CleanupStack::PopAndDestroy(3, shortTermKey);
	}


void CStaSession::SetKeyStreamSinkL(const RMessage2& aMessage)
/**
 	Gets a key stream sink from the client.
 	@param	aMessage	Standard server-side handle to message.
  */
	{
	TInt len = aMessage.GetDesMaxLengthL(0);
	HBufC8* des = HBufC8::NewLC(len);
	TPtr8 ptr(des->Des());
	aMessage.ReadL(0,ptr);		
	delete iKeyStreamSink;
	iKeyStreamSink = NULL;
	TRAPD(err, 
		iKeyStreamSink = CKeyStreamSink::InternalizeLC(ptr); 
		CleanupStack::Pop(iKeyStreamSink););
	// In addition to the production supported key stream sinks, there is also the Test key stream sink
	// If the production code could not load the key stream sink, it must be the test key stream sink - load it
	if (err == KErrNotSupported)		
		{		
		RDesReadStream readStream(ptr);
		CleanupClosePushL(readStream);		
		iKeyStreamSink = CTestKeyStreamSink::NewL(readStream);
		CleanupStack::PopAndDestroy(&readStream);
		}		
	else
		User::LeaveIfError(err);	
	CleanupStack::PopAndDestroy(des);
	}

void CStaSession::SetIpSecAssociationL(const RMessage2& aMessage)
	{
	TInt len = aMessage.GetDesMaxLengthL(0);
	HBufC8* des = HBufC8::NewLC(len);
	TPtr8 ptr(des->Des());
	aMessage.ReadL(0,ptr);
	
	RDesReadStream readStream(ptr);
	CleanupClosePushL(readStream);
	
	TInt32 spi = readStream.ReadInt32L();
	HBufC8* encryptionKey = HBufC8::NewLC(readStream, 128);
	HBufC8* authenticationKey = HBufC8::NewLC(readStream, 128);
	
	CKeyAssociation *ipSecKeyAssociation = CIpSecKeyAssociation::NewLC(spi, encryptionKey, authenticationKey);			
	iKeyStreamSink->ProcessNewKeyAssociationL(*ipSecKeyAssociation);
	CleanupStack::PopAndDestroy(5, des);
	}

void CStaSession::SetSdpMediaFieldL(const RMessage2& aMessage)
/**
 	Gets an SDP object from the client.
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
	
	DoSetSdpMediaFieldL(Server()->iFs, iSdp, iRo, ptr, KStaPrivateFolder());
	CleanupStack::PopAndDestroy(des);
	
	//Leave if no RO has been found
	if(!iRo)
		{
		User::Leave(KErrCANoRights);
		}
	}

void CStaSession::SetSdpDocumentL(const RMessage2& aMessage)
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
	}

void CStaSession::VerifyRoL()
/**
 	Verifies that the rights object exists.
 */
	{
	if(!iRo)
		{//The RO defined in the SDP does not exist
		User::Leave(KErrCANoRights);
		}
	if(iRo->IsExpired())
		{
		User::Leave(KErrCANoPermission);
		}
	}

void CStaSession::PanicClient(const RMessagePtr2& aMessage, TInt aReason)
/**
	Panic the client which sent the supplied message with
	category KScsClientPanicCat and the supplied reason.

	@param	aMessage		Client message.
	@param	aReason			Why the client will be panicked.
	@see KStaClientPanicCat
 */
	{
	aMessage.Panic(KStaClientPanicCat, aReason);
	}
