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
*
*/


#include "clientfactories.h"
#include "cfskeystoreclient.h"

#include "CFSCertStoreClient.h"
#include "CFSCertAppsClient.h"

#include <mctkeystoreuids.h>



/*static*/ MCTTokenInterface* CClientInterfaceFactory::ClientInterfaceL(TInt32 aUid, MCTToken& aToken, RFileStoreClientSession& aClient)
{
	MCTTokenInterface* tokenInterface = NULL;	
	
	switch(aUid)
	{
	case KInterfaceKeyStore:
		tokenInterface = CFSKeyStoreClient::NewKeyStoreUserInterfaceL(aToken, aClient);
		break;
	
	case KInterfaceKeyStoreManager:
		tokenInterface = CFSKeyStoreClient::NewKeyStoreManagerInterfaceL(aToken, aClient);
		break;

	case KInterfaceWritableCertStore:
		tokenInterface = CFSCertStoreClient::NewWritableInterfaceL(aToken, aClient);
		break;
	
	case KInterfaceCertStore:
		tokenInterface = CFSCertStoreClient::NewReadableInterfaceL(aToken, aClient);
		break;

	case KInterfaceCertApps:
		tokenInterface = CFSCertAppsClient::NewInterfaceL(aToken, aClient);
		break;

	default:
		User::Leave(KErrArgument);
		break;
	}

	return (tokenInterface);
}
