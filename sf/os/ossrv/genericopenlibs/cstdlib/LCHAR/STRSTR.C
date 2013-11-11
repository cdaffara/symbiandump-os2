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
* <<strstr>>---find string segment
* INDEX
* strstr
* ANSI_SYNOPSIS
* #include <string.h>
* char *strstr(const char *<[s1]>, const char *<[s2]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char *strstr(<[s1]>, <[s2]>)
* char *<[s1]>;
* char *<[s2]>;
* Locates the first occurence in the string pointed to by <[s1]> of
* the sequence of characters in the string pointed to by <[s2]>
* (excluding the terminating null  character).
* RETURNS
* Returns a pointer to the located string segment, or a null
* pointer if the string <[s2]> is not found. If <[s2]> points to
* a string with zero length, the <[s1]> is returned.
* PORTABILITY
* <<strstr>> is ANSI C.
* <<strstr>> requires no supporting OS subroutines.
* QUICKREF
* strstr ansi pure
* 
*
*/



#include <string.h>

/**
Find substring.
Scans string1 for the first occurrence of string2.
The search does not include terminating null-characters.
@return A pointer to the first occurrence of lookfor in searchee.
If lookfor is not found in searchee the function returns NULL.
@param searchee Null-terminated string to search. 
@param lookfor Null-terminated string containing the substring to search for.
*/
EXPORT_C char *
strstr (const char *searchee, const char *lookfor)
{
  if (*searchee == 0)
    {
      if (*lookfor)
	return (char *) NULL;
      return (char *) searchee;
    }

  while (*searchee)
    {
      size_t i;
      i = 0;

      for (;;)
	{
	  if (lookfor[i] == 0)
	    {
	      return (char *) searchee;
	    }

	  if (lookfor[i] != searchee[i])
	    {
	      break;
	    }
	  i++;
	}
      searchee++;
    }

  return (char *) NULL;
}
