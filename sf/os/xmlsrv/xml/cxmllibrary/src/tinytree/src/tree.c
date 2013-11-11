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


#include <xml/cxml/nw_tinytree.h>
#include "cxml_vector.h"

NW_TinyTree_t*
NW_TinyTree_new(){
  NW_TinyTree_t* tree = (NW_TinyTree_t*) NW_Mem_Malloc (sizeof(NW_TinyTree_t));
  if (tree != NULL){
    tree->tree = NULL;
    tree->root_index = 0;
    tree->ebuffer = NULL;
    tree->context = NULL;
  }
  return tree;
}

NW_Status_t
NW_TinyTree_construct(NW_TinyTree_t *tree,
                      CXML_Vector_Metric_t initialNodeCount,
                      void *buffer,
                      NW_TinyTree_Offset_t buffsz,
                      void *context,
                      NW_Bool freeBuff){

  NW_ASSERT(tree != NULL);

  tree->tree =
  NW_TinyTree_TreeVector_Construct (sizeof(NW_TinyTree_Node_t),
                              initialNodeCount,
                              tree);
  if(tree->tree == NULL) {
    return NW_STAT_OUT_OF_MEMORY;
  }

  /* Make the supplied buffer the first ebuffer segment */
  tree->ebuffer = NW_TinyTree_EBuffer_Construct((NW_Uint8*) buffer, 
                                          buffsz, 
                                          NW_TINY_TREE_BLOCK_SIZE_DEFAULT,
                                          freeBuff);
  if(tree->ebuffer == NULL){
    NW_TinyTree_TreeVector_Destruct(tree->tree);
    tree->tree = NULL;
    return NW_STAT_OUT_OF_MEMORY;
  }

  /* Init the rest of the tree */
  tree->root_index = 0;
  tree->context = context;
  return NW_STAT_SUCCESS;
}

void
NW_TinyTree_destruct(NW_TinyTree_t *tree)
{
  if (tree != NULL){
    if (tree->tree != NULL) {
      NW_TinyTree_TreeVector_Destruct (tree->tree);
    }
    if (tree->ebuffer != NULL){
      NW_TinyTree_EBuffer_Destruct (tree->ebuffer);
    }
    tree->tree = NULL;
    tree->root_index = 0;
    tree->ebuffer = NULL;
    tree->context = NULL;
  }
}

static 
NW_TinyTree_Node_t*
NW_TinyTree_appendNodeAt(NW_TinyTree_t *tree, 
                         NW_TinyTree_Node_t *node, 
                         NW_TinyTree_Index_t index){
  NW_TinyTree_TreeNode_t* sentinel;
	NW_TinyTree_Node_t* retNode = NULL;


  NW_ASSERT(tree != NULL);
  NW_ASSERT(tree->tree != NULL);
  NW_ASSERT(node != NULL);

	sentinel = (NW_TinyTree_TreeNode_t*) NW_Mem_Malloc(sizeof(NW_TinyTree_TreeNode_t));
	if(sentinel == NULL){
		return NULL;
	}
  sentinel->flags = TNODE_FLAG_TREE;
  sentinel->tree = tree;

  /* Add the new element at the specified index */
  retNode = (NW_TinyTree_Node_t*)
         CXML_Vector_InsertAt(tree->tree->vector, (void*)node, index, sentinel);
           NW_Mem_Free(sentinel);
	return retNode;
}

/* Add a node, appending storage to the tree vector.
 * The appended node is created as an orphan and needs to be attached
 * to the tree somewhere.
 */

static
NW_TinyTree_Node_t*
NW_TinyTree_appendNode(NW_TinyTree_t *tree, 
                       NW_TinyTree_Node_t *node){

  NW_ASSERT(tree != NULL);
  NW_ASSERT(node != NULL);

  /* Add the new element */

  return NW_TinyTree_appendNodeAt(tree, node, CXML_Vector_AtEnd);
}


/* Get the node at a given index */

static
NW_TinyTree_Node_t*
NW_TinyTree_getNode(NW_TinyTree_t *tree, 
                    NW_TinyTree_Index_t index){

  NW_ASSERT(tree != NULL);
  NW_ASSERT(tree->tree != NULL);
  NW_ASSERT(tree->tree->vector != NULL);
  NW_ASSERT(index > 0);
  
  return (NW_TinyTree_Node_t*)
    CXML_Vector_ElementAt(tree->tree->vector, (CXML_Vector_Metric_t)index);
}

/*
* Create an unattached node which references the source buffer at offset.
*/

NW_TinyTree_Node_t*
NW_TinyTree_createNode(NW_TinyTree_t* tree, 
                       NW_TinyTree_Offset_t offset){
  
  NW_TinyTree_Node_t node;

  NW_ASSERT(tree != NULL);
  NW_ASSERT(tree->tree != NULL);
  /* The root must have been set before creating any new nodes */
  NW_ASSERT(tree->root_index == 1);

  /* Initialize a node on the stack */
  node.source_offset = offset;
  node.flags = 0;
  node.first_child = 0;
  node.next_sibling = 0; 
  node.tree = tree;
  node.token = 0;
  /* Copy this into the tree */
  return NW_TinyTree_appendNode(tree, &node);
}

/* 
* Create a root node which references the source buffer at offset. 
* Returns a pointer to the root node.
*/

NW_TinyTree_Node_t*
NW_TinyTree_setRoot(NW_TinyTree_t *tree, 
                    NW_TinyTree_Offset_t offset){
  
  NW_TinyTree_Node_t root_node;
  
  NW_ASSERT(tree != NULL);
  NW_ASSERT(tree->tree != NULL);
  /* The root must not have set been already */
  NW_ASSERT(tree->root_index == 0);

  tree->root_index = 1;
  root_node.source_offset = offset;
  root_node.flags = TNODE_FLAG_ROOT;
  root_node.first_child = 0;
  root_node.next_sibling = 0;
  root_node.tree = tree;
  root_node.token = 0;
  /* TODO: make this a dummy element! */
  if (NW_TinyTree_appendNodeAt(tree, &root_node, 0) == NULL) {
    return NULL;
  }

  return NW_TinyTree_appendNodeAt(tree, &root_node, tree->root_index);
}

/*
* Get the root node of the tree.
*/

NW_TinyTree_Node_t*
NW_TinyTree_getRoot(NW_TinyTree_t *tree){

  NW_ASSERT(tree != NULL);
  /* Return NULL if not set */
  if(tree->root_index == 0)
    return NULL;
  return NW_TinyTree_getNode(tree, tree->root_index);
}

NW_TinyTree_Node_t*
NW_TinyTree_findNextSibling(NW_TinyTree_Node_t *node){

  NW_ASSERT(node != NULL);

  if (((node->flags & TNODE_FLAG_LASTSIBLING) == TNODE_FLAG_LASTSIBLING)
      || (node->next_sibling == 0)) {
    return NULL;
  }
  
  return node->next_sibling;
}

/* Find last sibling address */

NW_TinyTree_Node_t*
NW_TinyTree_findLastSibling(NW_TinyTree_Node_t *node)
{
  NW_TinyTree_Node_t* sibling = node;

  NW_ASSERT(node != NULL);

  while (((sibling->flags & TNODE_FLAG_LASTSIBLING) != TNODE_FLAG_LASTSIBLING)
         && (sibling->next_sibling != 0)) {
    sibling = sibling->next_sibling;
  }

  /* Because the array always starts with the tree node, no node ever has index 0 */
  if(sibling == node)
    return NULL;
  
  return sibling;
}

NW_TinyTree_Node_t*
NW_TinyTree_findFirstChild(NW_TinyTree_Node_t *node)
{
  NW_ASSERT(node != NULL);
  return node->first_child;
}


/*
* Find a node's last child
*/

NW_TinyTree_Node_t*
NW_TinyTree_findLastChild(NW_TinyTree_Node_t* node){
  
  NW_TinyTree_Node_t* first;
  NW_TinyTree_Node_t* last;

  NW_ASSERT(node != NULL);

  first = NW_TinyTree_findFirstChild(node);

  if(first == NULL){
    return NULL; /* No children */
  }

  last = NW_TinyTree_findLastSibling(first);

  if(last == NULL){ /* No siblings, only child */
    return first;
  }

  return last;
}

/* 
* Get a node's parent
*/

NW_TinyTree_Node_t* 
NW_TinyTree_findParent(NW_TinyTree_Node_t *node)
{
 
  NW_TinyTree_Node_t *last_sibling;

  NW_ASSERT(node != NULL);

  /* Is this the root node ? */
  if((node->flags & TNODE_FLAG_ROOT) == TNODE_FLAG_ROOT)
    return NULL;
  
  /* Is next sibling zero ? */
  if (node->next_sibling == 0)
    return NULL;

  /* The next sibling index of the last sibling points back to parent */

  last_sibling = NW_TinyTree_findLastSibling(node);

  if (last_sibling == NULL){
    last_sibling = node;  /* No siblings */
  }

  return last_sibling->next_sibling;
}


/*
* Attach a node as a sibling after another node.
*/

NW_Status_t 
NW_TinyTree_attachAfter(NW_TinyTree_Node_t *node, 
                        NW_TinyTree_Node_t *sibling)
{
  NW_ASSERT(node != NULL);
  NW_ASSERT(sibling != NULL);

  if((node->flags & TNODE_FLAG_LASTSIBLING) == TNODE_FLAG_LASTSIBLING){
    node->flags &= ~TNODE_FLAG_LASTSIBLING; 
    sibling->flags |= TNODE_FLAG_LASTSIBLING;
  }
  
  sibling->next_sibling = node->next_sibling;
  node->next_sibling = sibling;
  return NW_STAT_SUCCESS;
}

NW_TinyTree_Node_t*
NW_TinyTree_findPreviousSibling(NW_TinyTree_Node_t *node){

  NW_TinyTree_Node_t *sibling;
  NW_TinyTree_Node_t *parent; 

  NW_ASSERT(node != NULL);

  /* First find the parent */
  parent = NW_TinyTree_findParent(node);

  if(parent == NULL)
    return NULL;

  /* Then get first child */
  sibling = NW_TinyTree_findFirstChild(parent);
  NW_ASSERT(sibling != NULL);
  if(sibling == node)
    return NULL; /* Only child */

  /* Find a sibling whose next_sibling points to me */
  while(sibling->next_sibling != node){ 
    NW_ASSERT(sibling->next_sibling != NULL);
    sibling = sibling->next_sibling;
  }
  return sibling;
}

/*
* Attach a node as a sibling before another node.
*/

NW_Status_t 
NW_TinyTree_attachBefore(NW_TinyTree_Node_t *node, 
                         NW_TinyTree_Node_t *sibling)
{
  NW_TinyTree_Node_t *previous = NW_TinyTree_findPreviousSibling(node);

  NW_ASSERT(node != NULL);
  NW_ASSERT(sibling != NULL);

  /* Has a previous sibling, insert after */
  if(previous != NULL){
    NW_TinyTree_attachAfter(previous, sibling);
    return NW_STAT_SUCCESS;
  }

  /*Otherwise, insert as first child */
  previous = NW_TinyTree_findParent(node);
  if(previous == NULL)
     return NW_STAT_FAILURE; /* TODO: return a more descriptive error status */

  previous->first_child = sibling;
  /*lint -e{794} Conceivable use of null pointer */
  sibling->next_sibling = node;
  
  return NW_STAT_SUCCESS;
}

/*
* Attach a child (as last child) to a node 
*/

NW_Status_t
NW_TinyTree_attachChild(NW_TinyTree_Node_t *node, 
                        NW_TinyTree_Node_t *child)
{
  NW_TinyTree_Node_t *last_child = NW_TinyTree_findLastChild(node);
  
  NW_ASSERT(node != NULL);
  NW_ASSERT(child != NULL);

  /* If there are no children, attach as first child */
  if(last_child == NULL){
    node->first_child = child;
    /* Point child's next_sibling back to parent and set LASTSIBLING flag */
    child->next_sibling = node;
    child->flags |= TNODE_FLAG_LASTSIBLING;
    return NW_STAT_SUCCESS;
  }
  /* Else attach as sibling to last child */
  else{
    return NW_TinyTree_attachAfter(last_child, child);
  }
}

NW_Status_t
NW_TinyTree_deleteNode(NW_TinyTree_Node_t *node)
{
  NW_ASSERT(node != NULL);

  /* If not the root node, then adjust parent or sibling indexes */
  if((node->flags & TNODE_FLAG_ROOT)!= TNODE_FLAG_ROOT){
    NW_TinyTree_Node_t *previous_sibling;
    previous_sibling = NW_TinyTree_findPreviousSibling(node);
    /* If we are first child, modify parent's first_child index */
    if(previous_sibling == NULL){
      NW_TinyTree_Node_t *parent = NW_TinyTree_findParent(node);
      NW_ASSERT(parent != NULL);
      /* If also last child (i.e. only child) */
      if((node->flags & TNODE_FLAG_LASTSIBLING) == TNODE_FLAG_LASTSIBLING){
        /* Zero the parent's child index */
        parent->first_child = 0;
      }
      /* Not only child */
      else{
        /* Move parent child offset to my next sibling */
        parent->first_child = node->next_sibling;
      }
    }
    /* Not the first child */
    else {
      /* Adjust previous sibling next_sibling index */
      previous_sibling->next_sibling = node->next_sibling;
      if((node->flags & TNODE_FLAG_LASTSIBLING) == TNODE_FLAG_LASTSIBLING){
        previous_sibling->flags |= TNODE_FLAG_LASTSIBLING;
      }
    }
  }
  
 /* Note that we don't actually remove the node from the tree vector
  * since this may cause other nodes to be moved, invalidating any
  * references to them.
  */

  return NW_STAT_SUCCESS;
}

NW_Status_t
NW_TinyTree_setContext(NW_TinyTree_t *tree, 
                       void *context){
  NW_ASSERT(tree != NULL);
  tree->context = context;
  return NW_STAT_SUCCESS;
}

void*
NW_TinyTree_getContext(NW_TinyTree_t *tree){
  NW_ASSERT(tree != NULL);
  return tree->context;
}

NW_Uint16 
NW_TinyTree_Node_getFlags(NW_TinyTree_Node_t *node){
  NW_ASSERT(node != NULL);
  return node->flags;
}

NW_Status_t
NW_TinyTree_Node_setUserFlags(NW_TinyTree_Node_t *node, 
                              NW_Uint16 flags){
  NW_ASSERT(node != NULL);
  node->flags &= ~TNODE_USR_FLAGS; /*Zero user flags */
  node->flags |= (flags & TNODE_USR_FLAGS);
  return NW_STAT_SUCCESS;
}

NW_TinyTree_Offset_t
NW_TinyTree_Node_getSourceOffset(NW_TinyTree_Node_t *node){
  NW_ASSERT(node != NULL);

  return node->source_offset;
}

void *
NW_TinyTree_Node_getSourceAddress(NW_TinyTree_t *tree, 
                                  NW_TinyTree_Node_t* node){

  NW_ASSERT(node != NULL);
  NW_ASSERT(tree != NULL);
 
  /*lint -e{794} Conceivable use of null pointer */
  return (void *)(NW_TinyTree_EBuffer_AddressAt(tree->ebuffer, 
						node->source_offset));
}


NW_Status_t
NW_TinyTree_Node_GetSegmentAndOffset(NW_TinyTree_t* tree,
                                     NW_TinyTree_Node_t* node,
                                     NW_Uint8** segment,             
                                     NW_TinyTree_Offset_t* segSize, 
                                     NW_TinyTree_Offset_t* offset) {
  NW_ASSERT(tree != NULL);
  NW_ASSERT(node != NULL);
  
  return NW_TinyTree_EBuffer_GetSegmentAndOffset(tree->ebuffer, 
                                                 node->source_offset,
                                                 segment,
                                                 segSize,
                                                 offset);
}

NW_Status_t
NW_TinyTree_GetSourceOffset(NW_TinyTree_t* tree,
                            NW_Uint8*  segmentAddr,         
                            NW_TinyTree_Offset_t  offset,
                            NW_TinyTree_Offset_t* index){
  NW_ASSERT(tree != NULL);

  return NW_TinyTree_EBuffer_GetIndex(tree->ebuffer, segmentAddr, offset, index);
}



NW_Uint8*
NW_TinyTree_GetWritableBlock(NW_TinyTree_t* tree,
                             NW_TinyTree_Offset_t      size,
                             NW_TinyTree_Offset_t    * source_offset){  /* OUT */

  return NW_TinyTree_EBuffer_GetWritableBlock(tree->ebuffer, size, source_offset);

}

NW_TinyTree_t*
NW_TinyTree_Node_findTreeOld(NW_TinyTree_Node_t *node)
{
  NW_TinyTree_Node_t *parent = NW_TinyTree_findParent(node);
  NW_ASSERT(node != NULL);

  // find rootNode 
  while (parent != NULL)
  {
    node = parent;
    parent = NW_TinyTree_findParent(parent);
  }
  // root node should be next to sentinal tree node 
  while(node != NULL){
    if((node->flags & TNODE_FLAG_TREE) == TNODE_FLAG_TREE){
      return ((NW_TinyTree_TreeNode_t*)node)->tree;
    }
    --node;
  }
  // Not reached 
  NW_ASSERT(0);
  return NULL;
}

EXPORT_C NW_TinyTree_t*
NW_TinyTree_Node_findTree(NW_TinyTree_Node_t *node)
    {
    if (!node->tree)
        {
        return NW_TinyTree_Node_findTreeOld(node);
        }
    else
        {
        return node->tree;
        }
        
    }

/*
* Create a new node as a child of an existing node. The child
* references the source buffer at offset. The new child node is
* returned.  
*/

NW_TinyTree_Node_t*
NW_TinyTree_createChild(NW_TinyTree_t *tree, 
                        NW_TinyTree_Node_t *parent, 
                        NW_TinyTree_Offset_t offset){

  NW_TinyTree_Node_t *child = NW_TinyTree_createNode(tree, offset);

  if (child != NULL){
    NW_TinyTree_attachChild(parent, child);
  }
  
  return child;
}

/*
* Create a new node as an immediate sibling to an existing node. The
* child references the source buffer at offset. The new child node is
* returned.  
*/
NW_TinyTree_Node_t*
NW_TinyTree_createSibling(NW_TinyTree_t *tree, 
                          NW_TinyTree_Node_t *node, 
                          NW_TinyTree_Offset_t offset)
{
  NW_TinyTree_Node_t *sibling = NW_TinyTree_createNode(tree, offset);
  NW_TinyTree_attachAfter(node, sibling);
  return sibling;
}

void
NW_TinyTree_recurse(NW_TinyTree_t* tree,
                    NW_TinyTree_Node_t* start_node, 
                    void (*Node_CB) (NW_TinyTree_t*, NW_TinyTree_Node_t *, void *),
                    void * context)
{
  NW_TinyTree_Node_t *child;
  
  (*(Node_CB)) (tree, start_node, context);
  if((child = NW_TinyTree_findFirstChild(start_node)) != 0){
    NW_TinyTree_recurse(tree, child, Node_CB, context);
    while((child = NW_TinyTree_findNextSibling(child)) != 0){
      NW_TinyTree_recurse(tree, child, Node_CB, context);
    }
  }
}

/* Initialize a node iterator with a start node */

void
NW_TinyTree_NodeIterator_init(NW_TinyTree_Node_t* start_node, 
                              NW_TinyTree_NodeIterator_t* iterator)
{
  iterator->start_node = start_node; 
  iterator->traversal_node = NULL;
}

/* 
* Iterate through a subtree returning each node exactly once.
* The algorithm follows the toplogical ordering of the tree (if there is
* an edge from v0 to v1, always visit v0 before v1). It's equivalent to 
* solving a labyrinth by keeping your right hand on the wall!
*/

NW_TinyTree_Node_t*
NW_TinyTree_NodeIterator_iterate(NW_TinyTree_NodeIterator_t *iterator){
  
  NW_TinyTree_Node_t* node;

  if(iterator->start_node == NULL)
    return NULL;

  if(iterator->traversal_node == NULL){
    node = iterator->start_node;
    if(iterator->start_node->first_child != 0){
      iterator->traversal_node = iterator->start_node->first_child;
    }
    else{
      iterator->start_node = NULL;
    }
    return node;
  }

  node = iterator->traversal_node;
  
  /* First try to move down */
  if(iterator->traversal_node->first_child != 0){
    iterator->traversal_node = iterator->traversal_node->first_child;
    return node;
  }

  /* If you can't move down, move right */

  if((iterator->traversal_node->flags & TNODE_FLAG_LASTSIBLING) != TNODE_FLAG_LASTSIBLING){
    iterator->traversal_node = iterator->traversal_node->next_sibling;
    return node;
  }

  /* If you can't move right, move back up */
  /* TODO:  some other expression here */
  while(node != NULL){
    iterator->traversal_node = iterator->traversal_node->next_sibling;
    /* If you reached the start, quit */
    if(iterator->traversal_node == iterator->start_node){
      iterator->start_node = NULL;
      return node;
    }
    /* Otherwise, try to move right */
    if((iterator->traversal_node->flags & TNODE_FLAG_LASTSIBLING) != TNODE_FLAG_LASTSIBLING){
      iterator->traversal_node = iterator->traversal_node->next_sibling;
      return node;
    }
    /* Otherwise, continue to move up */
  }
  return NULL;
}

/* Iterate the siblings of a node */ 

NW_TinyTree_Node_t*
NW_TinyTree_NodeIterator_iterateSiblings(NW_TinyTree_NodeIterator_t *iterator){
  if((iterator->start_node->flags & TNODE_FLAG_LASTSIBLING)== TNODE_FLAG_LASTSIBLING)
    return NULL;
  else{
    iterator->start_node = iterator->start_node->next_sibling;
    return iterator->start_node;
  }
}





