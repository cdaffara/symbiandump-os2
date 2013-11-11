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

#include <string.h>
#include "cxml_internal.h"
#include <xml/cxml/nw_encoder_wbxmlwriter.h>
#include <xml/cxml/nw_xmlp_xmlparser.h>
#include <xml/cxml/nw_xmlp_xmlp2wbxml.h>
#include <xml/cxml/nw_encoder_stringtable.h>
#include "cxml_xmlp_entity.h"


typedef struct CXML_Encoding_String_s {
   NW_Int8* charSetStr;
   NW_Uint32   encodingVal;
}CXML_Encoding_String_t;

/* Number of encoding string supported */

#define CXML_Encoding_Str_Num 5

static
const CXML_Encoding_String_t CXML_EncodingStr_Array[] = 
{
 {(NW_Int8*) "utf-8", HTTP_utf_8 },
 {(NW_Int8*) "us-ascii", HTTP_us_ascii  },
 {(NW_Int8*) "iso-10646-ucs-2", HTTP_iso_10646_ucs_2 },
 {(NW_Int8*) "iso-8859-1", HTTP_iso_8859_1 },
 {(NW_Int8*) "utf-8", HTTP_utf_16}
};

typedef struct NW_XML_XmlpWbxmlEncoder_s
{
    NW_WBXML_Writer_t* pE;
    /*
    need access to last tag token to backpatch attribute and content flags
    */
    NW_Uint32 lastTagTokenIndex;
    NW_Uint32 publicID; /* needed to pass to start callback */
    NW_Uint32 encoding; /* needed to pass to start callback */

} NW_XML_XmlpWbxmlEncoder_t;

/* The following function finds the encoding in the XML declaration statement (if any). 
 * This function is called only when there is no BOM present and encoding is assumed 
 * ASCII/Latine-1.
 * e.g. <?xml version="1.0" encoding="iso-8859-1"?>, here XML Declaration has the encoding
 * of "iso-8859-1"
 */

static NW_Bool NW_XML_FindEncoding(NW_Uint32 length, const unsigned char* pBuf, NW_Uint32* encodingVal)
{

 NW_Bool s = NW_FALSE;
 NW_Uint8* sourceWalker     = NULL;
 NW_Uint8* encodingStart    = NULL;
 NW_Uint8* encodingValStart = NULL;
 NW_Uint8* endXMLDecl       = NULL;
 NW_Uint32  xmlDeclLen       = 0;
 NW_Uint32  encodingValLen   = 0;
 NW_Uint32 i = 0;

 /* Look for end of the XML Declration statement */

 endXMLDecl = (NW_Uint8*) strstr((char*) pBuf+2, "?>");

 if(endXMLDecl)
 {
  xmlDeclLen =  endXMLDecl -  pBuf;


     if(xmlDeclLen < length)
     {
      encodingStart = (NW_Uint8*) strstr(( (char*)(pBuf+2) ),"encoding");

      /*Move the pointer after the encoding */


       if(encodingStart)
       {
        sourceWalker = encodingStart + 8;

        /*Skip any space */
        while(CXML_Str_Isspace((NW_Ucs2)(*sourceWalker & 0xffff) ) ) 
        {
            sourceWalker++;
        }

        if(*sourceWalker == '=')
        {
            sourceWalker++;
            while(CXML_Str_Isspace((NW_Ucs2)(*sourceWalker & 0xffff) ) )
            {
                sourceWalker++;
            }
        }

        if( (*sourceWalker == '\'') || (*sourceWalker == '\"') )
        {
         sourceWalker++;
        }

		 encodingValStart = sourceWalker;

		/* End the Encoding value consume if one of following condition is reached:
		 * 1) Closing quote has encounted.
		 * 2) '>' closing tag has encountered
		 * 3) Space had encounted
		 * 4) '?' has encounded.
		 * There is possibilty of some arguments after encoding like following:
		 * <?xml version="1.0" encoding="iso-8859-1" standalone="no"?>
		 */

		while( (*sourceWalker != '\'') && (*sourceWalker != '\"') &&
			   (*sourceWalker != '>') && (*sourceWalker != '?') )
		{
		 encodingValLen++;
		 sourceWalker++;
		}

        for(i=0; i < CXML_Encoding_Str_Num; i++)
	    {
         if(!strncmp((char*) encodingValStart, (char*) CXML_EncodingStr_Array[i].charSetStr,encodingValLen))
         {
          *encodingVal = CXML_EncodingStr_Array[i].encodingVal;
          s = NW_TRUE;
          break;
         }
        }/*end for() */
       }/*end if(encodingStart)*/
     }/*end if(xmlDeclLen < length)*/
 }/*end if(endXMLDecl)*/

 return s;
}/*end NW_XML_FindEncoding()*/

NW_Status_t
EXPORT_C
NW_XML_ComputeEncoding(NW_Uint32 length, const unsigned char* pBuf,
                             NW_Uint32* pEncoding, NW_Endianness_t* pEndianness)
{
    NW_Uint32 c0, c1, c2, c3;
    if (length < 4)
    {
        return NW_STAT_FAILURE;
    }
    c0 = pBuf[0];
    c1 = pBuf[1];
    c2 = pBuf[2];
    c3 = pBuf[3];
    if (c0 == 0 && c1 == 0 && c2 == 0xfe && c3 == 0xff)
    {
        *pEncoding = 1001; /* ISO-10646-UCS-4 IANA MIBenum */
        *pEndianness = NW_BIG_ENDIAN;
    }
    else if (c0 == 0 && c1 == 0 && c2 == 0 && c3 == 0x3c)
    {
        *pEncoding = 1001; /* ISO-10646-UCS-4 IANA MIBenum */
        *pEndianness = NW_BIG_ENDIAN;
    }
    else if (c0 == 0xff && c1 == 0xfe && c2 == 0 && c3 == 0)
    {
        *pEncoding = 1001; /* ISO-10646-UCS-4 IANA MIBenum */
        *pEndianness = NW_LITTLE_ENDIAN;
    }
    else if (c0 == 0x3c && c1 == 0 && c2 == 0 && c3 == 0)
    {
        *pEncoding = 1001; /* ISO-10646-UCS-4 IANA MIBenum */
        *pEndianness = NW_LITTLE_ENDIAN;
    }
    else if (c0 == 0xfe && c1 == 0xff && !(c2 == 0 && c3 == 0))
    {
        /* call it UCS-2 instead of UTF-16 IANA MIBenum, favor over
           UTF-16BE 1013 */
        *pEncoding = 1000;
        *pEndianness = NW_BIG_ENDIAN;
    }
    else if (c0 == 0 && c1 == 0x3c && c2 == 0 && c3 == 0x3f)
    {
        /* could be UTF-16 but must read xml encoding decl to tell */
        *pEncoding = 1000; /* UCS-2 IANA MIBenum */
        *pEndianness = NW_BIG_ENDIAN;
    }
    else if (c0 == 0xff && c1 == 0xfe && !(c2 == 0 && c3 == 0))
    {
         /* call it UCS-2 instead of UTF-16 IANA MIBenum, favor over
            UTF-16LE 1014 */
        *pEncoding = 1000;
        *pEndianness = NW_LITTLE_ENDIAN;
    }
    else if (c0 == 0x3c && c1 == 0 && c2 == 0x3f && c3 == 0)
    {
        /* could be UTF-16 but must read xml encoding decl to tell */
        *pEncoding = 1000; /* UCS-2 IANA MIBenum */
        *pEndianness = NW_LITTLE_ENDIAN;
    }
    else if (c0 == 0x3c && c1 == 0x3f && c2 == 0x78 && c3 == 0x6d)
    {
        /* could be anything with anything with an ASCII subset,
        must read xml encoding decl to tell */
        if( !NW_XML_FindEncoding (length,pBuf, pEncoding) )
        {
        /*If encoding is not in the XML Declaration */
        *pEncoding = 106; /* UTF-8 IANA MIBenum, no endianness */
        }
        *pEndianness = NW_NATIVE_ENDIAN; 
    }
    else if (c0 == 0xef && c1 == 0xbb && c2 == 0xbf) /* c3 ignored */
    {
        *pEncoding = 106; /* UTF-8 IANA MIBenum, no endianness */
        *pEndianness = NW_NATIVE_ENDIAN; /* Ha! */
    }
    else
    {
        /* default to UTF-8 and let the parser barf if it isn't */
        *pEncoding = 106; /* UTF-8 IANA MIBenum, no endianness */
        *pEndianness = NW_NATIVE_ENDIAN; /* Ha! */
    }
    return NW_STAT_SUCCESS;
}



/* When encoding is UCS-2, the parser has changed the byte order to
native, however, the WBXML should be written in network (i.e., big
endian) order.  The way this is done is to change the byte order
in-place to network order, write the WBXML, and then put the byte
order back to native in-place. */
static
void
NW_XML_XmlpWbxmlEncoder_Ucs2NativeToNetworkByteOrder(NW_Uint32 byteCount,
                                                     NW_Uint8* pBuf)
{
  NW_Uint32 i;
  NW_Uint16 c_ucs2 = 1;

  if (((NW_Uint8*)&c_ucs2)[0] == 1) { /* test for little endian host */
    for (i = 0; i < byteCount; i += 2) {
      (void)NW_Mem_memcpy(&c_ucs2, pBuf + i, sizeof(NW_Uint16));
      pBuf[i] = (NW_Uint8)((c_ucs2 >> 8) & 0xff);
      pBuf[i+1] = (NW_Uint8)(c_ucs2 & 0xff);
    }
  }
}

static
void
NW_XML_XmlpWbxmlEncoder_Ucs2NetworkToNativeByteOrder(NW_Uint32 byteCount,
                                                     NW_Uint8* pBuf)
{
  NW_Uint32 i;
  NW_Uint16 c_ucs2 = 1;

  if (((NW_Uint8*)&c_ucs2)[0] == 1) { /* test for little endian host */
    for (i = 0; i < byteCount; i += 2) {
      /* this is a clever trick: pick up bytes in big endian order,
         force the result to memory via taking address of result which
         will put the 16-bits into native byte order, then copy the
         result over the original bytes */
      c_ucs2 = (NW_Uint16)((pBuf[i] << 8) | pBuf[i+1]);
      (void)NW_Mem_memcpy(pBuf + i, &c_ucs2, sizeof(NW_Uint16));
    }
  }
}

/* end of line normalization replaces DOS or Mac style end of line
with Unix style end of line.  If no normalization is needed, then on
return *ppTextOut == pTextIn (i.e., no malloc was done), otherwise you
need to call NW_Mem_Free(*ppTextOut) when you are done with the output
NOTE: only works for native byte order!
*/
static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_EndOfLineNormalization(NW_Uint32 encoding,
                                               NW_Uint32* pCharCount,
                                               NW_Uint32* pByteCount,
                                               const NW_Uint8* pTextIn,
                                               NW_Uint8** ppTextOut)
{
  NW_Uint32 i;
  NW_Uint32 j;
  NW_Uint32 elideCharCount;
  NW_Uint32 elideByteCount;
  NW_Int32 byteCount;
  NW_Ucs2 cUCS2;
  NW_Uint8 crHit;
  NW_Uint8 needsNormalization = 0;

  /* the output is the input unless it must be normalized */
  *ppTextOut = (NW_Uint8*)pTextIn;

  /* scan to count #xD's that need translating */
  elideCharCount = 0;
  elideByteCount = 0;
  crHit = 0;
  for (i = 0; i < *pByteCount; i += (NW_Uint32)byteCount) {
    byteCount = NW_String_readChar((NW_Uint8*)&(pTextIn[i]), &cUCS2, encoding);
    if (byteCount < 0) {
      return NW_STAT_FAILURE;
    }
    /* catch DOS 0xd 0xa eol */
    if (crHit && (cUCS2 == 0xa)) {
      elideCharCount++;
      elideByteCount += (NW_Uint32)byteCount;
    }
    if (cUCS2 == 0xd) {
      needsNormalization = 1;
      crHit = 1;
    } else {
      crHit = 0;
    }
  }
  if (needsNormalization) {
    /* alloc a new string */
    *ppTextOut = (NW_Uint8*) NW_Mem_Malloc(*pByteCount - elideByteCount);
    if (*ppTextOut == NULL) {
      *ppTextOut = (NW_Uint8*)pTextIn;
      return NW_STAT_OUT_OF_MEMORY;
    }

    /* make a normalized copy of input */
    crHit = 0;
    j = 0;
    for (i = 0; i < *pByteCount; i += (NW_Uint32)byteCount) {
      byteCount = NW_String_readChar((NW_Uint8*)&(pTextIn[i]), &cUCS2, encoding);
      if (byteCount < 0) {
        NW_Mem_Free(*ppTextOut);
        *ppTextOut = (NW_Uint8*)pTextIn;
        return NW_STAT_FAILURE;
      }
      /* check that readChar isn't trying to read off end of buffer */
      NW_ASSERT((i + byteCount - 1) < *pByteCount);
      if (cUCS2 == 0xd) {
        crHit = 1;
        if (byteCount == 1) {
          (*ppTextOut)[j++] = 0xa;
        } else if (byteCount == 2) {
          /* this looks odd but takes into account either endianess */
          if (pTextIn[i] == 0xd) {
            (*ppTextOut)[j++] = (NW_Uint8)0xa;
            (*ppTextOut)[j++] = 0;
          } else {
            (*ppTextOut)[j++] = 0;
            (*ppTextOut)[j++] = (NW_Uint8)0xa;
          }
        } else {
          NW_ASSERT(byteCount <= 2); /* code bug, force debug stop here */
          NW_Mem_Free(*ppTextOut);
          *ppTextOut = (NW_Uint8*)pTextIn;
          return NW_STAT_FAILURE;
        }
      } else {
        if (!(crHit && (cUCS2 == 0xa))) {
          (void)NW_Mem_memcpy(&((*ppTextOut)[j]), &(pTextIn[i]), (NW_Uint32)byteCount);
          j += (NW_Uint32)byteCount;
        }
        crHit = 0;
      }
    }
    *pByteCount -= elideByteCount;
    *pCharCount -= elideCharCount;
  }
  return NW_STAT_SUCCESS;
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_New(NW_Uint32 publicID, NW_Uint32 encoding, void** ppV)
{
    NW_WBXML_Dictionary_t* pDictionary;
    NW_XML_XmlpWbxmlEncoder_t** ppTE = (NW_XML_XmlpWbxmlEncoder_t**)ppV;
    *ppTE = (NW_XML_XmlpWbxmlEncoder_t*) NW_Mem_Malloc(sizeof(NW_XML_XmlpWbxmlEncoder_t));
    if (!*ppTE) {
        return NW_STAT_OUT_OF_MEMORY;
    }
    (*ppTE)->lastTagTokenIndex = 0;
    (*ppTE)->publicID = publicID;
    (*ppTE)->encoding = encoding;
    (*ppTE)->pE = (NW_WBXML_Writer_t*) NW_Mem_Malloc(sizeof(NW_WBXML_Writer_t));
    if (!((*ppTE)->pE)) {
        return NW_STAT_OUT_OF_MEMORY;
    }
    /* Note that we only have access to one dictionary and we
    should really have two: one for tags and one for attributes. */
    pDictionary = NW_WBXML_Dictionary_getByPublicId((*ppTE)->publicID);

    if(  ( CXML_Additional_Feature_Supprted() & CXML_DTD_SUPPORT_ON) )
    {
     NW_Encoder_StringTable_t* strTable = NW_Encoder_StringTable_new();
   

     NW_WBXML_Writer_Initialize((*ppTE)->pE,
                             0, NULL, NULL, /* no mem needed for sizing */
                             pDictionary,
							 pDictionary,
                             NW_Encoder_StringTable_getStringTableOffset,
							 NW_Encoder_StringTable_addToStringTable,
                             strTable, 
							 NW_Encoder_StringTable_StringTableIterateInit,
							 NW_Encoder_StringTable_StringTableIterateNext,
                             1 /* do sizing only */);
    }
    else
    {

    NW_WBXML_Writer_Initialize((*ppTE)->pE,
                               0, NULL, NULL, /* no mem needed for sizing */
                               pDictionary, pDictionary,
                               NULL, NULL, NULL, NULL, NULL, /* string table stuff */
                               1 /* do sizing only */);
    }
    return NW_STAT_SUCCESS;
}

static
void
NW_XML_XmlpWbxmlEncoder_SetToWrite(NW_XML_XmlpWbxmlEncoder_t* pTE,
                                   NW_Uint32 byteLength, NW_Uint8* pBuf,
                                   NW_WBXML_Writer_GrowBuf_t growBufCallback)
{

    if( ( CXML_Additional_Feature_Supprted() & CXML_DTD_SUPPORT_ON) )
    {
        NW_Encoder_StringTable_t* strTable = NW_Encoder_StringTable_new();
   

        NW_WBXML_Writer_Initialize(pTE->pE,
                             byteLength, pBuf,
                             growBufCallback,
                             pTE->pE->pTagDictionary,
							 pTE->pE->pAttributeDictionary,
                             NW_Encoder_StringTable_getStringTableOffset,
							 NW_Encoder_StringTable_addToStringTable,
                             strTable, 
							 NW_Encoder_StringTable_StringTableIterateInit,
							 NW_Encoder_StringTable_StringTableIterateNext,
                             0 /* don't do sizing, write */);
    }
   else
    {
    NW_WBXML_Writer_Initialize(pTE->pE,
                               byteLength, pBuf,
                               growBufCallback,
                               pTE->pE->pTagDictionary,
                               pTE->pE->pAttributeDictionary,
                               NULL, NULL, NULL, NULL, NULL,  /* string table stuff */
                               0 /* don't do sizing, write */);
    }
}

void
NW_XML_XmlpWbxmlEncoder_Delete(void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_Mem_Free(pE); /* note that this doesn't free WBXML buf and string table. */
    NW_Mem_Free(pTE);
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_StartDocument_CB(NW_XML_Reader_t* pT, void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;

    NW_REQUIRED_PARAM(pT);

    /* Note: We do not build a string table because it isn't clear if
    it is worthwhile. */

    return NW_WBXML_Writer_Header(pE, 3, pTE->publicID, pTE->encoding, 0);
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Tag_Start_CB(NW_XML_Reader_t* pT,
                                     const NW_XML_Reader_Interval_t* pI_name,
                                     void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_Uint32 length;
    NW_Uint32 nameByteLength;
    NW_Uint8* pName;
    NW_Status_t s;
    if (!NW_XML_Reader_Interval_IsWellFormed(pI_name)) {
        return NW_STAT_FAILURE;
    }
    length = pI_name->stop - pI_name->start;
    nameByteLength = length;
    s = NW_XML_Reader_DataAddressFromBuffer(pT, pI_name->start,
                                         &nameByteLength,
                                         &pName);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (nameByteLength != length) {
        return NW_STAT_FAILURE;
    }
    length = pI_name->charStop - pI_name->charStart;
    s = NW_WBXML_Writer_TagString(pE, pTE->encoding, length, nameByteLength,
                            pName, &(pTE->lastTagTokenIndex));
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    /* default to content but erase if see empty tag end */
    return NW_WBXML_Writer_TagSetContentFlag(pE, pTE->lastTagTokenIndex);
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Attr_Start_CB(NW_XML_Reader_t* pT,
                                      const NW_XML_Reader_Interval_t* pI_name,
                                      void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_REQUIRED_PARAM(pT);
    NW_REQUIRED_PARAM(pI_name);
    return NW_WBXML_Writer_TagSetAttributesFlag(pE, pTE->lastTagTokenIndex);
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Attr_VarVal_CB(NW_XML_Reader_t* pT,
                                        const NW_XML_Reader_Interval_t* pI_name,
                                        const NW_XML_Reader_Interval_t* pI_value,
                                        void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_Uint32 length;
    NW_Uint32 nameLength;
    NW_Uint8* pName;
    NW_Uint32 valueByteLength;
    NW_Uint32 valueCharCount;
    NW_Uint8* pValue;
    NW_Status_t s;

    if (!NW_XML_Reader_Interval_IsWellFormed(pI_name)) {
        return NW_STAT_FAILURE;
    }

    /* var name setup */
    length = pI_name->stop - pI_name->start;
    nameLength = length; /* byte length */
    s = NW_XML_Reader_DataAddressFromBuffer(pT, pI_name->start,
                                         &nameLength, &pName);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (nameLength != length) {
        return NW_STAT_FAILURE;
    }
    nameLength = pI_name->charStop - pI_name->charStart; /* char count */

    /* value setup */
    length = pI_value->stop - pI_value->start;
    valueByteLength = length;
    s = NW_XML_Reader_DataAddressFromBuffer(pT, pI_value->start,
                                         &valueByteLength, &pValue);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (valueByteLength != length) {
        return NW_STAT_FAILURE;
    }
    valueCharCount = pI_value->charStop - pI_value->charStart;

    if (pT->encoding == HTTP_iso_10646_ucs_2) {
      NW_XML_XmlpWbxmlEncoder_Ucs2NativeToNetworkByteOrder(valueByteLength,
                                                           pValue);
    }
    s = NW_WBXML_Writer_AttributeAndValue(pE, pTE->encoding, nameLength,
                                          pName, valueCharCount,
                                          valueByteLength, pValue);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (pT->encoding == HTTP_iso_10646_ucs_2) {
      NW_XML_XmlpWbxmlEncoder_Ucs2NetworkToNativeByteOrder(valueByteLength,
                                                           pValue);
    }
    return NW_STAT_SUCCESS;
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Attributes_End_CB(NW_XML_Reader_t* pT,
                                          NW_Uint32 attributeCount, void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_Status_t s;
    NW_REQUIRED_PARAM(pT);
    /* if there were any attributes, then must write end token */
    if (attributeCount) {
        s = NW_WBXML_Writer_End(pE);
        if (NW_STAT_IS_FAILURE(s)) {
            return s;
        }
    }
    return NW_STAT_SUCCESS;
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_CoreTextHandler(NW_Uint32 encoding,
                                        NW_Uint32 charCount,
                                        NW_Uint32 byteLength,
                                        NW_Uint8* pText,
                                        NW_WBXML_Writer_t* pE)
{
    NW_Uint8* pNormalizedText;
    NW_Status_t s;

    s = NW_XML_XmlpWbxmlEncoder_EndOfLineNormalization(encoding,
                                                       &charCount,
                                                       &byteLength,
                                                       pText,
                                                       &pNormalizedText);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    if (encoding == HTTP_iso_10646_ucs_2) {
      NW_XML_XmlpWbxmlEncoder_Ucs2NativeToNetworkByteOrder(byteLength,
                                                           pNormalizedText);
    }
    s = NW_WBXML_Writer_Text(pE, encoding, byteLength, pNormalizedText);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    if (pText == pNormalizedText) {
      if (encoding == HTTP_iso_10646_ucs_2) {
        NW_XML_XmlpWbxmlEncoder_Ucs2NetworkToNativeByteOrder(byteLength,
                                                             pNormalizedText);
      }
    } else {
      NW_Mem_Free(pNormalizedText);
    }
    return NW_STAT_SUCCESS;
}


static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Content_CB(NW_XML_Reader_t* pT,
                                   const NW_XML_Reader_Interval_t* pI_content,
                                   void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_Status_t s;
    NW_Uint32 length;
    NW_Uint32 byteLength;
    NW_Uint32 charCount;
    NW_Uint8* pContent;
    NW_Uint8* pNormalizedContent;

    if (!NW_XML_Reader_Interval_IsWellFormed(pI_content)) {
        return NW_STAT_FAILURE;
    }
    length = pI_content->stop - pI_content->start;
    byteLength = length;
    s = NW_XML_Reader_DataAddressFromBuffer(pT, pI_content->start,
                                            &byteLength,
                                            &pContent);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (byteLength != length) {
        return NW_STAT_FAILURE;
    }
    charCount = pI_content->charStop - pI_content->charStart;
    s = NW_XML_XmlpWbxmlEncoder_EndOfLineNormalization(pT->encoding,
                                                       &charCount,
                                                       &byteLength,
                                                       pContent,
                                                       &pNormalizedContent);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (pT->encoding == HTTP_iso_10646_ucs_2) {
      NW_XML_XmlpWbxmlEncoder_Ucs2NativeToNetworkByteOrder(byteLength,
                                                           pNormalizedContent);
    }
    s = NW_WBXML_Writer_Text(pE, pT->encoding, byteLength, pNormalizedContent);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (pContent == pNormalizedContent) {
      if (pT->encoding == HTTP_iso_10646_ucs_2) {
        NW_XML_XmlpWbxmlEncoder_Ucs2NetworkToNativeByteOrder(byteLength,
                                                             pNormalizedContent);
      }
    } else {
      NW_Mem_Free(pNormalizedContent);
    }
    return NW_STAT_SUCCESS;
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Tag_End_CB(NW_XML_Reader_t* pT,
                                   const NW_XML_Reader_Interval_t* pI_name,
                                   NW_Uint32 emptyTagFlag, void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_Status_t s;
    NW_REQUIRED_PARAM(pT);
    NW_REQUIRED_PARAM(pI_name);
    if (emptyTagFlag) {
        s = NW_WBXML_Writer_TagClearContentFlag(pE, pTE->lastTagTokenIndex);
    } else {
        s = NW_WBXML_Writer_End(pE);
    }
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    return s;
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Cdata_CB(NW_XML_Reader_t* pT,
                                 const NW_XML_Reader_Interval_t* pI_cdata,
                                 void* pV)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_Uint8* pText;
    NW_Uint8* pQuotedText;
    NW_Uint8* pSrc;
    NW_Uint8* pDest;
    NW_Uint32 encoding = pT->encoding;
    NW_Uint32 length;
    NW_Uint32 byteLength;
    NW_Uint32 charCount;
    NW_Uint32 ampCount;
    NW_Uint32 i, j;
    NW_Ucs2 cUCS2;
    NW_Status_t s;

    /* code only supports the following charset values */
    NW_ASSERT((encoding == HTTP_us_ascii)
              || (encoding == HTTP_iso_8859_1)
              || (encoding == HTTP_utf_8)
              || (encoding == HTTP_iso_10646_ucs_2));
    
    charCount = pI_cdata->charStop - pI_cdata->charStart;
    length = pI_cdata->stop - pI_cdata->start;
    if (length < 1) {
      return NW_STAT_SUCCESS;
    }
    byteLength = length;
    s = NW_XML_Reader_DataAddressFromBuffer(pT, pI_cdata->start,
                                            &byteLength,
                                            &pText);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
    if (byteLength != length) {
      return NW_STAT_FAILURE;
    }

    /* Cdata is not supposed to have any entity expansion done on it.
       Since we don't alter the original document text in general, we
       have done the entity expansion outside of cXML.  Therefore, we
       must quote any ampersands in the Cdata to prevent later entity
       expansion. */

    /* count ampersands */
    pSrc = pText;
    ampCount = 0;
    for (i = 0; i < charCount; i++) {
      j = NW_String_readChar(pSrc, &cUCS2, pT->encoding);
      pSrc += j;
      /* don't need to check j < 1, text has already been validated */
      if (cUCS2 == '&') {
        ampCount++;
      }
    }

    pQuotedText = pText;
    if (ampCount) {
      /* allocate a new buffer

      need incremental length for: & => &amp;
      start with single byte charset value */
      ampCount *= 4;
      /* double for ucs2 */
      if (pT->encoding == HTTP_iso_10646_ucs_2) {
        ampCount *= 2;
      }
      pQuotedText = (NW_Uint8*)NW_Mem_Malloc(byteLength + ampCount);
      if (pQuotedText == NULL) {
        return NW_STAT_OUT_OF_MEMORY;
      }

      /* copy and quote ampersands */
      pSrc = pText;
      pDest = pQuotedText;
      for (i = 0; i < charCount; i++) {
        j = NW_String_readChar(pSrc, &cUCS2, encoding);
        /* don't need to check for byteCount < 1, text has already been
           validated */
        NW_Mem_memcpy(pDest, pSrc, j);
        pSrc += j;
        pDest += j;
        if (cUCS2 == '&') {
          if (encoding == HTTP_iso_10646_ucs_2) {
            /* double byte */
            *((NW_Uint16*)pDest) = 'a';
            *((NW_Uint16*)(pDest + 2)) = 'm';
            *((NW_Uint16*)(pDest + 4)) = 'p';
            *((NW_Uint16*)(pDest + 6)) = ';';
            pDest += 8;
          } else {
            /* single byte encodings */
            NW_Mem_memcpy(pDest, "amp;", 4);
            pDest += 4;
          }
        }
      }
      byteLength += ampCount;
      if (pT->encoding == HTTP_iso_10646_ucs_2) {
        charCount += (ampCount / 2);
      } else {
        charCount = byteLength;
      }
    }

    /* output the text */
    s = NW_XML_XmlpWbxmlEncoder_CoreTextHandler(pT->encoding,
                                                charCount,
                                                byteLength,
                                                pQuotedText,
                                                pE);
    if (pQuotedText != pText) {
      NW_Mem_Free(pQuotedText);
    }
    return s;
}

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_EndDocument_CB(NW_XML_Reader_t* pT, void* pV)
{
    /* When debugging, this func provides a place to capture the final WBXML. */
    NW_REQUIRED_PARAM(pT);
    NW_REQUIRED_PARAM(pV);
    return NW_STAT_SUCCESS;
}


/* Function to check the string table of encoder process. If the string table is
 * found and supported then keep a copy of this so that WBXML parser can populate 
 * its string table from this.
 */
static 
void NW_XML_StrTbl(struct NW_XML_Parser_EventCallbacks_s* eventCBs,
                                                           void** WBXMLEncStrTbl)
{
  //Keep the copy of the WBXML string table, so that it can be added to the
  //DOM later. 
   if( (NW_Bool) (CXML_Additional_Feature_Supprted() & CXML_DTD_SUPPORT_ON) ) 
   {
     NW_Uint32 stringTableByteCount = 0;
     NW_Encoder_StringTable_t* strTable = (NW_Encoder_StringTable_t*)
           ((NW_XML_XmlpWbxmlEncoder_t*)eventCBs->pClientPointer)->pE->pStringTableObject;
     if(strTable)
     {
       stringTableByteCount = NW_Encoder_StringTable_getTotalBytes(strTable);
       if( (stringTableByteCount == 0) || (WBXMLEncStrTbl == NULL) )
       {
        //Empty Table
        NW_Encoder_StringTable_delete(strTable);
        if(WBXMLEncStrTbl)
        {
         *WBXMLEncStrTbl = NULL;
        }
       }
       else
       {
        if(WBXMLEncStrTbl)
        {
        *WBXMLEncStrTbl = (void*) strTable;
        }
       }//end else
    }//end if(strTable)
     else
     {
      if(WBXMLEncStrTbl)
        {
         *WBXMLEncStrTbl = NULL;
        }
     }
   }//end if((NW_Bool) CXML_Additional_Feature_Supprted() ) 
   else
   {
    if(WBXMLEncStrTbl)
    {
     *WBXMLEncStrTbl = NULL;
    }
   }
 return;
}/*NW_XML_StrTbl()*/


//Write the entity content now. There are two possibilities for
//entities.
//
// 1) If it is character or decimal or predefined entities. 
//    In this case, intEntityValStr = NULL.
//
// 2) If it is "Internal Entity" then intEntityValStr != NULL.
//     So, this is directly written to WBXML encoder as inline string.
//    In this case, entityVal = 0;

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Entity_CB(NW_XML_Reader_t* pT,NW_Uint32 numEntity,
                                   void* pV, CXML_Uint8* intEntityValStr)
{
    NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pW = pTE->pE;
    NW_Status_t s = NW_STAT_FAILURE;

    NW_REQUIRED_PARAM(pT);
    NW_REQUIRED_PARAM(pV);

    if(intEntityValStr == NULL)
    {
	 s = NW_WBXML_Writer_Entity(pW,numEntity);
    }
    else if( (intEntityValStr != NULL) && (numEntity == 0) )
    {
     NW_Uint32 valBufByteCnt = 0;
     NW_Int32 valBufLen = NW_String_charBuffGetLength( (void*)intEntityValStr,
                                                 pT->encoding,
                                                 &valBufByteCnt);

      /* Write internal entity as Inline string */

      s = NW_WBXML_Writer_Text( pW, pT->encoding, valBufByteCnt, intEntityValStr);
     
    }/*end else if*/

    return s;
}/*end NW_XML_XmlpWbxmlEncoder_Entity_CB() */

/* This is used for if Entity is there in the Attribute value */

static
NW_Status_t
NW_XML_XmlpWbxmlEncoder_Entity_Attr_VarVal_CB(NW_XML_Reader_t* pT,
                                        const NW_XML_Reader_Interval_t* pI_name,
                                        NW_Uint8* pValue,
										NW_Uint32 valueByteLength,
                                        void* pV)
{
NW_XML_XmlpWbxmlEncoder_t* pTE = (NW_XML_XmlpWbxmlEncoder_t*)pV;
    NW_WBXML_Writer_t* pE = pTE->pE;
    NW_Uint32 length;
    NW_Uint32 nameLength;
	NW_Uint32 valueCharCount;
    NW_Uint8* pName;
    NW_Status_t s;

    if (!NW_XML_Reader_Interval_IsWellFormed(pI_name)) {
        return NW_STAT_FAILURE;
    }

    /* var name setup */
    length = pI_name->stop - pI_name->start;
    nameLength = length; /* byte length */
    s = NW_XML_Reader_DataAddressFromBuffer(pT, pI_name->start,
                                         &nameLength, &pName);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (nameLength != length) {
        return NW_STAT_FAILURE;
    }
    nameLength = pI_name->charStop - pI_name->charStart; /* char count */

    /* value setup */
    valueCharCount = valueByteLength;
    

    if (pT->encoding == HTTP_iso_10646_ucs_2) {
      NW_XML_XmlpWbxmlEncoder_Ucs2NativeToNetworkByteOrder(valueByteLength,
                                                           pValue);
    }
    s = NW_WBXML_Writer_AttributeAndValue(pE, pTE->encoding, nameLength,
                                          pName, valueCharCount,
                                          valueByteLength, pValue);
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }
    if (pT->encoding == HTTP_iso_10646_ucs_2) {
      NW_XML_XmlpWbxmlEncoder_Ucs2NetworkToNativeByteOrder(valueByteLength,
                                                           pValue);
    }
    return NW_STAT_SUCCESS;
}/*end NW_XML_XmlpWbxmlEncoder_Entity_Attr_VarVal_CB(..)*/





NW_Status_t
NW_XML_XmlToWbxml(NW_Buffer_t* pInBuf, NW_Uint32 encoding, 
                  NW_Buffer_t** ppOutBuf, NW_Uint32* line, 
                  NW_Uint32 publicID, void** WBXMLEncStrTbl)
{
    NW_Status_t s;
    NW_XML_Reader_t t;
    NW_Uint8* pWbxmlBuf;
    NW_Uint8* docStartAddress;
    NW_Uint32 docByteLength;
    NW_Uint32 wbxmlBufLength;
    struct NW_XML_Parser_EventCallbacks_s wbxmlEncodeCallbacks;
    NW_XML_XmlpWbxmlEncoder_t* ppTE = NULL;
    NW_WBXML_Writer_t* pW = NULL;

    NW_ASSERT( pInBuf != NULL );
    NW_ASSERT( ppOutBuf != NULL );
    NW_ASSERT( line != NULL );

    *line = 0;
    if (pInBuf->length == 0 || pInBuf->data == NULL) {
      return NW_STAT_FAILURE;
    }
    /* For UTF-8 and UCS-2 handle BOM (byte order mark).  Also, for
       UCS-2 handle network (big) to native byte order conversion. */
    docByteLength = pInBuf->length;
    docStartAddress = pInBuf->data;
    if (encoding == HTTP_utf_8) {
      /* look for BOM and remove if found */
      if (docByteLength >= 3) {
        if ((docStartAddress[0] == 0xef)
            && (docStartAddress[1] == 0xbb)
            && (docStartAddress[2] == 0xbf)) {
          docByteLength -= 3;
          docStartAddress += 3;
        }
      }
    }
    if (encoding == HTTP_iso_10646_ucs_2) {
      /* WARNING: we are assuming network byte order (i.e., big) for
         the input document */
      /* verify the doc has an even number of bytes, check LSB != 1 */
      if ((docByteLength & 1) == 1) {
        return NW_STAT_FAILURE;
      }

      /* make a pass over the doc, forcing byte order to native byte order */
      NW_XML_XmlpWbxmlEncoder_Ucs2NetworkToNativeByteOrder(docByteLength,
                                                           docStartAddress);
      /* now look for BOM and remove if found */
      if (docByteLength >= 2) {
        if (((docStartAddress[0] == 0xfe) && (docStartAddress[1] == 0xff))
            || ((docStartAddress[0] == 0xff) && (docStartAddress[1] == 0xfe))) {
          docByteLength -= 2;
          docStartAddress += 2;
        }
      }
    }
    /* verify there is some doc remaining after BOM processing */
    if (docByteLength == 0) {
      return NW_STAT_FAILURE;
    }

    /* malloc the output buffer */
    *ppOutBuf = (NW_Buffer_t*) NW_Mem_Malloc(sizeof(NW_Buffer_t));
    if (!*ppOutBuf) {
        return NW_STAT_OUT_OF_MEMORY;
    }
    (*ppOutBuf)->length = 0;
    (*ppOutBuf)->allocatedLength = 0;
    (*ppOutBuf)->data = NULL;

    /* Null out all WBXML generation callback addresses. */
    NW_Mem_memset(&wbxmlEncodeCallbacks, 0,
                  sizeof(struct NW_XML_Parser_EventCallbacks_s));
    /* Add in-use WBXML generation callbacks. */
    wbxmlEncodeCallbacks.StartDocument_CB = NW_XML_XmlpWbxmlEncoder_StartDocument_CB;
    wbxmlEncodeCallbacks.Tag_Start_CB = NW_XML_XmlpWbxmlEncoder_Tag_Start_CB;
    wbxmlEncodeCallbacks.Attr_Start_CB = NW_XML_XmlpWbxmlEncoder_Attr_Start_CB;
    wbxmlEncodeCallbacks.Attr_VarVal_CB = NW_XML_XmlpWbxmlEncoder_Attr_VarVal_CB;
    wbxmlEncodeCallbacks.Attributes_End_CB = NW_XML_XmlpWbxmlEncoder_Attributes_End_CB;
    wbxmlEncodeCallbacks.Tag_End_CB = NW_XML_XmlpWbxmlEncoder_Tag_End_CB;
    wbxmlEncodeCallbacks.Content_CB = NW_XML_XmlpWbxmlEncoder_Content_CB;
    wbxmlEncodeCallbacks.Cdata_CB = NW_XML_XmlpWbxmlEncoder_Cdata_CB;
    wbxmlEncodeCallbacks.EndDocument_CB = NW_XML_XmlpWbxmlEncoder_EndDocument_CB;

    wbxmlEncodeCallbacks.Entity_CB = NW_XML_XmlpWbxmlEncoder_Entity_CB;
    wbxmlEncodeCallbacks.Attr_Entity_VarVal_CB = NW_XML_XmlpWbxmlEncoder_Entity_Attr_VarVal_CB;


    /* The encoder contains a WBXML_Writer that defaults to a sizing pass. */
    s = NW_XML_XmlpWbxmlEncoder_New(publicID, encoding,
                                    &(wbxmlEncodeCallbacks.pClientPointer));
    if (NW_STAT_IS_FAILURE(s)) {
        return s;
    }

    ppTE = (NW_XML_XmlpWbxmlEncoder_t*)wbxmlEncodeCallbacks.pClientPointer;
    pW   = (NW_WBXML_Writer_t*) ppTE->pE;

    /* Hand the document text to the Reader in preparation for parsing. */
    s = NW_XML_Reader_InitFromBuffer(&t, docByteLength, docStartAddress);
    if (NW_STAT_IS_FAILURE(s)) {
        NW_XML_XmlpWbxmlEncoder_Delete(wbxmlEncodeCallbacks.pClientPointer);
        if(pW->pStringTableObject)
        {
         NW_Encoder_StringTable_delete(pW->pStringTableObject);
        }
        return s;
    }
    NW_XML_Reader_SetEncoding(&t, encoding);

    /* Parse and do a sizing pass for the WBXML. */
    s = NW_XML_Parse(&t, &wbxmlEncodeCallbacks);
    if (NW_STAT_IS_FAILURE(s)) {
        NW_Uint32 column;
        NW_XML_Reader_GetLineColumn(&t, line, &column);
        if(pW->pStringTableObject)
        {
         NW_Encoder_StringTable_delete(pW->pStringTableObject);
        }
        NW_XML_XmlpWbxmlEncoder_Delete(wbxmlEncodeCallbacks.pClientPointer);
        return s;
    }

    /* If there is any string table data then free it after PASS-1 as
     * PASS-2 will again store the data in this. The PASS-1 only calulate
     * size of the WBXML buffer.
     */

    NW_Encoder_StringTable_delete((NW_Encoder_StringTable_t*)
           ((NW_XML_XmlpWbxmlEncoder_t*)wbxmlEncodeCallbacks.pClientPointer)->pE->pStringTableObject);


    /* Allocate buffer and reinitialize writer to write to buf. */
    wbxmlBufLength = (((NW_XML_XmlpWbxmlEncoder_t*)
                       wbxmlEncodeCallbacks.pClientPointer)->pE->index);
    pWbxmlBuf = (NW_Uint8*) NW_Mem_Malloc(wbxmlBufLength);
    if (!pWbxmlBuf) {
        if(pW->pStringTableObject)
        {
         NW_Encoder_StringTable_delete(pW->pStringTableObject);
        }
        NW_XML_XmlpWbxmlEncoder_Delete(wbxmlEncodeCallbacks.pClientPointer);
        return NW_STAT_OUT_OF_MEMORY;
    }
    NW_XML_XmlpWbxmlEncoder_SetToWrite((NW_XML_XmlpWbxmlEncoder_t*)
                                       wbxmlEncodeCallbacks.pClientPointer,
                                       wbxmlBufLength, pWbxmlBuf,
                                       NULL /* no growth callback */);
    /* Reregister the document text with the Reader in preparation for parsing. */
    s = NW_XML_Reader_InitFromBuffer(&t, docByteLength, docStartAddress);
    if (NW_STAT_IS_FAILURE(s)) {
        if(pW->pStringTableObject)
        {
         NW_Encoder_StringTable_delete(pW->pStringTableObject);
        }
        NW_XML_XmlpWbxmlEncoder_Delete(wbxmlEncodeCallbacks.pClientPointer);
        return s;
    }
    NW_XML_Reader_SetEncoding(&t, encoding);

    /* Parse and generate WBXML. */
    s = NW_XML_Parse(&t, &wbxmlEncodeCallbacks);
    if (NW_STAT_IS_FAILURE(s)) {
        NW_Uint32 column;
        NW_XML_Reader_GetLineColumn(&t, line, &column);
        if(pW->pStringTableObject)
        {
         NW_Encoder_StringTable_delete(pW->pStringTableObject);
        }
        NW_XML_XmlpWbxmlEncoder_Delete(wbxmlEncodeCallbacks.pClientPointer);
        return s;
    }

    /* The string table  support check and initialize parameter accordingly. */
     

    NW_XML_StrTbl(&wbxmlEncodeCallbacks,WBXMLEncStrTbl);

    NW_XML_XmlpWbxmlEncoder_Delete(wbxmlEncodeCallbacks.pClientPointer);
    (*ppOutBuf)->length = wbxmlBufLength;
    (*ppOutBuf)->allocatedLength = wbxmlBufLength;
    (*ppOutBuf)->data = pWbxmlBuf;
    return NW_STAT_SUCCESS;
}


