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
#include <xml/cxml/nw_dom_node.h>
#include "nw_dom_wbxmltypes.h"

/** Extension **/

/*
 * Creates and inits new extension with a given token and string
 */

/* TODO: this is unlike other new methods which do not init
   Ask Deepika about this???? */


NW_DOM_Extension_t *
NW_DOM_Extension_new(NW_Uint32 token,
                     NW_String_t *str)
{
  NW_DOM_Extension_t *ext =
    (NW_DOM_Extension_t *)NW_Mem_Malloc(sizeof(NW_DOM_Extension_t));

  if (ext == NULL){
    return NULL;
  }

  NW_DOM_Extension_initialize(ext, token, str);
  return ext;
}

/*
 * Initializes an extension with a token and string
 * Returns NW_STAT_SUCCESS or NW_STAT_FAILURE
 */

NW_Status_t
NW_DOM_Extension_initialize(NW_DOM_Extension_t *ext,
                            NW_Uint32 token,
                            NW_String_t *str)
{
  NW_Status_t s = NW_STAT_FAILURE;

  if (ext) {
  ext->token = token;
  ext->value.string = *str;
    s = NW_STAT_SUCCESS;
}
  return s;
}

/*
 * Returns token of extension
 */

NW_Uint32
NW_DOM_Extension_getToken(NW_DOM_Extension_t *e)
{

  NW_ASSERT(e != NULL);

  return e->token;
}

/** Opaque **/

/**
 * Creates a new Opaque type with a given data and length
 */

/* TODO: Why does this also init ???? */

NW_DOM_Opaque_t *
NW_DOM_Opaque_new(NW_Uint32 length, NW_Byte* data)
{
  NW_DOM_Opaque_t *opaque =
    (NW_DOM_Opaque_t *) NW_Mem_Malloc(sizeof (NW_DOM_Opaque_t));

  if (opaque == NULL){
    return NULL;
  }

  NW_DOM_Opaque_initialize(opaque, length, data);

  return opaque;
}

/*
 * Initializes Opaque data type
 * Returns NW_DOMSTAT_BAD_INPUT_PARAM
 *         NW_DOMSTAT_SUCCESS
 */
NW_Status_t
NW_DOM_Opaque_initialize(NW_DOM_Opaque_t *opaque, NW_Uint32 length, NW_Byte* data)
{
  NW_Status_t s = NW_STAT_FAILURE;

  if (opaque) {
  opaque->data = data;
  opaque->length = length;
  s = NW_STAT_SUCCESS;
  }
  return s;
}

NW_Status_t
NW_DOM_Opaque_delete(NW_DOM_Opaque_t *opaque)
{
  NW_ASSERT(opaque != NULL);

  if (NW_DOM_Opaque_getUserOwnsData(opaque)){
    if (opaque->data != NULL){
      NW_Mem_Free (opaque->data);
    }
  }

  NW_Mem_Free(opaque);
  return NW_STAT_SUCCESS;
}


/*
combined WBXML attribute value and text components
 */


NW_Status_t
NW_DOM_WbxmlComponent_setType(NW_WbxmlComponent_t AttributeValueOrText,
                              NW_TinyDom_AttrVal_t* pV,
                              NW_Uint32 type)
{
  NW_Status_t s = NW_STAT_BAD_INPUT_PARAM;

  if (pV
      && (type < NW_WBXML_ATTR_COMPONENT_INVALID)
      && !((AttributeValueOrText == NW_WBXML_TEXT_COMPONENT)
           && (type == NW_WBXML_ATTR_COMPONENT_TOKEN))) {
    pV->type = type;
    s = NW_STAT_SUCCESS;
  }
  return s;
}

NW_Status_t
NW_DOM_WbxmlComponent_initFromString(NW_TinyDom_AttrVal_t* pV,
                                     NW_String_t * pString)
{
  NW_Status_t s = NW_STAT_BAD_INPUT_PARAM;

  if (pV
      && pString) {
    pV->type = NW_WBXML_ATTR_COMPONENT_STRING;
    pV->component.string = *pString;
    s = NW_STAT_SUCCESS;
  }
  return s;
}

NW_Status_t
NW_DOM_WbxmlComponent_initFromEntity(NW_TinyDom_AttrVal_t* pV,
                                     NW_Uint32 entity)
{
  NW_Status_t s = NW_STAT_BAD_INPUT_PARAM;

  if (pV) {
    pV->type = NW_WBXML_ATTR_COMPONENT_ENTITY;
    pV->component.entity = entity;
    s = NW_STAT_SUCCESS;
  }
  return s;
}

/*
 Initializes a WBXML component of type NW_WBXML_ATTR_COMPONENT_EXT
 Returns NW_STAT_SUCCESS or NW_STAT_FAILURE if args are not valid.

 Makes a shallow copy of str.
 */
NW_Status_t
NW_DOM_WbxmlComponent_initFromExtension(NW_TinyDom_AttrVal_t* val,
                                        NW_Uint16 token,
                                        NW_String_t* str)
{
  NW_Uint8 t; /* 8-bit token */

  NW_ASSERT(val != NULL);
  NW_ASSERT(token != 0);

  val->type = NW_WBXML_ATTR_COMPONENT_INVALID;
  t = (NW_Uint8)token;
  if ((t == NW_WBXML_EXT_0)
      || (t == NW_WBXML_EXT_1)
      || (t == NW_WBXML_EXT_2)
      || (t == NW_WBXML_EXT_T_0)
      || (t == NW_WBXML_EXT_T_1)
      || (t == NW_WBXML_EXT_T_2)
      || (t == NW_WBXML_EXT_I_0)
      || (t == NW_WBXML_EXT_I_1)
      || (t == NW_WBXML_EXT_I_2)) {
    if ((t != NW_WBXML_EXT_0)
        && (t != NW_WBXML_EXT_1)
        && (t != NW_WBXML_EXT_2)
        && (str->storage == NULL)) {
      return NW_STAT_FAILURE;
    }
    val->type = NW_WBXML_ATTR_COMPONENT_EXT;
    val->component.ext.type = NW_TINYDOM_EXTENSION_TYPE_NORMAL;
    val->component.ext.token = token;
    if ((t == NW_WBXML_EXT_0)
        || (t == NW_WBXML_EXT_1)
        || (t == NW_WBXML_EXT_2)) {
      NW_String_initialize(&(val->component.ext.value.string), NULL, 0);
    } else {
      /* struct assignment, shallow copy */
      val->component.ext.value.string = *str;
    }
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_FAILURE;
}

NW_Status_t
NW_DOM_WbxmlComponent_initFromExtensionInt(NW_TinyDom_AttrVal_t* val,
                                           NW_Uint16 token,
                                           NW_Uint32 x)
{
  NW_Uint8 t = (NW_Uint8)token;

  NW_ASSERT(val != NULL);

  if ((t == NW_WBXML_EXT_T_0)
      || (t == NW_WBXML_EXT_T_1)
      || (t == NW_WBXML_EXT_T_2)) {
    val->type = NW_WBXML_ATTR_COMPONENT_EXT;
    val->component.ext.type = NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER;
    val->component.ext.token = token;
    val->component.ext.value.x = x;
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_FAILURE;
}

NW_Status_t
NW_DOM_WbxmlComponent_initFromOpaque(NW_TinyDom_AttrVal_t* pV,
                                     NW_Uint32 byteCount,
                                     NW_Uint8* pData)
{
  NW_Status_t s = NW_STAT_BAD_INPUT_PARAM;

  if (pV) {
    pV->type = NW_WBXML_ATTR_COMPONENT_OPAQUE;
    pV->component.opaque.length = byteCount;
    pV->component.opaque.data = pData;
    s = NW_STAT_SUCCESS;
  }
  return s;
}

NW_Status_t
NW_DOM_WbxmlComponent_initFromToken(NW_TinyDom_AttrVal_t* pV,
                                    NW_Uint32 token)
{
  NW_Status_t s = NW_STAT_BAD_INPUT_PARAM;

  if (pV) {
    pV->type = NW_WBXML_ATTR_COMPONENT_TOKEN;
    pV->component.value_token = token;
    s = NW_STAT_SUCCESS;
  }
  return s;
}

NW_Uint32
NW_DOM_WbxmlComponent_getEntity(NW_TinyDom_AttrVal_t* pV)
{
  NW_Uint32 x = 0;
  if (pV) {
    x = pV->component.entity;
  }
  return x;
}

NW_Status_t
NW_DOM_WbxmlComponent_getString(NW_TinyDom_AttrVal_t* pV,
                                NW_String_t* pString)
{
  NW_Status_t s = NW_STAT_BAD_INPUT_PARAM;

  if (pV
      && (pV->type == NW_WBXML_ATTR_COMPONENT_STRING)) {
    s = NW_String_copy(pString, &(pV->component.string));
  }
  return s;
}


NW_Status_t
NW_DOM_WbxmlComponent_getExtensionToken(NW_TinyDom_AttrVal_t* pV,
                                        NW_Uint16* pToken)
{
  NW_Status_t s = NW_STAT_BAD_INPUT_PARAM;

  if (pV
      && pToken
      && (pV->type == NW_WBXML_ATTR_COMPONENT_EXT)) {
    *pToken = (NW_Uint16)(pV->component.ext.token);
    s = NW_STAT_SUCCESS;
  }
  return s;
}

/* If component is of type NW_WBXML_ATTR_COMPONENT_EXT, and extension
 isn't EXT_T_[0,1,2] in non string table reference form, then returns token.
 Otherwise, returns 0 and returns str->length = 0, str->storage = NULL.

 All returned strings are shallow copies and the only proper way to
 free these is to call NW_String_delete(pStr). */
NW_Uint16
NW_DOM_WbxmlComponent_getExtension(NW_TinyDom_AttrVal_t* val,
                                   NW_String_t* str)
{
  NW_Uint16 fqToken;
  NW_Uint8 t; /* 8-bit token */

  NW_ASSERT (val != NULL);
  NW_ASSERT (str != NULL);

  NW_String_initialize(str, NULL, 0);

  if (val->type != NW_WBXML_ATTR_COMPONENT_EXT) {
    return 0;
  }

  /* TBD If it could be gauranteed that the extension was constructed
     correctly then it would be enough to just unconditionally
     shallowCopy. */

  fqToken = (NW_Uint16)(val->component.ext.token);
  t = (NW_Uint8)fqToken;

  if (((t == NW_WBXML_EXT_T_0)
       || (t == NW_WBXML_EXT_T_1)
       || (t == NW_WBXML_EXT_T_2))
      && (val->component.ext.type
          == NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER)) {
    /* This function is not for this case: use "getExtensionInt" form. */
    return 0;
  }

  if ((t != NW_WBXML_EXT_0) && (t != NW_WBXML_EXT_1) && (t != NW_WBXML_EXT_2)) {
    /* By fiat we do nothing with the single byte extension tokens.  A
     more correct version might be to callback to the client to
     request a mapping to a client determined string.

     In the normal case where EXT_T refers to the string table, both
     the EXT_T and EXT_I forms are represented by an explicit pointer
     to the string value since for the EXT_T form we don't have a
     pointer to the string table anyway. */
    NW_String_shallowCopy(str, &val->component.ext.value.string);
  }
  return fqToken;
    }

NW_Status_t
NW_DOM_WbxmlComponent_getExtensionInt(NW_TinyDom_AttrVal_t* val,
                                      NW_Uint32* x)
{
  NW_Uint16 fqToken;
  NW_Uint8 t; /* 8-bit token */

  NW_ASSERT(val != NULL);
  NW_ASSERT(x != NULL);

  if ((val->type != NW_WBXML_ATTR_COMPONENT_EXT)
      || ((val->component.ext.type
           != NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER)))
  {
    return NW_STAT_FAILURE;
  }

  fqToken = (NW_Uint16)(val->component.ext.token);
  t = (NW_Uint8)fqToken;
  if ((t == NW_WBXML_EXT_T_0)
      || (t == NW_WBXML_EXT_T_1)
      || (t == NW_WBXML_EXT_T_2)) {
    *x = val->component.ext.value.x;
    return NW_STAT_SUCCESS;
  }
  return NW_STAT_FAILURE;
}

NW_Uint8*
NW_DOM_WbxmlComponent_getOpaque(NW_TinyDom_AttrVal_t* pV,
                                NW_Uint32 *pOpaqueByteCount)
{
  if (pV
      && pOpaqueByteCount
      && (pV->type == NW_WBXML_ATTR_COMPONENT_OPAQUE)) {
    *pOpaqueByteCount = pV->component.opaque.length;
    return pV->component.opaque.data;
  }
  if (pOpaqueByteCount) {
    *pOpaqueByteCount = 0;
  }
  return NULL;
}

NW_Uint16
NW_DOM_WbxmlComponent_getToken(NW_TinyDom_AttrVal_t* pV)
{
  if (pV
      && (pV->type == NW_WBXML_ATTR_COMPONENT_TOKEN)) {
    return (NW_Uint16)pV->component.value_token;
  }
  return 0;
}

/* Returns

  NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED - if encoding is not supported

  NW_STAT_DOM_NO_STRING_RETURNED

  NW_STAT_SUCCESS

 MODIFIED - string - the String representation of item */
NW_Status_t
NW_DOM_WbxmlComponent_toString(NW_TinyDom_AttrVal_t* pV,
                               NW_String_t *string,
                               NW_Uint32 encoding)
{
  NW_Status_t status;

  if (!pV
      || !string) {
    return NW_STAT_DOM_NO_STRING_RETURNED;
  }

  NW_String_initialize(string, NULL, 0);

  if ((NW_String_charsetValid(encoding)) != NW_STAT_SUCCESS) {
    return NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED;
  }

  switch (pV->type)
  {
    case NW_WBXML_ATTR_COMPONENT_TOKEN:
      status = NW_String_tokenToString(pV->component.value_token, string, encoding);
      if(status != NW_STAT_SUCCESS){
        return NW_STAT_DOM_NO_STRING_RETURNED;
      }
      return NW_STAT_SUCCESS;
    case NW_WBXML_ATTR_COMPONENT_STRING:
      {
        NW_Byte *storage = pV->component.string.storage;
        status = NW_String_initialize(string, storage, encoding);
        if(status != NW_STAT_SUCCESS) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        return NW_STAT_SUCCESS;
      }
    case NW_WBXML_ATTR_COMPONENT_ENTITY:
      status = NW_String_entityToString(pV->component.entity,
                                        string, encoding);
      if(status != NW_STAT_SUCCESS){
        return NW_STAT_DOM_NO_STRING_RETURNED;
      }
  return NW_STAT_SUCCESS;
    case NW_WBXML_ATTR_COMPONENT_OPAQUE:
      return NW_STAT_DOM_NO_STRING_RETURNED;
    case NW_WBXML_ATTR_COMPONENT_EXT:
      {
        NW_Uint8 t; /* 8-bit token */

        t = (NW_Uint8)(pV->component.ext.token);
        if ((t == NW_WBXML_EXT_0)
            || (t == NW_WBXML_EXT_1)
            || (t == NW_WBXML_EXT_2)) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        if ((pV->component.ext.type
             == NW_TINYDOM_EXTENSION_TYPE_EXT_T_INTEGER)
            && ((t == NW_WBXML_EXT_T_0)
                || (t == NW_WBXML_EXT_T_1)
                || (t == NW_WBXML_EXT_T_2))) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        if (pV->component.ext.value.string.storage == NULL) {
          return NW_STAT_DOM_NO_STRING_RETURNED;
        }
        /* struct assignment, shallow copy */
        *string = pV->component.ext.value.string;
        return NW_STAT_SUCCESS;
      }
    default:
      return NW_STAT_DOM_NO_STRING_RETURNED;
  }
}

/* converts an entire component sequence (via iterator on text or attrval) to string

returns either NW_STAT_SUCCESS or NW_STAT_DOM_NO_STRING_RETURNED if something fails */
NW_Status_t
NW_DOM_WbxmlComponent_sequenceToString(NW_WbxmlComponent_t AttributeValueOrText,
                                       NW_TinyDom_AttributeHandle_t* pIterator,
                                       NW_Uint32 encoding,
                                       NW_String_t* pOutString)
{
  NW_TinyDom_AttrVal_t component;
  NW_String_t tempString;
  NW_Status_t s = NW_STAT_SUCCESS; /* if no sequence, then declare success! */

  NW_Mem_memset(&component, 0, sizeof(NW_TinyDom_AttrVal_t));

  while ((AttributeValueOrText == NW_WBXML_TEXT_COMPONENT) ?
         NW_TinyDom_TextHandle_iterate(pIterator, &component)
         : NW_TinyDom_AttributeHandle_iterateValues(pIterator, &component)) {

    s = NW_DOM_WbxmlComponent_toString(&component, &tempString, encoding);
    if (s != NW_STAT_SUCCESS) {
      break;
    }
    if ((pOutString->length == 0) || (pOutString->storage == NULL)) {
      s = NW_String_deepCopy(pOutString, &tempString);
    } else {
      s = NW_String_concatenate(pOutString, &tempString, encoding);
    }
    NW_String_deleteStorage(&tempString);
    if (s != NW_STAT_SUCCESS) {
      break;
    }
  }
  if (s != NW_STAT_SUCCESS) {
    NW_String_deleteStorage(pOutString);
    if (s == NW_STAT_OUT_OF_MEMORY)
    {
      s = NW_STAT_OUT_OF_MEMORY;
    }
    else
    {
    s = NW_STAT_DOM_NO_STRING_RETURNED;
    }
  }
  return s;
}



/*
 * Returns length of opaque data
 */

NW_Uint32
NW_DOM_Opaque_getLength(NW_DOM_Opaque_t *opaque)
{
  NW_ASSERT(opaque != NULL);

  return opaque->length & 0x7FFFFFFF;
}

/*
 * Returns data from opaque
 */

NW_Byte *
NW_DOM_Opaque_getData(NW_DOM_Opaque_t *opaque)
{
  NW_ASSERT(opaque != NULL);
  return opaque->data;
}


/*
 * Returns true if user owns data storage and can
 * deallocate it
 */
NW_Bool
NW_DOM_Opaque_getUserOwnsData(NW_DOM_Opaque_t *opaque)
{
  NW_ASSERT(opaque != NULL);

  /* The MSB of NW_Byte length if set represents that the
  string is from storage buffer, so BufferOwns String */
  if((opaque->length & 0x80000000) == 0){
    return NW_FALSE;
  }
  return NW_TRUE;
}

NW_Status_t
NW_DOM_Opaque_setUserOwnsData(NW_DOM_Opaque_t *opaque)
{
  NW_ASSERT(opaque != NULL);

  /* The MSB of NW_Byte length if set represents that the
  string is from storage buffer, so BufferOwns String */

  opaque->length = opaque->length | 0x80000000;
  return NW_STAT_SUCCESS;
}
