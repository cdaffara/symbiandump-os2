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
* <<strchr>>---search for character in string
* INDEX
* strchr
* ANSI_SYNOPSIS
* #include <string.h>
* char * strchr(const char *<[string]>, int <[c]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char * strchr(<[string]>, <[c]>);
* char *<[string]>;
* int *<[c]>;
* This function finds the first occurence of <[c]> (converted to
* a char) in the string pointed to by <[string]> (including the
* terminating null character).
* RETURNS
* Returns a pointer to the located character, or a null pointer
* if <[c]> does not occur in <[string]>.
* PORTABILITY
* <<strchr>> is ANSI C.
* <<strchr>> requires no supporting OS subroutines.
* QUICKREF
* strchr ansi pure
* 
*
*/



#include <string.h>

/**
Find character in string.
Returns the first occurrence of i in string.
The null-terminating character is included as part of the string 
and can also be searched.
@return a pointer to the first occurrence of i in string is returned.
otherwise NULL is returned.
@param s Null-terminated string scanned in the search. 
@param i Character to be found.
*/
EXPORT_C char *
strchr (const char *s, int i)
{
	i = (char)i;

	for (;;)
	{
		int c = *s++;
		if (c == i)
			return (char *) (s - 1);
		if (c == 0)
			return NULL;
	}
}

EXPORT_C char *
index (const char *s, int i)
{
    return strchr(s, i);
}
