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
#include "cxml_internal.h"
#include <xml/cxml/nw_tinydom.h>
#include <xml/cxml/nw_wbxml_parse.h>
#include <xml/cxml/nw_wbxml_reader.h>
#include <xml/cxml/nw_wbxml_event.h>
#include "nw_wbxml_parsei.h"

/*Parser flags */

/* Top four bits are pass state */
#define T_PARSE_PASS_MASK      0xF0000000
#define T_PARSE_PASS_1         0x00000000
#define T_PARSE_PASS_2         0x10000000
#define T_PARSE_PASS_3         0x20000000

/* Next 8 bits are for parser states */
#define T_PARSE_S_MASK         0x07F00000
#define T_PARSE_S_TAG_START    0x00000000
#define T_PARSE_S_ATTR_START   0x00100000
#define T_PARSE_S_ATTR_VALS    0x00200000
#define T_PARSE_S_CONTENT      0x00300000
#define T_PARSE_F_EXT          0x08000000 /* Extension substate flag */

  /* Remaining 20 bits are flags */
#define T_PARSE_FLAG_TEXT      0x00000001
#define T_PARSE_FLAG_ATTR      0x00000002

/*
* Implements the tiny dom parser. This includes the methods to build a
* tiny dom tree, and the underlying support routines to access the
* "virtual elements" of the tree. The only thing that currently gets
* in the way of a clean parser interface are several references to the
* underlying parser state here. The underlying parser needs to be
* fixed so it is never necessary to pass this. Several TODOs in this
* file indicate places where references to parser state should be
* removed.  
*/

static
NW_Uint32
GET_PASS(NW_TinyDom_Parser_t* parser){
  return parser->state & T_PARSE_PASS_MASK;
}

static
void 
SET_PASS(NW_TinyDom_Parser_t* parser, NW_Uint32 pass){
  parser->state &= ~T_PARSE_PASS_MASK;
  parser->state |= pass;
}

static
NW_Uint32
GET_STATE(NW_TinyDom_Parser_t* parser){
  return parser->state & T_PARSE_S_MASK;
}

static
void 
SET_STATE(NW_TinyDom_Parser_t* parser, NW_Uint32 state){
  parser->state &= ~T_PARSE_S_MASK;
  parser->state |= state;
}

/* Callbacks to handle parsing passes: 
*
* Tiny dom uses multiple passes to implement the tree lazily. The
* first two build the tree. Later, lazy deserialization invokes
* further passes over parts of the source.
* 
* Pass 1 counts nodes and allocates space, pass 2 builds the
* tree. However, there is much common behavior for both passes, since
* accurate counting of nodes requires running the parser state
* machine. Common behavior is implemented in common callbacks, while
* pass-specific behavior is mostly implemented in pass-specific
* callbacks.
*
* The tree built by passes 1 & 2 contains a node for every tag. If a tag
* has attributes, the tree contains a node for the first attribute,
* attached as the first child of the node. For text content, the tree
* contains a node for the first text element in each sequence of text
* elements. The goal of the tree design is to make it easy to find
* elements for deserialization while using the minimum amount of
* storage.
* 
* The deserialization API invokes parser methods that know how to
* parse one of the node types stored in the tree (tags, attributes,
* text). The API sets the parser offset using the node offset,
* identifies the node type using the node type flags, and then calls
* the appropriate parser method to parse a section of the source.
* 
* When called for deserialization the parser generates events as usual:
* these are handled by the pass 3 handlers which build the appropriate
* data structures for the API to return.
* 
* This may seem like a rather byzantine mechanism for
* deserialization, but the goal is to use exactly the same parsing
* code for all operations on the source. This makes it easy to change
* or replace parsing code.  Also, all of the parsing logic is kept in
* the parser module: the dom module (this one) has no knowledge of
* parsing. This makes it easy to attach the dom module to different
* parsers (one for wbxml, one for xml).  
*
* A note on buffer usage: tiny trees use segmented expandable buffers
* (ebuffers) so that the tree can be read and written in minimum memory.
* When building a tree from an existing buffer (passes 1 & 2), the whole
* buffer is contained in the ebuffer segment 0. This means that the 
* offsets we store in the tree nodes (which are indexes into the ebuffer)
* are the same as the parser offsets in the buffer. However, for 
* deserialization (pass 3), we may be reading a tree that was constructed
* or modified via the dom api. This may have multiple segments and a given
* node offset may refer to an address in any segment. Therefore, when setting
* the parser offset from a node source offset, we first need to translate
* the node source offset into a segment + offset form, and use this to set
* the parser.   
*/


static 
NW_Status_t 
Pass_1_StartDocument_CB (NW_WBXML_Parser_t *parser, 
                         NW_WBXML_Document_t *doc, 
                         void *context)
{

  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  NW_REQUIRED_PARAM(parser);
  NW_REQUIRED_PARAM(doc);

  SET_STATE(tiny_parser, T_PARSE_S_CONTENT);
  tiny_parser->state &= ~T_PARSE_FLAG_TEXT; /* prepare for text */
  tiny_parser->node_count = 1;
  return NW_STAT_SUCCESS;

}

static 
NW_Status_t 
Pass_2_StartDocument_CB (NW_WBXML_Parser_t *parser, 
                         NW_WBXML_Document_t *doc, 
                         void *context)
{
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  NW_REQUIRED_PARAM(parser);
  NW_REQUIRED_PARAM(doc);

  tiny_parser->dom_tree->root_node = NW_TinyTree_setRoot(&(tiny_parser->dom_tree->tree), 0);

  if (tiny_parser->dom_tree->root_node != NULL){
    tiny_parser->current_node = tiny_parser->dom_tree->root_node;
    NW_TinyTree_Node_setUserFlags(tiny_parser->current_node, T_DOM_NODE_DOC);
    SET_STATE(tiny_parser, T_PARSE_S_CONTENT);
    tiny_parser->state &= ~T_PARSE_FLAG_TEXT; /* prepare for text */
    return NW_STAT_SUCCESS;
  }
  else{
    return NW_STAT_OUT_OF_MEMORY;
  }  
}

/* Private utility function to do NW_Byte reordering of ucs2 strings */
/* TODO:  move this to our string library */

static 
void
NW_String_Ucs2_ntoh(NW_String_t *string){
  NW_Uint32 i;
  NW_Ucs2 character;

  for(i = 0; i < string->length; i+=2){
    character = (NW_Ucs2)((string->storage[i] << 8) | string->storage[i+1]);
    (void) NW_Mem_memcpy (&string->storage[i], &character, sizeof (character));
  }
}

/* 
 * This callback sets the parser pass and takes care of any NW_Byte reordering 
 * that needs to be done on the string table. For some encodings (such as ucs2)
 * this allows applications to reference strings from the original storage.
 */

/* TODO:  TODO: Add a flag to document to check if this has already been done 
  (in case the same buffer is parsed over again from the beginning.)
 */


static 
NW_Status_t 
Pass_1_EndDocument_CB (NW_WBXML_Parser_t *parser, 
                       void *context)
{

  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;
  //NW_String_t string;
  //NW_WBXML_Document_t *doc = tiny_parser->dom_tree->doc;

  NW_REQUIRED_PARAM(parser);

  // WLIU: all byte order conversions are moved to pass 2.
  //if(doc->charset == HTTP_iso_10646_ucs_2){
    /* TODO:  redefine string table as an NW_String_t ?? */
    //string.storage = doc->strtbl.data;
    //string.length = doc->strtbl.length;
    //NW_String_Ucs2_ntoh(&string);
  //}
  SET_PASS(tiny_parser, T_PARSE_PASS_2);
  return NW_STAT_SUCCESS;  
}


static 
NW_Status_t 
Pass_2_EndDocument_CB (NW_WBXML_Parser_t *parser, 
                       void *context)
{

  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  NW_REQUIRED_PARAM(parser);

  SET_PASS(tiny_parser, T_PARSE_PASS_3);
  return NW_STAT_SUCCESS;  
}

static 
NW_Status_t 
Pass_1_Tag_Start_CB (NW_WBXML_Parser_t *parser, 
                     void *context)
{

  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  NW_REQUIRED_PARAM(parser);

  tiny_parser->node_count++;
  return NW_STAT_SUCCESS;
}


static 
NW_Status_t 
Pass_2_Tag_Start_CB (NW_WBXML_Parser_t *parser, 
                     void *context)
{
  NW_Status_t status = NW_STAT_SUCCESS;
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  //Atleast there should be root node, before any tag

  if(tiny_parser->current_node != NULL)
  {
   tiny_parser->current_node = 
     NW_TinyTree_createChild(&(tiny_parser->dom_tree->tree), 
                            tiny_parser->current_node, 
                            (NW_TinyTree_Offset_t)parser->offset);

   if (tiny_parser->current_node != NULL)
   {
     NW_TinyTree_Node_setUserFlags(tiny_parser->current_node, T_DOM_NODE_TAG);
     status = NW_STAT_SUCCESS;
   }
   else
   {
    status =  NW_STAT_FAILURE;
   }  
  }//endif(tiny_parser->current_node != NULL)
  else
  {
   status =  NW_STAT_FAILURE;
  }

  return status;
}//end Pass_2_Tag_Start_CB(..)


static 
NW_Status_t 
Tag_Start_CB (NW_WBXML_Parser_t *parser, 
              void *context)

{
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;
  NW_Status_t s = NW_STAT_FAILURE;

  switch(GET_PASS(tiny_parser)){
  case T_PARSE_PASS_1:
    s = Pass_1_Tag_Start_CB(parser, context);
    break;
  case T_PARSE_PASS_2:
    s = Pass_2_Tag_Start_CB(parser, context);
    break;
  default:
    NW_ASSERT(NW_FALSE);
  }
  SET_STATE(tiny_parser, T_PARSE_S_TAG_START);
  tiny_parser->state &= ~T_PARSE_FLAG_TEXT; /* No longer accumulating text */
  tiny_parser->state &= ~T_PARSE_FLAG_ATTR; /* Turn off attribute flag */
  return s;
}

static
NW_Status_t
Pass_2_Pi_CB (NW_WBXML_Parser_t *parser, 
              void *context)
{
  /* This is exactly like pass 2 tag start except for the node user flag. */
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;
  NW_Status_t s = Tag_Start_CB(parser, context);

  if (s == NW_STAT_SUCCESS) {
    NW_TinyTree_Node_setUserFlags(tiny_parser->current_node, T_DOM_NODE_PI);
  }
  return s;
}

static 
NW_Status_t 
Tag_End_CB (NW_WBXML_Parser_t *parser, 
            void *context)
{
  NW_Status_t status = NW_STAT_SUCCESS;
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;
  NW_Int32 lastvalid;
  NW_REQUIRED_PARAM(parser);

  lastvalid = NW_TinyDom_getLastValid(tiny_parser->dom_tree);
  if(GET_PASS(tiny_parser)== T_PARSE_PASS_2)
  {
    /* for the dom tree appending, we won't handle close tag which is beyond last valid mark */
    if (lastvalid == -1 || (NW_Int32)parser->offset <= lastvalid)
	{
      if(tiny_parser->current_node != NULL)
	  {
      tiny_parser->current_node = NW_TinyTree_findParent(tiny_parser->current_node);
	  }
	  else
	  {
	   status = NW_STAT_FAILURE;
	  }
	}
  }

  if(status == NW_STAT_SUCCESS)
  {
   SET_STATE(tiny_parser, T_PARSE_S_CONTENT);
   tiny_parser->state &= ~T_PARSE_FLAG_TEXT; /* No longer accumulating text */
  }
  return status;
}//end Tag_End_CB (..)

/* 
* The first attribute is added as (first) child of the current node.
* This makes it easy to find attributes for later deserialization
*/

static 
NW_Status_t 
Attr_Start_CB (NW_WBXML_Parser_t *parser, 
               void *context)
{
  NW_Status_t status = NW_STAT_SUCCESS;
  
  NW_TinyTree_Node_t* attr_node;
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  switch(GET_STATE(tiny_parser)){
  case T_PARSE_S_TAG_START:
  case T_PARSE_S_ATTR_START:
  case T_PARSE_S_ATTR_VALS:
    if((tiny_parser->state & T_PARSE_FLAG_ATTR) != T_PARSE_FLAG_ATTR){
      tiny_parser->state |= T_PARSE_FLAG_ATTR;
      switch (GET_PASS(tiny_parser)){
      case T_PARSE_PASS_1:
        tiny_parser->node_count++;
        break;
      case T_PARSE_PASS_2:
        attr_node = NW_TinyTree_createChild(&(tiny_parser->dom_tree->tree), 
                                            tiny_parser->current_node, 
                                            (NW_TinyTree_Offset_t)parser->offset);
        if (attr_node != NULL){
          NW_TinyTree_Node_setUserFlags(attr_node, T_DOM_NODE_ATTR);
        }
        else{
          status = NW_STAT_OUT_OF_MEMORY;
        }                    
        break;
      default:
        NW_ASSERT(NW_FALSE);
        status = NW_STAT_FAILURE;
      }
    }
    SET_STATE(tiny_parser, T_PARSE_S_ATTR_START);
    break;
  default:
    status = NW_STAT_FAILURE;
  }
  return status;  
}

static 
NW_Status_t 
Attr_Val_CB (NW_WBXML_Parser_t *parser, 
             void *context)
{

  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  NW_REQUIRED_PARAM(parser);

  switch(GET_STATE(tiny_parser)){
  case T_PARSE_S_ATTR_START:
  case T_PARSE_S_ATTR_VALS:
    SET_STATE(tiny_parser,T_PARSE_S_ATTR_VALS);
    return NW_STAT_SUCCESS;
  default:
    return NW_STAT_FAILURE;
  }
}

static 
NW_Status_t 
Content_CB (NW_WBXML_Parser_t *parser, 
            void *context)
{

  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  SET_STATE(tiny_parser, T_PARSE_S_CONTENT);
  /* We need to save the current offset to use if we need a text node. Ugh! */
  tiny_parser->content_offset = (NW_TinyTree_Offset_t)parser->offset; 
  return NW_STAT_SUCCESS;
}

/* Count code page switches */

static
NW_Status_t 
Pass_1_CodePage_CB (NW_WBXML_Parser_t *parser, 
                    void *context)
{
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  NW_REQUIRED_PARAM(parser);

  tiny_parser->cp_count++;
  return NW_STAT_SUCCESS;
}


/* 
* The first text element in a series of text elements is added as
* a child to the current element. This may be followed by several more
* text elements in the source buffer.  
*/

static 
NW_Status_t
Text_CB (NW_WBXML_Parser_t *parser, 
         NW_Uint32 val, 
         void *context)
{
  NW_Status_t status = NW_STAT_SUCCESS;
  
  NW_TinyTree_Node_t* text_node;
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)context;

  NW_REQUIRED_PARAM(parser);
  NW_REQUIRED_PARAM(val);

  /*We're getting content but aren't yet accumulating text */
  if((GET_STATE(tiny_parser)==T_PARSE_S_CONTENT)
     &&((tiny_parser->state & T_PARSE_FLAG_TEXT) != T_PARSE_FLAG_TEXT)){

    switch(GET_PASS(tiny_parser)){
    case T_PARSE_PASS_1:
      tiny_parser->node_count++;
      break;
    case T_PARSE_PASS_2:
      /* Attach text as child to current node. */

      if(tiny_parser->current_node != NULL)
      {
       text_node = NW_TinyTree_createChild(&(tiny_parser->dom_tree->tree), 
                                          tiny_parser->current_node, 
                                          tiny_parser->content_offset);
       if (text_node != NULL){
         NW_TinyTree_Node_setUserFlags(text_node, T_DOM_NODE_TEXT);
       }
       else{
         status = NW_STAT_FAILURE;
       }      
      }
      else
      {
       status = NW_STAT_FAILURE;
      }  
      break;
    default:
      NW_ASSERT(NW_FALSE);
      status = NW_STAT_FAILURE;
    }
    tiny_parser->state |= T_PARSE_FLAG_TEXT;
  }
  return status;
}



static 
NW_Status_t 
Pass_1_InlineString_CB (NW_WBXML_Parser_t *parser, 
                        NW_Uint32 len, 
                        void *context)
{

  return Text_CB(parser, len, context);
  
}

/* 
 * Pass 2 text callback to adjust the NW_Byte ordering of inline strings if needed.
 * For some string encodings (such as ucs2) this allows the application to reference
 * them directly from the original storage //TODO: Add a flag to buffer
 */

static 
NW_Status_t 
Pass_2_InlineString_CB (NW_WBXML_Parser_t *parser, 
                        NW_Uint32 len, 
                        void *context)
{

  NW_String_t string;
  NW_TinyDom_Parser_t *tiny_parser = (NW_TinyDom_Parser_t*)parser->context;

  
  if(tiny_parser->dom_tree->doc->charset == HTTP_iso_10646_ucs_2)
    {
     NW_WBXML_Parser_getStringInline(parser, tiny_parser->dom_tree->doc, 
            &string);
     NW_String_Ucs2_ntoh(&string);
    }
  
  return Text_CB(parser, len, context);
}



static 
NW_Status_t 
Extension_CB (NW_WBXML_Parser_t *parser, 
              void *context){

  return Text_CB(parser, 0, context);

}


static const NW_WBXML_EventHandler_t Pass_1_Handler = {
  Pass_1_StartDocument_CB,
  Pass_1_EndDocument_CB,
  Tag_Start_CB, /* PI treated exactly like tag start in pass 1 */
  0,
  Tag_Start_CB,
  Tag_End_CB,
  Attr_Start_CB,
  Attr_Val_CB,
  Content_CB,
  Pass_1_CodePage_CB,
  Extension_CB,  
  0,
  0,
  Pass_1_InlineString_CB,
  Text_CB,
  Text_CB,
  Text_CB,
  Text_CB,
  0  /*TODO: Add exception callback */
};

static const NW_WBXML_EventHandler_t Pass_2_Handler = {
  Pass_2_StartDocument_CB,
  Pass_2_EndDocument_CB,
  Pass_2_Pi_CB,
  Tag_End_CB, /* PI end treated exactly like tag end in pass 2 */
  Tag_Start_CB,
  Tag_End_CB,
  Attr_Start_CB,
  Attr_Val_CB,
  Content_CB,
  0,
  Extension_CB,  
  0,
  0,
  Pass_2_InlineString_CB,
  Text_CB,
  Text_CB,
  Text_CB,
  Text_CB,
  0
};

void
NW_TinyDom_Tree_construct(NW_TinyDom_Tree_t* dom_tree,
                          NW_WBXML_Parser_t* parser,
                          NW_WBXML_Document_t* doc,
                          NW_WBXML_Writer_t* writer)
{
  NW_ASSERT(dom_tree != NULL);
  dom_tree->doc = doc;
  dom_tree->parser = parser;
  dom_tree->writer = writer;
  dom_tree->root_node = 0;
/*  dom_tree->tree = 0;*/
}

void
NW_TinyDom_Tree_destruct(NW_TinyDom_Tree_t* dom_tree)
{
  if (dom_tree != NULL) {
    NW_TinyTree_destruct(&(dom_tree->tree));
  }
}

EXPORT_C void
NW_TinyDom_Parser_construct(NW_TinyDom_Parser_t* dom_parser,
                            NW_TinyDom_Tree_t *dom_tree)
{
  dom_parser->state = 0;
  dom_parser->node_count = 0;
  dom_parser->cp_count = 0;
  dom_parser->current_node = 0;
  dom_parser->content_offset = 0;
  dom_parser->dom_tree = dom_tree;
}

/* Build the tree by running pass 1 and pass 2. */

/* TODO: ADD OUT OF MEMORY HANDLER!!!  */

NW_Status_t
NW_TinyDom_Parser_buildTree(NW_TinyDom_Parser_t *dom_parser, 
                            char *buffer, 
                            NW_Uint32 buffsize,
                            NW_Bool freeBuff)
{

  NW_Status_t status;
  NW_WBXML_CP_Registry_Entry_t* registry;
  NW_WBXML_Dictionary_t* dictionary = NULL;
  NW_String_t string;

  /* Run pass 1 (count nodes and code page switches) */

  status = NW_WBXML_Parser_registerHandler(dom_parser->dom_tree->parser, 
                                           &Pass_1_Handler, 
                                           (void *) dom_parser);
  if (status != NW_STAT_SUCCESS){
    return status;
  }
  status = NW_WBXML_Parser_parseBuffer (dom_parser->dom_tree->parser, 
                                        dom_parser->dom_tree->doc, 
                                        (NW_Byte *) buffer, buffsize);

  if (status != NW_STAT_SUCCESS){
    return status;
  }

  /* Initialize the writer */

  /* First get the dictionary */
  dictionary = NW_WBXML_Dictionary_getByIndex(dom_parser->dom_tree->parser->dictionary);
  
  if(dictionary == NULL){
    return NW_STAT_FAILURE;
  }
  
  /* Initialize the writer */

  NW_WBXML_Writer_Initialize(dom_parser->dom_tree->writer,
                             0, NULL,
                             NULL,
                             dictionary,
                             dictionary,
                             NW_Encoder_StringTable_getStringTableOffset,
                             NW_Encoder_StringTable_addToStringTable,
                             dom_parser->dom_tree->doc->strtbl_extension,
                             NW_Encoder_StringTable_StringTableIterateInit,
                             NW_Encoder_StringTable_StringTableIterateNext,
                             NW_TRUE  /* sizing only pass */);

  /* Setup the tree and any code page registry */

  status = NW_TinyTree_construct(&(dom_parser->dom_tree->tree),
                        (CXML_Vector_Metric_t)(dom_parser->node_count/4 + 2),
                        buffer,
                        buffsize,
                        (void*)dom_parser,
                        freeBuff);
  if (status != NW_STAT_SUCCESS) {
    return status;
  }

  /* TODO: move all of this down into the wbxml parser itself */
  if(dom_parser->cp_count > 0){
    /* There are code page switches, so add a code page registry */
    registry = (NW_WBXML_CP_Registry_Entry_t*) 
      NW_Mem_Malloc((dom_parser->cp_count + 1) * sizeof (NW_WBXML_CP_Registry_Entry_t));
    if (registry == NULL){
      return NW_STAT_OUT_OF_MEMORY;
    }

    NW_Mem_memset(registry, 0, (dom_parser->cp_count + 1) * sizeof (NW_WBXML_CP_Registry_Entry_t) );


    status = NW_WBXML_Parser_addCPRegistry(dom_parser->dom_tree->parser, 
                                           registry, dom_parser->cp_count);
    if (status != NW_STAT_SUCCESS){
      return status;
    }
  }
  /* Run pass 2 (build tree and any code page registry) */
  NW_WBXML_Parser_reset (dom_parser->dom_tree->parser); 
  NW_WBXML_Parser_registerHandler (dom_parser->dom_tree->parser, 
                                   &Pass_2_Handler, 
                                   (void*) dom_parser);
  if(dom_parser->dom_tree->doc->charset == HTTP_iso_10646_ucs_2){
    string.storage = dom_parser->dom_tree->doc->strtbl.data;
    string.length = dom_parser->dom_tree->doc->strtbl.length;

    if (string.length)
      NW_String_Ucs2_ntoh(&string);
  }
  /* set last valid to undefined for non-appending dom tree parsing */
  NW_TinyDom_setLastValid(dom_parser->dom_tree, -1);
  status = NW_WBXML_Parser_parseBuffer(dom_parser->dom_tree->parser, 
                                       dom_parser->dom_tree->doc, 
                                       (NW_Byte *) buffer, 
                                       buffsize);
  if (status != NW_STAT_SUCCESS){
    return status;
  }

  return NW_STAT_SUCCESS;
}

/**********************************************************
*/
NW_Status_t
NW_TinyDom_Parser_incrementalBuildTree(
                                       NW_TinyDom_Parser_t *dom_parser, 
                                       char *buffer, 
                                       NW_Uint32 buffsize,
                                       NW_Bool freeBuff,
                                       NW_Int32 lastValid)
{
  
  NW_Status_t status;
  NW_WBXML_CP_Registry_Entry_t *old_registry, *registry;
  NW_WBXML_Dictionary_t* dictionary = NULL;
  NW_Uint32 old_cp_offset = 0;
  NW_String_t string;

  if (dom_parser->dom_tree->tree.tree == NULL)
  {
    status = NW_TinyTree_construct(&(dom_parser->dom_tree->tree),
      (CXML_Vector_Metric_t)(10),/*todo: remove hardcode number */
      buffer,
      buffsize,
      (void*)dom_parser,
      freeBuff);
    if (status != NW_STAT_SUCCESS) {
      goto finish_ibuildtree;
    }
  }
  /* Initialize the writer */
  if (dom_parser->dom_tree->parser->dictionary == 0)
  {
    dom_parser->dom_tree->parser->p = (NW_Byte *)buffer;
    dom_parser->dom_tree->parser->left = buffsize;
    if ((status = NW_WBXML_Parser_docHeaderParse (dom_parser->dom_tree->parser, dom_parser->dom_tree->doc))
      != NW_STAT_SUCCESS)
      goto finish_ibuildtree;
    
    /* Now get the dictionary from the document */
    
    if (dom_parser->dom_tree->doc->publicid > 0){
      dom_parser->dom_tree->parser->dictionary =
        NW_WBXML_Dictionary_getIndexByPublicId (dom_parser->dom_tree->doc->publicid);
    }
    
    else if (dom_parser->dom_tree->doc->doc_type){
      dom_parser->dom_tree->parser->dictionary =
        NW_WBXML_Dictionary_getIndexByDocType (dom_parser->dom_tree->doc->doc_type, dom_parser->dom_tree->doc->charset);
    }
    
    /* If a dictionary could not be attained try using the default public id */
    if (dom_parser->dom_tree->parser->dictionary == 0){
      dom_parser->dom_tree->doc->publicid = dom_parser->dom_tree->doc->default_public_id;
      dom_parser->dom_tree->parser->dictionary =
        NW_WBXML_Dictionary_getIndexByPublicId (dom_parser->dom_tree->doc->publicid);
    }
    
    /* Make the StartDocument callback */
    /* Run pass 2 (build tree and any code page registry) */
    NW_WBXML_Parser_registerHandler (dom_parser->dom_tree->parser, 
      &Pass_2_Handler, 
      (void*) dom_parser);
    SET_PASS(dom_parser, T_PARSE_PASS_2);
    
    if (dom_parser->dom_tree->parser->handler && dom_parser->dom_tree->parser->handler->StartDocument_CB)
    {
      status = (*(dom_parser->dom_tree->parser->handler->StartDocument_CB)) (dom_parser->dom_tree->parser, 
        dom_parser->dom_tree->doc, dom_parser);
      if (status != NW_STAT_SUCCESS)
        goto finish_ibuildtree;
    }  
    
    /* Initialize the writer */
    NW_WBXML_Writer_Initialize(dom_parser->dom_tree->writer,
      0, NULL,
      NULL,
      dictionary,
      dictionary,
      NW_Encoder_StringTable_getStringTableOffset,
      NW_Encoder_StringTable_addToStringTable,
      dom_parser->dom_tree->doc->strtbl_extension,
      NW_Encoder_StringTable_StringTableIterateInit,
      NW_Encoder_StringTable_StringTableIterateNext,
      NW_TRUE  /* sizing only pass */);
  }
  else
  {
    dom_parser->dom_tree->parser->p = (NW_Byte *)buffer + dom_parser->dom_tree->parser->offset;
    dom_parser->dom_tree->parser->left = buffsize - dom_parser->dom_tree->parser->offset;
    
    // copy the previous Ebuffer to overwrite the previous part of wbxml buffer.
    // since the previous part has native endianess, the conversion of endianess 
    // (big endian to little endian) only occurs to new contents.
    (void)NW_Mem_memcpy( 
      buffer, dom_parser->dom_tree->tree.ebuffer->segmentList->storage,
      dom_parser->dom_tree->parser->offset);

  }
  /* First get the dictionary */
  dictionary = NW_WBXML_Dictionary_getByIndex(dom_parser->dom_tree->parser->dictionary);
  
  if(dictionary == NULL){
    status = NW_STAT_FAILURE;
    goto finish_ibuildtree;
  }
  
  /* Setup the tree and any code page registry */
  
  
  NW_TinyDom_setLastValid(dom_parser->dom_tree, lastValid);
  
  /* TODO: move all of this down into the wbxml parser itself */
  if(dom_parser->cp_count > 0){
    /* There are code page switches, so add a code page registry */
    old_registry = dom_parser->dom_tree->parser->cp_registry.storage;
    registry = (NW_WBXML_CP_Registry_Entry_t*)NW_Mem_Malloc((dom_parser->cp_count + 1) * sizeof (NW_WBXML_CP_Registry_Entry_t));
    if (registry == NULL){
      status = NW_STAT_OUT_OF_MEMORY;
      goto finish_ibuildtree;
    }

    NW_Mem_memset(registry, 0, ( (dom_parser->cp_count + 1) * sizeof (NW_WBXML_CP_Registry_Entry_t) ) );
    if (old_registry)
    {
      old_cp_offset = dom_parser->dom_tree->parser->cp_registry.current - dom_parser->dom_tree->parser->cp_registry.storage;
      NW_Mem_memcpy(registry, old_registry, 
        dom_parser->dom_tree->parser->cp_registry.count*sizeof(NW_WBXML_CP_Registry_Entry_t));
      NW_Mem_Free(old_registry);
    }
    status = NW_WBXML_Parser_addCPRegistry(dom_parser->dom_tree->parser, 
      registry, dom_parser->cp_count);
    if (status != NW_STAT_SUCCESS){
      goto finish_ibuildtree;
    }
    dom_parser->dom_tree->parser->cp_registry.current = registry + old_cp_offset;
    // WLIU_DEBUG: dom_parser->dom_tree->parser->cp_registry.realcount = old_cp_offset;
    /* Make sure flag is not set, so that new cp entry could be added to the registry */
    dom_parser->dom_tree->parser->flags &= ~NW_WBXML_REGISTRY_INIT; 
  }
  
  //NW_WBXML_Parser_reset (dom_parser->dom_tree->parser); 
  
  /* Run pass 2 (build tree and any code page registry) */
  NW_WBXML_Parser_registerHandler (dom_parser->dom_tree->parser, 
    &Pass_2_Handler, 
    (void*) dom_parser);
  SET_PASS(dom_parser, T_PARSE_PASS_2);
  SET_STATE(dom_parser, T_PARSE_S_CONTENT);
  dom_parser->state &= ~T_PARSE_FLAG_TEXT; /* prepare for text */
  
  // convert endianess if necessary
  if(dom_parser->dom_tree->doc->charset == HTTP_iso_10646_ucs_2){
    string.storage = dom_parser->dom_tree->doc->strtbl.data;
    string.length = dom_parser->dom_tree->doc->strtbl.length;
    if (string.length)
      NW_String_Ucs2_ntoh(&string);
  }

  NW_TinyTree_EBuffer_Destruct (dom_parser->dom_tree->tree.ebuffer );
  
  dom_parser->dom_tree->tree.ebuffer = NW_TinyTree_EBuffer_Construct((NW_Byte *)buffer, 
    buffsize, 
    NW_TINY_TREE_BLOCK_SIZE_DEFAULT,
    freeBuff);
  
  status = NW_WBXML_Parser_parseBuffer(dom_parser->dom_tree->parser, 
    dom_parser->dom_tree->doc, 
    (NW_Byte *) buffer, 
    buffsize);
  //WLIU_DEBUG: dom_parser->dom_tree->parser->lastValid = NW_TinyDom_getLastValid(dom_parser->dom_tree);
  if (status != NW_STAT_SUCCESS){
    goto finish_ibuildtree;
  }
  dom_parser->dom_tree->parser->offset = NW_TinyDom_getLastValid(dom_parser->dom_tree);
finish_ibuildtree:
  return status;
}

/*****************************************************************************
*/
/* Get a pointer to the dom parser from the tiny tree */

EXPORT_C NW_TinyDom_Parser_t*
NW_TinyDom_getParser(NW_TinyTree_t *tree)

{
  return (NW_TinyDom_Parser_t*)NW_TinyTree_getContext(tree);
}

/* Get a pointer to the dom tree from a tiny tree */

EXPORT_C NW_TinyDom_Tree_t*
NW_TinyDom_getTree(NW_TinyTree_t *tree)

{

  NW_TinyDom_Parser_t *dom_parser;

  dom_parser = NW_TinyDom_getParser(tree);
  if(dom_parser != NULL){
    return dom_parser->dom_tree;
  }
  return NULL;
}

/* Get the doc header from the tiny tree */

NW_WBXML_Document_t *
NW_TinyDom_getDocHeader(NW_TinyTree_t *tree)

{
  NW_TinyDom_Tree_t *dom_tree = NW_TinyDom_getTree(tree);

  if(dom_tree != NULL){
    return dom_tree->doc;
  }
  return NULL;
}

/* Deserialization API */

NW_Uint16
NW_TinyDom_Node_getType(NW_TinyTree_Node_t* node){
  return (NW_Uint16)(NW_TinyTree_Node_getFlags(node) & TNODE_USR_FLAGS);
}

/* Deserialization callbacks */


/* 
* Tag callbacks 
* TODO: Combine these with the rest of the pass 3 callbacks
*/

static 
NW_Status_t 
T_Tag_Start_CB (NW_WBXML_Parser_t *parser, 
                void *context)
{
  NW_TinyDom_Tag_t *tag = (NW_TinyDom_Tag_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = tag->tiny_parser;

  NW_REQUIRED_PARAM(parser);

  SET_STATE(tiny_parser, T_PARSE_S_TAG_START);
  return NW_STAT_SUCCESS;
}

static 
NW_Status_t 
T_FQToken_CB (NW_WBXML_Parser_t *parser, 
              NW_Uint32 token, 
              void *context){

  NW_TinyDom_Tag_t *tag = (NW_TinyDom_Tag_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = tag->tiny_parser;

  NW_REQUIRED_PARAM(parser);

  switch (GET_STATE(tiny_parser)){ 
  case T_PARSE_S_TAG_START:
    tag->fq_token = token;
    break;
  default:
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_SUCCESS;
}

static 
NW_Status_t 
T_TableString_CB (NW_WBXML_Parser_t *parser, 
                  NW_Uint32 index, 
                  void *context)
{
  NW_TinyDom_Tag_t *tag = (NW_TinyDom_Tag_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = tag->tiny_parser;

  NW_REQUIRED_PARAM(parser);

  switch (GET_STATE(tiny_parser)){ 
  case T_PARSE_S_TAG_START:
    tag->name_index = index;
    break;
  default:
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_SUCCESS;
}

static const NW_WBXML_EventHandler_t Tag_Handler = {
  0,
  0,
  0,
  0,
  T_Tag_Start_CB,
  0,
  0,
  0,
  0,
  0,
  0,  
  0,
  T_FQToken_CB,
  0,
  T_TableString_CB,
  0,
  0,
  0,
  0
};


/* 
* Pass 3 callbacks. These are called by the parser when deserializing
* text and attributes. Since text elements are just a subset of
* attribute elements, almost exactly the same callbacks can be used
* for both.
*
* Note that parser state between callbacks is only meaningful if the
* callbacks are part of an atomic sequence. Such sequences always
* start with an Attr_Start, Attr_Val or Content callback, followed by
* one of the data type callbacks. Any attempt to use the parser state
* machine in another way is not reentrant.  
*/

static 
NW_Status_t 
Pass_3_Attr_Start_CB (NW_WBXML_Parser_t *parser, 
                      void *context)
{
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;

  NW_REQUIRED_PARAM(parser);

  SET_STATE(tiny_parser, T_PARSE_S_ATTR_START);
  return NW_STAT_SUCCESS;
}

static 
NW_Status_t 
Pass_3_Attr_Val_CB (NW_WBXML_Parser_t *parser, 
                    void *context)
{
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;

  NW_REQUIRED_PARAM(parser);

  SET_STATE(tiny_parser, T_PARSE_S_ATTR_VALS);
  return NW_STAT_SUCCESS;
}

static 
NW_Status_t 
Pass_3_Content_CB (NW_WBXML_Parser_t *parser, 
                   void *context)
{
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;

  NW_REQUIRED_PARAM(parser);

  SET_STATE(tiny_parser, T_PARSE_S_CONTENT);
  return NW_STAT_SUCCESS;
}


static 
NW_Status_t 
Pass_3_Extension_CB (NW_WBXML_Parser_t *parser, 
                     void *context)
{
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;

  NW_REQUIRED_PARAM(parser);

  /* Set the extension substate flag */
  tiny_parser->state |= T_PARSE_F_EXT;
  return NW_STAT_SUCCESS;
}


static 
NW_Status_t 
Pass_3_FQToken_CB (NW_WBXML_Parser_t *parser, 
                   NW_Uint32 token, 
                   void *context)
{
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;
  NW_TinyDom_AttrVal_t* attr_val = (NW_TinyDom_AttrVal_t*)(handle->value);

  NW_REQUIRED_PARAM(parser);

  switch (GET_STATE(tiny_parser)){ 
  case T_PARSE_S_ATTR_START:
    handle->fq_token = token;
    break;
  case T_PARSE_S_ATTR_VALS:
  case T_PARSE_S_CONTENT:
    if((tiny_parser->state & T_PARSE_F_EXT) == T_PARSE_F_EXT){
      attr_val->type = NW_WBXML_ATTR_COMPONENT_EXT;
      attr_val->component.ext.type
        = (NW_Uint8)((tiny_parser->dom_tree->parser->ext_t_not_table_index) ?
           NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER
           : NW_TINYDOM_EXTENSION_TYPE_NORMAL);
      attr_val->component.ext.token = token;
      attr_val->component.ext.value.string.length = 0;
      attr_val->component.ext.value.string.storage = NULL;
    }
    else{ /*TODO: NW_ASSERT that this is not called with T_PARSE_S_ATTR_CONTENT? */
      attr_val->type = NW_WBXML_ATTR_COMPONENT_TOKEN;
      attr_val->component.value_token = token;
    }
    break;
  default:
    break;
  }
  return NW_STAT_SUCCESS;
}

static 
NW_Status_t 
Pass_3_InlineString_CB (NW_WBXML_Parser_t *parser, 
                        NW_Uint32 len, 
                        void *context)
{
  NW_Status_t status;
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;
  NW_TinyDom_AttrVal_t* attr_val = (NW_TinyDom_AttrVal_t*)(handle->value);

  NW_REQUIRED_PARAM(len);

  switch (GET_STATE(tiny_parser)){ 
  case T_PARSE_S_ATTR_VALS:
  case T_PARSE_S_CONTENT:
    if((tiny_parser->state & T_PARSE_F_EXT) == T_PARSE_F_EXT){
      status = NW_WBXML_Parser_getStringInline(parser, 
                                               tiny_parser->dom_tree->doc, 
                                               &(attr_val->component.ext.value.string));
      if(status != NW_STAT_SUCCESS){
        return status;
      }
      tiny_parser->state &= ~T_PARSE_F_EXT; /* Unset extension sub-state */
    }
    else{ 
      attr_val->type = NW_WBXML_ATTR_COMPONENT_STRING;
      status = NW_WBXML_Parser_getStringInline(parser, 
                                               tiny_parser->dom_tree->doc, 
                                               &(attr_val->component.string)); 
      if(status != NW_STAT_SUCCESS){
        return status;
      }

    }
    break;
  default:
    break;
  }
  return NW_STAT_SUCCESS;
}

static 
NW_Status_t 
Pass_3_TableString_CB (NW_WBXML_Parser_t *parser, 
                       NW_Uint32 index, 
                       void *context)
{
  NW_Status_t status;
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;
  NW_TinyDom_AttrVal_t* attr_val = (NW_TinyDom_AttrVal_t*)(handle->value);

  NW_REQUIRED_PARAM(parser);

  switch (GET_STATE(tiny_parser)){
  case T_PARSE_S_ATTR_START:
    handle->name_index = index;
    break;
  case T_PARSE_S_ATTR_VALS:
  case T_PARSE_S_CONTENT:
    if((tiny_parser->state & T_PARSE_F_EXT) == T_PARSE_F_EXT){
      status = NW_WBXML_Document_getTableString(tiny_parser->dom_tree->doc, 
                                                index, 
                                                &(attr_val->component.ext.value.string));
      if(status != NW_STAT_SUCCESS){
        return status;
      }
      tiny_parser->state &= ~T_PARSE_F_EXT; /* Unset extension sub-state */
    }
    else{ 
      attr_val->type = NW_WBXML_ATTR_COMPONENT_STRING;
      status = NW_WBXML_Document_getTableString(tiny_parser->dom_tree->doc, 
                                                index, 
                                                &(attr_val->component.string));
      if(status != NW_STAT_SUCCESS){
        return status;
      } 
    }
    break;
  default:
    break;
  }
  return NW_STAT_SUCCESS;
}


/* TODO: Fill in. This is to get inline integers */

static 
NW_Status_t 
Pass_3_Binary_CB (NW_WBXML_Parser_t *parser, 
                  NW_Uint32 x,
                  void *context)
{
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;

  NW_REQUIRED_PARAM(parser);

  if ((tiny_parser->state & T_PARSE_F_EXT) == T_PARSE_F_EXT) {
    NW_TinyDom_AttrVal_t* attr_val = (NW_TinyDom_AttrVal_t*)(handle->value);
    attr_val->component.ext.value.x = x;
  }
  tiny_parser->state &= ~T_PARSE_F_EXT; /* Unset extension sub-state */
  return NW_STAT_SUCCESS;
}

static 
NW_Status_t 
Pass_3_Opaque_CB (NW_WBXML_Parser_t *parser, 
                  NW_Uint32 len, 
                  void *context)
{
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;  
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;
  NW_TinyDom_AttrVal_t* attr_val = (NW_TinyDom_AttrVal_t*)(handle->value);

  switch (GET_STATE(tiny_parser)){ 
  case T_PARSE_S_ATTR_VALS:
  case T_PARSE_S_CONTENT:
    attr_val->type = NW_WBXML_ATTR_COMPONENT_OPAQUE;
    NW_WBXML_Parser_getOpaque(parser, len, &(attr_val->component.opaque)); 
    break;
  default:
      break;
  }
  return NW_STAT_SUCCESS;
}

static NW_Status_t 
Pass_3_Entity_CB (NW_WBXML_Parser_t *parser, 
                  NW_Uint32 e, 
                  void *context)
{
  NW_TinyDom_AttributeHandle_t *handle = (NW_TinyDom_AttributeHandle_t*)context;
  NW_TinyDom_Parser_t *tiny_parser = handle->tlit.tiny_parser;
  NW_TinyDom_AttrVal_t* attr_val = (NW_TinyDom_AttrVal_t*)(handle->value);

  NW_REQUIRED_PARAM(parser);

  switch (GET_STATE(tiny_parser)){ 
  case T_PARSE_S_ATTR_VALS:
  case T_PARSE_S_CONTENT:
    attr_val->type = NW_WBXML_ATTR_COMPONENT_ENTITY;
    attr_val->component.entity = e;
    break;
  default:
    break;
  }
  return NW_STAT_SUCCESS;
}


static const NW_WBXML_EventHandler_t Pass_3_Handler = {
  0,
  0,
  0,
  0,
  0,
  0,
  Pass_3_Attr_Start_CB,
  Pass_3_Attr_Val_CB,
  Pass_3_Content_CB,
  0,
  Pass_3_Extension_CB,  
  0,
  Pass_3_FQToken_CB,
  Pass_3_InlineString_CB,
  Pass_3_TableString_CB,
  Pass_3_Binary_CB,
  Pass_3_Opaque_CB,
  Pass_3_Entity_CB,
  0
};


/*
* The tiny list iterator is a generic iterator that 
* can be used to invoke parser iterator methods.
*/

static void
TinyDom_ListIterator_init(NW_TinyDom_ListIterator_t *it, 
                          NW_TinyDom_Parser_t *tiny_parser, 
                          NW_TinyTree_Node_t *list_node, 
                          void *context){
  it->tiny_parser = tiny_parser;
  NW_TinyTree_Node_GetSegmentAndOffset(&(tiny_parser->dom_tree->tree),list_node, (NW_Uint8**)&(it->segment), &(it->segSize), &(it->offset));
  it->state = NW_STAT_WBXML_ITERATE_MORE;
  it->context = context;
}

/*
*
* This is a bit complicated: you pass in an iterator function (from
* the parser) that knows how to iterate the type you are interested
* in (e.g. attribute_iterate), and you pass in a handler that the
* parser iterator's callbacks will invoke.  When called, a handler
* receives the interator context field as the context arg.  The
* handler is used to select elements, gather data, etc.  
*
* This, NW_TinyDom_attribute_handle_init, and NW_TinyDom_tag_get, should
* not be called reentrantly (from a parser callback). This doesn't
* seem like a serious restriction. If this becomes a problem, the
* ParserSaveContext/ParserRestoreContext lines can be uncommented to
* make these reentrant.
*/

static 
NW_TinyTree_Offset_t
TinyDom_ListIterator_iterate(NW_TinyDom_ListIterator_t *it, 
                             const NW_WBXML_EventHandler_t *handler, 
                             NW_Status_t (*type_iterator) (NW_WBXML_Parser_t*))
{
  NW_TinyTree_Offset_t startOffset;
  NW_TinyTree_Offset_t sourceOffset;

  
  if (it->state == NW_STAT_WBXML_ITERATE_MORE){
   

    /* Set the parser buffer and buffer size to the values stored in the iterator */
    NW_WBXML_Parser_resetBuffer(it->tiny_parser->dom_tree->parser, 
                                (NW_Byte*) it->segment, 
                                it->segSize);
    
    /* Register the callbacks */
    NW_WBXML_Parser_registerHandler (it->tiny_parser->dom_tree->parser, 
                                     handler, 
                                     it->context);
    startOffset = it->offset;
    /* Now set the parser offset to the value stored in the iterator */
    NW_WBXML_Parser_setOffset(it->tiny_parser->dom_tree->parser, it->offset);
    /* Run the type iterator */
    it->state = (NW_Uint32)(*(type_iterator))(it->tiny_parser->dom_tree->parser);
    /* Set the new iterator offset (the segment should be unchanged)*/
    it->offset = 
      (NW_TinyTree_Offset_t)NW_WBXML_Parser_getOffset(it->tiny_parser->dom_tree->parser);

    if((it->tiny_parser->dom_tree->parser->status != NW_STAT_SUCCESS) ||
       (it->offset == startOffset)){ /* Hack, hack . . The parser didn't advance. */
        return 0;
    }
 
    /* Convert the iterator offset into a source offset to return */

    NW_TinyTree_GetSourceOffset(&(it->tiny_parser->dom_tree->tree), 
                                (NW_Byte*) it->segment, startOffset, &sourceOffset);
    return sourceOffset;
  }
  return 0;
}

void
NW_TinyDom_AttrListHandle_init(NW_TinyDom_AttrListHandle_t *it, 
                               NW_TinyDom_Parser_t *tiny_parser, 
                               NW_TinyTree_Node_t *list_node)
{
  TinyDom_ListIterator_init(it, tiny_parser, list_node, 0); 
}

/* 
* Iterate through a list of attributes. There are
* no attribute list callbacks since we just want to iterate the
* attribute list, returning the start of each attribute.  
*/

NW_TinyTree_Offset_t
NW_TinyDom_AttrListHandle_iterate(NW_TinyDom_AttrListHandle_t *it){
  /* Call list_iterate with a null handler.*/
  return TinyDom_ListIterator_iterate(it, 0, NW_WBXML_Parser_attributeListIterate);
}

/*
* Internal function to iterate the values of an attribute.
*/

NW_TinyTree_Offset_t
NW_TinyDom_AttributeHandle_valsIterate(NW_TinyDom_ListIterator_t *it)
{
  return TinyDom_ListIterator_iterate(it, &Pass_3_Handler, NW_WBXML_Parser_attributeValsIterate);
}

/*String indexes must be less than the largest offset */
#define ILLEGAL_STRING_INDEX ((NW_TinyTree_Offset_t)~0) 

/*
* Initialize an attribute handle by parsing an attribute's start
* token and values, calling the attribute handlers.   
*/

void 
NW_TinyDom_AttributeHandle_init(NW_TinyDom_AttributeHandle_t *handle, 
                                NW_TinyDom_Parser_t *tiny_parser, 
                                NW_TinyTree_Offset_t offset)
{
  /*lint --e{794} Conceivable use of null pointer */

  NW_TinyTree_Node_t dummy_node;
  void *segment;
  NW_TinyTree_Offset_t segSize;
  NW_TinyTree_Offset_t segOffset;

  handle->fq_token = 0;
  handle->name_index = ILLEGAL_STRING_INDEX;
  handle->value = NULL;
  /* Run parser once to get token, name */
  handle->tlit.tiny_parser = tiny_parser;

  /* Fill in a dummy node with the supplied source offset */
  dummy_node.source_offset = offset;

  /* Use this to initialize the parser */
  NW_TinyTree_Node_GetSegmentAndOffset(&(tiny_parser->dom_tree->tree), 
                                       &dummy_node, (NW_Uint8**)&segment, &segSize, &segOffset);

  NW_WBXML_Parser_resetBuffer(tiny_parser->dom_tree->parser, (NW_Byte*) segment, segSize);
  
  NW_WBXML_Parser_registerHandler (tiny_parser->dom_tree->parser, &Pass_3_Handler, handle);
  NW_WBXML_Parser_setOffset(tiny_parser->dom_tree->parser, segOffset);
  /* Run parser once to get past name */
  NW_WBXML_Parser_attributeNameParse(tiny_parser->dom_tree->parser);

 /* Update the dummy node source offset to reflect the parser advance */
  segOffset = 
      (NW_TinyTree_Offset_t)NW_WBXML_Parser_getOffset(tiny_parser->dom_tree->parser);

  NW_TinyTree_GetSourceOffset(&(tiny_parser->dom_tree->tree), (NW_Byte*) segment, 
                              segOffset, &(dummy_node.source_offset));

  TinyDom_ListIterator_init((NW_TinyDom_ListIterator_t*)&(handle->tlit), 
                            tiny_parser, 
                            &dummy_node, 
                            handle);
}

/* Get tag name and token */

static 
void
NW_TinyDom_getTag(NW_TinyDom_Parser_t* tiny_parser, 
                  NW_TinyTree_Node_t* node, 
                  NW_TinyDom_Tag_t* tag)
{
  NW_WBXML_Parser_t *parser = tiny_parser->dom_tree->parser;
  void *segment;
  NW_TinyTree_Offset_t segSize;
  NW_TinyTree_Offset_t segOffset;

  /*NW_WBXML_Parser_t saved_context; */

  tag->tiny_parser = tiny_parser;
  /*NW_WBXML_ParserSaveContext(parser, &saved_context); */
  
  /* Convert the node source offset to segment and offset form */
  NW_TinyTree_Node_GetSegmentAndOffset(&(tiny_parser->dom_tree->tree), 
                                       node, (NW_Uint8**)&segment, &segSize, &segOffset);

  /* Use these values to init the parser */
  NW_WBXML_Parser_resetBuffer(tiny_parser->dom_tree->parser, (NW_Byte*) segment, segSize);
  NW_WBXML_Parser_setOffset(parser, segOffset);
  NW_WBXML_Parser_registerHandler (parser, &Tag_Handler, tag);
  /* Parse the tag name */
  NW_WBXML_Parser_tagNameParse(parser);
  /*NW_WBXML_ParserRestoreContext(parser, &saved_context); */
}

EXPORT_C NW_Uint32
NW_TinyDom_getTagToken(NW_TinyDom_Parser_t* tiny_parser, 
                       NW_TinyTree_Node_t* node)
{
  NW_TinyDom_Tag_t tag;
  NW_TinyDom_getTag(tiny_parser, node, &tag);
  return tag.fq_token;
}


NW_Status_t
NW_TinyDom_getTagName(NW_TinyDom_Parser_t* tiny_parser, 
                      NW_TinyTree_Node_t* node, 
                      NW_String_t* name)
{
  NW_TinyDom_Tag_t tag;

  tag.name_index = ILLEGAL_STRING_INDEX;
  NW_TinyDom_getTag(tiny_parser, node, &tag);
  
  if(tag.name_index != ILLEGAL_STRING_INDEX){
    /*TODO: replace ??*/
    return NW_WBXML_Document_getTableString(tiny_parser->dom_tree->doc, 
                                            tag.name_index, 
                                            name); 
  }
  return NW_STAT_WBXML_NO_NAME;
}

NW_Uint32
NW_TinyDom_AttributeHandle_getToken(NW_TinyDom_AttributeHandle_t *handle)
{
  return handle->fq_token;
}

/* getName is going to return the entire "start token" which might
   include a value prefix */
NW_Status_t
NW_TinyDom_AttributeHandle_getName(NW_TinyDom_AttributeHandle_t *handle,  
                                   NW_String_t *name)
{
  NW_TinyDom_Parser_t * tiny_parser = handle->tlit.tiny_parser;

  if(handle->name_index != ILLEGAL_STRING_INDEX){
    /*TODO: replace?? */
    return NW_WBXML_Document_getTableString(tiny_parser->dom_tree->doc, 
                                            handle->name_index, 
                                            name);
  }
  return NW_STAT_WBXML_NO_NAME;
}

NW_TinyTree_Offset_t
NW_TinyDom_AttributeHandle_iterateValues(NW_TinyDom_AttributeHandle_t* handle, 
                                         NW_TinyDom_AttrVal_t *value)
{
  handle->value = value; /* Set up handle to receive value.*/
  return NW_TinyDom_AttributeHandle_valsIterate((NW_TinyDom_ListIterator_t*)&(handle->tlit));
}


/* Init a text handle */ 

void 
NW_TinyDom_TextHandle_init(NW_TinyDom_TextHandle_t* handle, 
                           NW_TinyDom_Parser_t* tiny_parser, 
                           NW_TinyTree_Offset_t offset)
{
  NW_TinyTree_Node_t dummy_node;

  dummy_node.source_offset = offset; 
  /* Initialize the iterator with this handle as the context */
  TinyDom_ListIterator_init((NW_TinyDom_ListIterator_t *)&(handle->tlit), 
                            tiny_parser, 
                            &dummy_node, 
                            handle);
  handle->value = NULL;
}

/* Iterate through the text items in a text element, calling the text handlers */

NW_TinyTree_Offset_t
NW_TinyDom_TextHandle_iterate(NW_TinyDom_TextHandle_t *handle, 
                              NW_TinyDom_Text_t *item)
{
  handle->value = item;
  return TinyDom_ListIterator_iterate(&(handle->tlit), 
    &Pass_3_Handler, 
    NW_WBXML_Parser_textIterate);
}

void
NW_TinyDom_setLastValid(NW_TinyDom_Tree_t* dom_tree, NW_Int32 lastValid)
{
  NW_ASSERT(dom_tree->tree.tree != NULL);
  dom_tree->tree.tree->lastValid = lastValid;
}


NW_Int32
NW_TinyDom_getLastValid(NW_TinyDom_Tree_t* dom_tree)
{
  if (dom_tree->tree.tree == NULL)
    return 0;
  return dom_tree->tree.tree->lastValid;
}











