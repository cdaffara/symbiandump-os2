/* EXIT.C
 * 
 * Portions Copyright (c) 1990-1999 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */

/*
 * Copyright (c) 1990 Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 */

/*
FUNCTION
<<exit>>---end program execution

INDEX
	exit

ANSI_SYNOPSIS
	#include <stdlib.h>
	void exit(int <[code]>);

TRAD_SYNOPSIS
	#include <stdlib.h>
	void exit(<[code]>)
	int <[code]>;

DESCRIPTION
Use <<exit>> to return control from a program to the host operating
environment.  Use the argument <[code]> to pass an exit status to the
operating environment: two particular values, <<EXIT_SUCCESS>> and
<<EXIT_FAILURE>>, are defined in `<<stdlib.h>>' to indicate success or
failure in a portable fashion.

<<exit>> does two kinds of cleanup before ending execution of your
program.  First, it calls all application-defined cleanup functions
you have enrolled with <<atexit>>.  Second, files and streams are
cleaned up: any pending output is delivered to the host system, each
open file or stream is closed, and files created by <<tmpfile>> are
deleted.

RETURNS
<<exit>> does not return to its caller.

PORTABILITY
ANSI C requires <<exit>>, and specifies that <<EXIT_SUCCESS>> and
<<EXIT_FAILURE>> must be defined.

Supporting OS subroutines required: <<_exit>>.
*/

#include <stdlib.h>
#include <unistd.h>	/* for _exit() declaration */
#include <reent.h>
#include <stdlib_r.h>	/* for _atexit_processing() */

#ifndef _REENT_ONLY

/* GCC knows that exit() should not return, but prior to GCC 2.5 there is
 * no way of declaring the function as __attribute__ ((noreturn)).
 * Expect to be warned about this function in MARM builds.
 */
EXPORT_C void 
exit (int code) _ATTRIBUTE((noreturn))
{
  _atexit_processing_r(_REENT);
  _exit (code);

  /* GCC may insert a call to abort() here. */
}

#endif
