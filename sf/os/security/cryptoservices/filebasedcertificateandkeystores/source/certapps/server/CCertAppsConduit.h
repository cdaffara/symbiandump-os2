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
* CCertAppsConduit class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CCERTAPPSCONDUIT_H__
#define __CCERTAPPSCONDUIT_H__

#include <e32std.h>
#include <e32base.h>

// forward declarations
class CFSCertAppsServer;

/**
 * Unmarshalls incoming client messages, calls the cert apps server and
 * marshalls the results back to the client again.
 */
class CCertAppsConduit : public CBase
	{
public:
	static CCertAppsConduit* NewL(CFSCertAppsServer& aServer);

	// Function to handle a CertApp request
	void ServiceCertAppsRequestL(const RMessage2& aMessage);

public:
	~CCertAppsConduit();

private:
	// Message handler functions called by service method - these unmarshall the
	// parameters and call the real server
	void AddL(const RMessage2& aMessage) const;
	void RemoveL(const RMessage2& aMessage) const;
	void ApplicationCountL(const RMessage2& aMessage) const;
	void ApplicationsL(const RMessage2& aMessage) const;
	void ApplicationL(const RMessage2& aMessage) const;

private:
	CCertAppsConduit(CFSCertAppsServer& aServer);
	void ConstructL();
private:
	CFSCertAppsServer& iServer;
	};

#endif
