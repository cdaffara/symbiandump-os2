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
**  File: dictionary.c
**  Description:
*****************************************************************/
#include "cxml_internal.h"
#include "nw_wbxml_parsei.h"
#include <xml/cxml/nw_string_char.h>
#include <xml/cxml/nw_wbxml_dictionary.h>
#include "DictionaryContext.h"

/* HTTP_iso_8859_1 IANA MIBenum 4 */
#define NW_WBXML_DICTIONARY_CHARSET 4

/*
 * The dictionaries ...
 */


static
NW_Bool
NW_WBXML_Dictionary_CmpDictDocType(NW_WBXML_Dictionary_t* dictionary,
                                   NW_String_t* string,
                                   NW_Uint32 encoding)
{
  NW_Ucs2* docType;

  docType = dictionary->doc_type;
  while (*docType != 0)
  {
    NW_Int32 bytesRead;
    NW_Ucs2 c;

    bytesRead = NW_String_readChar(string->storage, &c, encoding);
    if (c != *docType){
      return NW_FALSE;
    }
    NW_ASSERT(bytesRead > 0);
    string->storage = string->storage + bytesRead;
    string->length = string->length - (NW_Uint32)bytesRead;
    docType++;
  }
  return NW_TRUE;
}

static 
NW_String_UCS2Buff_t *
getTagByToken (NW_WBXML_Codepage_t * page, 
               NW_Byte token)
{
  NW_WBXML_DictEntry_t *e;
  NW_Int32 r;
  NW_Int32 l = 0;
  
  if (page == NULL)
    return NULL;
  
  r = page->count - 1;
  e = page->tokens;
  
  /* Binary search (tokens must be stored in order) */
  
  while (r >= l)
  {
    NW_Int32 m = (l + r) / 2;
    if (token == e[m].token)
      return e[m].name;
    if (token < e[m].token)
      r = m - 1;
    else
      l = m + 1;
  }
  
  return NULL;
}

/*
 * Return -1 if tag is not found, page is 0 or tag is 0
 */

static 
NW_Int16
getTokenByTag (NW_WBXML_Codepage_t * page, 
               NW_String_UCS2Buff_t * tag,
               NW_Bool matchCase)
{
  NW_Byte *names;
  NW_Int32 r;
  NW_Int32 l = 0;
  
  if (page == NULL || tag == NULL)
    return -1;
  
  names = page->names;
  r = page->count - 1;
  
  while (r >= l)
  {
    NW_Int32 m = (l + r) / 2;
    NW_Int32 c = NW_String_UCS2BuffCmp (tag, (page->tokens)[names[m]].name, matchCase);
    if (c == 0)
      return (page->tokens)[names[m]].token;
    if (c < 0)
      r = m - 1;
    else
      l = m + 1;
  }
  
  return -1;
}

static 
NW_Status_t
getTokenByTag2 (NW_WBXML_Codepage_t * page,
                NW_Uint32 encoding,
                NW_Uint32 charCount,
                NW_Uint8 * name,
                NW_Uint16 * pageToken)
{
    NW_Byte *names;
    NW_Int32 r;
    NW_Int32 l = 0;

    if (page == NULL || name == NULL || pageToken == NULL) {
        return NW_STAT_FAILURE;
    }

    names = page->names;
    r = page->count - 1;

    while (r >= l)
    {
        NW_Int32 m = (l + r) / 2;
        NW_Int32 c;
        NW_Status_t s;
        s = NW_String_CmpToNativeAlignedUCS2 (encoding, charCount, name,
                                              (NW_Uint16*)((page->tokens)[names[m]].name),
                                              &c);
        if (NW_STAT_IS_FAILURE(s)) {
            return s;
        }
        if (c == 0) {
            *pageToken = (page->tokens)[names[m]].token;
            return NW_STAT_SUCCESS;
        }
        if (c < 0)
            r = m - 1;
        else
            l = m + 1;
    }
    return NW_STAT_FAILURE;
}

static 
NW_Status_t
getTokenByTag_HandleDuplicates (NW_WBXML_Codepage_t * page,
																NW_Uint32 encoding,
																NW_Uint32 charCount,
																NW_Uint8 * name,
																NW_Uint16 * pageToken,
																NW_Bool isName)
{
    NW_Byte *names;
		NW_Int32 i = 0;
		NW_Uint16 tmpToken;

    if (page == NULL || name == NULL || pageToken == NULL) {
        return NW_STAT_FAILURE;
    }

    names = page->names;

    while (i < page->count)
    {
        NW_Int32 c;
        NW_Status_t s;
        tmpToken = (page->tokens)[names[i]].token;
				/* If the tag is AttributeTag, then MSB of lower order byte (token) 
				 * should be unset. If the tag is AttributeValueTag, then MSB of 
				 * lower order byte (token) should be set.
				 */
				if((isName && !(tmpToken & 0x80)) || (!isName && (tmpToken & 0x80)))
				{
				    s = NW_String_CmpToNativeAlignedUCS2 (encoding, charCount, name,
							                                    (NW_Uint16*)((page->tokens)[names[i]].name),
																									&c);
						if (NW_STAT_IS_FAILURE(s)) {
						    return s;
						}
						if (c == 0) {
						    *pageToken = tmpToken;
								return NW_STAT_SUCCESS;
						}
				}
				i++;
    }
    return NW_STAT_FAILURE;
}

/* 
 * Dictionary ids are a 1-based index into the dictionary table 
 *
 * Return NULL if id is < 1 or id is greater than dictionary_count
 */

static 
NW_WBXML_Dictionary_t *
getDictionaryById (NW_Uint32 id)
{
  NW_WBXML_Dictionary_t *d;
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();
  
  if (id < 1 || id > GetDictionaryCount())
    return NULL;
  
//#ifdef __WINS__  
  d = dictionaries[id - 1];
//#endif
  
  return d;
}

/* 
 * This function checks if the dictionary has already been added
 * to the 'dictionaries' table. The comparison is based on 
 * puclic ids, and therefore it is important that each dictionary
 * has unique public id.
 *
 */
static NW_Bool ExistingDictionary(NW_WBXML_Dictionary_t *dictionary)
{
//#ifdef __WINS__
  NW_Uint32 i;
  NW_Uint32 dictionary_count = GetDictionaryCount();
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();

  for (i=0; i<dictionary_count; i++) {
      if (dictionaries[i]->public_id == dictionary->public_id) {
          return NW_TRUE;
      }
  }
//#endif
  return NW_FALSE;
}

EXPORT_C NW_Status_t
NW_WBXML_Dictionary_initialize (NW_Int32 n, 
                                NW_WBXML_Dictionary_t * d[])
{
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();

  /* Make sure that adding dictionaries is done by calling NW_WBXML_Dictionary_add(). */
  if (dictionaries == NULL)
  {
    dictionaries = (NW_WBXML_Dictionary_t **)NW_Mem_Malloc(sizeof(NW_WBXML_Dictionary_t*) * MAX_DICTIONARIES);
    if (dictionaries == NULL) {
		DestroyDictionaries();
        return NW_STAT_OUT_OF_MEMORY;
    }
    StoreDictionaries(dictionaries);
    StoreDictionaryCount(0);
  }
  UpdateDictRefCnt(NW_CONTEXT_REF_DICT_CNT_INR);
  return NW_WBXML_Dictionary_add(n, d);
}

NW_Status_t 
NW_WBXML_Dictionary_add(NW_Int32 n, NW_WBXML_Dictionary_t* d[])
{
  NW_Int32 i;
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();

  if (dictionaries == NULL)
  {
    return NW_STAT_FAILURE;
  }
  
  NW_ASSERT(n > 0);
  NW_ASSERT(d != NULL);

  /* This loop adds only the new dictionaries received in the
     function call (the *d[] table) into the 'dictionaries' table. */
  for (i=0; i<n; i++) {
      if (!ExistingDictionary(d[i])) {
           NW_Uint32 dictionary_count = GetDictionaryCount();
     
          /* Return error if the table is full */
          if ((NW_Uint32)GetDictionaryCount() >= MAX_DICTIONARIES) {
              return NW_STAT_FAILURE;
          }
          dictionaries[dictionary_count++] = d[i];
          StoreDictionaryCount(dictionary_count);
      }
  }

  return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_WBXML_Dictionary_destroy ()
{
  UpdateDictRefCnt(NW_CONTEXT_REF_DICT_CNT_DCR);
  DestroyDictionaries();
  return NW_STAT_SUCCESS;
}


/*
 * Return NULL if GetDictionaryById returns 0 or if GetTagByToken returns 0
 */

EXPORT_C NW_String_UCS2Buff_t *
NW_WBXML_Dictionary_getTagByFqToken (NW_Uint32 fq_token)
{
  NW_WBXML_Dictionary_t *d;
  NW_Byte cp_index = (NW_Byte) ((fq_token & NW_WBXML_MASK_CODEPAGE) >> 8);
  NW_Byte token = (NW_Byte) (fq_token & NW_WBXML_MASK_TOKEN);
  NW_WBXML_Codepage_t page;
  NW_Uint16 dict_id = (NW_Uint16) ((fq_token & NW_WBXML_MASK_DICTIONARY) >> 16);
  
  if ((d = getDictionaryById (dict_id)) == NULL)
    return NULL;
  
  if ((fq_token & NW_WBXML_MASK_CPSTATE) == NW_WBXML_CP_STATE_TAG)
  {
    if (cp_index > (d->tag_page_count - 1))
      return NULL;
    page = d->tag_pages[cp_index];
    token &= NW_WBXML_MASK_TAG_ID;  /* Only lowest 6 bits for tags */
  }
  else
  {
    if (cp_index > (d->attr_page_count - 1))
      return NULL;
    page = d->attr_pages[cp_index];
  }
  
  
  return getTagByToken (&page, token);
}

NW_String_UCS2Buff_t *
NW_WBXML_Dictionary_getElementNameByToken (NW_WBXML_Dictionary_t *dictionary, 
                                           NW_Uint16 token)
{
  NW_Byte cp_index = (NW_Byte) ((token & NW_WBXML_MASK_CODEPAGE) >> 8);
  NW_Byte tok = (NW_Byte) (token & NW_WBXML_MASK_TOKEN);
  NW_WBXML_Codepage_t page;
  
  if (dictionary == NULL)
    return NULL;
  
  if (cp_index > (dictionary->tag_page_count - 1))
    return NULL;
  page = dictionary->tag_pages[cp_index];
  tok &= NW_WBXML_MASK_TAG_ID;  /* Only lowest 6 bits for tags */
  return getTagByToken (&page, tok);
}

EXPORT_C NW_String_UCS2Buff_t *
NW_WBXML_Dictionary_getAttributeNameByToken (NW_WBXML_Dictionary_t *dictionary, 
                                             NW_Uint16 token)
{
  NW_Byte cp_index = (NW_Byte) ((token & NW_WBXML_MASK_CODEPAGE) >> 8);
  NW_Byte tok = (NW_Byte) (token & NW_WBXML_MASK_TOKEN);
  NW_WBXML_Codepage_t page;
  
  if (dictionary == NULL)
    return NULL;
  
  if (cp_index > (dictionary->attr_page_count - 1))
    return NULL;
  page = dictionary->attr_pages[cp_index];
  return getTagByToken (&page, tok);
}

/*
 * These return the lower 16 bits of the fully qualified token 
 * i.e., the token and code page. The rest of the token can be
 * constructed by the caller if needed. We don't use all 32 bits
 * in order to be able to return a signed quantity to indicate
 * failure.
 */

/*
 * Return -1 if the token is not found for the given attribute name or if
 * dictionary or name is 0
 */

EXPORT_C NW_Int16
NW_WBXML_Dictionary_getAttributeToken (NW_WBXML_Dictionary_t * dictionary,
                                       const NW_String_t* name,
                                       NW_Uint32 encoding,
                                       NW_Bool matchCase)
{
  NW_Int16 token = -1;
  NW_WBXML_Codepage_t *page;
  NW_Byte i;
  NW_Ucs2* ucs2Name;
  NW_Status_t status;
  
  if (dictionary == NULL || name == NULL){
    return -1;
  }
  if (encoding != HTTP_iso_10646_ucs_2){
    status = NW_String_stringToUCS2Char(name, encoding, &ucs2Name);
    NW_ASSERT(status == NW_STAT_SUCCESS);
		/* To fix TI compiler warning */
		(void) status;
  }
  else{
    ucs2Name = (NW_Ucs2*)name->storage;
  }
  
  for (i = 0; i < dictionary->attr_page_count; i++)
  {
    page = dictionary->attr_pages + i;
    /* this is a hack - ucs2buff types should be removed */
    if ((token = getTokenByTag (page, (NW_String_UCS2Buff_t*) ucs2Name, matchCase)) >= 0){
      token = (NW_Int16) (token | (i << 8));
      break;
    }
  }
  if (encoding != HTTP_iso_10646_ucs_2){
    NW_Mem_Free(ucs2Name);  
  }
  return token;
}

NW_Status_t
NW_WBXML_Dictionary_getAttributeToken2 (NW_WBXML_Dictionary_t * dictionary,
                                        NW_Uint32 encoding,
                                        NW_Uint32 charCount,
                                        NW_Uint8 * name,
                                        NW_Uint16 * pageToken,
                                        NW_Bool isName)
{
  NW_WBXML_Codepage_t *page;
  NW_Byte i;
  NW_Status_t s;
  
  if (dictionary == NULL || name == NULL || pageToken == NULL) {
    return NW_STAT_FAILURE;
  }
  
  for (i = 0; i < dictionary->attr_page_count; i++) {
    page = dictionary->attr_pages + i;
    s = getTokenByTag2 (page, encoding, charCount, name, pageToken);
    if (NW_STAT_IS_FAILURE(s)) {
        /* failure may mean "not found" so continue to next page */
        continue;
    }
      if (isName) {
        if (*pageToken >= 128) {
					s = getTokenByTag_HandleDuplicates (page, encoding, charCount, name, pageToken, isName);
					if (NW_STAT_IS_FAILURE(s)) {
							/* failure may mean "not found" so continue to next page */
          continue;
        }
        }
      } else {
        if (*pageToken < 128) {
					s = getTokenByTag_HandleDuplicates (page, encoding, charCount, name, pageToken, isName);
					if (NW_STAT_IS_FAILURE(s)) {
							/* failure may mean "not found" so continue to next page */
          continue;
        }
      }
      }
      *pageToken |= (i << 8);
      return NW_STAT_SUCCESS;
    }
  return NW_STAT_FAILURE;
}

NW_Status_t
NW_WBXML_Dictionary_getAttributeNameToken (NW_WBXML_Dictionary_t * dictionary,
                                           NW_Uint32 encoding,
                                           NW_Uint32 charCount,
                                           NW_Uint8 * name,
                                           NW_Uint16 * pageToken)
{
    return NW_WBXML_Dictionary_getAttributeToken2 (dictionary, encoding,
                                                   charCount, name, pageToken,
                                                   NW_TRUE);
    
}

NW_Status_t
NW_WBXML_Dictionary_getAttributeValueToken (NW_WBXML_Dictionary_t * dictionary,
                                            NW_Uint32 encoding,
                                            NW_Uint32 charCount,
                                            NW_Uint8 * name,
                                            NW_Uint16 * pageToken)
{
    return NW_WBXML_Dictionary_getAttributeToken2 (dictionary, encoding,
                                                   charCount, name, pageToken,
                                                   NW_FALSE);
    
}

/*
 * Return -1 if the token is not found for the given tag name or if
 * dictionary or name is 0
 */

EXPORT_C NW_Int16
NW_WBXML_Dictionary_getTagToken (NW_WBXML_Dictionary_t * dictionary,
                                 NW_String_UCS2Buff_t * name,
                                 NW_Bool matchCase)
{
  NW_Int16 token = 0;
  NW_WBXML_Codepage_t *page;
  NW_Byte i;
  
  if (dictionary == NULL || name == NULL)
    return -1;
  
  for (i = 0; i < dictionary->tag_page_count; i++)
  {
    page = dictionary->tag_pages + i;
    if ((token = getTokenByTag (page, name, matchCase)) >= 0)
      return (NW_Int16)(token | (i << 8));
  }
  
  return -1;
}

NW_Status_t
NW_WBXML_Dictionary_getTagToken2(NW_WBXML_Dictionary_t * dictionary,
                                 NW_Uint32 encoding,
                                 NW_Uint32 charCount,
                                 NW_Uint8* name,
                                 NW_Uint16* pageToken)
{
  NW_WBXML_Codepage_t *page;
  NW_Byte i;
  NW_Status_t s;
  
  if (dictionary == NULL || name == NULL || pageToken == NULL) {
    return NW_STAT_FAILURE;
  }
  
  for (i = 0; i < dictionary->tag_page_count; i++) {
    page = dictionary->tag_pages + i;
    s = getTokenByTag2 (page, encoding, charCount, name, pageToken);
    if (NW_STAT_IS_FAILURE(s)) {
        /* failure may mean "not found" so continue to next page */
        continue;
    }
      *pageToken |= (i << 8);
      return NW_STAT_SUCCESS;
    }
  return NW_STAT_FAILURE;
}

/* Linear searches are ok here since we only do these once */

/*
 * Return NULL if a dictionary is not found for the given public_id
 */

EXPORT_C
NW_WBXML_Dictionary_t *
NW_WBXML_Dictionary_getByPublicId (NW_Uint32 public_id)
{
//#ifdef __WINS__
  NW_Uint32 i;
  NW_Uint32 dictionary_count = GetDictionaryCount();
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();
  
  for (i = 0; i < dictionary_count; i++)
  {
    if (dictionaries[i]->public_id == public_id)
      return dictionaries[i];
  }
//#endif  
  return NULL;
}

/*
 * Return NULL if a dictionary is not found for the given doc_type or if
 *   doc_type is 0
 */

NW_WBXML_Dictionary_t *
NW_WBXML_Dictionary_getByDocType (NW_String_t * doc_type, NW_Uint32 encoding)
{
//#ifdef __WINS__
  NW_Uint32 i;
  NW_Uint32 dictionary_count = GetDictionaryCount();
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();
  
  if (doc_type == NULL)
    return NULL;
  
  for (i = 0; i < dictionary_count; i++)
  {
    if (NW_WBXML_Dictionary_CmpDictDocType(dictionaries[i], doc_type, encoding)){
      return dictionaries[i];
  }
  }
//#endif  
  return NULL;
}

/* Get the 1-based index into the dictionary table */

/*
 * Return 0 if the dictionary is not found
 */

NW_Uint32
NW_WBXML_Dictionary_getIndexByPublicId (NW_Uint32 public_id)
{
//#ifdef __WINS__
  NW_Uint32 i;
  NW_Uint32 dictionary_count = GetDictionaryCount();
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();
  
  for (i = 0; i < dictionary_count; i++)
  {
    if (dictionaries[i]->public_id == public_id)
      return i + 1;
  }
//#endif  
  return 0;
}

/*
 * Return 0 if the dictionary is not found or doc_type is 0
 */

NW_Uint32
NW_WBXML_Dictionary_getIndexByDocType (NW_String_t * doc_type, NW_Uint32 encoding)
{
//#ifdef __WINS__
  NW_Uint32 i;
  NW_Uint32 dictionary_count = GetDictionaryCount();
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();
  
  if(doc_type == NULL){
    return 0;
  }
  
  for (i = 0; i < dictionary_count; i++) {
    if (NW_WBXML_Dictionary_CmpDictDocType(dictionaries[i], doc_type, encoding)){
      return i + 1;
    }
  }
//#endif
  return 0;
}

NW_WBXML_Dictionary_t * 
NW_WBXML_Dictionary_getByIndex(NW_Uint32 dictIndex)
{
//#ifdef __WINS__
  NW_WBXML_Dictionary_t **dictionaries = GetDictionaries();

  if (dictIndex == 0){
    return NULL;
  }
  return dictionaries[dictIndex -1];
//#else
//  return NULL;
//#endif
}

/*
 * Given a tag or attribute token, if the token is a literal,
 * use the given name to lookup the tag/attribute's "real" token
 * and return that token.
 *
 *
 * Returns NW_STAT_SUCCESS
 *         NW_STAT_FAILURE - if the token lookup fails (NOT fatal)
 *         NW_STAT_OUT_OF_MEMORY
 */

EXPORT_C NW_Status_t
NW_WBXML_Dictionary_resolveLiteralToken(NW_Uint32 *token,     /* In/Out */
                                        NW_String_t *name,    /* Ask Deepika why NULL is allowed */
                                        NW_Bool is_tag,       /* NW_TRUE == token is for tag; 
                                                                 NW_FALSE == attribute */
                                        NW_Uint32 encoding,   /* Used in the name conversion */
                                        NW_Bool matchCase)
{
  
  NW_Ucs2 * buff;
  NW_Uint32 dict_id = ((*token) & NW_WBXML_MASK_DICTIONARY) >> 16;
  NW_Int16 tmp_token;  
  
  if (NW_WBXML_Dictionary_getTagByFqToken(*token) != NULL){
    /* The token is NOT a literal, no need for further processing */
    return NW_STAT_SUCCESS;
  }
  
  if(name == NULL){ /* TODO:  Ask Deepika if this can be made an assert?? */
    return NW_STAT_FAILURE;
  }

  /* 
   * Before looking up the name in the dictionary, must convert
   * name to the type used in dictionary names.
   */

  if (NW_String_stringToUCS2Char(name, encoding, &buff) != NW_STAT_SUCCESS) {
    return NW_STAT_OUT_OF_MEMORY;
  }
  
  tmp_token = (NW_Int16) ((is_tag == NW_TRUE) ?
    NW_WBXML_Dictionary_getTagToken(getDictionaryById(dict_id), (NW_String_UCS2Buff_t *) buff, matchCase) : 
    NW_WBXML_Dictionary_getAttributeToken(getDictionaryById(dict_id), name, encoding, matchCase));
  
  NW_Mem_Free (buff);

  if (tmp_token == -1)
  {
    return NW_STAT_FAILURE;
  }
  
  *token = ((NW_Uint16) dict_id << 16)| (NW_Uint16) tmp_token;
  
  return NW_STAT_SUCCESS; 
}


/* 1. returns success, oom, or failure
   2. if oom or failure, no leaks and no residual memory allocations
   3. if oom or failure on return *ppDocType is NULL
   4  if success on return *ppDocType is a valid string or NULL if
   no matching dictionary or dictionary did not have a doc_type string. */
NW_Status_t
NW_WBXML_Dictionary_publicId_to_doctypeString(NW_Uint32 publicId,
                                              NW_String_t** ppDocType)
{
  NW_WBXML_Dictionary_t *pDictionary
    = NW_WBXML_Dictionary_getByPublicId (publicId);
  *ppDocType = NULL;
  if (pDictionary != NULL) {
    if (pDictionary->doc_type != NULL) {
      *ppDocType = NW_String_new();
      if (*ppDocType != NULL) {
        if (NW_String_initialize(*ppDocType, pDictionary->doc_type,
                                 NW_WBXML_DICTIONARY_CHARSET)
            != NW_STAT_SUCCESS) {
          NW_String_delete(*ppDocType);
          *ppDocType = NULL;
          return NW_STAT_FAILURE;
        }
      } else {
        return NW_STAT_OUT_OF_MEMORY;
      }
    }
  }
  return NW_STAT_SUCCESS;
}

