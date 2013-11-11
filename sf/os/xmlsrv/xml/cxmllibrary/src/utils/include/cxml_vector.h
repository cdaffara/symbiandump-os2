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


 /***************************************************************************
**   File: cxml_vector.h
**   Purpose:  Provides the interface to vector utils
**************************************************************************/

#ifndef CXML_VECTOR_H
#define CXML_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Includes
*/
#include "cxml_internal.h"

struct CXML_Vector_s {
  CXML_Vector_Metric_t elementSize;
  CXML_Vector_Metric_t capacity;
  CXML_Vector_Metric_t size;
  CXML_Vector_Metric_t segmentSize;
  CXML_Uint8** segmentList;
  CXML_Vector_Metric_t segmentListSize;
  CXML_Vector_Metric_t numSegments;
};

CXML_Vector_t*
CXML_Vector_Construct(CXML_Vector_Metric_t elementSize,
											CXML_Vector_Metric_t segmentSize);

void CXML_Vector_Destruct(CXML_Vector_t* vector);

CXML_Uint8*
CXML_Vector_AddressAt(const CXML_Vector_t* thisObj,
											CXML_Vector_Metric_t index);

void**
CXML_Vector_InsertAt(CXML_Vector_t* thisObj,
										 void* element,
										 CXML_Vector_Metric_t where,
										 void* sentinel);

NW_Status_t
CXML_Vector_RemoveAt(CXML_Vector_t* thisObj,
										 CXML_Vector_Metric_t index);

void**
CXML_Vector_ElementAt(const CXML_Vector_t* vector,
											CXML_Vector_Metric_t index);

CXML_Vector_Metric_t
CXML_Vector_GetElementIndex(const CXML_Vector_t* vector,
														void* target);

void
CXML_Vector_AdjustSegment(CXML_Vector_t* vector);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* CXML_VECTOR_H */

