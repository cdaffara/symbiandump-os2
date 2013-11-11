/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


/*****************************************************************
**  File: parseI.h
**
**  Description: definitions internal to WBXML parser component 
*****************************************************************/

#ifndef NW_PARSER_WBXML_PARSE_I_H
#define NW_PARSER_WBXML_PARSE_I_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_wbxml_parse.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/*
 * Parser states, shared by reader and parser
 */

#define NW_WBXML_PARSER_S_MASK                0x0F
#define NW_WBXML_PARSER_S_START               0x00
#define NW_WBXML_PARSER_S_HEADER              0x01
#define NW_WBXML_REGISTRY_INIT                0x10


/* 
 * Parser internal status codes, shared by reader and parser 
 */

#define NW_WBXML_PARSER_OK                   0x0
#define NW_WBXML_PARSER_NOMEMORY             0x1
#define NW_WBXML_PARSER_BYTECODEERROR        0x2
#define NW_WBXML_PARSER_CHARSET_UNSUPPORTED  0x4
#define NW_WBXML_PARSER_UNKNOWN_ERROR        0x8

/* Parse exceptions */

#define NW_WBXML_EXCEPTION_MEMORY               1
#define NW_WBXML_EXCEPTION_BYTECODE             2

/* 
* WBXML token components 
*
* The wbxml parser stores tokens as 32 bits. The meaning
* of each token is determined as a 4-tuple:
* the token itself (8 bits), the code page (8 bits),
* the dictionary in which it is defined (for which we
* allocate 15 bits) and a state flag that indicates whether
* the token is in attribute or tag code space.
* 
* Within the lowest 8 bits (the token component) the
* wbxml spec defines further subsets of bits for tags
* and attributes. The parser itself only int32erprets 
* the lowest 8 bits. The remaining bits are used for
* token value lookups in dictionaries.
*/

/* TODO:  export some of these definitions? */

#define NW_WBXML_MASK_TOKEN          0x000000FF
#define NW_WBXML_MASK_CODEPAGE       0x0000FF00
#define NW_WBXML_MASK_DICTIONARY     0x7FFF0000
#define NW_WBXML_MASK_CPSTATE        0x80000000

#define NW_WBXML_CP_STATE_TAG        0x00000000
#define NW_WBXML_CP_STATE_ATTR       0x80000000

void
NW_WBXML_CPRegistry_getCodePages(NW_WBXML_CP_Registry_t* registry,
                              NW_Uint32 offset,
                              NW_Uint8 *tag_code_page,
                              NW_Uint8 *attribute_code_page);
                              
NW_Status_t 
NW_WBXML_Parser_docHeaderParse(NW_WBXML_Parser_t * parser, 
                        NW_WBXML_Document_t * doc);
  
NW_Status_t 
NW_WBXML_Parser_bodyParse(NW_WBXML_Parser_t * parser);

void 
NW_WBXML_Parser_bytecodeError (NW_WBXML_Parser_t * parser);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_PARSER_WBXML_PARSE_I_H */
