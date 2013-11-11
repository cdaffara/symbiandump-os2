/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file
 @internalAll
 @released
*/

#ifndef _SYS_TYPES1_H
#define _SYS_TYPES1_H

/**
@internalAll
*/
#  define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1L << ((n) % NFDBITS)))
/**
@internalAll
*/
#  define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1L << ((n) % NFDBITS)))
/**
@internalAll
*/
#  define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1L << ((n) % NFDBITS)))
/**
@internalAll
*/
#  define	FD_ZERO(p)	memset((caddr_t)(p), 0, sizeof (*(p)))


#undef __go32_types__
#endif	/* _SYS_TYPES1_H */
