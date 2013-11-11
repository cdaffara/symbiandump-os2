/* GETC.C
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
<<getc>>---read a character (macro)

INDEX
	getc

ANSI_SYNOPSIS
	#include <stdio.h>
	int getc(FILE *<[fp]>);

TRAD_SYNOPSIS
	#include <stdio.h>
	int getc(<[fp]>)
	FILE *<[fp]>;

DESCRIPTION
<<getc>> is a macro, defined in <<stdio.h>>.  You can use <<getc>>
to get the next single character from the file or stream
identified by <[fp]>.  As a side effect, <<getc>> advances the file's
current position indicator.

For a subroutine version of this macro, see <<fgetc>>.

RETURNS
The next character (read as an <<unsigned char>>, and cast to
<<int>>), unless there is no more data, or the host system reports a
read error; in either of these situations, <<getc>> returns <<EOF>>.

You can distinguish the two situations that cause an <<EOF>> result by
using the <<ferror>> and <<feof>> functions.

PORTABILITY
ANSI C requires <<getc>>; it suggests, but does not require, that
<<getc>> be implemented as a macro.  The standard explicitly permits
macro implementations of <<getc>> to use the argument more than once;
therefore, in a portable program, you should not use an expression
with side effects as the <<getc>> argument.

Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
<<lseek>>, <<read>>, <<sbrk>>, <<write>>.
*/

#include <stdio.h>
#include "LOCAL.H"

/*
 * A subroutine version of the macro getc.
 */

#undef getc

/**
Get the next character.
Returns the next character of the stream and increases the file pointer
to point to the next character.
@return The character read is returned as an int value.
If the End Of File has been reached or there has been an error reading, 
the function returns EOF.
@param fp pointer to an open file.
*/
EXPORT_C int
getc (register FILE *fp)
{
  /* CHECK_INIT is called (eventually) by __srefill.  */

  return __sgetc (fp);
}
