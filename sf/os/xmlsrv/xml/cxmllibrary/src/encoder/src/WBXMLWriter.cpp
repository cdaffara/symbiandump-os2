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
#include <xml/cxml/nw_wbxml_token.h>
#include <xml/cxml/nw_encoder_wbxmlwriter.h>
#include <xml/cxml/nw_wbxml_parse.h>
#include "cxml_mem.h"

// WLIU_DEBUG: #include "flogger.h"
  /* a reserved name used for encoding tags and attributes that
otherwise aren't in the dictionaries. zzzunknown must be in the
dictionary for both tag names and attribute names */
#define zzzunknownCharCount 10
static const NW_Uint8 zzzunknown[] = {'z','z','z','u','n','k','n','o','w','n','\0'};
#define zzzunknownEncoding HTTP_utf_8


static
NW_Status_t
NW_WBXML_Writer_Memcpy(NW_WBXML_Writer_t* pW, const NW_Uint8* pBytes,
                       NW_Uint32 writeLength)
{
  NW_Status_t s = NW_STAT_SUCCESS;
  if (writeLength == 0) {
    return NW_STAT_FAILURE;
  }
  if (!pW->sizing) {
    if (((pW->index + writeLength - 1) >= pW->byteCount)
        && (pW->growBufCallback != NULL)) {
      /* try to grow buf */
      s = (*pW->growBufCallback)(pW, writeLength);
      if (NW_STAT_IS_FAILURE(s)) {
        return s;
      }
    }
    if ((pW->index + writeLength - 1) >= pW->byteCount) {
      /* grow buf unavailable or didn't alloc enough */
      return NW_STAT_OUT_OF_MEMORY;
    }
    NW_Mem_memcpy(pW->pBuf + pW->index, pBytes, writeLength);
  }
  pW->index += writeLength;
  return s;
}

typedef struct NW_WBXML_Multibyte_s {
  NW_Uint8 length; /* encoded length in bytes from 1..5 */
  NW_Uint8 buf[5];
} NW_WBXML_Multibyte_t;

/* see the WBXML specification for the definition of this function */
static
void
NW_WBXML_EncodeMultibyte(NW_Uint32 x, NW_WBXML_Multibyte_t* pM)
{
  NW_Uint8 i;
  NW_Uint8 littleEndian[5];

  littleEndian[0] = (NW_Uint8)(x & 0x7f);
  for (i = 1; i < 5; i++) {
    x >>= 7; /* unsigned x, no sign extension */
    littleEndian[i] = (NW_Uint8)(x & 0x7f);
    if (!x) {
      break;
    }
  }
  pM->length = i;
  for (i = 0; i < pM->length; i++) {
    pM->buf[i] = littleEndian[pM->length - 1 - i];
    if ((i + 1) < pM->length) {
      pM->buf[i] |= 0x80;
    }
  }
}

static
NW_Status_t
NW_WBXML_Writer_Multibyte(NW_WBXML_Writer_t* pW, NW_Uint32 x)
{
  NW_WBXML_Multibyte_t multibyte;

  NW_WBXML_EncodeMultibyte(x, &multibyte);
  return NW_WBXML_Writer_Memcpy(pW, multibyte.buf,
                                multibyte.length);
}

typedef enum {
    NW_WBXML_TAG,
    NW_WBXML_ATTRIBUTE
} NW_WBXML_Writer_ItemType;

static
NW_Status_t
NW_WBXML_Writer_MaybeSwitchPage(NW_WBXML_Writer_t* pW, NW_Uint8 page,
                                NW_WBXML_Writer_ItemType itemType,
                                NW_Bool* switched)
{
  NW_Status_t s = NW_STAT_SUCCESS;
  NW_Uint8 token = NW_WBXML_SWITCH_PAGE; /* global so on all code pages */
  NW_Bool changePage = 0;

  *switched = NW_FALSE;
  switch (itemType) {
  case NW_WBXML_TAG:
    changePage = (NW_Bool)(page != pW->tagCodePage);
    if (changePage) {
      pW->tagCodePage = page;
      // WLIU_DEBUG: pW->cp_count++;
    }
    break;
  case NW_WBXML_ATTRIBUTE:
    changePage = (NW_Bool)(page != pW->attributeCodePage);
    if (changePage) {
      pW->attributeCodePage = page;
      // WLIU_DEBUG: pW->cp_count++;
    }
    break;
  default:
    s = NW_STAT_FAILURE;
    break;
  }
  if (changePage) { 
    s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
    // WLIU_DEBUG: RFileLogger::WriteFormat(_L("Browser"), _L("cp_count.txt"), EFileLoggingModeAppend, _L("=== cp_count: %x, pW->index: %x \n"), pW->cp_count, pW->index);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    s = NW_WBXML_Writer_Memcpy(pW, &page, 1);
        if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    *switched = NW_TRUE;
  }
  return s;
}

EXPORT_C
NW_Status_t
NW_WBXML_Writer_Header(NW_WBXML_Writer_t* pW, NW_Uint8 WBXMLVersion,
                       NW_Uint32 publicIdentifier, NW_Uint32 charsetMIBEnum,
                       NW_Uint32 stringTableByteCount)

{
  NW_Status_t s;
  NW_Uint32 byteCount;
  NW_Uint8* pBuf;
  NW_Encoder_StringTableIterator_t strTableIterator;

  /* header = version publicid charset stringtable */

  s = NW_WBXML_Writer_Memcpy(pW, &WBXMLVersion, 1);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  s = NW_WBXML_Writer_Multibyte(pW, publicIdentifier);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  s = NW_WBXML_Writer_Multibyte(pW, charsetMIBEnum);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  s = NW_WBXML_Writer_Multibyte(pW, stringTableByteCount);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (stringTableByteCount) {
    NW_ASSERT(pW->stringTableIterateInit);
    NW_ASSERT(pW->stringTableIterateNext);
    s = (*(pW->stringTableIterateInit))(pW->pStringTableObject,
                                        &strTableIterator);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    s = (*(pW->stringTableIterateNext))(&strTableIterator,
                                        &byteCount, &pBuf);
    while (s == NW_STAT_WBXML_ITERATE_MORE){
      s = NW_WBXML_Writer_Memcpy(pW, pBuf, byteCount);
      if (s != NW_STAT_SUCCESS){
        return s;
      }
      s = (*(pW->stringTableIterateNext))(&strTableIterator,
                                          &byteCount, &pBuf);
    }
    if (s != NW_STAT_WBXML_ITERATE_DONE){
      return NW_STAT_FAILURE;
    }
  }
  return NW_STAT_SUCCESS;
}

/* if boolean "set" is 1 then "or" with mask, else "and" with ~mask */
static
NW_Status_t
NW_WBXML_Writer_TagFlagOp(NW_WBXML_Writer_t* pW, NW_Uint32 index,
                          NW_Uint8 mask, NW_Bool set)
{
  NW_Status_t s = NW_STAT_SUCCESS;
  if (!pW->sizing) {
    if (index < pW->byteCount) {
      if (set) {
        (pW->pBuf)[index] |= mask;
      } else {
        (pW->pBuf)[index] &= ~mask;
      }
    } else {
      s = NW_STAT_FAILURE;
    }
  }
  return s;
}

EXPORT_C
NW_Status_t
NW_WBXML_Writer_TagSetContentFlag(NW_WBXML_Writer_t* pW, NW_Uint32 index)
{
  return NW_WBXML_Writer_TagFlagOp(pW, index, NW_WBXML_FLAGS_CONTENT, 1);
}

EXPORT_C
NW_Status_t
NW_WBXML_Writer_TagClearContentFlag(NW_WBXML_Writer_t* pW, NW_Uint32 index)
{
  return NW_WBXML_Writer_TagFlagOp(pW, index, NW_WBXML_FLAGS_CONTENT, 0);
}

EXPORT_C
NW_Status_t
NW_WBXML_Writer_TagSetAttributesFlag(NW_WBXML_Writer_t* pW, NW_Uint32 index)
{
  return NW_WBXML_Writer_TagFlagOp(pW, index, NW_WBXML_FLAGS_ATTRIBUTES, 1);
}

NW_Status_t
NW_WBXML_Writer_TagClearAttributesFlag(NW_WBXML_Writer_t* pW, NW_Uint32 index)
{
  return NW_WBXML_Writer_TagFlagOp(pW, index, NW_WBXML_FLAGS_ATTRIBUTES, 0);
}

NW_Status_t
NW_WBXML_Writer_TagToken(NW_WBXML_Writer_t* pW, NW_Uint16 fqToken,
                    NW_Uint32* pTagIndex)
{
  NW_Status_t s;
  NW_Uint8 token;
  NW_Uint8 page;
  NW_Bool  switched = NW_FALSE;

  token = NW_WBXML_Dictionary_extractToken(fqToken);
  page =  NW_WBXML_Dictionary_extractPage(fqToken);

  s = NW_WBXML_Writer_MaybeSwitchPage(pW, page, NW_WBXML_TAG, &switched);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  *pTagIndex = pW->index;
  return NW_WBXML_Writer_Memcpy(pW, &token, 1); /* tag token */
  }

static
NW_Status_t
NW_WBXML_Writer_Literal(NW_WBXML_Writer_t* pW, NW_Uint32 stringTableIndex)
  {
  NW_Status_t s;

  /* global in WBXML spec, so on all code pages */
  static const NW_Uint8 literalToken = NW_WBXML_LITERAL;

  s = NW_WBXML_Writer_Memcpy(pW, &literalToken, 1);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
  return NW_WBXML_Writer_Multibyte(pW, stringTableIndex);
  }

/* This function will write to the extension string Table. The buffer
 * "pBuf" is not NULL terminated. This can handle any type of encoding.
 */


static
NW_Status_t
NW_WBXML_Writer_StringTableLiteral2(NW_WBXML_Writer_t* pW,
                                   NW_Uint32 charCount,
                                   const NW_Uint8* pBuf,
                                   NW_Uint32 encoding)
{
 NW_Uint32 byteCount = 0;
 NW_Uint32 totalByteCount = 0;
 NW_Ucs2 c;
 NW_Uint32 numbytes;
 NW_Uint8* literalName = NULL;
 NW_Status_t s = NW_STAT_SUCCESS;
 NW_Uint32 i =0;
 NW_Uint32 tableIndex = 0;

 if(pBuf == NULL)
 {
  return NW_STAT_BAD_INPUT_PARAM;
 }

 if (pW->addToStringTable != NULL) 
   {

   /*
    * pBuf is not NULL terminated. So, need to use the following method.
    */

    numbytes = NW_String_readChar( (NW_Byte*) pBuf,&c,encoding);

   /* Calculate the length of string. Also add the number of characters 
    * required for the NULL termination.
    */

    byteCount = (numbytes * charCount) ;
        
    totalByteCount = byteCount + numbytes;


    literalName = (NW_Uint8*) NW_Mem_Malloc(totalByteCount);

    if (literalName != NULL) 
     {
      (void)NW_Mem_memcpy(literalName, pBuf, byteCount );


       for(i=0; i < numbytes; i++)
       {
        literalName[byteCount+i] = '\0';
       }


       /* During the PASS -1, the Literal is also added to the string table
        * so the correct index of Literal can be found. The index of the literal
        * is written in the WBXML buffer. The index is UINT 32 so (index = 01) and
        * (index == 0x80) is encoded differently in the WBXML integer encoding. So, 
        * string table function calculates the correct index. So, the calling function
        * should free the string table after PASS-1. Currently, PASS-1 and PASS-2 is 
        * used in the XML parser only.
        */


       s = (*pW->addToStringTable)(pW->pStringTableObject,
                                    totalByteCount, literalName,
                                    &tableIndex);
      if (NW_STAT_IS_SUCCESS(s))
      {
       s = NW_WBXML_Writer_Literal(pW,tableIndex);
       if(literalName != NULL)
       {
        NW_Mem_Free(literalName);
       }
      }
     }/*end if (literalName != NULL) */
     else
     {
      s = NW_STAT_OUT_OF_MEMORY;
     }
 } /*end if(pW->addToStringTable != NULL)*/

return s;
}/* NW_WBXML_Writer_StringTableLiteral2(..) */

/* Examine the out parameter *pFound to see if the string was found in
the string table.  If it wasn't found, then it can't be written as a literal
in WBXML.  A failure return code means something else went wrong. The key
part of this function is that it just queries the string table and doesn't
try to force the string to be added to the string table. */
static
NW_Status_t
NW_WBXML_Writer_StringTableLiteral(NW_WBXML_Writer_t* pW,
                                   NW_Uint32 byteCount,
                                   const NW_Uint8* pBuf,
                                   NW_Bool* pFound)
{
  NW_Status_t s;
  NW_Uint32 tableIndex = 0;

  *pFound = 0;
  if (pW->getStringTableOffset) {
    s = (*pW->getStringTableOffset)(pW->pStringTableObject,
                                    byteCount, pBuf,
                                    pFound, &tableIndex);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
  }
  if (*pFound) {
    /* global, so on all code pages */
    static const NW_Uint8 tableRefToken = NW_WBXML_STR_T;

    s = NW_WBXML_Writer_Memcpy(pW, &tableRefToken, 1);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    return NW_WBXML_Writer_Multibyte(pW, tableIndex);
  }
  /* *pFound indicates whether the string table entry was found so
     we return success */
  return NW_STAT_SUCCESS;
}

/* charCount should not include null termination character if any exists */
EXPORT_C
NW_Status_t
NW_WBXML_Writer_TagString(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                          NW_Uint32 charCount, NW_Uint32 byteCount,
                          NW_Uint8* pBuf, NW_Uint32* pTagIndex)
{
  NW_Status_t s;
  NW_Uint16 fqToken;

  (void) byteCount;

  /* try token */
  s = NW_WBXML_Dictionary_getTagToken2(pW->pTagDictionary, encoding,
                                       charCount, pBuf, &fqToken);
  if (NW_STAT_IS_SUCCESS(s)) {
    return NW_WBXML_Writer_TagToken(pW, fqToken, pTagIndex);
  }

  /* try string table reference */
  if (pW->addToStringTable != NULL)
  {
    *pTagIndex = pW->index;
    s = NW_WBXML_Writer_StringTableLiteral2(pW, charCount, pBuf, encoding);
    // Return on success or failure (out of memory, etc.) except
    // NW_STAT_STR_TBL_OVERFLOW.
    if (s != NW_STAT_STR_TBL_OVERFLOW)
    {
      return s;
    }
  }
  
  /* no token, not in string table...try to encode as the unknown tag */
  s = NW_WBXML_Dictionary_getTagToken2(pW->pTagDictionary, zzzunknownEncoding,
                                       zzzunknownCharCount,
                                       (NW_Uint8*)&zzzunknown[0],
                                       &fqToken);
  if (NW_STAT_IS_SUCCESS(s)) 
  {
   return NW_WBXML_Writer_TagToken(pW, fqToken, pTagIndex);
  }

  /* getting here means no token, no string table entry
  and the reserved unknown tag is missing from the dictionary */
  NW_ASSERT(s == NW_STAT_SUCCESS); /* force error to be caught here */
  return s;
}

static
NW_Status_t
NW_WBXML_Writer_InlineString(NW_WBXML_Writer_t* pW,
                             NW_Uint32 encoding,
                             NW_Uint32 byteCount,
                             const NW_Uint8* pTextIn)
{
  NW_Uint8* pTextOut = (NW_Uint8*)pTextIn; /* default */
  NW_Status_t s;
  NW_Bool nullTerminated;
  NW_Uint8 token = NW_WBXML_STR_I; /* global so on all code pages */

  nullTerminated = NW_FALSE;

  s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
  if (NW_STAT_IS_FAILURE(s)) {
    if (pTextOut != pTextIn) {
      NW_Mem_Free(pTextOut);
    }
    return s;
  }
  s = NW_WBXML_Writer_Memcpy(pW, pTextOut, byteCount);
  if (NW_STAT_IS_FAILURE(s)) {
    if (pTextOut != pTextIn) {
      NW_Mem_Free(pTextOut);
    }
    return s;
  }
  if (pTextOut != pTextIn) {
    NW_Mem_Free(pTextOut);
  }

  /* test string to see if written bytes are already null terminated */
  /* FUTURE MISSING UTILITY SUPPORT
  this char encoding API isn't implemented yet
  NW_Char_SimpleString_t n;
  s = NW_CharEncoding_nullTermination(encoding, &n);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  test if null termination already written and if not...
  s = NW_WBXML_Writer_Memcpy(pW, n.buf, n.byteCount);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  */
  /* BUG HACK BEGIN
  Should not assume null termination missing but this works
  with the XMLP client.  Remove this when char encoding
  null termination utility support is available */

  if (encoding == HTTP_iso_10646_ucs_2){
    /* just look for any 1 bits in the UCS2 char */
    NW_Uint8 lastChar = (pTextIn + byteCount)[-1];
    lastChar |= (pTextIn + byteCount)[-2];
    if (lastChar == 0){
      nullTerminated = NW_TRUE;
    }
  }
  else if ((encoding == HTTP_iso_8859_1) ||
            (encoding == HTTP_utf_8) ||
            (encoding == HTTP_us_ascii))
  {
    NW_Uint8 lastChar = (pTextIn + byteCount)[-1];
    if (lastChar == 0){
      nullTerminated = NW_TRUE;
    }
  }
  else{
    NW_ASSERT(0);
  }
  if (!nullTerminated)
  {
  token = 0;
  s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  if (encoding == HTTP_iso_10646_ucs_2) {
    s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
  }
  }
  /* BUG HACK END */
  return NW_STAT_SUCCESS;
}

/*
1. charCount should not include null termination character if any exists
2. if there is an attribute name but no value use valueCharCount==0,
   valueByteCount==0 and pValue==NULL
*/
EXPORT_C
NW_Status_t
NW_WBXML_Writer_AttributeAndValue2(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                                  NW_Uint32 nameCharCount, NW_Uint8* pName,
                                  NW_Uint32 valueCharCount, NW_Uint32 valueByteCount,
                                  NW_Uint8* pValue,
                                  NW_Uint32* cp_count)
{
  /*
  Note: According to the WBXML specification, attribute codes (tokens) may
  encode all or a prefix of the attribute value as well as the attribute name.
  The implementation here is simpler, it assumes that attribute names are
  encoded separately from attribute values.
  */
  NW_Status_t s;
  NW_Uint16 fqToken;
  NW_Uint8 token;
  NW_Uint8 page;
  NW_Bool switched = NW_FALSE;
  NW_Bool extStrTbl = NW_FALSE;

  *cp_count = 0;
  /* Note: This dictionary lookup function is insufficient to look up
  combined attribute and value.  To do that we must be able to pass both
  the name and value and get both a return token and a character count
  of the value characters represented by the token so we can tell if there
  are remaing value characters to write. */
  s = NW_WBXML_Dictionary_getAttributeNameToken(pW->pAttributeDictionary,
                                                encoding,
                                                nameCharCount, pName, &fqToken);
  if (NW_STAT_IS_FAILURE(s)) {
    /* failure here means that we should encode as the reserved unknown
    name, so force that */
    fqToken = NW_WBXML_LITERAL;
  }
  token = NW_WBXML_Dictionary_extractToken(fqToken);
  page =  NW_WBXML_Dictionary_extractPage(fqToken);


  if ((token & NW_WBXML_MASK_TAG_ID) == NW_WBXML_LITERAL)
  {
   
   /* If strig table is supported then the attribute name not found 
    * in the dictionary is stored in string table a LITRAL otherwise
    * the workaround is to encode the name as a reserved unknown. 
   */

     if (pW->getStringTableOffset)
     {
      s = NW_WBXML_Writer_StringTableLiteral2(pW,nameCharCount,pName, encoding);

      if (NW_STAT_IS_FAILURE(s)) 
      {
       return s;
      }
      extStrTbl = NW_TRUE;
     }//end if(pW->getStringTableOffset)
     else
     {

       s = NW_WBXML_Dictionary_getAttributeNameToken(pW->pAttributeDictionary,
                                                  zzzunknownEncoding,
                                                  zzzunknownCharCount,
                                                  (NW_Uint8*)&zzzunknown[0],
                                                  &fqToken);
       if (NW_STAT_IS_FAILURE(s)) 
       {
           /* failure here means the reserved unknown name is missing from
           the dictionary */
          NW_ASSERT(s==NW_STAT_SUCCESS);/*force error to be caught here*/
          return s;
       }
        token = NW_WBXML_Dictionary_extractToken(fqToken);
        page =  NW_WBXML_Dictionary_extractPage(fqToken);
     }//end else
  }//end if ((token & NW_WBXML_MASK_TAG_ID) == NW_WBXML_LITERAL)

     //
     //If attribute name is found in the dictionary only then check for the
     // Page switching.
     //

     if(!extStrTbl)
     {

        s = NW_WBXML_Writer_MaybeSwitchPage(pW, page, NW_WBXML_ATTRIBUTE, &switched);
        if (NW_STAT_IS_FAILURE(s)) {
            return s;
         }
        if (switched)
            *cp_count = *cp_count + 1 ;
        s = NW_WBXML_Writer_Memcpy(pW, &token, 1); /* attribute name token */
        if (NW_STAT_IS_FAILURE(s)) {
           return s;
         }
     }//end if(!extStrTbl)   


  if ((valueCharCount == 0) && (pValue != NULL))
  {
    token = NW_WBXML_STR_I; /* global so on all code pages */

    s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    token = 0;
    s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    if (encoding == HTTP_iso_10646_ucs_2) {
      s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
      if (NW_STAT_IS_FAILURE(s)) {
        return s;
      }
    }
  }
  else
  {
    if ((valueCharCount != 0) && (pValue != NULL))
    {
      s = NW_WBXML_Dictionary_getAttributeValueToken(pW->pAttributeDictionary,
                                                    encoding,
                                                    valueCharCount, pValue,
                                                    &fqToken);
      if (NW_STAT_IS_FAILURE(s)) {
        /* failure means couldn't find a token, force to literal */
        fqToken = NW_WBXML_LITERAL; /* page is irrelevant here */
      }
      token = NW_WBXML_Dictionary_extractToken(fqToken);
      page =  NW_WBXML_Dictionary_extractPage(fqToken);
      if ((token & NW_WBXML_MASK_TAG_ID) == NW_WBXML_LITERAL)
      {
        s = NW_WBXML_Writer_InlineString(pW, encoding,
                                        valueByteCount, pValue);
        if (NW_STAT_IS_FAILURE(s)) {
          return s;
        }
      } else {
        s = NW_WBXML_Writer_MaybeSwitchPage(pW, page,
                                            NW_WBXML_ATTRIBUTE, &switched);
        if (NW_STAT_IS_FAILURE(s)) {
          return s;
        }
        if (switched)
          *cp_count = *cp_count + 1 ;
        s = NW_WBXML_Writer_Memcpy(pW, &token, 1);/*attr value token*/
        if (NW_STAT_IS_FAILURE(s)) {
          return s;
        }
      }
    }
  }
  return NW_STAT_SUCCESS;
}

EXPORT_C
NW_Status_t
NW_WBXML_Writer_AttributeAndValue(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                                  NW_Uint32 nameCharCount, NW_Uint8* pName,
                                  NW_Uint32 valueCharCount, NW_Uint32 valueByteCount,
                                  NW_Uint8* pValue)
{
  NW_Uint32 cp_cnt = 0;
  return NW_WBXML_Writer_AttributeAndValue2(pW, encoding, nameCharCount, pName, valueCharCount, 
                                  valueByteCount, pValue, &cp_cnt);

}

NW_Status_t
NW_WBXML_Writer_AttributeNameString(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                                    NW_Uint32 nameCharCount,
                                    NW_Uint32 nameByteCount, NW_Uint8* pName)
{
  NW_Status_t s;
  NW_Uint16 fqToken;

  (void) nameByteCount;

  /* try token */
  s = NW_WBXML_Dictionary_getAttributeToken2(pW->pAttributeDictionary,
                                             encoding, nameCharCount,
                                             pName, &fqToken,
                                             NW_TRUE /* isName */);
  if (NW_STAT_IS_SUCCESS(s)) {
    return NW_WBXML_Writer_AttributeToken(pW, fqToken);
  }

  /* try string table */
  if (pW->addToStringTable != NULL) {
    s = NW_WBXML_Writer_StringTableLiteral2(pW, nameCharCount, pName, encoding);
    return s;
    
  }
  /* no token, no string table so try unknown token */
  s = NW_WBXML_Dictionary_getAttributeToken2(pW->pAttributeDictionary,
                                             zzzunknownEncoding,
                                             zzzunknownCharCount,
                                             (NW_Uint8*)&zzzunknown[0],
                                             &fqToken,
                                             NW_TRUE /* isName */);
  if (NW_STAT_IS_SUCCESS(s)) {
    return NW_WBXML_Writer_AttributeToken(pW, fqToken);
  }
  NW_ASSERT(NW_STAT_IS_SUCCESS(s)); /* force debug to stop here */
  return NW_STAT_FAILURE;
}

NW_Status_t
NW_WBXML_Writer_AttributeToken(NW_WBXML_Writer_t* pW,
                               NW_Uint16 fqToken)

{
  NW_Status_t s;
  NW_Uint8 token;
  NW_Uint8 page;
  NW_Bool switched = NW_FALSE;

  token = NW_WBXML_Dictionary_extractToken(fqToken);
  page =  NW_WBXML_Dictionary_extractPage(fqToken);

  s = NW_WBXML_Writer_MaybeSwitchPage(pW, page, NW_WBXML_ATTRIBUTE, &switched);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  return NW_WBXML_Writer_Memcpy(pW, &token, 1); /* attribute token */
}

NW_Status_t
NW_WBXML_Writer_Entity(NW_WBXML_Writer_t* pW,
                       NW_Uint32 entity)
{
  NW_Status_t s;
  static const NW_Uint8 entityToken = NW_WBXML_ENTITY;

  s = NW_WBXML_Writer_Memcpy(pW, &entityToken, 1);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  return NW_WBXML_Writer_Multibyte(pW, entity);
}

/*
This is a helper function for the EXT_T_[0,1,2] forms for these extensions.
*/
NW_Status_t
NW_WBXML_Writer_ExtensionUseStringTable(NW_WBXML_Writer_t* pW,
                                        NW_Uint16 fqToken,
                                        NW_Uint32 byteCount,
                                        NW_Uint8* pBuf)
{
  NW_Uint32 tableIndex;
  NW_Status_t s;

  if (pW->addToStringTable != NULL) {
    s = (*pW->addToStringTable)(pW->pStringTableObject,
                                byteCount, pBuf, &tableIndex);
    if (NW_STAT_IS_SUCCESS(s)) {
      return NW_WBXML_Writer_Extension(pW, fqToken, tableIndex, 0, NULL);
    }
  }
  return NW_STAT_FAILURE;
}

/*
There are three type of extensions:

1. one of three possible single byte tokens
     NW_WBXML_EXT_[0,1,2]
2. an extension token followed by a multibyte encoded NW_Uint32 value
     NW_WBXML_EXT_T_[0,1,2] multibyte(anonymousValue)
3. an extension token followed by an in-line string
     NW_WBXML_EXT_I_[0,1,2] null-terminated-string

This function handles all three cases so you have to pass the appropriate
arguments for each case:

1. fqToken should be one of NW_WBXML_EXT_[0,1,2] (with any page value)
   and anonymousValue, byteCount and pBuf should be 0 or NULL

2. fqToken should be one of NW_WBXML_EXT_T_[0,1,2] (with any page value),
   anonymousValue should be the value to multibyte encode
   and byteCount and pBuf should be 0 and NULL

3. fqToken should be one of NW_WBXML_EXT_I_[0,1,2] (with any page value),
   byteCount should be the byte length of the null terminated string
   pointed to by pBuf
   and anonymousValue is ignored
*/
NW_Status_t
NW_WBXML_Writer_Extension(NW_WBXML_Writer_t* pW,
                          NW_Uint16 fqToken,
                          NW_Uint32 anonymousValue,
                          NW_Uint32 byteCount,
                          NW_Uint8* pBuf)
{
  NW_Status_t s;
  NW_Uint8 token = (NW_Uint8)(fqToken & NW_WBXML_MASK_TOKEN);

  s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }

  switch (token) {
  case NW_WBXML_EXT_0:
  case NW_WBXML_EXT_1:
  case NW_WBXML_EXT_2:
    NW_ASSERT(byteCount == 0);
    NW_ASSERT(pBuf == NULL);
    break;
  case NW_WBXML_EXT_T_0:
  case NW_WBXML_EXT_T_1:
  case NW_WBXML_EXT_T_2:
    NW_ASSERT(byteCount == 0);
    NW_ASSERT(pBuf == NULL);
    s = NW_WBXML_Writer_Multibyte(pW, anonymousValue);
    break;
  case NW_WBXML_EXT_I_0:
  case NW_WBXML_EXT_I_1:
  case NW_WBXML_EXT_I_2:
    NW_ASSERT(byteCount > 0);
    NW_ASSERT(pBuf != NULL);
    s = NW_WBXML_Writer_Memcpy(pW, pBuf, byteCount);
    break;
  default:
    NW_ASSERT(!"wrong token");
    return NW_STAT_FAILURE;
  }
  return s;
}

EXPORT_C
NW_Status_t
NW_WBXML_Writer_Opaque(NW_WBXML_Writer_t* pW, NW_Uint32 byteCount,
                       NW_Uint8* pBuf)
{
  NW_Status_t s;
  NW_Uint8 token = NW_WBXML_OPAQUE; /* global so on all code pages */
  s = NW_WBXML_Writer_Memcpy(pW, &token, 1);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  s = NW_WBXML_Writer_Multibyte(pW, byteCount);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }
  s = NW_WBXML_Writer_Memcpy(pW, pBuf, byteCount);
  return s;
}

EXPORT_C
NW_Status_t
NW_WBXML_Writer_Text(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                     NW_Uint32 byteCount, const NW_Uint8* pText)
{
  NW_Status_t s;
  NW_Bool found = 0;

  /* try string table */
  s = NW_WBXML_Writer_StringTableLiteral(pW, byteCount, pText, &found);
  if (NW_STAT_IS_FAILURE(s) || found) {
    return s;
  }
  /* back off to inline string */
  return NW_WBXML_Writer_InlineString(pW, encoding, byteCount, pText);
}

EXPORT_C
NW_Status_t
NW_WBXML_Writer_PI(NW_WBXML_Writer_t* pW)
{
  NW_Uint8 piToken = 0x43; /* global, so on all code pages */
  return NW_WBXML_Writer_Memcpy(pW, &piToken, 1);
}


EXPORT_C
NW_Status_t
NW_WBXML_Writer_End(NW_WBXML_Writer_t* pW)
{
  NW_Uint8 endToken = NW_WBXML_END; /* global, so on all code pages */
  return NW_WBXML_Writer_Memcpy(pW, &endToken, 1);
}

EXPORT_C
void
NW_WBXML_Writer_Initialize(NW_WBXML_Writer_t* pW,
                           NW_Uint32 byteCount, NW_Uint8* pBuf,
                           NW_WBXML_Writer_GrowBuf_t growBufCallback,
                           NW_WBXML_Dictionary_t* pTagDictionary,
                           NW_WBXML_Dictionary_t* pAttributeDictionary,
                           NW_WBXML_Writer_GetStringTableOffset_t getStringTableOffset,
                           NW_WBXML_Writer_AddToStringTable_t addToStringTable,
                           void* pStringTableObject,
                           NW_WBXML_Writer_StringTableIterateInit_t stringTableIterateInit,
                           NW_WBXML_Writer_StringTableIterateNext_t stringTableIterateNext,
                           NW_Bool sizing)
{
  pW->index = 0;
  pW->byteCount = byteCount;
  pW->pBuf = pBuf;
  pW->growBufCallback = growBufCallback;
  pW->pTagDictionary = pTagDictionary;
  pW->pAttributeDictionary = pAttributeDictionary;
  pW->getStringTableOffset = getStringTableOffset;
  pW->addToStringTable = addToStringTable;
  pW->pStringTableObject = pStringTableObject;
  pW->stringTableIterateInit = stringTableIterateInit;
  pW->stringTableIterateNext = stringTableIterateNext;
  pW->tagCodePage = 0;
  pW->attributeCodePage = 0;
  pW->sizing = sizing;
  //WLIU_DEBUG: pW->cp_count = 0;
}

void
NW_WBXML_Writer_SetToSizing(NW_WBXML_Writer_t* pW)
{
  pW->index = 0;
  pW->tagCodePage = 0;  /* TBD BUG ? will this give correct code pages */
  pW->attributeCodePage = 0;/* TBD BUG ? will this give correct code pages */
  pW->sizing = 1;
  // WLIU_DEBUG: pW->cp_count = 0;
}


void
NW_WBXML_Writer_SetToWrite(NW_WBXML_Writer_t* pW, NW_Uint32 byteCount, NW_Uint8* pBuf)
{
  pW->index = 0;
  pW->byteCount = byteCount;
  pW->pBuf = pBuf;
  pW->tagCodePage = 0;  /* TBD BUG ? will this give correct code pages */
  pW->attributeCodePage = 0;/* TBD BUG ? will this give correct code pages */
  pW->sizing = 0;
  // WLIU_DEBUG: pW->cp_count = 0;
}
