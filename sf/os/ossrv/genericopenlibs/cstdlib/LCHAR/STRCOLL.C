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
* <<strcoll>>---locale specific character string compare
* INDEX
* strcoll
* ANSI_SYNOPSIS
* #include <string.h>
* int strcoll(const char *<[stra]>, const char * <[strb]>);
* TRAD_SYNOPSIS
* #include <string.h>
* int strcoll(<[stra]>, <[strb]>)
* char *<[stra]>;
* char *<[strb]>;
* <<strcoll>> compares the string pointed to by <[stra]> to
* the string pointed to by <[strb]>, using an interpretation
* appropriate to the current <<LC_COLLATE>> state.
* RETURNS
* If the first string is greater than the second string,
* <<strcoll>> returns a number greater than zero.  If the two
* strings are equivalent, <<strcoll>> returns zero.  If the first
* string is less than the second string, <<strcoll>> returns a
* number less than zero.
* PORTABILITY
* <<strcoll>> is ANSI C.
* <<strcoll>> requires no supporting OS subroutines.
* QUICKREF
* strcoll ansi pure
* 
*
*/



#include <string.h>

/**
Compares a to b character by character according to the character table 
set by current locale.
This function starts comparing the first character of each string. 
If they are equal to each other continues with the following pair
until the characters differ or until end of string is reached.
@return a value indicating the lexicographical relation between the strings
@param a Null-terminated string to compare. 
@param b Null-terminated string to compare.
*/
EXPORT_C int
strcoll (const char *a, const char *b)
{
  /* Since we don't yet support locales, this is easy! */
  return strcmp (a, b);
}
