/*-

 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
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
 *	@(#)stdio.h	8.5 (Berkeley) 4/29/95
 * $FreeBSD: src/include/stdio.h,v 1.56 2004/06/20 10:01:30 tjr Exp $
 *  Portions Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 */

#ifndef	_STDIO_H_
#define	_STDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/cdefs.h>
#include <sys/_null.h>
#include <sys/_types.h>

#ifdef __SYMBIAN32__
#include <_ansi.h>
#include <errno.h>
#include <sys/types.h>
#endif



typedef	__off_t		fpos_t;

#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
typedef	__off_t		fpos64_t;
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */

#ifndef _SIZE_T_DECLARED
typedef	__size_t	size_t;
#define	_SIZE_T_DECLARED
#endif

#ifndef __SYMBIAN32__
//#ifndef __DOXYGEN__ 
#if (__BSD_VISIBLE || __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE) && !__SYMBIAN32__
#ifndef _VA_LIST_DECLARED
typedef	__va_list	va_list;
#define va_list __e32_va_list
#define	_VA_LIST_DECLARED
#endif
#endif
//#endif //__DOXYGEN__
#else //__SYMBIAN32__
#include <stdarg_e.h>
#include <stdarg.h>
#endif //__SYMBIAN32__

#ifdef __SYMBIAN32__
#ifndef __DOXYGEN__
#define vfscanf __vfscanf
#endif //__DOXYGEN__
#ifndef _VA_COPY_DEFINED
#define va_copy(dst,src) (dst = src)
#define  _VA_COPY_DEFINED
#endif //_VA_COPY_DEFINED
#endif // __SYMBIAN32__
#define	_FSTDIO			/* Define for new stdio with functions. */

/*
 * NB: to fit things in six character monocase externals, the stdio
 * code uses the prefix `__s' for stdio objects, typically followed
 * by a three-character attempt at a mnemonic.
 */

/* stdio buffers */
struct __sbuf {
	unsigned char *_base;
	int	_size;
};

/* hold a buncha junk that would grow the ABI */
struct __sFILEX;

/*
 * stdio state variables.
 *
 * The following always hold:
 *
 *	if (_flags&(__SLBF|__SWR)) == (__SLBF|__SWR),
 *		_lbfsize is -_bf._size, else _lbfsize is 0
 *	if _flags&__SRD, _w is 0
 *	if _flags&__SWR, _r is 0
 *
 * This ensures that the getc and putc macros (or inline functions) never
 * try to write or read from a file that is in `read' or `write' mode.
 * (Moreover, they can, and do, automatically switch from read mode to
 * write mode, and back, on "r+" and "w+" files.)
 *
 * _lbfsize is used only to make the inline line-buffered output stream
 * code as compact as possible.
 *
 * _ub, _up, and _ur are used when ungetc() pushes back more characters
 * than fit in the current _bf, or when ungetc() pushes back a character
 * that does not match the previous one in _bf.  When this happens,
 * _ub._base becomes non-nil (i.e., a stream has ungetc() data iff
 * _ub._base!=NULL) and _up and _ur save the current values of _p and _r.
 */
typedef	struct __sFILE {
	unsigned char *_p;	/* current position in (some) buffer */
	int	_r;		/* read space left for getc() */
	int	_w;		/* write space left for putc() */
	short _flags;		/* flags, below; this FILE is free if 0 */
	short	_file;		/* fileno, if Unix descriptor, else -1 */
	struct	__sbuf _bf;	/* the buffer (at least 1 byte, if !NULL) */
	int	_lbfsize;	/* 0 or -_bf._size, for inline putc */

	/* operations */
	void	*_cookie;	/* cookie passed to io functions */
	int	(*_close)(void *);
	int	(*_read)(void *, char *, int);
	fpos_t	(*_seek)(void *, fpos_t, int);
	int	(*_write)(void *, const char *, int);

	/* separate buffer for long sequences of ungetc() */
	struct	__sbuf _ub;	/* ungetc buffer */
	struct __sFILEX *_extra; /* additions to FILE to not break ABI */
	int	_ur;		/* saved _r when _r is counting ungetc data */

	/* tricks to meet minimum requirements even when malloc() fails */
	unsigned char _ubuf[3];	/* guarantee an ungetc() buffer */
	unsigned char _nbuf[1];	/* guarantee a getc() buffer */

	/* separate buffer for fgetln() when line crosses buffer boundary */
	struct	__sbuf _lb;	/* buffer for fgetln() */

	/* Unix stdio files get aligned to block boundaries on fseek() */
	int	_blksize;	/* stat.st_blksize (may be != _bf._size) */
	fpos_t	_offset;	/* current lseek offset */
} FILE;

#ifndef _STDSTREAM_DECLARED
__BEGIN_DECLS
#if (!defined(__SYMBIAN32__) && (!defined(__WINSCW__) || !defined(__WINS__)))
extern FILE *__stdinp;
extern FILE *__stdoutp;
extern FILE *__stderrp;
#endif //EMULATOR
__END_DECLS
#define	_STDSTREAM_DECLARED
#endif

#define	__SLBF	0x0001		/* line buffered */
#define	__SNBF	0x0002		/* unbuffered */
#define	__SRD	0x0004		/* OK to read */
#define	__SWR	0x0008		/* OK to write */
	/* RD and WR are never simultaneously asserted */
#define	__SRW	0x0010		/* open for reading & writing */
#define	__SEOF	0x0020		/* found EOF */
#define	__SERR	0x0040		/* found error */
#define	__SMBF	0x0080		/* _buf is from malloc */
#define	__SAPP	0x0100		/* fdopen()ed in append mode */
#define	__SSTR	0x0200		/* this is an sprintf/snprintf string */
#define	__SOPT	0x0400		/* do fseek() optimization */
#define	__SNPT	0x0800		/* do not do fseek() optimization */
#define	__SOFF	0x1000		/* set iff _offset is in fact correct */
#define	__SMOD	0x2000		/* true => fgetln modified _p text */
#define	__SALC	0x4000		/* allocate string space dynamically */
#define	__SIGN	0x8000		/* ignore this file in _fwalk */

/*
 * The following three definitions are for ANSI C, which took them
 * from System V, which brilliantly took internal interface macros and
 * made them official arguments to setvbuf(), without renaming them.
 * Hence, these ugly _IOxxx names are *supposed* to appear in user code.
 *
 * Although numbered as their counterparts above, the implementation
 * does not rely on this.
 */
#define	_IOFBF	0		/* setvbuf should set fully buffered */
#define	_IOLBF	1		/* setvbuf should set line buffered */
#define	_IONBF	2		/* setvbuf should set unbuffered */

#define	BUFSIZ	1024		/* size of buffer used by setbuf */
#define	EOF	(-1)

/*
 * FOPEN_MAX is a minimum maximum, and is the number of streams that
 * stdio can provide without attempting to allocate further resources
 * (which could fail).  Do not use this for anything.
 */
				/* must be == _POSIX_STREAM_MAX <limits.h> */
#define	FOPEN_MAX	20	/* must be <= OPEN_MAX <sys/syslimits.h> */
#ifndef __SYMBIAN32__
#define	FILENAME_MAX	1024	/* must be <= PATH_MAX <sys/syslimits.h> */
#else
#define	FILENAME_MAX	256	/* must be <= PATH_MAX <sys/syslimits.h> */
#endif /* __SYMBIAN32__ */

/* System V/ANSI C; this is the wrong way to do this, do *not* use these. */
#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#define GET_WSD_VAR_NAME(var,varprefix) 	_##varprefix##_##var
char **GET_WSD_VAR_NAME(tmpdirptr, g)();
#define __tmpdirptr (*GET_WSD_VAR_NAME(tmpdirptr, g)())
#else
extern char* __tmpdirptr;
#endif 

#if __XSI_VISIBLE
#ifndef __SYMBIAN32__
#define	P_tmpdir	"/var/tmp/"
#else
#define	P_tmpdir	(tmpdirname())
#define WIDEP_tmpdir   (tmpdirname())
#endif //__SYMBIAN32__
#endif
#ifndef __SYMBIAN32__
#define	L_tmpnam	1024	/* XXX must be == PATH_MAX */
#else
#define	L_tmpnam	256	/* XXX must be == PATH_MAX */
#endif /* __SYMBIAN32__ */
#define	TMP_MAX		308915776

#ifndef SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))

#define GET_WSD_VAR_NAME(var,varprefix) 	_##varprefix##_##var

FILE **GET_WSD_VAR_NAME(__stdinp, g)();
FILE **GET_WSD_VAR_NAME(__stdoutp, g)();
FILE **GET_WSD_VAR_NAME(__stderrp, g)();

#define __stdinp   (*GET_WSD_VAR_NAME(__stdinp, g)())
#define __stdoutp (*GET_WSD_VAR_NAME(__stdoutp, g)())
#define __stderrp (*GET_WSD_VAR_NAME(__stderrp, g)())
#endif //EMULATOR

#ifndef __SYMBIAN32__
#define	stdin	__stdinp
#define	stdout	__stdoutp
#define	stderr	__stderrp
#else
__BEGIN_DECLS
IMPORT_C FILE *__stdin  (void);
IMPORT_C FILE *__stdout (void);
IMPORT_C FILE *__stderr (void);
IMPORT_C char * tmpdirname(void);
__END_DECLS
#define stdin 	  (__stdin())
#define stdout  (__stdout())
#define stderr  (__stderr())
#endif

__BEGIN_DECLS
/*
 * Functions defined in ANSI C standard.
 */
IMPORT_C void	 clearerr(FILE *);
IMPORT_C int	 fclose(FILE *);
IMPORT_C int	 feof(FILE *);
IMPORT_C int	 ferror(FILE *);
IMPORT_C int	 fflush(FILE *);
IMPORT_C int	 fgetc(FILE *);
IMPORT_C int	 fgetpos(FILE * __restrict, fpos_t * __restrict);
IMPORT_C char	*fgets(char * __restrict, int, FILE * __restrict);
IMPORT_C FILE	*fopen(const char * __restrict, const char * __restrict);
IMPORT_C int	 fprintf(FILE * __restrict, const char * __restrict, ...);
IMPORT_C int	 fputc(int, FILE *);
IMPORT_C int	 fputs(const char * __restrict, FILE * __restrict);
IMPORT_C size_t	 fread(void * __restrict, size_t, size_t, FILE * __restrict);
IMPORT_C FILE	*freopen(const char * __restrict, const char * __restrict, FILE * __restrict);
IMPORT_C int	 fscanf(FILE * __restrict, const char * __restrict, ...);
IMPORT_C int	 fseek(FILE *, long, int);
IMPORT_C int	 fsetpos(FILE *, const fpos_t *);
IMPORT_C long	 ftell(FILE *);
IMPORT_C size_t	 fwrite(const void * __restrict, size_t, size_t, FILE * __restrict);
IMPORT_C int	 getc(FILE *);
IMPORT_C int	 getchar(void);
IMPORT_C char	*gets(char *);
IMPORT_C void	 perror(const char *);
IMPORT_C int	 printf(const char * __restrict, ...);
IMPORT_C int	 putc(int, FILE *);
IMPORT_C int	 putchar(int);
IMPORT_C int	 puts(const char *);
IMPORT_C int	 remove(const char *);
IMPORT_C int	 rename(const char *, const char *);
IMPORT_C void	 rewind(FILE *);
IMPORT_C int	 scanf(const char * __restrict, ...);
IMPORT_C void	 setbuf(FILE * __restrict, char * __restrict);

#ifdef __SYMBIAN32__
IMPORT_C int     set_fmode(char mode);
IMPORT_C char     get_fmode(void);
#endif

IMPORT_C int	 setvbuf(FILE * __restrict, char * __restrict, int, size_t);
IMPORT_C int	 sprintf(char * __restrict, const char * __restrict, ...);
IMPORT_C int	 sscanf(const char * __restrict, const char * __restrict, ...);
IMPORT_C FILE	*tmpfile(void);
IMPORT_C char	*tmpnam(char *);
IMPORT_C int	 ungetc(int, FILE *);
IMPORT_C int	 vfprintf(FILE * __restrict, const char * __restrict,
	    va_list);
IMPORT_C int	 vprintf(const char * __restrict, va_list);
IMPORT_C int	 vsprintf(char * __restrict, const char * __restrict,
	    va_list);


#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
#define fgetpos64	fgetpos
#define fopen64		fopen
#define freopen64	freopen
#define fsetpos64	fsetpos
#define tmpfile64	tmpfile
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */


#if __ISO_C_VISIBLE >= 1999
IMPORT_C int	 snprintf(char * __restrict, size_t, const char * __restrict,
	    ...) __printflike(3, 4);
IMPORT_C int	 vfscanf(FILE * __restrict, const char * __restrict, va_list)
	    __scanflike(2, 0);
IMPORT_C int	 vscanf(const char * __restrict, va_list) __scanflike(1, 0);
IMPORT_C int	 vsnprintf(char * __restrict, size_t, const char * __restrict,
	    va_list) __printflike(3, 0);
IMPORT_C int	 vsscanf(const char * __restrict, const char * __restrict, va_list)
	    __scanflike(2, 0);
#endif

/*
 * Functions defined in all versions of POSIX 1003.1.
 */
#if __BSD_VISIBLE || __POSIX_VISIBLE <= 199506
/* size for cuserid(3); UT_NAMESIZE + 1, see <utmp.h> */
#define	L_cuserid	17	/* legacy */
#endif

#if __POSIX_VISIBLE
#ifndef __SYMBIAN32__
#define	L_ctermid	1024	/* size for ctermid(3); PATH_MAX */
#else
#define	L_ctermid	256	   /* size for ctermid(3); PATH_MAX */
#endif /* __SYMBIAN32__ */
IMPORT_C FILE	*fdopen(int, const char *);
IMPORT_C int	 fileno(FILE *);
IMPORT_C int __sfileno(FILE* p);
#endif /* __POSIX_VISIBLE */

#if __POSIX_VISIBLE >= 199209
IMPORT_C int  pclose(FILE *);
IMPORT_C FILE	*popen(const char *, const char *);
#endif

#ifdef __SYMBIAN32__
IMPORT_C int popen3(const char *file, const char *cmd, char** envp, int fds[3]);
#endif

#if __POSIX_VISIBLE >= 199506
IMPORT_C int	 ftrylockfile(FILE *);
IMPORT_C void	 flockfile(FILE *);
IMPORT_C void	 funlockfile(FILE *);

/*
 * These are normally used through macros as defined below, but POSIX
 * requires functions as well.
 */
IMPORT_C int	 getc_unlocked(FILE *);
IMPORT_C int	 getchar_unlocked(void);
IMPORT_C int	 putc_unlocked(int, FILE *);
IMPORT_C int	 putchar_unlocked(int);
#endif

#if __POSIX_VISIBLE >= 200112
IMPORT_C int	 fseeko(FILE *, __off_t, int);
IMPORT_C __off_t	 ftello(FILE *);

#if defined(SYMBIAN_OE_LARGE_FILE_SUPPORT) && !defined(SYMBIAN_OE_NO_LFS)
#define fseeko64	fseeko
#define ftello64	ftello
#endif /* SYMBIAN_OE_LARGE_FILE_SUPPORT && !SYMBIAN_OE_NO_LFS */

#endif

#if __BSD_VISIBLE || __XSI_VISIBLE > 0 && __XSI_VISIBLE < 600
IMPORT_C int	 getw(FILE *);
IMPORT_C int	 putw(int, FILE *);
#endif /* BSD or X/Open before issue 6 */

#if __XSI_VISIBLE
IMPORT_C char	*tempnam(const char *, const char *);
#endif

/*
 * Routines that are purely local.
 */
#if __BSD_VISIBLE
IMPORT_C int	 asprintf(char **, const char *, ...) __printflike(2, 3);
#if __GNUC__ == 2 && __GNUC_MINOR__ >= 7 || __GNUC__ >= 3
#define	__ATTR_FORMAT_ARG	__attribute__((__format_arg__(2)))
#else
#define	__ATTR_FORMAT_ARG
#endif
IMPORT_C void	 setbuffer(FILE *, char *, int);
IMPORT_C int	 setlinebuf(FILE *);
IMPORT_C int	 vasprintf(char **, const char *, va_list)
	    __printflike(2, 0);

#ifndef __SYMBIAN32__
/*
 * The system error table contains messages for the first sys_nerr
 * positive errno values.  Use strerror() or strerror_r() from <string.h>
 * instead.
 */

/* Provide the declarations for `sys_errlist' and `sys_nerr' if they
 * are available on this system.  Even if available, these variables
 * should not be used directly.  The `strerror' function provides
 * all the necessary functionality.
 */
 
extern __const int sys_nerr;
extern __const char *__const sys_errlist[];
#endif /* __SYMBIAN32__ */

/*
 * Portability hacks.  See <sys/types.h>.
 */
#ifndef _FTRUNCATE_DECLARED
#define	_FTRUNCATE_DECLARED
IMPORT_C int	 ftruncate(int, __off_t);
#endif
#ifndef _LSEEK_DECLARED
#define	_LSEEK_DECLARED
IMPORT_C __off_t	 lseek(int, __off_t, int);
#endif
#ifndef _MMAP_DECLARED
#define	_MMAP_DECLARED
IMPORT_C void	*mmap(void *, size_t, int, int, int, __off_t);
#endif
#ifndef _TRUNCATE_DECLARED
#define	_TRUNCATE_DECLARED
IMPORT_C int	 truncate(const char *, __off_t);
#endif
#endif /* __BSD_VISIBLE */

#ifdef __SYMBIAN32__
IMPORT_C int setecho(int fd, uint8_t echoval);
#endif

/*
 * Functions internal to the implementation.
 */
#ifdef __SYMBIAN32__
IMPORT_C int	__srget(FILE *);
IMPORT_C int	__swbuf(int, FILE *);
#else
int	__srget(FILE *);
int	__swbuf(int, FILE *);
#endif /*__SYMBIAN32__ */

/*
 * The __sfoo macros are here so that we can
 * define function versions in the C library.
 */
#define	__sgetc(p) (--(p)->_r < 0 ? __srget(p) : (int)(*(p)->_p++))
#if defined(__GNUC__) && defined(__STDC__)
static __inline int __sputc(int _c, FILE *_p) {
	if (--_p->_w >= 0 || (_p->_w >= _p->_lbfsize && (char)_c != '\n'))
		return (*_p->_p++ = _c);
	else
		return (__swbuf(_c, _p));
}
#else
/*
 * This has been tuned to generate reasonable code on the vax using pcc.
 */
#define	__sputc(c, p) \
	(--(p)->_w < 0 ? \
		(p)->_w >= (p)->_lbfsize ? \
			(*(p)->_p = (c)), *(p)->_p != '\n' ? \
				(int)*(p)->_p++ : \
				__swbuf('\n', p) : \
			__swbuf((int)(c), p) : \
		(*(p)->_p = (c), (int)*(p)->_p++))
#endif

#define	__sfeof(p)	(((p)->_flags & __SEOF) != 0)
#define	__sferror(p)	(((p)->_flags & __SERR) != 0)
#define	__sclearerr(p)	((void)((p)->_flags &= ~(__SERR|__SEOF)))

#ifndef __SYMBIAN32__
extern int __isthreaded;

#define	feof(p)		(!__isthreaded ? __sfeof(p) : (feof)(p))
#define	ferror(p)	(!__isthreaded ? __sferror(p) : (ferror)(p))
#define	clearerr(p)	(!__isthreaded ? __sclearerr(p) : (clearerr)(p))

#if __POSIX_VISIBLE
#define	fileno(p)	(!__isthreaded ? __sfileno(p) : (fileno)(p))
#endif

#define	getc(fp)	(!__isthreaded ? __sgetc(fp) : (getc)(fp))
#define	putc(x, fp)	(!__isthreaded ? __sputc(x, fp) : (putc)(x, fp))

#define	getchar()	getc(stdin)
#define	putchar(x)	putc(x, stdout)

#else
IMPORT_C int* isthreaded(void);
#define	__isthreaded	(*isthreaded())

#define	feof(p)		(!__isthreaded ? __sfeof(p) : (feof)(p))
#define	ferror(p)	(!__isthreaded ? __sferror(p) : (ferror)(p))
#define	clearerr(p)	(!__isthreaded ? __sclearerr(p) : (clearerr)(p))

#if __POSIX_VISIBLE
#define	fileno(p)	(!__isthreaded ? __sfileno(p) : (fileno)(p))
#endif

#define	getc(fp)	(!__isthreaded ? __sgetc(fp) : (getc)(fp))
#define	putc(x, fp)	(!__isthreaded ? __sputc(x, fp) : (putc)(x, fp))

#define	getchar()	getc(stdin)
#define	putchar(x)	putc(x, stdout)
#endif //__SYMBIAN32__


#if __POSIX_VISIBLE >= 199506
#ifndef __SYMBIAN32__
#define	getc_unlocked(fp)	__sgetc(fp)
#define	putc_unlocked(x, fp)	__sputc(x, fp)

#define	getchar_unlocked()	getc_unlocked(stdin)
#define	putchar_unlocked(x)	putc_unlocked(x, stdout)
#endif
#endif

//---
#ifdef __cplusplus
}
#endif
//---

__END_DECLS
#endif /* !_STDIO_H_ */
