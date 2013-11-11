/*-
 * Copyright (c) 1990, 1993

 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)stdlib.h	8.5 (Berkeley) 5/19/95
 * $FreeBSD: src/include/stdlib.h,v 1.57 2005/01/09 03:55:12 tjr Exp $
 * Portions Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).All rights reserved.
 */

#ifndef _STDLIB_H_
#define	_STDLIB_H_

//---
#ifdef __cplusplus
#include <e32def.h>

extern "C" {
#endif

//---
#include <sys/cdefs.h>
#include <sys/_null.h>
#include <sys/_types.h>

#include <_ansi.h>

#ifdef __SYMBIAN32__
#ifdef __WINSCW__
#ifndef __SOFTFP
#define __SOFTFP
#endif /* __SOFTFP */
#endif//__WINSCW__
#endif//__SYMBIAN32__

#if __BSD_VISIBLE
#ifndef _RUNE_T_DECLARED
typedef	__rune_t	rune_t;
#define	_RUNE_T_DECLARED
#endif
#endif

#ifndef _SIZE_T_DECLARED
typedef	__size_t	size_t;
#define	_SIZE_T_DECLARED
#endif

#ifndef __SYMBIAN32__
#ifndef	__cplusplus
#ifndef _WCHAR_T_DECLARED
typedef	__wchar_t	wchar_t;
#define	_WCHAR_T_DECLARED
#endif //_WCHAR_T_DECLARED
#endif //__cplusplus
#else
//C++ built-in wchar data type which doesn't clash with the definition of wchar
#if !defined _WCHAR_T_DECLARED && defined __cplusplus && defined __WINSCW__
#if __option(wchar_type)
#define	_WCHAR_T_DECLARED
#endif //__option (wchar_type)
#endif //__WINSCW__

#if !defined _WCHAR_T_DECLARED && !defined __wchar_t_defined
#if defined __cplusplus
#if defined __WINSCW__
typedef unsigned short int wchar_t;
#define	_WCHAR_T_DECLARED
#endif // __WINSCW__
#else
typedef unsigned short int wchar_t;
#define	_WCHAR_T_DECLARED
#endif // __cplusplus 
#endif // __wchar_t_defined
#endif //__SYMBIAN32__

typedef struct {
	int	quot;		/* quotient */
	int	rem;		/* remainder */
} div_t;

typedef struct {
	long	quot;
	long	rem;
} ldiv_t;

#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0

#define	RAND_MAX	0x7fffffff

#ifndef __SYMBIAN32__
extern int __mb_cur_max;
#define	MB_CUR_MAX	__mb_cur_max
#else
#define	MB_CUR_MAX 4
#endif

//for armv5
#define inline __inline

__BEGIN_DECLS
IMPORT_C void	 abort(void) __dead2;
IMPORT_C int	 abs(int) __pure2;
IMPORT_C int	 atexit(void (*)(void));
IMPORT_C double	 atof(const char *) __SOFTFP;
IMPORT_C int	 atoi(const char *);
IMPORT_C long	 atol(const char *);
IMPORT_C void	*bsearch(const void *, const void *, size_t,
	    size_t, int (*)(const void *, const void *));
IMPORT_C void	*calloc(size_t, size_t);
IMPORT_C div_t	 div(int, int) __pure2;
IMPORT_C void	 exit(int) __dead2;
IMPORT_C void	 free(void *);
IMPORT_C char	*getenv(const char *);
IMPORT_C long	 labs(long) __pure2;
IMPORT_C ldiv_t	 ldiv(long, long) __pure2;
IMPORT_C void	*malloc(size_t);
IMPORT_C int	 mblen(const char *, size_t);
IMPORT_C size_t	 mbstowcs(wchar_t * __restrict , const char * __restrict, size_t);
IMPORT_C int	 mbtowc(wchar_t * __restrict, const char * __restrict, size_t);
IMPORT_C void	 qsort(void *, size_t, size_t,
	    int (*)(const void *, const void *));
IMPORT_C int	 rand(void);
IMPORT_C void	*realloc(void *, size_t);
IMPORT_C void	 srand(unsigned);
IMPORT_C double	 strtod(const char * __restrict, char ** __restrict) __SOFTFP;

IMPORT_C 
float	 strtof(const char * __restrict, char ** __restrict) __SOFTFP;

IMPORT_C long	 strtol(const char * __restrict, char ** __restrict, int);

IMPORT_C 
long double
	 strtold(const char * __restrict, char ** __restrict) __SOFTFP;

IMPORT_C
unsigned long
	 strtoul(const char * __restrict, char ** __restrict, int);

IMPORT_C int	 system(const char *);
IMPORT_C int	 wctomb(char *, wchar_t);
IMPORT_C size_t	 wcstombs(char * __restrict, const wchar_t * __restrict, size_t);

/*
 * Functions added in C99 which we make conditionally available in the
 * BSD^C89 namespace if the compiler supports `long long'.
 * The #if test is more complicated than it ought to be because
 * __BSD_VISIBLE implies __ISO_C_VISIBLE == 1999 *even if* `long long'
 * is not supported in the compilation environment (which therefore means
 * that it can't really be ISO C99).
 *
 * (The only other extension made by C99 in thie header is _Exit().)
 */
#if __ISO_C_VISIBLE >= 1999
#ifdef __LONG_LONG_SUPPORTED
/* LONGLONG */
typedef struct {
	long long quot;
	long long rem;
} lldiv_t;

/* LONGLONG */
IMPORT_C
long long
	 atoll(const char *);

/* LONGLONG */
IMPORT_C
long long
	 llabs(long long) __pure2;

/* LONGLONG */
IMPORT_C lldiv_t	 lldiv(long long, long long) __pure2;

/* LONGLONG */
IMPORT_C 
long long
	 strtoll(const char * __restrict, char ** __restrict, int);

/* LONGLONG */
IMPORT_C 
unsigned long long
	 strtoull(const char * __restrict, char ** __restrict, int);
#endif /* __LONG_LONG_SUPPORTED */

IMPORT_C void	 _Exit(int) __dead2;
#endif /* __ISO_C_VISIBLE >= 1999 */

/*
 * Extensions made by POSIX relative to C.  We don't know yet which edition
 * of POSIX made these extensions, so assume they've always been there until
 * research can be done.
 */
#if __POSIX_VISIBLE /* >= ??? */
IMPORT_C int	 setenv(const char *, const char *, int);
IMPORT_C void	 unsetenv(const char *);
#endif

/*
 * The only changes to the XSI namespace in revision 6 were the deletion
 * of the ttyslot() and valloc() functions, which FreeBSD never declared
 * in this header.  For revision 7, ecvt(), fcvt(), and gcvt(), which
 * FreeBSD also does not have, and mktemp(), are to be deleted.
 */
#if __XSI_VISIBLE
/* XXX XSI requires pollution from <sys/wait.h> here.  We'd rather not. */
/* long	 a64l(const char *); */
#ifndef _MKSTEMP_DECLARED
IMPORT_C int	 mkstemp(char *);

#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
#define mkstemp64 mkstemp
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */

#define	_MKSTEMP_DECLARED
#endif
IMPORT_C int	 putenv(const char *);
IMPORT_C long	 random(void);
IMPORT_C void	 srandom(unsigned long);
IMPORT_C char	*realpath(const char *, char resolved_path[]);
IMPORT_C char	*setstate(/* const */ char *);
IMPORT_C char	*initstate(unsigned long /* XSI requires u_int */, char *, long);
#ifndef _SETKEY_DECLARED
int	 setkey(const char *);
#define	_SETKEY_DECLARED
#endif
#endif /* __XSI_VISIBLE */

#if __BSD_VISIBLE
extern const char *_malloc_options;
extern void (*_malloc_message)(const char *, const char *, const char *,
	    const char *);


__uint32_t
	 arc4random(void);
IMPORT_C const char *	 getprogname(void);
IMPORT_C void    *reallocf(void *, size_t);
IMPORT_C void	 setprogname(const char *);

#ifdef __SYMBIAN_COMPILE_UNUSED__
void	 sranddev(void);
void	 srandomdev(void);
#endif
/* Deprecated interfaces, to be removed in FreeBSD 6.0. */
IMPORT_C
__int64_t
	 strtoq(const char *, char **, int);
IMPORT_C
__uint64_t
	 strtouq(const char *, char **, int);

extern char *suboptarg;			/* getsubopt(3) external variable */
#endif /* __BSD_VISIBLE */

#ifdef __SYMBIAN32__ /* For libcrypt library */
#ifndef _SETKEY_DECLARED
void setkey(const char *key);
#endif
#endif
__END_DECLS

//---
#ifdef __cplusplus
}
#endif
//---

#endif /* !_STDLIB_H_ */
