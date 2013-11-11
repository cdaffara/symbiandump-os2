/* FTELL.C
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
 */

/*
FUNCTION
<<ftell>>---return position in a stream or file

INDEX
	ftell

ANSI_SYNOPSIS
	#include <stdio.h>
	long ftell(FILE *<[fp]>);

TRAD_SYNOPSIS
	#include <stdio.h>
	long ftell(<[fp]>)
	FILE *<[fp]>;

DESCRIPTION
Objects of type <<FILE>> can have a ``position'' that records how much
of the file your program has already read.  Many of the <<stdio>> functions
depend on this position, and many change it as a side effect.

The result of <<ftell>> is the current position for a file
identified by <[fp]>.  If you record this result, you can later
use it with <<fseek>> to return the file to this
position.

In the current implementation, <<ftell>> simply uses a character
count to represent the file position; this is the same number that
would be recorded by <<fgetpos>>.

RETURNS
<<ftell>> returns the file position, if possible.  If it cannot do
this, it returns <<-1L>>.  Failure occurs on streams that do not support
positioning; the global <<errno>> indicates this condition with the
value <<ESPIPE>>.

PORTABILITY
<<ftell>> is required by the ANSI C standard, but the meaning of its
result (when successful) is not specified beyond requiring that it be
acceptable as an argument to <<fseek>>.  In particular, other
conforming C implementations may return a different result from
<<ftell>> than what <<fgetpos>> records.

No supporting OS subroutines are required.
*/

/*
 * ftell: return current offset.
 */

#include <stdio_r.h>
#include <errno.h>

#include "LOCAL.H"

/**
Return the current position in a stream.
Returns the current position pointed by the position indicator of the stream. 
When a file has been opened in binary mode the value obtained corresponds to 
the number of bytes from the beginning of the file. In files opened in 
text-mode this is not granted because of carriage-return translations under that mode.
@return On success, the current file pointer position is returned.
If an error occurs -1 is returned.
@param Pointer to an open file.
*/
EXPORT_C long
ftell (FILE * fp)
{
  fpos_t pos;

  /* Ensure stdio is set up.  */

  CHECK_INIT (fp);

  if (fp->_seek == NULL)
    {
      __errno_r(fp->_data) = ESPIPE;
      return -1L;
    }

  /* Find offset of underlying I/O object, then
     adjust for buffered bytes.  */

  if (fp->_flags & __SOFF)
    pos = fp->_offset;
  else
    {
      pos = (*fp->_seek) (fp->_cookie, (fpos_t) 0, SEEK_CUR);
      if (pos == -1L)
	return pos;
    }
  if (fp->_flags & __SRD)
    {
      /*
       * Reading.  Any unread characters (including
       * those from ungetc) cause the position to be
       * smaller than that in the underlying object.
       */
      pos -= fp->_r;
      if (HASUB (fp))
	pos -= fp->_ur;
    }
  else if (fp->_flags & __SWR && fp->_p != NULL)
    {
      /*
       * Writing.  Any buffered characters cause the
       * position to be greater than that in the
       * underlying object.
       */
      pos += fp->_p - fp->_bf._base;
    }

  return pos;
}
