/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef __OCSPSUPPORT_H__
#define __OCSPSUPPORT_H__

#include <e32std.h>
// file duplicated from \\swi\\inc
namespace Swi
{

/// The name of the OCSP Support Server
_LIT(KOcspSupportServerName,"!OCSPSUPPORTSERVER");

/// The name of the OCSP Support Executable
_LIT(KOcspSupportServerExeName,"OCSPSUPPORT");

/// The name of the OCSP Support Thread
_LIT(KOcspSupportServerThreadName,"OCSPSUPPORTSERVER-THREAD");

/// The current version of the OCSP Support Server
const TInt KOcspSupportServerVersionMajor=0;
const TInt KOcspSupportServerVersionMinor=1;
const TInt KOcspSupportServerVersionBuild=0;

/**
 * This class is a handle to the server, which uses HTTP to make the OCSP request
 */
class ROcspHandler : public RSessionBase
	{
	static const TInt KDefaultBufferSize;
public:
	/// Connect to the server	
	TInt Connect();
	
	/// Close the connection tothe server
	void Close();
	
	/// Cancel the OCSP request
	void CancelRequest();
	
	/**
	 * Get the response data from the OCSP request. This is only valid when SendRequest
	 * has been called, and completed with no error. Ownership is transferred to client.
	 *
	 * @param	aIapOut		The IAP that was used in this request
	 * @return	The body of the OCSP response
	 */
	TPtrC8 GetResponseL(TUint32& aIapOut) const;
	
	/**
	 * Send an OCSP request, using the server.
	 * @param	aUri		The URI of the server to send the request to.
	 * @param	aRequest	The request data to send.
	 * @param	aTimeout	The request timeout value in milliseconds
	 * @param	aIap		The IAP to use for this OCSP request
	 * @param	aStatus		Request status. On completion contains an error code.
	 */
	void SendRequest(const TDesC8& aUri, const TDesC8& aRequest, const TInt aTimeout, TUint32 aIap, TRequestStatus& aStatus);

private:
	/// Starts the server which carries out the requests
	TInt StartServer();
	
	
	mutable HBufC8* iResultData;
	};

} // namespace Swi

#endif // #ifndef __OCSPSUPPORT_H__
