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
*
*/


#include "CFSCertAppsClient.h"
#include "fsmarshaller.h"
#include "clientutils.h"
#include "clientsession.h"
#include <certificateapps.h>

const TInt KDefaultBufferSize = 256;

MCTTokenInterface* CFSCertAppsClient::NewInterfaceL(MCTToken& aToken, 
													RFileStoreClientSession& aClient)
	{
	// Destroyed by MCTTokenInterface::DoRelease() (no refcounting)
	return new (ELeave) CFSCertAppsClient(KInterfaceCertApps, aToken, aClient);
	}

CFSCertAppsClient::CFSCertAppsClient(TInt aUID,
									 MCTToken& aToken, 
									 RFileStoreClientSession& aClient) :
	CFSClient(aUID, aToken, aClient)	
	{
	}

CFSCertAppsClient::~CFSCertAppsClient()
	{
	}

void CFSCertAppsClient::DoRelease()
	{
	MCTTokenInterface::DoRelease();
	}

MCTToken& CFSCertAppsClient::Token()
	{
	return iToken;
	}

void CFSCertAppsClient::RunL()
	{
	// RunL should never get called
	__ASSERT_DEBUG(EFalse, FSTokenPanic(EInvalidRequest));
	}

void CFSCertAppsClient::AddL(const TCertificateAppInfo& aClient)
	{
	// Package up the certificate app info into a buffer. It is 
	// then sent as the second parameter (index 1)
	TPckgC<TCertificateAppInfo> pckg(aClient);
	User::LeaveIfError(iClient.SendRequest(EAddApp, TIpcArgs(0, &pckg)));
	}

void CFSCertAppsClient::RemoveL(const TUid& aUid)
	{
	// Package up Uid
	TPckgC<TUid> pckg(aUid);
	User::LeaveIfError(iClient.SendRequest(ERemoveApp, TIpcArgs(0, &pckg)));
	}

TInt CFSCertAppsClient::ApplicationCountL() const
	{
	// Parameter is a package of TInt which is returned from the server
	TInt appCount = 0;
	TPckg<TInt> pckg(appCount);
	User::LeaveIfError(iClient.SendRequest(EGetAppCount, TIpcArgs(0, &pckg)));
	return appCount;
	}

void CFSCertAppsClient::ApplicationsL(RArray<TCertificateAppInfo>& aAppArray) const
	{
	// This message contains the following parameters:
	// Param2: [OUT] TDes8 - The buffer to write into; if buffer size too
	//               small then will return KErrOverflow with param 2 being 
	//               required size

	TRAPD(err, DoApplicationsL(aAppArray));
	FreeRequestBuffer();
	User::LeaveIfError(err);
	}

void CFSCertAppsClient::DoApplicationsL(RArray<TCertificateAppInfo>& aAppArray) const
	{
	TIpcArgs args(0, 0, &iRequestPtr);
	SendSyncRequestAndHandleOverflowL(EGetApps, KDefaultBufferSize, args);
	TokenDataMarshaller::ReadL(iRequestPtr, aAppArray);
	}

void CFSCertAppsClient::ApplicationL(const TUid& aUid, TCertificateAppInfo& aInfo) const
	{
	// The parameters for the ApplicationL function are as follows:
	// Param1: [IN] TUid - The Uid of the app to retrieve
	// Param2: [OUT] TCertificateAppInfo - The app info returned

	// Package everything up and ship them
	TPckgC<TUid> pckgUid(aUid);
	TPckg<TCertificateAppInfo> pckgInfo(aInfo);

	TIpcArgs args(0, &pckgUid, &pckgInfo);
	User::LeaveIfError(iClient.SendRequest(EGetApplication, args));
	}
