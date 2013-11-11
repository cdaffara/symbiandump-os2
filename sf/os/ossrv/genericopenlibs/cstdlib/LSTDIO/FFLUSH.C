/* FFLUSH.C
 * 
 * Portions Copyright (c) 1990-2006 Nokia Corporation and/or its subsidiary(-ies).
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
 */

/*
FUNCTION
<<fflush>>---flush buffered file output

INDEX
	fflush

ANSI_SYNOPSIS
	#include <stdio.h>
	int fflush(FILE *<[fp]>);

TRAD_SYNOPSIS
	#include <stdio.h>
	int fflush(<[fp]>)
	FILE *<[fp]>;

DESCRIPTION
The <<stdio>> output functions can buffer output before delivering it
to the host system, in order to minimize the overhead of system calls.

Use <<fflush>> to deliver any such pending output (for the file
or stream identified by <[fp]>) to the host system.

If <[fp]> is <<NULL>>, <<fflush>> delivers pending output from all
open files.

RETURNS
<<fflush>> returns <<0>> unless it encounters a write error; in that
situation, it returns <<EOF>>.

PORTABILITY
ANSI C requires <<fflush>>.

No supporting OS subroutines are required.
*/

#include <stdio.h>
#include "LOCAL.H"

/**
Flush a stream. If the given stream has been opened for writing operations
the output buffer is phisically written to the file. If the stream was open
for reading operations the content of the input buffer is cleared. The stream
remains open after this call.

@param fp pointer to an open file.

@return On Success, a 0 value indicates success.
		On Failure, EOF is returned and errno may be set.
*/
EXPORT_C int
fflush (FILE * fp)
{
  register unsigned char *p;
  register int n, t;

  if (fp == NULL)
  	{
    struct _reent *r = _REENT2;
    if (!r)
	  return EOF; // Memory for library globals is not allocated (errno not set).
    return _fwalk (r, fflush);
  	}
  CHECK_INIT (fp);

  t = fp->_flags;
  if ((t & __SWR) == 0 || (p = fp->_bf._base) == NULL)
    return 0;
  n = fp->_p - p;		/* write this much */

  /*
   * Set these immediately to avoid problems with longjmp
   * and to allow exchange buffering (via setvbuf) in user
   * write function.
   */
  fp->_p = p;
  fp->_w = t & (__SLBF | __SNBF) ? 0 : fp->_bf._size;

  while (n > 0)
    {
      t = (*fp->_write) (fp->_cookie, (char *) p, n);
      if (t <= 0)
	{
	  fp->_flags |= __SERR;
	  return EOF;
	}
      p += t;
      n -= t;
    }
  return 0;
}
