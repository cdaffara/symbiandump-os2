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
**  File: tinydom2wbxml.c
**
**  Description: maps TinyDom types onto WBXMLWriter functions
*****************************************************************/

#include "cxml_internal.h"
#include "nw_encoder_tinydom2wbxml.h"
#include <xml/cxml/nw_dom_text.h>

/*
Writes an AttrVal
Return NW_STAT_BAD_INPUT_PARAM
       NW_STAT_OUT_OF_MEMORY
       NW_STAT_SUCCESS
*/
NW_Status_t
NW_Encoder_writeAttrVal(NW_WBXML_Writer_t* pW, NW_DOM_AttrVal_t *val, NW_Uint32 encoding)
{
  if ((val == NULL) || (pW == NULL)) {
    return NW_STAT_BAD_INPUT_PARAM;
  }

  switch (NW_DOM_AttrVal_getType(val)) {
  case NW_DOM_ATTR_VAL_STRING:
    {
      NW_String_t *string = &(val->component.string);
      return NW_WBXML_Writer_Text(pW, encoding,
                                  string->length, string->storage);
    }
  case NW_DOM_ATTR_VAL_EXTENSION:
    {
      NW_Uint8 t; /* 8-bit token */
      NW_String_t str;
      NW_Status_t status;

      t = (NW_Uint8)(val->component.ext.token);
      if ((t == NW_WBXML_EXT_0)
          || (t == NW_WBXML_EXT_1)
          || (t == NW_WBXML_EXT_2)) {
        status
          = NW_WBXML_Writer_Extension(pW,
                                      (NW_Uint16)(val->component.ext.token),
                                      0, 0, NULL);
      } else if ((t == NW_WBXML_EXT_T_0)
                 || (t == NW_WBXML_EXT_T_1)
                 || (t == NW_WBXML_EXT_T_2)) {
        NW_ASSERT((val->component.ext.type
                   == NW_TINYDOM_EXTENSION_TYPE_NORMAL)
                  || (val->component.ext.type
                     == NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER));
        if (val->component.ext.type == NW_TINYDOM_EXTENSION_TYPE_NORMAL) {
          NW_Uint16 token = NW_DOM_TextItem_getExtension(val, &str);
          status
            = NW_WBXML_Writer_ExtensionUseStringTable(pW,
                                                      token,
                                                      str.length,
                                                      str.storage);
        } else {
          status = NW_WBXML_Writer_Extension(pW,
                                             (NW_Uint16)(val->component.ext.token),
                                             val->component.ext.value.x,
                                             0, NULL);
        }
      } else if ((t == NW_WBXML_EXT_I_0)
                 || (t == NW_WBXML_EXT_I_1)
                 || (t == NW_WBXML_EXT_I_2)) {
        NW_Uint16 token = NW_DOM_TextItem_getExtension(val, &str);
        status = NW_WBXML_Writer_Extension(pW, token,
                                           0, str.length, str.storage);
      } else {
        status = NW_STAT_FAILURE;
      }
      return status;
    }
  case NW_DOM_ATTR_VAL_ENTITY:
    {
      NW_Uint32 entity = NW_DOM_TextItem_getEntity(val);
      return NW_WBXML_Writer_Entity(pW, entity);
    }
  case NW_DOM_ATTR_VAL_OPAQUE:
    {
      NW_Uint32 length = 0;
      NW_Byte* data = NW_DOM_AttrVal_getOpaque(val, &length);
      return NW_WBXML_Writer_Opaque(pW, length, data);
    }
  case NW_DOM_ATTR_VAL_TOKEN:
    {
      NW_Uint16 fqToken = NW_DOM_AttrVal_getToken(val);
      return NW_WBXML_Writer_AttributeToken(pW, fqToken);
    }
  default:
      break;
  }

  return NW_STAT_FAILURE;
}

/*
Writes an attrFqToken with AttrVal
Return NW_STAT_BAD_INPUT_PARAM
       NW_STAT_OUT_OF_MEMORY
       NW_STAT_SUCCESS
       NW_STAT_FAILURE
*/
NW_Status_t
NW_Encoder_writeAttributeByToken(NW_WBXML_Writer_t* pW,
                                 NW_Uint16 attrFqToken,
                                 NW_TinyDom_AttrVal_t *val,
                                 NW_Uint32 encoding)
{
  NW_Status_t s;
  NW_Uint8 token = (NW_Uint8)(attrFqToken & NW_WBXML_MASK_TOKEN);

  if ((token == NW_WBXML_LITERAL) || (val == NULL) || (pW == NULL)) {
    return NW_STAT_BAD_INPUT_PARAM;
  }

  s = NW_WBXML_Writer_AttributeToken(pW, attrFqToken);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }

  /* TODO: Talk to Shaun about it */
  if ((NW_DOM_AttrVal_getType(val) == NW_DOM_ATTR_VAL_TOKEN) &&
      (attrFqToken == val->component.value_token))
  {}
  else{
    s = NW_Encoder_writeAttrVal(pW, val, encoding);
    if (NW_STAT_IS_FAILURE(s)) {
      return s;
    }
  }

  return NW_WBXML_Writer_End(pW);
}

/*
Writes an Attribute by name plus AttrVal
Return NW_STAT_BAD_INPUT_PARAM
       NW_STAT_OUT_OF_MEMORY
       NW_STAT_SUCCESS
       NW_STAT_FAILURE
*/
NW_Status_t
NW_Encoder_writeAttributeByName(NW_WBXML_Writer_t* pW,
                                NW_String_t *attrName,
                                NW_TinyDom_AttrVal_t *val,
                                NW_Uint32 encoding)
{
  NW_Status_t s;
  NW_Uint32 byteCount;
  NW_Int32 charCount;

  if ((pW == NULL) || (attrName == NULL) || (val == NULL)) {
    return NW_STAT_BAD_INPUT_PARAM;
  }

  charCount = NW_String_charBuffGetLength(attrName->storage,
                                          encoding,
                                          &byteCount);
  if (charCount < 0) {
    return NW_STAT_BAD_INPUT_PARAM;
  }
  NW_ASSERT(byteCount == attrName->length);
  s = NW_WBXML_Writer_AttributeNameString(pW, encoding, (NW_Uint32)charCount,
                                          byteCount, attrName->storage);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }

  s = NW_Encoder_writeAttrVal(pW, val, encoding);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }

  return NW_WBXML_Writer_End(pW);
}

/*
Writes an Element by token
Return NW_STAT_BAD_INPUT_PARAM
       NW_STAT_OUT_OF_MEMORY
       NW_STAT_SUCCESS
*/
NW_Status_t
NW_Encoder_writeElementByToken(NW_WBXML_Writer_t* pW,
                               NW_Uint16 elementToken)
{
  NW_Status_t s;
  NW_Uint32 ignoreIndex;

  if (pW == NULL) {
    return NW_STAT_BAD_INPUT_PARAM;
  }
  s = NW_WBXML_Writer_TagToken(pW, elementToken, &ignoreIndex);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }

  return NW_WBXML_Writer_End(pW);
}

/*
Writes an Element
Return NW_STAT_BAD_INPUT_PARAM
       NW_STAT_OUT_OF_MEMORY
       NW_STAT_SUCCESS
*/
NW_Status_t
NW_Encoder_writeElementByName(NW_WBXML_Writer_t* pW,
                              NW_String_t *elementName,
                              NW_Uint32 encoding)
{
  NW_Status_t s;
  NW_Uint32 ignoreIndex;
  NW_Uint32 byteCount;
  NW_Int32 charCount;

  if ((elementName == NULL) || (pW == NULL)) {
    return NW_STAT_BAD_INPUT_PARAM;
  }

  charCount = NW_String_charBuffGetLength(elementName->storage,
                                          encoding,
                                          &byteCount);
  if (charCount < 0) {
    return NW_STAT_BAD_INPUT_PARAM;
  }
  NW_ASSERT(byteCount == elementName->length);
  s = NW_WBXML_Writer_TagString(pW, encoding, (NW_Uint32)charCount, byteCount,
                                elementName->storage, &ignoreIndex);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }

  return NW_WBXML_Writer_End(pW);
}

/*
Writes a TextItem
Return NW_STAT_BAD_INPUT_PARAM
       NW_STAT_OUT_OF_MEMORY
       NW_STAT_SUCCESS
*/
NW_Status_t
NW_Encoder_writeText(NW_WBXML_Writer_t* pW,
                     NW_TinyDom_Text_t *text,
                     NW_Uint32 encoding)
{
  NW_Status_t s;
  s = NW_Encoder_writeAttrVal(pW, text, encoding);
  if (NW_STAT_IS_FAILURE(s)) {
    return s;
  }

  return NW_WBXML_Writer_End(pW);
}


