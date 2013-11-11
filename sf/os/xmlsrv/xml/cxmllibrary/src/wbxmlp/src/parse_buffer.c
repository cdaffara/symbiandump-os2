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


/*****************************************************************
**  File: wbxml_parse_buffer.c
**  Description:
*
* A buffer-based implementation of the "parser_reader interface".
* To create a parser that gets wbxml from some other source (such
* as reading from a stream), these are the only functions that should
* have to be ported. wbxml_parse.c contains implementation of the
* parsing logic, independent of how wbxml is read/stored (with a few
* small exceptions).
*
* The parser reader is responsible for safely getting various primitive
* data types and in-line strings from the wbxml NW_Byte code. The reader
* also returns a pointer to the string table (which in this case is
* just a pointer into the wbxml buffer.)
*
*****************************************************************/

#include "cxml_internal.h"
#include <xml/cxml/nw_wbxml_reader.h>
#include <xml/cxml/nw_wbxml_dictionary.h>
#include <xml/cxml/nw_wbxml_event.h>
#include <xml/cxml/nw_wbxml_token.h>
#include <xml/cxml/nw_wbxml_opaque.h>
#include "nw_wbxml_parsei.h"
#include <xml/cxml/nw_string_char.h>
#include <xml/cxml/nw_tinydom.h>

/*
 * Initialize the parser
 */

EXPORT_C void
NW_WBXML_Parser_newInPlace (NW_WBXML_Parser_t * parser)
{
  NW_ASSERT(parser != NULL);
  
  parser->handler = NULL;
  parser->flags = 0;
  parser->state = NW_WBXML_PARSER_S_START;
  parser->tag_code_page = 0;
  parser->attribute_code_page = 0;
  parser->cp_registry.storage = NULL;
  parser->cp_registry.current = 0;
  parser->cp_registry.count = 0;
  parser->dictionary = 0;
  parser->doc = NULL;
  parser->p = NULL;
  parser->offset = 0;
  parser->left = 0;
  parser->status = NW_STAT_SUCCESS;
  parser->recursiveCallCnt = 0;
  parser->ext_t_not_table_index = 0;
}


void
NW_WBXML_Parser_reset (NW_WBXML_Parser_t * parser)
{
  NW_ASSERT(parser != NULL);

  parser->handler = NULL;
  parser->state = NW_WBXML_PARSER_S_START;
  parser->tag_code_page = 0;
  parser->attribute_code_page = 0;
  parser->p = NULL;
  parser->offset = 0;
  parser->left = 0;
  parser->status = NW_STAT_SUCCESS;
  parser->recursiveCallCnt = 0;
}

void
NW_WBXML_Parser_delete (NW_WBXML_Parser_t * parser)
{
  if (parser->cp_registry.storage != NULL) {
    NW_Mem_Free(parser->cp_registry.storage);
  }
}


/*
* Safely advance the parser through bytecode.
* The functions which read values from the parser don't advance
* the parser since this may not be necessary or safe (e.g. when reading
* the last data item.).
*
* "Count" may equal "left" so zero is a possible "left" value.
*/

NW_Int32
NW_WBXML_Parser_advance (NW_WBXML_Parser_t * parser,
                         NW_Int32 count)
{
  NW_ASSERT(parser != NULL);

  if ((count > (NW_Int32) parser->left)
      || (((NW_Int32)parser->offset + count) < 0)){
    return -1;
  }
  parser->p += count;
  parser->offset = (NW_Uint32) ((NW_Int32)parser->offset +  count);
  parser->left = (NW_Uint32) ((NW_Int32) parser->left - count);
  return count;
}

/* Get the current offset */

NW_Uint32
NW_WBXML_Parser_getOffset(NW_WBXML_Parser_t * parser)
{
  NW_ASSERT(parser != NULL);

  return parser->offset;
}

/* Safely set the parser to a new offset */

NW_Int32
NW_WBXML_Parser_setOffset(NW_WBXML_Parser_t * parser,
                          NW_Uint32 offset)
{
  NW_Int32 count =
    (NW_Int32)offset - (NW_Int32)NW_WBXML_Parser_getOffset(parser);

  NW_ASSERT(parser != NULL);

  /* If the code page registry exists and is inited, reset code pages */
  if((parser->cp_registry.count > 0)
    && ((parser->flags & NW_WBXML_REGISTRY_INIT) == NW_WBXML_REGISTRY_INIT)){
    NW_WBXML_CPRegistry_getCodePages(&(parser->cp_registry),
                                     offset,
                                     &(parser->tag_code_page),
                                     &(parser->attribute_code_page));
  }

  /*
   * Try to "advance" the parser to the new position. This will fail
   * if the offset is not in the parser's current buffer.
   */

  return NW_WBXML_Parser_advance(parser, count);
}


/* Check if there is more NW_Byte code to read */

NW_Int32
NW_WBXML_Parser_hasMoreBytecode (NW_WBXML_Parser_t * parser)
{
  NW_ASSERT(parser != NULL);

  return (parser->left > 0);
}


/*
 * Read one unsigned NW_Byte from the current parser position.
 */

NW_Uint8
NW_WBXML_Parser_readUint8 (NW_WBXML_Parser_t * parser)
{
  NW_ASSERT(parser != NULL);

  return (NW_Uint8) (*(parser->p));
}


/*
 * Safely read an unsigned_32 from the current parser position
 */

NW_Int32
NW_WBXML_Parser_readMbUint32 (NW_WBXML_Parser_t * parser,
                              NW_Uint32 * val)
{
  NW_Uint32 i, space, next;

  NW_ASSERT(parser != NULL);

  space = parser->left;

  for (i = 0, *val = 0;; i++, *val <<= 7)
  {
    if (i > 4 || i == space)
    {
      return -1;
    }
    next = (parser->p)[i];
    if (!(next & 0x80))
    {
      *val |= next;
      break;
    }
    else
      *val |= (next &= 0x7F);
  }
  return (NW_Int32)(i + 1);
}

/*
 * Safely get the length of an inline string at current parser position
 */

NW_Int32 NW_WBXML_Parser_getInlineStrLen(NW_WBXML_Parser_t *parser,
                                         NW_WBXML_Document_t *doc)
{
  NW_Uint32 len = 0;

  NW_ASSERT(parser != NULL);

  if (!NW_String_valid(parser->p, parser->left, doc->charset)) {
    return -1;
  }

  if(NW_String_charBuffGetLength (parser->p, doc->charset, &len) < 0){
    return -1;
  }
  return (NW_Int32)len;
}

/*
 * Safely read an inline string at the current parser position
 */

EXPORT_C NW_Status_t
NW_WBXML_Parser_getStringInline (NW_WBXML_Parser_t * parser,
                                 NW_WBXML_Document_t * doc,
                                 NW_String_t *string)
{
  NW_ASSERT(parser != NULL);
  NW_ASSERT(doc != NULL);
  NW_ASSERT(string != NULL);

  if (!NW_String_valid (parser->p, parser->left, doc->charset))
  {
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  if (NW_String_initialize (string, parser->p, doc->charset)
      != NW_STAT_SUCCESS){
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }
  return NW_STAT_SUCCESS;
}


/*
 * Safely get an opaque at the current parser position
 */

EXPORT_C NW_Status_t
NW_WBXML_Parser_getOpaque (NW_WBXML_Parser_t * parser,
                           NW_Uint32 length,
                           NW_WBXML_Opaque_t *opaque)
{
  NW_ASSERT(parser != NULL);
  NW_ASSERT(length != 0);
  NW_ASSERT(opaque != NULL);

  if (length > parser->left){
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  return NW_WBXML_Opaque_construct (opaque, parser->p, length);
}

/*
 * Read in the string table
 */

NW_Status_t
NW_WBXML_Parser_readStringTable (NW_WBXML_Parser_t * parser,
                                 NW_WBXML_Document_t * doc,
                                 NW_Byte **table)
{
  NW_Int32 skip;

  NW_ASSERT(parser != NULL);
  NW_ASSERT(doc != NULL);
  NW_ASSERT(table != NULL);

  *table = NULL;

  /* Read the string table length into the document header */

  skip = NW_WBXML_Parser_readMbUint32 (parser, &(doc->strtbl.length));
  if (skip < 0) {
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  /* Try to advance over the length bytes to beginning of table */

  if (NW_WBXML_Parser_advance (parser, skip) < 0){
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  /* Set table to current parser location*/

  *table = parser->p;

  /* Try to advance to end of table */

  if (NW_WBXML_Parser_advance (parser, (NW_Int32) doc->strtbl.length) < 0){
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  return NW_STAT_SUCCESS;
}


/* Reset the parser to the start of a buffer. The buffsize is
 * critical here since it is used in all safety tests by the
 * parser reader to make sure the parser doesn't read past the
 * end of the buffer.
 */

NW_Status_t
NW_WBXML_Parser_resetBuffer(NW_WBXML_Parser_t *parser,
                            NW_Byte *buff,
                            NW_Uint32 buffsize)
{
  NW_ASSERT(parser != NULL);

  parser->offset = 0;
  parser->p = buff;
  if (buffsize == 0 || buff == NULL) {
    parser->left = 0;
  }
  else {
    parser->left = buffsize;
  }
  parser->tag_code_page = 0;
  parser->attribute_code_page = 0;
  parser->state = NW_WBXML_PARSER_S_HEADER;
  parser->recursiveCallCnt = 0;
  return NW_STAT_SUCCESS;
}


/*
 * This method sets the parser to a buffer which may contain only
 * wbxml fragments and no header. Used by writable DOM only.
 *
 */

NW_Status_t
NW_WBXML_Parser_setBuffer (NW_WBXML_Parser_t * parser,
                           NW_WBXML_Document_t * doc,
                           NW_Byte * buff,
                           NW_Uint32 buffsize)
{
  NW_ASSERT(parser != NULL);
  NW_ASSERT(doc != NULL);
  NW_ASSERT(buff != NULL);
  NW_ASSERT(buffsize > 0);

  /* Find dictionary from document */

  if (doc->publicid > 0){
    parser->dictionary =
      NW_WBXML_Dictionary_getIndexByPublicId (doc->publicid);
  }
  else if (doc->doc_type){
    parser->dictionary =
      NW_WBXML_Dictionary_getIndexByDocType (doc->doc_type, doc->charset);
  }
  else{
    NW_ASSERT(0); /* Doc not correctly initialized */
  }

  /* Set the buffer */

  NW_WBXML_Parser_resetBuffer(parser, buff, buffsize);
  return NW_STAT_SUCCESS;
}

/*
 * Parse a document from a buffer
 */

EXPORT_C NW_Status_t
NW_WBXML_Parser_parseBuffer (NW_WBXML_Parser_t * parser,
                             NW_WBXML_Document_t * doc,
                             NW_Byte * buff,
                             NW_Uint32 buffsize)
{
  NW_Status_t status;
  
  NW_ASSERT(parser != NULL);
  NW_ASSERT(doc != NULL);
  NW_ASSERT(buff != NULL);
  NW_ASSERT(buffsize > 0);

  if (parser->p == NULL)
  {
    parser->p = buff;
    parser->left = buffsize;
    
    /* First parser the document header */
    
    if ((status = NW_WBXML_Parser_docHeaderParse (parser, doc))
      != NW_STAT_SUCCESS)
      return status;
    
    /* Now get the dictionary from the document */
    
    if (doc->publicid > 0){
      parser->dictionary =
        NW_WBXML_Dictionary_getIndexByPublicId (doc->publicid);
    }
    
    else if (doc->doc_type){
      parser->dictionary =
        NW_WBXML_Dictionary_getIndexByDocType (doc->doc_type, doc->charset);
    }
    
    /* If a dictionary could not be attained try using the default public id */
    if (parser->dictionary == 0){
      doc->publicid = doc->default_public_id;
      parser->dictionary =
        NW_WBXML_Dictionary_getIndexByPublicId (doc->publicid);
    }

    /* Make the StartDocument callback */
    if (parser->handler && parser->handler->StartDocument_CB)
    {
      status = (*(parser->handler->StartDocument_CB)) (parser, doc,
        parser->context);
      if (status != NW_STAT_SUCCESS)
        return status;
    }
  }

  /* Parse the document body */

  return NW_WBXML_Parser_bodyParse (parser);
}
