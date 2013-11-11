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

#ifndef NW_TREE_ALLOC_H
#define NW_TREE_ALLOC_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define MIN_SEGMENT_SIZE 64

/* ----------------------------------------------------------------------- **
   These methods are private to the tiny tree module.
** ----------------------------------------------------------------------- **/

 
/* ----------------------------------------------------------------------- **
   Storage mananagement:

   Header for a storage block. When the block is allocated by the
   tree_alloc methods, the storage block will be contiguous with the
   header. However, when the block was supplied externally (for
   example, by the parser) the storage will be somewhere else.  
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @typedef:     NW_TinyTree_RelativeOffset

    @synopsis:    Relative offset into structure.

    @scope:       public
    @type:        NW_Uint32

    @description: Relative offset into structure.
 ** ----------------------------------------------------------------------- **/
typedef NW_Uint32 NW_TinyTree_RelativeOffset_t;


/** ----------------------------------------------------------------------- **
    @struct:      NW_TinyTree_SegHeader

    @synopsis:    Segment header.

    @scope:       public
    @variables:
       NW_TinyTree_RelativeOffset_t next
                  Offset into segment.

       NW_TinyTree_Offset_t size
                  Offset.

       NW_TinyTree_Offset_t free_offset
                  Free offset.

       void* initializer NW_Byte__ptr__NW_TinyTree_Offset_t
                  Funcion pointer for initializer.

       NW_Byte* storage
                  Storage.

    @description: Segment header.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyTree_SegHeader_s{
  NW_TinyTree_RelativeOffset_t next;
  NW_TinyTree_Offset_t size;
  NW_TinyTree_Offset_t free_offset;
  void (*initializer) (NW_Byte*, NW_TinyTree_Offset_t);
  NW_Byte* storage;
} NW_TinyTree_SegHeader_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_segmentGetFreeSpace

    @synopsis:    Get free space.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_SegHeader_t* segment
                  The segment header.

    @description: Get free space.

    @returns:     NW_TinyTree_Offset_t
                  Free space pointer.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Offset_t 
NW_TinyTree_segmentGetFreeSpace(NW_TinyTree_SegHeader_t* segment);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_getFreeStorageSpace

    @synopsis:    Get free storage space.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_SegHeader_t* base
                  The segment header.

    @description: Get free storage space.

    @returns:     NW_TinyTree_Offset_t
                  Free storage space pointer.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Offset_t
NW_TinyTree_getFreeStorageSpace(NW_TinyTree_SegHeader_t* base);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_addSegment

    @synopsis:    Add segment.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_SegHeader_t* base
                  The segment header.

       [in] NW_TinyTree_Offset_t size
                  Size of new segment.

    @description: Add segment.

    @returns:     NW_TinyTree_SegHeader_t*
                  New header.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_SegHeader_t*
NW_TinyTree_addSegment(NW_TinyTree_SegHeader_t* base, NW_TinyTree_Offset_t size);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_freeSegments

    @synopsis:    Free segments.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_SegHeader_t* base
                  The segment header.

    @description: Free segments.
 ** ----------------------------------------------------------------------- **/
void 
NW_TinyTree_freeSegments(NW_TinyTree_SegHeader_t* base);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_offsetGetSegment

    @synopsis:    Get segment header.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_SegHeader_t* base
                  The segment header.

       [out] NW_TinyTree_Offset_t* offset
                  Data offset.

    @description: Get the segment header associated with an offset. This 
                  allocates a new segment if the offset is beyond any 
                  currently allocated segment. If a new segment is 
                  allocated, the offset is readjusted to the beginning of 
                  the new segment.

    @returns:     NW_TinyTree_SegHeader_t*
                  Segment header or NULL if error.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_SegHeader_t*
NW_TinyTree_offsetGetSegment(NW_TinyTree_SegHeader_t* base, NW_TinyTree_Offset_t* offset);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_offsetIncrement

    @synopsis:    Increment.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_SegHeader_t* base
                  The segment header.

       [in] NW_TinyTree_Offset_t offset
                  Offset.

       [in] NW_TinyTree_Offset_t delta
                  Delta.

    @description: Increment.

    @returns:     NW_TinyTree_Offset_t
                  New offset.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_Offset_t
NW_TinyTree_offsetIncrement(NW_TinyTree_SegHeader_t* base, 
            NW_TinyTree_Offset_t offset, 
            NW_TinyTree_Offset_t delta);


/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_addressGetSegment

    @synopsis:   Get segment.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_SegHeader_t* base
                  The segment header.

       [out] NW_Byte** address
                  Address of segment.

    @description: Get the segment header associated with an offset. 
                  This allocates a new segment if the offset is beyond 
                  any currently allocated segment. If a new segment is 
                  allocated, the offset is readjusted to the beginning 
                  of the new segment.  

    @returns:     NW_TinyTree_SegHeader_t*
                  The segment header.

 ** ----------------------------------------------------------------------- **/
NW_TinyTree_SegHeader_t*
NW_TinyTree_addressGetSegment(NW_TinyTree_SegHeader_t* base, 
                              NW_Byte** address);
  

/** ----------------------------------------------------------------------- **
    @function:    NW_TinyTree_addressIncrement

    @synopsis:    Increment.

    @scope:       public

    @parameters:
       [in] NW_TinyTree_SegHeader_t* base
                  The segment header.

       [in] NW_Byte* address
                  Address to increment.

       [in] NW_TinyTree_Offset_t delta
                  Delta.

    @description: Increment.

    @returns:     NW_Byte*
                  New address.

 ** ----------------------------------------------------------------------- **/
NW_Byte*
NW_TinyTree_addressIncrement(NW_TinyTree_SegHeader_t* base, 
             NW_Byte* address, 
             NW_TinyTree_Offset_t delta);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_TREE_ALLOC_H */
