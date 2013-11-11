/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ups/promptrequest.h>

using namespace UserPromptService;

EXPORT_C CPromptRequest* CPromptRequest::NewL(
		const TSecureId& aClientSid, const TVendorId& aClientVid, const TThreadId& aClientThreadId, TProcessId &aClientProcessId,
		const TSecureId& aServerSid, const TServiceId& aServiceId, RBuf& aDestination, RBuf8& aOpaqueData, 
		TBool aSecurityResult)
/**
Creates a new prompt request object.\n

The ownership of the aDestination and aOpaqueData RBufs is transfered
to this class. The callers RBufs will be empty after this call.


@param aClientSid		The SID of the client process requesting the service.
@param aClientVid		The VID of the client process requesting the service.
@param aClientThreadId	The id of the thread within the client process requesting the service.
@param aServerSid		The SID of the system server that provides the service.
@param aServiceId		The UID of the requested service.
@param aDestination		The data that the service acts on E.g. A phone number, IAP, domain name
@param aOpaqueData		Opaque data structure populated by the system server. Typically, this 
						contains extra information about the requested service.	
@param aSecurityResult	ETrue if client passed system server's security check; otherwise, EFalse.										

@return					A pointer to the new prompt request object.
*/
	{
	CPromptRequest* self = CPromptRequest::NewLC(
		aClientSid, aClientVid, aClientThreadId, aClientProcessId, 
		aServerSid, aServiceId, aDestination, aOpaqueData, aSecurityResult);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CPromptRequest* CPromptRequest::NewLC(
		const TSecureId& aClientSid, const TVendorId& aClientVid, const TThreadId& aClientThreadId, TProcessId &aClientProcessId,
		const TSecureId& aServerSid, const TServiceId& aServiceId, RBuf& aDestination, RBuf8& aOpaqueData, 
		TBool aSecurityResult)
/**
Creates a new prompt request object and places the pointer on the cleanup stack.\n

The ownership of the aDestination and aOpaqueData RBufs is transfered
to this class. The callers RBufs will be empty after this call.

@param aClientSid		The SID of the client process requesting the service.
@param aClientVid		The VID of the client process requesting the service.
@param aClientThreadId	The id of the thread within the client process requesting the service.
@param aServerSid		The SID of the system server that provides the service.
@param aServiceId		The UID of the requested service.
@param aDestination		The data that the service acts on E.g. A phone number, IAP, domain name
@param aOpaqueData		Opaque data structure populated by the system server. Typically, this 
						contains extra information about the requested service.	
@param aSecurityResult	ETrue if client passed system server's security check; otherwise, EFalse.

@return					A pointer to the new prompt request object.
*/
	{
	CPromptRequest* self = new(ELeave) 
		CPromptRequest(aClientSid, aClientVid, aClientThreadId, aClientProcessId,
		aServerSid, aServiceId, aDestination, aOpaqueData, aSecurityResult);
	CleanupStack::PushL(self);
	return self;
	}
	
CPromptRequest::CPromptRequest(
	const TSecureId& aClientSid, const TVendorId& aClientVid, const TThreadId& aClientThreadId, TProcessId &aClientProcessId,
	const TSecureId& aServerSid, const TServiceId& aServiceId, 
	RBuf& aDestination, RBuf8& aOpaqueData, 
	TBool aSecurityResult)
/**
Constructor
@param aClientSid			The client's SID
@param aClientVid			The client's VID
@param aClientThreadId		The client's thread id.
@param aServerId			The system server's SID.
@param aServiceId			The id of the service.
@param aDestination			The destination.
@param aOpaqueData			The opaque data.
@param aSecurityResult		Whether the client passed the system server's security check
*/
:	iClientSid(aClientSid), iClientVid(aClientVid), iClientThreadId(aClientThreadId), iClientProcessId(aClientProcessId),
	iServerSid(aServerSid), iServiceId(aServiceId),
	iSecurityResult(aSecurityResult)
	{
	iDestination.Close();
	iDestination.Swap(aDestination);
	iOpaqueData.Close();
	iOpaqueData.Swap(aOpaqueData);	
	}
			
EXPORT_C const TSecureId& CPromptRequest::ClientSid() const
/**
Gets the SID of the client process requesting the service.
@return The client's SID.
*/
	{	
	return iClientSid;
	}

EXPORT_C TBool CPromptRequest::IsClientSidProtected() const
/**
Determines whether the SID of the client process is in the protected range. 
If the SID is not protected then it is likely that identify of the client process
cannot reliable verified because it may an unsigned application. If so, a warning
should be displayed in the prompt dialog.

@return ETrue if the SID is in the protected range; otherwise EFalse is returned.
*/
	{
	return ! (iClientSid.iId & 0x80000000);
	}
	
EXPORT_C const TVendorId& CPromptRequest::ClientVid() const
/**
Gets the VID of the client process requesting the service.
@return The client's VID.
*/
	{
	return iClientVid;
	}
	
EXPORT_C const TThreadId& CPromptRequest::ClientThreadId() const
/**
Gets the TThreadId of the client which issued the request to the system server.
This thread ID may be invalid if the thread has exited (we do NOT hold an open RThread to keep around).
@return The client's thread id.
*/
	{
	return iClientThreadId;
	}	

EXPORT_C const TProcessId& CPromptRequest::ClientProcessId() const
/**
Gets the TProcessId of the client which issued the request to the system server.
This process ID may be invalid if the process has exited (we do NOT hold an open RProcess to keep around).
@return The client's process id.
*/
	{
	return iClientProcessId;
	}	

EXPORT_C const TSecureId& CPromptRequest::ServerSid() const
/**
Gets the SID of the system server that provides the service.
@return The system server's SID.
*/
	{
	return iServerSid;
	}
	
EXPORT_C const TServiceId& CPromptRequest::ServiceId() const
/**
Gets the UID of the requested service.
@return The service UID.
*/
	{
	return iServiceId;
	}
	
EXPORT_C const TDesC& CPromptRequest::Destination() const
/**
Gets the data that the service acts on e.g. the phone number to dial.
@return A reference to a descriptor containing the destination string.
*/
	{
	return iDestination;
	}

EXPORT_C const TDesC8& CPromptRequest::OpaqueData() const
/**
Gets the opaque data structure provided by the system server. 
@return A reference to a descriptor containing the opaque data.
*/
	{
	return iOpaqueData;
	}	

EXPORT_C TBool CPromptRequest::SecurityResult() const
/**
Whether the client processed the system server's security check.
@return ETrue, if the client process passed the system server's 
security check; otherwise, EFalse is returned.
*/
	{
	return iSecurityResult;
	}
	
CPromptRequest::~CPromptRequest()
/**
Destructor
*/
	{
	iDestination.Close();
	iOpaqueData.Close();
	}
