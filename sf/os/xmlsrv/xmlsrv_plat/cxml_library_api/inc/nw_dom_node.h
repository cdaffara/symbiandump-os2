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
#ifndef NW_DOM_NODE_H
#define NW_DOM_NODE_H

#include <xml/cxml/nw_tinytree.h>
#include <xml/cxml/nw_tinydom.h>
#include <xml/cxml/nw_string_string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @typedef:     NW_DOM_Node

    @synopsis:    Dom nodes are defined to be just tiny tree nodes.  

    @scope:       public
     
    @type:        NW_TinyTree_Node_t

    @description: Dom nodes are defined to be just tiny tree nodes.
                  This allows the Dom API to return the 32 bit pointers
                  it gets from the Tiny Tree and Tiny Dom APIs rather
                  than having to allocate any memory. This memory
                  savings comes at some cost, however: most Tiny Tree
                  API calls require a tree reference, and getting a tree
                  reference from an tiny dom node is an inefficient
                  operation (it requires a linear search of the node's
                  storage segment). We assume that this computation vs
                  memory tradeoff is worthwhile on small platforms.  One
                  alternative would be to require the tree as a
                  parameter to many Dom API calls, but this would be a
                  substantial departure from the standard Dom model.

 ** ----------------------------------------------------------------------- **/
typedef NW_TinyTree_Node_t NW_DOM_Node_t;

/* Specific node types used by the DOM API */

typedef NW_Uint16 NW_DOM_NodeType_t;

#define NW_DOM_ELEMENT_NODE                1
#define NW_DOM_TEXT_NODE                   2
#define NW_DOM_CDATA_SECTION_NODE          4 /* only for XML  */
#define NW_DOM_PROCESSING_INSTRUCTION_NODE 8
#define NW_DOM_COMMENT_NODE                16 /* only for XML  */
#define NW_DOM_DOCUMENT_NODE               32

typedef NW_DOM_Node_t NW_DOM_ElementNode_t;
/*typedef NW_DOM_Node_t NW_DOM_DocumentNode_t; */
/* Declared in nw_tiny_tree.h */
typedef NW_DOM_Node_t NW_DOM_TextNode_t;
typedef NW_DOM_Node_t NW_DOM_CommentNode_t;
typedef NW_DOM_Node_t NW_DOM_CDATASectionNode_t;
typedef NW_DOM_Node_t NW_DOM_ProcessingInstructionNode_t;

/*  ----------------------------------------------------------------------- **
    GENERAL NODE METHODS - applicable to all node types
    These methods will be repeated for other Node types also
 ** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getNodeName

    @synopsis:    Get name of node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* elem
                  The node.

       [in-out] NW_String_t* name
                  The name of the node.

    @description: Get name of node.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Node name returned.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Node type could not be determined.
       
       [NW_STAT_DOM_NO_STRING_RETURNED]
                  Node does not have name.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  It is not a NW_DOM_ELEMENT_NODE

       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory trying to allocate string storage.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Illegal character set.

       [NW_STAT_FAILURE]
                  String found but empty.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_DOM_Node_getNodeName(NW_DOM_Node_t* elem, NW_String_t* name); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getNodeType

    @synopsis:    Gets the type of DOM node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Gets the type of DOM node.

    @returns:     NW_DOM_NodeType_t
                  Node type or zero if not recognized node.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_NodeType_t
NW_DOM_Node_getNodeType(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getNodeToken

    @synopsis:    Gets the token associated with the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Returns fully qualified node token. This includes the 
                  token, codepage and a attribute/element flag.

    @returns:     NW_Uint16
                  The 8 bit token and 8 bit codepage which includes an 
                  attribute/elent flag.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint16 
NW_DOM_Node_getNodeToken(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_isWBXML

    @synopsis:    Checks if the node was created by parsing WBXML content.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Checks if the node was created by parsing WBXML content.

    @returns:     NW_Bool
                  NW_TRUE if WBXML type, otherwise NW_FALSE.

 ** ----------------------------------------------------------------------- **/
NW_Bool 
NW_DOM_Node_isWBXML(NW_DOM_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getParentNode

    @synopsis:    Gets the parent of the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Finds parent node of the given node. All nodes except 
                  DocumentNode have a parentNode.

    @returns:     NW_DOM_Node_t*
                  Parent or NULL if invalid operation or if parent node 
                  is a DocumentNode.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_Node_t* 
NW_DOM_Node_getParentNode(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getFirstChild

    @synopsis:    Finds the first child of the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Returns valid values only for ELEMENT_NODE and DOCUMENT_NODE.
                  These are the only nodes that have children. For other nodes
                  the value is NULL.

    @returns:     NW_DOM_Node_t*
                  The first child of this node or NULL if the node has no child.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_Node_t* 
NW_DOM_Node_getFirstChild(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getLastChild

    @synopsis:    Finds the last child of the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Returns valid values only for ELEMENT_NODE and DOCUMENT_NODE.
                  These are the only nodes that have children. For other nodes
                  the value is either NULL or 0.

    @returns:     NW_DOM_Node_t*
                  The last child of this node or NULL if the node has no child.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_Node_t* 
NW_DOM_Node_getLastChild(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_hasChildNodes

    @synopsis:    Checks if the node has any child nodes.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Returns valid values only for ELEMENT_NODE and DOCUMENT_NODE.
                  These are the only nodes that have children. For other nodes
                  the value is NULL.

    @returns:     NW_Bool
                  NW_TRUE if the node has child nodes, otherwise NW_FALSE.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Bool 
NW_DOM_Node_hasChildNodes(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getNextSibling

    @synopsis:    Gets the next sibling of the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Gets the next sibling of the node.

    @returns:     NW_DOM_Node_t*
                  The node immediately following this node, NULL if there 
                  is no next sibling or this node is NULL.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_Node_t* 
NW_DOM_Node_getNextSibling(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getPreviousSibling

    @synopsis:    Gets the previous sibling of the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Gets the previous sibling of the node.

    @returns:     NW_DOM_Node_t*
                  The node immediately preceding this node in the DOM tree,
                  NULL if there is no previous sibling or the node is NULL.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_Node_t* 
NW_DOM_Node_getPreviousSibling(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_getOwnerDocument

    @synopsis:    Gets the owner document node of this node.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

    @description: Gets the owner document node of this node.

    @returns:     NW_DOM_DocumentNode_t*
                  The owner document node (NW_DOM_DocumentNode_t) associated
                  with this node, NULL if the node is NULL or is a DocumentNode.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_DOM_DocumentNode_t* 
NW_DOM_Node_getOwnerDocument(NW_DOM_Node_t* node); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_insertBefore

    @synopsis:    Inserts newChild before the refChild.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_Node_t* node
                  The parent node.

       [in] NW_DOM_Node_t* newChild
                  The node to be inserted.

       [in] NW_DOM_Node_t* refChild
                  The node before which the newChild node should be inserted.

    @description: Inserts the newChild node before the existing refChild 
                  node; and if refChild is NULL, inserts it at the end of
                  the list of children.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Inserted successfully.

       [NW_STAT_BAD_INPUT_PARAM]
                  One of the input parameters was null.
     
       [NW_STAT_NOT_FOUND]
                  Reference child is not a child of given node.

       [NW_STAT_DOM_WRONG_DOC_ERR]
                  New child was created from a different document than the 
                  one that created the node.

       [NW_STAT_DOM_HEIRARCHY_REQ_ERR] 
                  Node is of the type that does not allow children of the 
                  type of newChild node.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_Node_insertBefore(NW_DOM_Node_t* node, 
                         NW_DOM_Node_t* newChild, 
                         NW_DOM_Node_t* refChild);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_replaceChild

    @synopsis:    Replaces oldChild with the newChild.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

       [in] NW_DOM_Node_t* newChild
                  The node that will replace oldChild.

       [in] NW_DOM_Node_t* oldChild
                  The node to be replaced.

    @description: Replaces oldChild with the newChild.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Child replaced.

       [NW_STAT_BAD_INPUT_PARAM]
                  Bad data.

       [NW_STAT_NOT_FOUND]
                  oldChild is not a child of given node.

       [NW_STAT_DOM_WRONG_DOC_ERR]
                  newChild was created from a different document than the 
                  one that created the node.

       [NW_STAT_DOM_HEIRARCHY_REQ_ERR]
                  Node is of the type that does not allow children of the 
                  type of newChild node.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_Node_replaceChild(NW_DOM_Node_t* node, 
                         NW_DOM_Node_t* newChild, 
                         NW_DOM_Node_t* oldChild);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_removeChild

    @synopsis:    Removes the oldChild node as a child of the node.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_Node_t* node
                  The parent node.

       [in] NW_DOM_Node_t* oldChild
                  The child node to be removed.

    @description: Removes the oldChild, and deletes the node and its child nodes.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Nodes deleted.

       [NW_STAT_BAD_INPUT_PARAM]
                  Bad data.

       [NW_STAT_NOT_FOUND]
                  oldChild is not a child of given node.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_Node_removeChild(NW_DOM_Node_t* node, 
                        NW_DOM_Node_t* oldChild); 


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Node_appendChild

    @synopsis:    Appends a new child.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The node.

       [in] NW_DOM_Node_t* newChild
                  The node to be added.

    @description: Appends a new child.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Child node appended.

       [NW_STAT_BAD_INPUT_PARAM]
                  An input parameter was null.
 
       [NW_STAT_DOM_WRONG_DOC_ERR]
                  newChild was created from a different document than the 
                  one that created the node.

       [NW_STAT_DOM_HEIRARCHY_REQ_ERR]
                  Node is of the type that does not allow children of the type
                  of the newChild node.


 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_Node_appendChild(NW_DOM_Node_t* node, 
                        NW_DOM_Node_t* newChild);


/** ----------------------------------------------------------------------- **
    @struct:      NW_DOM_NodeIterator

    @synopsis:    Iterator for walking node list.

    @scope:       public
    @variables:
       NW_TinyTree_NodeIterator_t nodeIter
                  The node iterator.

       NW_Uint16 token
                  Fully qualified token (i.e. token and codepage)

    @description: NW_DOM_NodeIterator_t is used in lieu of the NodeList 
                 object specified in DOM. This data type is an iterator 
                 default.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_DOM_NodeIterator_s {
  NW_TinyTree_NodeIterator_t nodeIter;
  NW_Uint16 token;
}NW_DOM_NodeIterator_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_NodeIterator_new

    @synopsis:    Creates new NodeIterator.

    @scope:       public

    @description: Creates new NodeIterator.

    @returns:     NW_DOM_NodeIterator_t*
                  New iterator.

 ** ----------------------------------------------------------------------- **/
NW_DOM_NodeIterator_t*
NW_DOM_NodeIterator_new(void);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_NodeIterator_initialize

    @synopsis:    Initializes iterator.

    @scope:       public

    @parameters:
       [in] NW_DOM_NodeIterator_t* handle
                  The node iterator.

       [in] NW_DOM_Node_t* node
                  Starting point for iteration.
       [in] NW_Uint16 token
                  Initialize with this token.

    @description: Initializes the Node handle which uses the given node
                  as the starting point to iterate down the tree.


    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Iterator initialized.

       [NW_STAT_FAILURE]
                  Required parameter is null.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_NodeIterator_initialize(NW_DOM_NodeIterator_t* handle, 
                               NW_DOM_Node_t* node, 
                               NW_Uint16 token);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_NodeIterator_delete

    @synopsis:    Deletes the handle.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_NodeIterator_t* handle
                  The node iterator.

    @description: Deletes the handle.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Always returned.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_NodeIterator_delete(NW_DOM_NodeIterator_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_NodeIterator_nextNode

    @synopsis:    Returns the next node.

    @scope:       public

    @parameters:
       [in] NW_DOM_NodeIterator_t* handle
                  The node iterator.

    @description: Returns the next node.

    @returns:     NW_DOM_Node_t* 
                  The next node if there is one, otherwise NULL.

 ** ----------------------------------------------------------------------- **/
NW_DOM_Node_t* 
NW_DOM_NodeIterator_nextNode(NW_DOM_NodeIterator_t* handle);


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_DOM_NODE_H */
