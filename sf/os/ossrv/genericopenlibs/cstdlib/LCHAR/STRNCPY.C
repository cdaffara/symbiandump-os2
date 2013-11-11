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
* <<strncpy>>---counted copy string
* INDEX
* strncpy
* ANSI_SYNOPSIS
* #include <string.h>
* char *strncpy(char *<[dst]>, const char *<[src]>, size_t <[length]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char *strncpy(<[dst]>, <[src]>, <[length]>)
* char *<[dst]>;
* char *<[src]>;
* size_t <[length]>;
* <<strncpy>> copies not more than <[length]> characters from the
* the string pointed to by <[src]> (including the terminating
* null character) to the array pointed to by <[dst]>.  If the
* string pointed to by <[src]> is shorter than <[length]>
* characters, null characters are appended to the destination
* array until a total of <[length]> characters have been
* RETURNS
* This function returns the initial value of <[dst]>.
* PORTABILITY
* <<strncpy>> is ANSI C.
* <<strncpy>> requires no supporting OS subroutines.
* QUICKREF
* strncpy ansi pure
* 
*
*/



#include <string.h>

/**
Copy characters from one string to another.
Copies the first num characters of src to dest.
No null-character is implicitly appended to dest after copying process.
So dest may not be null-terminated if no null-caracters are copied from src.
If num is greater than the length of src, dest is padded with zeros until num.
@return dst is returned.
@param dst Destination string. Space allocated should be at least 
num characters long. 
@param src Null-terminated string. 
@param n Number of characters to be copied.
*/
EXPORT_C char *strncpy (char *dst, const char *src, size_t n)
	{
	char *dscan;
	const char *sscan;
	size_t count;

	dscan = dst;
	sscan = src;
   	count = n;
   	while (count > 0)
    	{
       	--count;
       	if ((*dscan++ = *sscan++) == '\0')
 		break;
     	}
   	while (count-- > 0)
    	*dscan++ = '\0';
 
   	return dst;
	}
