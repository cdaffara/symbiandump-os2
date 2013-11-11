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
* <<strcpy>>---copy string
* INDEX
* strcpy
* ANSI_SYNOPSIS
* #include <string.h>
* char *strcpy(char *<[dst]>, const char *<[src]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char *strcpy(<[dst]>, <[src]>)
* char *<[dst]>;
* char *<[src]>;
* <<strcpy>> copies the string pointed to by <[src]>
* (including the terminating null character) to the array
* pointed to by <[dst]>.
* RETURNS
* This function returns the initial value of <[dst]>.
* PORTABILITY
* <<strcpy>> is ANSI C.
* <<strcpy>> requires no supporting OS subroutines.
* QUICKREF
* strcpy ansi pure
* 
*
*/



#include <string.h>
#include <stdlib.h>

/*SUPPRESS 560*/
/*SUPPRESS 530*/

/**
Copy string.
Copies the content pointed by src to dest stopping after the terminating null-character is copied.
dest should have enough memory space allocated to contain src string.
@return des is returned
@param s1 Destination string. Should be enough long to contain s2. 
@param s2 Null-terminated string to copy.
*/
EXPORT_C char *
strcpy (char *s1, const char *s2)
	{
	char *s = s1;
   	while ((*s1++ = *s2++)!=0);
   	return s;
	}

/**
Copy  the  wide-character string  pointed to by  s2 (including the terminating null
wide-character code) into the array pointed to  by  s1.
@return s1
@param s1 wide-character string
@param s2 wide-character string
*/
EXPORT_C wchar_t * wcscpy (wchar_t *s1, const wchar_t *s2)
{
  wchar_t *s = s1;

  while ((*s1++ = *s2++)!=L'\0')
    ;

  return s;
}

/**
Function shall return a pointer to a new string,
which is a duplicate of the string pointed to by str.
@return a pointer to a new string
@param str string
*/
EXPORT_C char * strdup (const char *str)
{
  size_t len = strlen (str) + 1;
  char *copy = (char *)malloc (len);
  if (copy)
    {
      memcpy (copy, str, len);
    }
  return copy;
}

EXPORT_C wchar_t * wcsdup (const wchar_t *str)
{
  size_t len = wcslen(str) + 1;
  wchar_t *copy;
  
  len *= sizeof(wchar_t);
  copy = (wchar_t *)malloc (len);
  if (copy)
    {
      memcpy (copy, str, len);
    }
  return copy;
}
