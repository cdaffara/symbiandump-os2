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
 @publishedAll
 @released
*/

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <sys/types1.h>
#endif

# include <stddef.h>	
# include <machine/types.h>

# ifndef	_POSIX_SOURCE
#  define	physadr		physadr_t
#  define	quad		quad_t

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;		/* System V compatibility */
typedef	unsigned int	uint;		/* System V compatibility */
# endif	/*!_POSIX_SOURCE */

#ifndef __time_t_defined
typedef _TIME_T_ time_t;
#define __time_t_defined
#endif

typedef	long	daddr_t;
typedef	char *	caddr_t;

#ifdef __go32_types__
typedef	unsigned long	ino_t;
#else
#ifdef __sparc__
typedef	unsigned long	ino_t;
#else
typedef	unsigned short	ino_t;
#endif
#endif

typedef	short	dev_t;

typedef	long	off_t;

typedef	unsigned short	uid_t;
typedef	unsigned short	gid_t;
typedef int pid_t;
typedef	long	key_t;
typedef int mode_t;

typedef unsigned int nlink_t;

# ifndef	_POSIX_SOURCE
#  define	NBBY	8		/* number of bits in a byte */
/**
Select uses bit masks of file descriptors in longs.
These macros manipulate such bit fields (the filesystem macros use chars).
FD_SETSIZE may be defined by the user, but the default here
should be >= NOFILE (param.h).
*/
#  ifndef	FD_SETSIZE
#	define	FD_SETSIZE	60
#  endif
typedef	long	fd_mask;

#  define	NFDBITS	(sizeof (fd_mask) * NBBY)	/* bits per mask */
#  ifndef	howmany

#	define	howmany(x,y)	(((x)+((y)-1))/(y))
#  endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

# endif	/* _POSIX_SOURCE */
#undef __go32_types__
#endif	/* _SYS_TYPES_H */
