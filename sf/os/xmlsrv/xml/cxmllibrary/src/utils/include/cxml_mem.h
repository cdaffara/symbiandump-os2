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


/*
** Includes
*/
#include <xml/cxml/cxml_type_def.h>

/***************************************************************************
**   File: cxml_mem.h
**   Purpose:  Provides the interface to memory management
**************************************************************************/

#ifndef CXML_MEM_H
#define CXML_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

  /*
#ifdef _DEBUG
#define FEATURE_MEMORY_PROFILE
#endif
*/

#define NW_Mem_memset(x, y, z)  CXML_Mem_memset(x, y, z) 
#define NW_Mem_memcpy(x, y, z)  CXML_Mem_memcpy(x, y, z) 
#define NW_Mem_memmove(x, y, z) CXML_Mem_memmove(x, y, z)
#define NW_Mem_memcmp(x, y, z)  CXML_Mem_memcmp(x, y, z) 

#ifdef FEATURE_MEMORY_PROFILE
#define NW_Mem_Malloc(x)        CXML_Mem_Malloc_Pro(x,__FILE__,__LINE__) 
#define NW_Mem_Free(x)          CXML_Mem_Free_Pro(x,__FILE__,__LINE__) 

void *CXML_Mem_Malloc_Pro(CXML_Uint32 nbytes, char *file, CXML_Uint32 line);
void CXML_Mem_Free_Pro(void *buffer, char *file, CXML_Uint32 line);

#define NW_CTX_MEM_PRO_LAST_PTR  26
#define NW_CTX_MEM_PRO_SEQ_NO 27

#else
#define NW_Mem_Malloc(x)        CXML_Mem_Malloc(x) 
#define NW_Mem_Free(x)          CXML_Mem_Free(x) 

void *CXML_Mem_Malloc(CXML_Uint32 nbytes);
void CXML_Mem_Free(void *buffer);
#endif

/*
**-------------------------------------------------------------------------
**  External Public Functions
**-------------------------------------------------------------------------
*/

void *CXML_Mem_memset(void *s, CXML_Uint32 c, CXML_Uint32 n);

void *CXML_Mem_memcpy(void *s1, const void *s2, CXML_Uint32 n);

void *CXML_Mem_memmove(void *s1, const void *s2, CXML_Uint32 n);

CXML_Int32 CXML_Mem_memcmp(const void *s1, const void *s2, CXML_Uint32 n);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* CXML_MEM_H */


