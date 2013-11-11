/* FPRINTF.C
 * 
 * Portions Copyright (c) 1990-1999 Nokia Corporation and/or its subsidiary(-ies).
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
#include <stdio.h>

#include <stdarg.h>

/**
Print formatted data to a stream.
Prints to the specified stream a sequence of arguments formatted 
as the format argument specifies.
@return On success, the total number of characters printed is returned.
On error, a negative number is returned.
@param fp Pointer to an open file. 
@param fmt String that contains the text to be printed.
*/
EXPORT_C int
fprintf (FILE * fp, const char *fmt,...)
{
  int ret;
  va_list ap;

  va_start (ap, fmt);
  ret = vfprintf (fp, fmt, ap);
  va_end (ap);
  return ret;
}
