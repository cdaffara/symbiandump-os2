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
* <<strcat>>---concatenate strings
* INDEX
* strcat
* ANSI_SYNOPSIS
* #include <string.h>
* char *strcat(char *<[dst]>, const char *<[src]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char *strcat(<[dst]>, <[src]>)
* char *<[dst]>;
* char *<[src]>;
* <<strcat>> appends a copy of the string pointed to by <[src]>
* (including the terminating null character) to the end of the
* string pointed to by <[dst]>.  The initial character of
* <[src]> overwrites the null character at the end of <[dst]>.
* RETURNS
* This function returns the initial value of <[dst]>
* PORTABILITY
* <<strcat>> is ANSI C.
* <<strcat>> requires no supporting OS subroutines.
* QUICKREF
* strcat ansi pure
* 
*
*/



#include <string.h>

/*SUPPRESS 560*/
/*SUPPRESS 530*/

/**
Append string.
Appends s2 string to s1 string. 
The terminating null character in s1 is overwritten by the first character of s2.
The resulting string includes a null-character at end.
@return s1.
@param s1 Pointer to a null-terminated string with enough space allocated to 
contain both s2 and s1. 
@param s2 Null-terminated string to append.
*/
EXPORT_C char * strcat (char *s1, const char *s2)
{
  char *s = s1;

  while (*s1)
    s1++;

  while ((*s1++ = *s2++)!=0)
    ;
  return s;
}

/**
Append a copy of the wide-character string pointed to by s2
@return s1
@param s1 wide-character
@param s2 wide-character
*/
EXPORT_C wchar_t * wcscat (wchar_t *s1, const wchar_t *s2)
{
  wchar_t *s = s1;

  while (*s1)
    s1++;

  while ((*s1++ = *s2++)!=L'\0')
    ;
  return s;
}
