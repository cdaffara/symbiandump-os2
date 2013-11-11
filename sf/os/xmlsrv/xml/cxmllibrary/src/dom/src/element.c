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
#include <xml/cxml/nw_dom_element.h>
#include <xml/cxml/nw_dom_attribute.h>
#include <xml/cxml/nw_dom_document.h>
#include <xml/cxml/nw_wbxml_dictionary.h>

/* 
 * Returns
 *  NW_STAT_DOM_NODE_TYPE_ERR - if it is not an Element Node
 *  NW_STAT_OUT_OF_MEMORY
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_SUCCESS  
 *
 * MODIFIED name (of the element)
 */

EXPORT_C NW_Status_t
NW_DOM_ElementNode_getTagName(NW_DOM_ElementNode_t *elem, 
                              NW_String_t *name)
{
  NW_DOM_DocumentNode_t *docNode;
  NW_Uint32 encoding;
  NW_Status_t status;
  NW_Uint32 token;
  NW_TinyTree_t *tiny_tree;

  NW_ASSERT(elem != NULL);
  NW_ASSERT(name != NULL);
  
  if (NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE){
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  /*
   * Following method will return name if the node is XML type
   * or it is a literal tag
   */

  tiny_tree = NW_TinyTree_Node_findTree(elem);

  status = NW_TinyDom_getTagName(NW_TinyDom_getParser(tiny_tree), elem, name);

  if (status == NW_STAT_SUCCESS){
    return NW_STAT_SUCCESS;
  }
  /* No string returned means that getTagName succeeded but
   * didn't find a literal name . . .
   */
  
  if (status != NW_STAT_WBXML_NO_NAME){
    return NW_STAT_DOM_NO_STRING_RETURNED;
  }
  
  /* . . . so we try to get a name from dictionary */

  docNode = NW_DOM_Node_getOwnerDocument(elem);
  encoding = NW_DOM_DocumentNode_getCharacterEncoding(docNode);  
  token = NW_TinyDom_getTagToken(NW_TinyDom_getParser(tiny_tree), elem);
  return NW_String_tokenToString(token, name, encoding);
}

/*
 * Returns the fully qualified token for the node
 */

EXPORT_C NW_Uint16 
NW_DOM_ElementNode_getTagToken(NW_DOM_ElementNode_t *elem)
{
  NW_Uint16 type;
  NW_TinyTree_t *tiny_tree;

  NW_ASSERT(elem != NULL);
  
  type = NW_TinyDom_Node_getType(elem);

  if (type != NW_DOM_ELEMENT_NODE){
    return 0;
  }
  tiny_tree = NW_TinyTree_Node_findTree(elem);  

  if (elem->token == 0)
  {
      elem->token = 
        (NW_Uint16)NW_TinyDom_getTagToken(NW_TinyDom_getParser(tiny_tree), elem);

    /*  if (NW_DOM_Node_hasChildNodes(elem)){
        token |= NW_WBXML_FLAGS_CONTENT;
      }
      if (NW_DOM_ElementNode_hasAttributes(elem)){
        token |= NW_WBXML_FLAGS_ATTRIBUTES;
      }
      elem->token = token;*/
  }
  return (NW_Uint16)(elem->token & 0xff3f);
}


/* 
 * Returns
 *  NW_STAT_DOM_NODE_TYPE_ERR
 *  NW_STAT_NOT_FOUND - if attribute is not found
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_SUCCESS
 *
 * This method returns attribute value as string
 * IN - attributeName - name of attribute
 * MODIFIED - attributeValue 

Note: The passed in argument "attributeName" must be given as aligned,
native byte order, null terminated, UCS-2.  The reason is that clients
wish to process documents in various encodings but they don't want to
have to convert element and attribute names which are program constants
into whatever charset the document appears in.
 */

EXPORT_C NW_Status_t
NW_DOM_ElementNode_getAttribute(
  NW_DOM_ElementNode_t *elem, 
  NW_String_t *attributeName, 
  NW_String_t *attributeValue)
{
  NW_DOM_AttributeHandle_t attributeHandle;
  NW_Status_t status;

  NW_ASSERT(elem != NULL);
  NW_ASSERT(attributeName != NULL);
  NW_ASSERT(attributeValue != NULL);

  if ((NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
      && (NW_DOM_Node_getNodeType(elem) != NW_DOM_PROCESSING_INSTRUCTION_NODE)) {
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  status = NW_DOM_ElementNode_getAttributeHandleByName(elem, 
                                                       attributeName, 
                                                       &attributeHandle);
  if (status != NW_STAT_SUCCESS){ 
    return status;
  }

  return NW_DOM_AttributeHandle_getValue(&attributeHandle, attributeValue);
}


/*
 * Returns NW_TRUE if element has attributes, otherwise NW_FALSE
 */

EXPORT_C NW_Bool
NW_DOM_ElementNode_hasAttributes(NW_DOM_ElementNode_t *elem)
{
  NW_DOM_Node_t *child;

  NW_ASSERT(elem != NULL);

  if ((NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
      && (NW_DOM_Node_getNodeType(elem) != NW_DOM_PROCESSING_INSTRUCTION_NODE))
  {
    return NW_FALSE;
  }

  child = NW_TinyTree_findFirstChild(elem);

  while(child){
    if (NW_TinyDom_Node_getType(child) == T_DOM_NODE_ATTR){
      return NW_TRUE;
    }
    child = NW_TinyTree_findNextSibling(child);
  }
  return NW_FALSE;
}

/*
 * Returns
 *         NW_STAT_DOM_NODE_TYPE_ERR
 *         NW_STAT_SUCCESS
 * MODIFIED iterator
 * WAP-adapted DOM extension
 */

EXPORT_C NW_Status_t 
NW_DOM_ElementNode_getAttributeListIterator(NW_DOM_Node_t *node, 
                                            NW_DOM_AttributeListIterator_t 
                                            *iterator)
{
  NW_DOM_Node_t *child;
  NW_TinyTree_t *tiny_tree;

  NW_ASSERT(node != NULL);
  NW_ASSERT(iterator != NULL);

  if ((NW_DOM_Node_getNodeType(node) != NW_DOM_ELEMENT_NODE)
      && (NW_DOM_Node_getNodeType(node) != NW_DOM_PROCESSING_INSTRUCTION_NODE))
  {
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  tiny_tree = NW_TinyTree_Node_findTree(node);

  (void) NW_DOM_AttributeListIterator_initialize(iterator);

  child = NW_TinyTree_findFirstChild(node);
  iterator->node = child;
  if(child){
    if (NW_TinyDom_Node_getType(child) == T_DOM_NODE_ATTR){
      NW_TinyDom_AttrListHandle_init(&iterator->attrListHandle, 
                                     NW_TinyDom_getParser(tiny_tree), 
                                     child);
      return NW_STAT_SUCCESS;
    }
    child = NW_TinyTree_findNextSibling(child);
    while (child){
      if (NW_TinyDom_Node_getType(child) == T_DOM_NODE_ATTR){
        NW_TinyDom_AttrListHandle_init(&iterator->attrListHandle, 
                                     NW_TinyDom_getParser(tiny_tree), 
                                     child);
        return NW_STAT_SUCCESS;
      }
      child = NW_TinyTree_findNextSibling(child);
    }
  }
  return NW_STAT_NOT_FOUND;
}

/* 
 * Returns
 *  NW_STAT_DOM_NODE_TYPE_ERR
 *  NW_STAT_NOT_FOUND - if attribute is not found
 *  NW_STAT_SUCCESS
 *
 * IN - attributeName
 * MODIFIED - handle
 */

EXPORT_C NW_Status_t
NW_DOM_ElementNode_getAttributeHandleByName(NW_DOM_ElementNode_t *elem,
                                            NW_String_t *attributeName,
                                            NW_DOM_AttributeHandle_t
                                            *attributeHandle)
{
  NW_Status_t nwStatus;
  NW_Status_t status;
  NW_DOM_AttributeListIterator_t attrListIterator;
  NW_Bool attributeFound = NW_FALSE;
  NW_String_t currentName;
  NW_Uint32 docEncoding;
  NW_Int32 charCount;
  NW_Uint32 byteCount = 0;
  NW_Int32 cmpResult;

  NW_ASSERT(elem != NULL);
  NW_ASSERT(attributeHandle != NULL);
  NW_ASSERT(attributeName != NULL);

  if ((NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
      && (NW_DOM_Node_getNodeType(elem) != NW_DOM_PROCESSING_INSTRUCTION_NODE))
  {
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  if (!NW_DOM_ElementNode_hasAttributes(elem)){
    return NW_STAT_NOT_FOUND;
  }

  /* Initialize the handle */
  status =
  NW_DOM_ElementNode_getAttributeListIterator(elem, &attrListIterator);

  if (status != NW_STAT_SUCCESS){
    return status;
  }

  while (!attributeFound){
    if (NW_DOM_AttributeListIterator_getNextAttribute(&attrListIterator,
                                                      attributeHandle)
        == NW_STAT_WBXML_ITERATE_DONE){
      return NW_STAT_NOT_FOUND;
    }

    status = NW_DOM_AttributeHandle_getName(attributeHandle, &currentName);
    if (status != NW_STAT_SUCCESS){
      continue;
    }

    docEncoding = NW_DOM_AttributeHandle_getEncoding (attributeHandle);
    charCount = NW_String_charBuffGetLength(currentName.storage, docEncoding,
                                            &byteCount);
    if (charCount < 0) {
      return NW_STAT_FAILURE;
    }

    nwStatus = NW_String_CmpToNativeAlignedUCS2(docEncoding, (NW_Uint32) charCount,
                                                currentName.storage,
                                                (NW_Uint16*)(attributeName->storage),
                                                &cmpResult);
    NW_String_deleteStorage(&currentName);
    if (NW_STAT_IS_FAILURE(nwStatus)){
      continue;
    }
    if (cmpResult == 0){
      attributeFound = NW_TRUE;
    }
  }

  /* Proceed: Attribute Found */
  
  /* If attributeHandle->node != NULL, that means that the attribute has an
  independent node to itself, and is not a part of list node */

  if (attributeHandle->node != NULL){
    NW_Bool nextAttrWithSameNameFound = NW_FALSE;
    NW_DOM_AttributeHandle_t h;
    h.node = NULL;

    while (!nextAttrWithSameNameFound){
      if (NW_DOM_AttributeListIterator_getNextAttribute(&attrListIterator, &h)
          == NW_STAT_WBXML_ITERATE_DONE){
        break;
      }
      status = NW_DOM_AttributeHandle_getName(&h, &currentName);
      if (status != NW_STAT_SUCCESS){
        break;
      }
      docEncoding = NW_DOM_AttributeHandle_getEncoding(attributeHandle);
      charCount = NW_String_charBuffGetLength(currentName.storage, docEncoding,
                                              &byteCount);
      if (charCount < 0) {
        return NW_STAT_FAILURE;
      }
      nwStatus = NW_String_CmpToNativeAlignedUCS2(docEncoding, (NW_Uint32)charCount,
                                                  currentName.storage,
                                                  (NW_Uint16*)(attributeName->storage),
                                                  &cmpResult);
      NW_String_deleteStorage(&currentName);
      if (NW_STAT_IS_FAILURE(nwStatus)){
        continue;
      }
      if (cmpResult == 0){
        nextAttrWithSameNameFound = NW_TRUE;
      }
    }

    if (nextAttrWithSameNameFound){
      NW_TinyDom_Parser_t *parser =
      attrListIterator.attrListHandle.tiny_parser;
      NW_DOM_AttributeHandle_initWithNode(attributeHandle, parser, h.node);
    }
  }

  return NW_STAT_SUCCESS;
}

/**
 * The following 2 methods use token to return attributes
 */

/* 
 * Returns
 *  NW_STAT_DOM_NODE_TYPE_ERR
 *  NW_STAT_NOT_FOUND - if attribute is not found
 *  NW_STAT_SUCCESS
 *
 * IN - token 
 * MODIFIED - handle
 */

EXPORT_C NW_Status_t
NW_DOM_ElementNode_getAttributeHandleByToken(NW_DOM_ElementNode_t *elem, 
                                             NW_Uint16 token, 
                                             NW_DOM_AttributeHandle_t *handle)
{
  NW_DOM_AttributeListIterator_t attributeListIterator;
  NW_Bool attributeFound = NW_FALSE;
  NW_Uint16 currentToken = 0;
  NW_Status_t status;
  NW_TinyDom_Parser_t *parser;

  NW_ASSERT(token != 0);
  NW_ASSERT(elem != NULL);
  NW_ASSERT(handle != NULL);

  if ((NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
    && (NW_DOM_Node_getNodeType(elem) != NW_DOM_PROCESSING_INSTRUCTION_NODE)){
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  if (!NW_DOM_ElementNode_hasAttributes(elem)){
    return NW_STAT_NOT_FOUND;
  }

  status = 
    NW_DOM_ElementNode_getAttributeListIterator(elem, &attributeListIterator);

  if (status != NW_STAT_SUCCESS){
    return status;
  }

  while (!attributeFound){
    if (NW_DOM_AttributeListIterator_getNextAttribute(&attributeListIterator, 
                                                      handle) 
        == NW_STAT_WBXML_ITERATE_DONE){
      return NW_STAT_NOT_FOUND;
    }
    currentToken = NW_DOM_AttributeHandle_getToken(handle);
    if (currentToken == token){
      attributeFound = NW_TRUE;
    }
  }
  /* if we arrive here, attributeFound must be NW_TRUE */
  if (handle->node != NULL)
  {
    NW_Bool nextAttrWithSameNameFound = NW_FALSE;
    NW_DOM_AttributeHandle_t h;
    h.node = NULL;

    while (!nextAttrWithSameNameFound){
      if (NW_DOM_AttributeListIterator_getNextAttribute(&attributeListIterator,
                                                        &h) 
          == NW_STAT_WBXML_ITERATE_DONE){
        return NW_STAT_BAD_INPUT_PARAM;
      }

      currentToken = NW_DOM_AttributeHandle_getToken(&h);
      if (currentToken == token)
        nextAttrWithSameNameFound = NW_TRUE;
    }

    parser = attributeListIterator.attrListHandle.tiny_parser;
      NW_DOM_AttributeHandle_initWithNode(handle, parser, h.node);
    }

  return NW_STAT_SUCCESS;
}

/* 
 * Returns
 *  NW_STAT_NOT_FOUND - if attribute is not found
 *  NW_STAT_DOM_NODE_TYPE_ERR
 *  NW_STAT_DOM_NO_STRING_RETURNED
 *  NW_STAT_SUCCESS
 *
 * This method returns attribute value as string
 * IN - token of attribute
 * MODIFIED - attributeValue 
 */

NW_Status_t
NW_DOM_ElementNode_getAttributeValueStringByToken(NW_DOM_ElementNode_t *elem, 
                                                  NW_Uint16 token, 
                                                  NW_String_t *attributeValue)
{
  NW_DOM_AttributeHandle_t attributeHandle;
  NW_Status_t status;

  NW_ASSERT(elem != NULL);
  NW_ASSERT(attributeValue != NULL);

  if ((NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
      && (NW_DOM_Node_getNodeType(elem) != NW_DOM_PROCESSING_INSTRUCTION_NODE)) 
   {
    return NW_STAT_DOM_NODE_TYPE_ERR;
   }

  status = 
    NW_DOM_ElementNode_getAttributeHandleByToken(elem, 
                                                 token, 
                                                 &attributeHandle);
  if (status != NW_STAT_SUCCESS){
    return status;
  }

  return NW_DOM_AttributeHandle_getValue(&attributeHandle, attributeValue);
}

/* 
 * IN - name - of element
 */

NW_Status_t
NW_DOM_ElementNode_getElementIteratorByTagName(NW_DOM_ElementNode_t *elem, 
                                               NW_String_t* name,
                                               NW_DOM_NodeIterator_t *handle)
{
  NW_DOM_DocumentNode_t *doc; 
  NW_WBXML_Dictionary_t *dict = NULL;
  NW_String_UCS2Buff_t *buff = NULL;

  NW_Uint32 encoding; 
  NW_Int16 token = 0;
  
  NW_ASSERT(elem != NULL);
  NW_ASSERT(name != NULL);
  NW_ASSERT(handle != NULL);
  
  doc  = NW_DOM_Node_getOwnerDocument(elem);
  if (doc == NULL) {
    return NW_STAT_FAILURE;
  }
  encoding = NW_DOM_DocumentNode_getCharacterEncoding(doc);
  
  dict = NW_DOM_DocumentNode_getDictionary(doc);
  buff = NW_String_stringToUCS2Buff(name, encoding);
  if((dict == NULL) || (buff == NULL)){
    NW_Mem_Free(buff);
    return NW_STAT_FAILURE;
  }
  token = NW_WBXML_Dictionary_getTagToken(dict, buff, NW_TRUE);
  NW_Mem_Free(buff);
  if (token < 0) {
    return NW_STAT_FAILURE;
  }
    return NW_DOM_ElementNode_getElementIteratorByTagToken(elem, 
                                                         (NW_Uint16)token, 
                                                         handle);
}

/* 
 * IN - token - of element
 */

NW_Status_t
NW_DOM_ElementNode_getElementIteratorByTagToken(NW_DOM_ElementNode_t *elem, 
                                                NW_Uint16 token,
                                                NW_DOM_NodeIterator_t *handle)
{

  NW_ASSERT(elem != NULL);
  NW_ASSERT(handle != NULL);

  if (NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
    return NW_STAT_DOM_NODE_TYPE_ERR;

  return NW_DOM_NodeIterator_initialize(handle, elem, token); 
}

                          

/*
 * If the attribute with the given name exists then a its value will 
 * be changed to input value, otherwise a new attribute will be created
 * LIMITATION: If the given attribute was already present when the document
 * was parsed the first time, it can not be removed. In this case a new 
 * attribute is added, and you will have 2 attributes with same name
 * Returns
 *  NW_STAT_BAD_INPUT_PARAM - If input args are NULL
 *  NW_STAT_DOM_NODE_TYPE_ERR - If it is not an element node
 *  NW_STAT_SUCCESS
 *
 */

EXPORT_C NW_Status_t
NW_DOM_ElementNode_setAttribute(NW_DOM_ElementNode_t *elem, 
                                NW_String_t *attrName, 
                                NW_String_t *val)
{
  NW_Status_t status;
  NW_TinyTree_t* tiny_tree;
  NW_TinyDom_Tree_t *tree;
  NW_TinyTree_Node_t *attr = NULL;
  NW_DOM_DocumentNode_t *doc = NW_DOM_Node_getOwnerDocument(elem);
  NW_WBXML_Dictionary_t *dictionary = NW_DOM_DocumentNode_getDictionary(doc);
  NW_Uint32 encoding = NW_DOM_DocumentNode_getCharacterEncoding(doc);
  NW_Int32 fqToken;
  NW_DOM_AttrVal_t attrValue;

  if ((elem == NULL) || (attrName == NULL) || 
    (val == NULL) || (dictionary == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  if (NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE) 
    return NW_STAT_DOM_NODE_TYPE_ERR;

  status = NW_DOM_ElementNode_removeAttribute(elem, attrName);
  if ((status != NW_STAT_SUCCESS) && (status != NW_STAT_NOT_FOUND)) {
    return status;
  }

  tiny_tree = NW_TinyTree_Node_findTree(elem);
  tree = NW_TinyDom_getTree(tiny_tree);

  fqToken = NW_WBXML_Dictionary_getAttributeToken(dictionary, attrName, encoding, NW_TRUE);

  status = NW_DOM_AttrVal_initFromString(&attrValue, val);
  if (fqToken == -1)
    attr = NW_TinyDom_createAttributeByName(tree, attrName, &attrValue);
  else
    attr = NW_TinyDom_createAttributeByToken(tree, (NW_Uint8)fqToken, &attrValue);
  
  return NW_DOM_Node_appendChild(elem, attr);
}

/**
 * Removes given attribute
 * Limited Use: You can only remove attributes that you 
 * explicitly created and added to the element. None of the
 * attributes in the originally parsed document can not be removed
 * Returns
 *  NW_STAT_BAD_INPUT_PARAM - If input args are NULL
 *  NW_STAT_DOM_NODE_TYPE_ERR - If it is not an element node
 *  NW_STAT_NOT_FOUND - if attribute is not found
 *  NW_STAT_SUCCESS
 **/
EXPORT_C NW_Status_t
NW_DOM_ElementNode_removeAttribute(
  NW_DOM_ElementNode_t *elem, NW_String_t* attributeName)
{
  NW_DOM_AttributeHandle_t attrHandle;
  NW_Status_t status;

  if ((elem == NULL) || (attributeName == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  if (NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
    return NW_STAT_DOM_NODE_TYPE_ERR;

  status = NW_DOM_ElementNode_getAttributeHandleByName(elem, attributeName, &attrHandle);
  if (status != NW_STAT_SUCCESS)
    return status;

  /* 
   * If the attribute was a newly added one it is deleted,
   * but SUCCESS is returned in both cases
   */
  if (attrHandle.node != NULL)
    NW_TinyTree_deleteNode(attrHandle.node);

  return NW_STAT_SUCCESS;
}



/*
 * If the attribute with the given name exists then its value will 
 * be changed to input value, otherwise a new attribute will be created
 * LIMITATION: If the given attribute was already present when the document
 * was parsed the first time, it can not be removed. In this case a new 
 * attribute is added, and you will have 2 attributes with same name
 * Returns
 *  NW_STAT_BAD_INPUT_PARAM - If input args are NULL
 *  NW_STAT_NOT_FOUND - if attribute is not found
 *  NW_STAT_DOM_NODE_TYPE_ERR - If it is not an element node
 *  NW_STAT_SUCCESS
 *
 */
EXPORT_C NW_Status_t
NW_DOM_ElementNode_setAttributeByToken(
  NW_DOM_ElementNode_t *elem, NW_Uint16 token, NW_DOM_AttrVal_t *attrValue)
{
  NW_Status_t status;
  NW_TinyTree_t* tinyTree;
  NW_TinyDom_Tree_t *tree;
  NW_TinyTree_Node_t *attr = NULL;
  NW_DOM_DocumentNode_t *doc = NW_DOM_Node_getOwnerDocument(elem);
  NW_WBXML_Dictionary_t *dictionary = NW_DOM_DocumentNode_getDictionary(doc);

  if ((elem == NULL) || (attrValue == NULL) || (dictionary == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  if (NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE) {
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  tinyTree = NW_TinyTree_Node_findTree(elem);
  tree = NW_TinyDom_getTree(tinyTree);

  status = NW_DOM_ElementNode_removeAttributeByToken(elem, token);
  if ((status == NW_STAT_SUCCESS) || (status == NW_STAT_NOT_FOUND)) {
    attr = NW_TinyDom_createAttributeByToken(tree, (NW_Uint8)token, attrValue);
    return NW_DOM_Node_appendChild(elem, attr);
  }
  return status;
}

/**
 * If the attribute with the given name exists then a its value will 
 * be changed to input value, otherwise a new attribute will be created
 * LIMITATION: If the given attribute was already present when the document
 * was parsed the first time, it can not be removed. In this case a new 
 * attribute is added, and you will have 2 attributes with same name
 * Returns
 *  NW_STAT_BAD_INPUT_PARAM - If input args are NULL
 *  NW_STAT_DOM_NODE_TYPE_ERR - If it is not an element node
 *  NW_STAT_SUCCESS
 *
 */
EXPORT_C NW_Status_t
NW_DOM_ElementNode_setAttributeByAttrVal(
  NW_DOM_ElementNode_t *elem, 
  NW_String_t *attrName, 
  NW_DOM_AttrVal_t *attrValue)
{
  NW_Status_t status;
  NW_TinyTree_t* tinyTree;
  NW_TinyDom_Tree_t *tree;
  NW_TinyTree_Node_t *attr = NULL;
  NW_DOM_DocumentNode_t *doc = NW_DOM_Node_getOwnerDocument(elem);
  NW_WBXML_Dictionary_t *dictionary = NW_DOM_DocumentNode_getDictionary(doc);
  NW_Uint32 encoding = NW_DOM_DocumentNode_getCharacterEncoding(doc);
  NW_Int32 fqToken;

  if ((elem == NULL) || (attrName == NULL) || 
    (attrValue == NULL) || (dictionary == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  if (NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
    return NW_STAT_DOM_NODE_TYPE_ERR;

  status = NW_DOM_ElementNode_removeAttribute(elem, attrName);
  if ((status != NW_STAT_SUCCESS) && (status != NW_STAT_NOT_FOUND)) {
    return status;
  }
  tinyTree = NW_TinyTree_Node_findTree(elem);
  tree = NW_TinyDom_getTree(tinyTree);

  fqToken = NW_WBXML_Dictionary_getAttributeToken(dictionary, attrName, encoding, NW_TRUE);
  if (fqToken == -1){
    attr = NW_TinyDom_createAttributeByName(tree, attrName, attrValue);
  } else {
    attr = NW_TinyDom_createAttributeByToken(tree, (NW_Uint8)fqToken, attrValue);
  }
  return NW_DOM_Node_appendChild(elem, attr);
}

EXPORT_C NW_Status_t
NW_DOM_ElementNode_setAttributeByAttributeHandle(NW_DOM_ElementNode_t* elem,
                                                 NW_DOM_AttributeHandle_t* handle)
{
  if ((elem == NULL)
      || (handle == NULL)) {
    return NW_STAT_BAD_INPUT_PARAM;
  }
  if (NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE) {
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }
  if (handle->node == NULL) {
    return NW_STAT_FAILURE;
  }
  return NW_DOM_Node_appendChild(elem, handle->node);
}

/**
 * Removes given attribute
 * Limited Use: You can only remove attributes that you 
 * explicitly created and added to the element. None of the
 * attributes in the originally parsed document can not be removed
 * Returns
 *  NW_STAT_BAD_INPUT_PARAM - If input args are NULL
 *  NW_STAT_NOT_FOUND - if attribute is not found
 *  NW_STAT_DOM_NODE_TYPE_ERR - If it is not an element node
 *  NW_STAT_SUCCESS
 */
NW_Status_t
NW_DOM_ElementNode_removeAttributeByToken(
  NW_DOM_ElementNode_t *elem, NW_Uint16 attributeToken)
{
  NW_DOM_AttributeHandle_t attrHandle;
  NW_Status_t status;

  if ((elem == NULL) || (attributeToken == 0))
    return NW_STAT_BAD_INPUT_PARAM;

  if (NW_DOM_Node_getNodeType(elem) != NW_DOM_ELEMENT_NODE)
    return NW_STAT_DOM_NODE_TYPE_ERR;

  status = NW_DOM_ElementNode_getAttributeHandleByToken(elem, attributeToken, &attrHandle);
  if ((status != NW_STAT_SUCCESS)) 
    return status;

  /*
   * If the attribute was a newly added one it is deleted,
   * but SUCCESS is returned in both cases
   */
  if (attrHandle.node != NULL)
    NW_TinyTree_deleteNode(attrHandle.node);

  return NW_STAT_SUCCESS;
}

