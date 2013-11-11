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
#include <xml/cxml/nw_tinytree.h>
#include "nw_tinytree_alloc.h"

/* 
* This is not a generalized allocator. It is intended to support
* dynamic extension of the node array or storage buffers associated
* with a tiny tree. The goal of the design is to provide a kind of
* virtual array whose storage can be allocated in several
* non-contiguous segments located anywhere in memory. Since there
* will be gaps between segments and segments may be allocated in
* out-of-order locations (for example a second segment may be
* allocated at an address lower than the first segment) simple
* pointer arithmetic and array indexing cannot be used to address
* array elements. However, rather than trying to provide a totally
* general non-contiguous array package here, certain limitations have
* been imposed. These simplify the implementation but mean that this
* module can only be used with certain constraints. These constraints
* are not currently a problem for the tiny dom parser, but any change
* in the way the parser uses this module must be done with extreme
* care.  Eventually, we may want to generalize this package if this
* can be done without adding too much to the footprint of computing
* burden.
* 
* The main constraint is that any code which writes to or reads from
* a dynamically extended array must be sure that operations involving
* ordinary pointer arithmetic and array indexing always occur within
* the boudaries of a single segment.  Operations that may result in
* crossing a segment boundary must use the supplied address and
* offset increment methods (which can be thought of as operator
* overloads for the += operator). Furthermore, it needs to be
* understood that any increment which results in crossing a segment
* boundary may result in the allocation of a new segment if the
* resulting address is not within an existing segment.  When an
* address or offset is incremented to a new segment the result will
* be adjusted to the new segment and may have an unexpected value
* (for example, incrementing address x by a positive increment may
* result in an address that is less than x.). One important rule is
* that the result of incrementing x by some value i is not guaranteed
* to be idempotent if the increment crosses a segment boundary: i.e
* addressIncrement(x,i) == addressIncrement(x,i) is not guaranteed to
* be true. Future references to an address that results from an
* increment operations must always use the result of the
* operation. So, for example, (j = addressIncrement(x,i)) ==
* addressIncrement(x,x+j) is guaranteed to be true.
*
* Segment storage addresses are always padded to align with the size
* of the data object to be stored. This means that segments allocated
* for a specific object type must be treated as as arrays of that
* object type (or as arrays of bytes).
* 
* The parser code always uses this module according to these rules.
* Specifically, no tree node crosses a segment boundary and no
* parsable fragment of wbxml (a fragment which the parser can
* complete) every crosses a segment boundary. This latter condition
* allows the parser to treat its current buffer as a simple array of
* bytes.  Another rule is that all of the offsets stored in tree
* nodes are guaranteed to address an allocated segment. For example,
* when writing a node, the storage offset is incremented using the
* offset increment * operation. The resulting offsets (stored in
* nodes) can then be safely used to address the written-to storage.
* 
*/

/* 
* Allocate segments for buffer and node array storage. The base
* segment address is the one from which all relative offsets are
* calculated.  Segments are probably not contiguous, and, given that
* new segments might be allocated anywhere relative to existing
* segments, the offset must be a relative one.  This limits the
* maximum relative offset to the beginning of a new segment to be a
* signed integer of the offset type.  
*/

#include <limits.h>
#define MAX_REL_OFFSET INT_MAX

static
NW_TinyTree_SegHeader_t* 
NW_TinyTree_findLastSegment(NW_TinyTree_SegHeader_t *base){
  NW_TinyTree_SegHeader_t *last_seg = base;
  while(last_seg->next != 0)
  {
    last_seg = (NW_TinyTree_SegHeader_t*)((NW_Byte*)base + last_seg->next);
  }
  return last_seg;
}


NW_TinyTree_Offset_t
NW_TinyTree_segmentGetFreeSpace(NW_TinyTree_SegHeader_t *segment){
  return (NW_TinyTree_Offset_t)(segment->size-segment->free_offset);
}


NW_TinyTree_Offset_t
NW_TinyTree_getFreeStorageSpace(NW_TinyTree_SegHeader_t *base){
  NW_TinyTree_SegHeader_t *last_seg = NW_TinyTree_findLastSegment(base);
  return NW_TinyTree_segmentGetFreeSpace(last_seg);
}

NW_TinyTree_SegHeader_t* 
NW_TinyTree_addSegment(NW_TinyTree_SegHeader_t *base, 
                       NW_TinyTree_Offset_t size){
  NW_TinyTree_SegHeader_t *new_seg;
  NW_TinyTree_SegHeader_t *last_seg = NW_TinyTree_findLastSegment(base); 
  NW_Int32 offset;

  /* The extra node is added to make sure we have space to pad the segment storage to an even node boundary */
  NW_Uint32 alloc_size = size + sizeof(NW_TinyTree_SegHeader_t) + sizeof(NW_TinyTree_Node_t);
  new_seg = (NW_TinyTree_SegHeader_t*)NW_Mem_Malloc(alloc_size);
  
  if(new_seg != 0){
    offset = (NW_Byte*)new_seg - (NW_Byte*)base;
    if(abs(offset) > MAX_REL_OFFSET){
      NW_Mem_Free(new_seg);
      return 0;
    }
    NW_Mem_memset(new_seg, 0, alloc_size);
    /* Shift the storage pointer to an even boundary of a node so we can use this as an array of nodes */
    new_seg->initializer = base->initializer;
    last_seg->next = (NW_TinyTree_RelativeOffset_t)offset;
    new_seg->size = size;
    new_seg->free_offset = 0;
    new_seg->storage = (NW_Uint8*)
      (((((NW_Uint32) new_seg) + sizeof(NW_TinyTree_SegHeader_t) - 1)
        / sizeof (NW_TinyTree_Node_t) + 1)
       * sizeof (NW_TinyTree_Node_t));
    if(new_seg->initializer)
      (*(new_seg->initializer))(new_seg->storage, size);
  }
  return new_seg;
}

/*
* Free segments allocated by addSegment only.
*/

void 
NW_TinyTree_freeSegments(NW_TinyTree_SegHeader_t *base){
  NW_TinyTree_SegHeader_t *lastSegment = NULL;
  NW_TinyTree_SegHeader_t *current = base;
  NW_TinyTree_SegHeader_t *previous = NULL;

  NW_Uint16 totalAdditionalSegments = 0;
  NW_Uint16 index = 0;
  NW_Uint16 i = 0;

  while (current->next != 0)
  {
    totalAdditionalSegments++; 
    current = (NW_TinyTree_SegHeader_t*)((NW_Byte*)base + current->next); 
  }
  lastSegment = current;

  while(index< totalAdditionalSegments)
  {
    current = base;
    i = 0;
    while (i < (totalAdditionalSegments - index))
    {
      previous = current;
      current = (NW_TinyTree_SegHeader_t*)((NW_Byte*)base + current->next);   
      i++;
    }
    NW_Mem_Free(lastSegment);
    lastSegment = previous;
    index++;
  }

}


/*
* Get the segment header associated with an offset. This allocates a new
* segment if the offset is beyond any currently allocated segment. If a new
* segment is allocated, the offset is readjusted to the beginning of the new
* segment.  
*/

NW_TinyTree_SegHeader_t*
NW_TinyTree_addressGetSegment(NW_TinyTree_SegHeader_t *base, 
                              NW_Byte **address){
  
  NW_TinyTree_SegHeader_t *segment = base;
  while(segment != 0){
    if ((*address > segment->storage) && (*address < (segment->storage + segment->size)))
      return segment;
    if (segment->next == 0){ /* Add new segment */
      segment = NW_TinyTree_addSegment(base, segment->size); /* Same size as last segment */
      if(segment == 0){
        break;
      }
      /* 
      * Reset address to beginning of new segment storage.
      */
      *address = segment->storage;
      return segment;
    }
    segment = (NW_TinyTree_SegHeader_t*)((NW_Byte*)base + segment->next);
  }
  return 0;
}


NW_Byte*
NW_TinyTree_addressIncrement(NW_TinyTree_SegHeader_t *base, 
                             NW_Byte *address, 
                             NW_TinyTree_Offset_t delta){
  NW_Byte* new_address = address + delta;
  NW_TinyTree_SegHeader_t *segment = NW_TinyTree_addressGetSegment(base, &new_address);
  if(new_address >= (segment->storage + segment->free_offset)) /* Haven't touched this memory before */
  {
    segment->free_offset = (NW_TinyTree_Offset_t)(new_address - segment->storage);
  }
  return new_address;
}

/*
* Get the segment header associated with an offset. This allocates a new
* segment if the offset is beyond any currently allocated segment. If a new
* segment is allocated, the offset is readjusted to the beginning of the new
* segment.  
*/

NW_TinyTree_SegHeader_t*
NW_TinyTree_offsetGetSegment(NW_TinyTree_SegHeader_t *base, 
                             NW_TinyTree_Offset_t *offset){
  
  NW_TinyTree_SegHeader_t *segment = base;
  while(segment != 0){
    if (((base->storage + *offset) > segment->storage) && ((base->storage + *offset) < (segment->storage + segment->size)))
      return segment;
    if (segment->next == 0){ /* Add new segment */
      if (segment->size > MIN_SEGMENT_SIZE)
        segment = NW_TinyTree_addSegment(base, segment->size); /* Same size as last segment */
      else
        segment = NW_TinyTree_addSegment(base, MIN_SEGMENT_SIZE);
      if(segment == 0){
        break;
      }
      /* 
      * Reset offset to beginning of new segment storage.
      */
      *offset = (NW_TinyTree_Offset_t)(segment->storage - base->storage);
      return segment;
    }
    segment = (NW_TinyTree_SegHeader_t*)((NW_Byte*)base + segment->next);
  }
  return 0;
}

NW_TinyTree_Offset_t
NW_TinyTree_offsetIncrement(NW_TinyTree_SegHeader_t *base, 
                            NW_TinyTree_Offset_t offset, 
                            NW_TinyTree_Offset_t delta){
  NW_TinyTree_Offset_t new_offset = (NW_TinyTree_Offset_t)(offset + delta);
  NW_TinyTree_SegHeader_t *segment = NW_TinyTree_offsetGetSegment(base, &new_offset);
  if(base->storage + new_offset >= (segment->storage + segment->free_offset)) /* Haven't touched this memory before */
  {
    segment->free_offset = (NW_TinyTree_Offset_t)(base->storage + new_offset - segment->storage);
  }
  return new_offset;
}











