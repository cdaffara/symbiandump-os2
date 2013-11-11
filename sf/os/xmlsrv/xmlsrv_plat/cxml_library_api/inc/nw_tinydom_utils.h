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
    @package:     NW_TinyDom

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef TINY_DOM_UTILS_H
#define TINY_DOM_UTILS_H

#include <xml/cxml/nw_wbxml_parse.h>
#include <xml/cxml/nw_tinydom.h>
#include <xml/cxml/nw_encoder_wbxmlwriter.h>
#include <xml/cxml/nw_dom_document.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @struct:       NW_TinyDom_Parser_Wrapper

    @synopsis:    A wrapper to conceal messy parsing details.

    @scope:       public
    @variables:
       NW_WBXML_Parser_t wbxmlParser
                  The WBXMl Parser.

       NW_TinyDom_Parser_t tinyParser
                  The tiny parser.

       NW_TinyDom_Tree_t tinyDOMTree
                  The DOM Tree.

       NW_WBXML_Writer_t writer
                  The writer.

       NW_WBXML_Document_t document
                  The source document.

    @description: A wrapper to conceal messy parsing details.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_Parser_Wrapper_s{
  NW_WBXML_Parser_t   wbxmlParser;
  NW_TinyDom_Parser_t tinyParser;
  NW_TinyDom_Tree_t   tinyDOMTree;
  NW_WBXML_Writer_t writer;
  NW_WBXML_Document_t document;
} NW_TinyDom_Parser_Wrapper_t;

/* Deprecated - Left in for backwards copatibility */
typedef struct NW_TinyDom_Parser_Wrapper_s Parser_t;

 
/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_ParserInitialize

    @synopsis:    Helper function to initialize the parser.

    @scope:       public

    @parameters:
       [in] Parser_t* p
                  Pointer to a Parser_t struct

       [in] NW_WBXML_Dictionary_t* dictionaries[]
                  Pointer to the WBXML dictionaries.

       [in] NW_Int32 dictionaryCount
                  Number of dictionaries.

       [in] NW_Uint32 default_public_id
                  Default public (i.e.dictionary) ID.

    @description: Helper function to initialize the parser.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Parser initialized.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t NW_TinyDom_ParserInitialize(Parser_t* p, 
                                        NW_WBXML_Dictionary_t* dictionaries[],
                                        NW_Int32 dictionaryCount,
                                        NW_Uint32 default_public_id);

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_MakeDOMTree

    @synopsis:    Helper function to parse the buffer, creating a tree.

    @scope:       public

    @parameters:
       [in] Parser_t* p
                  Pointer to parser struct.

       [in] NW_Byte* buffer
                  Pointer to content.

       [in] NW_Uint32 length
                  Number of bytes in buffer.

       [out] NW_DOM_DocumentNode_t** root
                  Pointer to document root node

    @description: Helper function to parse the buffer, creating a tree.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Made DOM tree.

       [NW_STAT_FAILURE]
                  Failed to make tree.

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t NW_TinyDom_MakeDOMTree (Parser_t* p, 
                                    NW_Byte* buffer,
                                    NW_Uint32 length, 
                                    NW_DOM_DocumentNode_t** root);

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AppendDOMTree

    @synopsis:    Helper function to parse the buffer, appending new nodes to the tree.

    @scope:       public

    @parameters:
       [in] Parser_t* p
                  Pointer to parser struct.

       [in] NW_Byte* buffer
                  Pointer to content.

       [in] NW_Uint32 length
                  Number of bytes in buffer.

       [in] NW_Uint32 cp_count
                  Number of code page switch count

       [in] NW_Int32 lastValid
                  Index pointing to the position in buffer where last valid parse stopped at.

       [out] NW_DOM_DocumentNode_t** root
                  Pointer to document root node

    @description: Helper function to parse the buffer, appending new nodes to the tree.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Made DOM tree.

       [NW_STAT_FAILURE]
                  Failed to make tree.

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t NW_TinyDom_AppendDOMTree (Parser_t* p, 
                                    NW_Byte* buffer,
                                    NW_Uint32 length, 
                                    NW_Uint32 cp_count,
                                    NW_Int32 lastValid,
                                    NW_DOM_DocumentNode_t** root);

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_ParserDelete

    @synopsis:    Free the internal memory in allocated in 
                  NW_TinyDom_MakeDOMTree.

    @scope:       public

    @parameters:
       [in] Parser_t* p
                  Pointer to parser struct.

    @description: Free the internal memory in allocated in 
                  NW_TinyDom_MakeDOMTree.

 ** ----------------------------------------------------------------------- **/
IMPORT_C void NW_TinyDom_ParserDelete(Parser_t* p);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* TINY_DOM_UTILS_H */
