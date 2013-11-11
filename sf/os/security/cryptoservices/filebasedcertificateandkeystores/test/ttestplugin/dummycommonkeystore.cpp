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


#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "cdummytokentypeclient.h"

#ifdef MANAGER_INTERFACE_ONE
const TInt KDummyImplementationId = 0x101FAAA2;
#else
#ifdef KEYSTORE_INTERFACE
const TInt KDummyImplementationId = 0x101FAAA4;
#else 
#ifdef MANAGER_INTERFACE_TWO
const TInt KDummyImplementationId = 0x101FAAA6;
#endif
#endif
#endif

//	Create a client session on the tokentype server
static CCTTokenType* NewFunctionL();

CCTTokenType* NewFunctionL()
	{
	CCTTokenType* dummyKeyStore = CDummyTokenTypeClient::NewL();
	return dummyKeyStore;
	}

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KDummyImplementationId, NewFunctionL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return (ImplementationTable);
	}
