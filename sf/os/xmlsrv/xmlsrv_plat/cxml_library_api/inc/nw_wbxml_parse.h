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

#ifndef NW_PARSER_WBXML_PARSE_H
#define NW_PARSER_WBXML_PARSE_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_wbxml_token.h>
#include <xml/cxml/nw_wbxml_event.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_CP_Registry_Entry

    @synopsis:    Registry entry.

    @scope:       public
    @variables:
       NW_Uint8 code_page
                  The code page.

       NW_Uint32 start_offset
                  Start offset.

    @description: Each entry must record if it is for tokens or attributes.
                  A trick to encode this without extra memory relies on the
                  use of two bytes in WBXML to encode a page switch.  With two 
                  bytes one has an even offset and one an odd offset.  We pick
                  the even if it is a tag page and the odd offset if it is an 
                  attribute page. 
 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_CP_Registry_Entry_s{
  NW_Uint8 code_page;
  NW_Uint32 start_offset;
}NW_WBXML_CP_Registry_Entry_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_CP_Registry

    @synopsis:    The code page registry.

    @scope:       public
    @variables:
       NW_WBXML_CP_Registry_Entry_t* storage
                  Storage array.

       NW_WBXML_CP_Registry_Entry_t* current
                  Current entry.

       NW_Int32 count
                  Entry count.

    @description: The code page registry.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_CP_Registry_s {
  NW_WBXML_CP_Registry_Entry_t* storage;
  NW_WBXML_CP_Registry_Entry_t* current;
  NW_Int32 count;
  // WLIU_DEBUG:NW_Int32 realcount;
} NW_WBXML_CP_Registry_t;

/* ----------------------------------------------------------------------- **
   TO DO: the NW_Byte code pointer needs to be removed and put into a
   derived class in the buffer implementation 
** ----------------------------------------------------------------------- **/

/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_Parser

    @synopsis:    The parser.

    @scope:       public
    @variables:
       const struct NW_WBXML_EventHandler_s* handler
                  Event handlers.

       void* context
                  Context to pass to event handlers.

       NW_Uint32 flags
                  The flags.

       NW_Uint32 state
                  The state.

       NW_Uint8 tag_code_page
                  The tag code page.

       NW_Uint8 attribute_code_page
                  The attribute code page.

       NW_WBXML_CP_Registry_t cp_registry
                  The code page registry.

       NW_Uint32 dictionary
                  1-based index into dictionary table.

       NW_WBXML_Document_t* doc
                  The document.

       NW_Uint32 offset
                  Offset into bytecode.

       NW_Uint32 left
                  Byte code left after current pointer.

       NW_Status_t status
                  Parser status from CallBacks.

       NW_Byte* p
                  Byte code pointer.

       NW_Uint8 ext_t_not_table_index
                  A boolean: whether the associated index with
                  EXT_T_[0,1,2] refs a string table or not, defaults
                  to 0 meaning use string table, 1 means associated
                  integer with EXT_T_[0,1,2] is not a string table
                  index

    @description: The parser.
 ** ----------------------------------------------------------------------- **/
struct NW_WBXML_Parser_s {
  const struct NW_WBXML_EventHandler_s* handler; /* Event handlers */
  void* context;                           /* Context to pass to event handlers */
  NW_Uint32 flags;                         
  NW_Uint32 state;
  NW_Uint8 tag_code_page;
  NW_Uint8 attribute_code_page;
  NW_WBXML_CP_Registry_t cp_registry;
  NW_Uint32 dictionary;                     /* 1-based index into dictionary table */
  NW_WBXML_Document_t* doc;
  NW_Uint32 offset;                          /*Offset into bytecode */
  NW_Uint32 left;                            /* Byte code left after current pointer */
  NW_Status_t status;                       /* Parser status from CallBacks */
  NW_Byte* p;                               /*Byte code pointer */
  NW_Uint32 recursiveCallCnt;               /* counts "element parse/content parse" recursive calls*/
  /* a boolean, whether EXT_T_[0,1,2] refs a string table or not,
     defaults to 0 meaning use string table, 1 means associated
     integer with EXT_T_[0,1,2] is not a string table index */
  NW_Uint8 ext_t_not_table_index;
//  WLIU_DEBUG: NW_Int32 lastValid;
};



/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_newInPlace

    @synopsis:    Parser initialization 

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parser initialization 
 ** ----------------------------------------------------------------------- **/
IMPORT_C void
NW_WBXML_Parser_newInPlace (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_reset

    @synopsis:    Reset the parser.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Reset the parser.

 ** ----------------------------------------------------------------------- **/
void
NW_WBXML_Parser_reset (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_delete

    @synopsis:    Destructor.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Destructor.

 ** ----------------------------------------------------------------------- **/
void
NW_WBXML_Parser_delete (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_addCPRegistry

    @synopsis:    Add CP registry.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_WBXML_CP_Registry_Entry_t* storage
                  The entries.

       [in] NW_Int32 count
                  The number of entries.

    @description: Create an uninitialized code page registry. As long
                  as the registry is not created, it will be
                  ignored. Once it has been created, the parser will
                  check the REGISTRY_INIT flag. If this is not set, the
                  registry will be initialized as the parser runs. If
                  this is set, then the registry will be consulted when
                  updating the offset.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_addCPRegistry(NW_WBXML_Parser_t* parser,
                              NW_WBXML_CP_Registry_Entry_t* storage,
                              NW_Int32 count);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_registerHandler

    @synopsis:    Register handler.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

       [in] const struct NW_WBXML_EventHandler_s* handler
                  The handler.

       [in] void* context
                  The context.

    @description: Cache the parser's event handler and context.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_WBXML_Parser_registerHandler(NW_WBXML_Parser_t* parser,
                                const struct NW_WBXML_EventHandler_s* handler,
                                void* context);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_saveContext

    @synopsis:    Save parser context.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

       [out] NW_WBXML_Parser_t* context
                  The context.

    @description: Save/restore context can be used if the parser is to be
                  invoked reentrantly (in a callback).  Normally this should
                  be avoided since the only really safe way to do this is to
                  save the whole parser object on the stack. 

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_saveContext(NW_WBXML_Parser_t* parser,
                            NW_WBXML_Parser_t* context);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_restoreContext

    @synopsis:    Restore parser context.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_WBXML_Parser_t* context
                  The context.

    @description: Restore parser context. See description for saveContext().

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_restoreContext(NW_WBXML_Parser_t* parser, 
                               NW_WBXML_Parser_t* context);

/* ----------------------------------------------------------------------- **
   Get and set methods
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_getOffset

    @synopsis:    Get offset.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Get offset.

    @returns:     NW_Uint32
                  Read offset.

 ** ----------------------------------------------------------------------- **/
NW_Uint32 
NW_WBXML_Parser_getOffset(NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_setOffset

    @synopsis:    Set offset.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_Uint32 offset
                  New offset.

    @description: Set offset.

    @returns:     NW_Int32
                  Offset or -1 if out of range.

 ** ----------------------------------------------------------------------- **/
NW_Int32 
NW_WBXML_Parser_setOffset(NW_WBXML_Parser_t* parser, 
                          NW_Uint32 offset);  


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_setTagCodepage

    @synopsis:    Set tag codepage.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_Uint8 page
                  The page.

    @description: Set tag codepage.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_setTagCodepage(NW_WBXML_Parser_t* parser, 
                               NW_Uint8 page);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_getTagCodepage

    @synopsis:    Get tag codepage.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Get tag codepage.

    @returns:     NW_Uint8
                  The code page.

 ** ----------------------------------------------------------------------- **/
NW_Uint8
NW_WBXML_Parser_getTagCodepage(NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_setAttrCodepage

    @synopsis:    Set attribute codepage.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_Uint8 page
                  The code page.

    @description: Set attribute codepage.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_setAttrCodepage(NW_WBXML_Parser_t* parser, 
                                NW_Uint8 page);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_getAttrCodepage

    @synopsis:    Get attribute codepage.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Get attribute codepage.

    @returns:     NW_Uint8
                  Code page.

 ** ----------------------------------------------------------------------- **/
NW_Uint8
NW_WBXML_Parser_getAttrCodepage(NW_WBXML_Parser_t* parser);


/* ----------------------------------------------------------------------- **
    Parse document elements 
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_tagNameParse

    @synopsis:    Tag name parse.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parse the token and name part of a tag. This generates a
                  fully qualified token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Tag name parsed.

       [NW_STAT_WBXML_HAS_ATTRIBUTES]
                  Has attributes.

       [NW_STAT_WBXML_HAS_ATTR_CONTENT]
                  Has attribute content.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_tagNameParse(NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_attributeListIterate

    @synopsis:    Parse an attribute list.
    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parse an attribute list

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

       [NW_STAT_WBXML_ITERATE_DONE]
                  Done.

       [NW_STAT_WBXML_ITERATE_MORE]
                  More to do.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_WBXML_Parser_attributeListIterate (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_attributeValsIterate

    @synopsis:    Parse an attribute value list.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parse an attribute value list.

    @returns:     NW_Status_t 
                  Status of operation.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

       [NW_STAT_WBXML_ITERATE_DONE]
                  Done.

       [NW_STAT_WBXML_ITERATE_MORE]
                  More to do.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_WBXML_Parser_attributeValsIterate (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_attributeNameParse

    @synopsis:    Parse an attribute.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parse an attribute.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute parsed.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_WBXML_Parser_attributeNameParse (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_textIterate

    @synopsis:    Parse text.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parse text.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

       [NW_STAT_WBXML_ITERATE_DONE]
                  Done.

       [NW_STAT_WBXML_ITERATE_MORE]
                  More to do.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_textIterate(NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_parser_attributeListIterate

    @synopsis:    Parse an attribute list.
    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parse an attribute list.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

       [NW_STAT_WBXML_ITERATE_DONE]
                  Done.

       [NW_STAT_WBXML_ITERATE_MORE]
                  More to do.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_parser_attributeListIterate(NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_parser_attributeValsIterate

    @synopsis:    Parse attribute values.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parse attribute values.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

       [NW_STAT_WBXML_ITERATE_DONE]
                  Done.

       [NW_STAT_WBXML_ITERATE_MORE]
                  More to do.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_WBXML_parser_attributeValsIterate (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_attributeParse

    @synopsis:    Attribute parse.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Attribute parse.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Done.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_attributeParse(NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_contentParse

    @synopsis:    Parse content.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

    @description: Parse content.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Done.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_contentParse (NW_WBXML_Parser_t* parser);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_extensionParse

    @synopsis:    Parse extension.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_Int32 token_state
                  Token state.

    @description: Parse extension.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Done.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_extensionParse (NW_WBXML_Parser_t* parser,
                                NW_Int32 token_state);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_opaqueParse

    @synopsis:    Parse opaque data.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_Int32 token_state
                  Token state.

    @description: Parse opaque data.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Done.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_opaqueParse (NW_WBXML_Parser_t* parser, 
                             NW_Int32 token_state);

/* ----------------------------------------------------------------------- **
   TODO READ_WRITE only? 
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_setBuffer

    @synopsis:    Sets parser to use a buffer without parsing it.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_WBXML_Document_t* doc
                  The document.

       [in] NW_Byte* buf
                  The buffer.

       [in] NW_Uint32 bufsize
                  The size of the buffer.

    @description: This method sets the parser to use a buffer without
                  actually parsing it.  This is used by clients who want
                  to create a tree from scratch without parsing a 
                  document. This requires that doc be initialized with
                  any document type and charset and that the parser be 
                  initialized.  This should leave the parser in the same
                  state as NW_WBXML_Parser_parserBuffer() followed by
                  NW_WBXML_Parser_reintialize().  

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_setBuffer (NW_WBXML_Parser_t* parser, 
                           NW_WBXML_Document_t* doc,
                           NW_Byte* buf,
                           NW_Uint32 bufsize);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_resetBuffer

    @synopsis:    Reset buffer.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_Byte* buff
                  The buffer.

       [in] NW_Uint32 buffsize
                  The size of the buffer.

    @description: Reset the parser buffer without resetting the doc.  
                  This is more efficient when, for example, the buffer 
                  gets resized due to a reallocation.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Parser_resetBuffer(NW_WBXML_Parser_t* parser, 
                            NW_Byte* buff, 
                            NW_Uint32 buffsize);

/* ----------------------------------------------------------------------- **
   TODO: Move to parser buffer header?
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Parser_parseBuffer

    @synopsis:    Parse buffer.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Parser_t* parser
                  The parser.

       [in] NW_WBXML_Document_t* doc
                  The document.

       [in] NW_Byte* buf
                  The buffer.

       [in] NW_Uint32 bufsize
                  The size of the buffer.

    @description: Parse document from a buffer.  

    @returns:     NW_Status_t
                  Status of the operation.

       [NW_STAT_SUCCESS]
                  Done.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Parse error.

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

       [NW_STAT_BAD_INPUT_PARAM]
                  Requird parameter is NULL.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Invalid character set.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_WBXML_Parser_parseBuffer (NW_WBXML_Parser_t* parser,
                             NW_WBXML_Document_t* doc,
                             NW_Byte* buf,
                             NW_Uint32 bufsize);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_PARSER_WBXML_PARSE_H */
