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


/** ----------------------------------------------------------------------- **
    @package:     NW_DOM

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/
#ifndef NW_DOM_DOCUMENT_H
#define NW_DOM_DOCUMENT_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_dom_node.h>
#include <xml/cxml/nw_dom_text.h>
#include <xml/cxml/nw_dom_element.h>
#include <xml/cxml/nw_wbxml_dictionary.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_getPublicIdAsNumber

    @synopsis:    Gets the public id of the document as an integer.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* d
                  The document node.

    @description: Gets the public id of the document as an integer.

    @returns:     NW_Uint32
                  publicID or 0

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint32 
NW_DOM_DocumentNode_getPublicIdAsNumber(NW_DOM_DocumentNode_t* d);


/** ----------------------------------------------------------------------- **

    @function:    NW_DOM_DocumentNode_getPublicId

    @synopsis:    Gets the publicid of the document as string (docType).

    @scope:       public      

    @parameters: 
       [in]  NW_DOM_DocumentNode_t* d
                  The document node.

       [out] NW_String_t* str
                  The public id as a string.

    @description: The document type declaration associated with this 
                  document is returned.

    @returns:     NW_Status_t 
                  Status of operation.
                 
       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Error reading node.

       [NW_STAT_SUCCESS]
                  Success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_DOM_DocumentNode_getPublicId(NW_DOM_DocumentNode_t* d, 
                                NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_getVersion

    @synopsis:    Returns the version of the document, 

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

    @description: Returns the version of the document. Returns 0 if this
                  is not valid.

    @returns:     NW_Uint8 
                  Version number or 0.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint8 
NW_DOM_DocumentNode_getVersion(NW_DOM_DocumentNode_t* doc);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_getCharacterEncoding

    @synopsis:    Returns the supported encoding of the document.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

    @description: Returns the supported encoding of the document or 0 
                  for invalid document.

    @returns:     NW_Uint32  
                  Encoding or 0.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint32 
NW_DOM_DocumentNode_getCharacterEncoding(NW_DOM_DocumentNode_t* doc);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_getDocumentElement

    @synopsis:    Returns child node that is the root of this document

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

    @description: Returns child node that is the root of this document

    @returns:     NW_DOM_DocumentNode_t* 
                  Child node that is the root element of the document; 
                  NULL if doc is not a document node.
                  
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_ElementNode_t* 
NW_DOM_DocumentNode_getDocumentElement(NW_DOM_DocumentNode_t* doc);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_getElementIteratorByTagName

    @synopsis:    Gets the handle to iterate over elements of a given name.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_String_t* tagName
                  Name of the element.

       [out] NW_DOM_NodeIterator_t* handle
                  Iterator to iterate over nodes of same name)

    @description: Gets the handle to iterate over elements of a given name.

    @returns:     NW_Status_t                  
                  Status of operation.

       [NW_STAT_SUCCESS] 
                  Success

       [NW_STAT_FAILURE] 
                  Failure

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not NW_DOM_DOCUMENT_NODE.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_DOM_DocumentNode_getElementIteratorByTagName(NW_DOM_DocumentNode_t* doc, 
                                                NW_String_t* tagName,
                                                NW_DOM_NodeIterator_t* handle);


/** ----------------------------------------------------------------------- **
    @function:   NW_DOM_DocumentNode_getElementIteratorByTagToken

    @synopsis:    Gets the iterator to iterate over elements of a given token.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_Uint16 token
                  Token of the element to be found.

       [out] NW_DOM_NodeIterator_t* handle
                  Iterator to iterate over nodes of same name.

    @description: Gets the iterator to iterate over elements of a given token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS] 
                  Success

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not NW_DOM_DOCUMENT_NODE.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_DOM_DocumentNode_getElementIteratorByTagToken(NW_DOM_DocumentNode_t* doc, 
                                                 NW_Uint16 token,
                                                 NW_DOM_NodeIterator_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_getDictionary

    @synopsis:    Returns dictionary used by this document   

    @scope:       public

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                 The document node.

    @description: Returns dictionary used by this document   

    @returns:     NW_WBXML_Dictionary_t*
                  Dictionary or null if not found.
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_WBXML_Dictionary_t* 
NW_DOM_DocumentNode_getDictionary(NW_DOM_DocumentNode_t* doc);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_createElementNode

    @synopsis:    Creates a new element with the given name.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_String_t* name
                  Name of the element to be created.

    @description: Creates a new element with the given name.

    @returns:     NW_DOM_ElementNode_t*                  
                  New element with the given name; NULL if doc is not a 
                  document node.
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_ElementNode_t* 
NW_DOM_DocumentNode_createElementNode(NW_DOM_DocumentNode_t* doc, NW_String_t* name);


/** ----------------------------------------------------------------------- **
    @function:   NW_DOM_DocumentNode_createElementNodeByToken

    @synopsis:    Creates a new element with the given token.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                 The document node.

       [in] NW_Uint16 token
                 Token of the element to be created.

    @description: Creates a new element with the given token. 

    @returns:     NW_DOM_ElementNode_t*  
                  New element with the given token; NULL if doc is not a 
                  document node or token is an invalid token.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_ElementNode_t* 
NW_DOM_DocumentNode_createElementNodeByToken(
  NW_DOM_DocumentNode_t* doc, NW_Uint16 token);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_createTextNodeWithTextItem

    @synopsis:    Returns a pointer to the created Text Node.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_DOM_TextItem_t* data
                  The value for the new text node.

    @description: Returns a pointer to the created Text Node.

    @returns:     NW_DOM_TextNode_t*
                  New text node; NULL if if doc is not a document node.
                  
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_TextNode_t* 
NW_DOM_DocumentNode_createTextNodeWithTextItem(
  NW_DOM_DocumentNode_t* doc, NW_DOM_TextItem_t* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_createTextNode

    @synopsis:    Creates a text node.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_String_t* data
                  The value for the new text node.

    @description: Creates a text node with the given data.

    @returns:     NW_DOM_TextNode_t*
                  New text node; NULL if if doc is not a document node.
                  
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_TextNode_t* 
NW_DOM_DocumentNode_createTextNode(NW_DOM_DocumentNode_t* doc, NW_String_t* data);


/** ----------------------------------------------------------------------- **
    @function:     NW_DOM_DocumentNode_createCommentNode

    @synopsis:     Creates a comment node with the given data.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_String_t* data
                  Value for the new comment node.

    @description: Creates a comment node with the given data.

    @returns:     NW_DOM_CommentNode_t*
                  New comment node; NULL if doc is not a document node.
                   
 ** ----------------------------------------------------------------------- **/
NW_DOM_CommentNode_t* 
NW_DOM_DocumentNode_createCommentNode(NW_DOM_DocumentNode_t* doc, 
                                      NW_String_t* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_createCDATASectionNode

    @synopsis:    Creates a CDATA section.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

      [in] NW_String_t* data
                  The value for the new CDATA section node.

    @description: Creates a CDATA section node with the given data.

    @returns:     NW_DOM_CDATASectionNode_t*
                  New CData section node; NULL if doc is not a document node.
                  
 ** ----------------------------------------------------------------------- **/
NW_DOM_CDATASectionNode_t* 
NW_DOM_DocumentNode_createCDATASectionNode(NW_DOM_DocumentNode_t* doc, 
                                           NW_String_t* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_createProcessingInstructionNodeByAttrVal

    @synopsis:    Creates a processing instruction node.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_String_t* target
                  The target for the processing instruction.

       [in] NW_DOM_AttrVal_t* data
                  Data for the processing instruction.

    @description: Creates a processing instruction node with the given data.
                  In WBXML, a processing instruction node can have a combination 
                  of opaque, string, token, extension, and entity as the data.

    @returns:     NW_DOM_ProcessingInstructionNode_t*
                  New processing instruction node; NULL if this is a node 
                  type mismatch.
                  
 ** ----------------------------------------------------------------------- **/
NW_DOM_ProcessingInstructionNode_t* 
NW_DOM_DocumentNode_createProcessingInstructionNodeByAttrVal(
 NW_DOM_DocumentNode_t* doc, NW_String_t* target, NW_DOM_AttrVal_t* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_createProcessingInstructionNodeByToken

    @synopsis:    Creates a processing instruction node.

    @scope:       public      

    @parameters: 

       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_Uint16 token
                  Token identifying the target.

       [in] NW_DOM_AttrVal_t* data
                  Data for the processing instruction.

    @description: Creates a processing instruction node with the given data.
                  In WBXML, a processing instruction node can have a combination 
                  of opaque, string, token, extension, and entity as the data.

    @returns:     NW_DOM_ProcessingInstructionNode_t*
                  New processing instruction node; NULL if this is a node type 
                  mismatch.
                  
 ** ----------------------------------------------------------------------- **/
NW_DOM_ProcessingInstructionNode_t* 
NW_DOM_DocumentNode_createProcessingInstructionNodeByToken(
  NW_DOM_DocumentNode_t* doc, NW_Uint16 token, NW_DOM_AttrVal_t* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_createProcessingInstructionNode

    @synopsis:    Creates a processing instruction node.

    @scope:       public      

    @parameters: 
       [in] NW_DOM_DocumentNode_t* doc
                  The document node.

       [in] NW_String_t* target
                  The target for the processing instruction.

       [in] NW_String_t* data
                  Data for the processing instruction.

    @description: Creates a processing instruction node with the given data.

    @returns:     NW_DOM_ProcessingInstructionNode_t*
                  New processing instruction node; NULL if doc is not a document 
                  node.
                  
 ** ----------------------------------------------------------------------- **/
NW_DOM_ProcessingInstructionNode_t* 
NW_DOM_DocumentNode_createProcessingInstructionNode(
  NW_DOM_DocumentNode_t* doc, NW_String_t* target, NW_String_t* data);


/** ----------------------------------------------------------------------- **
    @struct:       NW_TinyDom_Handle

    @synopsis:    A handle to hold all the state required to build a tree.

    @scope:       public
    @variables:
       NW_WBXML_Parser_t wbxmlParser
                  WBXML parser

       NW_TinyDom_Parser_t tinyParser
                  Tiny parser

       NW_TinyDom_Tree_t tinyDOMTree
                  Tiny DOM tree

       NW_WBXML_Writer_t writer
                  WBXML writer

       NW_WBXML_Document_t document
                  WBXML document

    @description: A handle to hold all the state required to build a tree.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyDom_Handle_s{
  NW_WBXML_Parser_t   wbxmlParser;
  NW_TinyDom_Parser_t tinyParser;
  NW_TinyDom_Tree_t   tinyDOMTree;
  NW_WBXML_Writer_t writer;
  NW_WBXML_Document_t document;
} NW_TinyDom_Handle_t;


/* "Factory methods" that generate a dom tree and return the tree
 * root as a document node 
 */
 

/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_BuildTree

    @synopsis:    Create a dom tree by parsing a buffer. 

    @scope:       public      

    @parameters: 
        [in] NW_TinyDom_Handle_t* h
                  Handle for new tree.

        [in] NW_Byte* buffer
                  Buffer to parse.

        [in] NW_Int32 length
                  Length of buffer.

        [in] NW_Bool encoded
                  Encoded if true, otherwise text.

        [in] NW_Uint32 publicID
                  Dictionary to apply.
				
 
        [in] NW_Bool extTNotStringTable
                  If NW_FALSE then extension tokens EXT_T_[0,1,2] have
                  an associated index into the string table.  If NW_TRUE
                  then the NW_Uint32 associated with EXT_T_[0,1,2] is
                  just an anonymous value.

    @description: Create a dom tree by parsing a buffer. 

    @returns:     NW_DOM_DocumentNode_t*
                  Node of created tree.
                  
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_DocumentNode_t* 
NW_DOM_DocumentNode_BuildTree(NW_TinyDom_Handle_t* h, 
                              NW_Byte* buffer,
                              NW_Int32 length,
                              NW_Bool encoded,
                              NW_Uint32 publicID,
                              NW_Bool extTNotStringTable);



/* ************************************************************************/

/*
 * Create and populate a dom tree by parsing a buffer, returning the
 * document node.
 */
IMPORT_C
NW_DOM_DocumentNode_t * 
NW_DOM_DocumentNode_BuildWBXMLTree(NW_TinyDom_Handle_t *h, 
                                   NW_Byte *buffer,
                                   NW_Int32 length,
                                   NW_Bool freeBuff,
                                   NW_Bool extTNotStringTable);

/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_createDocumentWithNumberPublicId

    @synopsis:    Create an empty dom tree.

    @scope:       public      

    @parameters: 
       [in] NW_TinyDom_Handle_t* h
                  The tiny DOM handle.

       [in] NW_Uint8 version
                  Version of the document to be created.

       [in] NW_Uint32 publicid
                  Unique identifier for the DTD.

       [in] NW_Uint32 encoding
                  Encoding for the document.
 
       [in] NW_Bool extTNotStringTable
                  If NW_FALSE then extension tokens EXT_T_[0,1,2] have
                  an associated index into the string table.  If NW_TRUE
                  then the NW_Uint32 associated with EXT_T_[0,1,2] is
                  just an anonymous value.

       [in] NW_Bool enableStringTable
                  Set to true for noraml WBXML.  Set to false to prevent
                  writable DOM from using a string table.


    @description: Create an empty dom tree.
                  In XML and HTML, a public id identifying the DTD is in 
                  string form, but in WBXML sometimes numbers identify the 
                  well-known DTDs. Thus, there is a need for two methods for 
                  document creation.

    @returns:     NW_DOM_DocumentNode_t*
                  New document node; NULL if error during creation.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_DocumentNode_t* 
NW_DOM_DocumentNode_createDocumentWithNumberPublicId(NW_TinyDom_Handle_t* h,
                                                     NW_Uint8 version,
                                                     NW_Uint32 publicid, 
                                                     NW_Uint32 encoding,
                                                     NW_Bool extTNotStringTable,
                                                     NW_Bool enableStringTable);

/** ----------------------------------------------------------------------- **
    @function:    CXML_DOM_DocumentNode_BuildTree

    @synopsis:    Create a dom tree by parsing a buffer and retuen the error
                  status also. 

    @scope:       public      

    @parameters: 
        [in] NW_TinyDom_Handle_t* h
                  Handle for new tree.

        [in] NW_Byte* buffer
                  Buffer to parse.

        [in] NW_Int32 length
                  Length of buffer.

        [in] NW_Bool encoded
                  Encoded if true, otherwise text.

        [in] NW_Uint32 publicID
                  Dictionary to apply.
				
 
        [in] NW_Bool extTNotStringTable
                  If NW_FALSE then extension tokens EXT_T_[0,1,2] have
                  an associated index into the string table.  If NW_TRUE
                  then the NW_Uint32 associated with EXT_T_[0,1,2] is
                  just an anonymous value.

        [out] NW_Status_t* errorStatus
                  Returns the error status code as defined in the WBXML 
                  parser if any error is occured during the parsing of WBXML.
                  If DOM tree is created successfully then this return NW_STAT_SUCCESS.

    @description: Create a dom tree by parsing a buffer. 

    @returns:     NW_DOM_DocumentNode_t*
                  Node of created tree.
                  
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_DocumentNode_t* 
CXML_DOM_DocumentNode_BuildTree(NW_TinyDom_Handle_t* h, 
                              NW_Byte* buffer,
                              NW_Int32 length,
                              NW_Bool encoded,
                              NW_Uint32 publicID,
                              NW_Bool extTNotStringTable,
                              NW_Status_t* errorStatus);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_DocumentNode_Delete

    @synopsis:    Free the tree created by one of the above methods.

    @scope:       public      

    @parameters: 
      [in-out] NW_DOM_DocumentNode_t* docNode
                  Tree to free.

    @description: Free the tree created by one of the above methods.

 ** ----------------------------------------------------------------------- **/
IMPORT_C void 
NW_DOM_DocumentNode_Delete(NW_DOM_DocumentNode_t* docNode);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_DOM_DOCUMENT_H */
