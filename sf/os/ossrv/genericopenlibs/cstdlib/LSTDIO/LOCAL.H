/* LOCAL.H
 * 
 * Portions Copyright (c) 1990-1999 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
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
 *	%W% (UofMD/Berkeley) %G%
 */

/*
 * Information local to this implementation of stdio,
 * in particular, macros and private variables.
 */

#include <_ansi.h>
#include <stdarg.h>
#include <reent.h>
#include <unistd.h>
/**
@internalComponent
*/
extern int    __svfscanf	(FILE *, const char *,va_list);
extern FILE*  __sfp		(struct _reent *);
extern int    __sflags		(struct _reent *,const wchar_t*, int*);
extern int    __srefill		(FILE *);
extern int    __sread		(void *, char *, int);
extern int    __swrite		(void *, char const *, int);
extern fpos_t __sseek		(void *, fpos_t, int);
extern int    __sclose		(void *);
extern void   __sinit		(struct _reent *);
extern void   __smakebuf	(FILE *);
extern int    _fwalk		(struct _reent *, int (*)(FILE *));
struct _glue* __sfmoreglue	(struct _reent *,int n);
extern int    __srefill		(FILE *fp);
extern int    __srget		(FILE *fp);
extern int    __swbuf		(int c, FILE *fp);

/**
Macros used inside the implementation of STDIO. These used to be in
the <stdio.h> include file, but we want to hide them as much as
possible to give us a change to change the implementation if 
necessary.
@internalComponent
*/
#define	__sfeof(p)	(((p)->_flags & __SEOF) != 0)
#define	__sferror(p)	(((p)->_flags & __SERR) != 0)
#define	__sclearerr(p)	((void)((p)->_flags &= ~(__SERR|__SEOF)))
#define	__sfileno(p)	((p)->_file)
/**
@internalComponent
*/
#define	feof(p)		__sfeof(p)
#define	ferror(p)	__sferror(p)
#define	clearerr(p)	__sclearerr(p)
/**
@internalComponent
*/
#define	__sgetc(p) (--(p)->_r < 0 ? __srget(p) : (int)(*(p)->_p++))
#define	__sputc(c, p) \
	(--(p)->_w < 0 ? \
		(p)->_w >= (p)->_lbfsize ? \
			(*(p)->_p = (unsigned char)(c)), *(p)->_p != '\n' ? \
				(int)*(p)->_p++ : \
				__swbuf('\n', p) : \
			__swbuf((int)(c), p) : \
		(*(p)->_p = (unsigned char)(c), (int)*(p)->_p++))

/** 
Called by the main entry point fns to ensure stdio has been initialized.  
@internalComponent
*/
#define CHECK_INIT(fp)	/* done in the construction of the struct _reent */
/*
#define CHECK_INIT(fp) \
    {					\
      if ((fp)->_data == 0)			\
		(fp)->_data = _REENT;		\
      if (!(fp)->_data->__sdidinit)	\
		__sinit ((fp)->_data);		\
    }
*/

/**
Return true iff the given FILE cannot be written now.  
@internalComponent
*/
#define	cantwrite(fp) \
  ((((fp)->_flags & __SWR) == 0 || (fp)->_bf._base == NULL) && \
   __swsetup(fp))

/**
Test whether the given stdio file has an active ungetc buffer;
release such a buffer, without restoring ordinary unread data.  
@internalComponent
*/
#define	HASUB(fp) ((fp)->_ub._base != NULL)
#define	FREEUB(fp) { \
	if ((fp)->_ub._base != (fp)->_ubuf) \
		_free_r(fp->_data, (char *)(fp)->_ub._base); \
	(fp)->_ub._base = NULL; \
}

/**
Test for an fgetline() buffer.  
@internalComponent
*/
#define	HASLB(fp) ((fp)->_lb._base != NULL)
#define	FREELB(fp) { _free_r(fp->_data,(char *)(fp)->_lb._base); (fp)->_lb._base = NULL; }

/* 
WARNING: _dcvt is defined in the stdlib directory, not here!  
@internalComponent
*/
char *_dcvt		(struct _reent *, char *, double, int, int, char, int);
char *_sicvt	(char *, short, char);
char *_icvt		(char *, int, char);
char *_licvt	(char *, long, char);
#ifdef __GNUC__
char *_llicvt	(char *, long long, char);
#endif
/**
@internalComponent
*/
#define CVT_BUF_SIZE 128
/**
@internalComponent
*/
#define	NDYNAMIC 4	/* add four more whenever necessary */
