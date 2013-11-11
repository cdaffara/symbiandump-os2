/* SETBUF.C
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
<<setbuf>>---specify full buffering for a file or stream

INDEX
	setbuf

ANSI_SYNOPSIS
	#include <stdio.h>
	void setbuf(FILE *<[fp]>, char *<[buf]>);

TRAD_SYNOPSIS
	#include <stdio.h>
	void setbuf(<[fp]>, <[buf]>)
	FILE *<[fp]>;
	char *<[buf]>;

DESCRIPTION
<<setbuf>> specifies that output to the file or stream identified by <[fp]>
should be fully buffered.  All output for this file will go to a
buffer (of size <<BUFSIZ>>, specified in `<<stdio.h>>').  Output will
be passed on to the host system only when the buffer is full, or when
an input operation intervenes.

You may, if you wish, supply your own buffer by passing a pointer to
it as the argument <[buf]>.  It must have size <<BUFSIZ>>.  You can
also use <<NULL>> as the value of <[buf]>, to signal that the
<<setbuf>> function is to allocate the buffer.

WARNINGS
You may only use <<setbuf>> before performing any file operation other
than opening the file.

If you supply a non-null <[buf]>, you must ensure that the associated
storage continues to be available until you close the stream
identified by <[fp]>.

RETURNS
<<setbuf>> does not return a result.

PORTABILITY
Both ANSI C and the System V Interface Definition (Issue 2) require
<<setbuf>>.  However, they differ on the meaning of a <<NULL>> buffer
pointer: the SVID issue 2 specification says that a <<NULL>> buffer
pointer requests unbuffered output.  For maximum portability, avoid
<<NULL>> buffer pointers.

Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
<<lseek>>, <<read>>, <<sbrk>>, <<write>>.
*/

#include <stdio.h>
#include "LOCAL.H"

/**
Change stream buffering.
Changes the buffer used for I/O operations with the specified stream, or,
if the specified buffer is NULL it disables buffering with the stream.
This function should be called once the file associated with the stream 
has been opened but before any input or output operation has been done.
With buffered streams writing operations do not write directly 
to the device associated with them; the data is accumulated in the buffer 
and written to the device as a block.
All buffers are also flushed when program terminates.
@param fp pointer to an open file.
@param buf User allocated buffer. Must have a length of BUFSIZ bytes.
*/
EXPORT_C void setbuf (FILE * fp, char *buf)
{
  (void) setvbuf (fp, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}
