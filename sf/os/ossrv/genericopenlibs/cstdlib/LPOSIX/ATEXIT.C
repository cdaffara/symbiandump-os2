/* ATEXIT.C
 * 
 * Portions Copyright (c) 1990-2006 Nokia Corporation and/or its subsidiary(-ies).
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
<<atexit>>---request execution of functions at program exit

INDEX
	atexit

ANSI_SYNOPSIS
	#include <stdlib.h>
	int atexit(void (*<[function]>)(void);

TRAD_SYNOPSIS
	#include <stdlib.h>
	int atexit((<[function]>)
	void (*<[function]>)();

DESCRIPTION
You can use <<atexit>> to enroll functions in a list of functions that
will be called when your program terminates normally.  The argument is
a pointer to a user-defined function (which must not require arguments and
must not return a result).

The functions are kept in a LIFO stack; that is, the last function
enrolled by <<atexit>> will be the first to execute when your program
exits.

There is no built-in limit to the number of functions you can enroll
in this list; however, after every group of 32 functions is enrolled,
<<atexit>> will call <<malloc>> to get space for the next part of the
list.   The initial list of 32 functions is statically allocated, so
you can always count on at least that many slots available.

RETURNS
<<atexit>> returns <<0>> if it succeeds in enrolling your function,
<<-1>> if it fails (possible only if no space was available for
<<malloc>> to extend the list of functions or the library globals could not
be allocated).

PORTABILITY
<<atexit>> is required by the ANSI standard, which also specifies that
implementations must support enrolling at least 32 functions.

Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
<<lseek>>, <<read>>, <<sbrk>>, <<write>>.
*/

#include <stddef.h>
#include <stdlib.h>
#include <reent.h>

/*
 * Register a function to be performed at exit.
 * NB. MSVC 5.0 has a built-in prototype for atexit, so we have to use a #define
 * to map the name atexit to _epoc32_atexit
 */

/*
@return On Success, returns 0.
		On Failure, returns -1.
*/

EXPORT_C int
_epoc32_atexit (void (*fn)(void))
{
  register struct _atexit *p;
  struct _reent *rp = _REENT2;
  if (!rp)
	return -1; // Memory for library globals is not allocated (errno not set). 
 
  if ((p = rp->_atexit) == NULL)
    rp->_atexit = p = &rp->_atexit0;
  if (p->_ind >= _ATEXIT_SIZE)
    {
      if ((p = (struct _atexit *) malloc (sizeof *p)) == NULL)
		return -1;
      p->_ind = 0;
      p->_next = rp->_atexit;
      rp->_atexit = p;
    }
  p->_fns[p->_ind++] = fn;
  return 0;
}

EXPORT_C void 
_atexit_processing_r (struct _reent *rp)
{
  register struct _atexit *p;
  register int n;

  for (p = rp->_atexit; p; p = p->_next)
    for (n = p->_ind; --n >= 0;)
      (*p->_fns[n]) ();
  if (rp->__cleanup)
    (*rp->__cleanup) (rp);
}
