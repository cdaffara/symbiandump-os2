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


#include "cxml_internal.h"
#include <xml/cxml/nw_dom_attribute.h>
#include <xml/cxml/nw_wbxml_dictionary.h>
#include <xml/cxml/nw_tinydom.h>
#include <xml/cxml/nw_string_string.h>
#include "nw_dom_wbxmltypes.h"

/*
 * ATTRIBUTE LIST HANDLE
 * is used to iterate over the list of attributes of an element
 */

/*
 * Allocates memory for NW_DOM_AttributeListIterator
 * Returns NULL in case of failure to do so
 */

NW_DOM_AttributeListIterator_t *
NW_DOM_AttributeListIterator_new(void)
{
  NW_DOM_AttributeListIterator_t *handle =
    (NW_DOM_AttributeListIterator_t *) 
    NW_Mem_Malloc(sizeof (NW_DOM_AttributeListIterator_t));
  if (handle == NULL)
    return NULL;
  handle->node = NULL;
  return handle;
}

/* 
 * Initializes an AttributeListIterator
 * Returns NW_STAT_SUCCESS
 */

NW_Status_t
NW_DOM_AttributeListIterator_initialize(NW_DOM_AttributeListIterator_t *handle)
{

  NW_ASSERT(handle != NULL);  

  handle->node = NULL;
  handle->attrListHandle.state = NW_STAT_SUCCESS;
  handle->attrListHandle.offset = 0;
  handle->attrListHandle.tiny_parser = NULL;
  handle->attrListHandle.context = NULL;

  return NW_STAT_SUCCESS;
}

NW_Status_t
NW_DOM_AttributeListIterator_delete(NW_DOM_AttributeListIterator_t *handle)
{
  NW_ASSERT(handle != NULL);
  NW_Mem_Free(handle);
  return NW_STAT_SUCCESS;
}

/*
 * Returns 
 *   NW_STAT_WBXML_ITERATE_MORE
 *   NW_STAT_WBXML_ITERATE_DONE
 *
 * MODIFIED attributeHandle
 */

EXPORT_C NW_Status_t
NW_DOM_AttributeListIterator_getNextAttribute(NW_DOM_AttributeListIterator_t* listHandle, 
                                              NW_DOM_AttributeHandle_t *attributeHandle)
{
  NW_TinyTree_Offset_t offset = 0;
  NW_TinyDom_Parser_t *parser = NULL;
  
  NW_ASSERT(listHandle != NULL);
  NW_ASSERT(listHandle->node != NULL);
  NW_ASSERT(attributeHandle != NULL);
  
  if (listHandle->attrListHandle.state == NW_STAT_WBXML_ITERATE_MORE){
    parser = listHandle->attrListHandle.tiny_parser;
    offset = NW_TinyDom_AttrListHandle_iterate(&listHandle->attrListHandle);
    if (offset != 0){
      NW_DOM_AttributeHandle_initWithOffset(attributeHandle, parser, offset);
      return NW_STAT_WBXML_ITERATE_MORE;
    }
  }

  /*
   * Once we are done iterating over the initial set of attributes, 
   * next attribute node is located. A reference to this node is kept,
   * so that in subsequent call to this method we can proceed from there
   */
  
  NW_ASSERT(listHandle->node != NULL);

  listHandle->node = NW_TinyTree_findNextSibling(listHandle->node);
  parser = listHandle->attrListHandle.tiny_parser;
  if (parser == NULL)
    return NW_STAT_FAILURE; 

  while (listHandle->node){
    if (NW_TinyDom_Node_getType(listHandle->node) == T_DOM_NODE_ATTR){
      NW_DOM_AttributeHandle_initWithNode(attributeHandle, parser, listHandle->node);          
      return NW_STAT_WBXML_ITERATE_MORE;  
    }
    listHandle->node = NW_TinyTree_findNextSibling(listHandle->node);
  }
  return NW_STAT_WBXML_ITERATE_DONE;
}

NW_DOM_AttrVal_t *
NW_DOM_AttrVal_new(void)
{
  return (NW_DOM_AttrVal_t *) NW_Mem_Malloc(sizeof (NW_DOM_AttrVal_t));
}

/*
 * Initializes an AttrVal of type NW_DOM_ATTR_VAL_STRING
 * Returns NW_STAT_SUCCESS
 */

EXPORT_C NW_Status_t
NW_DOM_AttrVal_initFromString (NW_DOM_AttrVal_t *val, 
                               NW_String_t * string)
{
  NW_Status_t status;
  
  NW_ASSERT(val != NULL);
  NW_ASSERT(string != NULL);
  
  status = NW_DOM_AttrVal_setType(val, NW_DOM_ATTR_VAL_STRING);
  if (status != NW_STAT_SUCCESS)
    return status;
  val->component.string = *string;
  return NW_STAT_SUCCESS;
}

/*
 * Initializes an AttrVal of type NW_WBXML_ATTR_COMPONENT_ENTITY
 * Returns NW_STAT_SUCCESS
 */

NW_Status_t
NW_DOM_AttrVal_initFromEntity (NW_DOM_AttrVal_t *val, 
                               NW_Uint32 entity)
{
  NW_ASSERT(val != NULL);

  val->type = NW_WBXML_ATTR_COMPONENT_ENTITY;
  val->component.entity = entity;
  return NW_STAT_SUCCESS;
}


/*
 Initializes an AttrVal of type NW_WBXML_ATTR_COMPONENT_EXT
 Returns NW_STAT_SUCCESS or NW_STAT_FAILURE if args are not valid.

 Makes a shallow copy of str.
 */
NW_Status_t
NW_DOM_AttrVal_initFromExtension (NW_DOM_AttrVal_t *val, 
                                  NW_Uint16 token, 
                                  NW_String_t *str)
{
  NW_Uint8 t; /* 8-bit token */

  NW_ASSERT(val != NULL);
  NW_ASSERT(token != 0);

  val->type = NW_WBXML_ATTR_COMPONENT_INVALID;
  t = (NW_Uint8)token;
  if ((t == NW_WBXML_EXT_0)
      || (t == NW_WBXML_EXT_1)
      || (t == NW_WBXML_EXT_2)
      || (t == NW_WBXML_EXT_T_0)
      || (t == NW_WBXML_EXT_T_1)
      || (t == NW_WBXML_EXT_T_2)
      || (t == NW_WBXML_EXT_I_0)
      || (t == NW_WBXML_EXT_I_1)
      || (t == NW_WBXML_EXT_I_2)) {
    if ((t != NW_WBXML_EXT_0)
        && (t != NW_WBXML_EXT_1)
        && (t != NW_WBXML_EXT_2)
        && (str->storage == NULL)) {
      return NW_STAT_FAILURE;
    }
  val->type = NW_WBXML_ATTR_COMPONENT_EXT;
    val->component.ext.type = NW_TINYDOM_EXTENSION_TYPE_NORMAL;
  val->component.ext.token = token;
    if ((t == NW_WBXML_EXT_0)
        || (t == NW_WBXML_EXT_1)
        || (t == NW_WBXML_EXT_2)) {
      NW_String_initialize(&(val->component.ext.value.string), NULL, 0);
    } else {
      /* struct assignment, shallow copy */
      val->component.ext.value.string = *str;
    }
  return NW_STAT_SUCCESS;
}
  return NW_STAT_FAILURE;
}

NW_Status_t
NW_DOM_AttrVal_initFromExtensionInt (NW_DOM_AttrVal_t* val,
                                     NW_Uint16 token,
                                     NW_Uint32 x)
{
  NW_Uint8 t = (NW_Uint8)token;

  NW_ASSERT(val != NULL);

  if ((t == NW_WBXML_EXT_T_0)
      || (t == NW_WBXML_EXT_T_1)
      || (t == NW_WBXML_EXT_T_2)) {
    val->type = NW_WBXML_ATTR_COMPONENT_EXT;
    val->component.ext.type = NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER;
    val->component.ext.token = token;
    val->component.ext.value.x = x;
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_FAILURE;
}

/*
 * Initializes an AttrVal of type NW_WBXML_ATTR_COMPONENT_OPAQUE
 * Returns NW_STAT_SUCCESS
 */

NW_Status_t
NW_DOM_AttrVal_initFromOpaque (NW_DOM_AttrVal_t *val, 
                               NW_Uint32 length, 
                               NW_Byte* data)
{
  NW_ASSERT(val != NULL);
  NW_ASSERT(data != NULL);

  val->type = NW_WBXML_ATTR_COMPONENT_OPAQUE;
  val->component.opaque.data = data;
  val->component.opaque.length = length;
  return NW_STAT_SUCCESS;
}


/*
 * Initializes an AttrVal of type NW_WBXML_ATTR_COMPONENT_TOKEN
 * Returns NW_STAT_SUCCESS
 */

EXPORT_C NW_Status_t
NW_DOM_AttrVal_initFromToken (NW_DOM_AttrVal_t *val, NW_Uint16 token)
{
  NW_ASSERT(val != NULL); 

  val->type = NW_WBXML_ATTR_COMPONENT_TOKEN;
  val->component.value_token = (NW_Uint32)token;
  return NW_STAT_SUCCESS;
}


NW_Status_t
NW_DOM_AttrVal_delete (NW_DOM_AttrVal_t * attrVal)
{
  NW_ASSERT(attrVal != NULL);

  NW_Mem_Free(attrVal);

  return NW_STAT_SUCCESS;
}

/*
 * Returns the type of attribute value
 */

EXPORT_C NW_Uint16
NW_DOM_AttrVal_getType(NW_DOM_AttrVal_t *val)
{
  NW_ASSERT(val != NULL);
    
  switch(val->type)
  {
    case NW_WBXML_ATTR_COMPONENT_EXT:
      return NW_DOM_ATTR_VAL_EXTENSION;
    case NW_WBXML_ATTR_COMPONENT_STRING:
      return NW_DOM_ATTR_VAL_STRING;
    case NW_WBXML_ATTR_COMPONENT_ENTITY:
      return NW_DOM_ATTR_VAL_ENTITY;
    case NW_WBXML_ATTR_COMPONENT_TOKEN:
      return NW_DOM_ATTR_VAL_TOKEN;
    case NW_WBXML_ATTR_COMPONENT_OPAQUE:
      return NW_DOM_ATTR_VAL_OPAQUE;
    default: 
      return 0;
  }
}

/*
 * Sets the type of attribute value
 * Returns NW_STAT_SUCCESS
 */

/* TBD This is a very dangerous function and should be made internal
   use only (i.e., static)! */
NW_Status_t
NW_DOM_AttrVal_setType(NW_DOM_AttrVal_t *val, 
                       NW_Uint16 type)
{
  NW_ASSERT(val != NULL);
  
  switch(type)
  {
    case NW_DOM_ATTR_VAL_EXTENSION:
      val->type = NW_WBXML_ATTR_COMPONENT_EXT;
      break;
    case NW_DOM_ATTR_VAL_STRING:
      val->type = NW_WBXML_ATTR_COMPONENT_STRING;
      break;
    case NW_DOM_ATTR_VAL_ENTITY:
      val->type = NW_WBXML_ATTR_COMPONENT_ENTITY;
      break;
    case NW_DOM_ATTR_VAL_TOKEN:
      val->type = NW_WBXML_ATTR_COMPONENT_TOKEN;
      break;
    case NW_DOM_ATTR_VAL_OPAQUE:
      val->type = NW_WBXML_ATTR_COMPONENT_OPAQUE;
      break;
    default:
      return NW_STAT_BAD_INPUT_PARAM;
  }
  return NW_STAT_SUCCESS;
}

/*
 * Returns
 *    NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED - invalid encoding
 *    NW_STAT_DOM_NO_STRING_RETURNED - if the type of AttrVal was invalid
 *    NW_STAT_SUCCESS
 * 
 *  MODIFIED string (the value of NW_DOM_AttrVal)
 */

EXPORT_C NW_Status_t
NW_DOM_AttrVal_toString(NW_DOM_AttrVal_t *av, 
                        NW_String_t * string, 
                        NW_Uint32 encoding)
{
  NW_Status_t status;

  NW_ASSERT(av != NULL);
  NW_ASSERT(string != NULL);

  NW_String_initialize(string, NULL, 0);

  if ((NW_String_charsetValid(encoding)) != NW_STAT_SUCCESS)
    return NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED;

  switch (NW_DOM_AttrVal_getType(av))
  {
    case NW_DOM_ATTR_VAL_TOKEN:
      status = NW_String_tokenToString(av->component.value_token, string, encoding);
      if(status != NW_STAT_SUCCESS){
        return NW_STAT_DOM_NO_STRING_RETURNED;
      }
      return NW_STAT_SUCCESS;

    case NW_DOM_ATTR_VAL_STRING:
      {
        NW_Byte *storage = av->component.string.storage;
        status = NW_String_initialize(string, storage, encoding);
        if(status != NW_STAT_SUCCESS){
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        return NW_STAT_SUCCESS; 
      }

    case NW_DOM_ATTR_VAL_ENTITY:
      status = NW_String_entityToString(av->component.entity,
                                        string, encoding);
      if(status != NW_STAT_SUCCESS){
        return NW_STAT_DOM_NO_STRING_RETURNED;
      }
      return NW_STAT_SUCCESS; 
      
    case NW_DOM_ATTR_VAL_OPAQUE:
      return NW_STAT_DOM_NO_STRING_RETURNED;
    case NW_DOM_ATTR_VAL_EXTENSION:
      {
        NW_Uint8 t; /* 8-bit token */

        t = (NW_Uint8)(av->component.ext.token);
        if ((t == NW_WBXML_EXT_0)
            || (t == NW_WBXML_EXT_1)
            || (t == NW_WBXML_EXT_2)) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        if (av->component.ext.value.string.storage == NULL) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        /* struct assignment, shallow copy */
        *string = av->component.ext.value.string;
        return NW_STAT_SUCCESS; 
      }
    default:
      return NW_STAT_DOM_NO_STRING_RETURNED;
  }
}

/*
 * If attribute value is of type NW_DOM_ATTR_VAL_ENTITY 
 * return the associated entity
 */

NW_Uint32
NW_DOM_AttrVal_getEntity(NW_DOM_AttrVal_t *val)
{
  NW_ASSERT(val != NULL);
 
  if (NW_DOM_AttrVal_getType(val) != NW_DOM_ATTR_VAL_ENTITY)
    return val->component.entity;
  return 0;
}

NW_Status_t
NW_DOM_AttrVal_getExtensionToken(NW_DOM_AttrVal_t* val,
                                 NW_Uint16* token)
{
  if (NW_DOM_AttrVal_getType(val) != NW_DOM_ATTR_VAL_EXTENSION) {
    return NW_STAT_FAILURE;
  }
  *token = (NW_Uint16)(val->component.ext.token);
  return NW_STAT_SUCCESS;
}

/* If attribute value is of type NW_DOM_TEXT_ITEM_EXTENSION returns token
 otherwise returns 0 and returns str->length = 0, str->storage = NULL.

 All returned strings are shallow copies and the only proper way to
 free these is to call NW_String_delete(pStr). */
EXPORT_C NW_Uint16
NW_DOM_AttrVal_getExtension(NW_DOM_AttrVal_t *val, 
                            NW_String_t *str)
{
  NW_Uint16 fqToken;
  NW_Uint8 t; /* 8-bit token */

  NW_ASSERT (val != NULL);
  NW_ASSERT (str != NULL);

  NW_String_initialize(str, NULL, 0);

  if (NW_DOM_AttrVal_getType(val) != NW_DOM_ATTR_VAL_EXTENSION) {
    return 0;
  }

  /* TBD If it could be gauranteed that the extension was constructed
     correctly then it would be enough to just unconditionally
     shallowCopy. */

  fqToken = (NW_Uint16)(val->component.ext.token);
  t = (NW_Uint8)fqToken;

  if (((t == NW_WBXML_EXT_T_0)
       || (t == NW_WBXML_EXT_T_1)
       || (t == NW_WBXML_EXT_T_2))
      && (val->component.ext.type
         == NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER)) {
    /* This function is not for this case: use "getExtensionInt" form. */
    return 0;
  }

  if ((t != NW_WBXML_EXT_0) && (t != NW_WBXML_EXT_1) && (t != NW_WBXML_EXT_2)) {
    /* By fiat we do nothing with the single byte extension tokens.  A
     more correct version might be to callback to the client to
     request a mapping to a client determined string.

     In the normal case where EXT_T refers to the string table, both
     the EXT_T and EXT_I forms are represented by an explicit pointer
     to the string value since for the EXT_T form we don't have a
     pointer to the string table anyway. */
     NW_String_shallowCopy(str, &val->component.ext.value.string);
  }
  return fqToken;
}

NW_Status_t
NW_DOM_AttrVal_getExtensionInt(NW_DOM_AttrVal_t* val,
                               NW_Uint32* x)
{
  NW_Uint16 fqToken;
  NW_Uint8 t; /* 8-bit token */

  NW_ASSERT(val != NULL);
  NW_ASSERT(x != NULL);

  if ((NW_DOM_AttrVal_getType(val) != NW_DOM_ATTR_VAL_EXTENSION)
      || ((val->component.ext.type
           != NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER)))
  {
    return NW_STAT_FAILURE;
  }

  fqToken = (NW_Uint16)(val->component.ext.token);
  t = (NW_Uint8)fqToken;
  if ((t == NW_WBXML_EXT_T_0)
      || (t == NW_WBXML_EXT_T_1)
      || (t == NW_WBXML_EXT_T_2)) {
    *x = val->component.ext.value.x;
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_FAILURE;
}

/*
 * If attribute value is of type NW_DOM_ATTR_VAL_OPAQUE 
 * returns the associated opaque data and length.
 */

/* Note: change the signature so both values are out params? */

EXPORT_C NW_Byte *
NW_DOM_AttrVal_getOpaque(NW_DOM_AttrVal_t *val, 
                         NW_Uint32 *opaqueLen)
{
  NW_ASSERT(val != NULL);
  NW_ASSERT(opaqueLen != NULL);

  if (NW_DOM_AttrVal_getType(val) != NW_DOM_ATTR_VAL_OPAQUE)
    return NULL;
  
  *opaqueLen = val->component.opaque.length;
  return val->component.opaque.data;
}

/*
 * If attribute value is of type NW_WBXML_ATTR_COMPONENT_TOKEN, 
 * returns the associated token
 */

NW_Uint16 
NW_DOM_AttrVal_getToken(NW_DOM_AttrVal_t *val)
{
  NW_ASSERT(val != NULL);

  if (val->type == NW_WBXML_ATTR_COMPONENT_TOKEN)
    return (NW_Uint16)val->component.value_token;
  
  return 0;
}


/*****************************************
* ATTRIBUTE VAL ITERATOR
* iterates over the values of an attribute
******************************************/

EXPORT_C NW_Uint32
NW_DOM_AttributeHandle_getEncoding(NW_DOM_AttributeHandle_t *handle)
{
  NW_ASSERT(handle != NULL);

  return handle->tinyHandle.tlit.tiny_parser->dom_tree->doc->charset;
}
/* the following functions are not referenced, 8/12/01 */
/*
NW_Uint32
NW_DOM_AttributeHandle_getPublicId(NW_DOM_AttributeHandle_t *handle)
{
  NW_ASSERT(handle != NULL);

  return handle->tinyHandle.tlit.tiny_parser->dom_tree->doc->publicid;
}


NW_Status_t
NW_DOM_AttributeHandle_getDocType(NW_DOM_AttributeHandle_t *handle, 
                                  NW_String_t *str)
{
  NW_String_t *docType;

  NW_ASSERT(handle != NULL);
  NW_ASSERT(str != NULL);

  docType = handle->tinyHandle.tlit.tiny_parser->dom_tree->doc->doc_type;
  return NW_String_shallowCopy(str, docType);
}
*/

/*
 * Allocates memory for NW_DOM_AttributeHandle
 * Returns NULL in case of failure to do so
 */

NW_DOM_AttributeHandle_t *
NW_DOM_AttributeHandle_new(void)
{
  NW_DOM_AttributeHandle_t *handle =
    (NW_DOM_AttributeHandle_t *) NW_Mem_Malloc(sizeof (NW_DOM_AttributeHandle_t));
  if (handle == NULL){
    return NULL;
  }
  handle->node = NULL;
  return handle;
}

/* Initializes an attribute handle with an attribute node.  Note that
 only some attributes have nodes of their own. Such attributes are
 initilized by this method.  Also, this method initializes the
 attribute handle so that it may be used by
 NW_DOM_AttributeHandle_addVal().

 Returns NW_STAT_SUCCESS */

NW_Status_t
NW_DOM_AttributeHandle_initWithNode(NW_DOM_AttributeHandle_t *handle, 
                                    NW_TinyDom_Parser_t *parser, 
                                    NW_TinyTree_Node_t *node)
{
  NW_TinyTree_Offset_t offset;

  NW_ASSERT(handle != NULL);
  NW_ASSERT(node != NULL);
  NW_ASSERT(parser != NULL);

  offset = NW_TinyTree_Node_getSourceOffset(node);
  handle->node = node;
  NW_TinyDom_AttributeHandle_init(&handle->tinyHandle, parser, offset);
  return NW_STAT_SUCCESS;
}

/*
 * Initializes and Attribute Handle with a offset.
 * Returns NW_STAT_SUCCESS
 */

NW_Status_t
NW_DOM_AttributeHandle_initWithOffset(NW_DOM_AttributeHandle_t *handle, 
                                      NW_TinyDom_Parser_t *parser, 
                                      NW_TinyTree_Offset_t offset)
{
  NW_ASSERT(handle != NULL);
  NW_ASSERT(parser != NULL);
  
  handle->node = NULL;
  NW_TinyDom_AttributeHandle_init(&handle->tinyHandle, parser, offset);
  return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_DOM_AttributeHandle_initWithStartToken(NW_DOM_AttributeHandle_t* handle,
                                          NW_TinyDom_Parser_t* parser,
                                          NW_Uint16 fqToken)
{
  NW_ASSERT(handle != NULL);
  NW_ASSERT(parser != NULL);

  handle->node = NULL;
  return  NW_TinyDom_AttributeHandle_initWithStartToken(&handle->tinyHandle,
                                                        &handle->node,
                                                        parser,
                                                        fqToken);
}

EXPORT_C NW_Status_t
NW_DOM_AttributeHandle_addVal(NW_DOM_AttributeHandle_t* handle,
                              NW_DOM_AttrVal_t* value)
{
  NW_ASSERT(handle != NULL);
  NW_ASSERT(value != NULL);

  return NW_TinyDom_AttributeHandle_addVal(&(handle->tinyHandle),
                                           handle->node,
                                           value);
}

NW_Status_t
NW_DOM_AttributeHandle_delete(NW_DOM_AttributeHandle_t *handle)
{
  NW_ASSERT(handle != NULL);

  NW_Mem_Free(handle);
  return NW_STAT_SUCCESS;
}

/*
 * Returns the attribute name (without prefix if any)
 * Returns
 *  NW_STAT_OUT_OF_MEMORY
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_SUCCESS
 *
 * Modifies attributeName
 */

EXPORT_C NW_Status_t
NW_DOM_AttributeHandle_getName(NW_DOM_AttributeHandle_t *handle, 
                               NW_String_t* attributeName)
{
  NW_Status_t status;
  NW_Byte *p; /* pointer to find '=' sign location */
  NW_Uint32 encoding;
  NW_String_t temp;

  NW_ASSERT(handle != NULL);
  NW_ASSERT(attributeName != NULL);

  encoding = NW_DOM_AttributeHandle_getEncoding(handle);

  status = NW_DOM_AttributeHandle_getNameWithPrefix(handle, attributeName);
  if (status != NW_STAT_SUCCESS)
    return status;

  /* 
   * Before returning the attribute name, strip off any
   * part that is actually the attribute value's prefix.
   * For example, if name is "href=http://", the name
   * that should be returned is only "href".
   */
  if ((p = NW_String_findChar(attributeName, '=', encoding)) != NULL)
  {
   /* There an attribute value_prefix - so strip prefix
    * Create a new string without the '=...' or there
    * will be wasted memory.
    */
    status = NW_String_deepCopy(&temp, attributeName);
    if (status != NW_STAT_SUCCESS) {
      return status;
    }
    /* set rest of buffer to NULL */
    p = NW_String_findChar(&temp, '=', encoding);
    NW_Mem_memset(p, 0, (NW_Uint32)(temp.storage + NW_String_getByteCount(&temp) - p));

    /* Free up any user-owned storage in original string */
    NW_String_deleteStorage(attributeName);
    status = NW_String_initialize(attributeName, temp.storage, encoding);
    NW_String_setUserOwnsStorage(attributeName);
  }

  return NW_STAT_SUCCESS;
}

/*
 * Returns the token for the attribute
 */

EXPORT_C NW_Uint16 
NW_DOM_AttributeHandle_getToken(NW_DOM_AttributeHandle_t *handle)
{
  NW_ASSERT(handle != NULL);
  
  return (NW_Uint16)NW_TinyDom_AttributeHandle_getToken(&handle->tinyHandle);
}

/*
 * Returns the attribute prefix (for e.g. href="http://"
 * will return "http://"
 * Returns
 *  NW_STAT_OUT_OF_MEMORY
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_DOM_NO_VALUE_PREFIX
 *  NW_STAT_SUCCESS
 *
 * Modifies prefixName
 */

EXPORT_C NW_Status_t
NW_DOM_AttributeHandle_getValuePrefix(NW_DOM_AttributeHandle_t *handle,
                                      NW_String_t* prefixName)
{
  NW_Status_t status;
  NW_Byte *p = NULL;
  NW_Uint32 encoding; 
  
  NW_ASSERT(handle != NULL);
  NW_ASSERT(prefixName != NULL);

  encoding = NW_DOM_AttributeHandle_getEncoding(handle);
  status = NW_DOM_AttributeHandle_getNameWithPrefix(handle, prefixName);

  if (status != NW_STAT_SUCCESS)
    return status;
  
  if ((p = NW_String_findChar(prefixName, '=', encoding)) != NULL)
  { 
    NW_Uint32 len = 0;
    NW_Byte *prefix = NULL;
    /* Now make a new string with just the value's prefix */
    p++;
    
    if(NW_String_charBuffGetLength(p, encoding, &len) < 0){
      return NW_STAT_DOM_NO_STRING_RETURNED;
    }
    
    prefix = (NW_Byte *) NW_Mem_Malloc(len);
    if (prefix == NULL)
      return NW_STAT_OUT_OF_MEMORY;
    
    NW_Mem_memcpy(prefix, p, len);
    /* Free up any user-owned storage in old name */
    NW_String_deleteStorage(prefixName);
    status = NW_String_initialize(prefixName, prefix, encoding);
    if (status != NW_STAT_SUCCESS){
      return NW_STAT_DOM_NO_STRING_RETURNED;
    }
    return NW_String_setUserOwnsStorage(prefixName);
  }else{
    NW_String_deleteStorage(prefixName);
    return NW_STAT_DOM_NO_VALUE_PREFIX;
  }
}

/*
 * Returns the attribute name with prefix
 * Returns
 *  NW_STAT_OUT_OF_MEMORY
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_SUCCESS
 *
 * Modifies attributeName
 */

NW_Status_t
NW_DOM_AttributeHandle_getNameWithPrefix(NW_DOM_AttributeHandle_t* handle, 
                                         NW_String_t* attrName)
{
  NW_Status_t status;
  NW_Uint32 token;
  NW_Uint32 encoding;
  
  NW_ASSERT(handle != NULL);
  NW_ASSERT(attrName != NULL);

  token = NW_TinyDom_AttributeHandle_getToken(&handle->tinyHandle);
  encoding = NW_DOM_AttributeHandle_getEncoding(handle);
  status = NW_TinyDom_AttributeHandle_getName(&handle->tinyHandle, attrName);

  if (status == NW_STAT_SUCCESS)
    return NW_STAT_SUCCESS;

  if (status == NW_STAT_WBXML_NO_NAME){
    status = NW_String_tokenToString(token, attrName, encoding);
    if(status == NW_STAT_OUT_OF_MEMORY){
      return status;
    }
    if(status != NW_STAT_SUCCESS){
      return NW_STAT_DOM_NO_STRING_RETURNED;
    }
    return NW_STAT_SUCCESS; 
  }
  return NW_STAT_DOM_NO_STRING_RETURNED;
}

/* 
 * Returns 
 *   NW_STAT_WBXML_ITERATE_MORE
 *   NW_STAT_WBXML_ITERATE_DONE
 *
 * Modifies attrVal (the next NW_DOM_AttrVal in the attribute value list
 */

EXPORT_C NW_Status_t
NW_DOM_AttributeHandle_getNextVal(NW_DOM_AttributeHandle_t *handle, 
                                  NW_DOM_AttrVal_t *attrVal)
{
  NW_TinyTree_Offset_t offset;

  NW_ASSERT(handle != NULL);
  NW_ASSERT(attrVal != NULL);

  offset = NW_TinyDom_AttributeHandle_iterateValues(&handle->tinyHandle, 
                                                    attrVal);

  if (offset == 0)
    return NW_STAT_WBXML_ITERATE_DONE;

  return NW_STAT_WBXML_ITERATE_MORE;  
}

/*
 * Returns
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_SUCCESS
 * 
 * output valueString is the complete string representation of attribute value
 */

EXPORT_C NW_Status_t 
NW_DOM_AttributeHandle_getValue(NW_DOM_AttributeHandle_t *handle, 
                                NW_String_t* valueString)
{
  NW_String_t str;
  NW_Uint32 encoding;
  NW_Status_t status;
  NW_Bool prefixFound = NW_FALSE;

  NW_ASSERT(handle != NULL);
  NW_ASSERT(valueString != NULL);

  encoding = NW_DOM_AttributeHandle_getEncoding(handle);

  valueString->storage = NULL;
  valueString->length = 0;

  /*
   * If this attribute's name includes the an attribute value
   * prefix (e.g. name is something like "href=http://"), then
   * the attribute's value prefix must be extracted.
   */
  status = NW_DOM_AttributeHandle_getValuePrefix(handle, &str);
  if (status == NW_STAT_SUCCESS) {
    status = NW_String_deepCopy(valueString, &str);
    
    if (status != NW_STAT_SUCCESS){
	    NW_String_deleteStorage(&str);
      if (status == NW_STAT_OUT_OF_MEMORY)
      {
        return NW_STAT_OUT_OF_MEMORY;
      }
      else
      {
        return NW_STAT_DOM_NO_STRING_RETURNED;
      }
    }
	prefixFound = NW_TRUE;
  }
  else if(status != NW_STAT_DOM_NO_VALUE_PREFIX){
    return status;
  }
  
  status = NW_DOM_WbxmlComponent_sequenceToString(NW_WBXML_ATTRIBUTE_VALUE_COMPONENT,
                                                &(handle->tinyHandle),
                                                encoding,valueString);

  if( (status == NW_STAT_DOM_NO_STRING_RETURNED) &&
	  (valueString->length == 0) &&
	  (prefixFound == NW_TRUE) )
  {

	  status = NW_String_deepCopy(valueString, &str);
  }

  NW_String_deleteStorage(&str);
  return status;

}

/*
 * PROCESSING_INSTRUCTION NODE
 */

/*
 * Gets the name of the target for this processing instruction
 * Returns one of:
 *  NW_STAT_DOM_NODE_TYPE_ERR - not a DOM_ProcessingInstructionNode_t node 
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_SUCCESS
 */

NW_Status_t
NW_DOM_ProcessingInstructionNode_getTarget(NW_DOM_ProcessingInstructionNode_t* node, 
                                           NW_String_t *target)
{
  NW_DOM_AttributeHandle_t attrHandle;
  NW_Status_t status;

  NW_ASSERT(node != NULL);
  NW_ASSERT(target != NULL);

  status = NW_DOM_ProcessingInstructionNode_getHandle(node, &attrHandle);
  if (status != NW_STAT_SUCCESS)
    return status;

  return NW_DOM_AttributeHandle_getName(&attrHandle, target);
}

/*
 * Returns the token associated with the PI node (WBXML specific
 */

EXPORT_C NW_Uint16
NW_DOM_ProcessingInstructionNode_getTargetToken (NW_DOM_ProcessingInstructionNode_t* node)
{
  NW_DOM_AttributeHandle_t attrHandle;
  NW_Status_t status;

  NW_ASSERT(node != NULL);

  status = NW_DOM_ProcessingInstructionNode_getHandle(node, &attrHandle);

  if (status != NW_STAT_SUCCESS)
    return (NW_Uint16)0;

  return NW_DOM_AttributeHandle_getToken(&attrHandle);
}

/*
 * Returns one of:
 * NW_STAT_DOM_NODE_TYPE_ERR - not a NW_DOM_ProcessingInstructionNode_t node 
 * NW_STAT_DOM_NO_STRING_RETURNED 
 * NW_STAT_SUCCESS
 */

NW_Status_t
NW_DOM_ProcessingInstructionNode_getData(NW_DOM_ProcessingInstructionNode_t* node, 
                                         NW_String_t *data)
{
  NW_DOM_AttributeHandle_t attrHandle;
  NW_Status_t status;

  NW_ASSERT(node != NULL);
  NW_ASSERT(data != NULL);

  status = NW_DOM_ProcessingInstructionNode_getHandle(node, &attrHandle);

  if (status != NW_STAT_SUCCESS)
    return status;

  return NW_DOM_AttributeHandle_getValue(&attrHandle, data);
}


/*
 * Gets the attribute handle associated with the node
 * Returns one of:
 *  NW_STAT_DOM_NODE_TYPE_ERR - not a processing instruction node 
 *  NW_STAT_SUCCESS
 * Modifies handle
 */

NW_Status_t
NW_DOM_ProcessingInstructionNode_getHandle(NW_DOM_ProcessingInstructionNode_t* node, 
                                           NW_DOM_AttributeHandle_t *handle)
{
  NW_TinyTree_t* tiny_tree;
  NW_DOM_Node_t *child;

  NW_ASSERT(node != NULL);
  NW_ASSERT(handle != NULL);

  if (NW_DOM_Node_getNodeType(node) != NW_DOM_PROCESSING_INSTRUCTION_NODE)
    return NW_STAT_DOM_NODE_TYPE_ERR;

  tiny_tree = NW_TinyTree_Node_findTree(node);
  child = NW_TinyTree_findFirstChild(node);

  while(child){
    if (NW_TinyDom_Node_getType(child) == T_DOM_NODE_ATTR){
      break;
    }
    child = NW_TinyTree_findNextSibling(child);
  }
  if (child) {
    return NW_DOM_AttributeHandle_initWithNode(handle,
                                               NW_TinyDom_getParser(tiny_tree),
                                               child);
  }
  return NW_STAT_DOM_NODE_TYPE_ERR;
}
