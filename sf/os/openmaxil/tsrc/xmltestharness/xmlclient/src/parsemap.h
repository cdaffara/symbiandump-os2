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


#ifndef PARSEMAP_H_
#define PARSEMAP_H_

/* define PARSEMAP_LINKAGE to prepend extern "C" in front of symbols if using a C++ compiler */
#ifdef __cplusplus
#define PARSEMAP_LINKAGE extern "C"
#else
#define PARSEMAP_LINKAGE
#endif

/* the structure used in the parseMap_XXX tables */
typedef struct
{
	int value;              /* numeric value */
	const char* text;       /* textual equivalent of value */
	int length;             /* length of text. avoids heavy use of strlen */
} ParseMapEntry;

/* function that searches untyped parseMap_XXX tables. also performs literal integer parsing. */
PARSEMAP_LINKAGE int ParseMapSearch(const char* text, int* result, ParseMapEntry* entries, int numEntries);
PARSEMAP_LINKAGE const char* ParseMapSearchReverse(int value, char not_found_buf[11], ParseMapEntry* entries, int numEntries);

/* macro to declare a parser function of type T */
#define DECL_PARSETYPE(T) \
PARSEMAP_LINKAGE int parse_##T(const char* text, T* result); \
PARSEMAP_LINKAGE const char* format_##T(T value, char not_found_buf[11])

/* macro to begin a parseMap definition of type T */
#define PARSE_MAP_START(T) \
const ParseMapEntry parseMap_##T[] = {

/* macro to add a ParseMapEntry struct to a parseMap table. preprocessor is used to generate text and length values. */
#define PARSE_MAP_ENTRY(x) \
{ x, #x, sizeof(#x)-1 }

/* macro to create a ParseMapEntry where the text is not identical to the C identifier. */
#define PARSE_MAP_ALIAS(val, alias) \
{ val, alias, sizeof(alias)-1 }

#define PARSE_MAP_PREFIXENTRY(prefix, tail) \
PARSE_MAP_ENTRY(prefix##tail), \
PARSE_MAP_ALIAS(prefix##tail, #tail)

/* macro to end a parseMap definition. also defines the typed parser function */
#define PARSE_MAP_END(T) \
}; \
\
PARSEMAP_LINKAGE int parse_##T(const char* text, T* result) \
	{ \
	  return ParseMapSearch(text, (int*) result, (ParseMapEntry*) parseMap_##T, sizeof(parseMap_##T) / sizeof(ParseMapEntry)); \
	} \
\
PARSEMAP_LINKAGE const char* format_##T(T value, char not_found_buf[11]) \
	{ \
	  return ParseMapSearchReverse((int) value, not_found_buf, (ParseMapEntry*) parseMap_##T, sizeof(parseMap_##T) / sizeof(ParseMapEntry)); \
	} \
	

#endif /* PARSEMAP_H_ */
