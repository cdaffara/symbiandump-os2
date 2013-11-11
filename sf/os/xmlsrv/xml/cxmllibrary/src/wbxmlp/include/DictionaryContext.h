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

#ifndef NW_PARSER_WBXML_DICTIONARYCONTEXT_H
#define NW_PARSER_WBXML_DICTIONARYCONTEXT_H

/* TODO: Change dictionaries to be all static const. Currently
 * they are stored inside the context.
 */
 
#define MAX_DICTIONARIES 9

  
/* Temporary methods for storing dictionary & dictionary size inside the context.
 */

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_wbxml_dictionary.h>

 
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum  {
  NW_CONTEXT_REF_DICT_CNT_INR,
  NW_CONTEXT_REF_DICT_CNT_DCR  
} CXML_DICT_REF_CNT;

void StoreDictionaries(NW_WBXML_Dictionary_t** dictionaries);
NW_WBXML_Dictionary_t** GetDictionaries();
void StoreDictionaryCount(NW_Uint32 dictionary_count);
NW_Uint32 GetDictionaryCount();
void DestroyDictionaries();
/*The following function takes parameter of type CXML_DICT_REF_CNT only */
void UpdateDictRefCnt(CXML_DICT_REF_CNT updateVal);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */


#endif // NW_PARSER_WBXML_DICTIONARYCONTEXT_H
