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
* CCertStoreConduit class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CCERTSTORECONDUIT_H__
#define __CCERTSTORECONDUIT_H__

#include "fstokencliserv.h"
#include "fsmarshaller.h"
#include "fsdatatypes.h"
#include <ccertattributefilter.h>

class CFSCertStoreServer;

/**
 * Unmarshalls incoming client request data and uses it to execute certstore
 * operations.  Marshalls the return data into a suitable format to pass back to
 * to the client.
 */
class CCertStoreConduit : public CBase
	{
 public:
	static CCertStoreConduit* NewL(CFSCertStoreServer& aServer);
	virtual ~CCertStoreConduit();
	void ServiceCertStoreRequestL(const RMessage2& aMessage);
	
 private:
	// For MCTCertStore
	void ListCertsL(const RMessage2& aMessage);
	void GetCertL(const RMessage2& aMessage);
	void ApplicationsL(const RMessage2& aMessage);
	TInt IsApplicableL(const RMessage2& aMessage);
	TInt TrustedL(const RMessage2& aMessage);
	void RetrieveL(const RMessage2& aMessage);

	// For MCTWritableCertStore
	void AddCertL(const RMessage2& aMessage);
	void RemoveCertL(const RMessage2& aMessage);
	void SetApplicabilityL(const RMessage2& aMessage);
	void SetTrustL(const RMessage2& aMessage);

 private:
	CCertStoreConduit(CFSCertStoreServer& aServer);

	HBufC8* AllocResponseBufferLC(TInt aSize, const RMessage2& aMessage);

	CCertStoreConduit(const CCertStoreConduit&);			//	No copying
	CCertStoreConduit& operator=(const CCertStoreConduit&);	//	No copying

private:
	/// The cert store server used to fulfill client requests
	CFSCertStoreServer& iServer;
	};

#endif	//	__CCERTSTORECONDUIT_H__
