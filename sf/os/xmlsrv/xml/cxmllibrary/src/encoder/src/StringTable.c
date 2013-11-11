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


#include <xml/cxml/nw_encoder_stringtable.h>
#include <xml/cxml/nw_wbxml_parse.h>
#include <xml/cxml/nw_dom_attribute.h>
#include <xml/cxml/nw_dom_element.h>
#include <xml/cxml/nw_dom_text.h>
#include <xml/cxml/nw_dom_document.h>
#include "cxml_internal.h"

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_StringItem_initialize(NW_Encoder_StringItem_t *strItem,
                                  NW_String_t *str)
{
  NW_Status_t status = NW_STAT_SUCCESS;

  if (strItem == NULL){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  if(str == NULL)
  {
   return NW_STAT_BAD_INPUT_PARAM;
  }
 

   strItem->data.storage = CXML_Mem_Malloc(str->length + 1);

   if(strItem->data.storage == NULL)
   {
    return NW_STAT_OUT_OF_MEMORY;
   }

    NW_Mem_memcpy(strItem->data.storage,str->storage,str->length);

    strItem->data.storage[str->length] = '\0';
    
    if (status == NW_STAT_SUCCESS)
    {
     strItem->frequency = 1;
     strItem->data.length = str->length; 
    }
    else
    {
     status = NW_STAT_OUT_OF_MEMORY;
    }

  return status;
}

/*
 * Finds string contained in an item (AttrValItem or Text Item)
 * An item can be string, extension, entity, opaque, or token
 * Only extension and string may contain strings
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_SUCCESS
 */
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_StringTable_getItemString(NW_Encoder_StringTable_t *strTable,
                                     NW_DOM_AttrVal_t *item)
{
  NW_Uint32 ignoreVal;

  if ((strTable == NULL) || (item == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  switch(item->type) {
  case NW_DOM_ATTR_VAL_STRING:
    return NW_Encoder_StringTable_append(strTable, &item->component.string,
                                         NW_FALSE, &ignoreVal);

  case NW_DOM_ATTR_VAL_EXTENSION:
    {
      NW_Uint8 t; /* 8-bit token */

      t = (NW_Uint8)(item->component.ext.token);
      if ((item->component.ext.type == NW_TINYDOM_EXTENSION_TYPE_NORMAL)
          && ((t == NW_WBXML_EXT_T_0)
              || (t == NW_WBXML_EXT_T_1)
              || (t == NW_WBXML_EXT_T_2))) {
        return NW_Encoder_StringTable_append(strTable,
                                             &(item->component.ext.value.string),
                                             NW_FALSE, &ignoreVal);
      }
      break;
    }
  default:
    break;
  }
  return NW_STAT_SUCCESS;
}

/**
 * Finds strings in an attribute - iterates over attribute values
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_SUCCESS
 */
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_StringTable_getAttributeStrings(NW_Encoder_StringTable_t *strTable,
                                           NW_DOM_AttributeHandle_t *attrHandle)
{
  NW_Status_t status;
  NW_DOM_AttrVal_t attrVal;
  NW_Uint16 token;
  NW_Uint16 fqToken;

  if ((strTable == NULL) || (attrHandle == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  fqToken = NW_DOM_AttributeHandle_getToken(attrHandle);
  NW_ASSERT(fqToken);
  token = (NW_Uint16)(fqToken & NW_WBXML_MASK_TOKEN);
  if (token == NW_WBXML_LITERAL)
  {
    NW_Uint32 ignoreVal;
    NW_String_t attributeName;

    status = NW_DOM_AttributeHandle_getName(attrHandle, &attributeName);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
    status = NW_Encoder_StringTable_append(strTable, &attributeName, NW_TRUE,
                                           &ignoreVal);
  }

  while (NW_DOM_AttributeHandle_getNextVal(attrHandle, &attrVal) == NW_STAT_WBXML_ITERATE_MORE)
  {
    status = NW_Encoder_StringTable_getItemString(strTable, &attrVal);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
  }

  return NW_STAT_SUCCESS;
}

/**
 * Finds strings in an Element Node
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_SUCCESS
 */
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_StringTable_getElementStrings(NW_Encoder_StringTable_t *strTable,
                                                NW_DOM_ElementNode_t *e)
{
  NW_Status_t status;
  NW_DOM_AttributeListIterator_t listIterator;
  NW_DOM_AttributeHandle_t attrHandle;
  NW_Uint32 fqToken;
  NW_Uint8 token;

  if ((strTable == NULL) || (e == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  fqToken = NW_DOM_ElementNode_getTagToken(e);
  if (fqToken == 0){
    return NW_STAT_FAILURE;
  }

  token = (NW_Uint8)(fqToken & NW_WBXML_MASK_TOKEN);

  /* put element in table if it is LITERAL tag */
  if (token == NW_WBXML_LITERAL)
  {
    NW_String_t elementName;
    NW_Uint32 ignoreVal;

    if (NW_DOM_ElementNode_getTagName(e, &elementName) != NW_STAT_SUCCESS){
      return NW_STAT_FAILURE;
    }
    status = NW_Encoder_StringTable_append(strTable, &elementName, NW_TRUE,
                                           &ignoreVal);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
  }

  /* go through attribute strings */
  if (NW_DOM_ElementNode_hasAttributes(e))
  {
    status = NW_DOM_ElementNode_getAttributeListIterator(e, &listIterator);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
    while (NW_DOM_AttributeListIterator_getNextAttribute(&listIterator, &attrHandle) == NW_STAT_WBXML_ITERATE_MORE)
    {
      status = NW_Encoder_StringTable_getAttributeStrings(strTable, &attrHandle);
      if (status != NW_STAT_SUCCESS){
        return status;
      }
    }
  }
  return NW_STAT_SUCCESS;
}

/**
 * Finds strings in a TextNode
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_SUCCESS
 */
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_StringTable_getTextStrings(NW_Encoder_StringTable_t *strTable,
                                      NW_DOM_TextNode_t *textNode)
{
  NW_DOM_TextItemIterator_t textIter;
  NW_DOM_TextItem_t item;
  NW_Status_t status;

  if ((strTable == NULL) || (textNode == NULL))
  {
    return NW_STAT_BAD_INPUT_PARAM;
  }

  (void) NW_DOM_TextNode_getTextItemIterator(textNode, &textIter);
  while (NW_DOM_TextItemIterator_getNextTextItem(&textIter, &item) == NW_STAT_WBXML_ITERATE_MORE)
  {
    status = NW_Encoder_StringTable_getItemString(strTable, &item);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
  }
  return NW_STAT_SUCCESS;
}

/**
 * Finds strings in a Node (Recursive in nature)
 * So it finds strings in entire tree
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_SUCCESS
 */
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_StringTable_getNodeStrings(NW_Encoder_StringTable_t *strTable,
                                      NW_DOM_Node_t *node,
                                      NW_WBXML_Dictionary_t *dictionary)
{
  NW_Uint32 type;

  type = NW_DOM_Node_getNodeType(node);

  if ((strTable == NULL) || (node == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  switch (type)
  {
    case NW_DOM_ELEMENT_NODE:
      {
        NW_Encoder_StringTable_getElementStrings(strTable, node);
        if (NW_DOM_Node_getFirstChild(node))
        {
          NW_Encoder_StringTable_getNodeStrings(strTable, NW_DOM_Node_getFirstChild(node), dictionary);
        }

        if (NW_DOM_Node_getNextSibling(node))
        {
          NW_Encoder_StringTable_getNodeStrings(strTable, NW_DOM_Node_getNextSibling(node), dictionary);
        }
      }
      break;

    case NW_DOM_TEXT_NODE:
      NW_Encoder_StringTable_getTextStrings(strTable, node);

      if (NW_DOM_Node_getNextSibling(node))
      {
        NW_Encoder_StringTable_getNodeStrings (strTable, NW_DOM_Node_getNextSibling(node), dictionary);
      }

      break;

    case NW_DOM_PROCESSING_INSTRUCTION_NODE:
/*      PiPrint (node, encoding); */

     if (NW_DOM_Node_getNextSibling(node))
     {
       NW_Encoder_StringTable_getNodeStrings (strTable, NW_DOM_Node_getNextSibling(node), dictionary);
     }

      break;

    default:
      if (NW_DOM_Node_getNextSibling(node))
      {
        NW_Encoder_StringTable_getNodeStrings (strTable, NW_DOM_Node_getNextSibling(node), dictionary);
      }
  }
  return NW_STAT_SUCCESS;
}

/**
 * Finds all the string in document
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_OUT_OF_MEMORY
 *         NW_STAT_SUCCESS
 */
/* ------------------------------------------------------------------------- */
static
NW_Status_t
NW_Encoder_StringTable_findDocStrings(NW_Encoder_StringTable_t *strTable,
                                      NW_DOM_DocumentNode_t *doc,
                                      NW_WBXML_Dictionary_t *dictionary)
{
  NW_String_t docType;
  NW_DOM_ElementNode_t *elem;
  NW_Status_t status;

  if ((strTable == NULL) || (doc == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  if (NW_DOM_DocumentNode_getPublicIdAsNumber(doc) == 0)
  {
    NW_Uint32 ignoreVal;
    status = NW_DOM_DocumentNode_getPublicId(doc, &docType);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
    status = NW_Encoder_StringTable_append(strTable, &docType, NW_TRUE,
                                           &ignoreVal);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
  }
  elem = NW_DOM_DocumentNode_getDocumentElement(doc);
  status = NW_Encoder_StringTable_getNodeStrings(strTable, elem, dictionary);
  return status;
}

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */

/*
 * Creates a new String Table
 */
/* ------------------------------------------------------------------------- */
EXPORT_C
NW_Encoder_StringTable_t *
NW_Encoder_StringTable_new()
{
  NW_Encoder_StringTable_t* strTable;

  strTable = (NW_Encoder_StringTable_t*)
              NW_Mem_Malloc(sizeof(NW_Encoder_StringTable_t));
  if (strTable != NULL){
    strTable->vector = CXML_Vector_Construct(sizeof(NW_Encoder_StringItem_t), 4);
		if(strTable->vector == NULL)
		{
			NW_Mem_Free(strTable);
			return NULL;
		}
  }

  return strTable;
}

/*
 * Deletes a String Table
 */
/* ------------------------------------------------------------------------- */

EXPORT_C void
NW_Encoder_StringTable_delete(NW_Encoder_StringTable_t* strTable)
{
  CXML_Vector_t* vectorObj = NULL;
  NW_Encoder_StringItem_t* strItem = NULL;
  CXML_Vector_Metric_t index;

    if (strTable == NULL)
    {
     return;
    }
  
  if(strTable->vector)
  {
   vectorObj = strTable->vector;
  }
  
  //Free the data memory.

  
  for (index = 0; index < vectorObj->size; index++) 
  {
    strItem = (NW_Encoder_StringItem_t*) CXML_Vector_AddressAt(vectorObj,index);
    if(strItem)       
    {
     if((strItem->data).storage)
     {
      NW_Mem_Free ( (strItem->data).storage);
     }
    }		
  }/*end for() */

  CXML_Vector_Destruct(strTable->vector);
  NW_Mem_Free(strTable);
}

/* returns number of elements in the string table */
/* ------------------------------------------------------------------------- */
EXPORT_C CXML_Vector_Metric_t
NW_Encoder_StringTable_GetSize(NW_Encoder_StringTable_t *strTable)
{
  return (NW_Uint32)strTable->vector->size;
}

/**
 * Appends new found string in the document to the list. If the string
 * was already present in the list, then it increases its frequency,
 * otherwise it creates a new StringItem in the list
 *
 * MODIFIED index n= of the string into the table
 *
 * Returns NW_STAT_BAD_INPUT_PARAM
 *         NW_STAT_STR_TBL_OVERFLOW
 *         NW_STAT_OUT_OF_MEMORY
 *         NW_STAT_SUCCESS
 */
/* ------------------------------------------------------------------------- */
NW_Status_t
NW_Encoder_StringTable_append(NW_Encoder_StringTable_t *strTable,
                              NW_String_t *string,
                              NW_Bool literalOrDocType,
                              NW_Uint32 *extensionByteOffset)
{
  NW_Encoder_StringItem_t* strItem;
  NW_Encoder_StringItem_t newStrItem;
  NW_Uint32 ignoreVal;
  NW_Status_t status;

  if ((strTable == NULL) || (string == NULL)){
    return NW_STAT_BAD_INPUT_PARAM;
  }

  newStrItem.data.storage = NULL;

  /* Also, there can be malformed document. cXML parser wants to limits
   * non DTD elements or attributes. So check current number of elements
   * in the parser. This is for security reasons.
   */

   if((NW_Uint32)strTable->vector->size > CXML_MAX_STRTBL_ITEMS)
   {
    return NW_STAT_STR_TBL_OVERFLOW;
   }

  /* Find if string is already there in the string table.  By design
   if string isn't found, then *extensionByteOffset has value equal to
   the offset of the new item to add which is just the count of all
   bytes already in the string table. */
  strItem = NW_Encoder_StringTable_find(strTable, string, &ignoreVal,
                                         extensionByteOffset);
  if (strItem != NULL) {
    strItem->frequency++;
    return NW_STAT_SUCCESS;
  }
  /* string not found, create a new entry. We need a duplicating the 
   * strings.
  */

  status = NW_Encoder_StringItem_initialize(&newStrItem, string);

  
  if (status != NW_STAT_SUCCESS){
    return status;
  }
  if (literalOrDocType){
    newStrItem.frequency = 2;
  }
  CXML_Vector_InsertAt(strTable->vector, &newStrItem,
                       CXML_Vector_AtEnd, NULL);

  
  return NW_STAT_SUCCESS;
}

/*
 * Finds a string in the list and returns the corresponding
 * StringItem, otherwise returns NULL
 */
/* ------------------------------------------------------------------------- */
NW_Encoder_StringItem_t*
NW_Encoder_StringTable_find(NW_Encoder_StringTable_t *strTable,
                            NW_String_t *string,
                            NW_Uint32 *strIndex,
                            NW_Uint32 *byteOffset)
{
  CXML_Vector_Metric_t index;

  if ((strTable == NULL) || (string == NULL)){
    return NULL;
  }
  *strIndex = 0;
  *byteOffset = 0;
  for (index = 0;
       index < strTable->vector->size;
       index++)
  {
    NW_Encoder_StringItem_t *item;

    item = (NW_Encoder_StringItem_t*)
            CXML_Vector_ElementAt(strTable->vector, index);
    if (NW_String_equals(string, &item->data))
    {
      *strIndex = index;
      return item;
    }
    *byteOffset = *byteOffset + NW_String_getByteCount(&item->data);
  }
  return NULL;
}

/**
 * Finds the string at a given index
 */
/* ------------------------------------------------------------------------- */
NW_Status_t
NW_Encoder_StringTable_get(NW_Encoder_StringTable_t *strTable,
                           NW_Uint32 index,
                           NW_String_t* string)
{
  NW_Encoder_StringItem_t *strItem;

  strItem = (NW_Encoder_StringItem_t*)
          CXML_Vector_ElementAt(strTable->vector,
                                     (CXML_Vector_Metric_t)index);

  if (strItem){
    string->storage = strItem->data.storage;
    string->length = strItem->data.length;
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_FAILURE;
}

/**
 * Finds item at a given index
 */
/* ------------------------------------------------------------------------- */
NW_Encoder_StringItem_t*
NW_Encoder_StringTable_getItemAt(NW_Encoder_StringTable_t *strTable,
                                 NW_Uint32 index)
{
  return  (NW_Encoder_StringItem_t*)
          CXML_Vector_ElementAt(strTable->vector,
                                     (CXML_Vector_Metric_t)index);
}

/**
 * Removes a string item
 */
/* ------------------------------------------------------------------------- */
NW_Status_t
NW_Encoder_StringTable_removeAt(NW_Encoder_StringTable_t *strTable,
                                CXML_Vector_Metric_t index)
{
  return CXML_Vector_RemoveAt(strTable->vector, (CXML_Vector_Metric_t)index);
}

/*
 * Returns the total number of bytes used for storing all the
 * strings in the list
 */
/* ------------------------------------------------------------------------- */
EXPORT_C NW_Uint32
NW_Encoder_StringTable_getTotalBytes(NW_Encoder_StringTable_t *strTable)
{
  NW_Uint32 byteLength;
  CXML_Vector_Metric_t index;

  byteLength = 0;

  for (index = 0;
       index < strTable->vector->size;
       index++)
 {
    NW_Encoder_StringItem_t *strItem;

    strItem = (NW_Encoder_StringItem_t*)
            CXML_Vector_ElementAt(strTable->vector, index);
    byteLength += strItem->data.length;
  }

  return byteLength;
}

/* ------------------------------------------------------------------------- */
EXPORT_C NW_Status_t
NW_Encoder_StringTable_createFromDOM(NW_Encoder_StringTable_t *strTable,
                                     NW_DOM_DocumentNode_t *doc,
                                     NW_WBXML_Dictionary_t *dictionary)
{

  /* make a table of strings in the document */
  NW_Encoder_StringTable_findDocStrings(strTable, doc, dictionary);

  /* The string table will be freed once at the end. So, don't do
   * this here. This is causing the memory leak otherwise.
   */
  return NW_STAT_SUCCESS;
}


/* callback implementations */
/* ------------------------------------------------------------------------- */
EXPORT_C NW_Status_t
NW_Encoder_StringTable_getStringTableOffset(void* pStringTableObject,
                                            NW_Uint32 byteCount,
                                            const NW_Uint8* pString,
                                            NW_Bool* pFound,
                                            NW_Uint32* pTableOffset)
{
  NW_Encoder_StringItem_t *strItem;
  NW_Uint32 strIndex;
  NW_String_t string;

  string.storage = (NW_Byte*)pString;
  string.length = byteCount;

  strItem = NW_Encoder_StringTable_find((NW_Encoder_StringTable_t*) pStringTableObject, 
                                        &string, &strIndex, pTableOffset);

  if (strItem == NULL)
      *pFound = NW_FALSE;
  else
      *pFound = NW_TRUE;

  return NW_STAT_SUCCESS;
}

/* ------------------------------------------------------------------------- */
EXPORT_C NW_Status_t
NW_Encoder_StringTable_addToStringTable(void* pStringTableObject,
                                        NW_Uint32 byteCount,
                                        const NW_Uint8* pString,
                                        NW_Uint32* pTableOffset)
{
  NW_String_t string;

  string.storage = (NW_Byte*)pString;
  string.length = byteCount;
  *pTableOffset = 0;
  return NW_Encoder_StringTable_append((NW_Encoder_StringTable_t*) pStringTableObject, 
                                       &string, NW_TRUE, pTableOffset);
}

/* ------------------------------------------------------------------------- */
EXPORT_C NW_Status_t
NW_Encoder_StringTable_StringTableIterateInit(void* pStringTableObject,
                                              void* pStringTableIterator)
{
  NW_Encoder_StringTableIterator_t *strIterator;

  strIterator = (NW_Encoder_StringTableIterator_t*)pStringTableIterator;
  strIterator->strTable = (NW_Encoder_StringTable_t*)pStringTableObject;
  strIterator->currentIndex = 0;
  return NW_STAT_SUCCESS;
}

/* at end of string table return byteCount = 0 */
/* ------------------------------------------------------------------------- */
EXPORT_C NW_Status_t
NW_Encoder_StringTable_StringTableIterateNext(void* pStringTableIterator,
                                              NW_Uint32* pByteCount,
                                              NW_Uint8** ppBuf)
{
  NW_Encoder_StringTableIterator_t *strIterator;
  CXML_Vector_Metric_t size;
  NW_String_t string;
  NW_Status_t status;

  strIterator = (NW_Encoder_StringTableIterator_t*)pStringTableIterator;
  size = NW_Encoder_StringTable_GetSize(strIterator->strTable);

  if (strIterator->currentIndex >= size){
    return NW_STAT_WBXML_ITERATE_DONE;
  }
  status = NW_Encoder_StringTable_get(strIterator->strTable, strIterator->currentIndex, &string);
  if (status != NW_STAT_SUCCESS){
    return status;
  }
  *pByteCount = string.length;
  *ppBuf = string.storage;
  strIterator->currentIndex++;
  return NW_STAT_WBXML_ITERATE_MORE;
}

