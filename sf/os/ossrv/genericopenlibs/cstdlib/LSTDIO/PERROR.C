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
* <<perror>>---print an error message on standard error
* INDEX
* perror
* INDEX
* _perror_r
* ANSI_SYNOPSIS
* #include <stdio.h>
* void perror(char *<[prefix]>);
* void _perror_r(void *<[reent]>, char *<[prefix]>);
* TRAD_SYNOPSIS
* #include <stdio.h>
* void perror(<[prefix]>)
* char *<[prefix]>;
* void _perror_r(<[reent]>, <[prefix]>)
* char *<[reent]>;
* char *<[prefix]>;
* Use <<perror>> to print (on standard error) an error message
* corresponding to the current value of the global variable <<errno>>.
* Unless you use <<NULL>> as the value of the argument <[prefix]>, the
* error message will begin with the string at <[prefix]>, followed by a
* colon and a space (<<: >>). The remainder of the error message is one
* of the strings described for <<strerror>>.
* The alternate function <<_perror_r>> is a reentrant version.  The
* extra argument <[reent]> is a pointer to a reentrancy structure.
* RETURNS
* <<perror>> returns no result.
* PORTABILITY
* ANSI C requires <<perror>>, but the strings issued vary from one
* implementation to another.
* Supporting OS subroutines required: <<close>>, <<fstat>>, <<isatty>>,
* <<lseek>>, <<read>>, <<sbrk>>, <<write>>.
* 
*
*/



#include <stddef.h>
#include <stdio_r.h>
#include <string.h>

/**
A reentrant version of perror().
*/
EXPORT_C void
_perror_r (struct _reent *ptr, const char *s)
{
  char *error;
  FILE *efp = _stderr_r(ptr);

  if (s != NULL && *s != '\0')
    {
      fputs (s, efp);
      fputs (": ", efp);
    }

  error = strerror (ptr->_errno);
  if (*error == '\0')
      fprintf (efp, "error %d\n", ptr->_errno);
  else
    {
      fputs (error, efp);
      fputc ('\n', efp);
    }
}

#ifndef _REENT_ONLY

EXPORT_C void
perror (const char *s)
{
  _perror_r (_REENT, s);
}

#endif
