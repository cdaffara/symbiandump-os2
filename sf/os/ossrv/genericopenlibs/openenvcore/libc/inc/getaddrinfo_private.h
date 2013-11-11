/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef _GETADDRINFO_PRIVATE_H_
#define	 _GETADDRINFO_PRIVATE_H_

#include <netdb.h>

#ifdef __cplusplus
extern "C" {
#endif

int getaddrinfo_private(const char* aHostName, const struct addrinfo* aHints, struct addrinfo** aRes);

void freeaddrinfo_private(struct addrinfo* aInfo);

#ifdef __cplusplus
}
#endif

	
#endif //_GETADDRINFO_PRIVATE_H_


