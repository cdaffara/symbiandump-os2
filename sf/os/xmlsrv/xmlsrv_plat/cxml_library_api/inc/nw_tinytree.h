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
    @package:     NW_TinyTree

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_TINY_TREE_H
#define NW_TINY_TREE_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_tinytree_treevector.h>
#include <xml/cxml/nw_tinytree_ebuffer.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ----------------------------------------------------------------------- **
   Tiny tree definitions   
** ----------------------------------------------------------------------- **/



/** ----------------------------------------------------------------------- **
    @typedef:     NW_TinyTree_Index

    @synopsis:    An index into the node array.

    @scope:       public
    @type:        CXML_Vector_Metric_t

    @description: An index into the node array.
 ** ----------------------------------------------------------------------- **/
typedef CXML_Vector_Metric_t NW_TinyTree_Index_t; 


/** ----------------------------------------------------------------------- **
    @typedef:     NW_TinyTree_Offset

    @synopsis:    An offset into the source buffer.

    @scope:       public
    @type:        NW_Uint32

    @description: An offset into the source buffer.
 ** ----------------------------------------------------------------------- **/
/*
typedef NW_Uint32 NW_TinyTree_Offset_t;
*/

/*----------------------------------------------------------------------- **
   Node flags  
** ----------------------------------------------------------------------- **/

#define TNODE_FLAG_LASTSIBLING   0x8000 /* My next_sibling points to parent */

#define TNODE_FLAG_ROOT          0x4000 /* I'm the tree root */

#define TNODE_FLAG_TREE          0x2000 /* I'm a special sentinel node marking
                                           the beginning of a segment */

#define TNODE_FLAG_ORPHAN        0x1000 /* A node not part of a tree */

#define TNODE_USR_FLAGS          0x0FFF  /* 12 bits available to tree user */


/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyTree_Node

    @synopsis:    Tiny tree node structure.

    @scope:       public
    @variables:
       NW_Uint16 flags
                  Node description flags.

       NW_TinyTree_Offset_t source_offset
                  Offset of node data into original buffer.

       NW_TinyTree_Index_t first_child
                  Index of first child of this node.

       NW_TinyTree_Index_t next_sibling
                  Index of next sibling of this node.

    @description: Tiny tree node structure.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyTree_Node_s NW_TinyTree_Node_t;

struct NW_TinyTree_Node_s {
  NW_Uint16 flags;
  NW_TinyTree_Offset_t source_offset;
  NW_TinyTree_Node_t* first_child;
  NW_TinyTree_Node_t* next_sibling;
  NW_TinyTree_t* tree;
  NW_Uint16 token;
};
   

/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyTree_TinyTree

    @synopsis:    Tree structure. 

    @scope:       public
    @variables:
       NW_TinyTree_TreeVector_t* tree
                  Node data.

       NW_TinyTree_Index_t root_index
                  Node data.

       NW_TinyTree_EBuffer_t* ebuffer
                  Buffer data.

       void* context
                  Context accessible from tree (typically the parser that 
                  builds and reads the tree).

    @description: Tree structure. Provides access points for root 
                  and buffer data.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyTree_TinyTree_s {
  /* Node data */
  NW_TinyTree_TreeVector_t* tree;
  NW_TinyTree_Index_t root_index;
  /* Buffer data */
  NW_TinyTree_EBuffer_t* ebuffer;
  /* Context accessible from tree (typically the parser that builds and reads the tree) */
  void* context;
}NW_TinyTree_TinyTree_t;

/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyTree_TreeNode

    @synopsis:    A sentinel node that resides at the start of 
                  every tree vector segment.

    @scope:       public
    @variables:
       NW_Uint16 flags
                  Node description flags.

       NW_TinyTree_t* tree
                  The tree.

    @description: A special sentinel node that resides at the start of 
                  every tree vector segment, pointing to the tree itself.
                  This is used by applications like the DOM api that do 
                  not have a reference to the tree when operating on nodes.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyTree_TreeNode_s{
  NW_Uint16 flags;
  NW_TinyTree_t* tree;
}NW_TinyTree_TreeNode_t;


 /* ----------------------------------------------------------------------- ** 
    Public tiny tree API  
 ** ----------------------------------------------------------------------- **/



/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_new

    @synopsis:    Initialize a tree.

    @scope:       public

    @description: Initialize a tree.Allocates memory and initializes variables.

    @returns:     NW_TinyTree_TinyTree_t*
                  The new tree.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_t*
NW_TinyTree_new();


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_construct

    @synopsis:    Constructor.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_TinyTree_t* tree
                  The tiny tree.

       [in] CXML_Vector_Metric_t initialNodeCount
                  Initiali size of vector of nodes.

       [in] void* buffer
                  Link buffer into ebuffer.

       [in] NW_TinyTree_Offset_t buffsz
                  Size of block of data.

       [in] void* context
                  Context (i.e. parser).

       [in] NW_Bool freeBuff
                  Buffer deallocation flag.

    @description: Constructor. Adds data to the structure returned 
                  by NW_TinyTree_new.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Tree created.

       [NW_STAT_FAILURE]
                  Out of memory or general error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyTree_construct(NW_TinyTree_t* tree,
                      CXML_Vector_Metric_t initialNodeCount,
                      void* buffer,
                      NW_TinyTree_Offset_t buffsz,
                      void* context,
                      NW_Bool freeBuff);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_destruct

    @synopsis:    Clean up tree resources.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  default

    @description: Clean up tree resources.

 ** ----------------------------------------------------------------------- **/
void
NW_TinyTree_destruct(NW_TinyTree_t* tree);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_createNode

    @synopsis:    Create an unattached node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Offset_t offset
                  Offset into buffer.

    @description: Create an unattached node which references the source 
                  buffer at offset.

    @returns:     NW_TinyTree_Node_t*
                  Node or NULL if memory could not be allocated.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_createNode(NW_TinyTree_t* tree, 
                       NW_TinyTree_Offset_t offset);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_setRoot

    @synopsis:    Create a root node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Offset_t offset
                  Offset into buffer.

    @description: Create a root node which references the source buffer 
                  at offset. Returns a pointer to the root node.

    @returns:     NW_TinyTree_Node_t*
                  Pointer to the root node.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_setRoot(NW_TinyTree_t* tree, 
                    NW_TinyTree_Offset_t offset);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_getRoot

    @synopsis:    Get the root node of the tree.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

    @description: Get the root node of the tree.

    @returns:     NW_TinyTree_Node_t*
                  The root node or NULL if it is not valid.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_getRoot(NW_TinyTree_t* tree);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_findNextSibling

    @synopsis:    Find the next sibling of a node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

    @description: Find the next sibling of a node.

    @returns:     NW_TinyTree_Node_t*
                  The next sibling node or NULL if it is not valid.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_findNextSibling(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_findLastSibling

    @synopsis:    Find last sibling of a node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

    @description: Find last sibling of a node.

    @returns:     NW_TinyTree_Node_t*
                  The last sibling node or NULL if it is not valid.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_findLastSibling(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_findPreviousSibling

    @synopsis:    Find the previous sibling of a node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

    @description: Find the previous sibling of a node.

    @returns:     NW_TinyTree_Node_t*
                  The previous sibling node or NULL if it is not valid.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_findPreviousSibling(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_findFirstChild

    @synopsis:    Find the first child of a node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

    @description: Find the first child of a node.

    @returns:     NW_TinyTree_Node_t*
                  The first child node or NULL if it is not valid.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_findFirstChild(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_findLastChild

    @synopsis:    Find a node's last child.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

    @description: Find a node's last child.

    @returns:     NW_TinyTree_Node_t*
                  The last child node or NULL if it is not valid.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_findLastChild(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_findParent

    @synopsis:    Find a node's parent.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

    @description: Find a node's parent.

    @returns:     NW_TinyTree_Node_t*
                  The parent node of this node or NULL if it is not valid.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t* 
NW_TinyTree_findParent(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_attachAfter

    @synopsis:    Attach a sibling after a node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

       [in] NW_TinyTree_Node_t* sibling
                  Sibling node to attach.

    @description: Attach a sibling after a node.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_TinyTree_attachAfter(NW_TinyTree_Node_t* node, 
                        NW_TinyTree_Node_t* sibling);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_attachBefore

    @synopsis:    Attach a sibling before another node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

       [in] NW_TinyTree_Node_t* sibling
                  default

    @description: Attach a sibling before another node.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Node attached.

       [NW_STAT_FAILURE]
                  Could not attach node.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_TinyTree_attachBefore(NW_TinyTree_Node_t* node, 
                         NW_TinyTree_Node_t* sibling);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_attachChild

    @synopsis:    Attach a child to a node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The reference node.

       [in] NW_TinyTree_Node_t* child
                  The child to attach.

    @description: Attach a child to a node as last child.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Node attached.

       [NW_STAT_FAILURE]
                  Could not attach node.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyTree_attachChild(NW_TinyTree_Node_t* node, 
                        NW_TinyTree_Node_t* child);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_deleteNode

    @synopsis:    Delete a node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  The node to delete.

    @description: Delete a node, deleting any subtree below the node as well.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyTree_deleteNode(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_setContext

    @synopsis:    Set context.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] void* context
                  Context to set.

    @description: Set context.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyTree_setContext(NW_TinyTree_t* tree, 
                       void* context);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_getContext

    @synopsis:    Get context.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

    @description: Get context.

    @returns:     void*
                  The context.

 ** ----------------------------------------------------------------------- **/
void*
NW_TinyTree_getContext(NW_TinyTree_t* tree);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_Node_getFlags

    @synopsis:    Get flags.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_Node_t* node
                  The tiny tree.

    @description: Get flags.

    @returns:     NW_Uint16
                  The node flags.

 ** ----------------------------------------------------------------------- **/
NW_Uint16 
NW_TinyTree_Node_getFlags(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_Node_setUserFlags

    @synopsis:    Set user flags.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_Node_t* node
                  The tiny tree.

       [in] NW_Uint16 flags
                  The flags.

    @description: Set user flags.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyTree_Node_setUserFlags(NW_TinyTree_Node_t* node, 
                              NW_Uint16 flags);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_Node_getSourceOffset

    @synopsis:    Get source offset.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_Node_t* node
                  The tiny tree.

    @description: Get source offset.

    @returns:     NW_TinyTree_Offset_t
                  The offset.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Offset_t
NW_TinyTree_Node_getSourceOffset(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_Node_getSourceAddress

    @synopsis:    Get source address.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  Reference node.
    @description: Get source address.

    @returns:     void*
                  The address.

 ** ----------------------------------------------------------------------- **/
void*
NW_TinyTree_Node_getSourceAddress(NW_TinyTree_t* tree, 
                                  NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_Node_GetSegmentAndOffset

    @synopsis:    Get segment and offset.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  default

       [out] NW_Uint8** segment
                  default

       [out] NW_Uint32* segSize
                  default

       [out] NW_Uint32* offset
                  default

    @description: Get segment and offset.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Segment and offset returned.

       [NW_STAT_FAILURE]
                  Could not get segment and offset.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_TinyTree_Node_GetSegmentAndOffset(NW_TinyTree_t* tree,
                                     NW_TinyTree_Node_t* node,
                                     NW_Uint8** segment,
                                     NW_Uint32* segSize,
                                     NW_Uint32* offset);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_GetSourceOffset

    @synopsis:    Get source offset.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_Uint8* segmentAddr
                  default

       [out] NW_Uint32 offset
                  Offset.

       [out] CXML_Vector_Metric_t* index
                  Index of segment.

    @description: Get source offset.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Offset returned.

       [NW_STAT_FAILURE]
                  Could not get offset.

 ** ----------------------------------------------------------------------- **/
extern
NW_Status_t
NW_TinyTree_GetSourceOffset(NW_TinyTree_t* tree,
                            NW_Uint8* segmentAddr,         
                            NW_Uint32  offset,
                            NW_Uint32* index);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_Node_findTree

    @synopsis:    Find tree.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_Node_t* node
                  The tree node.

    @description: Get a reference to the tree that owns a node. This is
                  not likely to be a very efficient call since it can 
                  only find the tree safely by doing a linear search for
                  the sentinel tree node at the beginning of the current
                  segment.

    @returns:     NW_TinyTree_t*
                  Tiny tree or NULL if not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_TinyTree_t*
NW_TinyTree_Node_findTree(NW_TinyTree_Node_t* node);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_createChild

    @synopsis:    Create child node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* parent
                  Parent of new child.

       [in] NW_TinyTree_Offset_t offset
                  Offset.

    @description: Create a new node as a child of an existing node. The 
                  child references the source buffer at offset. The new 
                  child node is returned.  

    @returns:     NW_TinyTree_Node_t*
                  The new chold node or NULL if not successful.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_createChild(NW_TinyTree_t* tree, 
                        NW_TinyTree_Node_t* parent, 
                        NW_TinyTree_Offset_t offset);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_createSibling

    @synopsis:    Create sibling node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* node
                  Sibling of new node.

       [in] NW_TinyTree_Offset_t offset
                  Offset.

    @description: Create a new node as an immediate sibling to an 
                  existing node. The child references the source buffer
                  at offset. The new child node is returned.  

    @returns:     NW_TinyTree_Node_t*
                  New node or NULL if not successful.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_createSibling(NW_TinyTree_t* tree, 
                          NW_TinyTree_Node_t* node, 
                          NW_TinyTree_Offset_t offset);


  /* RME Documentation tools does not handle function call as parameter */
/* * ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_recurse

    @synopsis:    Recurse through tree using callback.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* start_node
                  Node to start to recurse from.

       [in] void (*Node_CB) (NW_TinyTree_t*, NW_TinyTree_Node_t* , void*)
                  Node handling calllback

       [in] void* context
                  Parser context.

    @description: Recurse through tree calling the callback function 
                  for each node.

 ** ----------------------------------------------------------------------- **/
void
NW_TinyTree_recurse(NW_TinyTree_t* tree,
                    NW_TinyTree_Node_t* start_node, 
                    void (*Node_CB) (NW_TinyTree_t*, NW_TinyTree_Node_t* , void*),
                    void* context);


/* ----------------------------------------------------------------------- **
   Node iterator functions. These iterate through nodes one at a time
   returning NULL when there are no more nodes to iterate.  
** ----------------------------------------------------------------------- **/
  
/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyTree_NodeIterator

    @synopsis:    default

    @scope:       public
    @variables:
       NW_TinyTree_Node_t* start_node
                  The start node.

       NW_TinyTree_Node_t* traversal_node
                  Present node.

       NW_TinyTree_t* tree
                  The tree.

    @description: Node iterator type definition.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyTree_NodeIterator_s{
  NW_TinyTree_Node_t* start_node;
  NW_TinyTree_Node_t* traversal_node;
}NW_TinyTree_NodeIterator_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_NodeIterator_init

    @synopsis:    Initialize a node iterator with a start node.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] NW_TinyTree_Node_t* start_node
                  Iterator start point.

       [out] NW_TinyTree_NodeIterator_t* iterator
                  Iterator structure.

    @description: Initialize a node iterator with a start node. Iterate
                  through nodes one at a time returning NULL when there 
                  are no more nodes to iterate.  
 ** ----------------------------------------------------------------------- **/
void
NW_TinyTree_NodeIterator_init(NW_TinyTree_Node_t* start_node,
                              NW_TinyTree_NodeIterator_t* iterator);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_NodeIterator_iterate

    @synopsis:    Iterate through a subtree.

    @scope:       public

    @parameters:
       [in-out] NW_TinyTree_NodeIterator_t* iterator
                  The iterator.

    @description: Iterate through a subtree returning each node exactly
                  once. The algorithm follows the toplogical ordering of
                  the tree (if there is an edge from v0 to v1, always 
                  visit v0 before v1). It's equivalent to solving a 
                  labyrinth by keeping your right hand on the wall!

    @returns:     NW_TinyTree_Node_t*
                  Next node in iteration or NULL if done.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_NodeIterator_iterate(NW_TinyTree_NodeIterator_t* iterator);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_NodeIterator_iterateSiblings

    @synopsis:    Iterate through the siblings of a node.

    @scope:       public

    @parameters:
       [in-out] NW_TinyTree_NodeIterator_t* iterator
                  The iterator.

    @description: Iterate through the siblings of a node.

    @returns:     NW_TinyTree_Node_t*
                  Next node in iteration or NULL if done.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Node_t*
NW_TinyTree_NodeIterator_iterateSiblings(NW_TinyTree_NodeIterator_t* iterator);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_GetWritableBlock

    @synopsis:    Get writable block.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_t* tree
                  The tiny tree.

       [in] CXML_Vector_Metric_t size
                  Requested size.

       [in] CXML_Vector_Metric_t* source_offset
                  Offset.

    @description: Get writable block.

    @returns:     NW_Uint8*
                  Writable block.

 ** ----------------------------------------------------------------------- **/
NW_Uint8*
NW_TinyTree_GetWritableBlock(NW_TinyTree_t* tree,
                             NW_Uint32  size,
                             NW_Uint32 *source_offset);


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_TINY_TREE_H */
