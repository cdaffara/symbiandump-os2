/* FINDFP.C
 * 
 * Portions Copyright (c) 1990-2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */

/* No user fns here.  Pesch 15apr92. */

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
 */

#include <stdio_r.h>
#include <stdlib_r.h>
#include <errno.h>
#include <string.h>
#include "LOCAL.H"

/* Initialise a FILE structure to a starting state */
#ifdef __GCCXML__
static void
_std (FILE *ptr, int flags, int file, struct _reent *data)
#else
static void
std (FILE *ptr, int flags, int file, struct _reent *data)
#endif
{
  ptr->_p = 0;
  ptr->_r = 0;
  ptr->_w = 0;
  ptr->_flags = (short)flags;
  ptr->_file = (short)file;
  ptr->_bf._base = 0;
  ptr->_lbfsize = 0;
  ptr->_cookie = ptr;
  ptr->_read = __sread;
  ptr->_write = __swrite;
  ptr->_seek = __sseek;
  ptr->_close = __sclose;
  ptr->_data = data;
}

struct _glue *
__sfmoreglue (struct _reent *d, register int n)
{
  struct _glue *g;
  FILE *p;

  g = (struct _glue *) _malloc_r (d, sizeof (*g) + n * sizeof (FILE));
  if (g == NULL)
    return NULL;
  p = (FILE *) (g + 1);
  g->_next = NULL;
  g->_niobs = n;
  g->_iobs = p;
  memset (p, 0, n * sizeof (FILE));
  return g;
}

/*
 * Find a free FILE for fopen et al.
 */

FILE *
__sfp (struct _reent *d)
{
  FILE *fp;
  int n;
  struct _glue *g;

  if (!d->__sdidinit)
    __sinit (d);
  for (g = &d->__sglue;; g = g->_next)
    {
      for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
	if (fp->_flags == 0)
	  goto found;
      if (g->_next == NULL &&
	  (g->_next = __sfmoreglue (d, NDYNAMIC)) == NULL)
	break;
    }
  d->_errno = ENOMEM;
  return NULL;

found:
  fp->_flags = 1;		/* reserve this slot; caller sets real flags */
  fp->_p = NULL;		/* no current pointer */
  fp->_w = 0;			/* nothing to read or write */
  fp->_r = 0;
  fp->_bf._base = NULL;		/* no buffer */
  fp->_bf._size = 0;
  fp->_lbfsize = 0;		/* not line buffered */
  fp->_file = -1;		/* no file */
  /* fp->_cookie = <any>; */	/* caller sets cookie, _read/_write etc */
  fp->_ub._base = NULL;		/* no ungetc buffer */
  fp->_ub._size = 0;
  fp->_lb._base = NULL;		/* no line buffer */
  fp->_lb._size = 0;
  fp->_data = d;
  return fp;
}

/*
 * exit() calls _cleanup() through *__cleanup, set whenever we
 * open or buffer a file.  This chicanery is done so that programs
 * that do not use stdio need not link it all in.
 *
 * The name `_cleanup' is, alas, fairly well known outside stdio.
 */
/**
A reentrant version of _cleanup().
*/
EXPORT_C void
_cleanup_r (struct _reent *ptr)
{
  (void) _fwalk(ptr, fclose);
}

#ifndef _REENT_ONLY

/**
Performs cleanup
*/
EXPORT_C void
_cleanup (void)
{
  _cleanup_r (_REENT);
}
#endif

/*
 * __sinit() is called whenever stdio's internal variables must be set up.
 */

void
__sinit (struct _reent *s)
{
  /* make sure we clean up on exit */
  s->__cleanup = _cleanup_r;	/* conservative */
  s->__sdidinit = 1;

#ifdef __GCCXML__
  _std (_stdin_r(s),  __SRD, 0, s);
  _std (_stdout_r(s), __SWR, 1, s);
  _std (_stderr_r(s), __SWR | __SNBF, 2, s);
#else
  std (_stdin_r(s),  __SRD, 0, s);
  std (_stdout_r(s), __SWR, 1, s);
  std (_stderr_r(s), __SWR | __SNBF, 2, s);
#endif

  /* initialise the head of the glue chain to cover stdim/stdout/stderr */
  s->__sglue._next = NULL;
  s->__sglue._niobs = 3;
  s->__sglue._iobs = &s->_sf[0];
}

/* Function interface to stdin/stdout/stderr 
 *
 * These functions should return a value which is fixed for any given
 * reentrancy context, but it doesn't have to be fixed at compile time,
 * and we don't need to have initialised stdio either.
 */

/**
Function interface to the "constants" stdin.
These functions guarantee to return a fixed value, so that it 
will be possible to use expressions such as if (fp != stdout) fclose(fp);
with complete confidence. Unfortunately it will rule out initialising global
variables with stdin/stdout/stderr, as in the common idiom:

static FILE *log = stderr;

This isn't currently possible with EPOC32. 
*/
EXPORT_C FILE *__stdin (void)
{
	return _stdin_r(_REENT);
}

/**
Function interface to the "constants" stdout
See __stdin
*/
EXPORT_C FILE *__stdout (void)
{
	return _stdout_r(_REENT);
}

/**
Function interface to the "constants" stderr
See __stdin
*/
EXPORT_C FILE *__stderr (void)
{
	return _stderr_r(_REENT);
}
