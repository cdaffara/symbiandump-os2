/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cxml_internal.h"

/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
static
CXML_Uint8*
CXML_Vector_AllocateSegment (CXML_Vector_t* thisObj, NW_TinyTree_TreeNode_t* sentinel)
{
  CXML_Uint8* buffer = NULL;
	if(sentinel)
	{
		buffer = (NW_Uint8*) NW_Mem_Malloc ((thisObj->segmentSize + 1) * thisObj->elementSize);
		if(buffer)
		{
		NW_Mem_memcpy(buffer, sentinel, sizeof(NW_TinyTree_TreeNode_t));
		buffer += thisObj->elementSize;
   	        }
	}
	else
	{
		buffer = (NW_Uint8*) NW_Mem_Malloc (thisObj->segmentSize * thisObj->elementSize);
	}
	return buffer;
}

/* ------------------------------------------------------------------------- */
static
NW_Status_t
CXML_Vector_ResizeCapacity (CXML_Vector_t* thisObj,
							CXML_Vector_Metric_t capacityNeeded,
							NW_TinyTree_TreeNode_t* sentinel)
{
  CXML_Vector_Metric_t newNumSegments;
  CXML_Vector_Metric_t newSegmentListSize;
  NW_Uint8** newSegmentList;
 // CXML_Vector_Metric_t index;
  
  /* calculate the new segmentList size */
  newNumSegments =
    (CXML_Vector_Metric_t) ((capacityNeeded - 1) / thisObj->segmentSize + 1);
  newSegmentListSize = (CXML_Vector_Metric_t)
    (((newNumSegments - 1) / CXML_SEGMENT_LIST_INCREMENT + 1) * CXML_SEGMENT_LIST_INCREMENT);

  /* if we are shrinking the array, we must first deallocate all the segments
     that will be obsolete */
  while (thisObj->numSegments > newNumSegments) {
    NW_Mem_Free (thisObj->segmentList[--thisObj->numSegments]);
  }
  thisObj->capacity =
    (CXML_Vector_Metric_t) (thisObj->numSegments * thisObj->segmentSize);
  
  /* allocate the new segmentList and copy the old segmentList entries into the
     new */
  newSegmentList =
    (NW_Uint8**) NW_Mem_Malloc (newSegmentListSize * sizeof (*newSegmentList));
  if (newSegmentList == NULL) {
    return NW_STAT_OUT_OF_MEMORY; 
  }
  (void) NW_Mem_memcpy (newSegmentList, thisObj->segmentList,
                        thisObj->numSegments * sizeof (*newSegmentList));
 
  /* free the old segmentList and install the new */
 NW_Mem_Free (thisObj->segmentList);

  thisObj->segmentList = newSegmentList;
  thisObj->segmentListSize = newSegmentListSize;

  /* if we are growing the array we need to allocate the new segments */
  while (thisObj->numSegments < newNumSegments) {
    thisObj->segmentList[thisObj->numSegments] =
      CXML_Vector_AllocateSegment(thisObj, sentinel);
    if (thisObj->segmentList[thisObj->numSegments++] == NULL) {
      thisObj->numSegments -= 1;
      return NW_STAT_OUT_OF_MEMORY; 
    }
  }
  thisObj->capacity =
    (CXML_Vector_Metric_t) (thisObj->numSegments * thisObj->segmentSize);
  
  /* successful completion */
  return NW_STAT_SUCCESS;
}

/* ------------------------------------------------------------------------- */
static
NW_Status_t
CXML_Vector_MoveElements (CXML_Vector_t* thisObj,
													CXML_Vector_Metric_t srcIndex,
													CXML_Vector_Metric_t dstIndex,
													NW_TinyTree_TreeNode_t* sentinel)
{
  NW_Int32 sizeDelta;
  CXML_Vector_Metric_t numElements;
  NW_Int32 index;

  /* */
  if (dstIndex > srcIndex) {
    sizeDelta = dstIndex - srcIndex;
  } else {
    sizeDelta = - (NW_Int32) (srcIndex - dstIndex);
  }

  if (thisObj->size + sizeDelta > thisObj->capacity) {
    NW_Status_t status;

    status = CXML_Vector_ResizeCapacity (thisObj,
			(CXML_Vector_Metric_t) (thisObj->size + sizeDelta), sentinel);
    if (status != NW_STAT_SUCCESS) {
      return status;
    }
  }

  /* now do the actual move */
  /* TODO: this is a very inefficient way of moving the data, we will probably
     need to implement a block move capability */
  numElements = (CXML_Vector_Metric_t) (thisObj->size - srcIndex);
  if (srcIndex > dstIndex) {
    for (index = 0; index < NW_INT32_CAST(numElements); index++) {
      (void) NW_Mem_memcpy (CXML_Vector_AddressAt (thisObj, 
				(CXML_Vector_Metric_t) (dstIndex + index)), CXML_Vector_AddressAt (thisObj,
				(CXML_Vector_Metric_t) (srcIndex + index)), thisObj->elementSize);
    }
  } else {
    for (index = numElements - 1; index >= 0; index--) {
      (void) NW_Mem_memcpy (CXML_Vector_AddressAt (thisObj, 
				(CXML_Vector_Metric_t) (dstIndex + index)), CXML_Vector_AddressAt (thisObj, 
				(CXML_Vector_Metric_t) (srcIndex + index)), thisObj->elementSize);
    }
  }

  /* successful completion */
  return NW_STAT_SUCCESS;
}

CXML_Vector_t*
CXML_Vector_Construct(CXML_Vector_Metric_t elementSize,
											CXML_Vector_Metric_t segmentSize)
{
	CXML_Vector_t* vector = (CXML_Vector_t*) NW_Mem_Malloc(sizeof(CXML_Vector_t));

if(vector)
 {
	vector->elementSize = elementSize;
	vector->capacity = 0;
	vector->size = 0;

	vector->segmentSize = segmentSize;
	vector->segmentListSize = CXML_SEGMENT_LIST_INCREMENT;
	
	//Allocate memory for one segment here. For more memory the
	//function CXML_Vector_ResizeCapacity() will do the job.

	vector->segmentList = (NW_Uint8**)
              NW_Mem_Malloc (vector->segmentListSize * sizeof (*vector->segmentList));

  if (vector->segmentList == NULL) {
		NW_Mem_Free(vector);
                return NULL;
  }
vector->numSegments = 0;
	}
	return vector;
}

void
CXML_Vector_Destruct(CXML_Vector_t* vector)
{
	CXML_Vector_Metric_t index;
	for (index = 0; index < vector->numSegments; index++) {
		NW_Mem_Free (vector->segmentList[index]);
	}
	NW_Mem_Free (vector->segmentList);
	NW_Mem_Free(vector);
}

void
CXML_Vector_AdjustSegment(CXML_Vector_t* vector)
{
	CXML_Vector_Metric_t index;

  /* 
   * Walk through segment list adjusting pointers to the 
   * sentinel element at the beginning of each segment
   */
  
  for (index = 0; index < vector->numSegments; index++) {
    vector->segmentList[index] -= vector->elementSize; 
  }
}

NW_Uint8*
CXML_Vector_AddressAt (const CXML_Vector_t* thisObj,
											 CXML_Vector_Metric_t index)
{
  CXML_Vector_Metric_t segmentIndex;

  /* determine the segment index and return the offset into that segment */
  segmentIndex =
    (CXML_Vector_Metric_t) (index / thisObj->segmentSize);
  return
    (NW_Uint8*) thisObj->segmentList[segmentIndex]
    + (index % thisObj->segmentSize) * thisObj->elementSize;
}

/* ------------------------------------------------------------------------- */
void**
CXML_Vector_InsertAt (CXML_Vector_t* thisObj,
											void* element,
											CXML_Vector_Metric_t where,
											void* sentinel)
{
  NW_Status_t status;

  /* convert the where if CXML_Vector_AtEnd is specified */
  if (where == CXML_Vector_AtEnd) {
    where = thisObj->size;
  }

  /* make sure that the where element is not out of bounds */
  NW_ASSERT (where <= thisObj->size);

  /* move all the elements up by one, if this fails we simply return
     the error code passed to us */
  status =
    CXML_Vector_MoveElements (thisObj, where,
                              (CXML_Vector_Metric_t) (where + 1),
                              (NW_TinyTree_TreeNode_t*) sentinel);
  if (status != NW_STAT_SUCCESS) {
    return NULL;
  }

  /* copy the element into vector */
  if (element != NULL) {
    (void) NW_Mem_memcpy (CXML_Vector_AddressAt (thisObj, where),
			element, thisObj->elementSize);
  } else {
    /*
     * if element is NULL, then we need to zero out the memory block.  This is necessary
	 * because later code which fills in the values for this newly allocated vector
	 * element may leave some bytes in the memory block un-assigned due to padding.
	 */
    NW_Mem_memset ( CXML_Vector_AddressAt (thisObj, where), 
			0, thisObj->elementSize);
  }

  /* increment the size count */
  thisObj->size += 1;

  /* successful completion */
  return (void**)
    CXML_Vector_AddressAt (thisObj, where);
}

/* ------------------------------------------------------------------------- */
NW_Status_t
CXML_Vector_RemoveAt (CXML_Vector_t* thisObj,
											CXML_Vector_Metric_t index)
{
  NW_Status_t status;

  /* convert the index if CXML_Vector_AtEnd is specified */
  if (index == CXML_Vector_AtEnd) {
    index = (CXML_Vector_Metric_t) (thisObj->size -  1);
  }

  /* make sure that the index element is not out of bounds */
  if (index >= thisObj->size) {
    return NW_STAT_FAILURE;
  }

  /* don't bother to move anything if the resultant size is zero */
  if (thisObj->size > 1) {
    /* move all the elements down by one, if this fails we simply return
       the error code passed to us */
    status =
      CXML_Vector_MoveElements (thisObj, (CXML_Vector_Metric_t) (index + 1),
																index, NULL);
    if (status != NW_STAT_SUCCESS) {
      return status;
    }
  }

  /* increment the size count */
  thisObj->size -= 1;

  /* successful completion */
  return NW_STAT_SUCCESS;
}

/* ------------------------------------------------------------------------- */
void**
CXML_Vector_ElementAt (const CXML_Vector_t* vector,
											 CXML_Vector_Metric_t index)
{
  if (index >= vector->size) {
    return NULL;
  }
  return (void**) CXML_Vector_AddressAt (vector, index);
}

/* ------------------------------------------------------------------------- */
CXML_Vector_Metric_t
CXML_Vector_GetElementIndex (const CXML_Vector_t* vector,
														 void* target)
{
  CXML_Vector_Metric_t index;

  for (index = 0; index < vector->size; index++) {
    void* element;

    /* get and compare the element */
    element = CXML_Vector_ElementAt (vector, index);
    if (NW_Mem_memcmp (target, element, vector->elementSize) == 0) {
      return index;
    }
  }

  /* no match found, return CXML_Vector_AtEnd */
  return CXML_Vector_AtEnd;
}

