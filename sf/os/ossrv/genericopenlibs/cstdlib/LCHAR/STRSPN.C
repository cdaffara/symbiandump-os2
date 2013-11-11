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
* <<strspn>>---find initial match
* INDEX
* strspn
* ANSI_SYNOPSIS
* #include <string.h>
* size_t strspn(const char *<[s1]>, const char *<[s2]>);
* TRAD_SYNOPSIS
* #include <string.h>
* size_t strspn(<[s1]>, <[s2]>)
* char *<[s1]>;
* char *<[s2]>;
* This function computes the length of the initial segment of
* the string pointed to by <[s1]> which consists entirely of
* characters from the string pointed to by <[s2]> (excluding the
* terminating null character).
* RETURNS
* <<strspn>> returns the length of the segment found.
* PORTABILITY
* <<strspn>> is ANSI C.
* <<strspn>> requires no supporting OS subroutines.
* QUICKREF
* strspn ansi pure
* 
*
*/



#include <string.h>

/**
Get length of substring composed of given characters.
Scans s1 character by character, returning the number of characters read before 
the first character not included in s2 is found.
The search does not include terminating null-characters.
@return the length of the initial substring of s1 
that is only composed of characters included in s2.
@param s1 Null-terminated string to be scanned. 
@param s2 Null-terminated string containing character set.
*/
EXPORT_C size_t
strspn (const char *s1, const char *s2)
{
  const char *s = s1;
  const char *c;

  while (*s1)
    {
      for (c = s2; *c; c++)
	{
	  if (*s1 == *c)
	    break;
	}
      if (*c == '\0')
	break;
      s1++;
    }

  return s1 - s;
}
