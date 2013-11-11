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

#ifndef NW_TINYTREE_EBUFFER_H
#define NW_TINYTREE_EBUFFER_H

#include <xml/cxml/cxml_proj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ----------------------------------------------------------------------- **  
   EBuffers (expandable buffers) provide storage blocks of bytes that
   can be accessed by index. Tiny trees use these to store serialized
   tree data. Node source offsets are stored as ebuffer indexes. EBuffers
   are similar to segmented vectors, but with variable segment sizes so
   that continuous byte blocks of different sizes can be stored efficiently.
   Unlike vectors, EBuffers store only byte data, so alignment calculations
   are not needed. Also, EBuffers support appending, but not insertion and 
   deletion. These  simplifications make it much easier to deal with 
   variable segment sizes while guaranteeing that continuous blocks remain
   continuous.
** ----------------------------------------------------------------------- **/

/* ------------------------------------------------------------------------- */

#define NW_TINY_TREE_BLOCK_SIZE_DEFAULT    ((CXML_Vector_Metric_t) 256)


/** ----------------------------------------------------------------------- **
    @struct:       NW_TinyTree_Segment

    @synopsis:    Tiny tree data segment.

    @scope:       public
    @variables:
       NW_Uint8* storage
                  Data storage.

       CXML_Vector_Metric_t segmentSize
                  Size of segment.

       CXML_Vector_Metric_t freeOffset
                  Free offset.

    @description: Tiny tree data segment.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyTree_Segment_s {
  NW_Uint8* storage;
  NW_TinyTree_Offset_t segmentSize;
  NW_TinyTree_Offset_t freeOffset;
} NW_TinyTree_Segment_t;


/** ----------------------------------------------------------------------- **
    @struct:       NW_TinyTree_EBuffer

    @synopsis:    Tiny tree E buffer.

    @scope:       public
    @variables:
       NW_Bool ownFirstBlock
                  Ownership flag.

       CXML_Vector_Metric_t blockSize
                  Size of block,

       NW_TinyTree_Segment_t* segmentList
                  List of segments.

       CXML_Vector_Metric_t segmentListSize
                  Size of segment list.

       CXML_Vector_Metric_t numSegments
                  Number of segments.

    @description: EBuffers (expandable buffers) provide storage blocks
                  of bytes that can be accessed by index. Tiny trees use
                  these to store serialized tree data. Node source
                  offsets are stored as ebuffer indexes. EBuffers are
                  similar to segmented vectors, but with variable
                  segment sizes so that continuous byte blocks of
                  different sizes can be stored efficiently.  Unlike
                  vectors, EBuffers store only byte data, so alignment
                  calculations are not needed. Also, EBuffers support
                  appending, but not insertion and deletion. These
                  simplifications make it much easier to deal with
                  variable segment sizes while guaranteeing that
                  continuous blocks remain continuous.

 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyTree_EBuffer_s {
  NW_Bool ownFirstBlock;
  CXML_Vector_Metric_t blockSize;
  NW_TinyTree_Segment_t* segmentList;
  CXML_Vector_Metric_t segmentListSize;
  CXML_Vector_Metric_t numSegments;
} NW_TinyTree_EBuffer_t;

/* ------------------------------------------------------------------------- *
   public methods
 * ------------------------------------------------------------------------- */
 
NW_TinyTree_EBuffer_t*
NW_TinyTree_EBuffer_Construct (NW_Uint8* initialBuffer,
                               NW_TinyTree_Offset_t initBuffSize,
                               NW_TinyTree_Offset_t blockSize,
                               NW_Bool freeBuff);

void
NW_TinyTree_EBuffer_Destruct (NW_TinyTree_EBuffer_t* thisObj);

extern 
NW_Uint8*
NW_TinyTree_EBuffer_GetWritableBlock(NW_TinyTree_EBuffer_t* ebuffer,
                                     NW_TinyTree_Offset_t   size,
                                     NW_TinyTree_Offset_t*  index);


extern
NW_Status_t
NW_TinyTree_EBuffer_GetSegmentAndOffset(NW_TinyTree_EBuffer_t* ebuffer,
                                        NW_TinyTree_Offset_t index,
                                        NW_Uint8** segment,              /* OUT */
                                        NW_TinyTree_Offset_t* segSize,  /* OUT */
                                        NW_TinyTree_Offset_t* offset);  /* OUT */

extern
NW_Status_t
NW_TinyTree_EBuffer_GetIndex(NW_TinyTree_EBuffer_t* ebuffer,
                             NW_Uint8* segmentAddr,         
                             NW_TinyTree_Offset_t   offset,
                             NW_TinyTree_Offset_t * index); /* OUT */

extern
NW_Uint8*
NW_TinyTree_EBuffer_AddressAt(NW_TinyTree_EBuffer_t* ebuffer,
                              NW_TinyTree_Offset_t index);


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif /* NW_TINYTREE_EBUFFER_H */
