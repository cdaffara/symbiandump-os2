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
#include <xml/cxml/nw_tinytree_treevector.h>
#include <xml/cxml/nw_tinytree.h>
/* ------------------------------------------------------------------------- */

NW_TinyTree_TreeVector_t*
NW_TinyTree_TreeVector_Construct (CXML_Vector_Metric_t elementSize,
																	CXML_Vector_Metric_t segmentSize,
																	NW_TinyTree_t *tree)
{
  NW_TinyTree_TreeVector_t* thisObj;
  

  /* for convenience */
  thisObj = (NW_TinyTree_TreeVector_t*) NW_Mem_Malloc (sizeof(NW_TinyTree_TreeVector_t));
  if(thisObj == NULL)
	{
		return NULL;
	}

	thisObj->vector = CXML_Vector_Construct(elementSize, segmentSize);
  if (thisObj->vector == NULL)
  {
    NW_Mem_Free (thisObj);
    return NULL;
  }

  /* initialize the object */
  thisObj->tree = tree;
  thisObj->lastValid = -1;
  /* successful completion */
  return thisObj;
}

/* Override the base class to account for the sentinel at the beginning of each
 * segment
 */

void
NW_TinyTree_TreeVector_Destruct (NW_TinyTree_TreeVector_t* thisObj)
{
	if(thisObj->vector)
	{
		/* Adjest the sentinel in the vector */
		CXML_Vector_AdjustSegment(thisObj->vector);

		/* Destroy the vector */
		CXML_Vector_Destruct(thisObj->vector);
	}
  NW_Mem_Free (thisObj);
}


