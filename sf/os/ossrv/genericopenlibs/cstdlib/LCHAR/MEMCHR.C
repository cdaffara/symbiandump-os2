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
* <<memchr>>---find character in memory
* INDEX
* memchr
* ANSI_SYNOPSIS
* #include <string.h>
* void *memchr(const void *<[src]>, int <[c]>, size_t <[length]>);
* TRAD_SYNOPSIS
* #include <string.h>
* void *memchr(<[src]>, <[c]>, <[length]>)
* void *<[src]>;
* void *<[c]>;
* size_t <[length]>;
* This function searches memory starting at <<*<[src]>>> for the
* character <[c]>.  The search only ends with the first
* occurrence of <[c]>, or after <[length]> characters; in
* particular, <<NULL>> does not terminate the search.
* RETURNS
* If the character <[c]> is found within <[length]> characters
* of <<*<[src]>>>, a pointer to the character is returned. If
* <[c]> is not found, then <<NULL>> is returned.
* PORTABILITY
* <<memchr>>> is ANSI C.
* <<memchr>>  requires no supporting OS subroutines.
* QUICKREF
* memchr ansi pure
* 
*
*/



#include <_ansi.h>
#include <string.h>

/**
Search buffer for a character.
Searches the first num bytes of memory block pointed by src_void for character c.
@return   A pointer to the first occurrence of c in buffer.
If character is not found the function returns NULL.
@param src_void Pointer to buffer. 
@param c Key character to look for. 
@param length Number of characters to check from buffer.
*/
EXPORT_C void *
memchr (const void* src_void, int c, size_t length)
{
  const unsigned char *src = (const unsigned char *) src_void;

  while (length--)
    {
      if (*src == c)
	return (char *) src;
      src++;
    }
  return NULL;
}
