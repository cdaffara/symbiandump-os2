/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Contains the source for the implmentation of a part of getaddrinfo API
 *     
 *
*/


#include <netdb.h>
#include "netdb_r.h"
#include "getaddrinfo_private.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SYMBIAN32__
int getaddrinfo_private(const char* aHostName, const struct addrinfo* aHints, struct addrinfo** aRes)
	{
	return _getaddrinfo_r(&errno, aHostName, aHints, aRes);
	}

void freeaddrinfo_private(struct addrinfo* aInfo)
	{
	_freeaddrinfo_r(aInfo);
	}

#endif
#ifdef __cplusplus
}
#endif

