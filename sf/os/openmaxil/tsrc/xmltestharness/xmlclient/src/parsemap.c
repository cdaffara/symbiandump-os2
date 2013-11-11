/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "parsemap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* function that searches untyped parseMap_XXX tables. also performs literal integer parsing. */
PARSEMAP_LINKAGE int ParseMapSearch(const char* text, int* result, ParseMapEntry* entries, int numEntries)
{
	int textlen = strlen(text);
	int i;
	char* endptr = 0;
	
	/* search the parse map for the matching string */
	for(i = 0; i < numEntries; i++) {
		if(entries[i].length == textlen && strcasecmp(text, entries[i].text) == 0) {
			*result = entries[i].value;
			return 1;
		}
	}
	/* match not found */
	
	/* try to parse string as an integer */
	errno = 0;
	i = strtol(text, &endptr, 0);
	if(errno == ERANGE)
		{
		errno = 0;
		i = (int) strtoul(text, &endptr, 0);
		if(errno == ERANGE)
			{
			return 0;
			}
		}
	if(endptr == (text + textlen))  /* all of string must be part of number */
		{
		*result = i;
		return 1;
		}
	
	/* fail */
	return 0;
}

PARSEMAP_LINKAGE const char* ParseMapSearchReverse(int value, char not_found_buf[11], ParseMapEntry* entries, int numEntries) {
	int i;
	
	/* search the parse map for the matching value */
	for(i = 0; i < numEntries; i++) {
		if(entries[i].value == value) {
			return entries[i].text;
		}
	}
	
	/* value not found in table, format hex string */
	sprintf(not_found_buf, "0x%08X", value);
	return not_found_buf;
}
