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
* <<strncmp>>---character string compare
* INDEX
* strncmp
* ANSI_SYNOPSIS
* #include <string.h>
* int strncmp(const char *<[a]>, const char * <[b]>, size_t <[length]>);
* TRAD_SYNOPSIS
* #include <string.h>
* int strncmp(<[a]>, <[b]>, <[length]>)
* char *<[a]>;
* char *<[b]>;
* size_t <[length]>
* <<strncmp>> compares up to <[length]> characters
* from the string at <[a]> to the string at <[b]>.
* RETURNS
* If <<*<[a]>>> sorts lexicographically after <<*<[b]>>>,
* <<strncmp>> returns a number greater than zero.  If the two
* strings are equivalent, <<strncmp>> returns zero.  If <<*<[a]>>>
* sorts lexicographically before <<*<[b]>>>, <<strncmp>> returns a
* number less than zero.
* PORTABILITY
* <<strncmp>> is ANSI C.
* <<strncmp>> requires no supporting OS subroutines.
* QUICKREF
* strncmp ansi pure
* 
*
*/



#include <string.h>

/**
Compare some characters of two strings.
Compares the first num characters of string1 to the first num characters of string2.
The comparison is performed character by character. 
If a character that is not equal in both strings is found the function ends 
and returns a value that determines which of them was greater.
@return a value indicating the lexicographical relation between the strings.
@param s1 Null-terminated string to compare 
@param s2 Null-terminated string to compare. 
@param n Maximum number of characters to compare.
*/
EXPORT_C int 
strncmp (const char *s1, const char *s2, size_t n)
{
	const unsigned char* p1 = (const unsigned char*)s1;
	const unsigned char* p2 = (const unsigned char*)s2;

	if (n == 0)
		return 0;

	for (;;)
	{
		int c1 = *p1++;
		int d = c1 - *p2++;
		if (d != 0)
			return d;
		if (c1 == 0)
			return d;
		if (--n == 0)
			return d;
	}
}
