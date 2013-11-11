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
**  File: wbxml_parse_logic.c
**
**  Description:
*
* Here is the main parser logic. None of these functions has
* any knowlege of how wbxml is stored or read, so they should
* work as well with a buffer, stream or file based parser. These
* functions require some implementation of the "parser reader
* interface" as defined in wbxml_reader.h.
*
*****************************************************************/

#include "cxml_internal.h"
#include <xml/cxml/nw_wbxml_token.h>
#include <xml/cxml/nw_wbxml_dictionary.h>
#include <xml/cxml/nw_wbxml_document.h>
#include <xml/cxml/nw_wbxml_event.h>
#include <xml/cxml/nw_wbxml_reader.h>
#include "nw_wbxml_parsei.h"
#include <xml/cxml/nw_string_char.h>

#ifndef HTTP_utf_8
#define HTTP_utf_8 0x6A
#endif

#define TOKEN_STATE_TAG         0
#define TOKEN_STATE_ATTR        1

#define WBXML_MAX_RECURSIVE_CALL_DEPTH 120  /* For parse element only */

/* The parser tries to be very strict about errors caused by bad NW_Byte
 * code, lack of memory, etc. For example, the parser will not read
 * past the end of the buffer. Callbacks can also set error status to
 * stop parsing. These will be detected as soon as possible after the
 * callback returns.  The following method is called with the return
 * value (NW_Status_t) from the callback functions.  The status is
 * saved and if it is not NW_STAT_SUCCESS, then the parser's flags
 * field is updated so that the parser will know that a callback has
 * raised an exception/error condition. The parser will quit as soon
 * as it checks the flags.
 */

static void
NW_WBXML_Parser_setStatus (NW_WBXML_Parser_t * parser,
                           NW_Status_t status)
{
  parser->status = status;

  switch (status)
  {
  case NW_STAT_SUCCESS:
    break;
  case NW_STAT_OUT_OF_MEMORY:
    parser->flags |= NW_WBXML_PARSER_NOMEMORY;
    break;
  case NW_STAT_WBXML_ERROR_BYTECODE:
    parser->flags |= NW_WBXML_PARSER_BYTECODEERROR;
    break;
  case NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED:
    parser->flags |= NW_WBXML_PARSER_CHARSET_UNSUPPORTED;
    break;
  default:
    parser->flags |= NW_WBXML_PARSER_UNKNOWN_ERROR;
    break;
  }
  return;
}

/*
 * Convert the parser's flag status code to a NW_Status_t constant
 */

static NW_Status_t
NW_WBXML_Parser_flagToStatus (NW_WBXML_Parser_t * parser)
{
  if (parser->flags & NW_WBXML_PARSER_NOMEMORY)
    return NW_STAT_OUT_OF_MEMORY;

  if (parser->flags & NW_WBXML_PARSER_BYTECODEERROR)
    return NW_STAT_WBXML_ERROR_BYTECODE;

  if (parser->flags & NW_WBXML_PARSER_CHARSET_UNSUPPORTED)
    return NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED;

  return parser->status;
}


#define GET_STATE(parser) ((parser)->state & NW_WBXML_PARSER_S_MASK)
#define SET_STATE(parser, st) \
  ((parser)->state = (~NW_WBXML_PARSER_S_MASK & parser->state) | (st))

#define CHECK_PARSER_STATUS \
  if((parser->flags & NW_WBXML_PARSER_S_MASK) != NW_WBXML_PARSER_OK) \
    return NW_WBXML_Parser_flagToStatus(parser)


/*
* Create an uninitialized code page registry. As long as the registry is not
* created, it will be ignored. Once it has been created, the parser will check
* the REGISTRY_INIT flag. If this is not set, the registry will be initialized
* as the parser runs. If this is set, then the registry will be consulted when
* updating the offset.
*/

NW_Status_t
NW_WBXML_Parser_addCPRegistry(NW_WBXML_Parser_t* parser,
                              NW_WBXML_CP_Registry_Entry_t* storage,
                              NW_Int32 count)
{
  NW_ASSERT(parser != NULL);
  NW_ASSERT(storage != NULL);

  parser->cp_registry.storage = storage;
  parser->cp_registry.current = storage;
  parser->cp_registry.count = count;
  // WLIU_DEBUG: parser->cp_registry.realcount = 0;
  parser->state &= ~NW_WBXML_REGISTRY_INIT; /* Make sure flag is not set */
  return NW_STAT_SUCCESS;
}

static void
NW_WBXML_CPRegistry_addEntry(NW_WBXML_CP_Registry_t *registry ,
                             NW_Uint8 code_page,
                             NW_Uint32 offset, /* must point to switch_page token */
                             NW_Uint8 token_state)
{
  NW_ASSERT(registry != NULL);
  NW_ASSERT(registry->current < (registry->storage + registry->count));

  /* ignoring page type (tag or attribute) check that registry is in
  increasing order of offset so that the algorith in
  NW_WBXML_CPRegistry_getCodePages() works */
  NW_ASSERT((registry->current > registry->storage) ?
            (offset > (registry->current - 1)->start_offset)
            : 1);
  /* Each entry must record if it is for tokens or attributes.  A
  trick to encode this without extra memory relies on the use of two
  bytes in WBXML to encode a page switch.  With two bytes one has an
  even offset and one an odd offset.  We pick the even if it is a tag
  page and the odd offset if it is an attribute page. */
  if (token_state == TOKEN_STATE_TAG) {
    if (offset & 1) { /* if it is odd, advance one to make it even */
      offset++;
    }
  } else {
    NW_ASSERT(token_state == TOKEN_STATE_ATTR);
    if ((offset & 1) == 0) { /* if it is even, advance one to make it odd */
      offset++;
    }
  }
  registry->current->start_offset = offset;
  registry->current->code_page = code_page;

  //if(registry->current < (registry->storage + registry->count - 1)){
    registry->current++;
    // WLIU_DEBUG: registry->realcount++;
  //
}

void
NW_WBXML_CPRegistry_getCodePages(NW_WBXML_CP_Registry_t* registry,
                                 NW_Uint32 offset,
                                 NW_Uint8 *tag_code_page,
                                 NW_Uint8 *attribute_code_page)
{
  NW_WBXML_CP_Registry_Entry_t* entry;

  NW_ASSERT(registry != NULL);

  *tag_code_page = 0;
  *attribute_code_page = 0;
  for (entry = registry->storage;
       entry < (registry->storage + registry->count);
       entry++){
    /* assume that the entries are in increasing offset order */
    NW_ASSERT((entry > registry->storage) ?
              (entry->start_offset > (entry - 1)->start_offset)
              : 1);

    if(offset > entry->start_offset) {
      if(entry->start_offset & 1) {
        /* odd offset indicates attributes */
        *attribute_code_page = entry->code_page;
      } else {
        /* even offset indicates tokens */
        *tag_code_page = entry->code_page;
      }
    } else {
      break; /* early out */
    }
  }
}


/*
  This function parses "version publicid charset strtbl" which is a
  sequence that comprises the "header" of a WBXML document.

  From specification WAP-192-WBXML-20010725-a, Version 1.3, 25 July 2001,
  section 5.3 BNF for Document Structure:

  start = version publicid charset strtbl body
  version = u_int8 // WBXML version number
  publicid = mb_u_int32 | ( zero index )
  charset = mb_u_int32
  strtbl = length *byte
  length = mb_u_int32

  The "version" specifies the WBXML specification version. The
  version byte contains the major version minus one in the upper
  four bits and the minor version in the lower four bits. For
  example, the version number 1.3 would be encoded as 0x03.

  The first form of publicid is a multi-byte positive integer value,
  greater than zero, representing a well-known XML document type given
  an assigned value.  See table at section 7.2.1 or try
  http://www.wapforum.org/wina/wbxml-public-docid.htm
  Note that 0x01 is used for "unknown or missing public identifier."

  The second form of publicid begins with a 0 byte followed by a
  string table index (note that these "index" values are all 0-based
  byte offsets from the start of the string table).  The entry in the
  string table encodes the XML document type string.

  The charset is the multi-byte encoding of the IANA charset MIBenum
  http://www.iana.org/assignments/character-sets

  The string table either has length 0, if empty, or the length is the
  byte count of the string table block not counting the length byte
  itself.

  This is what the specification says about "index" as an index into
  the string table:

  "Various tokens encode references to the contents of the string
  table. These references are encoded as scalar byte offsets from the
  first byte of the first string in the string table. For example, the
  offset of the first string is zero (0)."

  So, "index" is a 0-based byte offset from the first byte of the
  string table.  The first byte of the string table is the one after
  the multi-byte encoding of the string table length.

  This function returns one of:

  NW_STAT_SUCCESS
  NW_STAT_OUT_OF_MEMORY
  NW_STAT_WBXML_ERROR_BYTECODE
*/
NW_Status_t
NW_WBXML_Parser_docHeaderParse(NW_WBXML_Parser_t* pParser,
                               NW_WBXML_Document_t* pDoc)
{
  NW_Int32 bytesRead;
  NW_Uint32 docTypeByteOffset = 0;
  NW_Status_t substatus;
  NW_Status_t status = NW_STAT_WBXML_ERROR_BYTECODE;

  /* this is an internal function where caller should guarantee these */
  NW_ASSERT(pParser != NULL);
  NW_ASSERT(pDoc != NULL);
  NW_ASSERT(GET_STATE(pParser) == NW_WBXML_PARSER_S_START);

  /* version */
  pDoc->version = NW_WBXML_Parser_readUint8(pParser);
  if (NW_WBXML_Parser_advance(pParser, 1) < 0) {
    goto errorExitHeaderParse;
  }
  /* publicid */
  bytesRead = NW_WBXML_Parser_readMbUint32(pParser, &(pDoc->publicid));
  if ((bytesRead < 0)
      || (NW_WBXML_Parser_advance(pParser, bytesRead) < 0)) {
    goto errorExitHeaderParse;
  }
  /* check for publicid second form referencing string table */
  if (pDoc->publicid == 0) {
    /* save doctype string table index for after string table is read */
    bytesRead = NW_WBXML_Parser_readMbUint32(pParser, &docTypeByteOffset);
    if ((bytesRead < 0)
        || (NW_WBXML_Parser_advance(pParser, bytesRead) < 0)) {
      goto errorExitHeaderParse;
    }
  }

 if(pDoc->publicid == 1) //As per WBXML spec, invalid public identifier
  {
    return NW_STAT_WMLBROWSER_BAD_CONTENT;
  }

  /* charset */
  if (pDoc->version != 0) {
    bytesRead = NW_WBXML_Parser_readMbUint32(pParser, &(pDoc->charset));
    if ((bytesRead < 0)
        || (NW_WBXML_Parser_advance(pParser, bytesRead) < 0)) {
      goto errorExitHeaderParse;
    }
  } else {
    /* For WBXML version 1.0, which did not have a charset field in
       the document header, we hardwire the charset to UTF-8.  This is
       against the spec which says that the charset should come as
       side info with the doc (e.g., get charset from the load
       response), but the API needs to change to support this so
       rather than reject all version 1.0 docs we accept the UTF-8
       (includes ASCII but not Latin-1) encoded ones. */
    pDoc->charset = HTTP_utf_8;
  }
  substatus = NW_String_charsetValid(pDoc->charset);
  if (substatus != NW_STAT_SUCCESS) {
    status = substatus;
    goto errorExitHeaderParse;
  }
  /* strtbl */
  substatus = NW_WBXML_Parser_readStringTable(pParser, pDoc,
                                              &(pDoc->strtbl.data));
  if (substatus != NW_STAT_SUCCESS) {
    status = substatus;
    goto errorExitHeaderParse;
  }

  /* record remainder of doc as the body length */
  pDoc->body_len = pParser->left;

  /* deal with doctype string, maybe from string table */

  if(pDoc->doc_type)
  {
   NW_String_delete(pDoc->doc_type);
  }

  pDoc->doc_type = NULL;
  if (pDoc->publicid == 0) {
    /* now that we have the string table, get the doctype string */
    if ((docTypeByteOffset >= pDoc->strtbl.length)
        || (!NW_String_valid(pDoc->strtbl.data + docTypeByteOffset,
                             pDoc->strtbl.length - docTypeByteOffset,
                             pDoc->charset))) {
      goto errorExitHeaderParse;
    }
    pDoc->doc_type = NW_String_new();
    if (pDoc->doc_type != NULL) {
      if (NW_String_initialize(pDoc->doc_type,
                               pDoc->strtbl.data + docTypeByteOffset,
                               pDoc->charset) != NW_STAT_SUCCESS) {
        NW_String_delete(pDoc->doc_type);
        pDoc->doc_type = NULL;
        goto errorExitHeaderParse;
      }
      /* TBD should now go back and try to fill in the internal "publicId"
         from the WBXML dictionary to match the docType. */
    } else {
      status = NW_STAT_OUT_OF_MEMORY;
      goto errorExitHeaderParse;
    }
  } else {
    /* doc came with publicid so now fill in the type string. */
    substatus
      = NW_WBXML_Dictionary_publicId_to_doctypeString(pDoc->publicid,
                                                      &(pDoc->doc_type));
    /* ignore any error other than OOM */
    if (substatus == NW_STAT_OUT_OF_MEMORY) {
      status = NW_STAT_OUT_OF_MEMORY;
      goto errorExitHeaderParse;
    }
  }
  SET_STATE(pParser, NW_WBXML_PARSER_S_HEADER);
  pParser->doc = pDoc;
  status = NW_STAT_SUCCESS;
 errorExitHeaderParse:
  return status;
}

/*
 * Save/restore context can be used if the parser is to be invoked
 * reentrantly (in a callback).  Normally this should be avoided since
 * the only really safe way to do this is to save the whole parser
 * object on the stack. This uses a lot of space!
 */

NW_Status_t
NW_WBXML_Parser_saveContext(NW_WBXML_Parser_t *parser,
                            NW_WBXML_Parser_t *context)
{
  NW_ASSERT(parser != NULL);
  NW_ASSERT(context != NULL);

  NW_Mem_memcpy(context, parser, sizeof(*parser));
  return NW_STAT_SUCCESS;
}

NW_Status_t
NW_WBXML_Parser_restoreContext(NW_WBXML_Parser_t *parser,
                               NW_WBXML_Parser_t *context)
{
  NW_ASSERT(parser != NULL);
  NW_ASSERT(context != NULL);

  NW_Mem_memcpy(parser, context, sizeof(*parser));
  return NW_STAT_SUCCESS;
}

NW_Status_t
NW_WBXML_Parser_setTagCodepage(NW_WBXML_Parser_t *parser,
                               NW_Uint8 page)
{
  NW_ASSERT(parser != NULL);

  parser->tag_code_page = page;
  return NW_STAT_SUCCESS;
}

NW_Uint8
NW_WBXML_Parser_getTagCodepage(NW_WBXML_Parser_t *parser)
{
  NW_ASSERT(parser != NULL);

  return parser->tag_code_page;
}

NW_Status_t
NW_WBXML_Parser_setAttrCodepage(NW_WBXML_Parser_t *parser,
                                NW_Uint8 page)
{
  NW_ASSERT(parser != NULL);

  parser->attribute_code_page = page;
  return NW_STAT_SUCCESS;
}

NW_Uint8
NW_WBXML_Parser_getAttrCodepage(NW_WBXML_Parser_t *parser)
{
  NW_ASSERT(parser != NULL);

  return parser->attribute_code_page;
}

/*
* Turn an eight bit token into a fully qualified 32 bit token depending
* on the parser state.
*/

static
NW_Uint32
FqToken (NW_WBXML_Parser_t * parser,
         NW_Uint8 token,
         NW_Int32 token_state)
{
  NW_Uint32 fq_token = token;

  NW_ASSERT(parser != NULL);

  fq_token |= (parser->dictionary << 16);
  if (token_state == TOKEN_STATE_ATTR) {
    fq_token |=
      ((parser->attribute_code_page << 8) | NW_WBXML_CP_STATE_ATTR);
  } else {
    fq_token |= ((parser->tag_code_page << 8) | NW_WBXML_CP_STATE_TAG);
  }
  return fq_token;
}


/* Invoke callbacks */

/* The callback wrappers use the parser flags to implement an
 * "exception" mechanism. If a callback returns a non-OK status, the
 * parser will detect this as soon as possible and stop what
 * it is doing.
 */

/* Handle state callbacks */

static
void
Handle_Pi(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Pi_CB)){
    status = (*(parser->handler->Pi_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Pi_End(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Pi_End_CB)){
    status = (*(parser->handler->Pi_End_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Tag_Start(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Tag_Start_CB)){
    status = (*(parser->handler->Tag_Start_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Tag_End(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Tag_End_CB)){
    status = (*(parser->handler->Tag_End_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Attr_Start(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Attr_Start_CB)){
    status = (*(parser->handler->Attr_Start_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Attr_Val(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Attr_Val_CB)){
    status = (*(parser->handler->Attr_Val_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Content(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Content_CB)){
    status = (*(parser->handler->Content_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Codepage(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->CodePage_CB)){
    status = (*(parser->handler->CodePage_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Extension(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Extension_CB)){
    status = (*(parser->handler->Extension_CB)) (parser, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

/* Handle data type callbacks */

static
void
Handle_Fq_Token(NW_WBXML_Parser_t *parser,
                NW_Uint32 token)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->FQToken_CB)){
    status = (*(parser->handler->FQToken_CB)) (parser, token, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Inline_String(NW_WBXML_Parser_t *parser,
                     NW_Uint32 len)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->InlineString_CB)){
    status = (*(parser->handler->InlineString_CB)) (parser, len,
                                                    parser->context);
    
        NW_WBXML_Parser_setStatus (parser, status);
 } 
}

static
void
Handle_Table_String(NW_WBXML_Parser_t *parser,
                    NW_Uint32 index)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->TableString_CB)){
    status = (*(parser->handler->TableString_CB)) (parser, index,
                                                   parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Binary(NW_WBXML_Parser_t *parser,
              NW_Uint32 value)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Binary_CB)){
    status = (*(parser->handler->Binary_CB)) (parser, value, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Opaque(NW_WBXML_Parser_t *parser,
              NW_Uint32 len)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Opaque_CB)){
    status = (*(parser->handler->Opaque_CB)) (parser, len, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

static
void
Handle_Entity(NW_WBXML_Parser_t *parser,
              NW_Uint32 e)
{
  NW_Status_t status;
  if ((parser->handler) && (parser->handler->Entity_CB)){
    status = (*(parser->handler->Entity_CB)) (parser, e, parser->context);
    NW_WBXML_Parser_setStatus (parser, status);
  }
}

/*
* Safely get the next token, checking for switch pages along the way.
* While this does advance past any switch pages, it DOES NOT advance
* past the returned token.
*/

static
NW_Status_t
NW_WBXML_Parser_getNextToken (NW_WBXML_Parser_t * parser,
                              NW_Uint8 * token,
                              NW_Int32 token_state)
{

  NW_ASSERT(parser != NULL);

  if (!NW_WBXML_Parser_hasMoreBytecode (parser)) {
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  while ((*token = NW_WBXML_Parser_readUint8 (parser)) ==
         NW_WBXML_SWITCH_PAGE)
  {
    if (NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    if (!NW_WBXML_Parser_hasMoreBytecode (parser)) {
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }

    if (token_state == TOKEN_STATE_TAG){
      parser->tag_code_page = NW_WBXML_Parser_readUint8 (parser);
    } else {
      parser->attribute_code_page = NW_WBXML_Parser_readUint8 (parser);
    }

    if((parser->cp_registry.count > 0)
      && ((parser->flags & NW_WBXML_REGISTRY_INIT) != NW_WBXML_REGISTRY_INIT)){
      NW_ASSERT(parser->offset > 0);
      /* We have a registry but we haven't finished initing it yet */
      // WLIU_DEBUG: RFileLogger::WriteFormat(_L("Browser"), _L("cp_count.txt"), EFileLoggingModeAppend, _L("=== cp_count: %x, pW->index: %x \n"), parser->cp_registry.realcount +1, parser->offset - parser->lastValid);
      NW_WBXML_CPRegistry_addEntry(&(parser->cp_registry),
                                   NW_WBXML_Parser_readUint8(parser),
                                   /* must use switch page token position */
                                   parser->offset - 1,
                                   (NW_Uint8)token_state);
    }

    Handle_Codepage(parser);

    if (NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    if (!NW_WBXML_Parser_hasMoreBytecode (parser)) {
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }
  return NW_STAT_SUCCESS;
}

/*
 * Element parsing methods: The parser is made up of a set of methods
 * that know how to parse individual elements of the wbxml source. For
 * elements that repeat, these are implemented in iterators. Building
 * the parser from a set of element parsers and iterators makes it
 * simple to use the parser as a deserializer for wbxml elements.
 */

#define HAS_CONTENT(t)    ((t)&NW_WBXML_FLAGS_CONTENT)
#define HAS_ATTRIBUTES(t) ((t)&NW_WBXML_FLAGS_ATTRIBUTES)

/*
* Parse the token and name part of a tag. This generates a fully
* qualified token.
*/

NW_Status_t
NW_WBXML_Parser_tagNameParse(NW_WBXML_Parser_t *parser)
{
  NW_Uint8 token;
  NW_Int32 ilen;
  NW_Uint32 index;
  NW_Uint32 fq_token;
  NW_Status_t status;

  NW_ASSERT(parser != NULL);

  Handle_Tag_Start(parser);

  CHECK_PARSER_STATUS;

  status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_TAG);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  fq_token = FqToken (parser, token, TOKEN_STATE_TAG);
  Handle_Fq_Token(parser, fq_token);

  if (HAS_CONTENT(fq_token) || HAS_ATTRIBUTES(fq_token) ||
      (NW_WBXML_Parser_hasMoreBytecode (parser)))
  {
    if (NW_WBXML_Parser_advance (parser, 1) < 0){
        return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }

  /*
   * Note that while the spec defines tokens for literal tags with
   * and without attributes and content, they are just
   * NW_WBXML_LITERAL with the content and/or attributes flags
   * set. So ...
   */

  if ((token & NW_WBXML_MASK_TAG_ID) == NW_WBXML_LITERAL){

  /*
   * TODO:  Should check for another global token
   * and return an error?
   */

    ilen = NW_WBXML_Parser_readMbUint32 (parser, &index);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }

    Handle_Table_String(parser, index);

    if(NW_WBXML_Parser_advance(parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }

  if (HAS_ATTRIBUTES (fq_token)){   /* Has attributes, possibly content */
    status = NW_STAT_WBXML_HAS_ATTRIBUTES;
    if (HAS_CONTENT (fq_token)){
      status = NW_STAT_WBXML_HAS_ATTR_CONTENT;
    }
  }

  else if (HAS_CONTENT (fq_token)){ /* Has content, no attributes */
    status = NW_STAT_WBXML_HAS_CONTENT;
  }

  else{                             /* No attributes, no content */
    status = NW_STAT_SUCCESS;
    Handle_Tag_End(parser);
  }

  return status;
}

/* Parse an attribute list */

NW_Status_t
NW_WBXML_Parser_attributeListIterate(NW_WBXML_Parser_t *parser)
{
  NW_Uint8 token;
  NW_Status_t status;

  NW_ASSERT(parser != NULL);

  status = NW_WBXML_Parser_attributeParse (parser);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_ATTR);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  if (token == NW_WBXML_END){
    return NW_STAT_WBXML_ITERATE_DONE;
  }
  return NW_STAT_WBXML_ITERATE_MORE;
}

static
NW_Status_t
NW_WBXML_Parser_attributeListParse(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;

  /* Run the attribute list iterator till it completes */

  while ((parser->flags & NW_WBXML_PARSER_S_MASK) == NW_WBXML_PARSER_OK){
    status = NW_WBXML_Parser_attributeListIterate(parser);
    if(status == NW_STAT_WBXML_ITERATE_DONE){
      return NW_STAT_SUCCESS;
    }
    if(status != NW_STAT_WBXML_ITERATE_MORE){
      return status;
    }
  }
  return NW_WBXML_Parser_flagToStatus(parser);
}

/*
* Parse a processing instruction
*/
static
NW_Status_t
NW_WBXML_Parser_piParse (NW_WBXML_Parser_t * parser,
                         NW_Int32 token_state)
{

  NW_Uint8 token;
  NW_Status_t status;

  status = NW_WBXML_Parser_getNextToken (parser, &token, token_state);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  switch (token){
  case NW_WBXML_PI:
    Handle_Pi(parser);
    /*
    * Advance past the PI token and then get the PI's
    * target and value
    */
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    status = NW_WBXML_Parser_attributeParse (parser);
    if(status != NW_STAT_SUCCESS){
      return status;
    }
    /* Advance past the PI's end token */
    status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_ATTR);
    if(status != NW_STAT_SUCCESS){
      return status;
    }
    if (token == NW_WBXML_END){
      Handle_Pi_End(parser);
      /*
       * Must advance the reader past the end token but since this
       * PI may be the last NW_Byte in the bytecode, first check to see
       * if there is any bytecode left.
       */
      if (NW_WBXML_Parser_hasMoreBytecode (parser)){
        NW_WBXML_Parser_advance (parser, 1);
      }
    }
    else
      return NW_STAT_WBXML_ERROR_BYTECODE;
    break;
  default:
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }
  return NW_STAT_SUCCESS;
}

/* Parse an element, recursively parsing content */

static
NW_Status_t
NW_WBXML_Parser_elementParse (NW_WBXML_Parser_t * parser)
{
  NW_Status_t tag_status = NW_STAT_SUCCESS;
  NW_Status_t status     = NW_STAT_WBXML_ERROR_BYTECODE;

  if (++(parser->recursiveCallCnt) >= WBXML_MAX_RECURSIVE_CALL_DEPTH) {
    goto FuncExit;
  }

  /* The while loop checks parser status after each case. */
  while ((parser->flags & NW_WBXML_PARSER_S_MASK) == NW_WBXML_PARSER_OK){
    if (tag_status == NW_STAT_SUCCESS){
      tag_status = NW_WBXML_Parser_tagNameParse(parser);
      if(tag_status == NW_STAT_SUCCESS){
        status = NW_STAT_SUCCESS; /* Normal return */
        goto FuncExit;
      }
    }

    else if((tag_status == NW_STAT_WBXML_HAS_ATTRIBUTES) ||
            (tag_status == NW_STAT_WBXML_HAS_ATTR_CONTENT)) {

      status = NW_WBXML_Parser_attributeListParse (parser);
      if(status != NW_STAT_SUCCESS){
        goto FuncExit;
      }

      if (NW_WBXML_Parser_hasMoreBytecode (parser)){
        if(NW_WBXML_Parser_advance (parser, 1) < 0){
          status = NW_STAT_WBXML_ERROR_BYTECODE;
          goto FuncExit;
        }
      }

      if(tag_status == NW_STAT_WBXML_HAS_ATTR_CONTENT) {
        tag_status = NW_STAT_WBXML_HAS_CONTENT;
      }
      else {
        Handle_Tag_End(parser);
        status = NW_STAT_SUCCESS;       /* Normal return */
        goto FuncExit;
      }

      /*  TODO:  else check for error (token >= 128) */
    }

    else if((tag_status == NW_STAT_WBXML_HAS_CONTENT) ||
            (tag_status == NW_STAT_WBXML_HAS_ATTR_CONTENT)) {

      status = NW_WBXML_Parser_contentParse(parser);
      if(status != NW_STAT_SUCCESS){
        goto FuncExit;
      }

      Handle_Tag_End(parser);
      status = NW_STAT_SUCCESS; /* Normal return */
      goto FuncExit;
    }
    else{
      /* Unexpected NW_Byte code */
      status = NW_STAT_WBXML_ERROR_BYTECODE;
      goto FuncExit;
    }
  }
  status = NW_STAT_WBXML_ERROR_BYTECODE; /* Bad parser status */

  FuncExit:
  --(parser->recursiveCallCnt);
  return status;
}


/* Parse a sequence of text components until a non-text component is reached */


static
NW_Uint32
isTextToken(NW_Uint8 token){
  if((token == NW_WBXML_STR_I)
    ||(token == NW_WBXML_STR_T)
    ||(token == NW_WBXML_OPAQUE)
    ||(token == NW_WBXML_ENTITY)
    ||(token == NW_WBXML_EXT_I_0)
    || (token == NW_WBXML_EXT_I_1)
    || (token == NW_WBXML_EXT_I_2)
    || (token == NW_WBXML_EXT_T_0)
    || (token == NW_WBXML_EXT_T_1)
    || (token == NW_WBXML_EXT_T_2)
    || (token == NW_WBXML_EXT_0)
    || (token == NW_WBXML_EXT_1)
    || (token == NW_WBXML_EXT_2)){
    return 1;
  }
  return 0;
}


NW_Status_t
NW_WBXML_Parser_textIterate(NW_WBXML_Parser_t * parser){

  NW_Uint8 token;
  NW_Int32 ilen = 0;
  NW_Uint32 index;
  NW_Uint32 e;
  NW_Status_t status;

  status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_TAG);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  switch (token){
  case NW_WBXML_STR_I:
    Handle_Content(parser);
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    ilen = NW_WBXML_Parser_getInlineStrLen(parser, parser->doc);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Inline_String(parser, (NW_Uint32)ilen);
    if(NW_WBXML_Parser_advance(parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    break;
  case NW_WBXML_STR_T:
    Handle_Content(parser);
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    ilen = NW_WBXML_Parser_readMbUint32 (parser, &index);
    if ((ilen < 0) || (index >= parser->doc->strtbl.length)) {
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    if (!NW_String_valid(parser->doc->strtbl.data + index,
                         parser->doc->strtbl.length - index,
                         parser->doc->charset)) {
      return  NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Table_String(parser, index);
    if(NW_WBXML_Parser_advance(parser, ilen)< 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    break;
  case NW_WBXML_ENTITY:
    Handle_Content(parser);
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    ilen = NW_WBXML_Parser_readMbUint32 (parser, &e);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    if(NW_WBXML_Parser_advance (parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Entity(parser, e);
    break;
  case NW_WBXML_EXT_I_0:
  case NW_WBXML_EXT_I_1:
  case NW_WBXML_EXT_I_2:
  case NW_WBXML_EXT_T_0:
  case NW_WBXML_EXT_T_1:
  case NW_WBXML_EXT_T_2:
  case NW_WBXML_EXT_0:
  case NW_WBXML_EXT_1:
  case NW_WBXML_EXT_2:
    Handle_Content(parser);
    status = NW_WBXML_Parser_extensionParse (parser, TOKEN_STATE_TAG);
    if(status != NW_STAT_SUCCESS){
      return status;
    }
    break;
  case NW_WBXML_OPAQUE:
    Handle_Content(parser);
    status = NW_WBXML_Parser_opaqueParse (parser, TOKEN_STATE_TAG);
    if(status != NW_STAT_SUCCESS){
      return status;
    }
    break;
  case NW_WBXML_PI:
    status = NW_WBXML_Parser_piParse (parser, TOKEN_STATE_TAG);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
    break;
  default:
    return NW_STAT_FAILURE;
  }
  if (NW_WBXML_Parser_hasMoreBytecode (parser)) {
    status = NW_WBXML_Parser_getNextToken(parser, &token, TOKEN_STATE_TAG);
    if(status != NW_STAT_SUCCESS){
      return status;
    }
    if(isTextToken(token))
      return NW_STAT_WBXML_ITERATE_MORE;
  }
  return NW_STAT_WBXML_ITERATE_DONE;
}

static
NW_Status_t
NW_WBXML_Parser_textParse(NW_WBXML_Parser_t * parser){

  NW_Status_t status;

  /* Run the text iterator till it completes */

  while ((parser->flags & NW_WBXML_PARSER_S_MASK) == NW_WBXML_PARSER_OK){
    status = NW_WBXML_Parser_textIterate(parser);
    if(status == NW_STAT_WBXML_ITERATE_DONE){
      return NW_STAT_SUCCESS;
    }
    if(status != NW_STAT_WBXML_ITERATE_MORE){
      return status;
    }
  }
  return NW_WBXML_Parser_flagToStatus(parser);
}


/* Parse tag content */

NW_Status_t
NW_WBXML_Parser_contentParse (NW_WBXML_Parser_t * parser){

  NW_Uint8 token;
  NW_Status_t status;

  while ((parser->flags & NW_WBXML_PARSER_S_MASK) == NW_WBXML_PARSER_OK){

    status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_TAG);
    if(status != NW_STAT_SUCCESS){
      return status;
    }

    switch (token){
    case NW_WBXML_END:
      if (NW_WBXML_Parser_hasMoreBytecode (parser)){
        NW_WBXML_Parser_advance (parser, 1);
      }
      return NW_STAT_SUCCESS;             /* Normal return */

    case NW_WBXML_STR_I:
    case NW_WBXML_STR_T:
    case NW_WBXML_ENTITY:
    case NW_WBXML_PI:
    case NW_WBXML_EXT_I_0:
    case NW_WBXML_EXT_I_1:
    case NW_WBXML_EXT_I_2:
    case NW_WBXML_EXT_T_0:
    case NW_WBXML_EXT_T_1:
    case NW_WBXML_EXT_T_2:
    case NW_WBXML_EXT_0:
    case NW_WBXML_EXT_1:
    case NW_WBXML_EXT_2:
    case NW_WBXML_OPAQUE:
      status = NW_WBXML_Parser_textParse(parser);
      if(status != NW_STAT_SUCCESS){
        return status;
      }
      break;
    case NW_WBXML_LITERAL:
    case NW_WBXML_LITERAL_A:
    case NW_WBXML_LITERAL_C:
    case NW_WBXML_LITERAL_AC:
    default:
      Handle_Content(parser);
      status = NW_WBXML_Parser_elementParse (parser);
      if(status != NW_STAT_SUCCESS){
        return status;
      }
      break;
    }
  }
  return NW_WBXML_Parser_flagToStatus(parser);
}

/* Parse an extension */

NW_Status_t
NW_WBXML_Parser_extensionParse (NW_WBXML_Parser_t * parser,
                                NW_Int32 token_state)
{
  NW_Uint8 token = 0;
  NW_Uint32 fq_token;
  int ilen;
  NW_Uint32 value;
  NW_Status_t status;

  /*  if (token_state == TOKEN_STATE_TAG)
  * handler = parser->handler; TODO: deal with this
  */

  Handle_Extension(parser);

  status = NW_WBXML_Parser_getNextToken (parser, &token, token_state);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  fq_token = FqToken (parser, token, token_state);
  Handle_Fq_Token(parser, fq_token);

  switch (token){
  case NW_WBXML_EXT_I_0:
  case NW_WBXML_EXT_I_1:
  case NW_WBXML_EXT_I_2:
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }

    ilen = NW_WBXML_Parser_getInlineStrLen(parser, parser->doc);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }

    Handle_Inline_String(parser, (NW_Uint32)ilen);

    if(NW_WBXML_Parser_advance(parser, ilen)<0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    break;
  case NW_WBXML_EXT_T_0:
  case NW_WBXML_EXT_T_1:
  case NW_WBXML_EXT_T_2:

    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }

    ilen = NW_WBXML_Parser_readMbUint32 (parser, &value);

    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }

    if (parser->ext_t_not_table_index == 0) {
      Handle_Table_String(parser, value);
    } else {
      Handle_Binary(parser, value); /* handle ext_t anonymous int */
    }
    if(NW_WBXML_Parser_advance (parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    break;
  case NW_WBXML_EXT_0:
  case NW_WBXML_EXT_1:
  case NW_WBXML_EXT_2:
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    /* Note - For an extension token without other parameters the
     second parameter to Handle_Binary is bogus.  The callback is here
     so that the parser client can manage its internal state. */
    Handle_Binary(parser, 0);
    break;
  default:
    NW_ASSERT(NW_FALSE);
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }
  return NW_STAT_SUCCESS;
}


/*
* Parse an attribute
*/



NW_Status_t
NW_WBXML_Parser_attributeNameParse (NW_WBXML_Parser_t * parser){

  NW_Uint8 token;
  NW_Uint32 fq_token;
  NW_Uint32 index;
  NW_Int32 ilen;
  NW_Status_t status;

  if (!NW_WBXML_Parser_hasMoreBytecode(parser)) {
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_ATTR);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  if (token < 128){
    Handle_Attr_Start(parser);
    if (parser->status == NW_STAT_OUT_OF_MEMORY) {
      return parser->status;
    }
    if(NW_WBXML_Parser_advance (parser, 1)<0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }

    fq_token = FqToken (parser, token, TOKEN_STATE_ATTR);

    Handle_Fq_Token(parser, fq_token);
  }

  else{
    return NW_STAT_WBXML_ERROR_BYTECODE;
  }

  if (token == NW_WBXML_LITERAL){
    /*  TODO:  Should check for another global token and return an error? */
    ilen = NW_WBXML_Parser_readMbUint32 (parser, &index);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Table_String(parser, index);
    if(NW_WBXML_Parser_advance(parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }
  return NW_STAT_SUCCESS;
}


NW_Status_t
NW_WBXML_Parser_attributeValsIterate (NW_WBXML_Parser_t * parser)
{
  NW_Uint8 token;
  NW_Uint32 fq_token;
  NW_Uint32 index;
  NW_Uint32 e;
  NW_Int32 ilen;
  NW_Status_t status;

  status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_ATTR);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  if (token == NW_WBXML_STR_I){
    /* NW_WBXML_ATTR_COMPONENT_STRING; */
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    ilen = NW_WBXML_Parser_getInlineStrLen(parser, parser->doc);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Attr_Val(parser);
    Handle_Inline_String(parser, (NW_Uint32) ilen);
    if(NW_WBXML_Parser_advance(parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }
  else if (token == NW_WBXML_STR_T){
    /* NW_WBXML_ATTR_COMPONENT_STRING; */
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    ilen = NW_WBXML_Parser_readMbUint32 (parser, &index);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Attr_Val(parser);
    Handle_Table_String(parser, index);
    if(NW_WBXML_Parser_advance(parser, ilen)<0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }
  else if (token == NW_WBXML_OPAQUE){
    /* NW_WBXML_ATTR_COMPONENT_OPAQUE; */
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    ilen = NW_WBXML_Parser_readMbUint32 (parser, &index);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    /* Move past the length NW_Byte(s) */
    if(NW_WBXML_Parser_advance (parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Attr_Val(parser);
    Handle_Opaque(parser, index);
    if(NW_WBXML_Parser_advance (parser, (NW_Int32)index) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }
  else if (token == NW_WBXML_ENTITY){
    /*NW_WBXML_ATTR_COMPONENT_ENTITY; */
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    ilen = NW_WBXML_Parser_readMbUint32 (parser,&e);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Entity(parser, e);
    if(NW_WBXML_Parser_advance (parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }
  else if ((token == NW_WBXML_EXT_I_0)
           || (token == NW_WBXML_EXT_I_1)
           || (token == NW_WBXML_EXT_I_2)
           || (token == NW_WBXML_EXT_T_0)
           || (token == NW_WBXML_EXT_T_1)
           || (token == NW_WBXML_EXT_T_2)
           || (token == NW_WBXML_EXT_0)
           || (token == NW_WBXML_EXT_1)
           || (token == NW_WBXML_EXT_2)){

    /* NW_WBXML_ATTR_COMPONENT_EXT; */
    Handle_Attr_Val(parser);
    status = NW_WBXML_Parser_extensionParse (parser, TOKEN_STATE_ATTR);
    if(status != NW_STAT_SUCCESS){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }
  else if (token >= 128){
    /*NW_WBXML_ATTR_COMPONENT_TOKEN; */
    fq_token = FqToken(parser, token, TOKEN_STATE_ATTR);
    Handle_Attr_Val(parser);
    Handle_Fq_Token(parser, fq_token);
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
  }

  status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_ATTR);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  /*
  * Attribute tokens are either text tokens or
  * dictionary-defined tokens > 128
  */
  if(isTextToken(token) || (token >=128)){

    return NW_STAT_WBXML_ITERATE_MORE;          /* Normal exit */
  }
  else /*if ((token < 128) || (token == NW_WBXML_END)
    || (token == NW_WBXML_LITERAL))*/{
    /*parser->state &= ~NW_WBXML_PARSER_S_ATTR_VAL; */
    /* Don't advance parser */
    return NW_STAT_WBXML_ITERATE_DONE;      /* Normal exit */
  }
}

NW_Status_t
NW_WBXML_Parser_attributeParse(NW_WBXML_Parser_t *parser)
{
  NW_Status_t status;

  status = NW_WBXML_Parser_attributeNameParse(parser);
  if(status != NW_STAT_SUCCESS){
    return status;
  }
  while ((parser->flags & NW_WBXML_PARSER_S_MASK) == NW_WBXML_PARSER_OK){
    status = NW_WBXML_Parser_attributeValsIterate(parser);
    if(status == NW_STAT_WBXML_ITERATE_DONE){
      return NW_STAT_SUCCESS;
    }
    if(status != NW_STAT_WBXML_ITERATE_MORE){
      return status;
    }
  }
  return NW_WBXML_Parser_flagToStatus(parser);
}


/*
* Parse opaque data
*/

NW_Status_t
NW_WBXML_Parser_opaqueParse (NW_WBXML_Parser_t * parser,
                             NW_Int32 token_state)
{

  NW_Uint32 index;
  NW_Int32 ilen = 0;
  NW_Uint8 token;
  NW_Status_t status;

  status = NW_WBXML_Parser_getNextToken (parser, &token, token_state);
  if(status != NW_STAT_SUCCESS){
    return status;
  }

  switch (token){
  case NW_WBXML_OPAQUE:
    if(NW_WBXML_Parser_advance (parser, 1) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    ilen = NW_WBXML_Parser_readMbUint32 (parser, &index);
    if(ilen < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    if(NW_WBXML_Parser_advance (parser, ilen) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    Handle_Opaque(parser,index);
    if(NW_WBXML_Parser_advance (parser, (NW_Int32)index) < 0){
      return NW_STAT_WBXML_ERROR_BYTECODE;
    }
    break;
  default:
    /* return NW_STAT_WBXML_ERROR_BYTECODE;*/
    NW_ASSERT(NW_FALSE); /* Never reached */
  }
  return NW_STAT_SUCCESS;
}


/*
* Parse the body of a document
*
* RETURN NW_STAT_SUCCESS
*        NW_STAT_OUT_OF_MEMORY
*        NW_STAT_BAD_INPUT_PARAM
*        NW_STAT_WBXML_ERROR_BYTECODE
*        NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED
*/

NW_Status_t
NW_WBXML_Parser_bodyParse (NW_WBXML_Parser_t * parser)
{
  NW_Status_t status;
  NW_Uint8 token;

  NW_ASSERT(parser != NULL);
  // WLIU_DEBUG: RFileLogger::WriteFormat(_L("Browser"), _L("cp_count.txt"), EFileLoggingModeAppend, _L("====== body parse starts =====\n"));

  /* This is not quite the WBXML grammar because we want to permit
     loose HTML docs where there is no one document node. */
  while (((parser->flags & NW_WBXML_PARSER_S_MASK) == NW_WBXML_PARSER_OK)
         && NW_WBXML_Parser_hasMoreBytecode (parser)) {

    status = NW_WBXML_Parser_getNextToken (parser, &token, TOKEN_STATE_TAG);
    if(status != NW_STAT_SUCCESS){
      return status;
    }

    switch (token){
    case NW_WBXML_END:
      if (NW_WBXML_Parser_hasMoreBytecode (parser)) {
        if(NW_WBXML_Parser_advance (parser, 1) < 0){
          return NW_STAT_WBXML_ERROR_BYTECODE;
        }
        Handle_Tag_End(parser);
      }
      break;

    case NW_WBXML_STR_I:
    case NW_WBXML_STR_T:
    case NW_WBXML_ENTITY:
    case NW_WBXML_PI:
    case NW_WBXML_EXT_I_0:
    case NW_WBXML_EXT_I_1:
    case NW_WBXML_EXT_I_2:
    case NW_WBXML_EXT_T_0:
    case NW_WBXML_EXT_T_1:
    case NW_WBXML_EXT_T_2:
    case NW_WBXML_EXT_0:
    case NW_WBXML_EXT_1:
    case NW_WBXML_EXT_2:
    case NW_WBXML_OPAQUE:
      status = NW_WBXML_Parser_textParse(parser);
      if(status != NW_STAT_SUCCESS){
        return status;
      }
      break;

    case NW_WBXML_LITERAL:
    case NW_WBXML_LITERAL_A:
    case NW_WBXML_LITERAL_C:
    case NW_WBXML_LITERAL_AC:
    default:
      status = NW_WBXML_Parser_elementParse (parser);
      if(status != NW_STAT_SUCCESS){
        return status;
      }
      break;
    }
  }
  CHECK_PARSER_STATUS;

  /*
  * If we have a registry then it must be inited at this point, so
  * set the init flag.
  */

  if((parser->cp_registry.count > 0)
    && ((parser->flags & NW_WBXML_REGISTRY_INIT) != NW_WBXML_REGISTRY_INIT)){
    parser->flags |= NW_WBXML_REGISTRY_INIT;
  }

  // WLIU_DEBUG: RFileLogger::WriteFormat(_L("Browser"), _L("cp_count.txt"), EFileLoggingModeAppend, _L("====== body parse ends =====\n"));

  if (parser->handler && parser->handler->EndDocument_CB){
    return (*(parser->handler->EndDocument_CB)) (parser, parser->context);
  }
  return NW_STAT_SUCCESS;
}


/*
* Cache the parser's event handler and context
*
* RETURN: NW_STAT_SUCCESS
*/

EXPORT_C NW_Status_t
NW_WBXML_Parser_registerHandler (NW_WBXML_Parser_t * parser,
                                 const struct NW_WBXML_EventHandler_s * handler,
                                 void *context)
{

  NW_ASSERT(parser != NULL);

  parser->handler = handler;
  parser->context = context;

  return NW_STAT_SUCCESS;
}
