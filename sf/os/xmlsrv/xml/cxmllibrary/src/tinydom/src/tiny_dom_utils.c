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
#include <xml/cxml/nw_tinydom_utils.h>
#include <xml/cxml/nw_dom_document.h>


/****************************************************************************
 * Name:          NW_TinyDom_ParserInitialize
 * Description:   Helper function to initialize the parser
 *                
 * Parameters:    p -- pointer to a Parser_t struct
 *                dictionaries -- pointer to the WBXML dictionaries
 *                dictionaryCount -- number of dictionaries 
 *                
 * Algorithm:     This is just a wrapper to conceal unnecessary initialization details.
 * Return value:  NW_STAT_SUCCESS, NW_STAT_FAILURE, NW_STAT_OUT_OF_MEMORY
 ***************************************************************************/
EXPORT_C NW_Status_t NW_TinyDom_ParserInitialize(Parser_t *p, 
                                        NW_WBXML_Dictionary_t * dictionaries[],
                                        NW_Int32 dictionaryCount,
                                        NW_Uint32 default_public_id)
{
  NW_Status_t           status;
  
  NW_ASSERT(p != NULL);

  (void) NW_Mem_memset(p, 0, sizeof(*p));

  /* Initialize the dictionary to use */
  if (dictionaries){
    status = NW_WBXML_Dictionary_add( dictionaryCount, dictionaries );

    if (status == NW_STAT_FAILURE) {

      status = NW_WBXML_Dictionary_initialize (dictionaryCount, dictionaries);
      if (status != NW_STAT_SUCCESS) {
        return status;
      }
    }
  }

  /* create the WBXML Parser */
  NW_WBXML_Parser_newInPlace (&(p->wbxmlParser));
  status = NW_WBXML_Document_construct(&(p->document), default_public_id);

  /* This ASSERT removed for "out of memory" testing using CXML Testkit component */
//  NW_ASSERT(status == NW_STAT_SUCCESS);
  
  if (status == NW_STAT_SUCCESS){
    /* initialize the TinyDOM stuff */
    NW_TinyDom_Tree_construct (&(p->tinyDOMTree), &(p->wbxmlParser), 
                               &(p->document), &(p->writer));
    NW_TinyDom_Parser_construct (&(p->tinyParser), &(p->tinyDOMTree));
  }
  return status;
}

/****************************************************************************
 * Name:          NW_TinyDom_MakeDOMTree
 * Description:   Helper function to parse the buffer, creating a tree
 *                
 * Parameters:    p       -- pointer to a Parser_t struct
 *                buffer  -- pointer to content
 *                length  -- number of bytes in buffer
 *                root    -- pointer to document root node
 * Algorithm:     This is just a wrapper to conceal unnecessary parsing details.
 * Return value:  NW_STAT_SUCCESS, NW_STAT_FAILURE, NW_STAT_OUT_OF_MEMORY
 ***************************************************************************/
EXPORT_C NW_Status_t
NW_TinyDom_MakeDOMTree (Parser_t *p, 
                        NW_Byte *buffer,
                        NW_Uint32 length, 
                        NW_DOM_DocumentNode_t **root)
{
 
  NW_Status_t status;

  NW_ASSERT(p != NULL);
 
  /* build the tinyDOM tree */
  status = NW_TinyDom_Parser_buildTree (&(p->tinyParser), (char *)buffer, length, NW_FALSE);
  if ((status == NW_STAT_SUCCESS) && (root != NULL)) {
    *root = p->tinyDOMTree.root_node;
  }
  return status;
}

EXPORT_C NW_Status_t
NW_TinyDom_AppendDOMTree (Parser_t *p, 
                        NW_Byte *buffer,
                        NW_Uint32 length, 
                        NW_Uint32 cp_count,
                        NW_Int32 lastValid,
                        NW_DOM_DocumentNode_t **root)
{
 
  NW_Status_t status;

  NW_ASSERT(p != NULL);
 
  /* build the tinyDOM tree */
  //p->wbxmlParser.offset = p->wbxmlParser.lastValid;
  p->wbxmlParser.offset = NW_TinyDom_getLastValid(p->tinyParser.dom_tree);
  p->tinyParser.cp_count = cp_count;
  //p->wbxmlParser.lastValid = lastValid;
  status = NW_TinyDom_Parser_incrementalBuildTree (&(p->tinyParser), (char *)buffer, length, NW_FALSE, lastValid);
  if ((status == NW_STAT_SUCCESS) && (root != NULL)) {
    *root = p->tinyDOMTree.root_node;
  }
  return status;
}

/****************************************************************************
 * Name:          NW_TinyDom_ParserDelete
 * Description:   Free the internal memory in allocated in NW_TinyDom_MakeDOMTree:
 *                
 * Parameters:    p       -- pointer to a Parser_t struct
 *                
 * Algorithm:     
 * Return value:  None
 ***************************************************************************/
EXPORT_C void NW_TinyDom_ParserDelete(Parser_t *p)
{
  if (p != NULL) {
    NW_WBXML_Parser_delete(&(p->wbxmlParser));
    NW_TinyDom_Tree_destruct(&(p->tinyDOMTree));
    NW_WBXML_Document_destruct(&(p->document));
    (void) NW_Mem_memset(p, 0, sizeof(*p));
  }
}
