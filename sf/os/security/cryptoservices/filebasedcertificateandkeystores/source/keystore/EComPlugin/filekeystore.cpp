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
#include "CFSTokenTypeClient.h"
#include <mctkeystore.h>


//	Create a client session on the tokentype server
class CCTTokenType;
static CCTTokenType* NewFunctionL();
CCTTokenType* NewFunctionL()
	{
	CCTTokenType* fileKeyStore = NULL;
	TUid uid;
	uid.iUid = KTokenTypeFileKeystore;	//	0x101F7333
	fileKeyStore = CFSTokenTypeClient::NewL(uid);
	return (fileKeyStore);
	}

const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KTokenTypeFileKeystore, NewFunctionL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return (ImplementationTable);
	}
