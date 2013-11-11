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
* <<strxfrm>>---transform string
* INDEX
* strxfrm
* ANSI_SYNOPSIS
* #include <string.h>
* size_t strxfrm(char *<[s1]>, const char *<[s2]>, size_t <[n]>);
* TRAD_SYNOPSIS
* #include <string.h>
* size_t strxfrm(<[s1]>, <[s2]>, <[n]>);
* char *<[s1]>;
* char *<[s2]>;
* size_t <[n]>;
* This function transforms the string pointed to by <[s2]> and
* places the resulting string into the array pointed to by
* <[s1]>. The transformation is such that if the <<strcmp>>
* function is applied to the two transformed strings, it returns
* a value greater than, equal to, or less than zero,
* correspoinding to the result of a <<strcoll>> function applied
* to the same two original strings.
* No more than <[n]> characters are placed into the resulting
* array pointed to by <[s1]>, including the terminating null
* character. If <[n]> is zero, <[s1]> may be a null pointer. If
* copying takes place between objects that overlap, the behavior
* is undefined.
* With a C locale, this function just copies.
* RETURNS
* The <<strxfrm>> function returns the length of the transformed string
* (not including the terminating null character). If the value returned
* is <[n]> or more, the contents of the array pointed to by
* <[s1]> are indeterminate.
* PORTABILITY
* <<strxfrm>> is ANSI C.
* <<strxfrm>> requires no supporting OS subroutines.
* QUICKREF
* strxfrm ansi pure
* 
*
*/



#include <string.h>

/**
Transform string using locale settings.
Copies the first num characters of src to dest performing the apropiate
transformations for the current locale settings if needed.
No null-character is implicitly appended to dest after copying process. 
So dest may not be null-terminated if no null-caracters are copied from src.
If num is greater than the length of src, dest is padded with zeros until num.
The behavor of this function is the same as strncpy but performing locale
character transformations.
@return The length of the transformed string without the null-character terminator.
@param s1 Destination string. Space allocated should be at least num characters long. 
@param s2 Null-terminated string containing string to be transformed. 
@param n Number of characters to be transformed and stored in dest.
*/
EXPORT_C size_t
strxfrm (char *s1, const char *s2, size_t n)
{
  size_t res;
  res = 0;
  while (n-- > 0 && *s2)
    {
      *s1++ = *s2++;
      res++;
    }
  while (*s2)
    {
      s2++;
      res++;
    }

  return res;
}
