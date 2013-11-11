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
* <<strpbrk>>---find chars in string
* INDEX
* strpbrk
* ANSI_SYNOPSIS
* #include <string.h>
* char *strpbrk(const char *<[s1]>, const char *<[s2]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char *strpbrk(<[s1]>, <[s2]>)
* char *<[s1]>;
* char *<[s2]>;
* This function locates the first occurence in the string
* pointed to by <[s1]> of any character in string pointed to by
* <[s2]> (excluding the terminating null character).
* RETURNS
* <<strpbrk>> returns a pointer to the character found in <[s1]>, or a
* null pointer if no character from <[s2]> occurs in <[s1]>.
* PORTABILITY
* <<strpbrk>> requires no supporting OS subroutines.
* 
*
*/



#include <string.h>

/**
Scan string for specified characters.
Scans string1 character by character, returning a pointer to the first character
that matches with any of the characters in string2.
The search does not includes the terminating null-characters.
@return A pointer to the first appearance in string1 of a character specified in s2.
If none of the characters specified in s2 exists in s1, a NULL pointer is returned.
@param s1 Null-terminated string to be scanned. 
@param s2 Null-terminated string containing the character set to search for.
*/
EXPORT_C char *
strpbrk (const char *s1, const char *s2)
{
  const char *c = s2;
  if (!*s1)
    return (char *) NULL;

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

  if (*c == '\0')
    s1 = NULL;

  return (char *) s1;
}
