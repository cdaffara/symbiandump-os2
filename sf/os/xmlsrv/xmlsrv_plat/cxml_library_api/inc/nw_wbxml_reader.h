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

#warning The CXML API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

/** ----------------------------------------------------------------------- **
    @package:     NW_WBXML

    @synopsis:    default

    @description: Wbxml token definitions

 ** ----------------------------------------------------------------------- **/

/* ----------------------------------------------------------------------- **
  Definitions for the WBXML parser's reader
** ----------------------------------------------------------------------- **/

#ifndef NW_PARSER_WBXML_READER_H
#define NW_PARSER_WBXML_READER_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_wbxml_document.h>
#include <xml/cxml/nw_wbxml_opaque.h>
#include <xml/cxml/nw_wbxml_parse.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* The "parser-reader" interface */

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_advance

    @synopsis:    Advance parser.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t parser* parser
                  The parser.

       [in] NW_Int32 count
                  Amount to advance the parser.

    @description: Safely advance the parser through bytecode. The functions
                  which read values from the parser don't advance the parser
                  since this may not be necessary or safe (e.g. when reading
                  the last data item.).

    @returns:     NW_Int32
                  Amount advanced or -1 if out of range.

 ** ----------------------------------------------------------------------- **/
NW_Int32 
NW_WBXML_Parser_advance (NW_WBXML_Parser_t* parser, 
                         NW_Int32 count);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_hasMoreBytecode

    @synopsis:    Check if there is more NW_Byte code to read.
    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Check if there is more NW_Byte code to read.

    @returns:     NW_Int32
                  1 if more left otherwise 0.

 ** ----------------------------------------------------------------------- **/
NW_Int32 
NW_WBXML_Parser_hasMoreBytecode (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_readUint8

    @synopsis:    Read 8 bit unsigned integer.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Read 8 bit unsigned integer.

    @returns:     NW_Uint8
                  The integer.

 ** ----------------------------------------------------------------------- **/
NW_Uint8 
NW_WBXML_Parser_readUint8 (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_readMbUint32

    @synopsis:    Read 32 bit unsigned integer.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_Uint32* val
                  The value.

    @description: Read 32 bit unsigned integer.

    @returns:     NW_Int32
                  Number of bytes read or -1 if failed..

 ** ----------------------------------------------------------------------- **/
NW_Int32 
NW_WBXML_Parser_readMbUint32 (NW_WBXML_Parser_t* parser, 
                              NW_Uint32* val);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_getInlineStrLen

    @synopsis:    Get inline string length.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_WBXML_Document_t* doc
                  The document.

    @description: Safely get the length of an inline string at current 
                  parser position.

    @returns:     NW_Int32
                  Number of bytes read or -1 if failed.

 ** ----------------------------------------------------------------------- **/
NW_Int32 
NW_WBXML_Parser_getInlineStrLen (NW_WBXML_Parser_t* parser, 
                                 NW_WBXML_Document_t* doc);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_getStringInline

    @synopsis:    Get inline string.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_WBXML_Document_t* doc
                  The document.

       [out] NW_String_t* s
                  The returned string.default

    @description: Safely read an inline string at the current parser position.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String read.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Read error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_WBXML_Parser_getStringInline(NW_WBXML_Parser_t* parser,
                                NW_WBXML_Document_t* doc,
                                NW_String_t* s);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_getOpaque

    @synopsis:    Get opaque.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_Uint32 length
                  Maximum nuber of bytes to read.

       [out] NW_WBXML_Opaque_t* o
                  Opaque storage.

    @description: Safely get an opaque at the current parser position.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_WBXML_Parser_getOpaque(NW_WBXML_Parser_t* parser, 
                          NW_Uint32 length, 
                          NW_WBXML_Opaque_t* o);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_readStringTable

    @synopsis:    Read string table.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_WBXML_Document_t* doc
                  The document.

       [out] NW_Byte** table
                  The loaded table.

    @description: Read in the string table.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String table read.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Read error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_readStringTable(NW_WBXML_Parser_t* parser, 
                                NW_WBXML_Document_t* doc,
                                NW_Byte** table); 


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_PARSER_WBXML_READER_H */
