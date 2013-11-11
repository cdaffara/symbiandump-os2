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
* <<strcmp>>---character string compare
* INDEX
* strcmp
* ANSI_SYNOPSIS
* #include <string.h>
* int strcmp(const char *<[a]>, const char *<[b]>);
* TRAD_SYNOPSIS
* #include <string.h>
* int strcmp(<[a]>, <[b]>)
* char *<[a]>;
* char *<[b]>;
* <<strcmp>> compares the string at <[a]> to
* the string at <[b]>.
* RETURNS
* If <<*<[a]>>> sorts lexicographically after <<*<[b]>>>,
* <<strcmp>> returns a number greater than zero.  If the two
* strings match, <<strcmp>> returns zero.  If <<*<[a]>>>
* sorts lexicographically before <<*<[b]>>>, <<strcmp>> returns a
* number less than zero.
* PORTABILITY
* <<strcmp>> is ANSI C.
* <<strcmp>> requires no supporting OS subroutines.
* QUICKREF
* strcmp ansi pure
* 
*
*/



#include <string.h>

/**
Compare two strings.
Compares string1 to s1 character by character.
This function starts comparing the first character of each string.
If they are equal to each other continues with the following pair
until the characters differ or until end of string is reached.
@return a value indicating the lexicographical relation between the strings
@param s1 Null-terminated string to compare. 
@param s2 Null-terminated string to compare.
*/
EXPORT_C int
strcmp (const char *s1, const char *s2)
{
	const unsigned char* p1=(const unsigned char*)s1;
	const unsigned char* p2=(const unsigned char*)s2;
	for (;;)
	{
		int c1=*p1++;
		int d=c1-*p2++;
		if (d!=0)
			return d;
		if (c1==0)
			return d;
	}
}

/**
Compare the wide-character string pointed to by s1 to the wide-character
string pointed to by s2.
@return an integer greater than, equal to, or  less than zero, if the 
wide-character string pointed to by s1 is greater than, equal to,  
or less than the wide-character string pointed to by s2.
*/
EXPORT_C int wcscmp (const wchar_t *s1, const wchar_t *s2)
{
	for (;;)
	{
		int c1=*s1++;
		int d=c1-*s2++;
		if (d!=0)
			return d;
		if (c1==0)
			return d;
	}
}
