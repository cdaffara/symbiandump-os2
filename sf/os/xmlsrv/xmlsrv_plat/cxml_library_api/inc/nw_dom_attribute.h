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
#ifndef NW_DOM_ATTRIBUTE_H
#define NW_DOM_ATTRIBUTE_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_dom_node.h>
#include <xml/cxml/nw_tinytree.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @struct:      NW_DOM_AttributeListIterator

    @synopsis:    Iterates over the list of attributes of an element.

    @scope:       public
    @variables:
       NW_TinyDom_AttrListHandle_t attrListHandle
                  this object

       NW_TinyTree_Node_t* node
                  default

    @description:

                  The NW_DOM_AttributeListIterator_t data type can be
                  thought of as a replacement for the NamedNodeMap
                  structure specified in the W3C DOM specification. In
                  this specification, performing getAttributes on an
                  element results in getting a readonly NamedNodeMap
                  structure, whereas in WAP DOM it results in getting a
                  pointer to the NW_DOM_AttributeListIterator_t data
                  type.

                  The main difference between the two is that
                  NamedNodeMap allows you to get items using an index,
                  whereas using NW_DOM_AttributeListIterator_t you can
                  iterate over a list of attributes.  Other
                  functionality available in NamedNodeMap, such as
                  getNamedItem, setNamedItem, removeNamedItem, is
                  already present in the APIs defined for an
                  ElementNode.

                  NW_DOM_AttributeListIterator_t is used to iterate over
                  the list of attributes of an element. Note that when a
                  tree is built by the parser, only the first attribute
                  of an element has a node of its own.  For example,
                  <card id="card1" title="example"> has 2 attributes,
                  but only 1 node is allocated, pointing to the first
                  attribute start. In a read-write tree, each added
                  attribute gets a new node.
** ----------------------------------------------------------------------- **/
typedef struct NW_DOM_AttributeListIterator_s {
  /* Used to iterate over the original attributes */
  NW_TinyDom_AttrListHandle_t attrListHandle;
  /* Used to iterate over added attributes */
  NW_TinyTree_Node_t* node;
}NW_DOM_AttributeListIterator_t;

/* An attribute value in the DOM API */

typedef NW_TinyDom_AttrVal_t NW_DOM_AttrVal_t;

/** ----------------------------------------------------------------------- **
    @struct:      NW_DOM_AttributeHandle

    @synopsis:    default

    @scope:       public
    @variables:
       NW_TinyDom_AttributeHandle_t tinyHandle
                  The attribute handle.

       NW_TinyTree_Node_t* node
                  default

    @description: NW_DOM_AttributeHandle_t is used as an extension to
                  support WBXML attributes. In WBXML, attibutes can
                  consist of an attribute start token and several value
                  components. Moreover, an attribute start token can
                  represent either the attribute name, the attribute
                  name and the initial part of the value (a so-called
                  value prefix), or the complete attribute name and
                  value.This type has methods that return these elements
                  and allow iteration over the values.  Attribute
                  handles are also supported for XML documents since
                  applications may wish to use the attribute list
                  iterator mechanism for these documents. Attribute list
                  iterators return attribute handles.

                  NW_DOM_AttributeHandle is used to iterate over the
                 values of an attribute.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_DOM_AttributeHandle_s{
  NW_TinyDom_AttributeHandle_t tinyHandle;
  NW_TinyTree_Node_t* node;
}NW_DOM_AttributeHandle_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeListIterator_new

    @synopsis:    Constructor.

    @scope:       public

    @description: Allocates memory for NW_DOM_AttributeListIterator.
                  Returns NULL in case of failure to do so.

    @returns:     NW_DOM_AttributeListIterator_t*
                  New iterator.

 ** ----------------------------------------------------------------------- **/
NW_DOM_AttributeListIterator_t*
NW_DOM_AttributeListIterator_new(void);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeListIterator_initialize

    @synopsis:    Initialize the iterator.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttributeListIterator_t* handle
                  The attribute handle.

    @description: Initialize the iterator.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns this value.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttributeListIterator_initialize(NW_DOM_AttributeListIterator_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeListIterator_delete

    @synopsis:    Delete the iterator.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttributeListIterator_t* handle
                  The attribute handle.

    @description: Delete the iterator.

    @returns:     NW_Status_t
                  Status of operation.

      [NW_STAT_SUCCESS]
                  Always returns this value.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttributeListIterator_delete(NW_DOM_AttributeListIterator_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeListIterator_getNextAttribute

    @synopsis:    Get next attribute.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttributeListIterator_t* listIterator
                  Iterator for the attribute list.

       [out] NW_DOM_AttributeHandle_t* attributeHandle
                  A handle for an attribute.

    @description: Get next attribute.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_DOM_BAD_INPUT_PARAM_ERR]
                  Error on input.

       [NW_STAT_WBXML_ITERATE_MORE]
                  More attributes available.

       [NW_STAT_WBXML_ITERATE_DONE]
                  No more attributes available.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttributeListIterator_getNextAttribute(NW_DOM_AttributeListIterator_t* listIterator, 
                                              NW_DOM_AttributeHandle_t* attributeHandle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_new

    @synopsis:    Create new handle.

    @scope:       public

    @description: Create new handle.

    @returns:     NW_DOM_AttributeHandle_t*
                  Allocates memory for new handle.

 ** ----------------------------------------------------------------------- **/
NW_DOM_AttributeHandle_t*
NW_DOM_AttributeHandle_new(void);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_delete

    @synopsis:    Delete handle.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

    @description: Delete handle.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STATUS_SUCCESS]
                  Always returns NW_STATUS_SUCCESS.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttributeHandle_delete(NW_DOM_AttributeHandle_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_initWithNode

    @synopsis:    Initializes and Attribute Handle with an attribute node.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [in] NW_TinyDom_Parser_t* parser
                  Parser to assign.

       [in] NW_TinyTree_Node_t* node
                  Node to attach to handle.

    @description: Initializes and Attribute Handle with an attribute node.
                  Note that only some attributes have nodes of their own.
                  Such attributes are initilized by this method.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returned.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttributeHandle_initWithNode(NW_DOM_AttributeHandle_t* handle,
                                    NW_TinyDom_Parser_t* parser,
                                    NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_initWithOffset

    @synopsis:    Initializes and Attribute Handle with a offset.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [in] NW_TinyDom_Parser_t* parser
                  Parser to use.

       [in] NW_TinyTree_Offset_t offset
                  Offset to use.
    @description: Initializes and Attribute Handle with a offset.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returned.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttributeHandle_initWithOffset(NW_DOM_AttributeHandle_t* handle,
                                      NW_TinyDom_Parser_t* parser,
                                      NW_TinyTree_Offset_t offset);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_initWithStartToken

    @synopsis:    Initializes an Attribute Handle with a WBXML start token.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [in] NW_TinyDom_Parser_t* parser
                  Parser to use.

       [in] NW_Uint16 token
                  The attribute start token with page.

    @description: Initializes an Attribute Handle with WBXML start token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Success

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

       [NW_STAT_FAILURE]
                  Failure

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttributeHandle_initWithStartToken(NW_DOM_AttributeHandle_t* handle,
                                          NW_TinyDom_Parser_t* parser,
                                          NW_Uint16 fqToken);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_addVal

    @synopsis:    Appends an attribute value to an existing attribute handle

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [in] NW_DOM_AttrVal_t* value
                  The attribute value to append to handle

    @description: Appends an attribute value to an existing attribute handle

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Success

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

       [NW_STAT_FAILURE]
                  Failure

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttributeHandle_addVal(NW_DOM_AttributeHandle_t* handle,
                              NW_DOM_AttrVal_t* value);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_getName

    @synopsis:    Gets attribute name.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [out] NW_String_t* name
                  Attribute name without prefix.

    @description: Returns the attribute name (without prefix if any)

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Name returned.

       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  Name not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttributeHandle_getName(NW_DOM_AttributeHandle_t* handle, 
                               NW_String_t* name);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_getToken

    @synopsis:    Gets the token for the Attribute.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

    @description: Returns the token for the Attribute.

    @returns:     An attribute start token represented as two bytes: the
                  high order 8 bits represent the code page, the remaining
                  bits represent the token; returns 0 if the handle is
                  invalid or the document is not a WBXML document.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint16 
NW_DOM_AttributeHandle_getToken(NW_DOM_AttributeHandle_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_getValuePrefix

    @synopsis:    Gets attribute prefix.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [out] NW_String_t* prefix
                  Value prefix for the attribute.

    @description: Returns the attribute prefix (for e.g. href="http://"
                  will return "http://". This is a WBXML-specific extension.
                  Behavior is undefined for non-WBXML attributes.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Got prefix.

       [NW_STAT_DOM_NO_VALUE_PREFIX]
                  Error

       [NW_STAT_OUT_OF_MEMORY]
                  Error

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  Error

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttributeHandle_getValuePrefix(NW_DOM_AttributeHandle_t* handle, 
                                      NW_String_t* prefix);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_getNameWithPrefix

    @synopsis:    Gets the attribute name.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [out] NW_String_t* name
                  Attribute name.

    @description: Returns the attribute name (with prefix if any).
                  This is a WBXML-specific extension. Behavior is undefined
                  for non-WBXML attributes.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Name returned.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  Error

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttributeHandle_getNameWithPrefix(NW_DOM_AttributeHandle_t* handle,
                                         NW_String_t* name);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_getNextVal

    @synopsis:    Get next value from list.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [out] NW_DOM_AttrVal_t* attrVal
                  The next attribute value.

    @description: Gets a value if one is still available through the
                  iterator. Return status indicates if this is a valid
                  value or the iterator has hit the end of the list.

    @returns:     NW_Status_t
                  Status of operation.

      [NW_STAT_WBXML_ITERATE_MORE]
                  Valid value, more in list.

      [NW_STAT_WBXML_ITERATE_DONE]
                  Not valid, done with list.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttributeHandle_getNextVal(NW_DOM_AttributeHandle_t* handle, 
                                  NW_DOM_AttrVal_t* attrVal); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_getValue

    @synopsis:    Gets string representation of attribute value.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

       [out] NW_String_t* value
                  The attribute value as a string.

    @description: Gets string representation of attribute value.

    @returns:     NW_Status_t
                  Status of operation.

      [NW_STAT_SUCCESS]
                  Value returned.

      [NW_STAT_DOM_NO_STRING_RETURNED]
                  No string found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttributeHandle_getValue(NW_DOM_AttributeHandle_t* handle, 
                                NW_String_t* value); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttributeHandle_getEncoding

    @synopsis:    Returns the document encoding.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttributeHandle_t* handle
                  The attribute handle.

    @description: Returns the document encoding.

    @returns:     NW_Uint32
                  Attribute encoding.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint32
NW_DOM_AttributeHandle_getEncoding(NW_DOM_AttributeHandle_t* handle);


/* ----------------------------------------------------------------------- **
   Attribute values:
   In WBXML documents attribute values can be a combination of string,
   entity, extension, opaque, or token.
** ----------------------------------------------------------------------- **/

#define NW_DOM_ATTR_VAL_STRING       1
#define NW_DOM_ATTR_VAL_EXTENSION    2
#define NW_DOM_ATTR_VAL_ENTITY       3
#define NW_DOM_ATTR_VAL_OPAQUE       4
#define NW_DOM_ATTR_VAL_TOKEN        5


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_new

    @synopsis:    Constructor.

    @scope:       public

    @description: Constructor.

    @returns:     NW_DOM_AttrVal_t*
                  New attribute value.

 ** ----------------------------------------------------------------------- **/
NW_DOM_AttrVal_t*
NW_DOM_AttrVal_new(void);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_initFromString

    @synopsis:    Initialize attribute value from string.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [in] NW_String_t* string
                  String with which to initialize AttrVal.

    @description: Initializes an AttrVal of type NW_DOM_ATTR_VAL_STRING
                  from a string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Initialized.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttrVal_initFromString (NW_DOM_AttrVal_t* val, 
                               NW_String_t* string);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_initFromEntity

    @synopsis:    Initializes an attribute value.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [in] NW_Uint32 entity
                  Entity with which to initialize the attribute value.

    @description: Initializes an AttrVal of type NW_DOM_ATTR_VAL_ENTITY.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_initFromEntity (NW_DOM_AttrVal_t* val,
                               NW_Uint32 entity);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_initFromExtension

    @synopsis:    Initializes an attribute value.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [in] NW_Uint16 token
                  Token that specifies the extension type.

       [in] NW_String_t* str
                  String extension value.

    @description: Initializes an AttrVal of type NW_DOM_ATTR_VAL_EXTENSION.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_initFromExtension (NW_DOM_AttrVal_t* val,
                                  NW_Uint16 token,
                                  NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_initFromExtensionInt

    @synopsis:    Initializes an attribute value with the particular extension
                  types EXT_T_[0,1,2] + uint32 where uint32 is not an
                  index into the string table.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [in] NW_Uint16 token
                  Token from which the AttrVal will be initialized.
                  The token must be one of EXT_T_[0,1,2].

       [in] NW_Uint32 x
                  The associated 32-bit unsigned value.

    @description: Initializes an AttrVal with the particular extension
                  types EXT_T_[0,1,2] + uint32 where uint32 is not an
                  index into the string table.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Success.

       [NW_STAT_FAILURE]
                  Failure if token is not one of EXT_T_[0,1,2].

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_initFromExtensionInt (NW_DOM_AttrVal_t* val,
                                     NW_Uint16 token,
                                     NW_Uint32 x);

/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_initFromOpaque

    @synopsis:    Initializes an attribute value.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [in] NW_Uint32 length
                  Length of the data from which val will be initialized.

       [in] NW_Byte* data
                  Data from which val will be initialized.

    @description: Initializes an AttrVal of type NW_DOM_ATTR_VAL_OPAQUE.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_initFromOpaque (NW_DOM_AttrVal_t* val,
                               NW_Uint32 length,
                               NW_Byte* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_initFromToken

    @synopsis:    Initializes an AttrVal of type NW_DOM_ATTR_VAL_TOKEN

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [in] NW_Uint16 token
                  Token from which AttrVal will be initialized. The token
                  represents the attribute value and includes codepage
                  information in its most significant 8 bits.

    @description: Initializes an AttrVal of type NW_DOM_ATTR_VAL_TOKEN
                   with a token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttrVal_initFromToken (NW_DOM_AttrVal_t* val, NW_Uint16 token);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_delete

    @synopsis:    Destructor.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* attrVal
                  The AttrVal structure.

    @description: Destructor.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_delete (NW_DOM_AttrVal_t* attrVal);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_getType

    @synopsis:    Returns the type of attribute value.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

    @description: Returns the type of attribute value.

    @returns:     NW_Uint16
                  NW_DOM_ATTR_VAL enumeration.

       [NW_DOM_ATTR_VAL_EXTENSION]
                  Extension
       [NW_DOM_ATTR_VAL_STRING]
                  String
       [NW_DOM_ATTR_VAL_ENTITY]
                  Entity
       [NW_DOM_ATTR_VAL_TOKEN]
                  Token
       [NW_DOM_ATTR_VAL_OPAQUE]
                  Opaque
       [null]
                  Unknown type.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint16
NW_DOM_AttrVal_getType(NW_DOM_AttrVal_t* val);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_setType

    @synopsis:    Sets the type of attribute value.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [in] NW_Uint16 type
                  See NW_DOM_AttrVal_getType for values.

    @description: Sets the type of attribute value.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_setType(NW_DOM_AttrVal_t* val, NW_Uint16 type);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_toString

    @synopsis:    Gets representation of value.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_AttrVal_t* av
                  The AttrVal structure.

       [in-out] NW_String_t* str
                  String representation of attribute value.

       [in] NW_Uint32 encoding
                  default

    @description: Gets representation of value.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String returned.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  Invalid type of AttrVal.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Invalid encoding.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_AttrVal_toString(NW_DOM_AttrVal_t* av, 
                        NW_String_t* str, 
                        NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_getEntity

    @synopsis:    Returns the associated entity.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttrVal_t* item
                  The AttrVal structure.

    @description: Returns the associate entity if attribute value is of
                  type NW_DOM_ATTR_VAL_ENTITY.

    @returns:     NW_Uint32
                  The entity if val is of type entity; otherwise, 0.

 ** ----------------------------------------------------------------------- **/
NW_Uint32
NW_DOM_AttrVal_getEntity(NW_DOM_AttrVal_t* item);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_getExtensionToken

    @synopsis:    Returns extension token.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttrVal_t* val.
                  The AttrVal structure.

       [out] NW_Uint16* token
                  The token.

    @description: If AttrVal is of type NW_DOM_ATTR_VAL_EXTENSION then
                  assigns token to out parameter and returns NW_STAT_SUCCESS.
                  If not, it returns NW_STAT_FAILURE.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Success.

       [NW_STAT_FAILURE]
                  If AttrVal is of wrong type.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_getExtensionToken(NW_DOM_AttrVal_t* val,
                                 NW_Uint16* token);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_getExtension

    @synopsis:    Returns the extension if the attribute.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttrVal_t* item
                  The AttrVal structure.

       [out] NW_String_t* str
                  The string associated with the extension.

    @description: Returns the associated extension type if attribute value
                  is of type NW_DOM_ATTR_VAL_EXTENSION.

    @returns:     NW_Uint16
                  The extension token; or 0 if there are invalid input
                  parameters or if AttrVal is not of type extension.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint16
NW_DOM_AttrVal_getExtension(NW_DOM_AttrVal_t* item, 
                            NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_getExtensionInt

    @synopsis:    Only for EXT_T_[0,1,2] where associated value is not
                  a reference to the string table, returns value

    @scope:       public

    @parameters:
       [in] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [out] NW_Uint32* x
                  Value associated with extension if it is an extension,
                  otherwise undefined.

    @description: If AttrVal is of type NW_DOM_ATTRVAL_EXTENSION and
                  it was created as EXT_T_[0,1,2] with an associated
                  value that is not a reference to the string table
                  (i.e., created with
                  NW_DOM_AttrVal_initFromExtensionInt()), then it
                  returns the value via the out parameter x and
                  returns NW_STAT_SUCCESS.  Otherwise returns
                  NW_STAT_FAILURE and the value of x is undefined.

    @returns:     NW_Status_t

       [NW_STAT_SUCCESS]
                  Success.

       [NW_STAT_FAILURE]
                  Failure.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_getExtensionInt(NW_DOM_AttrVal_t* item,
                               NW_Uint32* x);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_getOpaque

    @synopsis:    Returns the associated opaque type.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

       [out] NW_Uint32* opaqueLen
                  Length of opaque data.

    @description: If attribute value is of type NW_DOM_ATTR_VAL_OPAQUE
                  returns the associated opaque data and length.

    @returns:     NW_Byte*
                  A pointer to the opaque data; or NULL if there are
                  invalid input parameters or if AttrVal is not of type
                  Opaque.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Byte*
NW_DOM_AttrVal_getOpaque(NW_DOM_AttrVal_t* val, 
                         NW_Uint32* opaqueLen);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_getString

    @synopsis:    Gets string of the attribute component.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttrVal_t* item
                  The AttrVal structure.

       [out] NW_String_t* string
                  String associated with the AttrVal.

    @description: Returns the string if attribute is of type
                  NW_DOM_ATTR_VAL_STRING.

    @returns:     NW_Status_t
                  Status of operation.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_AttrVal_getString(NW_DOM_AttrVal_t* item,
                         NW_String_t* string);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_AttrVal_getToken

    @synopsis:    Returns the associated token.

    @scope:       public

    @parameters:
       [in] NW_DOM_AttrVal_t* val
                  The AttrVal structure.

    @description: Returns the associated token if attribute value is
                  of type NW_DOM_ATTR_VAL_TOKEN.

    @returns:     NW_Uint16
                  The token if the attribute component value is of type
                  token; otherwise 0

 ** ----------------------------------------------------------------------- **/
NW_Uint16
NW_DOM_AttrVal_getToken(NW_DOM_AttrVal_t* val);

/* ----------------------------------------------------------------------- **
   PI Nodes
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ProcessingInstructionNode_getTarget

    @synopsis:    Gets the name of the target for this processing instruction.

    @scope:       public

    @parameters:
       [in] NW_DOM_ProcessingInstructionNode_t* node
                  A processing instruction node.

       [out] NW_String_t* target
                  Name of the target.

    @description: Gets the name of the target for this processing instruction.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Target returned.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not a DOM_ProcessingInstructionNode_t node.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  General error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_ProcessingInstructionNode_getTarget(NW_DOM_ProcessingInstructionNode_t* node,
                                           NW_String_t* target);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ProcessingInstructionNode_getTargetToken

    @synopsis:    Returns the token associated with a WBXML PI node.

    @scope:       public

    @parameters:
       [in] NW_DOM_ProcessingInstructionNode_t* node
                  A processing instruction node.

    @description: Returns the token associated with a WBXML PI node. This
                  method complements the method
                  NW_DOM_ProcessingInstructionNode_getTarget. It is specific
                  to WBXML, and behavior is undefined for non-WBXML nodes.

    @returns:     NW_Uint16
                  The token associated with the target (most significant
                  8 bits represent codepage).

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint16
NW_DOM_ProcessingInstructionNode_getTargetToken (NW_DOM_ProcessingInstructionNode_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ProcessingInstructionNode_getData

    @synopsis:    Gets the data associated with this processing instruction.

    @scope:       public

    @parameters:
       [in] NW_DOM_ProcessingInstructionNode_t* node
                  A processing instruction node.

       [out] NW_String_t* data
                  Data associated with the node.

    @description: Gets the data associated with this processing instruction.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Data returned.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not a NW_DOM_ProcessingInstructionNode_t node.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  General error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_ProcessingInstructionNode_getData(NW_DOM_ProcessingInstructionNode_t* node,
                                         NW_String_t* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ProcessingInstructionNode_getHandle

    @synopsis:    Gets the attribute handle associated with the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_ProcessingInstructionNode_t* node
                  A processing instruction node.

       [in-out] NW_DOM_AttributeHandle_t* handle
                  Handle to get target and data.

    @description: Gets the attribute handle associated with the node.
                  This supports WBXML only.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Handle returned.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not a processing instruction node.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_ProcessingInstructionNode_getHandle(NW_DOM_ProcessingInstructionNode_t* node,
                                           NW_DOM_AttributeHandle_t* handle);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_DOM_ATTRIBUTE_H */
