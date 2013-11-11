/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <mctcertstore.h>

class CCTTokenType;
static CCTTokenType* NewFunctionL();
CCTTokenType* NewFunctionL()
	{
	CCTTokenType* fileCertStore = NULL;
	TUid uid;
	uid.iUid = KTokenTypeFileCertstore;	//	0x101F501A
	fileCertStore = CFSTokenTypeClient::NewL(uid);
	return (fileCertStore);
	}

const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KTokenTypeFileCertstore, NewFunctionL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return (ImplementationTable);
	}



