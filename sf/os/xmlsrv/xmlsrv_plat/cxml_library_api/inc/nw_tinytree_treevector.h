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

#ifndef NW_TINYTREE_TREEVECTOR_H
#define NW_TINYTREE_TREEVECTOR_H

#include <xml/cxml/cxml_proj.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

typedef struct NW_TinyTree_TinyTree_s NW_TinyTree_t;

/** ----------------------------------------------------------------------- **
    @struct:       NW_TinyTree_TreeVector_t

    @synopsis:    default

    @scope:       public
    @variables:
       NW_TinyTree_t* tree
                  default

    @description: default
 ** ----------------------------------------------------------------------- **/
typedef struct NW_TinyTree_TreeVector_s {
  CXML_Vector_t* vector;
  NW_TinyTree_t* tree; /* Reference back to parent tree */
  NW_Int32 lastValid; // mark the end of valid wbxml, rest of buffer maybe faked close tags
                      // put this member in this structure to maintain the binary compatibility.
} NW_TinyTree_TreeVector_t;

extern
NW_TinyTree_TreeVector_t*
NW_TinyTree_TreeVector_Construct (CXML_Vector_Metric_t elementSize,
                            CXML_Vector_Metric_t segmentSize,
                            NW_TinyTree_t* tree);

void
NW_TinyTree_TreeVector_Destruct (NW_TinyTree_TreeVector_t* thisObj);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif /* NW_TINYTREE_TREEVECTOR_H */
