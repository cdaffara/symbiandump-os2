/* SETVBUF.C
 * 
 * Portions Copyright (c) 1990-2009 Nokia Corporation and/or its subsidiary(-ies).
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
<<setvbuf>>---specify file or stream buffering

INDEX
	setvbuf

ANSI_SYNOPSIS
	#include <stdio.h>
	int setvbuf(FILE *<[fp]>, char *<[buf]>,
	            int <[mode]>, size_t <[size]>);

TRAD_SYNOPSIS
	#include <stdio.h>
	int setvbuf(<[fp]>, <[buf]>, <[mode]>, <[size]>)
	FILE *<[fp]>;
	char *<[buf]>;
	int <[mode]>;
	size_t <[size]>;

DESCRIPTION
Use <<setvbuf>> to specify what kind of buffering you want for the
file or stream identified by <[fp]>, by using one of the following
values (from <<stdio.h>>) as the <[mode]> argument:

o+
o _IONBF
Do not use a buffer: send output directly to the host system for the
file or stream identified by <[fp]>.

o _IOFBF
Use full output buffering: output will be passed on to the host system
only when the buffer is full, or when an input operation intervenes.

o _IOLBF
Use line buffering: pass on output to the host system at every
newline, as well as when the buffer is full, or when an input
operation intervenes.
o-

Use the <[size]> argument to specify how large a buffer you wish.  You
can supply the buffer itself, if you wish, by passing a pointer to a
suitable area of memory as <[buf]>.  Otherwise, you may pass <<NULL>>
as the <[buf]> argument, and <<setvbuf>> will allocate the buffer.

WARNINGS
You may only use <<setvbuf>> before performing any file operation other
than opening the file.

If you supply a non-null <[buf]>, you must ensure that the associated
storage continues to be available until you close the stream
identified by <[fp]>.

RETURNS
A <<0>> result indicates success, <<EOF>> failure (invalid <[mode]> or
<[size]> can cause failure).

PORTABILITY
Both ANSI C and the System V Interface Definition (Issue 2) require
<<setvbuf>>. However, they differ on the meaning of a <<NULL>> buffer
pointer: the SVID issue 2 specification says that a <<NULL>> buffer
pointer requests unbuffered output.  For maximum portability, avoid
<<NULL>> buffer pointers.

Both specifications describe the result on failure only as a
nonzero value.

Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
<<lseek>>, <<read>>, <<sbrk>>, <<write>>.
*/

#include <stdio_r.h>
#include <stdlib_r.h>
#include "LOCAL.H"

/**
Change stream buffering.
Changes the buffer to be used for I/O operations with the specified stream.
Size and mode for the buffer can be specified.
This function should be called once the file associated with the stream 
has been opened but before any input or output operation has been done.
The size of the buffer is specified by the size parameter,
and can be any value between 2 and 32767 in bytes, 
this value may be rounded down by some system due to specific alignment.
buffer can be NULL.
@return If the buffer is correctly assigned to the file a 0 value is returned.
On error, a non-zero value is returned. This can be because an invalid type or size has been specified or because an error allocating memory (if NULL buffer was specified).
@param fp pointer to an open file. 
@param buf User allocated buffer. Must have at least a size of size bytes. 
@param mode Specifies a mode for file buffering
@param Buffer size in bytes, must be more than 0 and less than 32768, this value may be rounded down by some systems due to specific alignment, in which case the minimum value should be 2.
*/
EXPORT_C int
setvbuf (FILE * fp, char *buf, int mode, size_t size)
{
  int ret = 0;
  CHECK_INIT (fp);

  /*
   * Verify arguments.  The `int' limit on `size' is due to this
   * particular implementation.
   */

  if ((mode != _IOFBF && mode != _IOLBF && mode != _IONBF) || (int) size < 0)
    return (EOF);

  /*
   * Write current buffer, if any; drop read count, if any.
   * Make sure putc() will not think fp is line buffered.
   * Free old buffer if it was from malloc().  Clear line and
   * non buffer flags, and clear malloc flag.
   */

  (void) fflush (fp);
  fp->_r = 0;
  fp->_lbfsize = 0;
  if (fp->_flags & __SMBF)
    _free_r (fp->_data, (void *) fp->_bf._base);
  fp->_flags &= ~(__SLBF | __SNBF | __SMBF);

  if (mode == _IONBF)
    goto nbf;

  /*
   * Allocate buffer if needed. */
  if (buf == NULL)
    {
      if ((buf = (char *)malloc (size)) == NULL)
	{
	  ret = EOF;
	  /* Try another size... */
	  buf = (char *)malloc (BUFSIZ);
	}
      if (buf == NULL)
	{
	  /* Can't allocate it, let's try another approach */
nbf:
	  fp->_flags |= __SNBF;
	  fp->_w = 0;
	  fp->_bf._base = fp->_p = fp->_nbuf;
	  fp->_bf._size = 1;
	  return (ret);
	}
      fp->_flags |= __SMBF;
    }
  /*
   * Now put back whichever flag is needed, and fix _lbfsize
   * if line buffered.  Ensure output flush on exit if the
   * stream will be buffered at all.
   * Force the buffer to be flushed and hence malloced on first use
   */

  switch (mode)
    {
    case _IOLBF:
      fp->_flags |= __SLBF;
      fp->_lbfsize = -(int)size;
      /* FALLTHROUGH */

    case _IOFBF:
      /* no flag */
      fp->_data->__cleanup = _cleanup_r;
      fp->_bf._base = fp->_p = (unsigned char *) buf;
      fp->_bf._size = size;
      break;
    }

  /*
   * Patch up write count if necessary.
   */

  if (fp->_flags & __SWR)
    fp->_w = fp->_flags & (__SLBF | __SNBF) ? 0 : size;

  return 0;
}
