/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __OCSPSUPPORT_TRANSPORT_H__
#define __OCSPSUPPORT_TRANSPORT_H__

#include <ocsp.h>
#include "ocspsupport.h"

// file duplicated from \\swi\\inc
namespace Swi
{

/**
 * This class is a transport used by OCSP. This transport uses client/server framework
 * to make the request in a separate process and marshalls the data between the processes.
 * This allows SWIS to use OCSP, which uses HTTP, which cannot be used directly from
 * the TCB.
 */
class COcspSupportTransport :  public CBase, public MOCSPTransport
	{
public:
	IMPORT_C static COcspSupportTransport* NewL(TUint32& aIap);
	IMPORT_C static COcspSupportTransport* NewLC(TUint32& aIap);

// from MOCSPTransport
	void SendRequest (const TDesC8 &aURI, const TDesC8 &aRequest, const TInt aTimeout, TRequestStatus &aStatus);
	void CancelRequest ();
	TPtrC8 GetResponse () const;
	
	~COcspSupportTransport();
private:
	COcspSupportTransport(TUint32& aIap);
	void ConstructL();
	
	/// Handle to the server, used to forward the requests.
	ROcspHandler iOcspHandler;
	
	/// Result data
	mutable HBufC8* iResultData; // needs to be mutable due to MOCSPTransport interface dfn
	
	// Selected IAP
	TUint32& iIap;
	};
}

#endif //#ifndef __OCSPSUPPORT_TRANSPORT_H__
