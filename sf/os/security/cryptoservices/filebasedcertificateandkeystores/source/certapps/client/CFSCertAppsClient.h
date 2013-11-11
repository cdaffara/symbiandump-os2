/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CFSCertAppsClient class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CFSCERTAPPSCLIENT_H__
#define __CFSCERTAPPSCLIENT_H__

#include <mctcertapps.h>
#include "CFSClient.h"
#include "mctcertappinterface.h"

/**
 * Implementation of MCTCertApps interface, with inheritance from CFSClient.
 * Created by CClientInterfaceFactory. Marshals client data for server requests
 * and unpacks returned parameter data across IPC boundary.
 */
NONSHARABLE_CLASS(CFSCertAppsClient) : public CFSClient, public MCTCertApps
	{
public:
	static MCTTokenInterface* NewInterfaceL(MCTToken& aToken, RFileStoreClientSession& aClient);
	virtual ~CFSCertAppsClient();

	// From MCTTokenInterface
	virtual MCTToken& Token();
	virtual void DoRelease();

	// From MCTCertApps
	virtual void AddL(const TCertificateAppInfo& aClient);
	virtual void RemoveL(const TUid& aUid);
	virtual TInt ApplicationCountL() const;
	virtual void ApplicationsL(RArray<TCertificateAppInfo>& aAppArray) const;
	virtual void ApplicationL(const TUid& aUid, TCertificateAppInfo& aInfo) const;

private:	
	CFSCertAppsClient(TInt aUID, MCTToken& aToken, RFileStoreClientSession& aClient);

	// From CActive
	virtual void RunL();

	void DoApplicationsL(RArray<TCertificateAppInfo>& aAppArray) const;
	};

#endif	//	__CFSCertAppsCLIENT_H__
