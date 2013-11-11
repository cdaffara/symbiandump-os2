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
* <<strcspn>>---count chars not in string
* INDEX
* strcspn
* ANSI_SYNOPSIS
* size_t strcspn(const char *<[s1]>, const char *<[s2]>);
* TRAD_SYNOPSIS
* size_t strcspn(<[s1]>, <[s2]>)
* char *<[s1]>;
* char *<[s2]>;
* This function computes the length of the initial part of
* the string pointed to by <[s1]> which consists entirely of
* characters <[NOT]> from the string pointed to by <[s2]>
* (excluding the terminating null character).
* RETURNS
* <<strcspn>> returns the length of the substring found.
* PORTABILITY
* <<strcspn>> is ANSI C.
* <<strcspn>> requires no supporting OS subroutines.
* 
*
*/



#include <string.h>

/**
Search string for occurrence of character set.
Scans s1 character by character, returning the number of characters read
until the first occurrence of any character included in s2.
The search includes terminating null-characters, so the function 
will return the length of s1 if none of the characters included in s2 is in s1.
@return the position in s1 of the first occurence of a component character of s2.
@param s1 Null-terminated string to be scanned. 
@param s2 Null-terminated string containing the character set to search for.
*/
EXPORT_C size_t
strcspn (const char *s1, const char *s2)
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
      if (*c)
	break;
      s1++;
    }

  return s1 - s;
}
