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
* <<abort>>---abnormal termination of a program
* INDEX
* abort
* ANSI_SYNOPSIS
* #include <stdlib.h>
* void abort(void);
* TRAD_SYNOPSIS
* #include <stdlib.h>
* void abort();
* Use <<abort>> to signal that your program has detected a condition it
* cannot deal with.  Normally, <<abort>> ends your program's execution.
* Before terminating your program, <<abort>> raises the exception <<SIGABRT>>
* (using `<<raise(SIGABRT)>>').  If you have used <<signal>> to register
* an exception handler for this condition, that handler has the
* opportunity to retain control, thereby avoiding program termination.
* In this implementation, <<abort>> does not perform any stream- or
* file-related cleanup (the host environment may do so; if not, you can
* arrange for your program to do its own cleanup with a <<SIGABRT>>
* exception handler).
* RETURNS
* <<abort>> does not return to its caller.
* PORTABILITY
* ANSI C requires <<abort>>.
* Supporting OS subroutines required: <<getpid>>, <<kill>>.
* 
*
*/




#include <unistd.h>		/* for definition of _exit() */
/* #include <signal.h> */

EXPORT_C void
#ifdef EKA2
do_abort (void) _ATTRIBUTE((noreturn)) // Export in place of 'abort()' on EKA2 for binary compatibility.
#else
abort (void) _ATTRIBUTE((noreturn))
#endif
{
  for (;;)
    {
      /* raise (SIGABRT); */
      _exit (1);
    }
}

#if (defined (__ARMCC__) || defined (__X86GCC__))
void abort (void) _ATTRIBUTE((noreturn))
	{
	do_abort();
	}
#endif
