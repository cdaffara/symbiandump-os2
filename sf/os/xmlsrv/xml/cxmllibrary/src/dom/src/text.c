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
#include <xml/cxml/nw_tinytree.h>
#include <xml/cxml/nw_dom_text.h>
#include <xml/cxml/nw_dom_document.h>

/*
 * Returns
 *  NW_STAT_DOM_NODE_TYPE_ERR - not a text node
 *  NW_STAT_SUCCESS
 *
 */

EXPORT_C NW_Status_t
NW_DOM_TextNode_getTextItemIterator(NW_DOM_TextNode_t *node,
                                    NW_DOM_TextItemIterator_t *iterator)
{
  NW_TinyTree_t *tiny_tree;

  NW_ASSERT(node != NULL);
  NW_ASSERT(iterator != NULL);

  if ((NW_DOM_Node_getNodeType(node) != NW_DOM_TEXT_NODE)
      && (NW_DOM_Node_getNodeType(node) != NW_DOM_COMMENT_NODE)
      && (NW_DOM_Node_getNodeType(node) != NW_DOM_CDATA_SECTION_NODE)) {
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  tiny_tree = NW_TinyTree_Node_findTree(node);
  NW_TinyDom_TextHandle_init(iterator,
                             NW_TinyDom_getParser(tiny_tree),
                             NW_TinyTree_Node_getSourceOffset(node));

  return NW_STAT_SUCCESS;
}

/*
 * Returns
 *  NW_STAT_DOM_NODE_TYPE_ERR - not a text node
 *  NW_STAT_OUT_OF_MEMORY - unable to allocate memory for string storage
 *  NW_STAT_DOM_NO_STRING_RETURNED - If string storage is not assigned
 *  NW_STAT_SUCCESS
 *
 * MODIFIED valueString - the value of the node
 */

EXPORT_C NW_Status_t
NW_DOM_TextNode_getData(NW_DOM_TextNode_t *node,
                        NW_String_t *valueString)
{
  NW_Status_t status;
  NW_DOM_TextItemIterator_t iterator;
  NW_String_t str;
  NW_DOM_TextItem_t item;
  NW_DOM_DocumentNode_t *docNode;
  NW_Uint32 encoding;

  NW_ASSERT(node != NULL);
  NW_ASSERT(valueString != NULL);

  docNode = NW_DOM_Node_getOwnerDocument(node);
  encoding = NW_DOM_DocumentNode_getCharacterEncoding(docNode);

  if (NW_DOM_Node_getNodeType(node) != NW_DOM_TEXT_NODE){
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  /* Initializes the handle with values*/

  status = NW_DOM_TextNode_getTextItemIterator(node, &iterator);
  if (status != NW_STAT_SUCCESS){
    return status;
  }

  valueString->length = 0;
  valueString->storage = NULL;
  NW_String_setUserOwnsStorage(valueString);

  while (NW_DOM_TextItemIterator_getNextTextItem(&iterator, &item)
         == NW_STAT_WBXML_ITERATE_MORE){

    status = NW_DOM_TextItem_toString(&item, &str, encoding);

    if (status != NW_STAT_SUCCESS){
      return status;
    }

    if ((valueString->length == 0) || (valueString->storage == NULL)){
      status = NW_String_deepCopy(valueString, &str);
    }
    else{
      status = NW_String_concatenate(valueString, &str, encoding);
    }

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
  }

  NW_String_deleteStorage(&str);
  return NW_STAT_SUCCESS;
}


/*
 * This method creates a new TextNode and replaces the previous one
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_SUCCESS
 */

EXPORT_C NW_Status_t
NW_DOM_TextNode_setDataFromTextItem(NW_DOM_TextNode_t** node,
                                    NW_DOM_TextItem_t *val)
{
  NW_DOM_Node_t *parent = NW_DOM_Node_getParentNode(*node);
  NW_DOM_DocumentNode_t *doc = NW_DOM_Node_getOwnerDocument(*node);
  NW_DOM_TextNode_t *newNode = NULL;
  NW_Status_t status;

  if ((node == NULL) || (val == NULL) || (parent == NULL) || (doc == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  newNode = NW_DOM_DocumentNode_createTextNodeWithTextItem(doc, val);
  if (newNode == NULL)
    return NW_STAT_BAD_INPUT_PARAM;

  status = NW_DOM_Node_replaceChild(parent, newNode, *node);
  if (status != NW_STAT_SUCCESS)
    return status;
  *node = newNode;
  return NW_STAT_SUCCESS;
}


EXPORT_C NW_Status_t
NW_DOM_TextNode_addDataFromTextItem(NW_DOM_TextNode_t* node,
                                    NW_DOM_TextItem_t *val)
{
  NW_TinyTree_t* tinyTree;
  NW_TinyDom_Tree_t* domTree;
  NW_DOM_DocumentNode_t *docNode;
  NW_Uint32 encoding;

  if ((node == NULL) || (val == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  tinyTree = NW_TinyTree_Node_findTree(node);
  docNode = NW_DOM_Node_getOwnerDocument(node);
  if ((tinyTree == NULL) || (docNode == NULL)) {
    return NW_STAT_FAILURE;
  }
  domTree = NW_TinyDom_getTree(tinyTree);
  if (domTree == NULL) {
    return NW_STAT_FAILURE;
  }
  encoding = NW_DOM_DocumentNode_getCharacterEncoding(docNode);

  return NW_TinyDom_addDataFromTextItem(tinyTree, domTree, node,
                                        val, encoding);
}

/*
 * This method creates a new TextNode and replaces the previous one
 * Returns
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_SUCCESS
 */

EXPORT_C NW_Status_t
NW_DOM_TextNode_setData(NW_DOM_TextNode_t** node, NW_String_t *val)
{
  NW_DOM_Node_t *parent = NW_DOM_Node_getParentNode(*node);
  NW_DOM_DocumentNode_t *doc = NW_DOM_Node_getOwnerDocument(*node);
  NW_DOM_TextNode_t *newNode = NULL;
  NW_Status_t status;

  if ((node == NULL) || (val == NULL) || (parent == NULL) || (doc == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  newNode = NW_DOM_DocumentNode_createTextNode(doc, val);

  if (newNode == NULL){
    /* TBD is this correct?  maybe it is out-of-memory? */
    return NW_STAT_BAD_INPUT_PARAM;
  }
  /* TBD replaceChild is buggy because it may return the child as the status! */
  status = NW_DOM_Node_replaceChild(parent, newNode, *node);

  if (status != NW_STAT_SUCCESS){
    return status;
  }
  /* TBD isn't there a memory leak when the old child is not deleted? */

  *node = newNode;
  return NW_STAT_SUCCESS;
}


/**
 * TextItem methods
 **/


NW_DOM_TextItem_t *
NW_DOM_TextItem_new(void)
{
  return (NW_DOM_TextItem_t *) NW_Mem_Malloc(sizeof (NW_DOM_TextItem_t));
}

/*
 * Initializes a Text Item of type NW_DOM_TEXT_ITEM_STRING
 * Returns NW_STAT_SUCCESS
 */

EXPORT_C NW_Status_t
NW_DOM_TextItem_initFromString (NW_DOM_TextItem_t *item,
                                NW_String_t * string)
{
  NW_ASSERT(item != NULL);
  NW_ASSERT(string != NULL);

  item->type = NW_WBXML_ATTR_COMPONENT_STRING;
  item->component.string = *string;
  return NW_STAT_SUCCESS;
}

/**
 * Initializes a Text Item of type NW_DOM_TEXT_ITEM_ENTITY
 * Returns NW_STAT_SUCCESS
 */

EXPORT_C NW_Status_t
NW_DOM_TextItem_initFromEntity (NW_DOM_TextItem_t *item, NW_Uint32 entity)
{

  NW_ASSERT(item != NULL);

  item->type = NW_WBXML_ATTR_COMPONENT_ENTITY;
  item->component.entity = entity;
  return NW_STAT_SUCCESS;
}


/*
 Initializes a Text Item of type NW_DOM_TEXT_ITEM_EXTENSION
 Returns NW_STAT_SUCCESS or NW_STAT_FAILURE if args are not valid.

 Makes a shallow copy of str.
 */
EXPORT_C NW_Status_t
NW_DOM_TextItem_initFromExtension (NW_DOM_TextItem_t *item,
                                   NW_Uint16 token,
                                   NW_String_t *str)
{
  /* Text items and AttrVals are nearly the same thing. */
  return NW_DOM_AttrVal_initFromExtension(item, token, str);
}

/*
 Initializes a Text Item of type NW_DOM_TEXT_ITEM_EXTENSION for the
 particular case of EXT_T_[0,1,2] where the associated integer value
 is not a reference into the string table.

 Returns NW_STAT_SUCCESS or NW_STAT_FAILURE if args are not valid.
*/
EXPORT_C NW_Status_t
NW_DOM_TextItem_initFromExtensionInt (NW_DOM_TextItem_t *item,
                                      NW_Uint16 token,
                                      NW_Uint32 x)
{
  /* Text items and AttrVals are nearly the same thing. */
  return NW_DOM_AttrVal_initFromExtensionInt(item, token, x);
}


/*
 * Initializes a Text Item of type NW_DOM_TEXT_ITEM_OPAQUE
 * Returns NW_STAT_SUCCESS
 */

EXPORT_C NW_Status_t
NW_DOM_TextItem_initFromOpaque (NW_DOM_TextItem_t *item,
                                NW_Uint32 length,
                                NW_Byte* data)
{
  NW_ASSERT(item != NULL);
  NW_ASSERT(data != NULL);

  item->type = NW_WBXML_ATTR_COMPONENT_OPAQUE;
  item->component.opaque.length = length;
  item->component.opaque.data = data;

  return NW_STAT_SUCCESS;
}


NW_Status_t
NW_DOM_TextItem_delete(NW_DOM_TextItem_t *textItem)
{
  NW_ASSERT(textItem != NULL);

  NW_Mem_Free(textItem);

  return NW_STAT_SUCCESS;
}

/*
 * Returns the type of Text Item
 */

EXPORT_C NW_DOM_TextItemType_t
NW_DOM_TextItem_getType(NW_DOM_TextItem_t *item)
{

  NW_ASSERT(item != NULL);

  switch(item->type)
  {
    case NW_WBXML_ATTR_COMPONENT_EXT:
      return NW_DOM_TEXT_ITEM_EXTENSION;
    case NW_WBXML_ATTR_COMPONENT_STRING:
      return NW_DOM_TEXT_ITEM_STRING;
    case NW_WBXML_ATTR_COMPONENT_ENTITY:
      return NW_DOM_TEXT_ITEM_ENTITY;
    case NW_WBXML_ATTR_COMPONENT_OPAQUE:
      return NW_DOM_TEXT_ITEM_OPAQUE;
    default:
      return 0;
  }
}

/*
 * Sets the type of Text Item
 */

/* TBD This is a very dangerous function and should be made internal
   use only (i.e., static)! */
NW_Status_t
NW_DOM_TextItem_setType(NW_DOM_TextItem_t *item,
                        NW_DOM_TextItemType_t type)
{
  NW_ASSERT(item != NULL);

  switch(type)
  {
    case NW_DOM_TEXT_ITEM_EXTENSION:
      item->type = NW_WBXML_ATTR_COMPONENT_EXT;
      break;
    case NW_DOM_TEXT_ITEM_STRING:
      item->type = NW_WBXML_ATTR_COMPONENT_STRING;
      break;
    case NW_DOM_TEXT_ITEM_OPAQUE:
      item->type = NW_WBXML_ATTR_COMPONENT_OPAQUE;
      break;
    case NW_DOM_TEXT_ITEM_ENTITY:
      item->type = NW_WBXML_ATTR_COMPONENT_ENTITY;
      break;
    default:
      return NW_STAT_BAD_INPUT_PARAM;
  }
  return NW_STAT_SUCCESS;
}

/*
 * Returns
 *  NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED - if encoding is not supported
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_SUCCESS
 *
 * MODIFIED - string - the String representation of item
 */

EXPORT_C NW_Status_t
NW_DOM_TextItem_toString(NW_DOM_TextItem_t *item,
                         NW_String_t *string,
                         NW_Uint32 encoding)
{
  NW_Status_t status;

  NW_ASSERT(item != NULL);
  NW_ASSERT(string != NULL);

  NW_String_initialize(string, NULL, 0);

  if ((NW_String_charsetValid(encoding)) != NW_STAT_SUCCESS){
    return NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED;
  }

  switch (NW_DOM_TextItem_getType(item))
  {
    case NW_DOM_TEXT_ITEM_STRING:
      {
        NW_Byte *storage = item->component.string.storage;
        status = NW_String_initialize(string, storage, encoding);
        if(status == NW_STAT_SUCCESS){
          return NW_STAT_SUCCESS;
        }
        else{
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
      }
    case NW_DOM_TEXT_ITEM_ENTITY:
      status = NW_String_entityToString(item->component.entity,
                                        string, encoding);
        if(status == NW_STAT_SUCCESS){
          return NW_STAT_SUCCESS;
        }
        else{
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }

    case NW_DOM_TEXT_ITEM_OPAQUE:
      return NW_STAT_DOM_NO_STRING_RETURNED;
    case NW_DOM_TEXT_ITEM_EXTENSION:
      {
        NW_Uint8 t; /* 8-bit token */

        t = (NW_Uint8)(item->component.ext.token);
        if ((t == NW_WBXML_EXT_0)
            || (t == NW_WBXML_EXT_1)
            || (t == NW_WBXML_EXT_2)) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        if ((item->component.ext.type
             == NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER)
            && ((t == NW_WBXML_EXT_T_0)
                || (t == NW_WBXML_EXT_T_1)
                || (t == NW_WBXML_EXT_T_2))) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        if (item->component.ext.value.string.storage == NULL) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        /* struct assignment, shallow copy */
        *string = item->component.ext.value.string;
        return NW_STAT_SUCCESS;
      }
    default:
      return NW_STAT_DOM_NO_STRING_RETURNED;
  }
}


EXPORT_C NW_Uint32
NW_DOM_TextItem_getEntity(NW_DOM_TextItem_t *item)
{
  NW_ASSERT(item != NULL);

  if (NW_DOM_TextItem_getType(item) != NW_DOM_TEXT_ITEM_ENTITY){
    return 0;
  }

  return item->component.entity;
}

/*
 * If TextItem is of type NW_DOM_TEXT_ITEM_STRING returns NW_STAT_SUCCESS
 * and modifies string.
 */

EXPORT_C NW_Status_t
NW_DOM_TextItem_getString(NW_DOM_TextItem_t *item,
                          NW_String_t *string)
{
  NW_ASSERT(item != NULL);
  NW_ASSERT(string != NULL);

  if (NW_DOM_TextItem_getType(item) != NW_DOM_TEXT_ITEM_STRING){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  return NW_String_copy(string, &item->component.string);
}

EXPORT_C NW_Status_t
NW_DOM_TextItem_getExtensionToken(NW_DOM_TextItem_t* pItem,
                                  NW_Uint16* pX)
{
  /* Text items and AttrVals are nearly the same thing. */
  return NW_DOM_AttrVal_getExtensionToken(pItem, pX);
}

/* If TextItem is of type NW_DOM_TEXT_ITEM_EXTENSION and extension
 isn't EXT_T_[0,1,2] in non string table reference form, returns token
 otherwise returns 0 and returns str->length = 0, str->storage = NULL.

 All returned strings are shallow copies and the only proper way to
 free these is to call NW_String_delete(pStr). */
EXPORT_C NW_Uint16
NW_DOM_TextItem_getExtension(NW_DOM_TextItem_t *item,
                             NW_String_t *str)
{
  NW_Uint16 fqToken;
  NW_Uint8 t; /* 8-bit token */

  NW_ASSERT(item != NULL);
  NW_ASSERT(str != NULL);

  NW_String_initialize(str, NULL, 0);

  if (NW_DOM_TextItem_getType(item) != NW_DOM_TEXT_ITEM_EXTENSION) {
    return 0;
  }

  /* TBD If it could be gauranteed that the extension was constructed
     correctly then it would be enough to just unconditionally
     shallowCopy. */

  fqToken = (NW_Uint16)(item->component.ext.token);
  t = (NW_Uint8)fqToken;

  if (((t == NW_WBXML_EXT_T_0)
       || (t == NW_WBXML_EXT_T_1)
       || (t == NW_WBXML_EXT_T_2))
      && (item->component.ext.type
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
    NW_String_shallowCopy(str, &item->component.ext.value.string);
  }
  return fqToken;
}

EXPORT_C NW_Status_t
NW_DOM_TextItem_getExtensionInt(NW_DOM_TextItem_t* item,
                                NW_Uint32* x)
{
  NW_Uint16 fqToken;
  NW_Uint8 t; /* 8-bit token */

  NW_ASSERT(item != NULL);
  NW_ASSERT(x != NULL);

  if ((NW_DOM_TextItem_getType(item) != NW_DOM_TEXT_ITEM_EXTENSION)
      || ((item->component.ext.type
           != NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER)))
  {
    return NW_STAT_FAILURE;
  }

  fqToken = (NW_Uint16)(item->component.ext.token);
  t = (NW_Uint8)fqToken;
  if ((t == NW_WBXML_EXT_T_0)
      || (t == NW_WBXML_EXT_T_1)
      || (t == NW_WBXML_EXT_T_2)) {
    *x = item->component.ext.value.x;
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_FAILURE;
}

EXPORT_C NW_Byte *
NW_DOM_TextItem_getOpaque(NW_DOM_TextItem_t *item,
                          NW_Uint32 *opaqueLen)
{
  NW_ASSERT(item != NULL);

  if (NW_DOM_TextItem_getType(item) != NW_DOM_TEXT_ITEM_OPAQUE){
    return NULL;
  }

  *opaqueLen = item->component.opaque.length;
  return item->component.opaque.data;
}


/*
 * TEXT NODE HANDLE
 */

NW_DOM_TextItemIterator_t *
NW_DOM_TextItemIterator_new(void)
{
  return (NW_DOM_TextItemIterator_t *)
    NW_Mem_Malloc(sizeof (NW_DOM_TextItemIterator_t));
}


NW_Status_t
NW_DOM_TextItemIterator_delete(NW_DOM_TextItemIterator_t *handle)
{
  NW_ASSERT(handle != NULL);

  NW_Mem_Free (handle);

  return NW_STAT_SUCCESS;
}

/*
 * Returns NW_STAT_WBXML_ITERATE_MORE
 *         NW_STAT_WBXML_ITERATE_DONE
 *
 * MODIFIED textItem
 */

EXPORT_C NW_Status_t
NW_DOM_TextItemIterator_getNextTextItem(NW_DOM_TextItemIterator_t *handle,
                                        NW_DOM_TextItem_t *textItem)
{
  NW_TinyTree_Offset_t offset;

  NW_ASSERT(handle != NULL);
  NW_ASSERT(textItem != NULL);

  offset = NW_TinyDom_TextHandle_iterate(handle, textItem);
  if (offset == 0){
    return NW_STAT_WBXML_ITERATE_DONE;
  }
  return NW_STAT_WBXML_ITERATE_MORE;
}
