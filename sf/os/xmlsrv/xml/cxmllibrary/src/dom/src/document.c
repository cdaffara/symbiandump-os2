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
#include <xml/cxml/nw_tinytree.h>
#include <xml/cxml/nw_encoder_wbxmlwriter.h>
#include <xml/cxml/nw_xmlp_xmlp2wbxml.h>
#include <xml/cxml/nw_tinydom_utils.h>

/*#include "nw_parser.h"*/

/************************
* DOCUMENT - NODE Methods
*************************/

EXPORT_C void
NW_DOM_DocumentNode_Delete(NW_DOM_DocumentNode_t* docNode){
  NW_WBXML_Parser_t *wbxmlParser;
  NW_TinyDom_Tree_t *tinyDomTree;
  NW_WBXML_Document_t *document;
  NW_TinyTree_t *tinyTree;

if (docNode == NULL) {
    return;
  }

  tinyTree = NW_TinyTree_Node_findTree(docNode);
  
  if(tinyTree != NULL){
    document = NW_TinyDom_getDocHeader(tinyTree);
    tinyDomTree = NW_TinyDom_getTree(tinyTree);
    wbxmlParser = tinyDomTree->parser;
    if(wbxmlParser != NULL){
      NW_WBXML_Parser_delete(wbxmlParser);
    }
    if(tinyDomTree != NULL){
      NW_TinyDom_Tree_destruct(tinyDomTree);
    }
    if(document != NULL){
      NW_WBXML_Document_destruct(document);
    }
  }
}
  /* ???? Does the writer need any cleanup ? */


static void
NW_TinyDom_Handle_Initialize(NW_TinyDom_Handle_t *h, 
                             NW_Uint32 default_public_id,
                             NW_Bool extTNotStringTable)
{  
  NW_ASSERT(h != NULL);

  (void) NW_Mem_memset(h, 0, sizeof(*h));

  NW_WBXML_Parser_newInPlace (&(h->wbxmlParser));
  h->wbxmlParser.ext_t_not_table_index =(NW_Uint8)((extTNotStringTable == NW_FALSE)?
                                          CXML_FALSE : CXML_TRUE);
  NW_WBXML_Document_construct(&(h->document), default_public_id);
  NW_TinyDom_Tree_construct (&(h->tinyDOMTree), 
                             &(h->wbxmlParser), 
                             &(h->document), 
                             &(h->writer));
  NW_TinyDom_Parser_construct (&(h->tinyParser), &(h->tinyDOMTree));
}


/* ************************************************************************/

/*
 * Create and populate a dom tree by parsing a buffer, returning the
 * document node.
 */

EXPORT_C
NW_DOM_DocumentNode_t * 
NW_DOM_DocumentNode_BuildWBXMLTree(NW_TinyDom_Handle_t *h, 
                                   NW_Byte *buffer,
                                   NW_Int32 length,
                                   NW_Bool freeBuff,
                                   NW_Bool extTNotStringTable)
{
  NW_Status_t status;

  NW_ASSERT(h != NULL);

  NW_TinyDom_Handle_Initialize(h, 0, extTNotStringTable);

  status = NW_TinyDom_Parser_buildTree (&(h->tinyParser), 
                                        (char *)buffer, 
                                        (NW_Uint32)length,
                                        freeBuff);
  if (status == NW_STAT_SUCCESS) {
    return  h->tinyDOMTree.root_node;
  }
  return NULL;
}

/*
 * This function is same at that of NW_DOM_DocumentNode_BuildWBXMLTree(..)
 * but returns the error code.
 */

static
NW_DOM_DocumentNode_t * 
CXML_DOM_DocumentNode_BuildWBXMLTree(NW_TinyDom_Handle_t *h, 
                                   NW_Byte *buffer,
                                   NW_Int32 length,
                                   NW_Bool freeBuff,
                                   NW_Bool extTNotStringTable,
                                   NW_Status_t* errorStatus)
{
  NW_Status_t status;

  NW_ASSERT(h != NULL);

  NW_TinyDom_Handle_Initialize(h, 0, extTNotStringTable);

  status = NW_TinyDom_Parser_buildTree (&(h->tinyParser), 
                                        (char *)buffer, 
                                        (NW_Uint32)length,
                                        freeBuff);
  if (status == NW_STAT_SUCCESS) 
  {
    *errorStatus = status;
    return  h->tinyDOMTree.root_node;
  }
  *errorStatus = status;
  return NULL;
}


/* Function to copy the string table extension values from the 
 * WBXML encoder to WBXML parser
 */

static NW_Status_t
NW_DOM_Write_WBXML_StrTbl(NW_TinyDom_Handle_t *p, void* WBXMLEncStrTbl)
{
     NW_Status_t status = NW_STAT_SUCCESS;
     NW_Status_t s = NW_STAT_WBXML_ITERATE_DONE;
     NW_Uint32 stringTableByteCount = 0;
     NW_Encoder_StringTableIterator_t strTableIterator;
     NW_Uint8* pLiteralBuf;
     NW_String_t pLiteralStr;
     NW_Uint32 docTableIndex;
     NW_Encoder_StringTable_t* strTable = NULL;
     NW_WBXML_Document_t* doc = &(p->document);
         
     strTable = (NW_Encoder_StringTable_t*) WBXMLEncStrTbl;

     stringTableByteCount = NW_Encoder_StringTable_getTotalBytes(strTable);
     
     
     if(stringTableByteCount > 0)
     {
        /*The following function always return success so why bother to check? */

        NW_Encoder_StringTable_StringTableIterateInit(WBXMLEncStrTbl,&strTableIterator);


        s =  NW_Encoder_StringTable_StringTableIterateNext
                    (&strTableIterator,&stringTableByteCount,&pLiteralBuf);

        while(s == NW_STAT_WBXML_ITERATE_MORE)
        {

         pLiteralStr.length = stringTableByteCount;
         pLiteralStr.storage = pLiteralBuf;

         status =  NW_WBXML_Document_putTableString(doc,&pLiteralStr,&docTableIndex) ;

         if(status != NW_STAT_SUCCESS)
         {
          break;
         }

           s =  NW_Encoder_StringTable_StringTableIterateNext
                    (&strTableIterator,&stringTableByteCount,&pLiteralBuf) ;

        }/*end while */

     }/*end if(stringTableByteCount > 0)*/

     //Delete the tree

     NW_Encoder_StringTable_delete(WBXMLEncStrTbl);
     WBXMLEncStrTbl = NULL;
 return status;
}/*end NW_DOM_Write_WBXML_StrTbl()*/

static
NW_DOM_DocumentNode_t*
NW_DOM_DocumentNode_BuildXMLTree(NW_TinyDom_Handle_t *h, 
                                 NW_Byte *buffer,
                                 NW_Int32 length,
                                 NW_Uint32 publicID,
                                 NW_Bool extTNotStringTable)
{

  NW_Buffer_t inBuff;
  NW_Buffer_t *outBuff = NULL;
  NW_Uint32 encoding;
  NW_Endianness_t endianness;
  NW_DOM_DocumentNode_t* docNode;
  NW_Status_t status;
  NW_Uint32 line;
  void* WBXMLEncStrTbl;

  inBuff.length = length;
  inBuff.data = buffer;

  status = NW_XML_ComputeEncoding(length, 
                                  buffer,
                                  &encoding, 
                                  &endianness);
  if(status != NW_STAT_SUCCESS){
    return NULL;
  }

/*
 * If applications want to use HTML Parser they can call the exported
 * NW_HTMLP_HtmlToWbxml() and then call NW_DOM_DocumentNode_BuildWBXMLTree()
 */
  status = NW_XML_XmlToWbxml(&inBuff, encoding, &outBuff, &line, publicID,&WBXMLEncStrTbl); 

 

  if(status != NW_STAT_SUCCESS){
    if(outBuff != NULL){
      NW_Mem_Free(outBuff);
    }

    return NULL;
  }
  docNode = NW_DOM_DocumentNode_BuildWBXMLTree(h, 
                                              outBuff->data,
                                              outBuff->length,
                                              NW_TRUE,
                                              extTNotStringTable);

  /* Write extension string table related information to the WBXML encoder. */
  
  if( docNode && WBXMLEncStrTbl)
    {
     status = NW_DOM_Write_WBXML_StrTbl(h,WBXMLEncStrTbl);
     if(status != NW_STAT_SUCCESS)
     {
       if(outBuff != NULL)
       {
         NW_Mem_Free(outBuff);
       }
     }
    }/* end if(WBXMLEncStrTbl)*/
 
  NW_Mem_Free(outBuff);
  return docNode;
}

static
NW_DOM_DocumentNode_t*
CXML_DOM_DocumentNode_BuildXMLTree(NW_TinyDom_Handle_t *h, 
                                 NW_Byte *buffer,
                                 NW_Int32 length,
                                 NW_Uint32 publicID,
                                 NW_Bool extTNotStringTable,
                                 NW_Status_t* errorStatus)
{

  NW_Buffer_t inBuff;
  NW_Buffer_t *outBuff = NULL;
  NW_Uint32 encoding;
  NW_Endianness_t endianness;
  NW_DOM_DocumentNode_t* docNode;
  NW_Status_t status;
  NW_Uint32 line;
  void* WBXMLEncStrTbl;

  inBuff.length = length;
  inBuff.data = buffer;

  status = NW_XML_ComputeEncoding(length, 
                                  buffer,
                                  &encoding, 
                                  &endianness);
  if(status != NW_STAT_SUCCESS)
  {
    *errorStatus = status;
    return NULL;
  }

/*
 * If applications want to use HTML Parser they can call the exported
 * NW_HTMLP_HtmlToWbxml() and then call NW_DOM_DocumentNode_BuildWBXMLTree()
 */
  status = NW_XML_XmlToWbxml(&inBuff, encoding, &outBuff, &line, publicID,&WBXMLEncStrTbl); 

 

  if(status != NW_STAT_SUCCESS){
    if(outBuff != NULL){
      NW_Mem_Free(outBuff);
    }
    *errorStatus = status;
    return NULL;
  }

  docNode = CXML_DOM_DocumentNode_BuildWBXMLTree(h, 
                                              outBuff->data,
                                              outBuff->length,
                                              NW_TRUE,
                                              extTNotStringTable,
                                              errorStatus);

  /* Write extension string table related information to the WBXML encoder. */
  
  if( docNode && WBXMLEncStrTbl)
    {
     status = NW_DOM_Write_WBXML_StrTbl(h,WBXMLEncStrTbl);
     if(status != NW_STAT_SUCCESS)
     {
       if(outBuff != NULL)
       {
         NW_Mem_Free(outBuff);
       }
       *errorStatus = status;
     }
    }/* end if(WBXMLEncStrTbl)*/
 
  NW_Mem_Free(outBuff);
  return docNode;
}/*CXML_DOM_DocumentNode_BuildXMLTree() */

EXPORT_C NW_DOM_DocumentNode_t*
NW_DOM_DocumentNode_BuildTree(NW_TinyDom_Handle_t *h, 
                                   NW_Byte *buffer,
                                   NW_Int32 length,
                                   NW_Bool encoded,
                                   NW_Uint32 publicID,																	 
                                   NW_Bool extTNotStringTable)
{
  if(encoded == NW_TRUE){
    return  NW_DOM_DocumentNode_BuildWBXMLTree(h, 
                                               buffer,
                                               length,
                                               NW_FALSE,
                                               extTNotStringTable);
  }

  return NW_DOM_DocumentNode_BuildXMLTree(h,buffer, length, publicID, extTNotStringTable);
}




/*
 * Returns publicId of the document (WBXML Specific)
 * otherwise 0
 */

EXPORT_C NW_Uint32 
NW_DOM_DocumentNode_getPublicIdAsNumber(NW_DOM_DocumentNode_t *doc)
{
  NW_WBXML_Document_t *header;
  NW_TinyTree_t *tiny_tree;
  
  NW_ASSERT(doc != NULL);

  if (NW_DOM_Node_getNodeType(doc) != NW_DOM_DOCUMENT_NODE){
    return 0;
  }
 
  tiny_tree = NW_TinyTree_Node_findTree(doc);
  header = NW_TinyDom_getDocHeader(tiny_tree);
  NW_ASSERT(header != NULL);

  return header->publicid;
}

/*
 * Gets the publicid as string
 * Returns
 *    NW_STAT_DOM_NODE_TYPE_ERR
 *    NW_STAT_SUCCESS
 * MODIFIED - docType
 */

EXPORT_C NW_Status_t
NW_DOM_DocumentNode_getPublicId(NW_DOM_DocumentNode_t *doc, 
                                NW_String_t *docType)
{
  NW_WBXML_Document_t *header;
  NW_TinyTree_t *tiny_tree;

  NW_ASSERT(doc != NULL);
  NW_ASSERT(docType != NULL);

  if (NW_DOM_Node_getNodeType(doc) != NW_DOM_DOCUMENT_NODE)
    return NW_STAT_DOM_NODE_TYPE_ERR;

  tiny_tree = NW_TinyTree_Node_findTree(doc);
  header = NW_TinyDom_getDocHeader(tiny_tree);

  NW_ASSERT(header != NULL);
  NW_String_shallowCopy(docType, header->doc_type);

  return NW_STAT_SUCCESS;
}

/*
 * Returns the version of the document, 
 * 0 in case of invalid operation (e.g. NULL doc)
 */

EXPORT_C NW_Uint8 
NW_DOM_DocumentNode_getVersion(NW_DOM_DocumentNode_t *doc)
{
  NW_WBXML_Document_t *header;
  NW_TinyTree_t *tiny_tree;

  NW_ASSERT(doc != NULL); 

  if (NW_DOM_Node_getNodeType(doc) != NW_DOM_DOCUMENT_NODE){
    return 0;
  }

  tiny_tree = NW_TinyTree_Node_findTree(doc);
  header = NW_TinyDom_getDocHeader(tiny_tree);
  NW_ASSERT(header != NULL);
  
  return header->version;
  
}

/*
 * Returns the supported encoding of the document
 * or 0 for invalid document
 */

EXPORT_C NW_Uint32 
NW_DOM_DocumentNode_getCharacterEncoding(NW_DOM_DocumentNode_t *doc)
{
  NW_WBXML_Document_t *header;
  NW_TinyTree_t *tiny_tree;

  NW_ASSERT(doc != NULL); 

  if (NW_DOM_Node_getNodeType(doc) != NW_DOM_DOCUMENT_NODE)
    return 0;

  tiny_tree = NW_TinyTree_Node_findTree(doc);
  header = NW_TinyDom_getDocHeader(tiny_tree);
  NW_ASSERT(header != NULL);

  return header->charset;
}

/*
 * Returns child node that is the root of this document
 */

EXPORT_C NW_DOM_ElementNode_t * 
NW_DOM_DocumentNode_getDocumentElement(NW_DOM_DocumentNode_t *doc)
{
  NW_DOM_Node_t *docChild;
  NW_Uint16 type;
  
  NW_ASSERT(doc != NULL); 

  if (NW_DOM_Node_getNodeType(doc) != NW_DOM_DOCUMENT_NODE){
    return NULL;
  }

  /*
   * Get the first child of the document node
   * (can be a PI node)
   */

  docChild = NW_DOM_Node_getFirstChild(doc);

  type = NW_DOM_Node_getNodeType(docChild);    
  if (type == 0){
    return NULL;
  }

  while (type != NW_DOM_ELEMENT_NODE){
    docChild = NW_DOM_Node_getNextSibling(docChild);
    type = NW_DOM_Node_getNodeType(docChild);
  }

  return docChild;
}

/*
 * Gets the dictionary
 */

EXPORT_C NW_WBXML_Dictionary_t *
NW_DOM_DocumentNode_getDictionary(NW_DOM_DocumentNode_t *doc)
{
  NW_Uint32 publicid = 0;
  NW_Uint32 encoding;
  NW_WBXML_Dictionary_t *dictionary = NULL;
  NW_Status_t status;

  NW_ASSERT(doc != NULL);

  encoding = NW_DOM_DocumentNode_getCharacterEncoding(doc);
  publicid = NW_DOM_DocumentNode_getPublicIdAsNumber(doc);

  if (publicid != 0){
    dictionary = NW_WBXML_Dictionary_getByPublicId(publicid);
  }
  else{
    NW_String_t docType;

    status = NW_DOM_DocumentNode_getPublicId(doc, &docType);
    if (status != NW_STAT_SUCCESS){
      return NULL;
    }
    dictionary = NW_WBXML_Dictionary_getByDocType(&docType, encoding);
  }
  return dictionary;
}



NW_Status_t
NW_DOM_DocumentNode_getElementIteratorByTagName(NW_DOM_DocumentNode_t *doc, 
                                                NW_String_t* name,
                                                NW_DOM_NodeIterator_t *handle)
{
  NW_WBXML_Dictionary_t *dict = NULL;
  NW_String_UCS2Buff_t *buff = NULL;
  NW_Uint32 encoding;
  NW_Int16 token;

  NW_ASSERT(doc != NULL);
  NW_ASSERT(name != NULL);
  NW_ASSERT(handle != NULL);

  encoding = NW_DOM_DocumentNode_getCharacterEncoding(doc);
  dict = NW_DOM_DocumentNode_getDictionary(doc);
  buff = NW_String_stringToUCS2Buff(name, encoding);
  if((buff == NULL) || (dict == NULL)){
    NW_Mem_Free(buff);
    return NW_STAT_FAILURE;
  }
  token = NW_WBXML_Dictionary_getTagToken(dict, buff, NW_TRUE);
  NW_Mem_Free(buff);
  if (token < 0) {
    return NW_STAT_FAILURE;
  }
  else { 
  return NW_DOM_DocumentNode_getElementIteratorByTagToken(doc, 
                                                          (NW_Uint16)token, 
                                                          handle);
}
}

NW_Status_t
NW_DOM_DocumentNode_getElementIteratorByTagToken(NW_DOM_DocumentNode_t *doc, 
                                                 NW_Uint16 token,
                                                 NW_DOM_NodeIterator_t *handle)
{
  
  NW_DOM_ElementNode_t *elem;

  NW_ASSERT(doc != NULL);
  NW_ASSERT(handle != NULL);

  if (NW_DOM_Node_getNodeType(doc) != NW_DOM_DOCUMENT_NODE)
    return NW_STAT_DOM_NODE_TYPE_ERR;

  elem = NW_DOM_DocumentNode_getDocumentElement(doc);
  NW_ASSERT(elem != NULL);

  return NW_DOM_NodeIterator_initialize(handle, elem, token); 
}





/*
 * Returns a pointer to the created Element Node
 */
EXPORT_C NW_DOM_ElementNode_t *
NW_DOM_DocumentNode_createElementNode(
  NW_DOM_DocumentNode_t *doc, NW_String_t *name)
{
  NW_Uint32 encoding = NW_DOM_DocumentNode_getCharacterEncoding(doc);
  NW_String_UCS2Buff_t *ucs2Buff = NULL;
 NW_Uint8 *uint8Buf = NULL;
 NW_Int32 fqToken = 0;
  NW_WBXML_Dictionary_t *dictionary = NW_DOM_DocumentNode_getDictionary(doc);
  NW_TinyTree_t* tinyTree;
  NW_TinyDom_Tree_t *tree;
  
  if ((doc == NULL) || (name == NULL) || (dictionary == NULL))
  {
    return NULL;
  }

  ucs2Buff = NW_String_stringToUCS2Buff(name, encoding);
  if (ucs2Buff == NULL){
    return NULL;
  }
  uint8Buf = (NW_Uint8 *) CXML_Str_CvtToAscii((NW_Ucs2*) ucs2Buff);
  NW_Mem_Free(ucs2Buff);
  if(uint8Buf == NULL){
    return NULL;
  }
  tinyTree = NW_TinyTree_Node_findTree(doc);
  tree = NW_TinyDom_getTree(tinyTree);

  fqToken = (NW_Int32) NW_WBXML_Dictionary_getTagToken(dictionary, ucs2Buff, NW_TRUE);
  NW_Mem_Free(uint8Buf);
  if (fqToken == -1){
    return NW_TinyDom_createElementByName(tree, name);
  }
  
  return NW_TinyDom_createElementByToken(tree, (NW_Uint16)fqToken);  
}

/*
 * Returns a pointer to the created Element Node
 */
EXPORT_C NW_DOM_ElementNode_t * 
NW_DOM_DocumentNode_createElementNodeByToken(
  NW_DOM_DocumentNode_t *doc, NW_Uint16 token)
{
  NW_TinyTree_t* tinyTree;
  NW_TinyDom_Tree_t *tree;
  NW_WBXML_Dictionary_t *dictionary;
  
  dictionary = NW_DOM_DocumentNode_getDictionary(doc);
  if ((doc == NULL) || (dictionary == NULL) || (token == 0))
  {
    return NULL;
  }

  tinyTree = NW_TinyTree_Node_findTree(doc);
  tree = NW_TinyDom_getTree(tinyTree);

  return NW_TinyDom_createElementByToken(tree, (NW_Uint16)token);  
}


/*
 * Returns a pointer to the created Text Node
 */
EXPORT_C NW_DOM_TextNode_t * 
NW_DOM_DocumentNode_createTextNodeWithTextItem(
  NW_DOM_DocumentNode_t *doc, NW_DOM_TextItem_t *data)
{
  NW_TinyTree_t* tinyTree;
  NW_TinyDom_Tree_t *tree;

  if ((doc == NULL) || (data == NULL))
    return NULL;

  tinyTree = NW_TinyTree_Node_findTree(doc);
  tree = NW_TinyDom_getTree(tinyTree);

  return NW_TinyDom_createTextNode(tree, data);
}

/*
 * Returns a pointer to the created Text Node
 */
EXPORT_C NW_DOM_TextNode_t * 
NW_DOM_DocumentNode_createTextNode(
  NW_DOM_DocumentNode_t *doc, NW_String_t *data)
{
  NW_TinyTree_t* tinyTree;
  NW_TinyDom_Tree_t *tree;
  NW_DOM_TextItem_t item;

  if (NW_DOM_TextItem_initFromString(&item, data) != NW_STAT_SUCCESS)
    return NULL;

  if ((doc == NULL) || (data == NULL))
    return NULL;

  tinyTree = NW_TinyTree_Node_findTree(doc);
  tree = NW_TinyDom_getTree(tinyTree);

  return NW_TinyDom_createTextNode(tree, &item);
}

/*
 * Returns a pointer to the created Comment Node
 */
NW_DOM_CommentNode_t * 
NW_DOM_DocumentNode_createCommentNode(
  NW_DOM_DocumentNode_t *doc, NW_String_t *data)
{
  NW_DOM_TextNode_t *t = 
    NW_DOM_DocumentNode_createTextNode(doc, data);
  if (t == NULL)
    return NULL;
  NW_TinyTree_Node_setUserFlags(t, T_DOM_NODE_COMMENT);
  t->flags |= T_DOM_NODE_XML;
  return (NW_DOM_CommentNode_t *)t;
}


/*
 * Returns a pointer to the created CDataSection Node
 */
NW_DOM_CDATASectionNode_t * 
NW_DOM_DocumentNode_createCDATASectionNode(
  NW_DOM_DocumentNode_t *doc, NW_String_t *data)
{
  NW_DOM_TextNode_t *t = 
    NW_DOM_DocumentNode_createTextNode(doc, data);
  if (t == NULL)
    return NULL;
  NW_TinyTree_Node_setUserFlags(t, T_DOM_NODE_CDATASECTION);
  t->flags |= T_DOM_NODE_XML;
  return (NW_DOM_CommentNode_t *)t;
}


/*
 * Returns a pointer to the created ProcessingInstructionNode Node
 */
NW_DOM_ProcessingInstructionNode_t * 
NW_DOM_DocumentNode_createProcessingInstructionNodeByAttrVal(NW_DOM_DocumentNode_t* pDoc,
                                                             NW_String_t* pTargetString,
                                                             NW_DOM_AttrVal_t* pData)
{
  NW_TinyTree_Node_t* pPiNode;
  NW_TinyTree_Node_t* pAttrNode;
  NW_TinyTree_t* pTinyTree;
  NW_TinyDom_Tree_t* pTree;
  NW_WBXML_Dictionary_t* pDictionary;
  NW_Uint32 encoding;
  NW_Int32 fqToken;

  if ((pDoc == NULL) || (pTargetString == NULL) || (pData == NULL)) {
    return NULL;
  }
  pTinyTree = NW_TinyTree_Node_findTree(pDoc);
  pTree = NW_TinyDom_getTree(pTinyTree);

  pDictionary = NW_DOM_DocumentNode_getDictionary(pDoc);
  if (pDictionary == NULL) {
    return NULL;
  }
  encoding = NW_DOM_DocumentNode_getCharacterEncoding(pDoc);
  fqToken = NW_WBXML_Dictionary_getAttributeToken(pDictionary,
                                                  pTargetString,
                                                  encoding,
                                                  NW_TRUE /* name */);
  if (fqToken == -1)
  {
    pPiNode = NW_TinyTree_createNode(&(pTree->tree), 0);
    if (pPiNode == NULL) {
      return NULL;
    }
    NW_TinyTree_Node_setUserFlags(pPiNode, T_DOM_NODE_PI);

    pAttrNode = NW_TinyDom_createAttributeByName(pTree,
                                                 pTargetString,
                                                 pData);
    if (pAttrNode == NULL) {
      (void)NW_TinyTree_deleteNode(pPiNode);
      return NULL;
    }
    (void)NW_TinyTree_attachChild(pPiNode, pAttrNode);
    return pPiNode;
  }
  return NW_DOM_DocumentNode_createProcessingInstructionNodeByToken(pDoc,
                                                                    (NW_Uint16)fqToken,
                                                                    pData);
}

/*
 * Returns a pointer to the created ProcessingInstructionNode Node
 */

NW_DOM_ProcessingInstructionNode_t * 
NW_DOM_DocumentNode_createProcessingInstructionNodeByToken(
  NW_DOM_DocumentNode_t *pDoc, NW_Uint16 token, NW_DOM_AttrVal_t *pData)
{
  NW_TinyTree_Node_t* pPiNode;
  NW_TinyTree_Node_t* pAttrNode;
  NW_TinyTree_t* pTinyTree;
  NW_TinyDom_Tree_t* pTree;

  if ((pDoc == NULL) || (token == 0) || (pData == NULL))
    return NULL;

  pTinyTree = NW_TinyTree_Node_findTree(pDoc);
  pTree = NW_TinyDom_getTree(pTinyTree);
  pPiNode = NW_TinyTree_createNode(&(pTree->tree), 0);
  if (pPiNode == NULL) {
    return NULL;
  }
  NW_TinyTree_Node_setUserFlags(pPiNode, T_DOM_NODE_PI);

  pAttrNode = NW_TinyDom_createAttributeByToken(pTree,
                                                (NW_Uint8)token,
                                                pData);
  if (pAttrNode == NULL) {
    (void)NW_TinyTree_deleteNode(pPiNode);
    return NULL;
  }
  (void)NW_TinyTree_attachChild(pPiNode, pAttrNode);
  return pPiNode;
}

/*
 * Returns a pointer to the created ProcessingInstructionNode Node
 */
NW_DOM_ProcessingInstructionNode_t * 
NW_DOM_DocumentNode_createProcessingInstructionNode(
  NW_DOM_DocumentNode_t *doc, NW_String_t *target, NW_String_t *data)
{
  NW_DOM_AttrVal_t val;

  if ((doc == NULL) || (target == 0) || (data == NULL))
    return NULL;

  NW_DOM_AttrVal_initFromString(&val, data);
  return NW_DOM_DocumentNode_createProcessingInstructionNodeByAttrVal(doc, target, &val);  
}

/*
 * Returns a pointer to the created Document Node
 * - NOT SUPPRTED - ask Steve 
NW_DOM_DocumentNode_t * 
NW_DOM_DocumentNode_createDocument(
    NW_Uint8 version, NW_String_t *publicid, NW_Uint32 encoding)
{
  NW_WBXML_Parser_t *WBXMLParser;
  NW_TinyDom_Parser_t *tinyParser; 
  NW_TinyDom_Tree_t *domTree;
  NW_WBXML_Writer_t *writer;
  NW_Status_t status;

  WBXMLParser = NW_Mem_Malloc(sizeof(NW_WBXML_Parser_t));
  tinyParser = NW_Mem_Malloc(sizeof(NW_TinyDom_Parser_t));
  domTree = NW_Mem_Malloc(sizeof(NW_TinyDom_Tree_t));
  writer = NW_Mem_Malloc(sizeof(NW_WBXML_Writer_t));

  NW_WBXML_Parser_reset(WBXMLParser);
  domTree->doc->version = version;
  domTree->doc->doc_type = publicid;
  domTree->doc->charset = encoding;
  NW_TinyDom_Tree_construct(domTree, WBXMLParser, domTree->doc, writer);
  NW_TinyDom_Parser_construct(tinyParser, domTree);
  status = NW_TinyDom_Tree_create(domTree, tinyParser, domTree->doc, 
    WBXMLParser, writer, 2);
  if (status != NW_STAT_SUCCESS)
    return NULL;
  return NW_TinyDom_writeDocHeader(domTree, version, publicid, encoding);
}*/

/*
 * Create an empty dom tree, returning the document node.
 */

EXPORT_C NW_DOM_DocumentNode_t * 
NW_DOM_DocumentNode_createDocumentWithNumberPublicId(NW_TinyDom_Handle_t *h,
                                                     NW_Uint8 version,
                                                     NW_Uint32 publicid, 
                                                     NW_Uint32 encoding,
                                                     NW_Bool extTNotStringTable,
                                                     NW_Bool enableStringTable)
{
  NW_Status_t status = NW_STAT_SUCCESS;
  NW_TinyDom_Handle_Initialize(h, publicid, extTNotStringTable);

  h->document.charset = encoding;
  h->document.publicid = publicid;
  h->document.version = version;

  	 
  	 status = NW_TinyDom_Tree_create(&h->tinyDOMTree, &h->tinyParser, 
                                  &h->document, &h->wbxmlParser,
                                  &h->writer, 4, enableStringTable);
     
  

  if(status == NW_STAT_SUCCESS)
  {
  	return h->tinyDOMTree.root_node;
  }
  else
  {
  	
  	return NULL;
  }
}


/*
 * One of the API for Building the DOM tree.
 */

EXPORT_C NW_DOM_DocumentNode_t* 
CXML_DOM_DocumentNode_BuildTree(NW_TinyDom_Handle_t* h, 
                              NW_Byte* buffer,
                              NW_Int32 length,
                              NW_Bool encoded,
                              NW_Uint32 publicID,
                              NW_Bool extTNotStringTable,
                              NW_Status_t* errorStatus)
{
 *errorStatus = NW_STAT_SUCCESS;
 if(encoded == NW_TRUE)
 {
    return  CXML_DOM_DocumentNode_BuildWBXMLTree(h, 
                                                 buffer,
                                                 length,
                                                 NW_FALSE,
                                                 extTNotStringTable,
                                                 errorStatus);
  }

  return CXML_DOM_DocumentNode_BuildXMLTree(h,
                                            buffer, 
                                            length, 
                                            publicID, 
                                            extTNotStringTable,
                                            errorStatus);
 

}/*end CXML_DOM_DocumentNode_BuildTree()*/

