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


#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "thwtokentypeclient.h"

const TInt KHardwareImplementationId = 0x101FFFF4;

static CCTTokenType* NewFunctionL();

CCTTokenType* NewFunctionL()
	{
	CCTTokenType* hardwareKeyStore = CHardwareTokenTypeClient::NewL();
	return hardwareKeyStore;
	}

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KHardwareImplementationId, NewFunctionL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return (ImplementationTable);
	}
