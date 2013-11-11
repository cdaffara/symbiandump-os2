/* SCANF.C
 * 
 * Portions Copyright (c) 1990-2006 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */

/*
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <_ansi.h>
#include <stdio_r.h>
#include "LOCAL.H"
#include <stdarg.h>

/**
Read formatted data from standard input.
Reads data from the standard input and stores it into the locations given by argument(s). 

@param fmt String that can contain one or more of these items:
	   Whitespace characters: the function will read and ignore any whitespace characters
	   (this includes blank, newline and tab characters) encountered before the next
	   non-whitespace character. This includes any quantity of whitespace characters
	   (including none). Non-whitespace characters (any character not including blank,
	   newline, tab, or any format specifier begining with % character): this cause that
	   the function read and discard any character that match the given non-whitespace
	   character. 
	   If this character is not found the function ends returning error.

@return On Success, the number of items succesfully read. 
		On Failure, EOF is returned and errno may be set.
*/
EXPORT_C int scanf (const char *fmt, ...)
{
  int ret;
  va_list ap;
  struct _reent *r = _REENT2;
  if (!r)
	return EOF; // Memory for library globals is not allocated (errno not set). 
  va_start (ap, fmt);
  ret = __svfscanf (_stdin_r (r), fmt, ap);
  va_end (ap);
  return ret;
}

/**
A reentrant version of scanf().
*/
EXPORT_C  int _scanf_r (struct _reent *ptr, const char *fmt, ...)
{
  int ret;
  va_list ap;

  va_start (ap, fmt);
  ret = __svfscanf (_stdin_r (ptr), fmt, ap);
  va_end (ap);
  return (ret);
}

/**
Read formatted data from a stream.
Reads data from the current position of stream and stores it 
into the locations given by argument(s).
Locations pointed by each argument are filled with their corresponding type of value 
requested in the format string.
There must be the same number of type specifiers in format string than arguments passed.
@return   The number of items succesfully read.
@param fp Pointer to an open file. 
@param fmt String that can contain one or more of these item:
Whitespace characters: the function will read and ignore any whitespace characters 
(this includes blank, newline and tab characters)
encountered before the next non-whitespace character.
This includes any quantity of whitespace characters (including none).
Non-whitespace characters (any character not including blank, newline, tab,
or any format specifier begining with % character): 
this cause that the function read and discard any character that match the given non-whitespace character.
If this character is not found the function ends returning error.
*/
EXPORT_C int
fscanf (FILE * fp, const char *fmt, ...)
{
  int ret;
  va_list ap;

  va_start (ap, fmt);
  ret = __svfscanf (fp, fmt, ap);
  va_end (ap);
  return ret;
}
