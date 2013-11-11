/* FERROR.C
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
<<ferror>>---test whether read/write error has occurred

INDEX
	ferror

ANSI_SYNOPSIS
	#include <stdio.h>
	int ferror(FILE *<[fp]>);

TRAD_SYNOPSIS
	#include <stdio.h>
	int ferror(<[fp]>)
	FILE *<[fp]>;

DESCRIPTION
The <<stdio>> functions maintain an error indicator with each file
pointer <[fp]>, to record whether any read or write errors have
occurred on the associated file or stream.
Use <<ferror>> to query this indicator.

See <<clearerr>> to reset the error indicator.

RETURNS
<<ferror>> returns <<0>> if no errors have occurred; it returns a
nonzero value otherwise.

PORTABILITY
ANSI C requires <<ferror>>.

No supporting OS subroutines are required.
*/

#include <stdio.h>
#include "LOCAL.H"

#undef ferror
/**
Check for errors.
@return   If there were no errors a 0 value is returned.
Otherwise a non-zero value is returned .
*/
EXPORT_C int
ferror (FILE * fp)
{
  return __sferror (fp);
}
