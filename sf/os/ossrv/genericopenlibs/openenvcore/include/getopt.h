/*	$NetBSD: getopt.h,v 1.4 2000/07/07 10:43:54 ad Exp $	*/
/*	$FreeBSD: src/include/getopt.h,v 1.6 2004/02/24 08:09:20 ache Exp $ */

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Dieter Baron and Thomas Klausner.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
* Portions Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 */

#ifndef _GETOPT_H_
#define _GETOPT_H_

#include <sys/cdefs.h>

/*
 * GNU-like getopt_long()/getopt_long_only() with 4.4BSD optreset extension.
 * getopt() is declared here too for GNU programs.
 */
#define no_argument        0
#define required_argument  1
#define optional_argument  2

struct option {
	/* name of long option */
	const char *name;
	/*
	 * one of no_argument, required_argument, and optional_argument:
	 * whether option takes an argument
	 */
	int has_arg;
	/* if not NULL, set *flag to val when option found */
	int *flag;
	/* if flag not NULL, value to set *flag to; else return value */
	int val;
};

__BEGIN_DECLS
IMPORT_C int	getopt_long(int, char * const *, const char *,
	const struct option *, int *);
#ifndef _GETOPT_DECLARED
#define	_GETOPT_DECLARED
IMPORT_C int	 getopt(int, char * const [], const char *);

#ifndef __SYMBIAN32__
extern char *optarg;			/* getopt(3) external variables */
extern int optind, opterr, optopt;
#else
IMPORT_C int *__optopt(void);
IMPORT_C int *__opterr(void);
IMPORT_C int *__optind(void);
IMPORT_C char **__optarg(void);
#define optopt	  (*__optopt())
#define opterr	  (*__opterr())
#define optind	  (*__optind())
#define optarg	  (*__optarg())
#endif /* __SYMBIAN32__ */

#endif /* _GETOPT_DECLARED */
#ifndef _OPTRESET_DECLARED
#define	_OPTRESET_DECLARED

#ifndef __SYMBIAN32__
extern int optreset;			/* getopt(3) external variable */
#else
IMPORT_C int *__optreset(void);
#define optreset  (*__optreset())
#endif /* __SYMBIAN32__ */

#endif /*_OPTRESET_DECLARED */
__END_DECLS
 
#endif /* !_GETOPT_H_ */
