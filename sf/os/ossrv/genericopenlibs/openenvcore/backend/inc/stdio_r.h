/* STDIO_R.H
 * 
 */

/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Based on @(#)stdio.h	5.3 (Berkeley) 3/15/86
 */
/* Portions Copyright (c) 1990-2007 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. */
/* @file
@internalComponent
*/

#ifndef _STDIO_R_H_
#define	_STDIO_R_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "reent.h"

#define _stdin_r(x)	(&((x)->_sf[0]))
#define _stdout_r(x)	(&((x)->_sf[1]))
#define _stderr_r(x)	(&((x)->_sf[2]))

/*
Reentrant versions of the <stdio.h> functions
*/
void	_cleanup_r	(struct _reent *);
FILE *	_fdopen_r	(struct _reent *, int, const char *);
FILE *	_wfdopen_r	(struct _reent *, int, const wchar_t *);
FILE *	_fopen_r	(struct _reent *, const char *, const char *);
FILE *	_wfopen_r	(struct _reent *, const wchar_t *, const wchar_t *);
int	_getchar_r	(struct _reent *);
char *	_gets_r		(struct _reent *, char *);
int	_iprintf_r	(struct _reent *, const char *, ...);
int	_mkstemp_r	(struct _reent *, char *);
char *	_mktemp_r	(struct _reent *, char *);
void	_perror_r	(struct _reent *, const char *);
int	_printf_r	(struct _reent *, const char *, ...);
int	_putchar_r	(struct _reent *, int);
int	_puts_r		(struct _reent *, const char *);
int	_scanf_r	(struct _reent *, const char *, ...);
int	_sprintf_r	(struct _reent *, char *, const char *, ...);
char *	_tempnam_r	(struct _reent *, char *, char *);
FILE *	_tmpfile_r	(struct _reent *);
char *	_tmpnam_r	(struct _reent *, char *);
wchar_t *	_wtmpnam_r	(struct _reent *, wchar_t *);
int	_vfprintf_r	(struct _reent *, FILE *, const char *, __e32_va_list);


#ifdef __cplusplus
}
#endif
#endif /* _STDIO_H_ */
