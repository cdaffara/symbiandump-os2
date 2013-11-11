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

#ifndef NW_TINY_DOM_H
#define NW_TINY_DOM_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_tinytree.h>
#include <xml/cxml/nw_wbxml_document.h>
#include <xml/cxml/nw_wbxml_parse.h>
#include <xml/cxml/nw_wbxml_opaque.h>
#include <xml/cxml/nw_encoder_wbxmlwriter.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




enum NW_TinyDom_ExtensionType_e {
  NW_TINYDOM_EXTENSION_TYPE_NORMAL,
  NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER
};


/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyDom_Extension

    @synopsis:    Extension structure.

    @scope:       public
    @variables:
       NW_Uint32 token
                  Token.

       NW_String_t string
                  Extension string.

    @description: Extension structure.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_Extension_s{
  /* type is either NW_TINYDOM_EXTENSION_TYPE_NORMAL or
     NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER */
  NW_Uint8 type;
  NW_Uint32 token;
  union {
    NW_String_t string;
    NW_Uint32 x;
  } value;
} NW_TinyDom_Extension_t;


/** ----------------------------------------------------------------------- **
    @union:      NW_TinyDom_AttrValComponent

    @synopsis:    Attribute value component union.

    @scope:       public
    @variables:
       NW_Uint32 value_token
                  token

       NW_String_t string
                  string

       NW_TinyDom_Extension_t ext
                  extension

       NW_Uint32 entity
                  entity.

       NW_WBXML_Opaque_t opaque
                  opaque

    @description: Union of all possible attribute value types.
 ** ----------------------------------------------------------------------- **/
typedef union NW_TinyDom_AttrValComponent_u{
  NW_Uint32 value_token;
  NW_String_t string;
  NW_TinyDom_Extension_t ext;
  NW_Uint32 entity;
  NW_WBXML_Opaque_t opaque;
}NW_TinyDom_AttrValComponent_t;


/** ----------------------------------------------------------------------- **
    @struct:       NW_TinyDom_AttrVal

    @synopsis:    Attribute value.

    @scope:       public
    @variables:
       NW_Uint32 type
                  type

       NW_TinyDom_AttrValComponent_t component
                  attribute value union of all possible types.

    @description: An attribute value has a type info field and a union of
                  all the possible types. The type info values are defined
                  in NW_WBXML_Attribute.h. This type is designed to be
                  passed as an in/out param to the attribute value iterator.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_AttrVal_s {
  NW_Uint32 type;
  NW_TinyDom_AttrValComponent_t component;
} NW_TinyDom_AttrVal_t;


/* Node type bits */

#define T_DOM_NODE_DOC          (NW_Uint16)00
#define T_DOM_NODE_TAG          (NW_Uint16)01
#define T_DOM_NODE_ATTR         (NW_Uint16)02
#define T_DOM_NODE_TEXT         (NW_Uint16)03
#define T_DOM_NODE_PI           (NW_Uint16)04
#define T_DOM_NODE_COMMENT      (NW_Uint16)05
#define T_DOM_NODE_CDATASECTION (NW_Uint16)06
#define T_DOM_NODE_XML          (NW_Uint16)80


/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyDom_Tree

    @synopsis:    Wrapper for tree components.

    @scope:       public
    @variables:
       NW_WBXML_Document_t* doc
                  The document.

       NW_TinyTree_t tree
                  The tree.

       NW_WBXML_Parser_t* parser
                  Parser associated with this tree.

       NW_WBXML_Writer_t* writer
                  Writer for this tree.

       NW_TinyTree_Node_t* root_node
                  Root node of tree.

    @description:  Wrapper for tree components.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_Tree_s{
  NW_WBXML_Document_t* doc;
  NW_TinyTree_t tree;
  NW_WBXML_Parser_t* parser;
  NW_WBXML_Writer_t* writer;
  NW_TinyTree_Node_t* root_node;
}NW_TinyDom_Tree_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyDom_Parser

    @synopsis:    Parser structure.

    @scope:       public
    @variables:
       NW_Uint32 state
                  State.

       NW_TinyTree_Offset_t node_count
                  default

       NW_TinyTree_Offset_t cp_count
                  Current offset.

       NW_TinyDom_Tree_t* dom_tree
                  The tree.

       NW_TinyTree_Node_t* current_node
                  Present position in tree.

       NW_TinyTree_Offset_t content_offset
                  Present offset into buffer.

    @description: Parser structure.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_Parser_s{
  NW_Uint32 state;
  NW_TinyTree_Offset_t node_count;
  NW_TinyTree_Offset_t cp_count;
  NW_TinyDom_Tree_t* dom_tree;
  NW_TinyTree_Node_t* current_node;
  /* Offset where current content begins */
  NW_TinyTree_Offset_t content_offset;
}NW_TinyDom_Parser_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_Tree_construct

    @synopsis:    Constructor.

    @scope:       public

    @parameters:
       [in-out] NW_TinyDom_Tree_t* domTree
                  The tree.

       [in] NW_WBXML_Parser_t* wbxmlParser
                  The parser

       [in] NW_WBXML_Document_t* doc
                  The document.

       [in] NW_WBXML_Writer_t* writer
                  The writer.

    @description: Constructor.

 ** ----------------------------------------------------------------------- **/
void
NW_TinyDom_Tree_construct(NW_TinyDom_Tree_t* domTree,
                          NW_WBXML_Parser_t* wbxmlParser,
                          NW_WBXML_Document_t* doc,
                          NW_WBXML_Writer_t* writer);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_Tree_destruct

    @synopsis:    Destructor.

    @scope:       public

    @parameters:
       [in-out] NW_TinyDom_Tree_t* domTree
                  The DOM tree.

    @description: Destructor.

 ** ----------------------------------------------------------------------- **/
void
NW_TinyDom_Tree_destruct(NW_TinyDom_Tree_t* domTree);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_Parser_construct

    @synopsis:    Constructor.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Parser_t* domParser
                  The DOM parser.

       [in] NW_TinyDom_Tree_t* domTree
                  The DOM tree.

    @description: Constructor.

 ** ----------------------------------------------------------------------- **/
IMPORT_C void
NW_TinyDom_Parser_construct(NW_TinyDom_Parser_t* domParser,
                            NW_TinyDom_Tree_t* domTree);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_Parser_destruct

    @synopsis:    Desctructor.

    @scope:       public

    @parameters:
       [in-out] NW_TinyDom_Parser_t* domParser
                  The DOM parser.

    @description: Destructor.

 ** ----------------------------------------------------------------------- **/
void
NW_TinyDom_Parser_destruct(NW_TinyDom_Parser_t* domParser);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_Parser_buildTree

    @synopsis:    Build tiny DOM tree.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Parser_t* domParser
                  The DOM parser.

       [in] char* buffer
                  The buffer containing the document data.

       [in] NW_Uint32 buffsize
                  The size of the document buffer.

       [in] NW_Bool freeBuff
                  Flag

    @description: Builds a DOM tree. The DOM parser must previously have
                  been initialized to use a parser (WBXML or XML) that
                  knows how to parse the contents of the buffer.Build tiny
                  DOM tree. Run Pass1 and Pass 2.

    @returns:     NW_Status_t
                  default

       [NW_STAT_SUCCESS]
                  Tree built.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_Parser_buildTree(NW_TinyDom_Parser_t* domParser,
                            char* buffer,
                            NW_Uint32 buffsize,
                            NW_Bool freeBuff);

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_Parser_incrementalBuildTree

    @synopsis:    build/append tiny DOM tree.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Parser_t* domParser
                  The DOM parser.

       [in] char* buffer
                  The buffer containing the document data.

       [in] NW_Uint32 buffsize
                  The size of the document buffer.

       [in] NW_Bool freeBuff
                  Flag

    @description: Builds a DOM tree. The DOM parser must previously have
                  been initialized to use a parser (WBXML or XML) that
                  knows how to parse the contents of the buffer.Build tiny
                  DOM tree. Run Pass1 and Pass 2.

    @returns:     NW_Status_t
                  default

       [NW_STAT_SUCCESS]
                  Tree built.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_Parser_incrementalBuildTree(NW_TinyDom_Parser_t* domParser,
                            char* buffer,
                            NW_Uint32 buffsize,
                            NW_Bool freeBuff,
                            NW_Int32 lastValid);

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_Node_getType

    @synopsis:    Get node type.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_Node_t* node
                  The DOM parser.

    @description: Get node type.

    @returns:     NW_Uint16
                  Node flag.

 ** ----------------------------------------------------------------------- **/
NW_Uint16
NW_TinyDom_Node_getType(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyDom_ListIterator

    @synopsis:    ListIterator structure.

    @scope:       public
    @variables:
       NW_Uint32 state
                  Iterator state.

       void* segment
                  Segment

       NW_TinyTree_Offset_t segSize
                  Segment size

       NW_TinyTree_Offset_t offset
                  Offset into document.

       NW_TinyDom_Parser_t* tiny_parser
                  Parser used for iteration.

       void* context
                  Context.

    @description: ListIterator structure.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_ListIterator_s {
  NW_Uint32 state;
  void* segment;
  NW_TinyTree_Offset_t segSize;
  NW_TinyTree_Offset_t offset;
  NW_TinyDom_Parser_t* tiny_parser;
  void* context;
}NW_TinyDom_ListIterator_t;


/* Tiny dom routines to support tags */


/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyDom_Tag

    @synopsis:    Tag structure.

    @scope:       public
    @variables:
       NW_TinyDom_Parser_t* tiny_parser
                  This parser.

       NW_Uint32 fq_token
                  Token with codepage and attribute/element tag.

       NW_Uint32 name_index
                  Index of first character of name into string table.

    @description: Tag structure.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_Tag_s {
  NW_TinyDom_Parser_t* tiny_parser;
  NW_Uint32 fq_token;
  NW_Uint32 name_index;
}NW_TinyDom_Tag_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_getTagToken

    @synopsis:    Get tag token.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Parser_t* parser
                  The parser.

       [in] NW_TinyTree_Node_t* node
                  Node to use.

    @description: Get tag token.

    @returns:     NW_Uint32
                  Tag token.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint32
NW_TinyDom_getTagToken(NW_TinyDom_Parser_t* parser,
                       NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_getTagName

    @synopsis:    Get tag name.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Parser_t* parser
                  This parser.

       [in] NW_TinyTree_Node_t* node
                  This node.

       [out] NW_String_t* name
                  Name of tag.

    @description: Get tag name.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Name found.

       [NW_STAT_WBXML_NO_NAME]
                  No name found.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  No name found.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_getTagName(NW_TinyDom_Parser_t* parser,
                      NW_TinyTree_Node_t* node,
                      NW_String_t* name);

/* Tiny dom routines to support attributes */

/** ----------------------------------------------------------------------- **
    @typedef:     NW_TinyDom_AttrListHandle

    @synopsis:    Attribute list handle.

    @scope:       public
    @type:        NW_TinyDom_ListIterator_t

    @description: Attribute list handle.
 ** ----------------------------------------------------------------------- **/
typedef NW_TinyDom_ListIterator_t NW_TinyDom_AttrListHandle_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttrListHandle_init

    @synopsis:    Initialize attribute list handle.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_AttrListHandle_t* handle
                  This handle.

       [in] NW_TinyDom_Parser_t* parser
                  This parser.

       [in] NW_TinyTree_Node_t* node
                  This node.

    @description: Initialize attribute list handle.

 ** ----------------------------------------------------------------------- **/
void
NW_TinyDom_AttrListHandle_init(NW_TinyDom_AttrListHandle_t* handle,
                               NW_TinyDom_Parser_t* parser,
                               NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttrListHandle_iterate

    @synopsis:    Iterate through a list of attributes.

    @scope:       public

    @parameters:
       [in-out] NW_TinyDom_AttrListHandle_t* handle
                  The handle.

    @description: Iterate through a list of attributes. There are no
                  attribute list callbacks since we just want to iterate
                  the attribute list, returning the start of each attribute.

    @returns:     NW_TinyTree_Offset_t
                  Tree offset  or NULL if at end of iteration.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Offset_t
NW_TinyDom_AttrListHandle_iterate(NW_TinyDom_AttrListHandle_t* handle);


/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyDom_AttributeHandle

    @synopsis:    An attribute handle, for iteration over values.

    @scope:       public
    @variables:
       NW_TinyDom_ListIterator_t tlit
                  The iterator structure.

       void* value
                  The current value.

       NW_Uint32 fq_token
                  Fully qualified token. That is: token, codepage and
                  attribute flag.

       NW_Uint32 name_index
                  Index into string table.

    @description: An attribute handle, for iteration over values.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_AttributeHandle_s {
  NW_TinyDom_ListIterator_t tlit;
  /* field value is a pointer to the struct to fill in by parsing */
  NW_TinyDom_AttrVal_t* value;
  NW_Uint32 fq_token;
  NW_Uint32 name_index;
} NW_TinyDom_AttributeHandle_t;


/* ----------------------------------------------------------------------- **
   Attribute values are made up of components of several types. These are
   defined in WBXML/types.h
** ----------------------------------------------------------------------- **/

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttributeHandle_init

    @synopsis:    Initialize an attribute handle.

    @scope:       public

    @parameters:
       [in-out] NW_TinyDom_AttributeHandle_t* handle
                  The handle.

       [in-out] NW_TinyDom_Parser_t* parser
                  The parser.

       [in] NW_TinyTree_Offset_t offset
                  Offset into buffer.

    @description: Initialize an attribute handle by parsing an attribute's
                  start token and values, calling the attribute handlers.

 ** ----------------------------------------------------------------------- **/
void
NW_TinyDom_AttributeHandle_init(NW_TinyDom_AttributeHandle_t* handle,
                                NW_TinyDom_Parser_t* parser,
                                NW_TinyTree_Offset_t offset);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttributeHandle_valsIterate

    @synopsis:    Iterate over attribute values.

    @scope:       public

    @parameters:
       [in-out] NW_TinyDom_ListIterator_t *it
                  The iterator.

    @description: Initialize an attribute handle by parsing an attribute's
                  start token and values, calling the attribute handlers.

    @returns:     NW_TinyTree_Offset_t
                  Offset.  If offset is 0, then at end of iteration.


 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Offset_t
NW_TinyDom_AttributeHandle_valsIterate(NW_TinyDom_ListIterator_t *it);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttributeHandle_getToken

    @synopsis:    Get the token from the handle.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_AttributeHandle_t* handle
                  dThe handle.

    @description: Get the fully qualified token.

    @returns:     NW_Uint32
                  Token in handle structure.

 ** ----------------------------------------------------------------------- **/
NW_Uint32
NW_TinyDom_AttributeHandle_getToken(NW_TinyDom_AttributeHandle_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttributeHandle_getName

    @synopsis:    Get the name from the handle.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_AttributeHandle_t* handle
                  The handle.

       [out] NW_String_t* name
                  The name.

    @description: Get the name, if any, from the handle.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Name found.

       [NW_STAT_WBXML_NO_NAME]
                  Name not found.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Invalid string in table.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_AttributeHandle_getName(NW_TinyDom_AttributeHandle_t* handle,
                                   NW_String_t* name);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttributeHandle_iterateValues

    @synopsis:    Iterate the values.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_AttributeHandle_t* handle
                  The handle.

       [out] NW_TinyDom_AttrVal_t* value
                  Attribute value.

    @description: Iterate the values.

    @returns:     NW_TinyTree_Offset_t
                  Buffer offset or zero if done.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Offset_t
NW_TinyDom_AttributeHandle_iterateValues(NW_TinyDom_AttributeHandle_t* handle,
                                         NW_TinyDom_AttrVal_t* value);


/* TODO: Add method to get values as a string */


/** ----------------------------------------------------------------------- **
    @typedef:     NW_TinyDom_TextHandle

    @synopsis:    A text handle, for iteration over text components.

    @scope:       public
    @type:        NW_TinyDom_AttributeHandle_t

    @description: A text handle, for iteration over text components.
 ** ----------------------------------------------------------------------- **/
typedef NW_TinyDom_AttributeHandle_t NW_TinyDom_TextHandle_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_TextHandle_init

    @synopsis:    Initialize a text handle.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_TextHandle_t* handle
                  The handle.

       [in] NW_TinyDom_Parser_t* parser
                  The parser.

       [in] NW_TinyTree_Offset_t offset
                  The offset.

    @description: Initialize a text handle.

 ** ----------------------------------------------------------------------- **/
void
NW_TinyDom_TextHandle_init(NW_TinyDom_TextHandle_t* handle,
                           NW_TinyDom_Parser_t* parser,
                           NW_TinyTree_Offset_t offset);


/** ----------------------------------------------------------------------- **
    @typedef:     NW_TinyDom_Text

    @synopsis:    Text item types are a subset of attribute value types.

    @scope:       public
    @type:        NW_TinyDom_AttrVal_t

    @description: Text item types are a subset of attribute value types.
 ** ----------------------------------------------------------------------- **/
typedef NW_TinyDom_AttrVal_t NW_TinyDom_Text_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_TextHandle_iterate

    @synopsis:    Text item iterator.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_TextHandle_t* handle
                  The handle.

       [out] NW_TinyDom_Text_t* item
                  The returned text item.
    @description: Iterate through the text items in a text element, calling
                  the text handlers

    @returns:     NW_TinyTree_Offset_t
                  Offset into buffer or zero if no item returned.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Offset_t
NW_TinyDom_TextHandle_iterate(NW_TinyDom_TextHandle_t* handle,
                              NW_TinyDom_Text_t* item);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_getDocHeader

    @synopsis:    Get the doc header from the tiny tree.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tree.

    @description: Get the doc header from the tiny tree.

    @returns:     NW_WBXML_Document_t*
                  Doc header or NULL if NULL tree.

 ** ----------------------------------------------------------------------- **/
NW_WBXML_Document_t*
NW_TinyDom_getDocHeader(NW_TinyTree_t* tree);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_getParser

    @synopsis:    Get a pointer to the dom parser from the tiny_tree.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tree.

    @description: Get a pointer to the dom parser from the tiny_tree.

    @returns:     NW_TinyDom_Parser_t*
                  default

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_TinyDom_Parser_t*
NW_TinyDom_getParser(NW_TinyTree_t* tree);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_getTree

    @synopsis:    Get a pointer to the dom tree from a tiny tree.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tree.

    @description: Get a pointer to the dom tree from a tiny tree.

    @returns:     NW_TinyDom_Tree_t*
                  POinter to the DOM tree or NULL if not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_TinyDom_Tree_t*
NW_TinyDom_getTree(NW_TinyTree_t* tree);

/* ----------------------------------------------------------------------- **
   Dom write support methods: These are created as orphan nodes that
   then need to be attached to the tree.
** ----------------------------------------------------------------------- **/

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_createAttributeByToken

    @synopsis:    Create attribute by token.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Tree_t* dom_tree
                  The DOM tree.

       [in] NW_Uint16 token
                  The token.

       [in] NW_TinyDom_AttrVal_t* value
                  The new attribute.

    @description: Create attribute by token as an orphan node that
                  then need to be attached to the tree.

    @returns:     NW_TinyTree_Node_t*
                  Pointer to the new node or NULL if not successful.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyDom_createAttributeByToken(NW_TinyDom_Tree_t* dom_tree,
                                  NW_Uint16 token,
                                  NW_TinyDom_AttrVal_t* value);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_createAttributeByName

    @synopsis:    Create attribute by name.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Tree_t* dom_tree
                  The tree.

       [in] NW_String_t* name
                  The name.

       [in] NW_TinyDom_AttrVal_t* value
                  The new attribute.

    @description: Create attribute by name as an orphan node that
                  then need to be attached to the tree.

    @returns:     NW_TinyTree_Node_t*
                  Pointer to the new node or NULL if not successful.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyDom_createAttributeByName(NW_TinyDom_Tree_t* dom_tree,
                                 NW_String_t* name,
                                 NW_TinyDom_AttrVal_t* value);

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttributeHandle_initWithStartToken

    @synopsis:    Initialize the handle with a start token.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_AttributeHandle_t* tinyHandle
                  The handle.

       [out] NW_TinyTree_Node_t** ppNode
                  The tree node.

       [in] NW_TinyDom_Parser_t* parser
                  The parser.

       [in] NW_Uint16 fqToken
                  The token with page.

    @description: Initialize the handle with a start token.

    @returns:     NW_Status_t

        [NW_STAT_SUCCESS]
                  Success

        [NW_STAT_FAILURE]
                  A failure other than out-of-memory.

        [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_AttributeHandle_initWithStartToken(NW_TinyDom_AttributeHandle_t* tinyHandle,
                                              NW_TinyTree_Node_t** ppNode,
                                              NW_TinyDom_Parser_t* parser,
                                              NW_Uint16 fqToken);

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_AttributeHandle_addVal

    @synopsis:    Append the attribute value to the attribute handle.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_AttributeHandle_t* tinyHandle
                  The handle.

       [in] NW_TinyTree_Node_t* node
                  The tree node.

       [in] NW_DOM_AttrVal_t* value
                  The attribute value to append to handle

    @description: Append the attribute value to the attribute handle.

    @returns:     NW_Status_t

        [NW_STAT_SUCCESS]
                  Success

        [NW_STAT_FAILURE]
                  A failure other than out-of-memory.

        [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_AttributeHandle_addVal(NW_TinyDom_AttributeHandle_t* tinyHandle,
                                  NW_TinyTree_Node_t* node,
                                  NW_TinyDom_AttrVal_t* val);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_createElementByToken

    @synopsis:    Create element by token.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Tree_t* dom_tree
                  The tree.

       [in] NW_Uint16 token
                  The token.

    @description: Create element by token as an orphan node that
                  then need to be attached to the tree.

    @returns:     NW_TinyTree_Node_t*
                  Pointer to the new node or NULL if not successful.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyDom_createElementByToken(NW_TinyDom_Tree_t* dom_tree,
                                NW_Uint16 token);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_createElementByName

    @synopsis:    Create element by name.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Tree_t* dom_tree
                  The tree.

       [in] NW_String_t* name
                  The name.

    @description: Create element by name as an orphan node that
                  then need to be attached to the tree.

    @returns:     NW_TinyTree_Node_t*
                  Pointer to the new node or NULL if not successful.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyDom_createElementByName(NW_TinyDom_Tree_t* dom_tree,
                               NW_String_t* name);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_createTextNode

    @synopsis:    Create text node.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Tree_t* dom_tree
                  The tree.

       [in] NW_TinyDom_Text_t* text
                  The value.

    @description: Create element by name as an orphan node that
                  then needs to be attached to the tree.

    @returns:     NW_TinyTree_Node_t*
                  Pointer to the new node or NULL if not successful.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyDom_createTextNode(NW_TinyDom_Tree_t* dom_tree,
                          NW_TinyDom_Text_t* text);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_addDataFromTextItem

    @synopsis:    Adds data to a text node from a text item.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tinyTree
                  The tree.

       [in] NW_TinyDom_Tree_t* tinyDomTree
                  The DOM tree.

       [in] NW_TinyTree_Node_t* node
                  The text node.

       [in] NW_TinyTree_Text_t text
                  The value.

       [in] NW_Uint32 encoding
                  The IANA MIBenum for the character encoding.

    @description: Adds data to a text node from a text item.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data added.

       [NW_STAT_FAILURE]
                  Data not added.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_addDataFromTextItem(NW_TinyTree_t* tinyTree,
                               NW_TinyDom_Tree_t* tinyDomTree,
                               NW_TinyTree_Node_t* node,
                               NW_TinyDom_Text_t* val,
                               NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_Tree_create

    @synopsis:    Create a new tree.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Tree_t* dom_tree
                  The DOM tree.

       [in] NW_TinyDom_Parser_t* dom_parser
                  The parser to associate with this tree.

       [in] NW_WBXML_Document_t* doc
                  The source document.

       [in] NW_WBXML_Parser_t* parser
                  The WBXML parser.

       [in] NW_WBXML_Writer_t* writer
                  Tree writer.

       [in] NW_Uint16 init_node_count
                  Initial allocation.

       [in] NW_Bool enableStringTable
                  Set to true for normal WBXML.  Set to false to disable
                  writing from using a string table.

    @description: Create a new tree.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Tree created.

       [NW_STAT_FAILURE]
                  Dictionary not found or root node not created.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_Tree_create(NW_TinyDom_Tree_t* dom_tree,
                       NW_TinyDom_Parser_t* dom_parser,
                       NW_WBXML_Document_t* doc,
                       NW_WBXML_Parser_t* parser,
                       NW_WBXML_Writer_t* writer,
                       NW_Uint16 init_node_count,
                       NW_Bool enableStringTable);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_writeDocHeader

    @synopsis:    Write doc header.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_Tree_t* dom_tree
                  The tree.

       [in] NW_Uint8 version
                  The version.

       [in] NW_Uint32 publicid
                  The dictionary public ID.

       [in] NW_Uint32 encoding
                  Character encoding.

    @description: Write doc header.

    @returns:     NW_TinyTree_Node_t*
                  New node or NULL if not successful.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyDom_writeDocHeader(NW_TinyDom_Tree_t* dom_tree,
                                      NW_Uint8 version,
                                      NW_Uint32 publicid,
                                      NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyDom_removeAttrFromListNode

    @synopsis:    Remove attribute from list node.

    @scope:       public

    @parameters:
       [in] NW_TinyDom_AttrListHandle_t* h
                  The handle.

       [in] NW_Uint32 length
                  Length of attribute.

    @description: Remove attribute from list node.

    @returns:     NW_Status_t
                  Success of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyDom_removeAttrFromListNode(NW_TinyDom_AttrListHandle_t* h,
                                  NW_Uint32 length);

void
NW_TinyDom_setLastValid(NW_TinyDom_Tree_t* dom_tree, NW_Int32 lastValid);

NW_Int32
NW_TinyDom_getLastValid(NW_TinyDom_Tree_t* dom_tree);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */


#endif  /* NW_TINY_DOM_H */
