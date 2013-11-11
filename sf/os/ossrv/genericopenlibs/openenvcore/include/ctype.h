/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * This code is derived from software contributed to Berkeley by
 * Paul Borman at Krystal Technologies.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ctype.h	8.4 (Berkeley) 1/21/94
 *      $FreeBSD: src/include/ctype.h,v 1.28 2004/08/12 09:33:47 tjr Exp $
 *
 * Portions Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

 */

#ifndef _CTYPE_H_
#define	_CTYPE_H_

#include <sys/cdefs.h>
#include <sys/_types.h>
#include <_ctype.h>

#ifdef __SYMBIAN32__
#include <_ansi.h>

#ifdef __cplusplus
extern "C" {
#endif
#endif // __SYMBIAN32__

__BEGIN_DECLS
IMPORT_C int	isalnum(int);
IMPORT_C int	isalpha(int);
IMPORT_C int	iscntrl(int);
IMPORT_C int	isdigit(int);
IMPORT_C int	isgraph(int);
IMPORT_C int	islower(int);
IMPORT_C int	isprint(int);
IMPORT_C int	ispunct(int);
IMPORT_C int	isspace(int);
IMPORT_C int	isupper(int);
IMPORT_C int	isxdigit(int);
IMPORT_C int	tolower(int);
IMPORT_C int	toupper(int);

#ifdef __SYMBIAN32__
/* these two macros always expect that the argument sent is always in uppercase 
or lowercase respectively and works only with c locale.
else the behavior is undefined */
#define	_tolower(c)	((c) + 0x20)
#define	_toupper(c)	((c) - 0x20)
#define	isascii(c)	(((c) & ~0x7F) == 0)
#define	toascii(c)	((c) & 0x7F)
#endif//__SYMBIAN32__
__END_DECLS

#ifdef __SYMBIAN32__
#ifdef __cplusplus
}
#endif
#endif //__SYMBIAN32__

#endif /* !_CTYPE_H_ */
