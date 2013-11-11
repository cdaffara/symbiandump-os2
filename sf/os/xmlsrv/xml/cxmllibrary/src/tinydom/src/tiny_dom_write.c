/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
/* ************************************************************************/
#include "cxml_internal.h"
#include <xml/cxml/nw_wbxml_parse.h>
#include <xml/cxml/nw_wbxml_document.h>
#include <xml/cxml/nw_encoder_wbxmlwriter.h>
#include "nw_encoder_tinydom2wbxml.h"
#include <xml/cxml/nw_tinytree.h>
#include <xml/cxml/nw_tinydom.h>


/* Create an empty tree */

NW_Status_t
NW_TinyDom_Tree_create(NW_TinyDom_Tree_t *dom_tree,
                       NW_TinyDom_Parser_t *dom_parser,
                       NW_WBXML_Document_t *doc,
                       NW_WBXML_Parser_t *parser,
                       NW_WBXML_Writer_t* writer,
                       NW_Uint16 init_node_count,
                       NW_Bool enableStringTable)
{
  NW_WBXML_Dictionary_t* dictionary = NULL;

  /* First get the dictionary */

  if (doc->publicid != 0) {
    dictionary = NW_WBXML_Dictionary_getByPublicId (doc->publicid);
    parser->dictionary = NW_WBXML_Dictionary_getIndexByPublicId(doc->publicid);
  }
  else if (doc->doc_type != NULL) {
    dictionary = NW_WBXML_Dictionary_getByDocType (doc->doc_type, doc->charset);
    parser->dictionary = NW_WBXML_Dictionary_getIndexByDocType(doc->doc_type, doc->charset);
  }

  if (dictionary == NULL) {
    return NW_STAT_FAILURE;
  }

  /* TODO: make dictionary a member of dom_parser ?? */

  /* Initialize the writer */

  NW_WBXML_Writer_Initialize(writer,
                             0, NULL,
                             NULL,
                             dictionary,
                             dictionary,
                             ((enableStringTable == NW_TRUE) ?
                              NW_Encoder_StringTable_getStringTableOffset :
                              NULL),
                             ((enableStringTable == NW_TRUE) ?
                              NW_Encoder_StringTable_addToStringTable :
                              NULL),
                             ((enableStringTable == NW_TRUE) ?
                              doc->strtbl_extension :
                              NULL),
                             ((enableStringTable == NW_TRUE) ?
                              NW_Encoder_StringTable_StringTableIterateInit :
                              NULL),
                             ((enableStringTable == NW_TRUE) ?
                              NW_Encoder_StringTable_StringTableIterateNext :
                              NULL),
                             NW_TRUE);

  /* Construct the dom tree object */
  NW_TinyDom_Tree_construct(dom_tree, parser, doc, writer);

  /* Construct the dom parser object */
  NW_TinyDom_Parser_construct(dom_parser, dom_tree);

  /* Construct the tiny tree object, passing the dom parser as the
     context argument */
  
  
  if( (NW_TinyTree_construct(&(dom_tree->tree),
                        (CXML_Vector_Metric_t)(init_node_count + 1),
                        0,
                        0,
                        dom_parser,
                        NW_TRUE) ) == NW_STAT_FAILURE)
  {
	  return NW_STAT_OUT_OF_MEMORY;
  }

  /* The root node we create here actually has no valid buffer storage yet */
  NW_TinyTree_setRoot(&(dom_tree->tree), 0);

  /* TODO: Why does dom_tree need a doc member if parser has one too ???? */
  parser->doc = doc;

  dom_tree->root_node = NW_TinyTree_getRoot(&(dom_tree->tree));

  /* Write the doc header block. This will fill in the root node storage */
  dom_tree->root_node = NW_TinyDom_writeDocHeader(dom_tree,
                                                  doc->version,
                                                  doc->publicid,
                                                  doc->charset);
  if (dom_tree->root_node == NULL) {
    return NW_STAT_FAILURE;
  }
  /* Mark the root node as the doc node */
  NW_TinyTree_Node_setUserFlags(dom_tree->root_node, T_DOM_NODE_DOC);
  return NW_STAT_SUCCESS;
}

NW_TinyTree_Node_t *
NW_TinyDom_writeDocHeader(NW_TinyDom_Tree_t *dom_tree,
                          NW_Uint8 version,
                          NW_Uint32 publicid,
                          NW_Uint32 encoding)
{
  NW_Status_t status;
  NW_TinyTree_Offset_t offset;
  NW_TinyTree_Node_t * node = dom_tree->root_node;
  CXML_Vector_Metric_t size;

  /* If the root node isn't set, the tree wasn't properly inited */
  if (node != NULL) {
    NW_Uint8* buffer;
    /* Set up the writer for a sizing pass */
    NW_WBXML_Writer_SetToSizing(dom_tree->writer);
    /* Call the writer to run the sizing pass */
    status = NW_WBXML_Writer_Header(dom_tree->writer,
                                    version,
                                    publicid,
                                    encoding,
                                    0);


    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Allocate a buffer of the correct size */
    size = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(dom_tree->writer);
    buffer = NW_TinyTree_GetWritableBlock(&(dom_tree->tree), size, &offset);

	 if(buffer == NULL)
	 {
		 return NULL;
	 }

    /* Set up the writer for actual writing */
    NW_WBXML_Writer_SetToWrite(dom_tree->writer, size, buffer);
    /* Do the write */
    status = NW_WBXML_Writer_Header(dom_tree->writer,
                                    version,
                                    publicid,
                                    encoding,
                                    0);

    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Point the root node source offset at the new block */
    node->source_offset = offset;
  }
  return node;
}


NW_TinyTree_Node_t *
NW_TinyDom_createAttributeByToken(NW_TinyDom_Tree_t *dom_tree,
                                  NW_Uint16 token,
                                  NW_TinyDom_AttrVal_t *value)
{
  NW_Status_t status;
  NW_TinyTree_Offset_t offset;

  NW_TinyTree_Node_t * node = NW_TinyTree_createNode(&(dom_tree->tree),0);

  if (node != NULL) {
    NW_Uint8* buffer;
    CXML_Vector_Metric_t size;

    /* Set up the writer for a sizing pass */
    NW_WBXML_Writer_SetToSizing(dom_tree->writer);
    /* Call the writer to run the sizing pass */
    status = NW_Encoder_writeAttributeByToken(dom_tree->writer, token, value,
                                              dom_tree->doc->charset);

    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Allocate a buffer of the correct size */
    size = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(dom_tree->writer);
    buffer = NW_TinyTree_GetWritableBlock(&(dom_tree->tree), size, &offset);
    if (buffer == NULL)
    {
      return NULL;
    }

    /* Set up the writer for actual writing */
    NW_WBXML_Writer_SetToWrite(dom_tree->writer, size, buffer);
    /* Do the write */
    status = NW_Encoder_writeAttributeByToken(dom_tree->writer, token, value,
                                              dom_tree->doc->charset);
    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Point the node source offset at the new block */
    node->source_offset = offset;
    /* Mark node as attribute node */
    NW_TinyTree_Node_setUserFlags(node, T_DOM_NODE_ATTR);
    return node;
  }
  return NULL;
}

NW_TinyTree_Node_t *
NW_TinyDom_createAttributeByName(NW_TinyDom_Tree_t *dom_tree,
                                 NW_String_t *name,
                                 NW_TinyDom_AttrVal_t *value)
{
  NW_Status_t status;
  NW_TinyTree_Offset_t offset;

  NW_TinyTree_Node_t * node = NW_TinyTree_createNode(&(dom_tree->tree),0);

  if (node != NULL) {
    NW_Uint8* buffer;
    CXML_Vector_Metric_t size;

    /* Set up the writer for a sizing pass */
    NW_WBXML_Writer_SetToSizing(dom_tree->writer);
    /* Call the writer to run the sizing pass */
    status = NW_Encoder_writeAttributeByName(dom_tree->writer, name, value,
                                             dom_tree->doc->charset);

    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Allocate a buffer of the correct size */
    size = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(dom_tree->writer);
    buffer = NW_TinyTree_GetWritableBlock(&(dom_tree->tree), size, &offset);
    if (buffer == NULL)
    {
      return NULL;
    }

    /* Set up the writer for actual writing */
    NW_WBXML_Writer_SetToWrite(dom_tree->writer, size, buffer);
    /* Do the write */
    status = NW_Encoder_writeAttributeByName(dom_tree->writer, name, value,
                                             dom_tree->doc->charset);
    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Point the node source offset at the new block */
    node->source_offset = offset;
    /* Mark node as attribute node */
    NW_TinyTree_Node_setUserFlags(node, T_DOM_NODE_ATTR);
    return node;
  }
  return NULL;
}

NW_Status_t
NW_TinyDom_AttributeHandle_initWithStartToken(NW_TinyDom_AttributeHandle_t* tinyHandle,
                                              NW_TinyTree_Node_t** ppNode,
                                              NW_TinyDom_Parser_t* parser,
                                              NW_Uint16 fqToken)
{
  NW_Status_t status;
  NW_TinyTree_Offset_t offset;
  NW_Uint8* buffer;
  CXML_Vector_Metric_t size;
  NW_TinyDom_Tree_t* dom_tree = parser->dom_tree;

  /* Set up the writer for a sizing pass */
  NW_WBXML_Writer_SetToSizing(dom_tree->writer);
  /* Call the writer to run the sizing pass */
  status = NW_WBXML_Writer_AttributeToken(dom_tree->writer, fqToken);
  if (status != NW_STAT_SUCCESS) {
    return status;
  }
  status = NW_WBXML_Writer_End(dom_tree->writer);
  if (status != NW_STAT_SUCCESS) {
    return status;
  }
  /* Allocate a buffer of the correct size */
  size = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(dom_tree->writer);
  buffer = NW_TinyTree_GetWritableBlock(&(dom_tree->tree), size, &offset);
  if (buffer == NULL)
  {
    return NW_STAT_OUT_OF_MEMORY;
  }

  *ppNode = NW_TinyTree_createNode(&(dom_tree->tree), offset);
  if (*ppNode == NULL) {
    NW_Mem_Free(buffer);
    return NW_STAT_OUT_OF_MEMORY;
  }
  NW_TinyTree_Node_setUserFlags(*ppNode, T_DOM_NODE_ATTR);

  /* Set up the writer for actual writing */
  NW_WBXML_Writer_SetToWrite(dom_tree->writer, size, buffer);
  /* Do the write */
  status = NW_WBXML_Writer_AttributeToken(dom_tree->writer, fqToken);
  if (status != NW_STAT_SUCCESS) {
    return status;
  }
  status = NW_WBXML_Writer_End(dom_tree->writer);
  if (status != NW_STAT_SUCCESS) {
    return status;
  }
  /* initialize the attribute handle */
  NW_TinyDom_AttributeHandle_init(tinyHandle, parser, offset);

  return NW_STAT_SUCCESS;
}

NW_Status_t
NW_TinyDom_AttributeHandle_addVal(NW_TinyDom_AttributeHandle_t* tinyHandle,
                                  NW_TinyTree_Node_t* node,
                                  NW_TinyDom_AttrVal_t* val)
{
  NW_TinyDom_AttrVal_t av;
  NW_TinyTree_t* tinyTree = &(tinyHandle->tlit.tiny_parser->dom_tree->tree);
  NW_TinyDom_Tree_t* tinyDomTree = tinyHandle->tlit.tiny_parser->dom_tree;
  NW_Uint32 encoding;
  void* existingBuffer;
  NW_Uint8* buffer;
  CXML_Vector_Metric_t start = 0;
  CXML_Vector_Metric_t valSize;
  CXML_Vector_Metric_t existingSize;
  CXML_Vector_Metric_t offset;
  NW_Status_t status = NW_STAT_FAILURE;

  encoding = NW_TinyDom_getDocHeader(tinyTree)->charset;

  switch (val->type) {
  case NW_WBXML_ATTR_COMPONENT_TOKEN:
  case NW_WBXML_ATTR_COMPONENT_STRING:
  case NW_WBXML_ATTR_COMPONENT_EXT:
  case NW_WBXML_ATTR_COMPONENT_ENTITY:
  case NW_WBXML_ATTR_COMPONENT_OPAQUE:
    NW_WBXML_Writer_SetToSizing(tinyDomTree->writer);
    start
      = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(tinyDomTree->writer);
    status = NW_Encoder_writeAttrVal(tinyDomTree->writer, val,
                                     encoding);
    if (status != NW_STAT_SUCCESS) {
      return status;
    }
    status = NW_WBXML_Writer_End(tinyDomTree->writer);
    break;
  }
  if (status != NW_STAT_SUCCESS) {
    return status;
  }
  valSize
    = (CXML_Vector_Metric_t)(NW_WBXML_Writer_GetSize(tinyDomTree->writer)
       - start);

  /* existing content size */
  start = node->source_offset;
  NW_TinyDom_AttributeHandle_init(tinyHandle, tinyHandle->tlit.tiny_parser,
                                  node->source_offset);
  tinyHandle->value = &av; /* required to hold value results */
  while (NW_TinyDom_AttributeHandle_valsIterate(&(tinyHandle->tlit)) != 0) {
    /* empty loop body: we just want parser to advance */
  }
  existingSize = (CXML_Vector_Metric_t)(tinyHandle->tlit.offset - start);
  existingBuffer = NW_TinyTree_Node_getSourceAddress(tinyTree, node);

  buffer = NW_TinyTree_GetWritableBlock(tinyTree,
                                        ((CXML_Vector_Metric_t)
                                         (valSize + existingSize)),
                                        &offset);
  if (buffer == NULL) {
    return NW_STAT_OUT_OF_MEMORY;
  }
  NW_Mem_memcpy(buffer, existingBuffer, existingSize);
  NW_WBXML_Writer_SetToWrite(tinyDomTree->writer,
                             valSize,
                             buffer + existingSize);
  (void)NW_Encoder_writeAttrVal(tinyDomTree->writer, val, encoding);
  (void)NW_WBXML_Writer_End(tinyDomTree->writer);

  /* TBD there is no way to "free" the old storage */
  node->source_offset = offset;
  return NW_STAT_SUCCESS;
}


NW_TinyTree_Node_t *
NW_TinyDom_createElementByToken(NW_TinyDom_Tree_t *dom_tree, NW_Uint16 token) {

  NW_Status_t status;
  NW_TinyTree_Offset_t offset;

  NW_TinyTree_Node_t * node = NW_TinyTree_createNode(&(dom_tree->tree),0);

  if (node != NULL) {
    NW_Uint8* buffer;
    CXML_Vector_Metric_t size;

    /* Set up the writer for a sizing pass */
    NW_WBXML_Writer_SetToSizing(dom_tree->writer);
    /* Call the writer to run the sizing pass */
    status = NW_Encoder_writeElementByToken(dom_tree->writer, token);

    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Allocate a buffer of the correct size */
    size = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(dom_tree->writer);
    buffer = NW_TinyTree_GetWritableBlock(&(dom_tree->tree), size, &offset);
    if (buffer == NULL)
    {
      return NULL;
    }

    /* Set up the writer for actual writing */
    NW_WBXML_Writer_SetToWrite(dom_tree->writer, size, buffer);
    /* Do the write */
    status = NW_Encoder_writeElementByToken(dom_tree->writer, token);
    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }

    /* Point the node source offset at the new block */
    node->source_offset = offset;
    /* Mark node as attribute node */
    NW_TinyTree_Node_setUserFlags(node, T_DOM_NODE_TAG);
    return node;
  }
  return NULL;
}


NW_TinyTree_Node_t *
NW_TinyDom_createElementByName(NW_TinyDom_Tree_t *dom_tree, NW_String_t *name)
{
  NW_Status_t status;
  NW_TinyTree_Offset_t offset;

  NW_TinyTree_Node_t * node = NW_TinyTree_createNode(&(dom_tree->tree),0);

  if (node != NULL) {
    NW_Uint8* buffer;
    CXML_Vector_Metric_t size;

    /* Set up the writer for a sizing pass */
    NW_WBXML_Writer_SetToSizing(dom_tree->writer);
    /* Call the writer to run the sizing pass */
    status = NW_Encoder_writeElementByName(dom_tree->writer, name,
                                           dom_tree->doc->charset);
    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Allocate a buffer of the correct size */
    size = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(dom_tree->writer);
    buffer = NW_TinyTree_GetWritableBlock(&(dom_tree->tree), size, &offset);
    if (buffer == NULL)
    {
      return NULL;
    }

    /* Set up the writer for actual writing */
    NW_WBXML_Writer_SetToWrite(dom_tree->writer, size, buffer);
    /* Do the write */
    status = NW_Encoder_writeElementByName(dom_tree->writer, name,
                                           dom_tree->doc->charset);
    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }

    /* Point the node source offset at the new block */
    node->source_offset = offset;
    /* Mark node as attribute node */
    NW_TinyTree_Node_setUserFlags(node, T_DOM_NODE_TAG);
    return node;
  }
  return NULL;
}

NW_TinyTree_Node_t *
NW_TinyDom_createTextNode(NW_TinyDom_Tree_t *dom_tree,
                          NW_TinyDom_Text_t *text)
{
  NW_Status_t status;
  NW_TinyTree_Offset_t offset;

  NW_TinyTree_Node_t * node = NW_TinyTree_createNode(&(dom_tree->tree),0);

  if (node != NULL) {
    NW_Uint8* buffer;
    CXML_Vector_Metric_t size;

    /* Set up the writer for a sizing pass */
    NW_WBXML_Writer_SetToSizing(dom_tree->writer);
    /* Call the writer to run the sizing pass */
    status = NW_Encoder_writeText(dom_tree->writer, text,
                                  dom_tree->doc->charset);
    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Allocate a buffer of the correct size */
    size = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(dom_tree->writer);
    buffer = NW_TinyTree_GetWritableBlock(&(dom_tree->tree), size, &offset);
    if (buffer == NULL)
    {
      return NULL;
    }

    /* Set up the writer for actual writing */
    NW_WBXML_Writer_SetToWrite(dom_tree->writer, size, buffer);
    /* Do the write */
    status = NW_Encoder_writeText(dom_tree->writer, text,
                                  dom_tree->doc->charset);
    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    /* Point the node source offset at the new block */
    node->source_offset = offset;
    /* Mark node as attribute node */
    NW_TinyTree_Node_setUserFlags(node, T_DOM_NODE_TEXT);
    return node;
  }
  return NULL;
}

NW_Status_t
NW_TinyDom_addDataFromTextItem(NW_TinyTree_t* tinyTree,
                               NW_TinyDom_Tree_t* tinyDomTree,
                               NW_DOM_TextNode_t* node,
                               NW_TinyDom_AttrVal_t* val,
                               NW_Uint32 encoding)
{
  void* existingBuffer;
  NW_Uint8* buffer;
  NW_TinyDom_TextHandle_t iterator;
  NW_TinyDom_Text_t textItem;
  CXML_Vector_Metric_t start = 0;
  CXML_Vector_Metric_t valSize;
  CXML_Vector_Metric_t existingSize;
  CXML_Vector_Metric_t offset;
  NW_Status_t status = NW_STAT_FAILURE;

  switch (val->type) {
  case NW_WBXML_ATTR_COMPONENT_STRING:
  case NW_WBXML_ATTR_COMPONENT_EXT:
  case NW_WBXML_ATTR_COMPONENT_ENTITY:
  case NW_WBXML_ATTR_COMPONENT_OPAQUE:
    NW_WBXML_Writer_SetToSizing(tinyDomTree->writer);
    start
      = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(tinyDomTree->writer);
    status = NW_Encoder_writeAttrVal(tinyDomTree->writer, val,
                                     encoding);
    if (status != NW_STAT_SUCCESS) {
      return status;
    }
    status = NW_WBXML_Writer_End(tinyDomTree->writer);
    break;
  }
  if (status != NW_STAT_SUCCESS) {
    return status;
  }
  valSize
    = (CXML_Vector_Metric_t)(NW_WBXML_Writer_GetSize(tinyDomTree->writer)
       - start);

  /* existing content size */
  NW_TinyDom_TextHandle_init(&iterator,
                             NW_TinyDom_getParser(tinyTree),
                             NW_TinyTree_Node_getSourceOffset(node));
  start = (CXML_Vector_Metric_t)iterator.tlit.offset;
  while (NW_TinyDom_TextHandle_iterate(&iterator, &textItem) != 0) {
    /* empty loop body: we just want parser to advance */
  }
  existingSize = (CXML_Vector_Metric_t)(iterator.tlit.offset - start);
  existingBuffer = NW_TinyTree_Node_getSourceAddress(tinyTree, node);

  buffer = NW_TinyTree_GetWritableBlock(tinyTree,
                                        ((CXML_Vector_Metric_t)
                                         (valSize + existingSize)),
                                        &offset);
  if (buffer == NULL) {
    return NW_STAT_OUT_OF_MEMORY;
  }
  NW_Mem_memcpy(buffer, existingBuffer, existingSize);
  NW_WBXML_Writer_SetToWrite(tinyDomTree->writer,
                             valSize,
                             buffer + existingSize);
  (void)NW_Encoder_writeAttrVal(tinyDomTree->writer, val, encoding);
  (void)NW_WBXML_Writer_End(tinyDomTree->writer);

  /* TBD there is no way to "free" the old storage */
  node->source_offset = offset;
  return NW_STAT_SUCCESS;
}

NW_Status_t
NW_TinyDom_removeAttrFromListNode(NW_TinyDom_AttrListHandle_t *it, NW_Uint32 length)
{
  NW_Uint16 len = 0;
  NW_Byte* p = NULL;
  NW_Byte *moveTo = NULL;
  NW_Byte *moveFrom = NULL;

  p = (NW_Byte*)((NW_Byte*)it->segment + it->offset);
  moveTo = (NW_Byte*)(p - length);
  moveFrom = p;
  while (*p != 0x01)
  {
    len++;
    p++;
  }
  NW_Mem_memset(moveTo, 0, length);
  NW_Mem_memmove(moveTo, moveFrom, (len+1));
  return NW_STAT_SUCCESS;
}







