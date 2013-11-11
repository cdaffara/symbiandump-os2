/*
* Copyright (c) 1999 - 2000 Nokia Corporation and/or its subsidiary(-ies).
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
**   File: nwx_mem.c
**   Purpose:  Provides the implementation to memory management
**************************************************************************/

/*
**-------------------------------------------------------------------------
**  Include Files
**-------------------------------------------------------------------------
*/
#include "nwx_mem.h"
#ifdef FEATURE_MEMORY_PROFILE
#include "nwx_ctx.h"
#include "nwx_logger.h"
#endif // FEATURE_MEMORY_PROFILE

/*
**-------------------------------------------------------------------------
**  Constants
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Types
**-------------------------------------------------------------------------
*/
#ifdef FEATURE_MEMORY_PROFILE
typedef struct _MemProfile
{
    struct _MemProfile  *next;  // ptr to next mem block
    struct _MemProfile  *prev;  // ptr to previous mem block
    char                *file;  // filename of memory allocator
    NW_Int32               line;   // line # in filename of memory allocator
    NW_Int32               size;   // size of requested memory
    NW_Int32               seq;    // sequence number for allocation 
} MemProfile;
#endif // FEATURE_MEMORY_PROFILE

/*
**-------------------------------------------------------------------------
**  Macros
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Internal Prototypes
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  File Scoped Static Variables
**-------------------------------------------------------------------------
*/

/*
**-------------------------------------------------------------------------
**  Global Variable Definitions
**-------------------------------------------------------------------------
*/


/*
**-------------------------------------------------------------------------
**  Internal Functions
**-------------------------------------------------------------------------
*/


/*
**-------------------------------------------------------------------------
**  External Public (Exported) Functions
**-------------------------------------------------------------------------
*/

void *NW_Mem_memset(void *s, NW_Uint32 c, NW_Uint32 n)
{
  unsigned char *myPtr = (unsigned char *)s;
  unsigned char fill = (unsigned char)c;
  while (n > 0) {
    *myPtr = fill;
    myPtr++;
    n--;
  }
  return s;
}

EXPORT_C void *NW_Mem_memcpy(void *s1, const void *s2, NW_Uint32 n)
{
  unsigned char *myDest = (unsigned char *)s1;
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

void *NW_Mem_memmove(void *s1, const void *s2, NW_Uint32 n)
{
  unsigned char *tempBuff;
  if (n == 0) return NULL;
  tempBuff = NW_Mem_Malloc(n);
  if (tempBuff == NULL) {
    return NULL;
  }
  NW_Mem_memcpy(tempBuff, s2, n);
  NW_Mem_memcpy(s1, tempBuff, n);
  NW_Mem_Free(tempBuff);
  return s1;
}
  
NW_Int32 NW_Mem_memcmp(const void *s1, const void *s2, NW_Uint32 n)
{
  unsigned char *myDest = (unsigned char *)s1;
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
**  Name:   NW_Mem_Malloc
**  Description:  Allocates memory.
**  Parameters:   nbytes - number of bytes to allocate
**  Return Value: pointer to the allocated memory or NULL 
**********************************************************/

EXPORT_C void *NW_Mem_Malloc_No_Pro(NW_Uint32  nbytes)
{
  void *buf;
  buf = malloc(nbytes);
  return buf;
}

/************************************************************************
**  Name:   NW_Mem_Free
**  Description:  Frees allocated memory.
**  Parameters:   mem - pointer to the memory to free
**  Return Value: void
**************************************************************************/
EXPORT_C void NW_Mem_Free_No_Pro(void *mem)
{
  /* Note: free() handles NULL pointers corretly */
  free(mem);
  return;
}


// --------------------------------------------------------
//  PROFILED MEMORY MANAGEMENT ROUTINES
// --------------------------------------------------------

#ifdef FEATURE_MEMORY_PROFILE

/*****************************************************************
**  Name:  Asc_strlen
**  Description:  Returns string length of ascii string
**  Parameters:   *string - Null-terminated ascii string
**  Return Value: string length
******************************************************************/
NW_Uint32 Asc_strlen(const char *s)
{
  NW_Uint32 i = 0;
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

/*****************************************************************
**  Name:  Str_CvtFromAscii
**  Description:  Convert null-terminated ascii string to new ucs2 string
**  Parameters:   *string - Null-terminated ascii string
**  Return Value: pointer to new ucs2 string or NULL
******************************************************************/
NW_Ucs2 *Str_CvtFromAscii(const char *string)
{
  NW_Uint32 i, size;
  NW_Ucs2* res = NULL;

  NW_ASSERT(string);

  size = Asc_strlen(string) + 1;
  res = malloc(size * sizeof(NW_Ucs2));
  /* this is a really naive conversion from ascii */
  if (res != NULL) {
    for (i=0; i < size; i++) {
      res[i] = string[i];
    }
  }
  return res;
}


/*********************************************************
**  Name:   NW_Mem_Malloc_Pro
**  Description:  Allocates "profiled" memory.
**  Parameters:   nbytes - number of bytes to allocate
**                file - string of filename where this was called
**                line - line # of file where this was called
**  Return Value: pointer to the allocated memory or NULL 
**********************************************************/
void *NW_Mem_Malloc_Pro(NW_Uint32 nbytes, char *file, NW_Uint32 line)
{
    void *buf;
    MemProfile  *pblock, *last;  
    NW_Int32   len = nbytes + sizeof(MemProfile);
    NW_Int32   seqno;
    NW_Status_t status;

    pblock = malloc(len);
    
    // Store profile block data 

    if ((NW_Uint32) file > 0xf000)
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
        seqno = (NW_Uint32) NW_Ctx_Get(NW_CTX_MEM_PRO_SEQ_NO, 0); 
        seqno += 1;
        pblock->seq = seqno;
        status = NW_Ctx_Set(NW_CTX_MEM_PRO_SEQ_NO, 0, seqno); 
        NW_ASSERT(status == NW_STAT_SUCCESS);
    }
    pblock->next = NULL;

    // last = pblock
    last = pblock;
    status = NW_Ctx_Set(NW_CTX_MEM_PRO_LAST_PTR, 0, last);
    NW_ASSERT(status == NW_STAT_SUCCESS);

    
    // return mem block to caller              
    buf = (void *) ((NW_Byte *) pblock + sizeof(MemProfile));
    return (buf);
}

/************************************************************************
**  Name:   NW_Mem_Free_Pro
**  Description:  Frees allocated "profiled" memory.
**  Parameters:   mem - pointer to the memory to free
**                file - string of filename where this was called
**                line - line # of file where this was called
**  Return Value: void
**************************************************************************/
void NW_Mem_Free_Pro(void *buffer, char *file, NW_Uint32 line)
{
    MemProfile  *pblock, *last, *prevp, *nextp;  
    NW_Status_t status;
    
    if (buffer == NULL)
    {
        return;
    }
                                                         
    pblock = (MemProfile *) ((NW_Byte *) buffer - sizeof(MemProfile));

    // Get last ptr
    last = (MemProfile *) NW_Ctx_Get(NW_CTX_MEM_PRO_LAST_PTR, 0); 

    if ( pblock == last )
    {
        last = last->prev;
        status = NW_Ctx_Set(NW_CTX_MEM_PRO_LAST_PTR, 0, last);
        NW_ASSERT(status == NW_STAT_SUCCESS);
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


/************************************************************************
**  Name:  SendStringToLogger1 
**  Description:  Output Memory Profile Data to Debug Log 
**  Parameters:   none   
**  Return Value: void
**************************************************************************/
void SendStringToLogger1(char *strMsg)
{
    MemProfile  *pblock, *last;  
    NW_Ucs2 *uStrFileName = NULL;
        
    if (strMsg != NULL)
    {
        NW_Ucs2 *uStrMsg = Str_CvtFromAscii(strMsg);
        NW_LOG1(NW_LOG_LEVEL1, "NW_Mem_Pro_Log: %s", uStrMsg);
        free(uStrMsg);
        return;
    }

    // Get last ptr
    last = (MemProfile *) NW_Ctx_Get(NW_CTX_MEM_PRO_LAST_PTR, 0); 

    pblock = last;
    while ( pblock )
    {
        if (pblock->file == NULL)
        {
            uStrFileName = Str_CvtFromAscii("??");
        }
        else
        {
            uStrFileName = Str_CvtFromAscii(pblock->file);
        }
        NW_LOG6(NW_LOG_LEVEL1, "NW_Mem_Pro_Log: memBlock:%X, data:%X, File: %s ; Line: %d ; Size: %d ; SEQ: %d", 
                                pblock,
                                pblock + sizeof(MemProfile),
                                uStrFileName,
                                pblock->line,
                                pblock->size,
                                pblock->seq);
        NW_LOG0(NW_LOG_LEVEL1, "NW_Mem_Pro_Log: ------------------------------");
        pblock = pblock->prev;
        free(uStrFileName);
    }
}
#endif // FEATURE_MEMORY_PROFILE
