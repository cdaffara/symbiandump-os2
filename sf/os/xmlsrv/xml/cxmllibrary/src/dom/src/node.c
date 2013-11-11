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
#include <xml/cxml/nw_dom_node.h>
#include <xml/cxml/nw_dom_document.h>
#include <xml/cxml/nw_dom_element.h>

/**
 * GENERAL NODE METHODS - applicable to all node types
 * These methods will be repeated for other Node types also
 **/

/*
 * RETURN 
 * NW_STAT_SUCCESS
 * NW_STAT_OUT_OF_MEMORY - If memory could not be allocated for storage
 * NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED - If encoding is not valid
 * NW_STAT_DOM_NODE_TYPE_ERR - If node type is not a standard node type
 * NW_STAT_DOM_NO_STRING_RETURNED - If the storage was not assigned to NW_DOM_String
 *
 * MODIFIED nodeName
 */

EXPORT_C NW_Status_t 
NW_DOM_Node_getNodeName(NW_DOM_Node_t *node, NW_String_t *nodeName)
{
  NW_Uint16 type;
  NW_String_UCS2Buff_t *name = NULL;
  NW_DOM_DocumentNode_t *docNode;
  NW_Uint32 encoding;
  
  NW_ASSERT(node != NULL);
  NW_ASSERT(nodeName != NULL);  
   
  type = NW_DOM_Node_getNodeType(node);
  if (type == 0){
    return NW_STAT_DOM_NODE_TYPE_ERR;
  }

  switch (type) {
    case NW_DOM_ELEMENT_NODE:
      return NW_DOM_ElementNode_getTagName(node, nodeName);

    case NW_DOM_PROCESSING_INSTRUCTION_NODE:/* Not supported */
       return NW_STAT_DOM_NO_STRING_RETURNED;

    case NW_DOM_TEXT_NODE:
      name = (NW_String_UCS2Buff_t *) L"#text";
      break;

    case NW_DOM_DOCUMENT_NODE:
      name = (NW_String_UCS2Buff_t *) L"#document";
      break;

    case NW_DOM_COMMENT_NODE:
      name = (NW_String_UCS2Buff_t *) L"#comment";
      break;

    case NW_DOM_CDATA_SECTION_NODE:
      name = (NW_String_UCS2Buff_t *) L"#cdata-section";
      break;
    default: 
      return NW_STAT_DOM_NO_STRING_RETURNED;
  }

  docNode = NW_DOM_Node_getOwnerDocument(node);
  if (docNode == NULL) {
    return NW_STAT_DOM_NO_STRING_RETURNED;
  }
  encoding = NW_DOM_DocumentNode_getCharacterEncoding(docNode);
    return NW_String_UCS2BuffToString(name, nodeName, encoding);    
  }

/* 
 * returns NW_DOM_NodeType 
 * Returns 0 if Node type is not one of the recognized nodes
 */

EXPORT_C NW_Uint16 
NW_DOM_Node_getNodeType(NW_DOM_Node_t *node)
{
  NW_Uint16 type;

  NW_ASSERT(node != NULL);

  type = (NW_Uint16)(NW_TinyDom_Node_getType(node) & 0x0f);

  switch(type) {

  case T_DOM_NODE_DOC:
    return NW_DOM_DOCUMENT_NODE;

  case T_DOM_NODE_TAG:
    return NW_DOM_ELEMENT_NODE;

  case T_DOM_NODE_TEXT:
    return NW_DOM_TEXT_NODE;

  case T_DOM_NODE_PI:
    return NW_DOM_PROCESSING_INSTRUCTION_NODE;

  case T_DOM_NODE_COMMENT:
    return NW_DOM_COMMENT_NODE;

  case T_DOM_NODE_CDATASECTION:
    return NW_DOM_CDATA_SECTION_NODE;

  default:
  return 0;
}
}

/*  
 * Returns fully qualified node token (including dictionary id)
 * Returns 0 for invalid node
 */
EXPORT_C NW_Uint16 
NW_DOM_Node_getNodeToken(NW_DOM_Node_t *node)
{

  NW_ASSERT(node != NULL);

  if (NW_DOM_Node_getNodeType(node) == NW_DOM_ELEMENT_NODE){
    return NW_DOM_ElementNode_getTagToken(node);
  }
  return 0;
}

/* 
 * Returns NW_TRUE if the document is of WBXML type 
 * otherwise returns NW_FALSE
 */

NW_Bool 
NW_DOM_Node_isWBXML(NW_DOM_Node_t *node)
{
  NW_Uint16 type;

  NW_ASSERT(node != NULL);

  type = (NW_Uint16)(NW_TinyDom_Node_getType(node) & 0x8000);
  
  if (type == T_DOM_NODE_XML){
    return NW_FALSE;
  }
  return NW_TRUE;
}

/* 
 * Finds parent node of the given node
 * returns NULL for DOCUMENT_NODE 
 */

EXPORT_C NW_DOM_Node_t * 
NW_DOM_Node_getParentNode(NW_DOM_Node_t *node)
{
  NW_ASSERT (node != NULL); 

  if (NW_DOM_Node_getNodeType(node) == NW_DOM_DOCUMENT_NODE)
    return NULL;

  return NW_TinyTree_findParent(node);
}

/* 
 * Following 3 methods return valid values only
 * for ELEMENT_NODE and DOCUMENT_NODE, since these are the 
 * only nodes that have children. For other nodes, the value is
 * either NULL or 0
 */

EXPORT_C NW_DOM_Node_t * 
NW_DOM_Node_getFirstChild(NW_DOM_Node_t *node)
{
  NW_DOM_Node_t *child;

  NW_ASSERT(node != NULL);

  child = NW_TinyTree_findFirstChild(node);

  if(child)
  {
      if(NW_TinyDom_Node_getType(child) == T_DOM_NODE_ATTR)
        return NW_DOM_Node_getNextSibling(child);
      else
        return child;
  }
  return NULL;
}

EXPORT_C NW_DOM_Node_t * 
NW_DOM_Node_getLastChild(NW_DOM_Node_t *node)
{
  NW_DOM_Node_t *currentNode = NULL;
  NW_DOM_Node_t *previousNode = NULL;

  NW_ASSERT(node != NULL);

  currentNode = NW_DOM_Node_getFirstChild(node);
  while (currentNode != NULL) 
    {
    previousNode = currentNode;
    currentNode = NW_DOM_Node_getNextSibling(currentNode);
  }
  return previousNode;
}

EXPORT_C NW_Bool 
NW_DOM_Node_hasChildNodes(NW_DOM_Node_t *node)
{
  
  NW_ASSERT(node != NULL);
  
  if (NW_DOM_Node_getFirstChild(node))
    return NW_TRUE;
  return NW_FALSE;
}

EXPORT_C NW_DOM_Node_t * 
NW_DOM_Node_getNextSibling(NW_DOM_Node_t *node)
{
  NW_DOM_Node_t *sibling;
  
  NW_ASSERT(node != NULL);
 
  sibling = NW_TinyTree_findNextSibling(node);

  if (sibling == NULL){
    return NULL;
  }

  while ((sibling != NULL) && (NW_TinyDom_Node_getType(sibling) == T_DOM_NODE_ATTR)){
    sibling = NW_TinyTree_findNextSibling(sibling);
  }
  return sibling;
}

EXPORT_C NW_DOM_Node_t * 
NW_DOM_Node_getPreviousSibling(NW_DOM_Node_t *node)
{
  NW_DOM_Node_t *sibling;
  NW_ASSERT(node != NULL);

  sibling = NW_TinyTree_findPreviousSibling(node);

  if (sibling == NULL)
    return NULL;

  while ((sibling != NULL) && (NW_TinyDom_Node_getType(sibling) == T_DOM_NODE_ATTR)){
    sibling = NW_TinyTree_findPreviousSibling(sibling);
  }

  return sibling;
}

EXPORT_C NW_DOM_DocumentNode_t * 
NW_DOM_Node_getOwnerDocument(NW_DOM_Node_t *node)
{
  NW_TinyTree_t *tiny_tree;

  NW_ASSERT(node != NULL);

  if (NW_DOM_Node_getNodeType(node) == NW_DOM_DOCUMENT_NODE){
    return node;
  }

  tiny_tree = NW_TinyTree_Node_findTree(node);

  return NW_TinyTree_getRoot(tiny_tree); 
}

/*********************
 * NODE LIST ITERATOR
 **********************/

/*
 * Returns NULL if unable to allocate memory
 */

NW_DOM_NodeIterator_t *
NW_DOM_NodeIterator_new(void)
{
  return (NW_DOM_NodeIterator_t *) NW_Mem_Malloc(sizeof (NW_DOM_NodeIterator_t));
}

/*
 * Initializes the Node handle which uses the given node
 * as the starting point to iterate down the tree
 * Returns NW_STAT_SUCCESS
 *         NW_STAT_BAD_INPUT_PARAM
 */

NW_Status_t
NW_DOM_NodeIterator_initialize(NW_DOM_NodeIterator_t *handle, 
                               NW_DOM_Node_t *node, 
                               NW_Uint16 token)
{
  NW_ASSERT (handle != NULL);
  NW_ASSERT (node != NULL);

  NW_TinyTree_NodeIterator_init(node, &handle->nodeIter);
  handle->token = token;
  return NW_STAT_SUCCESS;
}

/*
 * Deletes the handle
 * Returns NW_STAT_SUCCESS
 */

NW_Status_t
NW_DOM_NodeIterator_delete(NW_DOM_NodeIterator_t *handle)
{
  
  NW_ASSERT(handle != NULL);

  NW_Mem_Free (handle);

  return NW_STAT_SUCCESS;
}

/*
 * Returns the next node
 */

NW_DOM_Node_t * 
NW_DOM_NodeIterator_nextNode(NW_DOM_NodeIterator_t *handle)
{
  NW_DOM_Node_t* node = NULL;
  NW_Uint16 token = 0;

  NW_ASSERT(handle != NULL);
  
  do {

    node = NW_TinyTree_NodeIterator_iterate(&handle->nodeIter);

    if (node == NULL){
      return NULL;
    }

    token = NW_DOM_Node_getNodeToken(node);

  } while (token != handle->token);
  
  return node;
}



/**
 * Inserts newChild before the refChild
 * Returns one of:
 *   NW_STAT_BAD_INPUT_PARAM
 *   NW_STAT_SUCCESS
 *   NW_STAT_NOT_FOUND - If reChild is not a child of given node
 *   NW_STAT_DOM_WRONG_DOC_ERR - If newChild was created from a 
 *        different document than the one that created the node
 *   NW_STAT_DOM_HEIRARCHY_REQ_ERR - If node is of the type that 
 *        does not allow children of the type of newChild node
 */
EXPORT_C NW_Status_t
NW_DOM_Node_insertBefore(
  NW_DOM_Node_t *node, 
  NW_DOM_Node_t *newChild, 
  NW_DOM_Node_t *refChild)
{
  NW_TinyTree_Node_t *child = NULL;
  NW_DOM_DocumentNode_t *doc = NULL;
  NW_DOM_DocumentNode_t *newDoc = NULL;
  
  if ((node == NULL) || (newChild == NULL) || (refChild == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  doc = NW_DOM_Node_getOwnerDocument(node);
  newDoc = NW_DOM_Node_getOwnerDocument(newChild);

  if ((doc == NULL) || (newDoc == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  if (doc != newDoc)
    return NW_STAT_DOM_WRONG_DOC_ERR;

  /* Only Element and Document nodes can have children */
  if ((NW_DOM_Node_getNodeType(node) != NW_DOM_ELEMENT_NODE) &&
      (NW_DOM_Node_getNodeType(node) != NW_DOM_DOCUMENT_NODE)) {
    return NW_STAT_DOM_HEIRARCHY_REQUEST_ERR;
  }

  child = NW_DOM_Node_getFirstChild (node);
  while (child)
  {
    if (child == refChild)
    {
      NW_TinyTree_attachBefore(child, newChild);
      return NW_STAT_SUCCESS;
    }
    child = NW_DOM_Node_getNextSibling(child);
  }
  /* The refChild is not a child of node */
  return NW_STAT_NOT_FOUND;
}

/**
 * Replaces oldChild with the newChild and removes old child
 * Returns one of:
 *   NW_STAT_BAD_INPUT_PARAM
 *   NW_STAT_SUCCESS
 *   NW_STAT_NOT_FOUND - If oldChild is not a child of given node
 *   NW_STAT_DOM_WRONG_DOC_ERR - If newChild was created from a 
 *        different document than the one that created the node
 *   NW_STAT_DOM_HEIRARCHY_REQ_ERR - If node is of the type that 
 *        does not allow children of the type of newChild node
 */

EXPORT_C NW_Status_t
NW_DOM_Node_replaceChild(NW_DOM_Node_t *node, NW_DOM_Node_t *newChild, NW_DOM_Node_t *oldChild)
{
  NW_TinyTree_Node_t *child = NULL;
  NW_DOM_DocumentNode_t *doc = NULL;
  NW_DOM_DocumentNode_t *newDoc = NULL;

  if ((node == NULL) || (oldChild == NULL) || (newChild == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  doc = NW_DOM_Node_getOwnerDocument(node);
  newDoc = NW_DOM_Node_getOwnerDocument(newChild);

  if ((doc == NULL) || (newDoc == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  if (doc != newDoc)
    return NW_STAT_DOM_WRONG_DOC_ERR;

  /* Only Element and Document nodes can have children */
  if ((NW_DOM_Node_getNodeType(node) != NW_DOM_ELEMENT_NODE) &&
      (NW_DOM_Node_getNodeType(node) != NW_DOM_DOCUMENT_NODE)) {
    return NW_STAT_DOM_HEIRARCHY_REQUEST_ERR;
  }

  child = NW_DOM_Node_getFirstChild (node);
  while (child)
  {
    if (child == oldChild)
    {
      NW_TinyTree_attachBefore(oldChild, newChild);
      return NW_DOM_Node_removeChild(node, oldChild);
    }
    child = NW_DOM_Node_getNextSibling(child);
  }
  /* The refChild is not a child of node */
  return NW_STAT_NOT_FOUND;  
}

/*
 * Removes the oldChild
 * Returns one of:
 *   NW_STAT_BAD_INPUT_PARAM
 *   NW_STAT_SUCCESS
 *   NW_STAT_NOT_FOUND - If oldChild is not a child of given node
 */
EXPORT_C NW_Status_t
NW_DOM_Node_removeChild(NW_DOM_Node_t *node, NW_DOM_Node_t *oldChild)
{
  NW_TinyTree_Node_t *child = NULL;

  if ((node == NULL) || (oldChild == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  child = NW_DOM_Node_getFirstChild (node);
  while (child)
  {
    if (child == oldChild)
    {
      NW_TinyTree_deleteNode(oldChild);
      return NW_STAT_SUCCESS;
    }
    child = NW_DOM_Node_getNextSibling(child);
  }

  /* The refChild is not a child of node */
  return NW_STAT_NOT_FOUND;  
}

/*
 * Appends a new child
 * Returns one of:
 *   NW_STAT_BAD_INPUT_PARAM
 *   NW_STAT_SUCCESS
 *   NW_STAT_DOM_WRONG_DOC_ERR - If newChild was created from a 
 *        different document than the one that created the node
 *   NW_STAT_DOM_HEIRARCHY_REQ_ERR - If node is of the type that 
 *        does not allow children of the type of newChild node
 */
EXPORT_C NW_Status_t
NW_DOM_Node_appendChild(NW_DOM_Node_t *node, NW_DOM_Node_t *newChild)
{
  NW_DOM_DocumentNode_t *doc = NULL;
  NW_DOM_DocumentNode_t *newDoc = NULL;

  if (( node == NULL) || (newChild == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  doc = NW_DOM_Node_getOwnerDocument(node);
  newDoc = NW_DOM_Node_getOwnerDocument(newChild);

 if ((doc == NULL) || (newDoc == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  if (doc != newDoc)
    return NW_STAT_DOM_WRONG_DOC_ERR;

  /* Only Element and Document nodes can have children */
  if ((NW_DOM_Node_getNodeType(node) != NW_DOM_ELEMENT_NODE) &&
      (NW_DOM_Node_getNodeType(node) != NW_DOM_DOCUMENT_NODE)) {
    return NW_STAT_DOM_HEIRARCHY_REQUEST_ERR;
  }

  return(NW_TinyTree_attachChild(node, newChild));
}


