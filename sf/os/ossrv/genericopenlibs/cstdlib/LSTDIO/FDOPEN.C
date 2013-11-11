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
* FUNCTION
* <<fdopen>>---turn open file into a stream
* INDEX
* fdopen
* INDEX
* _fdopen_r
* ANSI_SYNOPSIS
* #include <stdio.h>
* FILE *fdopen(int <[fd]>, const char *<[mode]>);
* FILE *_fdopen_r(void *<[reent]>,
* int <[fd]>, const char *<[mode]>);
* TRAD_SYNOPSIS
* #include <stdio.h>
* FILE *fdopen(<[fd]>, <[mode]>)
* int <[fd]>;
* char *<[mode]>;
* FILE *_fdopen_r(<[reent]>, <[fd]>, <[mode]>)
* char *<[reent]>;
* int <[fd]>;
* char *<[mode]>);
* <<fdopen>> produces a file descriptor of type <<FILE *>>, from a
* descriptor for an already-open file (returned, for example, by the
* system subroutine <<open>> rather than by <<fopen>>).
* The <[mode]> argument has the same meanings as in <<fopen>>.
* RETURNS
* File pointer or <<NULL>>, as for <<fopen>>.
* PORTABILITY
* <<fdopen>> is ANSI.
* 
*
*/



#include <sys/types.h>

#include <stdlib_r.h>
#include <stdio_r.h>
#include <errno.h>
#include "LOCAL.H"


#define MaxFullName 255
/**
A reentrant version of fdopen().
*/
EXPORT_C FILE * _fdopen_r (struct _reent *ptr, int fd, const char *mode)
	{
	wchar_t _wmode[MaxFullName+1];

	if (-1 != mbstowcs(_wmode, mode, MaxFullName))
		return _wfdopen_r(ptr, fd, _wmode);

	ptr->_errno = EILSEQ;
	return NULL;

	}
/**
A reentrant version of wfdopen().
*/
EXPORT_C FILE * _wfdopen_r (struct _reent *ptr, int fd, const wchar_t *mode)
{
  register FILE *fp;
  int flags, oflags;

  if ((flags = __sflags (ptr, mode, &oflags)) == 0)
    return 0;

  /* make sure the mode the user wants is a subset of the actual mode */
#ifdef F_GETFL
  if ((fdflags = _fcntl (fd, F_GETFL, 0)) < 0)
    return 0;
  fdmode = fdflags & O_ACCMODE;
  if (fdmode != O_RDWR && (fdmode != (oflags & O_ACCMODE)))
    {
      ptr->_errno = EBADF;
      return 0;
    }
#endif

  if ((fp = __sfp (ptr)) == 0)
    return 0;
  fp->_flags = (short)flags;
  /*
   * If opened for appending, but underlying descriptor
   * does not have O_APPEND bit set, assert __SAPP so that
   * __swrite() will lseek to end before each write.
   */
#ifdef F_GETFL
  if ((oflags & O_APPEND) && !(fdflags & O_APPEND))
#endif
    fp->_flags |= __SAPP;
  fp->_file = (short)fd;
  fp->_cookie = (void*) fp;

#undef _read
#undef _write
#undef _seek
#undef _close

  fp->_read = __sread;
  fp->_write = __swrite;
  fp->_seek = __sseek;
  fp->_close = __sclose;
  return fp;
}

#ifndef _REENT_ONLY
/**
This function associates a stream with an open file descriptor. 
A stream is a pointer to a FILE structure that contains information about a file.
A stream permits user-controlled buffering and formatted input and output.
@return a FILE pointer to the control block for the new stream.
@param fd The open file descriptor on which to open a stream.
@param mode The access mode for the stream. 
*/
EXPORT_C FILE * fdopen (int fd, const char *mode)
{
  return _fdopen_r (_REENT, fd, mode);
}

/**
A wide-character version of fdopen()
*/
EXPORT_C FILE * wfdopen (int fd, const wchar_t *mode)
{
  return _wfdopen_r (_REENT, fd, mode);
}

#endif
