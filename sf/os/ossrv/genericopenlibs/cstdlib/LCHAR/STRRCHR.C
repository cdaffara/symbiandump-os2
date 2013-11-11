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
* <<strrchr>>---reverse search for character in string
* INDEX
* strrchr
* ANSI_SYNOPSIS
* #include <string.h>
* char * strrchr(const char *<[string]>, int <[c]>);
* TRAD_SYNOPSIS
* #include <string.h>
* char * strrchr(<[string]>, <[c]>);
* char *<[string]>;
* int *<[c]>;
* This function finds the last occurence of <[c]> (converted to
* a char) in the string pointed to by <[string]> (including the
* terminating null character).
* RETURNS
* Returns a pointer to the located character, or a null pointer
* if <[c]> does not occur in <[string]>.
* PORTABILITY
* <<strrchr>> is ANSI C.
* <<strrchr>> requires no supporting OS subroutines.
* QUICKREF
* strrchr ansi pure
* 
*
*/



#include <string.h>

/**
Find last occurrence of character in string.
Returns the last occurrence of c in string.
The null-terminating character is included as part of the string and can also be searched.
@return If character is found, a pointer to the last occurrence of c in string is returned.
If not, NULL is returned.
@param s Null-terminated string scanned in the search. 
@param i Character to be found.
*/
EXPORT_C char *
strrchr (const char *s, int i)
{
	const char *last = NULL;
	i = (char)i;

	for (;;)
	{
		int c = *s++;
		if (c == i)
			last = s - 1;
		if (c == 0)
			break;
	}

  return (char *) last;
}

EXPORT_C char *
rindex (const char *s, int c)
{
  return strrchr (s, c);
}
