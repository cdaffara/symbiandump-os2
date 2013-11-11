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


/** ----------------------------------------------------------------------- **
    @package:     NW_WBXML

    @synopsis:    default

    @description: Wbxml token definitions

 ** ----------------------------------------------------------------------- **/

#ifndef NW_PARSER_WBXML_TOKEN_H
#define NW_PARSER_WBXML_TOKEN_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_string_string.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*
 * WBXML global tokens, section 7
 */

#define NW_WBXML_SWITCH_PAGE            0x00
#define NW_WBXML_END                    0x01
#define NW_WBXML_ENTITY                 0x02
#define NW_WBXML_STR_I                  0x03
#define NW_WBXML_LITERAL                0x04

#define NW_WBXML_EXT_I_0                0x40
#define NW_WBXML_EXT_I_1                0x41
#define NW_WBXML_EXT_I_2                0x42
#define NW_WBXML_PI                     0x43
#define NW_WBXML_LITERAL_C              0x44

#define NW_WBXML_EXT_T_0                0x80
#define NW_WBXML_EXT_T_1                0x81
#define NW_WBXML_EXT_T_2                0x82
#define NW_WBXML_STR_T                  0x83
#define NW_WBXML_LITERAL_A              0x84

#define NW_WBXML_EXT_0                  0xC0
#define NW_WBXML_EXT_1                  0xC1
#define NW_WBXML_EXT_2                  0xC2
#define NW_WBXML_OPAQUE                 0xC3
#define NW_WBXML_LITERAL_AC             0xC4


/*
 * WBXML tag tokens, section 5.8
 */

#define NW_WBXML_MASK_TAG_ID            0x3F  /* Masks off a tag's "has attribute"
                                               * and/or "has content" bit(s) */
#define NW_WBXML_FLAGS_ATTRIBUTES       0x80
#define NW_WBXML_FLAGS_CONTENT          0x40


/* TODO:  these defs belong somewhere else */
/* Attribute component type info */

#define NW_WBXML_ATTR_COMPONENT_TOKEN   0
#define NW_WBXML_ATTR_COMPONENT_STRING  1
#define NW_WBXML_ATTR_COMPONENT_EXT     2
#define NW_WBXML_ATTR_COMPONENT_ENTITY  3
#define NW_WBXML_ATTR_COMPONENT_OPAQUE  4
#define NW_WBXML_ATTR_COMPONENT_INVALID 5

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif


