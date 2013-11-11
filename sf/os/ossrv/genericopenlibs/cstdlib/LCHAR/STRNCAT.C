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
* <<strncat>>---concatenate strings
* INDEX
* strncat
* ANSI_SYNOPSIS
* #include <string.h>
* char *strncat(char *<[dst]>, const char *<[src]>, size_t <[length]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char *strncat(<[dst]>, <[src]>, <[length]>)
* char *<[dst]>;
* char *<[src]>;
* size_t <[length]>;
* <<strncat>> appends not more than <[length]> characters from
* the string pointed to by <[src]> (including the	terminating
* null character) to the end of the string pointed to by
* <[dst]>.  The initial character of <[src]> overwrites the null
* character at the end of <[dst]>.  A terminating null character
* is always appended to the result
* WARNINGS
* Note that a null is always appended, so that if the copy is
* limited by the <[length]> argument, the number of characters
* appended to <[dst]> is <<n + 1>>.
* RETURNS
* This function returns the initial value of <[dst]>
* PORTABILITY
* <<strncat>> is ANSI C.
* <<strncat>> requires no supporting OS subroutines.
* QUICKREF
* strncat ansi pure
* 
*
*/



#include <string.h>

/**
Append substring to string.
Appends num characters of src string to dest string. 
If the terminating null-character appears in src string 
before num character have been appended, the function appends 
the null-character to dest and ends.
The terminating null character in dest is overwritten by the first character of src.
The resulting string includes a null-character at end.
@return s1 is returned
@param s1 Pointer to a null-terminated string with enough space allocated to contain src
plus num characters. 
@param s2 Null-terminated string containing characters to be appended. 
@param n Number of characters to be appended from src to dest.
*/
EXPORT_C char *
strncat (char *s1, const char *s2, size_t n)
{
  char *s = s1;

  while (*s1)
    s1++;
  while (n-- != 0 && (*s1++ = *s2++)!=0)
    {
      if (n == 0)
	*s1 = '\0';
    }

  return s;
}
