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
* Description:
*
*/

#ifndef	_NETINET_TYPES_H
#define	_NETINET_TYPES_H

/**
 @file
 @publishedAll
 @released
*/

// This file defines various typedefs needed by header files in 'netinet' directory. 
#include <sys/types.h>

typedef unsigned short 	u_int16_t;
typedef unsigned int 	u_int32_t;

typedef u_int16_t n_short;
typedef u_int32_t n_long;
typedef u_int32_t n_time;

struct ether_addr
	{
   	u_char  ether_addr_octet[6];
	};

#endif	/* _NETINET_TYPES_H */
