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
*
*/



#include <_ansi.h>
#include <stdio_r.h>

#include <stdarg.h>

/**
A reentrant version of printf().
*/
EXPORT_C int
_printf_r (struct _reent *ptr, const char *fmt, ...)
{
  int ret;
  va_list ap;

  va_start (ap, fmt);
  ret = _vfprintf_r (ptr, _stdout_r(ptr), fmt, ap);
  va_end (ap);
  return ret;
}

#ifndef _REENT_ONLY

/**
Print formatted data to stdout.
Prints to standard output a sequence of arguments formatted as the format argument specifies.
@return On success, the total number of characters printed is returned.
On error, a negative number is returned.
@param fmt String that contains the text to be printed.
Optionally it can contain format tags that are substituted 
by the values specified in subsequent argument(s) and formatted as requested.
*/
EXPORT_C int
printf (const char *fmt, ...)
{
  int ret;
  va_list ap;

  va_start (ap, fmt);
  ret = vfprintf (__stdout(), fmt, ap);
  va_end (ap);
  return ret;
}

#endif /* ! _REENT_ONLY */
