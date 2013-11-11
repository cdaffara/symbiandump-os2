/*-
 * Copyright (c)1999 Citrus Project,
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/include/wchar.h,v 1.45 2004/08/12 12:19:10 tjr Exp $
 *
 * © Portions Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 */

/*-
 * Copyright (c) 1999, 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Julian Coleman.
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
 *
 *	$NetBSD: wchar.h,v 1.8 2000/12/22 05:31:42 itojun Exp $
 */

#ifndef _WCHAR_H_
#define _WCHAR_H_

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include <sys/cdefs.h>
#include <sys/_null.h>
#include <sys/_types.h>
#include <sys/stat.h>
#include <stdapis/machine/_limits.h>
#include <_ctype.h>
#include <_ansi.h>
#include <stdio.h>
#include <sys/dirent.h>
#ifdef __SYMBIAN32__
#include <stdarg.h>
#ifdef __WINSCW__
#ifndef __SOFTFP
#define __SOFTFP
#endif /* __SOFTFP */
#endif//__WINSCW__
#endif //__SYMBIAN32__

#ifndef _MBSTATE_T_DECLARED
typedef	__mbstate_t	mbstate_t;
#define	_MBSTATE_T_DECLARED
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
#else //__SYMBIAN32__
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

#ifndef _WINT_T_DECLARED
typedef	__wint_t	wint_t;
#define	_WINT_T_DECLARED
#endif

#ifndef	WCHAR_MIN 
#define	WCHAR_MIN	(wchar_t)__INT_MIN
#define	WCHAR_MAX	(wchar_t)__INT_MAX
#endif

#ifndef WEOF
#define	WEOF 	((wint_t)-1)
#endif

struct __sFILE;
struct tm;

__BEGIN_DECLS
IMPORT_C wint_t	btowc(int);
IMPORT_C wint_t	fgetwc(struct __sFILE *);
IMPORT_C wchar_t	*
	fgetws(wchar_t * __restrict, int, struct __sFILE * __restrict);
IMPORT_C wint_t	fputwc(wchar_t, struct __sFILE *);
IMPORT_C int	fputws(const wchar_t * __restrict, struct __sFILE * __restrict);
IMPORT_C int	fwide(struct __sFILE *, int);
IMPORT_C int	fwprintf(struct __sFILE * __restrict, const wchar_t * __restrict, ...);
IMPORT_C int	fwscanf(struct __sFILE * __restrict, const wchar_t * __restrict, ...);
IMPORT_C wint_t	getwc(struct __sFILE *);
IMPORT_C wint_t	getwchar(void);
IMPORT_C size_t	mbrlen(const char * __restrict, size_t, mbstate_t * __restrict);
IMPORT_C size_t	mbrtowc(wchar_t * __restrict, const char * __restrict, size_t,
	    mbstate_t * __restrict);
IMPORT_C int	mbsinit(const mbstate_t *);
IMPORT_C size_t	mbsrtowcs(wchar_t * __restrict, const char ** __restrict, size_t,
	    mbstate_t * __restrict);
IMPORT_C wint_t	putwc(wchar_t, struct __sFILE *);
IMPORT_C wint_t	putwchar(wchar_t);
IMPORT_C int	swprintf(wchar_t * __restrict, size_t n, const wchar_t * __restrict,
	    ...);
IMPORT_C int	swscanf(const wchar_t * __restrict, const wchar_t * __restrict, ...);
IMPORT_C wint_t	ungetwc(wint_t, struct __sFILE *);
IMPORT_C int	vfwprintf(struct __sFILE * __restrict, const wchar_t * __restrict,
	    va_list);
IMPORT_C int	vswprintf(wchar_t * __restrict, size_t n, const wchar_t * __restrict,
	    va_list);
IMPORT_C int	vwprintf(const wchar_t * __restrict, va_list);
IMPORT_C size_t	wcrtomb(char * __restrict, wchar_t, mbstate_t * __restrict);
IMPORT_C wchar_t	*wcscat(wchar_t * __restrict, const wchar_t * __restrict);
IMPORT_C wchar_t	*wcschr(const wchar_t *, wchar_t) __pure;
IMPORT_C int	wcscmp(const wchar_t *, const wchar_t *) __pure;
IMPORT_C int	wcscoll(const wchar_t *, const wchar_t *);
IMPORT_C wchar_t	*wcscpy(wchar_t * __restrict, const wchar_t * __restrict);
IMPORT_C size_t	wcscspn(const wchar_t *, const wchar_t *) __pure;
IMPORT_C size_t	wcsftime(wchar_t * __restrict, size_t, const wchar_t * __restrict,
	    const struct tm * __restrict);
IMPORT_C size_t	wcslen(const wchar_t *) __pure;
IMPORT_C wchar_t	*wcsncat(wchar_t * __restrict, const wchar_t * __restrict,
	    size_t);
IMPORT_C int	wcsncmp(const wchar_t *, const wchar_t *, size_t) __pure;
IMPORT_C wchar_t	*wcsncpy(wchar_t * __restrict , const wchar_t * __restrict, size_t);
IMPORT_C wchar_t	*wcspbrk(const wchar_t *, const wchar_t *) __pure;
IMPORT_C wchar_t	*wcsrchr(const wchar_t *, wchar_t) __pure;
IMPORT_C size_t	wcsrtombs(char * __restrict, const wchar_t ** __restrict, size_t,
	    mbstate_t * __restrict);
IMPORT_C size_t	wcsspn(const wchar_t *, const wchar_t *) __pure;
IMPORT_C wchar_t	*wcsstr(const wchar_t * __restrict, const wchar_t * __restrict)
	    __pure;
IMPORT_C size_t	wcsxfrm(wchar_t * __restrict, const wchar_t * __restrict, size_t);
IMPORT_C int	wctob(wint_t);
IMPORT_C double	wcstod(const wchar_t * __restrict, wchar_t ** __restrict) __SOFTFP;
IMPORT_C wchar_t	*wcstok(wchar_t * __restrict, const wchar_t * __restrict,
	    wchar_t ** __restrict);
IMPORT_C long	 wcstol(const wchar_t * __restrict, wchar_t ** __restrict, int);
IMPORT_C unsigned long
	 wcstoul(const wchar_t * __restrict, wchar_t ** __restrict, int);
IMPORT_C wchar_t	*wmemchr(const wchar_t *, wchar_t, size_t) __pure;
IMPORT_C int	wmemcmp(const wchar_t *, const wchar_t *, size_t) __pure;
IMPORT_C wchar_t	*wmemcpy(wchar_t * __restrict, const wchar_t * __restrict, size_t);
IMPORT_C wchar_t	*wmemmove(wchar_t *, const wchar_t *, size_t);
IMPORT_C wchar_t	*wmemset(wchar_t *, wchar_t, size_t);
IMPORT_C int	wprintf(const wchar_t * __restrict, ...);
IMPORT_C int	wscanf(const wchar_t * __restrict, ...);
#ifdef __SYMBIAN32__
IMPORT_C long long 	 wcstoq(const wchar_t * __restrict, wchar_t ** __restrict, int);
IMPORT_C unsigned long long wcstouq(const wchar_t * __restrict, wchar_t ** __restrict, int);
IMPORT_C wchar_t *wcswcs(const wchar_t * __restrict s, const wchar_t * __restrict find);
IMPORT_C int wpopen3 (const wchar_t* file, const wchar_t* cmd, wchar_t** env, int fids[3]);
#endif //__SYMBIAN32__

#ifndef _STDSTREAM_DECLARED
extern struct __sFILE *__stdinp;
extern struct __sFILE *__stdoutp;
extern struct __sFILE *__stderrp;
#define	_STDSTREAM_DECLARED
#endif

#ifndef __SYMBIAN32__
#define	getwc(fp)	fgetwc(fp)
#define	getwchar()	fgetwc(__stdinp)
#define	putwc(wc, fp)	fputwc(wc, fp)
#define	putwchar(wc)	fputwc(wc, __stdoutp)
#else //__SYMBIAN32__
IMPORT_C wint_t	getwc(struct __sFILE *);
IMPORT_C wint_t	getwchar(void);
IMPORT_C wint_t	putwc(wchar_t, struct __sFILE *);
IMPORT_C wint_t	putwchar(wchar_t);
#endif //__SYMBIAN32__


#if __ISO_C_VISIBLE >= 1999
IMPORT_C int	vfwscanf(struct __sFILE * __restrict, const wchar_t * __restrict,
	    va_list);
IMPORT_C int	vswscanf(const wchar_t * __restrict, const wchar_t * __restrict,
	    va_list);
IMPORT_C int	vwscanf(const wchar_t * __restrict, va_list);
IMPORT_C float	wcstof(const wchar_t * __restrict, wchar_t ** __restrict) __SOFTFP;
IMPORT_C long double
	wcstold(const wchar_t * __restrict, wchar_t ** __restrict) __SOFTFP;
#ifdef __LONG_LONG_SUPPORTED
/* LONGLONG */
IMPORT_C long long
	wcstoll(const wchar_t * __restrict, wchar_t ** __restrict, int);
/* LONGLONG */
IMPORT_C unsigned long long
	 wcstoull(const wchar_t * __restrict, wchar_t ** __restrict, int);
#endif
#endif	/* __ISO_C_VISIBLE >= 1999 */

#if __XSI_VISIBLE
IMPORT_C int	wcswidth(const wchar_t *, size_t);
IMPORT_C int	wcwidth(wchar_t);
#ifndef __SYMBIAN32__
#define	wcwidth(_c)	__wcwidth(_c)
#endif //__SYMBIAN32__
#endif

#if __BSD_VISIBLE

#ifdef __SYMBIAN_COMPILE_UNUSED__
wchar_t	*fgetwln(struct __sFILE * __restrict, size_t * __restrict);
#endif

IMPORT_C size_t	mbsnrtowcs(wchar_t * __restrict, const char ** __restrict, size_t,
	    size_t, mbstate_t * __restrict);
IMPORT_C size_t	wcsnrtombs(char * __restrict, const wchar_t ** __restrict, size_t,
	    size_t, mbstate_t * __restrict);
#endif
#ifdef __SYMBIAN32__
IMPORT_C wchar_t *wcpcpy(wchar_t *dst, const wchar_t *src);
IMPORT_C wchar_t *wcpncpy(wchar_t * dst, const wchar_t * src, size_t n);
IMPORT_C int wcscasecmp(const wchar_t *s1, const wchar_t *s2);
IMPORT_C wchar_t *wcsdup (const wchar_t *srcwcs);
IMPORT_C int wcsncasecmp(const wchar_t *s1, const wchar_t *s2, size_t n);
IMPORT_C size_t wcsnlen(const wchar_t *s, size_t maxlen);

IMPORT_C wchar_t* wrealpath (const wchar_t* , wchar_t* );
IMPORT_C int wrmdir (const wchar_t *);
IMPORT_C int wstat (const wchar_t *name, struct stat *st);
IMPORT_C int wsystem (const wchar_t* );
IMPORT_C int wunlink (const wchar_t *);
IMPORT_C FILE* wpopen( const wchar_t* command, const wchar_t* mode );

IMPORT_C int wopen(const wchar_t *, int, ...);
IMPORT_C FILE 	*wfopen(const wchar_t * __restrict,	const wchar_t * __restrict);
IMPORT_C int wrename (const wchar_t *oldpath, const wchar_t *newpath);
IMPORT_C int wchdir (const wchar_t *_path);
IMPORT_C int wchmod (const wchar_t *_path, mode_t _mode);
IMPORT_C  wchar_t* wgetcwd (wchar_t *_buf, size_t _size);
IMPORT_C int wmkdir (const wchar_t *_path, mode_t _mode);
IMPORT_C int wclosedir (WDIR *dp);
IMPORT_C struct wdirent *wreaddir (WDIR *dp);
IMPORT_C void wrewinddir (WDIR *dp);
IMPORT_C int waccess(const wchar_t *fn, int flags);
IMPORT_C int wcreat(const wchar_t* file, mode_t mode);
IMPORT_C void wseekdir(WDIR *dp,off_t index);
IMPORT_C  off_t wtelldir(const WDIR *dp);
IMPORT_C WDIR *wopendir (const wchar_t *_path);

#define _wcsupr(wcs) 						wcsupr(wcs)
#define _wcslwr(wcs) 						wcslwr(wcs)
#define _wcsset(wcs, wc) 				wcsset(wcs,wc)
#define _wcsnset(wcs, wc, size) 	wcsnset(wcs, wc, size)
#define _wcsrev(wcs) 						wcsrev(wcs)
#define _wcsicmp(wcs1, wcs2) 		wcsicmp(wcs1, wcs2)
#define _wstrdate(datestr) 			wstrdate(datestr)
#define _wstrtime(timestr) 			wstrtime(timestr)

IMPORT_C wchar_t* wcsupr(wchar_t *wcs);
IMPORT_C wchar_t* wcslwr(wchar_t *wcs);
IMPORT_C wchar_t* wcsset (wchar_t* wcs, wchar_t wc);
IMPORT_C wchar_t* wcsnset (wchar_t* wcs, wchar_t wc, size_t maxSize);
IMPORT_C wchar_t* wcsrev(wchar_t *wcs);
IMPORT_C int wcsicmp(const wchar_t* wcs1,const wchar_t * wcs2);
IMPORT_C wchar_t* wstrdate( const wchar_t *dateStr );
IMPORT_C wchar_t* wstrtime(const wchar_t *timeStr);
IMPORT_C FILE * wfdopen (int fd, const wchar_t *mode);
IMPORT_C FILE * wfreopen(const wchar_t * file,const wchar_t * mode,FILE *fp);	
IMPORT_C wchar_t* getws(wchar_t* str);
IMPORT_C int wremove(const wchar_t *file);
IMPORT_C int putws(wchar_t* str);

struct _wfinddata_t 
{
    unsigned    attrib;
    time_t      time_create;    /* -1 for symbian */
    time_t      time_access;    /* -1 for symbian*/
    time_t      time_write;
    size_t    	size;
    wchar_t     name[260];
};

#define _A_ARCH   0x0020		//Archive. Set whenever the file is changed and cleared by the BACKUP command. Value: 0x20.
#define _A_HIDDEN 0x0002 		//Hidden file. Not normally seen with the DIR command, unless the /AH option is used. Returns information about normal files and files with this attribute. Value: 0x02.
#define _A_NORMAL 0x0000	//Normal. File has no other attributes set and can be read or written to without restriction. Value: 0x00.
#define _A_RDONLY 0x0001		//Read-only. File cannot be opened for writing and a file with the same name cannot be created. Value: 0x01.
#define _A_SYSTEM  0x0004		//System file. Not normally seen with the DIR command, unless the /A or /A:S option is used
#define _A_SUBDIR 0x0010        // Subdirectory.        

IMPORT_C size_t	wcslcat(wchar_t *, const wchar_t *, size_t);
IMPORT_C size_t	 wcslcpy(wchar_t *, const wchar_t *, size_t);
IMPORT_C wchar_t *wasctime(const struct tm *);
IMPORT_C wchar_t *wctime(const time_t *);
IMPORT_C wchar_t *wsetlocale(int, const wchar_t *);
IMPORT_C void wperror(const wchar_t *);
IMPORT_C wchar_t*	wcserror(int num);
IMPORT_C int wfindnext(intptr_t, struct _wfinddata_t *); 
IMPORT_C intptr_t wfindfirst(const wchar_t* , struct _wfinddata_t* );
IMPORT_C	int findclose( intptr_t handle);

#define snwprintf swprintf
#define vsnwprintf vswprintf

IMPORT_C int wcsnicmp (const wchar_t *wcs1, const wchar_t *wcs2, size_t n);
IMPORT_C int wcsicoll(const wchar_t *wcs1, const wchar_t *wcs2);
IMPORT_C int wcsncoll(const wchar_t* wcs1, const wchar_t* wcs2, size_t n);
IMPORT_C int wcsnicoll(const wchar_t* wcs1, const wchar_t* wcs2, size_t n);
IMPORT_C wchar_t* wtmpnam(wchar_t *s);

#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
#define	wfopen64	wfopen
#define wfreopen64	wfreopen
#define wstat64		wstat
#define wcreat64	wcreat
#define wopen64		wopen
#define wreaddir64	wreaddir
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */
	

#endif //__SYMBIAN32__
__END_DECLS

#ifdef __cplusplus
}
#endif //__cplusplus
#endif /* !_WCHAR_H_ */
