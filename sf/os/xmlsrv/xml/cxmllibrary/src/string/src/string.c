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
**  File: xml_string.c
**
**  Description:
 *
 * The following routines are an interface to raw strings stored in 
 * various encodings The implementation assumes a few things that 
 * are not necessarily generally true: 
 *  - every character read can be converted into a ucs_2 character
 *  - the string terminating character is a null character (not a null 
 *    NW_Byte)
 * 
*****************************************************************/
#include "cxml_internal.h"
#include <xml/cxml/nw_string_string.h>
#include <xml/cxml/nw_string_char.h>
#include <xml/cxml/nw_wbxml_dictionary.h>

static NW_Status_t
StringUCS2Init(NW_String_UCS2String_t * string, void *storage)
{
  NW_Status_t status = NW_STAT_SUCCESS;
  NW_Uint32 numBytes = 0;

  string->storage = (NW_Byte*) storage;
  if (string->storage){
    if (NW_String_charBuffGetLength(storage, HTTP_iso_10646_ucs_2, &numBytes) < 0) {
      numBytes = 0;
      status = NW_STAT_FAILURE;
    }
  }
  string->length = numBytes;
  return status;
}


/*  
 * TODO: The following functions need to have the allocator passed 
 * as an argument !!
 */

/*
 * RETURN NULL if malloc fails
 */
EXPORT_C NW_String_t *
NW_String_new (void)
{
  NW_String_t *str =
    (NW_String_t *) NW_Mem_Malloc (sizeof (NW_String_t));
  if (str == NULL)
    return NULL;
  str->length = 0;
  str->storage = NULL;
  return str;
}

EXPORT_C NW_Status_t 
NW_String_initialize (NW_String_t *string, void *storage, NW_Uint32 encoding)
{
  NW_Status_t status;
  NW_Uint32 numBytes = 0;

  NW_ASSERT(string != NULL);

  if (storage == NULL) {
    string->storage = NULL;
    string->length = 0;

    return NW_STAT_SUCCESS;
  }

  if ((status = NW_String_charsetValid (encoding)) != NW_STAT_SUCCESS)
    return status;

  string->storage = (NW_Byte*) storage;

  if (string->storage){
    if (NW_String_charBuffGetLength(storage, encoding, &numBytes) < 0) {
      return NW_STAT_FAILURE;
    }
  }
  string->length = numBytes;

  return NW_STAT_SUCCESS;
}

/*
 * Freeing a string might not free the storage!
 * String storage is complicated by the fact that
 * it can be allocated outside the bounds
 * of the parser. If so, it is not freed here.
 *
 */

EXPORT_C void 
NW_String_delete(NW_String_t *s)
{

  NW_ASSERT(s != NULL);

  if (NW_String_getUserOwnsStorage(s)){
    NW_Mem_Free (s->storage);
  }

  NW_Mem_Free (s);
}

EXPORT_C NW_Status_t
NW_String_deleteStorage(NW_String_t *s)
{

  NW_ASSERT(s != NULL);

  if (NW_String_getUserOwnsStorage(s))
  {
    if (s->storage != NULL){
      NW_Mem_Free(s->storage);
    }
  }
  s->storage = NULL;
	s->length = 0;
  return NW_STAT_SUCCESS;
}
/* 
 * Determine the length of the given string in characters (not
 * bytes).
 *
 * RETURN length in characters or 0 if s is NULL
 */

EXPORT_C NW_Uint16
NW_String_getCharCount(NW_String_t *s, NW_Uint32 encoding)
{
  NW_Uint16 numChar = 0;
  NW_Uint32 getNumChar = NW_String_getCharCount32(s,encoding);
 
  if(getNumChar <= NW_UINT16_MAX)
  {
	 numChar =(NW_Uint16) getNumChar;
  }

  return numChar;
}
/* 
 * The following function is a duplication of NW_String_getByteCount to
 * handle big files.  For not affecting the components other than
 * browser, this function is only called inside the browser.).
 *
 * INSTEAD OF CALLING NW_String_getByteCount, ALL THE BROWSER CODES SHOULD
 * CALL THIS DUPLICATE FUNCTION TO GET CHARACTER COUNTS.
 * 
 */

EXPORT_C NW_Uint32
NW_String_getCharCount32(NW_String_t *s, NW_Uint32 encoding)
{
  NW_Int32 numChars;  
  NW_Uint32 byteCount;

  if (s == NULL)
    return 0;
 
  if (s->storage == NULL)
    return 0;
 
  numChars = NW_String_charBuffGetLength(s->storage, encoding, &byteCount);

  if ( numChars < 0 ) {
    numChars = 0;
  }

  return  NW_UINT32_CAST( numChars );  
}

EXPORT_C NW_Uint32
NW_String_getByteCount (NW_String_t *s)
{
  NW_Uint32 uint32Len;

  if (s == NULL)
    return 0;

  /* The MSB of NW_Byte length if set represents that the 
  string is from storage buffer, so BufferOwns String */
  uint32Len = (s->length) & 0x7FFFFFFF; 
  return uint32Len;
}

/*
 * Returns NULL or valid storage
 */
EXPORT_C NW_Byte *
NW_String_getStorage(NW_String_t *str)
{
  if (str == NULL)
    return NULL;

  return str->storage;
}

/* 
 * Boolean comparison. This function assumes encodings to be
 * the same. It does not attempt to convert strings of different 
 * encodings for comparison, since we want to treat encoding as 
 * implicit for xml strings. 
 *
 * RETURN 1 if the strings are equal; 0 if the strings are not equal
 */

EXPORT_C NW_Int32
NW_String_equals(const NW_String_t *s1, const NW_String_t *s2)
{
  if ( (s1 == NULL) || (s2 == NULL) )
    return 0;

  if (s1 == s2)
    return 1;

  if (NW_String_getByteCount((NW_String_t *) s1) != NW_String_getByteCount((NW_String_t *) s2))
    return 0;
  /* 
   * Assumes that strings are zero terminated, so a substring 
   * can't share storage with the super string
   */
  if (s1->storage == s2->storage)
    return 1;

  /* Same length, different storage, compare NW_Byte-by-NW_Byte */

  if (NW_Mem_memcmp (((NW_String_t *) s1)->storage, ((NW_String_t *) s2)->storage, 
      NW_String_getByteCount((NW_String_t *) s1)))
    return 0;

  return 1;
}

EXPORT_C NW_Bool
NW_String_getUserOwnsStorage(NW_String_t * s)
{
  NW_Uint32 i;
  if (s == NULL)
    return 0;
  
  /* The MSB of NW_Byte length if set represents that the 
  string is from storage buffer, so BufferOwns String */
  i = s->length & 0x80000000;
  if (i== 0)
    return NW_FALSE;
  return NW_TRUE;
}


EXPORT_C NW_Status_t
NW_String_setUserOwnsStorage(NW_String_t * s)
{

  NW_ASSERT(s != NULL);
  
  /* The MSB of NW_Byte length if set represents that the 
  string is from storage buffer, so BufferOwns String */
  s->length = s->length | 0x80000000;
  return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_String_clearUserOwnsStorage(NW_String_t * s)
{

  NW_ASSERT(s != NULL);
  
  /* The MSB of NW_Byte length if set represents that the 
  string is from storage buffer, so BufferOwns String */
  s->length = s->length & 0x7fffffff;
  return NW_STAT_SUCCESS;
}


/*
 * Conversion routines 
 */

/*
 * RETURN NW_STAT_SUCCESS
 *        NW_STAT_OUT_OF_MEMORY
 *
 * MODIFIED ret_string
 */
NW_Status_t
NW_String_stringToUCS2 (NW_String_UCS2String_t ** ret_string, NW_String_t * s, NW_Uint32 input_encoding)
{
  NW_String_UCS2Buff_t *storage;

  if (*ret_string == NULL)
    *ret_string = NW_String_new();

  if (*ret_string == NULL)
    return NW_STAT_OUT_OF_MEMORY;

  if (input_encoding == HTTP_iso_10646_ucs_2)
    return NW_String_copy(*ret_string, s);

  if ((input_encoding == HTTP_utf_8) ||
      (input_encoding == HTTP_us_ascii) ||
      (input_encoding == HTTP_iso_8859_1))
  {
    NW_Status_t status;
    storage = NW_String_charToUCS2Buff (s->storage, input_encoding);

    if (storage == NULL){
      return NW_STAT_OUT_OF_MEMORY;
    }

    status = NW_String_initialize(*ret_string, (void *)storage, HTTP_iso_10646_ucs_2);
    if (status == NW_STAT_SUCCESS){
      return NW_String_setUserOwnsStorage(*ret_string);
    }
    else{
      NW_String_delete(*ret_string);
      NW_Mem_Free(storage);
      return status;
    }
  }
  NW_String_delete(*ret_string);
  return NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED;
}

/*
 * RETURN NW_STAT_SUCCESS
 *        NW_STAT_OUT_OF_MEMORY
 *        NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED
 *
 * MODIFIED ret_string
 */
static
NW_Status_t
NW_String_stringFromUCS2 (NW_String_t ** ret_string, NW_String_UCS2String_t * s, NW_Uint32 output_encoding)
{
  NW_Byte *storage;
  NW_String_UCS2Buff_t *ucs2Storage = (NW_String_UCS2Buff_t *) (s->storage);
  NW_Uint32 byteLength = NW_String_getByteCount(s);
  NW_Status_t status;

  if (*ret_string == NULL)
    *ret_string = NW_String_new();

  if (*ret_string == NULL)
    return NW_STAT_OUT_OF_MEMORY;

  if (output_encoding == HTTP_iso_10646_ucs_2)
    return NW_String_copy(*ret_string, s);

  if ( (output_encoding == HTTP_utf_8) || (output_encoding == HTTP_us_ascii) )
  {
    storage = NW_String_UCS2ToUTF8 (ucs2Storage, byteLength);
  }
  else if (output_encoding == HTTP_iso_8859_1)
  {
    storage = NW_String_UCS2ToISO88591 (ucs2Storage, byteLength);
  }
  else {
    NW_String_delete(*ret_string);
    return NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED;
  }
  if (storage == NULL)
    return NW_STAT_OUT_OF_MEMORY;
  
  status =  NW_String_initialize(*ret_string, storage, output_encoding);
  if(status == NW_STAT_SUCCESS){
   return  NW_String_setUserOwnsStorage(*ret_string);
  }
  else{
    NW_String_delete(*ret_string);
    NW_Mem_Free(storage);
    return status;
  }
}


/*
 * Convert a NW_String_t (in any of the supported character 
 * set encodings) to a UCS2Buff string.
 *
 * NOTE: caller is responsible for freeing the pointer returned
 *   by this function
 *
 * NOTE: returned array is null-terminated.
 *
 * RETURN NULL if malloc fails
 */
NW_String_UCS2Buff_t *
NW_String_stringToUCS2Buff (NW_String_t * string, NW_Uint32 encoding)
{
  NW_Byte *b = string->storage;
  if (!NW_String_charsetValid(encoding))
    return NULL;

  return NW_String_charToUCS2Buff(b, encoding);
}

/*
 * RETURN NW_STAT_SUCCESS
 *        NW_STAT_OUT_OF_MEMORY
 *        NW_WBXML_ERROR_CHARSET_UNSUPPORTED
 *
 * MODIFIED ret_string
 */
EXPORT_C NW_Status_t 
NW_String_UCS2BuffToString(NW_String_UCS2Buff_t *buff, NW_String_t *str, NW_Uint32 encoding)
{
  NW_Status_t status;
  NW_String_UCS2String_t* ucs2String = NW_String_new();

  if (ucs2String == NULL) {
    return NW_STAT_OUT_OF_MEMORY;
  }

  status = StringUCS2Init(ucs2String, (void *)buff);
  if (status == NW_STAT_SUCCESS) {
    status = NW_String_stringFromUCS2 (&str, ucs2String, encoding);
  }
  /* Freeing only the NW_String_t and not the storage, since we
     used the input storage */
  NW_Mem_Free(ucs2String);
  return status;
}


/*
 * Returns
 * NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED - If unsupported encoding
 * NW_STAT_OUT_OF_MEMORY - If memory for string storage could not be allocated
 * NW_STAT_WBXML_NO_NAME
 * NW_STAT_SUCCESS
 */
EXPORT_C NW_Status_t 
NW_String_ucs2CharToString(NW_String_t *string, NW_Ucs2 *u, NW_Uint32 encoding)
{
  NW_Byte *storage;
  NW_String_t *ucs2_string = NW_String_new();
  NW_Status_t status;
  NW_Ucs2 *p = u;
  NW_Uint32 len = 0;

  NW_ASSERT(string != NULL);
  NW_ASSERT(u != NULL);

  if (ucs2_string == NULL){
    return NW_STAT_OUT_OF_MEMORY;
  }

  while (*p!= 0)
  {
    len++;
    p++;
  }
  
  if (NW_String_charsetValid(encoding) != NW_STAT_SUCCESS) {
    NW_String_delete(ucs2_string);
    return NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED;
  }
  /* Create the storage and copy the bytes */

  if ((storage = (NW_Byte*) NW_Mem_Malloc ((len + 1) * sizeof(NW_String_UCS2Buff_t))) == NULL){
    return NW_STAT_OUT_OF_MEMORY;
  }

#if 0
  {
    NW_Int32 index = 0;

    while(*u != 0)
    {
      storage[index++] = (NW_Byte)(*u >> 8);
      storage[index++] = (NW_Byte)(*u & 0x00ff);
      u++;
    }

    storage[index++] = (NW_Byte)0;
    storage[index] = (NW_Byte)0;
  }
#else
    NW_Mem_memcpy(storage, u, (len + 1) * sizeof(NW_String_UCS2Buff_t));
#endif

  status = NW_String_initialize(ucs2_string, storage, HTTP_iso_10646_ucs_2);
  if (status != NW_STAT_SUCCESS){
    NW_Mem_Free(storage);
    NW_String_delete(ucs2_string);
    return status;
  }
  NW_String_setUserOwnsStorage(ucs2_string);

  status = NW_String_stringFromUCS2(&string, ucs2_string, encoding);
  NW_String_delete(ucs2_string);

  return status;
}

/*
 * Conversion to string
 */
EXPORT_C NW_Status_t 
NW_String_entityToString(NW_Uint32 entity, NW_String_t *str, NW_Uint32 encoding)
{
  NW_Status_t status;
  NW_Ucs2 *s;
  NW_Ucs2 e;

  if ((s = (NW_Ucs2*) NW_Mem_Malloc(2 * sizeof(NW_Ucs2))) == NULL)
    return NW_STAT_OUT_OF_MEMORY;

  e = (NW_Ucs2) entity;
  s[0] = e;
  s[1] = 0;

  status = NW_String_ucs2CharToString(str, s, encoding);
  NW_Mem_Free(s);

  return status;
}


NW_Status_t 
NW_String_byteToString(NW_Byte *s, NW_String_t *str, NW_Uint32 encoding)
{
  return NW_String_initialize(str, s, encoding);
}

EXPORT_C NW_Status_t 
NW_String_tokenToString(NW_Uint32 token, NW_String_t *str, NW_Uint32 encoding)
{
  NW_String_UCS2Buff_t *t;

  /* Get the tag from the dictionary */
  if ((t = NW_WBXML_Dictionary_getTagByFqToken (token)) == NULL)
    /* 
     * This can happen if the token is not defined or if the dictonary 
      for this token's codepage is not defined.  So, make a string 
     *from the token.
     */
    return NW_STAT_FAILURE;

  return NW_String_UCS2BuffToString(t, str, encoding);
}


/*
 * String manipulation functions
 */

/* Duplicates storage 
 * Returns
 *  NW_STAT_BAD_INPUT_PARAM
 *  NW_STAT_OUT_OF_MEMORY
 *  NW_STAT_SUCCESS
 */
NW_Status_t 
NW_String_copy(NW_String_t *dest, NW_String_t *source)
{

  NW_ASSERT(dest != NULL);
  NW_ASSERT(source != NULL);

  if (!NW_String_getUserOwnsStorage(source))
    return NW_String_shallowCopy(dest, source);

  return NW_String_deepCopy(dest, source);
}

EXPORT_C NW_Status_t
NW_String_shallowCopy(NW_String_t *dest, NW_String_t *source)
{

  NW_ASSERT(dest != NULL);
  NW_ASSERT(source != NULL);

  dest->storage = source->storage;
  dest->length = source->length;
  return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_String_deepCopy(NW_String_t *dest, NW_String_t *source)
{
  NW_Uint32 len;
 
  NW_ASSERT(dest != NULL);
  NW_ASSERT(source != NULL);

  len = NW_String_getByteCount(source);

  dest->storage = (NW_Byte *) NW_Mem_Malloc (len);

  if (dest->storage == NULL){
    return NW_STAT_OUT_OF_MEMORY;
  }
  NW_Mem_memcpy(dest->storage, source->storage, len);
  dest->length = len;
  /* Since we allocated the new storage, we own it */
  NW_String_setUserOwnsStorage(dest);
  return NW_STAT_SUCCESS;
}

EXPORT_C NW_Status_t
NW_String_concatenate(NW_String_t *dest, NW_String_t *source, NW_Uint32 encoding)
{
  NW_Byte *storage = NULL;
  NW_Uint32 destLength = 0;
  NW_Uint32 sourceLength = 0;
 
  NW_ASSERT(dest != NULL);
  NW_ASSERT(source != NULL);
  NW_ASSERT(source->storage != NULL); 
  
  destLength = NW_String_getByteCount(dest);
  sourceLength = NW_String_getByteCount(source);
  NW_ASSERT(sourceLength != 0);

  /* get rid of the tailing NULL from dest string, in case of ucs2, 
     two bytes need to be removed. */
  if ( (encoding == HTTP_iso_10646_ucs_2) &&
      (destLength > 1) &&
      (dest->storage[(destLength-1)] == 0) && 
      (dest->storage[ (destLength-2) ] == 0) ) 
  {
    destLength--;
    destLength--;
  }
  else if ((encoding == HTTP_utf_8) ||
           (encoding == HTTP_us_ascii) ||
           (encoding == HTTP_iso_8859_1)) 
  {
    if ( (destLength > 0) && (dest->storage[destLength-1] == 0) ) {
      destLength--;
    }
  }
   
  storage = (NW_Byte*) NW_Mem_Malloc (destLength +sourceLength);
  if (storage == NULL)
    return NW_STAT_OUT_OF_MEMORY;

  if ((dest->storage != NULL) && (destLength > 0))
  {
    NW_Mem_memcpy(storage, dest->storage, destLength);
    NW_Mem_memcpy(storage + destLength, source->storage, sourceLength);
  }
  else
  {
    NW_Mem_memcpy(storage, source->storage, sourceLength);
  }
  NW_String_deleteStorage(dest);
  dest->storage = storage;
  dest->length = destLength +sourceLength;
  /* Since we allocated the new storage, we own it */
  NW_String_setUserOwnsStorage(dest);
  return NW_STAT_SUCCESS;
}

NW_Byte *
NW_String_findChar(NW_String_t *string, NW_Int32 character, NW_Uint32 encoding)
{
  NW_Byte *storage;
  NW_Uint32 length;
  NW_Uint32 index, numbytes;
  NW_Ucs2 c;

  if (string == NULL)
    return NULL;

  length  = NW_String_getCharCount(string, encoding);
  storage = string->storage;

  for (index= 0; index<length; index++)
  {
    /* read UCS2 character and advance pointer */
    numbytes = NW_String_readChar(storage, &c, encoding);
    if (c == character) {
      return storage;
    }
    storage += numbytes;
  }

  return NULL;  
}


/*
 * Functions
 */

/*
* IN - No of characters 
*/
NW_Status_t 
NW_String_ucs2CharInit(NW_Ucs2 **data, NW_Uint32 length)
{
  NW_ASSERT(data != NULL);

  if ((*data = (NW_Ucs2*) NW_Mem_Malloc (length *sizeof(NW_Ucs2))) == NULL) {
    return NW_STAT_OUT_OF_MEMORY;  
  }

  return NW_STAT_SUCCESS;
}

EXPORT_C NW_Int32 
NW_String_ucs2CharLength (NW_Ucs2 *data)
{
  NW_Int32 length = 0;

  if (data == NULL)
    return -1;

  while(*data != 0)
  {
    length++;
    data++;
  }
  return length;
}
/*
 * Given an arbitrary NW_Byte array with no known character encoding, 
 * convert it to a NW_Ucs2 string.
 * 
 */
EXPORT_C NW_Status_t
NW_String_byteToUCS2Char (NW_Byte * b, NW_Uint32 length, NW_Ucs2 **data)
{
  NW_Status_t status;
  NW_Byte *p = b;
  NW_Uint32 i = 0;

  NW_ASSERT(length != 0);
  NW_ASSERT(b != NULL);

  /*Need an extra NW_Byte for the null-terminator */
  status = NW_String_ucs2CharInit(data, length + 1);
  if (status != NW_STAT_SUCCESS)
    return status;

  for (i = 0; i < length; i++, p++)
  {
    (*data)[i] = *p;
  }

  /* Null-terminate the string */
  (*data)[length] = '\000';

  return NW_STAT_SUCCESS;
}


EXPORT_C NW_Status_t
NW_String_entityToUCS2Char(NW_Uint32 entity, NW_Ucs2 **data)
{
  NW_Status_t status;

  NW_ASSERT(data != NULL);

  status = NW_String_ucs2CharInit(data, 2);
  if (status != NW_STAT_SUCCESS)
    return status;

  (*data)[0] = (NW_Ucs2)entity;
  (*data)[1] = 0;

  return NW_STAT_SUCCESS;
}

/*
NW_Status_t 
NW_String_tokenToUCS2Char(NW_Uint32 token, NW_Ucs2 **data)
{
  NW_String_UCS2Buff_t *t;

  // Get the tag from the dictionary 
  if ((t = NW_WBXML_Dictionary_getTagByFqToken (token)) == NULL)
     
     // This can happen if the token is not defined or if the dictonary 
     //for this token's codepage is not defined.  So, make a string 
     //from the token.
     //
    return NW_STAT_FAILURE;

  return NW_String_UCS2BuffToUCS2Char(t, data);
}
*/

EXPORT_C NW_Status_t 
NW_String_stringToUCS2Char(const NW_String_t* string, 
                           NW_Uint32 encoding, 
                           NW_Ucs2 **data)
{
  NW_Uint32 i;
  NW_Int32 numChars;
  NW_Ucs2 c = 1;
  NW_Uint32 count = 0;
  NW_Byte *b = string->storage;
  NW_Status_t status;
  NW_Int32 retval = 0;
  NW_Uint32 numBytes = 0;

  if (b == NULL)
  {
    *data = NULL;
  }
  else
  {
    numChars = NW_String_charBuffGetLength(b, encoding, &numBytes);
    if(numChars < 0){
      return NW_STAT_FAILURE;
    }

    /* allocate the memory and point to it with *data */
    status = NW_String_ucs2CharInit(data, (NW_Uint32)numChars+1);

    if (status != NW_STAT_SUCCESS){
      return status;
    }

    /* copy the characters out of string->storage onto *data */
    for (i=0; c != 0; i++) {
      retval = NW_String_readChar((NW_Byte *) b + count, &c, encoding);
      if(retval < 0){
        NW_Mem_Free( *data );
	*data = NULL;
        return NW_STAT_FAILURE;
      }
      count += (NW_Uint32)retval;
      (*data)[i] = c;
    }

    /* finally null terminate the string storage */
    (*data)[numChars] = 0;
  }
  return NW_STAT_SUCCESS;
}
/*
NW_Status_t 
NW_String_UCS2BuffToUCS2Char(NW_String_UCS2Buff_t *buff, NW_Ucs2 **data)
{
  NW_Int32 numChars = 0;
  NW_Uint32 count;
  NW_Status_t status;

  NW_ASSERT(buff != NULL);  
  NW_ASSERT(data != NULL);

  numChars = NW_String_charBuffGetLength(buff, HTTP_iso_10646_ucs_2, &count);
  if (numChars < 0) {
    return NW_STAT_FAILURE;
  }
  // Create the storage and copy the bytes 
  status = NW_String_ucs2CharInit(data, (NW_Uint32)numChars+1);
  if (status != NW_STAT_SUCCESS) {
    return status;
  }
  (void)NW_Mem_memcpy(*data, buff, (NW_Uint32) numChars * sizeof(NW_Ucs2));
  (*data)[numChars] = 0;

  return NW_STAT_SUCCESS;
}
*/
