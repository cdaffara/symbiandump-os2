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
*
*/


/**
 @file
 @publishedPartner
 @released 
*/

#ifndef PROMPTREQUEST_H
#define PROMPTREQUEST_H

#include <e32base.h>
#include <e32cmn.h>
#include <e32std.h>

#include <ups/upstypes.h>

namespace UserPromptService
	{

	/** 
	 The data provided by the system server to the User Prompt Service in an Authorise request.
	 */
	NONSHARABLE_CLASS(CPromptRequest) : public CBase
		{
	public:				
		IMPORT_C static CPromptRequest* NewL(
			const TSecureId& aClientSid, const TVendorId& aClientVid, const TThreadId& aClientThreadId, TProcessId &aClientProcessId,
			const TSecureId& aServerSid, const TServiceId& aServiceId, RBuf& aDestination, RBuf8& aOpaqueData, 
			TBool aSecurityResult);

		IMPORT_C static CPromptRequest* NewLC(
			const TSecureId& aClientSid, const TVendorId& aClientVid, const TThreadId& aClientThreadId, TProcessId &aClientProcessId,
			const TSecureId& aServerSid, const TServiceId& aServiceId, RBuf& aDestination, RBuf8& aOpaqueData, 
			TBool aSecurityResult);

		IMPORT_C const TSecureId& ClientSid() const;
		IMPORT_C TBool IsClientSidProtected() const;		
		IMPORT_C const TVendorId& ClientVid() const;
		IMPORT_C const TSecureId& ServerSid() const;
		IMPORT_C const TServiceId& ServiceId() const;
		IMPORT_C const TDesC& Destination() const;
		IMPORT_C const TDesC8& OpaqueData() const;
		IMPORT_C const TThreadId& ClientThreadId() const;
		IMPORT_C const TProcessId& ClientProcessId() const;
		IMPORT_C TBool SecurityResult() const;

		~CPromptRequest();
	private:
	
	CPromptRequest(	const TSecureId& aClientSid, const TVendorId& aClientVid, const TThreadId& aClientThreadId, TProcessId &aClientProcessId,
		const TSecureId& aServerSid, const TServiceId& aServiceId, 
		RBuf& aDestination, RBuf8& aOpaqueData, 
		TBool aSecurityResult);				

		TSecureId iClientSid;		///< The secure id of the client process
		TVendorId iClientVid;		///< The vendor id of the client process
		TThreadId iClientThreadId;	///< The thread id of the client requesting the service
		TProcessId iClientProcessId; ///< The process id of the client requesting the service
		
		TSecureId iServerSid;		///< The secure id of the system server
		TServiceId iServiceId;		///< The ID of the server requested
		
		RBuf iDestination;			///< The destination e.g. the Internet Access Point
		RBuf8 iOpaqueData;			///< Opaque data 
		TBool iSecurityResult;		/// Whether the client pased the system server's security check.
		};
	}	

#endif // PROMPTREQUEST_H

