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
**   File: cxml_mem.c
**   Purpose:  Provides the implementation to memory management
**************************************************************************/

#include "cxml_internal.h"

#ifdef FEATURE_MEMORY_PROFILE
typedef struct _MemProfile
{
    struct _MemProfile  *next;  // ptr to next mem block
    struct _MemProfile  *prev;  // ptr to previous mem block
    char                *file;  // filename of memory allocator
    CXML_Int32               line;   // line # in filename of memory allocator
    CXML_Int32               size;   // size of requested memory
    CXML_Int32               seq;    // sequence number for allocation 
} MemProfile;
#endif

/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

void *CXML_Mem_memset(void *s, CXML_Uint32 c, CXML_Uint32 n)
{
  unsigned char *myPtr  = (unsigned char *)s;
  unsigned char fill    = (unsigned char)c;
  while (n > 0) {
    *myPtr = fill;
    myPtr++;
    n--;
  }
  return s;
}


void *CXML_Mem_memcpy(void *s1, const void *s2, CXML_Uint32 n)
{
  unsigned char *myDest   = (unsigned char *)s1;
  unsigned char *mySource = (unsigned char *)s2;

  if (n==0) return NULL;

  while (n > 0) {
    *myDest = *mySource;
    n--;
    myDest++;
    mySource++;
  }
  return s1;
}


void *CXML_Mem_memmove(void *s1, const void *s2, CXML_Uint32 n)
{
  unsigned char *myDest;
  unsigned char *mySource;

  if (n == 0) return NULL;

  if (s1 <= s2) {
    /* We can always copy from higher address to */
    /* lower address, even if buffer overlaps */
    myDest    = (unsigned char *)s1;
    mySource  = (unsigned char *)s2;
    while (n > 0) {
      *myDest = *mySource;
      myDest++;
      mySource++;
      n--;
    }

  } else {
    /* We can always copy in reverse order from */
    /* lower address to higher address, even if */
    /* buffer overlaps */
    myDest    = (unsigned char *)s1 + n;
    mySource  = (unsigned char *)s2 + n;
    while (n > 0) {
      n--;
      myDest--;
      mySource--;
      *myDest = *mySource;
    }

  }

  return s1;
}


CXML_Int32 CXML_Mem_memcmp(const void *s1, const void *s2, CXML_Uint32 n)
{
  unsigned char *myDest   = (unsigned char *)s1;
  unsigned char *mySource = (unsigned char *)s2;

  /* If s1 == s2, then immediately return 0. */
  if ( s1 == s2 )
    return 0 ;

  while (n > 0) {
    if (*myDest != *mySource) {
      return (*myDest - *mySource);
    }
    myDest++;
    mySource++;
    n--;
  }
  
  return 0;
}

/*********************************************************
**  Name:   CXML_Mem_Malloc
**  Description:  Allocates memory.
**  Parameters:   nbytes - number of bytes to allocate
**  Return Value: pointer to the allocated memory or NULL 
**********************************************************/
void *CXML_Mem_Malloc(CXML_Uint32 nbytes)
{
	return malloc(nbytes);
}

/************************************************************************
**  Name:   CXML_Mem_Free
**  Description:  Frees allocated memory.
**  Parameters:   mem - pointer to the memory to free
**  Return Value: void
**************************************************************************/
void CXML_Mem_Free(void *buffer)
{
  if (buffer != NULL) {
    /* Note: os_block_dealloc() will free both heap and block-table memory */
		free(buffer);
  }
  return;
}


#ifdef FEATURE_MEMORY_PROFILE
CXML_Uint32 Asc_strlen(const char *s)
{
  CXML_Uint32 i = 0;
  while (*s != '\0')
  {
    s++;
    i++;
  }
  return i;
}

/*****************************************************************
**  Name:  Asc_strcpy
**  Description:  Copies ascii string
**  Parameters:   s1 destination string
**                s2 source string
**  Return Value: pointer to s1
******************************************************************/
char *Asc_strcpy(char *s1, const char *s2)
{
  while (*s2 != '\0') {
    *s1 = *s2;
    s1++;
    s2++;
  }
  *s1 = *s2;
  return s1;
}

/*********************************************************
**  Name:   CXML_Mem_Malloc
**  Description:  Allocates memory.
**  Parameters:   nbytes - number of bytes to allocate
**  Return Value: pointer to the allocated memory or NULL 
**********************************************************/
void *CXML_Mem_Malloc_Pro(CXML_Uint32 nbytes, char *file, CXML_Uint32 line)
{
	void *buf;
    MemProfile  *pblock, *last;  
    CXML_Int32   len = nbytes + sizeof(MemProfile);
    CXML_Int32   seqno;

    pblock = malloc(len);
    
    // Store profile block data 

    if ((CXML_Uint32) file > 0xf000)
    {
        pblock->file = (char*)malloc(Asc_strlen(file) + 1);
        Asc_strcpy(pblock->file, file);
    }
    else
    {
        // no filename since this may be called via a function ptr
        pblock->file = NULL;
    }
    pblock->line = line;
    pblock->size = nbytes;

    // Set List Pointers
    // Get last ptr
    last = (MemProfile *) NW_Ctx_Get(NW_CTX_MEM_PRO_LAST_PTR, 0); 

    if (last == NULL)
    {
        pblock->prev = NULL;
        pblock->seq = 0;
    }
    else
    {
        pblock->prev = last;
        last->next = pblock;
        // can't use last-> because free my remove block (along with
        // seq numbers, need a context
        seqno = (CXML_Uint32) NW_Ctx_Get(NW_CTX_MEM_PRO_SEQ_NO, 0); 
        seqno += 1;
        pblock->seq = seqno;
        NW_Ctx_Set(NW_CTX_MEM_PRO_SEQ_NO, 0, seqno); 
    }
    pblock->next = NULL;

    // last = pblock
    last = pblock;
    NW_Ctx_Set(NW_CTX_MEM_PRO_LAST_PTR, 0, last);

    
    // return mem block to caller              
    buf = (void *) ((CXML_Byte *) pblock + sizeof(MemProfile));
    return (buf);
}

/************************************************************************
**  Name:   CXML_Mem_Free
**  Description:  Frees allocated memory.
**  Parameters:   mem - pointer to the memory to free
**  Return Value: void
**************************************************************************/
void CXML_Mem_Free_Pro(void *buffer, char *file, CXML_Uint32 line)
{
 MemProfile  *pblock, *last, *prevp, *nextp;  
    
    if (buffer == NULL)
    {
        return;
    }
                                                         
    pblock = (MemProfile *) ((CXML_Byte *) buffer - sizeof(MemProfile));

    // Get last ptr
    last = (MemProfile *) NW_Ctx_Get(NW_CTX_MEM_PRO_LAST_PTR, 0); 

    if ( pblock == last )
    {
        last = last->prev;
        NW_Ctx_Set(NW_CTX_MEM_PRO_LAST_PTR, 0, last);
    }
    else
    {
        prevp = pblock->prev; 
        nextp = pblock->next;
        if (prevp)
        {
            prevp->next = nextp;
        }
        if (nextp)
        {
            nextp->prev = prevp;
        }
    }

    // Free Filename pointer and memory block
    free(pblock->file);
    free(pblock);
    return;
}
#endif
