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
#include <xml/cxml/nw_tinytree_ebuffer.h>

/* ------------------------------------------------------------------------- *
   private methods
 * ------------------------------------------------------------------------- */

/* Get the index corresponding to the first element in a segment.
 * This can be added to an offset within the segment to get the
 * index of any element
 */
 
static
CXML_Vector_Metric_t
NW_TinyTree_EBuffer_GetFirstIndex(NW_TinyTree_EBuffer_t* ebuffer,
																	CXML_Vector_Metric_t segNum)
{
  CXML_Vector_Metric_t currentSegment;
  NW_TinyTree_Segment_t* segment;
  CXML_Vector_Metric_t firstIndex = 0;

  NW_ASSERT(segNum < ebuffer->numSegments);

  for (currentSegment = 0; currentSegment < segNum; currentSegment++){
    segment = &(ebuffer->segmentList[currentSegment]);
    firstIndex = (CXML_Vector_Metric_t)(firstIndex + segment->segmentSize);
  }

  return firstIndex;
}

/* Find a free block of a given size. This duplicates a bit of code from
 * GetFirstIndex in order to avoid repeated iteration over the buffer list.
 */

static
NW_Uint8*
NW_TinyTree_EBuffer_FindFreeBlock(NW_TinyTree_EBuffer_t* ebuffer,
																	CXML_Vector_Metric_t size,
																	CXML_Vector_Metric_t *index)      /* OUT */
{
  CXML_Vector_Metric_t currentSegment;
  NW_TinyTree_Segment_t* segment;
  CXML_Vector_Metric_t firstIndex = 0;
  NW_Uint8* storage;

  *index = CXML_Vector_AtEnd;

  for (currentSegment = 0; currentSegment < ebuffer->numSegments; currentSegment++){
    segment = &(ebuffer->segmentList[currentSegment]);

    if((segment->segmentSize - segment->freeOffset) >= size){
      *index = (CXML_Vector_Metric_t)(firstIndex + segment->freeOffset);
      storage = segment->storage + segment->freeOffset;
      segment->freeOffset = (CXML_Vector_Metric_t)(segment->freeOffset + size);
      return storage;
    }
    firstIndex = (CXML_Vector_Metric_t)(firstIndex + segment->segmentSize);
  }
  return 0;
}

static
NW_Status_t
NW_TinyTree_EBuffer_GrowSegmentList(NW_TinyTree_EBuffer_t* ebuffer){

  CXML_Vector_Metric_t newSegmentListSize;
  NW_TinyTree_Segment_t* newSegmentList;

 
  /* Since any new allocation is a continuous block contained in a
   * single segment, we grow the segment list by a constant amount 
  */

  newSegmentListSize = 
    (NW_Uint8)(ebuffer->segmentListSize + CXML_SEGMENT_LIST_INCREMENT);

  /* if(newSegmentListSize == 0) then it means that there is overflow in the 
   * newSegmentListSize so return with error here.
   */

  if(newSegmentListSize == 0)
  {
   return NW_STAT_OUT_OF_MEMORY;
  }


  newSegmentList =
    (NW_TinyTree_Segment_t*) NW_Mem_Malloc (newSegmentListSize * sizeof (*newSegmentList));

  if (newSegmentList == NULL) {
    return NW_STAT_OUT_OF_MEMORY;
  }
 
  /* Copy the old segment list */

  (void) NW_Mem_memcpy (newSegmentList, ebuffer->segmentList,
                        ebuffer->numSegments * sizeof (*newSegmentList));
  
  /* Free the old segment list and install the new */

  NW_Mem_Free (ebuffer->segmentList);
  ebuffer->segmentList = newSegmentList;
  ebuffer->segmentListSize = newSegmentListSize;
  return NW_STAT_SUCCESS;
}


/* 
 * Allocate a new segment that holds a block of a given size.
 */

static
NW_Uint8*
NW_TinyTree_EBuffer_AllocSegment(NW_TinyTree_EBuffer_t* ebuffer,
                                  CXML_Vector_Metric_t size,
                                  CXML_Vector_Metric_t *index)      /* OUT */
{
  NW_TinyTree_Segment_t* segment;

  *index = CXML_Vector_AtEnd;
  
  /* Make sure the segment list is big enough to hold the new segment */

  if(ebuffer->numSegments == ebuffer->segmentListSize){
    NW_TinyTree_EBuffer_GrowSegmentList(ebuffer);
  }

  /* The new segment is the first unused segment in the list */
  segment = &(ebuffer->segmentList[ebuffer->numSegments]);

  /* Increment the segment count */
  ebuffer->numSegments = (CXML_Vector_Metric_t)(ebuffer->numSegments + 1);

  /* Allocate segment storage with enough contiguous blocks to hold the requested size */

  segment->segmentSize = 
    (CXML_Vector_Metric_t)(((size - 1) / ebuffer->blockSize + 1) * ebuffer->blockSize);
  
  segment->storage = (NW_Uint8*)(NW_Mem_Malloc(segment->segmentSize));
  
  if(segment->storage == 0){
    segment->segmentSize = 0;
    return 0;
  }

  /* Free offset goes at end of allocated block */
  segment->freeOffset = size;

  /* Get the first index of the newly allocated segment */
  *index = NW_TinyTree_EBuffer_GetFirstIndex(ebuffer, (CXML_Vector_Metric_t)(ebuffer->numSegments - 1));

  return segment->storage;
}



/* ------------------------------------------------------------------------- *
   virtual methods
 * ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
NW_TinyTree_EBuffer_t*
NW_TinyTree_EBuffer_Construct (NW_Uint8* initialBuffer,
															 CXML_Vector_Metric_t initBuffSize,
															 CXML_Vector_Metric_t blockSize,
															 NW_Bool freeBuff)
{
	NW_TinyTree_EBuffer_t* thisObj = (NW_TinyTree_EBuffer_t*)
		NW_Mem_Malloc(sizeof(NW_TinyTree_EBuffer_t));

	if(thisObj)
	{
		/* initialize the object */
	 thisObj->blockSize = blockSize;
	 thisObj->ownFirstBlock = freeBuff;
	 thisObj->segmentListSize = CXML_SEGMENT_LIST_INCREMENT;
	 NW_ASSERT(thisObj->segmentListSize > 0);
	 thisObj->segmentList = (NW_TinyTree_Segment_t*)
	 NW_Mem_Malloc (thisObj->segmentListSize * sizeof (*thisObj->segmentList));
	 if (thisObj->segmentList == NULL) 
	 {
		NW_Mem_Free(thisObj);
        return NULL;
	 }
	 if (initialBuffer != NULL)
   {
     NW_ASSERT(initBuffSize != 0);
     thisObj->segmentList[0].storage = initialBuffer;
     thisObj->segmentList[0].segmentSize = initBuffSize;
     thisObj->segmentList[0].freeOffset = initBuffSize;
     thisObj->numSegments = 1;
   }
	 else 
   {
		/* If there's no first block, then we ignore the free flag passed in 
		 * since we will be allocating the first block and thus must free it
		*/
		thisObj->ownFirstBlock = NW_TRUE;  
		thisObj->numSegments = 0;
   }
	}
  return thisObj;
}

/* ------------------------------------------------------------------------- */
void
NW_TinyTree_EBuffer_Destruct (NW_TinyTree_EBuffer_t* thisObj)
{
  CXML_Vector_Metric_t index;

  /* release our resources */
  for (index = 0; index < thisObj->numSegments; index++) {
    if((index == 0) && (thisObj->ownFirstBlock == NW_FALSE)){
      continue;
    }
    NW_Mem_Free (thisObj->segmentList[index].storage);
  }
  NW_Mem_Free (thisObj->segmentList);
  NW_Mem_Free (thisObj);
}


/* ------------------------------------------------------------------------- *
   final methods
 * ------------------------------------------------------------------------- */


/* Get an unused block of a given size. Returns the address of the
 * block. Also returns an index (via the index OUT parameter) that can
 * be used for later lookups of the block.  
*/
 
NW_Uint8*
NW_TinyTree_EBuffer_GetWritableBlock(NW_TinyTree_EBuffer_t* ebuffer,
                                     CXML_Vector_Metric_t size,
                                     CXML_Vector_Metric_t *index)      /* OUT */
{
  NW_Uint8* storage;

  NW_ASSERT(ebuffer != NULL);
  
  /* First try to get an existing free block that's big enough */

  storage = NW_TinyTree_EBuffer_FindFreeBlock(ebuffer, size, index);
  if(storage != 0){
    return storage;
  }

  /* Otherwise, allocate a new segment that holds a block of the right size */

  return NW_TinyTree_EBuffer_AllocSegment(ebuffer, size, index); 
}


/* Get a segment address and an offset corresponding to a given
 * index. Also returns the segment size.
 */

NW_Status_t
NW_TinyTree_EBuffer_GetSegmentAndOffset(NW_TinyTree_EBuffer_t* ebuffer,
                                        CXML_Vector_Metric_t index,
                                        NW_Uint8            ** segmentAddr, /* OUT */
                                        CXML_Vector_Metric_t* segSize,  /* OUT */
                                        CXML_Vector_Metric_t* offset){  /* OUT */

  CXML_Vector_Metric_t currentSegment;
  NW_TinyTree_Segment_t* segment;
  CXML_Vector_Metric_t firstIndex = 0;
  
  NW_ASSERT(ebuffer != NULL);
  NW_ASSERT(segmentAddr != NULL);
  NW_ASSERT(segSize != NULL);
  NW_ASSERT(offset != NULL);

  for (currentSegment = 0; currentSegment < ebuffer->numSegments; currentSegment++){
    segment = &(ebuffer->segmentList[currentSegment]);
    if(index < (firstIndex + segment->segmentSize)){
      *segmentAddr  = segment->storage;
      *segSize = segment->segmentSize;
      *offset = (CXML_Vector_Metric_t)(index - firstIndex);
      return NW_STAT_SUCCESS;
    }
    firstIndex = (CXML_Vector_Metric_t)(firstIndex + segment->segmentSize);
  }

  return NW_STAT_FAILURE;
}

/* Get an index from a segment and offset */

NW_Status_t
NW_TinyTree_EBuffer_GetIndex(NW_TinyTree_EBuffer_t* ebuffer,
                             NW_Uint8*  segmentAddr,         
                             CXML_Vector_Metric_t offset,
                             CXML_Vector_Metric_t* index) /* OUT */

{

  CXML_Vector_Metric_t currentSegment;
  NW_TinyTree_Segment_t* segment;
  CXML_Vector_Metric_t firstIndex = 0;
  
  NW_ASSERT(ebuffer != NULL);

  for (currentSegment = 0; currentSegment < ebuffer->numSegments; currentSegment++){
    segment = &(ebuffer->segmentList[currentSegment]);
    if(segmentAddr == segment->storage){
      *index = (CXML_Vector_Metric_t)(firstIndex + offset);
      return NW_STAT_SUCCESS;
    }
    firstIndex = (CXML_Vector_Metric_t)(firstIndex + segment->segmentSize);
  }

  return NW_STAT_FAILURE;
}


/* Get the address corresponding to a given index */

NW_Uint8*
NW_TinyTree_EBuffer_AddressAt(NW_TinyTree_EBuffer_t* ebuffer,
                              CXML_Vector_Metric_t index){

  NW_Uint8* segmentAddr;
  CXML_Vector_Metric_t segSize;
  CXML_Vector_Metric_t offset;
  NW_Status_t status;

  NW_ASSERT(ebuffer != NULL);

  status = NW_TinyTree_EBuffer_GetSegmentAndOffset(ebuffer, index, &segmentAddr, &segSize, &offset);
  if(status == NW_STAT_SUCCESS){
    return segmentAddr + offset;
  }
  return 0;

}
