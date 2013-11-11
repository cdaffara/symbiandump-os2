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
    @package:     NW_DOM

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/
#ifndef NW_DOM_TEXT_H
#define NW_DOM_TEXT_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_dom_node.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @typedef:     NW_DOM_TextItemIterator

    @synopsis:    Used to iterate over the components of a WBXML text item.

    @scope:       public
     
    @type:        NW_TinyDom_TextHandle_t

    @description: Used to iterate over the components of a WBXML text item.

 ** ----------------------------------------------------------------------- **/
typedef NW_TinyDom_TextHandle_t NW_DOM_TextItemIterator_t;


/** ----------------------------------------------------------------------- **
    @typedef:     NW_DOM_TextItem

    @synopsis:    Extension to support text nodes.

    @scope:       public
     
    @type:        NW_TinyDom_Text_t

    @description: DOM_TextItem_t is an extension to support text nodes in 
                  WBXML. Text nodes in WBXML can be composed of a list of 
                  string, extension, opaque, or entitity data types. 

 ** ----------------------------------------------------------------------- **/
typedef NW_TinyDom_Text_t NW_DOM_TextItem_t;

/*  ----------------------------------------------------------------------- **
    TEXT - SPECIFIC methods 
 ** ----------------------------------------------------------------------- **/
  

/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextNode_getData

    @synopsis:    Gets the value of the text node as a string.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextNode_t* node
                  The text node.

       [out] NW_String_t* valueString
                  The value of the node

    @description: Gets the value of the text node as a string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data returned successfully.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not a text node

       [NW_STAT_OUT_OF_MEMORY]
                  Unable to allocate memory for string storage.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  String storage is not assigned.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextNode_getData(NW_DOM_TextNode_t* node, 
                        NW_String_t* valueString);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextNode_setData

    @synopsis:    Creates a new TextNode and replaces the previous one.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_TextNode_t** node
                  The text node.

       [in] NW_String_t* val
                  Value of the text node.

    @description: Creates a new TextNode and replaces the previous one.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  New text node created.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter was NULL.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextNode_setData(NW_DOM_TextNode_t** node, 
                        NW_String_t* val);


/*  ----------------------------------------------------------------------- **
    TextItem Methods 
    In XML a text node has only string data, but in WBXML a TextNode consists 
    of a list of one or more text items including strings, text entities, etc.
 ** ----------------------------------------------------------------------- **/

/** ----------------------------------------------------------------------- **
    @typedef:     NW_DOM_TextItemType

    @synopsis:    Enumerated text item type.

    @scope:       public
     
    @type:        NW_Uint8

    @description: Enumerated text item type.

 ** ----------------------------------------------------------------------- **/
typedef NW_Uint8 NW_DOM_TextItemType_t;

#define NW_DOM_TEXT_ITEM_STRING       1
#define NW_DOM_TEXT_ITEM_EXTENSION    2
#define NW_DOM_TEXT_ITEM_ENTITY       3
#define NW_DOM_TEXT_ITEM_OPAQUE       4


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_new

    @synopsis:    Creates net TextItem

    @scope:       public

    @description: Creates new text item.

    @returns:     NW_DOM_TextItem_t*
                  New text item.
 ** ----------------------------------------------------------------------- **/
NW_DOM_TextItem_t*
NW_DOM_TextItem_new(void);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_initFromString

    @synopsis:    Initializes a text item with a string.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [in] NW_String_t* string
                  Data from which item will be initialized.

    @description: Initializes a text item with a string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_initFromString (NW_DOM_TextItem_t* item, 
                                NW_String_t* string);

 
/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_initFromEntity

    @synopsis:    Initializes a text item with an entity.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_TextItem_t* item
                  The text item.

       [in] NW_Uint32 entity
                  default

    @description: Initializes a Text Item of type NW_DOM_TEXT_ITEM_ENTITY
                  with an entity.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_initFromEntity (NW_DOM_TextItem_t* item, 
                                NW_Uint32 entity);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_initFromExtension

    @synopsis:    Initializes a text item with an extension.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [in] NW_Uint16 token
                  Token from which the item will be initialized.

       [in] NW_String_t* str
                  Data from which item will be initialized.

    @description: Initializes a text item with an extension.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_initFromExtension (NW_DOM_TextItem_t* item, 
                                   NW_Uint16 token, 
                                   NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_initFromExtensionInt

    @synopsis:    Initializes a text item with the particular extension
                  types EXT_T_[0,1,2] + uint32 where uint32 is not an
                  index into the string table.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [in] NW_Uint16 token
                  Token from which the item will be initialized.
                  The token must be one of EXT_T_[0,1,2].

       [in] NW_Uint32 x
                  The associated 32-bit unsigned value.

    @description: Initializes a text item with the particular extension
                  types EXT_T_[0,1,2] + uint32 where uint32 is not an
                  index into the string table.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Success.

       [NW_STAT_FAILURE]
                  Failure if token is not one of EXT_T_[0,1,2].

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_initFromExtensionInt (NW_DOM_TextItem_t* item,
                                      NW_Uint16 token,
                                      NW_Uint32 x);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_initFromOpaque

    @synopsis:    Initializes a text item with an opaque value.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_TextItem_t* item
                  The text item.

       [in] NW_Uint32 length
                  default

       [in] NW_Byte* data
                  default

    @description: Initializes a Text Item of type NW_DOM_TEXT_ITEM_OPAQUE
                  with an opaque value.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_initFromOpaque (NW_DOM_TextItem_t* item, 
                                NW_Uint32 length, 
                                NW_Byte* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_delete

    @synopsis:    Deallocates a text item.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_TextItem_t* item
                  The text item.

    @description: Deallocates a text item.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_TextItem_delete(NW_DOM_TextItem_t* item);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_getType

    @synopsis:     Finds the type of a text item. 

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

    @description: Finds the type of a text item (one of: extension, string, 
                 opaque, or entity).

    @returns:     NW_DOM_TextItemType_t
                  Type returned. or zero if unknown.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_TextItemType_t 
NW_DOM_TextItem_getType(NW_DOM_TextItem_t* item);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_setType

    @synopsis:    Sets the type of TextItem.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_TextItem_t* item
                  The text item.

       [in] NW_DOM_TextItemType_t type
                  Type to which to set the text item.

    @description: Sets the type of TextItem

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Value set.

       [NW_STAT_BAD_INPUT_PARAM]
                  Invalid type.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_DOM_TextItem_setType(NW_DOM_TextItem_t* item, 
                        NW_DOM_TextItemType_t type);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_toString

    @synopsis:    Creates a string representation of the item. 

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [out] NW_String_t* string
                  String value.

       [in] NW_Uint32 encoding
                  Encoding to apply.

    @description: Creates a string representation of the item.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String returned.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  If encoding is not supported.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  Error extracting string.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_toString(NW_DOM_TextItem_t* item, 
                         NW_String_t* string, 
                         NW_Uint32 encoding);

/*  ----------------------------------------------------------------------- **
    The following methods return the a text item component
 ** ----------------------------------------------------------------------- **/

/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_getEntity

    @synopsis:    Gets TextItem entity.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

    @description: If item is of type NW_DOM_TEXT_ITEM_ENTITY returns entity
                  otherwise returns zero.

    @returns:     NW_Uint32
                  Entity or 0 if not NW_DOM_TEXT_ITEM_ENTITY

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint32
NW_DOM_TextItem_getEntity(NW_DOM_TextItem_t* item);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_getString

    @synopsis:    Gets Text Item string.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [out] NW_String_t* string
                  String associated with the text item.

    @description:  Gets Text Item string if item is of type 
                   NW_DOM_TEXT_ITEM_STRING returns and modifies string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Success default.

       [NW_STAT_BAD_INPUT_PARAM]
                  Not NW_DOM_TEXT_ITEM_STRING.

       [NW_STAT_OUT_OF_MEMORY]
                  Could not allocate memory for string.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_getString(NW_DOM_TextItem_t* item, 
                          NW_String_t* string);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_getExtensionToken

    @synopsis:    Returns extension token.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [out] NW_Uint16* token
                  The token.

    @description: If TextItem is of type NW_DOM_TEXT_ITEM_EXTENSION then
                  assigns token to out parameter and returns NW_STAT_SUCCESS.
                  If not, it returns NW_STAT_FAILURE.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Success.

       [NW_STAT_FAILURE]
                  If text item is of wrong type.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_getExtensionToken(NW_DOM_TextItem_t* item,
                                  NW_Uint16* token);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_getExtension

    @synopsis:    Returns extension token and string.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [out] NW_String_t* str
                  String associated with extension if it is a text extension,
                  otherwise string cleared.

    @description: If TextItem is of type NW_DOM_TEXT_ITEM_EXTENSION returns 
                  token  ans string for extension. If not, it returns zero 
                  and clears the string.  Some extensions do not have text 
                  associated with them. The string is cleared for these as 
                  well. If this is a single token extension then the string 
                  is cleared first.

    @returns:     NW_Uint16
                  Token or zero if not NW_DOM_TEXT_ITEM_EXTENSION.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint16
NW_DOM_TextItem_getExtension(NW_DOM_TextItem_t* item, 
                             NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_getExtensionInt

    @synopsis:    Only for EXT_T_[0,1,2] where associated value is not
                  a reference to the string table, returns value

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [out] NW_Uint32* x
                  Value associated with extension if it is a text extension,
                  otherwise undefined.

    @description: If TextItem is of type NW_DOM_TEXT_ITEM_EXTENSION and
                  it was created as EXT_T_[0,1,2] with an associated
                  value that is not a reference to the string table
                  (i.e., created with
                  NW_DOM_TextItem_initFromExtensionInt()), then it
                  returns the value via the out parameter x and
                  returns NW_STAT_SUCCESS.  Otherwise returns
                  NW_STAT_FAILURE and the value of x is undefined.

    @returns:     NW_Status_t

       [NW_STAT_SUCCESS]
                  Success.

       [NW_STAT_FAILURE]
                  Failure.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItem_getExtensionInt(NW_DOM_TextItem_t* item,
                                NW_Uint32* x);



/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItem_getOpaque

    @synopsis:    Returns opaque pointer.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItem_t* item
                  The text item.

       [out] NW_Uint32* opaqueLen
                  Length of the opaque data.Not set if NULL pointer returned.

    @description: If TextItem is of type NW_DOM_TEXT_ITEM_OPAQUE returns 
                  pointer to opaque.

    @returns:     NW_Byte*
                  Opaque data if item is of type opaque; otherwise NULL.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Byte*
NW_DOM_TextItem_getOpaque(NW_DOM_TextItem_t* item, 
                          NW_Uint32* opaqueLen);


/*  ----------------------------------------------------------------------- **
    TEXT ITEM ITERATORS - iterate over different text items
 ** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItemIterator_new

    @synopsis:    Create new TextItemIterator.

    @scope:       public

    @description: Create new TextItemIterator.

    @returns:     NW_DOM_TextItemIterator_t*
                  POinter to new iterator.

 ** ----------------------------------------------------------------------- **/
NW_DOM_TextItemIterator_t*
NW_DOM_TextItemIterator_new(void);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItemIterator_delete

    @synopsis:    Delete TextItemIterator.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItemIterator_t* handle
                  The text item iterator.

    @description: Delete TextItemIterator.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_TextItemIterator_delete(NW_DOM_TextItemIterator_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextNode_getTextItemIterator

    @synopsis:    Returns TextItemIterator.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextNode_t* node
                  The text item iterator.

       [out] NW_DOM_TextItemIterator_t* iterator
                  Iterator to iterate over the text items in a text node.

    @description: Returns TextItemIterator.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Success.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Node is not a text node.

  ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextNode_getTextItemIterator(NW_DOM_TextNode_t* node, 
                                    NW_DOM_TextItemIterator_t* iterator);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextItemIterator_getNextTextItem

    @synopsis:    Iterates to the next text item.

    @scope:       public

    @parameters:
       [in] NW_DOM_TextItemIterator_t* handle
                  The text item iterator.

       [out] NW_DOM_TextItem_t* textItem
                  Next text item.

    @description: Iterates to the next text item.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_WBXML_ITERATE_MORE]
                  More to be returned.

       [NW_STAT_WBXML_ITERATE_DONE]
                  Last item returned.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextItemIterator_getNextTextItem(NW_DOM_TextItemIterator_t* handle, 
                                        NW_DOM_TextItem_t* textItem);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextNode_setDataFromTextItem

    @synopsis:    Sets new data for the given text node. 

    @scope:       public

    @parameters:
       [in-out] NW_DOM_TextNode_t** node
                  The text node.

       [in] NW_DOM_TextItem_t* val
                  Value of the text node.

    @description: Sets new data for the given text node by creating a new 
                  TextNode and replaces the previous one.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data set.

       [NW_STAT_BAD_INPUT_PARAM]
                  Invalid parameter entered.

       [NW_STAT_DOM_HEIRARCHY_REQUEST_ERR]
                  Internal error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextNode_setDataFromTextItem(NW_DOM_TextNode_t** node, 
                                    NW_DOM_TextItem_t* val);

/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_TextNode_addDataFromTextItem

    @synopsis:    Adds new data to the given text node.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_TextNode_t* node
                  The text node.

       [in] NW_DOM_TextItem_t* val
                  Value of the text node.

    @description: Adds new data to the given text node.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data set.

       [NW_STAT_BAD_INPUT_PARAM]
                  Invalid parameter entered.

       [NW_STAT_DOM_HEIRARCHY_REQUEST_ERR]
                  Internal error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_TextNode_addDataFromTextItem(NW_DOM_TextNode_t* node,
                                    NW_DOM_TextItem_t* val);


/*  ----------------------------------------------------------------------- **
    COMMENT_NODE SPECIFIC METHODS - Unsupported
 ** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_CommentNode_getData

    @synopsis:    Gets the data associated with the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_CommentNode_t* comment
                  The comment node.

       [out] NW_String_t* c
                  Data for the node.

    @description: Gets the data associated with the node.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data returned.

       [NW_STAT_BAD_INPUT_PARAM]
                  Invalid parameter entered.

       [NW_STAT_DOM_HEIRARCHY_REQUEST_ERR]
                  Internal error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_CommentNode_getData(NW_DOM_CommentNode_t* comment, 
                           NW_String_t* c);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_CommentNode_setData

    @synopsis:    Sets text for node.

    @scope:       public

    @parameters:
       [in] NW_DOM_CommentNode_t* comment
                  The comment node.

       [in] NW_String_t* c
                  Data to set into node.

    @description: Sets text for node.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data set.

       [NW_STAT_BAD_INPUT_PARAM]
                  Invalid parameter entered.

       [NW_STAT_DOM_HEIRARCHY_REQUEST_ERR]
                  Internal error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_CommentNode_setData(NW_DOM_CommentNode_t* comment, 
                           NW_String_t* c);


/*  ----------------------------------------------------------------------- **
    CHARACTER_DATA_NODE SPECIFIC METHODS - Unsupported
 ** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_CDATASectionNode_getData

    @synopsis:    Gets the data associated with the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_CDATASectionNode_t* cdata
                  The CDATA node.

       [out] NW_String_t* c
                  Data for the node.

    @description: Gets the data associated with the node.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data returned.

       [NW_STAT_BAD_INPUT_PARAM]
                  Invalid parameter entered.

       [NW_STAT_DOM_HEIRARCHY_REQUEST_ERR]
                  Internal error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_CDATASectionNode_getData(NW_DOM_CDATASectionNode_t* cdata, 
                                NW_String_t* c);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_CDATASectionNode_setData

    @synopsis:    Sets text for node.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_CDATASectionNode_t* cdata
                  The CDATA node.

       [in] NW_String_t* c
                  Data to set into node.

    @description: Sets text for node.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data set.

       [NW_STAT_BAD_INPUT_PARAM]
                  Invalid parameter entered.

       [NW_STAT_DOM_HEIRARCHY_REQUEST_ERR]
                  Internal error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_CDATASectionNode_setData(NW_DOM_CDATASectionNode_t* cdata, 
                                NW_String_t* c);


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_DOM_TEXT_H */
