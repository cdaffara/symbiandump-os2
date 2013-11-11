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
* RSraClient implementation.	 See class and function definitions
* for more detail.
*
*/


#include "sraclient.h"

using namespace StreamAccess;

EXPORT_C RSraClient::RSraClient()
//Constructor
	: RSessionBase()
		{
		//empty
		}
		
	
EXPORT_C TInt RSraClient::Connect()
/**
	Connect to the Streaming Reference Agent server.

	@return		Symbian OS error code where KErrNone indicates
				success and any other value indicates failure.
 */
	{
	TVersion ver = SraVersion();
	TUidType fullUid = SraImageFullUid();
	TInt retries = 2;		// number of remaining retries

	for (;;)
		{
		TInt r = CreateSession(KSraName, ver);
		
		// if connected then finished
		if (r == KErrNone)
			return r;

		// if any reason other than server not available then abort
		if (r != KErrNotFound && r != KErrServerTerminated)
			return r;

		if (--retries == 0)
			return r;

		r = StartServerProcess(KSraImageName, fullUid);
		if (r != KErrNone && r != KErrAlreadyExists)
			return r;
		}	// for (;;)
	}
	

TInt RSraClient::StartServerProcess(const TDesC& aExeName, const TUidType& aFullExeUid)
/**
	This function is defined for the convenience of subclasses which need to start
	a server process before they can connect to the server.

	@param	aExeName		Executable which hosts the server.
	@param	aFullExeUid		The server executable's full UID.  This is used to ensure the
							intended executable is started, and not another executable
							with the same name.
	@return					Symbian OS error code.  KErrNone indicates success, and any other
							value indicates failure.
 */
	{
	RProcess pr;
	TInt r = pr.Create(aExeName, /* aCommand */ KNullDesC, aFullExeUid);
	if (r != KErrNone)
		return r;

	TRequestStatus rs;
	pr.Rendezvous(rs);
	if (rs != KRequestPending)
		r = rs.Int();
	else
		{
		pr.Resume();
		User::WaitForRequest(rs);
		r = (pr.ExitType() == EExitPending) ? rs.Int() : KErrGeneral;
		}

	pr.Close();
	return r;
	}


EXPORT_C void RSraClient::GetAttributeL(const TAttribute& aAttribute, TBool& aValue)
/**
 	Gets the value of a requested attribute listed in TAttribute enumeration.
 	
 	@param aAttribute The requested attribute.
 	@param aValue     The value of the requested attribute
 	
 	@see StreamAccess::TAttribute
 */
	{
	TPckg<TAttribute> pckgAttr(aAttribute);
	TPckg<TBool> pckgVal(aValue);
	User::LeaveIfError(RSessionBase::SendReceive(EGetAttribute, TIpcArgs(&pckgAttr, &pckgVal)));
	}


EXPORT_C HBufC* RSraClient::GetStringAttributeLC(const TStringAttribute& aAttribute)
/**
 	Gets the value of a requested string attribute listed in TStringAttribute enumeration.
 	
 	@param aAttribute The requested string attribute.
 	@return    A pointer to the value of the requested string attribute. 
 			   The ownership is transfered to the calling function.
 	@see StreamAccess::TStringAttribute
 */
	{
	TPckg<TStringAttribute> pckgStrAttr(aAttribute);
	TInt size = 0;
	TPckg<TInt> pckgSize(size);
	User::LeaveIfError(RSessionBase::SendReceive(EGetStringAttributeSize, TIpcArgs(&pckgStrAttr, &pckgSize)));
	
	HBufC8* value8 = HBufC8::NewLC(size);
	TPtr8 ptr8(value8->Des());
	
	User::LeaveIfError(RSessionBase::SendReceive(EGetStringAttributeData, TIpcArgs(&pckgStrAttr, &ptr8)));
	
	//Convert 8-bit to 16-bit
	HBufC* value = HBufC::NewL(ptr8.Size());
	TPtr ptr(value->Des());
	ptr.Copy(ptr8);

	CleanupStack::PopAndDestroy(value8);
	CleanupStack::PushL(value);
	return value;
	}


EXPORT_C void RSraClient::GetPostDeliveryRightsL(RFile& aFile, TDes8& aCid, TDes8& aRightsMimeType, TDes8& aContentMimeType)
/**
 	@param aFile The file handle of the post-delivery rights object obtained from 
 		    	 the streaming reference agent server. The receiving party is responsible
 		    	 to close the file handle.
 	@param aCid  The content Id.
 	@param aRightsMimeType The mime type of the post-acquisition rights object.
	@param aContentMimeType The mime type of the content.
 */
	{
	TInt fsh;			//session (RFs) handle
	TPckgBuf<TInt> fh;	//subsession (RFile) handle
	
	//The message is completed with the file-server (RFs) session handle
	//Pointer to the file subsession handle in slot 0
	fsh = RSessionBase::SendReceive(EGetPostDeliveryRights, TIpcArgs(&fh, &aCid, &aRightsMimeType, &aContentMimeType));
	User::LeaveIfError(fsh);
	
	// Adopt the file using the returned handles
	User::LeaveIfError(aFile.AdoptFromServer(fsh,fh()));
	}


EXPORT_C void RSraClient::SendKeyStreamSinkL(const CKeyStreamSink& aSink) const
/**
 	Sends a key stream sink object to the reference agent server.
 	@param aKey A key stream sink object.
 */
	{
	TPtr8 ptr(aSink.ExternalizeLC());
	User::LeaveIfError(RSessionBase::SendReceive(ESetKeyStreamSink, TIpcArgs(&ptr)));
	CleanupStack::PopAndDestroy(); // Delete the allocated buffer
	}

EXPORT_C void RSraClient::SendSdpMediaFieldL(const CSdpMediaField& aSdp) const
/**
 	Sends an SDP media field object data to the reference agent server.
 	@param aKey An SDP object data.
 */
	{
	HBufC8* sdpBuf(0);
	TPtr8 ptr(EncodeLC(aSdp, sdpBuf));
	User::LeaveIfError(RSessionBase::SendReceive(ESetSdpKeyStream, TIpcArgs(&ptr)));
	CleanupStack::PopAndDestroy(sdpBuf);
	}

EXPORT_C void RSraClient::SendSdpDocumentL(const CSdpDocument& aSdpDoc) const
/**
 	Sends an SDP document object data to the reference agent server.
 	@param aKey An SDP object data.
 */
	{
	HBufC8* sdpDocBuf(0);
	TPtr8 ptr(EncodeLC(aSdpDoc, sdpDocBuf));
	User::LeaveIfError(RSessionBase::SendReceive(ESetSdpDocument, TIpcArgs(&ptr)));
	CleanupStack::PopAndDestroy(sdpDocBuf);
	}
