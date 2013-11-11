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
// RStaClient implementation.	 See class and function definitions
// for more detail.
// 
//

#include "staclient.h"

using namespace StreamAccess;
using namespace ContentAccess;

EXPORT_C RStaClient::RStaClient()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
	: RScsClientBase()
		{
		//empty
		}
		
	
EXPORT_C TInt RStaClient::Connect()
/**
	Connect to the Streaming Test Agent server.

	@return		Symbian OS error code where KErrNone indicates
				success and any other value indicates failure.
 */
	{
	TVersion v = StaVersion();
	TUidType fullUid = StaImageFullUid();
	
	return RScsClientBase::Connect(KStaName, v, KStaImageName, fullUid);
	}
	
	
EXPORT_C void RStaClient::Close()
/**
	Closes the streaming test agent connection.
*/
	{
	RScsClientBase::Close();
	}


EXPORT_C void RStaClient::GetAttributeL(const TAttribute& aAttribute, TBool& aValue)
/**
 	Gets the value of a requested attribute listed in TAttribute enumeration.
 	
 	@param aAttribute The requested attribute.
 	@param aValue     The value of the requested attribute
 	
 	@see StreamAccess::TAttribute
 */
	{
	TPckg<TAttribute> attrBuf(aAttribute);
	TPckg<TBool> valueBuf(aValue);
	User::LeaveIfError(CallSessionFunction(EGetAttribute, TIpcArgs(&attrBuf, &valueBuf)));
	}


EXPORT_C HBufC* RStaClient::GetStringAttributeLC(const TStringAttribute& aAttribute)
/**
 	Gets the value of a requested string attribute listed in TStringAttribute enumeration.
 	
 	@param aAttribute The requested string attribute.
 	@return    A pointer to the value of the requested string attribute. 
 			   The ownership is transfered to the calling function.
 	@see StreamAccess::TStringAttribute
 */
	{
	TPckg<TStringAttribute> strAttr(aAttribute);
	TInt size = 0;
	TPckg<TInt> sizeBuf(size);
	User::LeaveIfError(CallSessionFunction(EGetStringAttributeSize, TIpcArgs(&strAttr, &sizeBuf)));
	
	HBufC8* value8 = HBufC8::NewLC(size);
	TPtr8 ptr8(value8->Des());
	
	User::LeaveIfError(CallSessionFunction(EGetStringAttributeData, TIpcArgs(&strAttr, &ptr8)));
	
	//Convert 8-bit to 16-bit
	HBufC* value = HBufC::NewL(ptr8.Size());
	TPtr ptr(value->Des());
	ptr.Copy(ptr8);

	CleanupStack::PopAndDestroy(value8);
	CleanupStack::PushL(value);
	return value;
	}


EXPORT_C void RStaClient::GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType)
/**
 	@param aFile The file handle of the post-delivery rights object obtained from 
 		    	 the streaming test agent server. The receiving party is responsible
 		    	 to close the file handle.
 	@param aCid  The content Id
 	@param aRightsMimeType The mime type of the post-acquisition rights object.
	@param aContentMimeType The mime type of the content.
 */
	{
	TInt fsh;			//session (RFs) handle
	TPckgBuf<TInt> fh;	//subsession (RFile) handle
	
	//The message is completed with the file-server (RFs) session handle
	//Pointer to the file subsession handle in slot 0
	fsh = CallSessionFunction(EGetPostDeliveryRights, TIpcArgs(&fh, &aCid, &aRightsMimeType, &aContentMimeType));
	User::LeaveIfError(fsh);
	
	// Adopt the file using the returned handles
	User::LeaveIfError(aFile.AdoptFromServer(fsh,fh()));
	}

EXPORT_C void RStaClient::SendKeyStreamL(const TDesC8& aKey) const
/**
 	Sends an encrypted short-term key to the test agent server.
 	@param aKey An encrypted key.
 */
	{
	User::LeaveIfError(CallSessionFunction(ESendKeyStream, TIpcArgs(&aKey)));
	}

EXPORT_C void RStaClient::SetKeyStreamSinkL(const CKeyStreamSink& aSink) const
/**
 	Sends a key stream sink object to the test agent server.
 	@param aKey A key stream sink object.
 */
	{
	TPtr8 ptr(aSink.ExternalizeLC());
	User::LeaveIfError(CallSessionFunction(ESetKeyStreamSink, TIpcArgs(&ptr)));
	CleanupStack::PopAndDestroy(); // Delete the allocated buffer
	}

EXPORT_C void RStaClient::SetSdpMediaFieldL(const CSdpMediaField& aSdp) const
/**
 	Sends an SDP key stream field to the test agent server.
 	@param aKey An SDP object data.
 */
	{
	HBufC8* sdpBuf(0);
	TPtr8 ptr(EncodeLC(aSdp, sdpBuf));
	User::LeaveIfError(CallSessionFunction(ESetSdpKeyStream, TIpcArgs(&ptr)));
	CleanupStack::PopAndDestroy(sdpBuf);
	}

EXPORT_C void RStaClient::SetSdpDocumentL(const CSdpDocument& aSdpDoc) const
/**
 	Sends an SDP document object data to the reference agent server.
 	@param aKey An SDP object data.
 */
	{
	HBufC8* sdpDocBuf(0);
	TPtr8 ptr(EncodeLC(aSdpDoc, sdpDocBuf));
	User::LeaveIfError(CallSessionFunction(ESetSdpDocument, TIpcArgs(&ptr)));
	CleanupStack::PopAndDestroy(sdpDocBuf);
	}

EXPORT_C void RStaClient::SendIpSecAssociationL(TInt32 aSpi, HBufC8* aEncryptionKey, HBufC8* aAuthenticationKey) const
	{
	CBufFlat* buf = CBufFlat::NewL(KExpandSize);
	CleanupStack::PushL(buf);
	
	RBufWriteStream stream(*buf);
	CleanupClosePushL(stream);
	
	stream.WriteInt32L(aSpi);
	stream << *aEncryptionKey;
	stream << *aAuthenticationKey;
	
	HBufC8* des = HBufC8::NewL(buf->Size());
	TPtr8 ptr(des->Des());
	buf->Read(0, ptr, buf->Size());
	
	CleanupStack::PopAndDestroy(2, buf);
	CleanupStack::PushL(des);	
	
	User::LeaveIfError(CallSessionFunction(ESetIpSecAssoc, TIpcArgs(&ptr)));
	CleanupStack::PopAndDestroy(des);
	}
