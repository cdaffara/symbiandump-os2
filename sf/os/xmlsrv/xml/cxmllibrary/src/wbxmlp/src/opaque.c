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


/*****************************************************************
**  File: opaque.c
**  Description: Methods on WBXML opaque data objects
*****************************************************************/

#include "cxml_internal.h"
#include <xml/cxml/nw_wbxml_opaque.h>

/*
 * Initialize an opaque object
 * 
 */

NW_Status_t
NW_WBXML_Opaque_construct(NW_WBXML_Opaque_t * opaque, 
                          NW_Byte * data, 
                          NW_Uint32 length)
{

  NW_ASSERT(opaque != NULL);

  opaque->length = length;
  opaque->data = (length ? data : NULL);

  return NW_STAT_SUCCESS;
}

/*
 * Create a new opaque structure
 */

NW_WBXML_Opaque_t *
NW_WBXML_Opaque_new (void)
{
  NW_WBXML_Opaque_t *opaque =
    (NW_WBXML_Opaque_t *) NW_Mem_Malloc (sizeof (NW_WBXML_Opaque_t));

  if(opaque != NULL){
    NW_WBXML_Opaque_construct(opaque,0,0);
   }

  return opaque;
}



/*
 * Delete an opaque structure
 */

NW_Status_t
NW_WBXML_Opaque_delete (NW_WBXML_Opaque_t * opaque)
{

  NW_ASSERT(opaque != NULL);

  /* The data member is a pointer to memory we
   * don't own, so it is not freed here
   */

  NW_Mem_Free(opaque);

  return NW_STAT_SUCCESS;
}
