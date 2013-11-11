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
**  File: character.c
**  Description:
* 
* Note: all of the char functions assume that the input buffer points
* to an array of characters which contains a null somewhere and is
* correctly encoded for the particular encoding. Bad things can happen
* if this is not the case. In order to avoid having to check these 
* conditions on every operation, a set of validate functions is provided
* to pre-test a string where the caller is not sure these conditions
* are met.  It is especially important, when calling character operations
* on bytecode, to make sure to validate all strings.
* 
*****************************************************************/
#include "cxml_internal.h"
#include <xml/cxml/nw_string_char.h>

/*  
* TODO:  Note that there is some duplication between the Validate*
* calls and the string length function.  The Validate*S could return
* the length too.
*/

/*
* Check that storage points to a valid UTF8 string no longer
* than length bytes.
*/
static NW_Int32
StringValidUTF8 (NW_Byte * storage, NW_Uint32 length)
{
  NW_Uint32 i;
  NW_Byte bits;

  NW_ASSERT(storage != NULL);
  NW_ASSERT(length != 0);

  for (i = 0; i < length;)
  {
    if (storage[i] == 0)
      {
        return 1;
      }
    bits = (NW_Byte) (storage[i] >> 4);
    if (bits < 8)
    {
          i++;
    }
    else if ((bits == 12) || (bits == 13))
    { 
  	  i+=2;
    }
    else if (bits == 14)
    {
	    i += 3;
    }
    else if (bits == 15)
    {
	    i += 4;
    }
    else
    {
          return 0;
    }
  }    
  return 0;
}


/*  
* Check validity of UCS2 string storage 
*/
static NW_Int32
StringValidUCS2 (NW_Byte * storage, NW_Uint32 length)
{
  NW_Uint32 i;
    
  NW_ASSERT(storage != NULL);
  NW_ASSERT(length != 0);

  for (i = 0; i < (length - 1); i += 2)
  {
    if (((storage[i] << 8) | storage[i + 1]) == 0)
    {
      return 1;
    }
  }
  return 0;
}


/* 
* Check validity of ISO8859 string storage
*/
static NW_Int32
StringValidISO88591 (NW_Byte * storage, NW_Uint32 length)
{

  NW_Uint32 i;

  NW_ASSERT(storage != NULL);
  NW_ASSERT(length != 0);

  for (i = 0; i < length; i++)
  {
    if (storage[i] == 0)
    {
	return 1;
    }
  }
  return 0;
}


/* 
* Check validity of ASCII string storage
*/
static NW_Int32
StringValidUSASCII (NW_Byte * storage, NW_Uint32 length)
{
  NW_Uint32 i;

  NW_ASSERT(storage != NULL);
  NW_ASSERT(length != 0);

  for (i = 0; i < length; i++)
  {
    if (storage[i] == 0)
    {
	return 1;
    }
  }
  return 0;
}


/*
* Check the given charset encoding (MIBENUM) and if it
*              is supported.
*/

NW_Status_t
NW_String_charsetValid (NW_Uint32 encoding)
{
  switch (encoding)
  {
  case HTTP_iso_10646_ucs_2:
  case HTTP_iso_8859_1:
  case HTTP_utf_8:
  case HTTP_us_ascii:
    return NW_STAT_SUCCESS;
  default:
    return NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED;
  }
}


/*
* RETURN -1 if the encoding is not supported
*/
NW_Int32
NW_String_valid(NW_Byte * storage, NW_Uint32 length, NW_Uint32 encoding)
{
  if (encoding == HTTP_iso_10646_ucs_2)
  {
	  return StringValidUCS2 (storage, length);
  }
  else if (encoding == HTTP_utf_8)
  {
	  return StringValidUTF8 (storage, length);
  }
  else if (encoding == HTTP_iso_8859_1)
  {
	  return StringValidISO88591 (storage, length);
  }
  else if (encoding == HTTP_us_ascii)
  {
	  return StringValidUSASCII (storage, length);
  }
  
  return -1;
}


/* 
* TODO:  The following routines are taken from Rainbow.
* They should be revisited for better efficiency, etc.
*/

/*
* Read one UTF8 character from a buffer and store it as a NW_Ucs2.
* Returns number of input bytes read.
*/
static NW_Int32
ReadUTF8Char (NW_Byte * buff, NW_Ucs2 * c)
{
  switch ((buff[0] >> 4) & 0xf)
  {
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    /* 1 NW_Byte */
    *c = (NW_Ucs2) buff[0];
    return 1;
    
  case 12:  
  case 13:
    /* 2 bytes */
    if ((buff[1] & 0xC0) != 0x80)
    {
	     return -1;
    }
    *c = (NW_Ucs2) (((buff[0] & 0x1F) << 6) | (buff[1] & 0x3F));
    return 2;
    
  case 14:
    /* 3 bytes */
    if (((buff[1] & 0xC0) != 0x80) && ((buff[2] & 0xC0) != 0x80))
    {
	     return -1;
    }
    *c = (NW_Ucs2) (((buff[0] & 0x0F) << 12) |
      ((buff[1] & 0x3F) << 6) | ((buff[2] & 0x3F) << 0));
    return 3;

  //we used not to handle 4-bytes UTF-8 case (only 16 bits handled), the case 15 is newly added, it may cause
  //problem if in an application the a 4-byte character would convert to ucs2 encoding. 
  case 15:  
    /* 4 bytes */

    if (((buff[1] & 0xC0) != 0x80) && ((buff[2] & 0xC0) != 0x80) && ((buff[3] & 0xC0) != 0x80))
    {
	     return -1;
    }
    *c = (((buff[0] & 0x07) << 18) |
          ((buff[1] & 0x3F) << 12) |
          ((buff[2] & 0x3F) << 6) |
           (buff[3] & 0x3F));
    return 4;


    
  default:
    return -1;    /* Bad format */
  }
}


/* 
* Write a NW_Ucs2 into a buffer as UTF8. Returns number of bytes written 
*/
NW_Uint32
NW_String_writeUTF8Char (NW_Ucs2 c, NW_Byte * buff)
{
  if (c <= 0x007F)
  {
    /* 0x0000 - 0x007F: 1 NW_Byte UTF-8 encoding. */
    buff[0] = (NW_Byte) c;
    return 1;
  }
  else if (c > 0x07FF)
  {
    /* 0x0800 - 0xFFFF: 3 NW_Byte UTF-8 encoding. */
    buff[0] = (NW_Byte) (0xE0 | ((c >> 12) & 0x0F));
    buff[1] = (NW_Byte) (0x80 | ((c >> 6) & 0x3F));
    buff[2] = (NW_Byte) (0x80 | ((c >> 0) & 0x3F));
    return 3;
  }
  else
  {
    /* 0x0080 - 0x07ff: 2 NW_Byte UTF-8 encoding. */
    buff[0] = (NW_Byte) (0xC0 | ((c >> 6) & 0x1F));
    buff[1] = (NW_Byte) (0x80 | ((c >> 0) & 0x3F));
    return 2;
  }
}


static NW_Int32
ReadInt16Char (NW_Byte * buff, NW_Ucs2 * c)
{
  /* read unaligned native-endian to aligned native-endian */
  (void) NW_Mem_memcpy(c, buff, sizeof(NW_Ucs2));
  return sizeof(NW_Ucs2);
}

static NW_Int32
ReadISO88591Char (NW_Byte * buff, NW_Ucs2 * c)
{
  *c = buff[0];
  return 1;
}

static NW_Int32
ReadUSASCIIChar (NW_Byte * buff, NW_Ucs2 * c)
{
  *c = buff[0];
  return 1;
}

/*
* Read one character of some encoding, returning the NW_Ucs2 
* equivalent and the count of raw characters read
*
* RETURN -1 if encoding is not supported
*/
EXPORT_C NW_Int32
NW_String_readChar (NW_Byte * buff, NW_Ucs2 * c, NW_Uint32 encoding)
{
  NW_Int32 nbytes = 0;
  
  if (encoding == HTTP_iso_10646_ucs_2)
    return ReadInt16Char (&buff[nbytes], c);
  else if (encoding == HTTP_utf_8)
    return ReadUTF8Char (&buff[nbytes], c);
  else if (encoding == HTTP_iso_8859_1)
    return ReadISO88591Char (&buff[nbytes], c);
  else if (encoding == HTTP_us_ascii)
    return ReadUSASCIIChar (&buff[nbytes], c);

  return -1;
}


/* 
* Get the length of a character string in some encoding. Returns the number
* of characters (less the terminating char). The out param byte_count returns
* the number of bytes of storage scanned (including the terminating char).
* Note that there is NO validity check here. This should be done first if
* needed.  TODO:  Also note that the validity check could return the length 
* directly, thus eliminating the need for call to this function when 
* doint32 validity checkint32.
*/
EXPORT_C NW_Int32
NW_String_charBuffGetLength (void *buffer, NW_Uint32 encoding, NW_Uint32 * byte_count)
{
  NW_Int32 chars = 0;
  NW_Ucs2 c = 1;
  NW_Int32 retval = 0;
  
  *byte_count = 0;
  while (c)
  {
    c = 0; /* partial protection against an infinite loop */
    retval = NW_String_readChar ((NW_Byte *) buffer + *byte_count, &c, encoding);
    if(retval < 0){
      return -1;
    }
    (*byte_count) += (NW_Uint32) retval;
    chars++;
  }
  
  return chars - 1;
}


/* 
* Conversions among character strings of various types and ucs2.
* These functions assume that the length in characters of the 
* input buffer has been pre-calculated, so that this operation
* doesn't have to be performed for every conversion. This works well
* for String_t which store the character count.
*
* RETURN NULL if malloc fails
*/
NW_String_UCS2Buff_t *
NW_String_charToUCS2Buff (NW_Byte * s, NW_Uint32 encoding)
{
  NW_String_UCS2Buff_t *storage;
  NW_Ucs2 c;
  NW_Int32 i;
  NW_Int32 count = 0;
  NW_Int32 length = 0;
  NW_Uint32 byteCount = 0;
  NW_Int32 retval = 0;
  
  if (!NW_String_charsetValid(encoding))
  {
    return NULL;
  }

  length = NW_String_charBuffGetLength(s, encoding, &byteCount);
  if(length < 0){
    return NULL;
  }
  storage =
    (NW_String_UCS2Buff_t*) 
    NW_Mem_Malloc(((NW_Uint32)length + 1) * sizeof (NW_String_UCS2Buff_t));
  if (storage == NULL)
  {
    return NULL;
  }
  
  for (i = 0; i < length; i++)
  {
    retval = NW_String_readChar (s + count, &c, encoding);
    if(retval < 0){
      NW_Mem_Free(storage);
      return NULL;
    }
    count += retval;
    storage[i].bytes[0] = (NW_Byte) ((c & 0xff00) >> 8);
    storage[i].bytes[1] = (NW_Byte) (c & 0xff);
  }
  storage[length].bytes[0] = 0;
  storage[length].bytes[1] = 0;
  
  return storage;
}


/*
* TODO: is this a public or private function ???
*/
NW_String_UCS2Buff_t *
NW_String_UTF8ToUCS2Buff (NW_Byte * s)
{
  return NW_String_charToUCS2Buff (s, HTTP_utf_8);
}


/*
* TODO: is this a public or private function ???
*/
NW_String_UCS2Buff_t *
NW_String_ISO88591ToUCS2Buff (NW_Byte * s)
{
  return NW_String_charToUCS2Buff (s, HTTP_iso_8859_1);
}


/*
* RETURN NULL if malloc fails
*/
NW_Byte *
NW_String_UCS2ToUTF8 (NW_String_UCS2Buff_t * s, NW_Uint32 length)
{
  NW_Byte *tstore;
  NW_Byte *storage;
  NW_Ucs2 c;
  NW_Uint32 i;
  NW_Int32 count = 0;
  NW_Ucs2 *src = (NW_Ucs2 *)s; /*WMS we should use UCS2 pointer, 
                                 because s is a structure and the size of a structure is not fixed
					             in ARM processor, the size of NW_String_UCS2Buff_t is 4 byte 
                                 (address alignment issue) */
    
  tstore = (NW_Byte *) NW_Mem_Malloc ((length + 1) * 3);
  if (tstore == NULL)
  {
    return NULL;
  }
  
  for (i = 0; i < length; i++)
    {
      ReadInt16Char ((NW_Byte *) (src + i), &c);
      count += NW_String_writeUTF8Char (c, tstore + count);
    }
    *(tstore + count) = 0;
    storage = (NW_Byte *) NW_Mem_Malloc (count + 1);
    if (storage)
    {
      NW_Mem_memcpy (storage, tstore, count + 1);
    }
    NW_Mem_Free (tstore);
    
    return storage;
}


/*
* RETURN NULL if malloc fails
* byteCount is total allocation size of s as far as conversion is concerned
*/
NW_Byte *
NW_String_UCS2ToISO88591 (NW_String_UCS2Buff_t * s, NW_Uint32 byteCount)
{
  NW_Byte *storage = NULL;
  NW_Ucs2 c;
  NW_Uint32 i;
  NW_Ucs2 *src = (NW_Ucs2 *)s; /*WMS we should use UCS2 pointer, 
                                 because s is a structure and the size of a structure is not fixed
					             in ARM processor, the size of NW_String_UCS2Buff_t is 4 byte 
                                 (address alignment issue) */
 
  storage = (NW_Byte *) NW_Mem_Malloc (byteCount + 1);
  if (storage == NULL)
  {
    return NULL;
  }
  
  for (i = 0; i < byteCount; i++)
  {
    ReadInt16Char ((NW_Byte *) (src + i), &c);
    storage[i] = (NW_Byte) (c & 0xff);
  }
  storage[byteCount] = 0;
  
  return storage;
}

/* Ordered comparison of ucs2 strings */
NW_Int32
NW_String_UCS2BuffCmp (NW_String_UCS2Buff_t * s1, 
                       NW_String_UCS2Buff_t * s2,
                       NW_Bool matchCase)
{
  NW_Ucs2 c1, c2;
  NW_Ucs2 *src1 = (NW_Ucs2 *)s1; /*WMS we should use UCS2 pointer, */
  NW_Ucs2 *src2 = (NW_Ucs2 *)s2; /*because s is a structure and the size of a structure is not fixed
					             in ARM processor, the size of NW_String_UCS2Buff_t is 4 byte 
                                 (address alignment issue) */

  while ( ( *src1 ) || ( *src2 ) )
  {
    ReadInt16Char ((NW_Byte *) src1++, &c1);
    ReadInt16Char ((NW_Byte *) src2++, &c2);

    if (matchCase == NW_FALSE) {
      c1 = CXML_Str_ToLower (c1);
      c2 = CXML_Str_ToLower (c2);
    }
    if (c1 == c2)
    {
      continue;
    }
    return (c1 < c2) ? -1 : 1;
  }
  
  return 0;
  
}


/* Assumes s2 is null terminated, native byte order
and aligned for 16-bit access */
NW_Status_t
NW_String_CmpToNativeAlignedUCS2 (NW_Uint32 encoding, NW_Uint32 charCount,
                                  NW_Uint8 * s1, NW_Uint16 * s2,
                                  NW_Int32 * r)
{
    NW_Uint32 i;
    NW_Int32 byteCount = 0;
    NW_Ucs2 c1;

    for (i = 0; i < charCount; i++, s1 += byteCount, s2++) {
        byteCount = NW_String_readChar (s1, &c1, encoding);
        if (byteCount < 0) {
            return NW_STAT_FAILURE;
        }
        *r = c1 - *s2;
        if (*r || (*s2 == 0)) {
            break;
        }
    }
    /* You can exit the above loop three ways: i == charCount or
    when i != charCount because one of mismatch or null termination
    of s2 is encountered.  The only one that needs a fixup is if
    i == charCount but s2 isn't at null termination. */

    /*lint -e{794} Conceivable use of null pointer */
    if ((i == charCount) && (*s2 != 0)) {
        *r = -*s2;
    }
    return NW_STAT_SUCCESS;
}
