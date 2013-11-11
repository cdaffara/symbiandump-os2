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
#include <xml/cxml/nw_encoder_domencoder.h>
#include <xml/cxml/nw_encoder_stringtable.h>
#include <xml/cxml/nw_dom_element.h>
#include <xml/cxml/nw_dom_text.h>
#include <xml/cxml/nw_dom_attribute.h>

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/*
* Encodes an Attribute Val
* Returns NW_STAT_BAD_INPUT_PARAM
*         NW_STAT_OUT_OF_MEMORY
*         NW_STAT_SUCCESS
*/
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_encodeAttrVal(NW_Encoder_t * encoder, NW_DOM_AttrVal_t *val)
{
  NW_Status_t status;

  if ((encoder == NULL) || (val == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }
  status = NW_STAT_SUCCESS;
  switch(NW_DOM_AttrVal_getType(val))
  {
  case NW_DOM_ATTR_VAL_STRING:
    {
      status = NW_WBXML_Writer_Text(&encoder->writer, encoder->encoding,
                                    val->component.string.length,
                                    val->component.string.storage);
      break;
    }
  case NW_DOM_ATTR_VAL_EXTENSION:
    {
      NW_Uint8 t; /* 8-bit token */
      NW_String_t str;

      t = (NW_Uint8)(val->component.ext.token);
      if ((t == NW_WBXML_EXT_0)
          || (t == NW_WBXML_EXT_1)
          || (t == NW_WBXML_EXT_2)) {
        status
          = NW_WBXML_Writer_Extension(&encoder->writer,
                                      (NW_Uint16)(val->component.ext.token),
                                      0, 0, NULL);
      } else if ((t == NW_WBXML_EXT_T_0)
                 || (t == NW_WBXML_EXT_T_1)
                 || (t == NW_WBXML_EXT_T_2)) {
        NW_ASSERT((val->component.ext.type
                   == NW_TINYDOM_EXTENSION_TYPE_NORMAL)
                  || (val->component.ext.type
                     == NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER));
        if (val->component.ext.type == NW_TINYDOM_EXTENSION_TYPE_NORMAL) {
          NW_Uint16 token = NW_DOM_TextItem_getExtension(val, &str);
          status
            = NW_WBXML_Writer_ExtensionUseStringTable(&encoder->writer,
                                                      token,
                                                      str.length,
                                                      str.storage);
        } else {
          status = NW_WBXML_Writer_Extension(&encoder->writer,
                                             (NW_Uint16)(val->component.ext.token),
                                             val->component.ext.value.x,
                                             0, NULL);
        }
      } else if ((t == NW_WBXML_EXT_I_0)
                 || (t == NW_WBXML_EXT_I_1)
                 || (t == NW_WBXML_EXT_I_2)) {
        NW_Uint16 token = NW_DOM_TextItem_getExtension(val, &str);
        status = NW_WBXML_Writer_Extension(&encoder->writer, token,
                                           0, str.length, str.storage);
      } else {
        status = NW_STAT_FAILURE;
      }
      break;
    }
  case NW_DOM_ATTR_VAL_ENTITY:
    {
      NW_Uint32 entity;
      entity = NW_DOM_TextItem_getEntity(val);
      status = NW_WBXML_Writer_Entity(&encoder->writer, entity);
      break;
    }
  case NW_DOM_ATTR_VAL_OPAQUE:
    {
      NW_Uint32 length;
      NW_Byte *data;
      data = NW_DOM_AttrVal_getOpaque(val, &length);
      status = NW_WBXML_Writer_Opaque(&encoder->writer, length, data);
      break;
    }
  case NW_DOM_ATTR_VAL_TOKEN:
    {
      NW_Uint16 fqToken = NW_DOM_AttrVal_getToken(val);
      status = NW_WBXML_Writer_AttributeToken(&encoder->writer, fqToken);
      break;
    }
  default:
    return NW_STAT_FAILURE;
  }
  return status;
}

/*
* Encodes an attribute
* Returns NW_STAT_BAD_INPUT_PARAM
*         NW_STAT_OUT_OF_MEMORY
*         NW_STAT_SUCCESS
*/
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_encodeAttribute(NW_Encoder_t * encoder,
                           NW_DOM_AttributeHandle_t * attrHandle)
{
  NW_Status_t status;
  NW_DOM_AttrVal_t attrVal;
  NW_Uint16 fqToken = 0;
  NW_Uint8 token = 0;
  NW_Ucs2 c;
  NW_Uint32 numbytes;

  fqToken = NW_DOM_AttributeHandle_getToken(attrHandle);

  if ((encoder == NULL) || (attrHandle == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }
  token = (NW_Uint8)(fqToken & NW_WBXML_MASK_TOKEN);

  if (token == NW_WBXML_LITERAL)
  {
    NW_String_t attributeName;
    if (NW_DOM_AttributeHandle_getName(attrHandle, &attributeName)
        != NW_STAT_SUCCESS){
      return NW_STAT_FAILURE;
    }

    numbytes = NW_String_readChar(attributeName.storage, &c, encoder->encoding);

    status = NW_WBXML_Writer_AttributeNameString(&encoder->writer,
                                                 encoder->encoding,
                                                 (attributeName.length - numbytes)/numbytes,
                                                 attributeName.length,
                                                 attributeName.storage);
  }
  else{
    status = NW_WBXML_Writer_AttributeToken(&encoder->writer, fqToken);
  }

  while (NW_DOM_AttributeHandle_getNextVal(attrHandle, &attrVal)
         == NW_STAT_WBXML_ITERATE_MORE)
  {
    status = NW_Encoder_encodeAttrVal(encoder, &attrVal);
    if (status != NW_STAT_SUCCESS)
      return status;
  }
  return NW_STAT_SUCCESS;
}

/*
* Encodes an element node
* Returns NW_STAT_BAD_INPUT_PARAM
*         NW_STAT_OUT_OF_MEMORY
*         NW_STAT_SUCCESS
*/
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_encodeElementNode(NW_Encoder_t * encoder, NW_DOM_ElementNode_t * e)
{
  NW_Status_t status;
  NW_DOM_AttributeListIterator_t listIterator;
  NW_DOM_AttributeHandle_t attrHandle;
  NW_Uint16 fqToken;
  NW_Uint8 token;
  NW_Uint32 tagIndex;
  NW_Int32 charCount;

  if ((encoder == NULL) || (e == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }
  fqToken = NW_DOM_ElementNode_getTagToken(e);
  token = (NW_Uint8)(fqToken & NW_WBXML_MASK_TOKEN);
  if (token == NW_WBXML_LITERAL)
  {
    NW_String_t elementName;

    if (NW_DOM_ElementNode_getTagName(e, &elementName) != NW_STAT_SUCCESS){
      return NW_STAT_FAILURE;
    }

    /*Get the char count */

    charCount = NW_String_charBuffGetLength(elementName.storage,
                                          encoder->encoding,
                                          &(elementName.length) );

    status = NW_WBXML_Writer_TagString(&encoder->writer, encoder->encoding, charCount,
                                elementName.length, elementName.storage, &tagIndex);
    /* Do not free literal element here as this will be freed when literal table 
     * will be freed.
     */

    // NW_String_delete(&elementName);
  }
  else{
    status = NW_WBXML_Writer_TagToken(&encoder->writer, fqToken, &tagIndex);
    NW_ASSERT(status == NW_STAT_SUCCESS);
  }

  if (NW_DOM_Node_getFirstChild(e)){
    status = NW_WBXML_Writer_TagSetContentFlag(&encoder->writer, tagIndex);
    NW_ASSERT(status == NW_STAT_SUCCESS);
  }
  if (NW_DOM_ElementNode_hasAttributes(e)){
    status = NW_WBXML_Writer_TagSetAttributesFlag(&encoder->writer, tagIndex);
    NW_ASSERT(status == NW_STAT_SUCCESS);
  }

  if (status != NW_STAT_SUCCESS){
    return status;
  }

  if (NW_DOM_ElementNode_hasAttributes(e))
  {
    status = NW_DOM_ElementNode_getAttributeListIterator(e, &listIterator);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
    while (NW_DOM_AttributeListIterator_getNextAttribute(&listIterator, &attrHandle)
      == NW_STAT_WBXML_ITERATE_MORE)
    {
      NW_Encoder_encodeAttribute(encoder, &attrHandle);
    }
    return NW_WBXML_Writer_End(&encoder->writer);
  }
  return NW_STAT_SUCCESS;
}

/*
* Encodes a TextNode
* Returns NW_STAT_BAD_INPUT_PARAM
*         NW_STAT_OUT_OF_MEMORY
*         NW_STAT_SUCCESS
*/
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_encodeTextNode (NW_Encoder_t * encoder, NW_DOM_TextNode_t * textNode)
{
  NW_DOM_TextItemIterator_t textIter;
  NW_DOM_TextItem_t item;
  NW_Status_t status;

  if ((encoder == NULL) || (textNode == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  status = NW_DOM_TextNode_getTextItemIterator(textNode, &textIter);
  while (NW_DOM_TextItemIterator_getNextTextItem(&textIter, &item)
         == NW_STAT_WBXML_ITERATE_MORE)
  {
    status = NW_Encoder_encodeAttrVal(encoder, &item);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
  }
  return NW_STAT_SUCCESS;
}

/*
* Encodes a Node (recursive)
* Returns NW_STAT_BAD_INPUT_PARAM
*         NW_STAT_OUT_OF_MEMORY
*         NW_STAT_SUCCESS
*/
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_encodeNode(NW_Encoder_t * encoder, NW_DOM_Node_t *node)
{
  NW_Uint32 type = NW_DOM_Node_getNodeType(node);

  if ((encoder == NULL) || (node == NULL))
    return NW_STAT_BAD_INPUT_PARAM;

  switch (type)
  {
  case NW_DOM_ELEMENT_NODE:
    {
      NW_Encoder_encodeElementNode(encoder, node);
      if (NW_DOM_Node_getFirstChild(node))
      {
        NW_Encoder_encodeNode(encoder, NW_DOM_Node_getFirstChild(node));
        /* Encode the tag terminator */
        NW_WBXML_Writer_End(&encoder->writer);
      }
      if (NW_DOM_Node_getNextSibling(node))
        NW_Encoder_encodeNode(encoder, NW_DOM_Node_getNextSibling(node));
    }
    break;

  case NW_DOM_TEXT_NODE:
    NW_Encoder_encodeTextNode(encoder, node);

    if (NW_DOM_Node_getNextSibling(node)){
      NW_Encoder_encodeNode (encoder, NW_DOM_Node_getNextSibling(node));
    }
    break;

  case NW_DOM_PROCESSING_INSTRUCTION_NODE:
    /* TODO: Pi Node support code */
    if (NW_DOM_Node_getNextSibling(node))
      NW_Encoder_encodeNode (encoder, NW_DOM_Node_getNextSibling(node));

    break;

  default:
    if (NW_DOM_Node_getNextSibling(node))
      NW_Encoder_encodeNode (encoder, NW_DOM_Node_getNextSibling(node));
  }
  return NW_STAT_SUCCESS;
}

/*
* Encodes the header of document (version, publicid, docType, charset,
* and string table
* Returns NW_STAT_BAD_INPUT_PARAM
*         NW_STAT_OUT_OF_MEMORY
*         NW_STAT_SUCCESS
*/
/* ------------------------------------------------------------------------- */
NW_Status_t
NW_Encoder_encodeDocHeader(NW_Encoder_t * encoder, NW_DOM_DocumentNode_t * doc)
{
  NW_Uint8 version;
  NW_Uint32 publicid;
  NW_Uint32 byteLength;

  version = NW_DOM_DocumentNode_getVersion(doc);
  publicid = NW_DOM_DocumentNode_getPublicIdAsNumber(doc);
  if (encoder->enableStringTable == NW_TRUE) {
    byteLength = NW_Encoder_StringTable_getTotalBytes(encoder->stringTable);
  } else {
    byteLength = 0;
  }

  /* This call also encodes the string table, if any is used. */
  return NW_WBXML_Writer_Header(&encoder->writer, version, publicid,
                                encoder->encoding, byteLength);
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/*
* Initializes an encoder
* Returns NW_STAT_BAD_INPUT_PARAM
*         NW_STAT_OUT_OF_MEMORY
*         NW_STAT_SUCCESS
*/
/* ------------------------------------------------------------------------- */
NW_Status_t
NW_Encoder_initialize(NW_Encoder_t *encoder, NW_Bool enableStringTable)
{
  if (encoder == NULL){
    return NW_STAT_BAD_INPUT_PARAM;
  }
  encoder->encoding = 0;
  encoder->enableStringTable = enableStringTable;
  encoder->stringTable = NULL;
  if (enableStringTable == NW_TRUE) {
    encoder->stringTable = NW_Encoder_StringTable_new();
    if (encoder->stringTable == NULL) {
      return NW_STAT_OUT_OF_MEMORY;
    }
  }
  return NW_STAT_SUCCESS;
}

/*
* Encodes the DOM document represented by document node
* Returns NW_STAT_BAD_INPUT_PARAM
*         NW_STAT_OUT_OF_MEMORY
*         NW_STAT_SUCCESS
*/
/* ------------------------------------------------------------------------- */
EXPORT_C NW_Status_t
NW_Encoder_encodeWBXML(NW_Encoder_t* encoder,
                       NW_DOM_DocumentNode_t * docNode,
                       NW_Bool enableStringTable,
                       NW_Uint32 *length,
                       NW_Byte **buffer)
{
  NW_Status_t status;
  NW_DOM_ElementNode_t *elem;
  NW_Uint32 publicid;
  NW_WBXML_Dictionary_t* dictionary;

  if ((encoder == NULL) || (docNode == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  status = NW_Encoder_initialize(encoder, enableStringTable);
  if (status != NW_STAT_SUCCESS){
    return status;
  }

  encoder->encoding = NW_DOM_DocumentNode_getCharacterEncoding(docNode);
  elem = NW_DOM_DocumentNode_getDocumentElement(docNode);
  publicid = NW_DOM_DocumentNode_getPublicIdAsNumber(docNode);

  /* initialize WBXMLWriter */
  if (publicid != 0){
    dictionary = NW_WBXML_Dictionary_getByPublicId(publicid);
  }
  else
  {
    NW_String_t docType;

    status = NW_DOM_DocumentNode_getPublicId(docNode, &docType);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
    dictionary = NW_WBXML_Dictionary_getByDocType(&docType, encoder->encoding);
  }
  if (enableStringTable == NW_TRUE) {
    /* assuming that same dictionary is used for both attributes and tags */
    status = NW_Encoder_StringTable_createFromDOM(encoder->stringTable,
                                                  docNode,
                                                  dictionary);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
  }

  /* Set up the writer for a sizing pass */
  NW_WBXML_Writer_SetToSizing(&encoder->writer);

  NW_WBXML_Writer_Initialize(&encoder->writer,
                             0,
                             NULL,
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
                              encoder->stringTable :
                              NULL),
                             ((enableStringTable == NW_TRUE) ?
                              NW_Encoder_StringTable_StringTableIterateInit :
                              NULL),
                             ((enableStringTable == NW_TRUE) ?
                              NW_Encoder_StringTable_StringTableIterateNext :
                              NULL),
                             NW_TRUE /* sizing only */);

  status = NW_Encoder_encodeDocHeader(encoder, docNode);
  if (status != NW_STAT_SUCCESS){
    return status;
  }
  status = NW_Encoder_encodeNode(encoder, elem);

    /* Allocate a buffer of the correct size */
  *length = (CXML_Vector_Metric_t)NW_WBXML_Writer_GetSize(&encoder->writer);
  *buffer = (NW_Byte*)NW_Mem_Malloc(*length);

  if (*buffer == NULL){
    return NW_STAT_OUT_OF_MEMORY;
  }

  /* Set up the writer for a writing pass */
  NW_WBXML_Writer_SetToWrite(&encoder->writer, *length, *buffer);

  status = NW_Encoder_encodeDocHeader(encoder, docNode);
  if (status != NW_STAT_SUCCESS){
    return status;
  }
  status = NW_Encoder_encodeNode(encoder, elem);
  NW_ASSERT(status == NW_STAT_SUCCESS);

  NW_Encoder_StringTable_delete(encoder->stringTable);
  return NW_STAT_SUCCESS;
}

