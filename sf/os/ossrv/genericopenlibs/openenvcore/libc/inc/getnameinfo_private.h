// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// wcharcnv.h
//

#ifndef _GETNAMEINFO_PRIVATE_H_
#define	 _GETNAMEINFO_PRIVATE_H_
#include <netdb.h>
#include <sys/types.h>
/*#include <wchar.h>
#include <wctype.h>
#include <stdapis/netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>*/
#include <wchar.h>
#include <wctype.h>
#ifdef __cplusplus
extern "C" {
#endif

struct hostent* gethostaddrdetails ( const void* ,int,int,int*,const char*);

struct hostent_buf
	{
	struct hostent iHostent;
	struct sockaddr iAddr;
	char* iPtrs[2];
	char iName[1];	// and following bytes
	};
	
#ifdef __cplusplus
}
#endif

	
#endif //_GETNAMEINFO_H_


