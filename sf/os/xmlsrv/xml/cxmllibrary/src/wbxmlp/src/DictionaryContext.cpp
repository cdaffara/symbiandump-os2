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
#include <e32std.h>
#include "DictionaryContext.h"
#include "cxml_internal.h"
#include "featmgr.h"
#include <xml/cxml/nw_wbxml_dictionary.h>

/* There is possibility that dictionary initialize is called by the embedded
 * application also. To make allocation/deallocation of dictionary possible.
 * The NW_CONTEXT_REF_DICT_CNT variable is added. This variable of TLS context 
 * will keep the reference count of the number of times dictionary 
 * inialization/destroy is called. 
 * 
 * The dictionary is destroyed when NW_CONTEXT_REF_DICT_CNT is zero.
 *
 */

typedef enum {
  NW_CONTEXT_DICTIONARY             = 0,
  NW_CONTEXT_DICTIONARY_COUNT       = 1,
  NW_CONTEXT_REF_DICT_CNT           = 2, /*Number of times dictionary rerferenced*/

  NW_CONTEXT_NUM_ENTRIES            = 3
  
} NW_DictionaryContext_Id_t;


typedef struct {
	NW_Uint16	numContexts;
	void *contexts[NW_CONTEXT_NUM_ENTRIES];
} NW_DictionaryContext_Array_t;


/*****************************************************************

  Name: NW_Ctx_Init()

  Description:  Initialize the context manager

  Parameters:   

  Return Value: NW_STAT_SUCCESS or NW_STAT_OUT_OF_MEMORY

******************************************************************/
static NW_Status_t DictContext_Init()
{
  NW_DictionaryContext_Array_t* contextArray;
  NW_Uint32 i;

  /*lint --e{429} Custodial pointer has not been freed or returned */

  /* Initialize the context manager once, and only once! */
  if (Dll::Tls() == NULL) {
    /* Allocate and init array to hold context pointers */  
    contextArray = new NW_DictionaryContext_Array_t;

    /*lint -e{774} Boolean within 'if' always evaluates to False */
    if (contextArray == NULL) {
      DestroyDictionaries();
      return NW_STAT_OUT_OF_MEMORY;
    }

	contextArray->numContexts = NW_CONTEXT_NUM_ENTRIES;
	
    for (i = 0; i < NW_CONTEXT_NUM_ENTRIES; i++) {
      contextArray->contexts[i] = NULL;
    }

    /* Store the pointer to the context array */
    if( Dll::SetTls( contextArray ) != KErrNone)
	{
	  DestroyDictionaries();
	}
  }  

  return NW_STAT_SUCCESS;
}


/*****************************************************************

  Name: NW_Ctx_Set()

  Description:  Set the context for the specified component

  Parameters:   component - which context
		        *ctx - pointer to the context to store

  Return Value: NW_STAT_SUCCESS or NW_STAT_FAILURE

******************************************************************/
NW_Status_t DictContext_Set(const NW_DictionaryContext_Id_t aContextId,
									 void *ctx)
{
  NW_Status_t status = NW_STAT_SUCCESS;
  NW_DictionaryContext_Array_t* contextArray;

  NW_ASSERT(aContextId < NW_CONTEXT_NUM_ENTRIES);

  /* Get the pointer to the context array */  
  contextArray = (NW_DictionaryContext_Array_t*)Dll::Tls();

  if (contextArray == NULL) {
    status = DictContext_Init();
    if (status != NW_STAT_SUCCESS) {
      return status;
    }
    contextArray = (NW_DictionaryContext_Array_t*)Dll::Tls();
  }

  /* Save the aContextId's context */
  if (contextArray != NULL) {
    /*lint -e{661} Possible access of out-of-bounds pointer */
    contextArray->contexts[aContextId] = ctx;
	return NW_STAT_SUCCESS;
  }

  return status;
}


/*****************************************************************

  Name: DictContext_Get()

  Description:  Get the context for the specified component

  Parameters:   aContextId - which context

  Return Value: pointer to the component's context or NULL

******************************************************************/
void *DictContext_Get(const NW_DictionaryContext_Id_t aContextId)
{
  NW_Status_t status;
  NW_DictionaryContext_Array_t* contextArray;

  /* Use "<=" rather than "<", as PushMtm tests last item for NULL */
  NW_ASSERT(aContextId <= NW_CONTEXT_NUM_ENTRIES);

  /* Get the pointer to the context array */
  contextArray = (NW_DictionaryContext_Array_t*)Dll::Tls();

  if (contextArray == NULL) {
    status = DictContext_Init();
    if (status != NW_STAT_SUCCESS) {
      return NULL;
    }
    contextArray = (NW_DictionaryContext_Array_t*)Dll::Tls();
  }

  /* Return the component's context */
  if (contextArray != NULL) {
    /*lint --e{661} Possible access of out-of-bounds pointer */
    /*lint --e{662} Possible creation of out-of-bounds pointer */
    return contextArray->contexts[aContextId];
  }

  return NULL;
}

void DestroyDictionaries()
{
  NW_DictionaryContext_Array_t* contextArray = (NW_DictionaryContext_Array_t*)Dll::Tls();

  if(contextArray)
  {

   /*Check the dictionary reference count. If it is zero 
    * then free the dictionary.
    */
   if(contextArray->contexts[NW_CONTEXT_REF_DICT_CNT] == 0)
   {
    NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();

    NW_Mem_Free(dictionaries);
    StoreDictionaries(NULL);
    StoreDictionaryCount(0);

   // Get the TLS pointer
  
    delete contextArray;
    contextArray = NULL;
    Dll::SetTls(contextArray);
   }
  }/*end if(contextArray)*/
} 

/* Temporary methods for storing dictionary & dictionary size inside the context.
 */
void StoreDictionaries(NW_WBXML_Dictionary_t** dictionaries)
{
    DictContext_Set(NW_CONTEXT_DICTIONARY, (void*)dictionaries);

}

NW_WBXML_Dictionary_t** GetDictionaries()
{
  return (NW_WBXML_Dictionary_t**)DictContext_Get(NW_CONTEXT_DICTIONARY);
}

void StoreDictionaryCount(NW_Uint32 dictionary_count)
{
    DictContext_Set(NW_CONTEXT_DICTIONARY_COUNT, (void*)dictionary_count);

}

NW_Uint32 GetDictionaryCount()
{
  NW_Uint32 count = (NW_Uint32)DictContext_Get(NW_CONTEXT_DICTIONARY_COUNT);

  return count;

}


void UpdateDictRefCnt(CXML_DICT_REF_CNT updateVal)
{
 NW_DictionaryContext_Array_t* contextArray;
 NW_Uint32 refCount = 0;

   /* Get the pointer to the context array */  

  contextArray = (NW_DictionaryContext_Array_t*)Dll::Tls();

  if(contextArray != NULL)
  {
   refCount = (NW_Uint32) contextArray->contexts[NW_CONTEXT_REF_DICT_CNT];
   if(updateVal == NW_CONTEXT_REF_DICT_CNT_INR)
   {
    contextArray->contexts[NW_CONTEXT_REF_DICT_CNT] =
         (void*)++refCount;
   }
   else if(updateVal == NW_CONTEXT_REF_DICT_CNT_DCR)
   {
    contextArray->contexts[NW_CONTEXT_REF_DICT_CNT] =
         (void*)--refCount;
   }

  }/*end if(contextArray != NULL)*/
  return; 
}/*end UpdateDictRefCnt(CXML_DICT_REF_CNT updateVal)*/


//
//For Non DTD element support and checking for other features in the release
//

EXPORT_C
CXML_Int32 CXML_Additional_Feature_Supprted()
{
 
CXML_Int32 featureFlag = 0;

featureFlag |= CXML_DTD_SUPPORT_ON;

 /* The feature manager not supported. May be hook up for future use. 
 
 CXML_Bool retVal = CXML_TRUE;

 FeatureManager::InitializeLibL();

 if(FeatureManager::FeatureSupported(KFeatureIdEcmaScript) )
 {
   retVal = CXML_TRUE;
 }
 else
 {
  retVal = CXML_FALSE;
 }


 retVal = CXML_TRUE;

 return retVal; 
 */

 return featureFlag;

}//end CXML_Non_DTD_supprted()

