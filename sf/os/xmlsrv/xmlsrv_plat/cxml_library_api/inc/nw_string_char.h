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



/** ----------------------------------------------------------------------- **
    @package:     NW_String

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_STRING_CHARACTER_H
#define NW_STRING_CHARACTER_H

#include <xml/cxml/cxml_proj.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HTTP_iso_10646_ucs_2        0x03E8
#define HTTP_iso_8859_1             0x04
#define HTTP_us_ascii               0x03
#define HTTP_utf_8                  0x6A
#define HTTP_utf_16                 1015


/** ----------------------------------------------------------------------- **
    @struct:       NW_String_UCS2Buff

    @synopsis:    A ucs2 character as stored in a ucs2 string.

    @scope:       public
    @variables:
       NW_Byte bytes[2]
                  default

    @description: A ucs2 character as stored in a ucs2 string. This is 
                  different from a NW_Ucs2 which is a 16 bit unsigned 
                  quantity stored in the platform's native integer 
                  representation.  NW_UCS2Buff_t stores bytes in 
                  native-endian format.

 ** ----------------------------------------------------------------------- **/
typedef struct NW_String_UCS2Buff_s {
  NW_Byte bytes[2];
} NW_String_UCS2Buff_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_String_charsetValid

    @synopsis:    Checks if charset is valid.

    @scope:       public

    @parameters:
       [in] NW_Uint32 encoding
                  default

    @description: Checks if charset is valid.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Valid charset.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Invalid charset.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_String_charsetValid (NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_valid

    @synopsis:    Validate string storage.

    @scope:       public

    @parameters:
       [in] NW_Byte* storage
                  The storage.

       [in] NW_Uint32 length
                  The length.

       [in] NW_Uint32 encoding
                  The encoding.

    @description: Validate string storage.

    @returns:     NW_Int32
                  1 if storage is specified encoding, otherwise 0.

 ** ----------------------------------------------------------------------- **/
NW_Int32 
NW_String_valid(NW_Byte* storage, NW_Uint32 length, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_writeUTF8Char

    @synopsis:    Write chars to storage in UTF8 format.

    @scope:       public

    @parameters:
       [in] NW_Ucs2 c
                  Character to write.

       [in-out] NW_Byte* buff
                  Buffer to write into.default

    @description: Write a NW_Ucs2 into a buffer as UTF8. Returns number of 
                  bytes written.

    @returns:     NW_Uint32
                  Number of bytes written.

 ** ----------------------------------------------------------------------- **/
NW_Uint32 
NW_String_writeUTF8Char (NW_Ucs2 c, NW_Byte* buff);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_readChar

    @synopsis:    Read chars from storage.

    @scope:       public

    @parameters:
       [in] NW_Byte* buff
                  default

       [out] NW_Ucs2* c
                  default

       [in] NW_Uint32 encoding
                  default

    @description: Read one character of some encoding, returning the
                  NW_Ucs2 equivalent and the count of raw bytes read.

    @returns:     NW_Int32
                  Number of characters read or -1 if invalid encoding.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Int32 
NW_String_readChar (NW_Byte* buff, NW_Ucs2* c, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_charBuffGetLength

    @synopsis:    String length of input character buffer.

    @scope:       public

    @parameters:
       [in] void* buffer
                  default

       [in] NW_Uint32 encoding
                  default

       [out] NW_Uint32* byte_count
                  default

    @description: Get the length of a character string in some
                  encoding. Returns the number of characters (less the
                  terminating char). The out param byte_count returns
                  the number of bytes of storage scanned (including the
                  terminating char).  Note that there is NO validity
                  check here. This should be done first if needed.

    @returns:     NW_Int32
                  Returns number of characters read.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Int32 
NW_String_charBuffGetLength (void* buffer, NW_Uint32 encoding, NW_Uint32* byte_count);


/* ----------------------------------------------------------------------- **
   Conversions
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_String_charToUCS2Buff

    @synopsis:    Convert to UCS2 buffer.

    @scope:       public

    @parameters:
       [in] NW_Byte* s
                  Characters to convert.

       [in] NW_Uint32 encoding
                  Initial encoding.

    @description: Conversions among character strings of various types
                  and ucs2.  These functions assume that the length in
                  characters of the input buffer has been
                  pre-calculated, so that this operation doesn't have to
                  be performed for every conversion. This works well for
                  String_t which store the character count.

    @returns:     NW_String_UCS2Buff_t*
                  New UCS2 buffer or NULL if failed.

 ** ----------------------------------------------------------------------- **/
NW_String_UCS2Buff_t*
NW_String_charToUCS2Buff (NW_Byte* s, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_UTF8ToUCS2Buff

    @synopsis:    Convert UTF8 to UCS2 buffer.

    @scope:       public

    @parameters:
       [in] NW_Byte* s
                  Characters to convert.

    @description: Convert UTF8 to UCS2 buffer.

    @returns:     NW_String_UCS2Buff_t*
                  Converted buffer or NULL if failed.

 ** ----------------------------------------------------------------------- **/
NW_String_UCS2Buff_t*
NW_String_UTF8ToUCS2Buff (NW_Byte* s);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_ISO88591ToUCS2Buff

    @synopsis:    Convert ISO88591 to UCS2 buffer.

    @scope:       public

    @parameters:
       [in] NW_Byte* s
                  Characters to convert.

    @description: Convert ISO88591 to UCS2 buffer.

    @returns:     NW_String_UCS2Buff_t*
                  Converted buffer or NULL if failed.

 ** ----------------------------------------------------------------------- **/
NW_String_UCS2Buff_t*
NW_String_ISO88591ToUCS2Buff (NW_Byte* s);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_UCS2ToUTF8

    @synopsis:    Convert UCS2 to UTF8.

    @scope:       public

    @parameters:
       [in] NW_String_UCS2Buff_t* s
                  Characters to convert.

       [in] NW_Uint32 length
                  Number of characters.

    @description: Convert UCS2 to UTF8.

    @returns:     NW_Byte*
                  Converted buffer or NULL if failed.

 ** ----------------------------------------------------------------------- **/
NW_Byte*
NW_String_UCS2ToUTF8 (NW_String_UCS2Buff_t* s, NW_Uint32 length);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_UCS2ToISO88591

    @synopsis:    Convert UCS2 to ISO88591.

    @scope:       public

    @parameters:
       [in] NW_String_UCS2Buff_t* s
                  Characters to convert.

       [in] NW_Uint32 length
                  Number of characters.

    @description: Convert UCS2 to ISO88591.

    @returns:     NW_Byte*
                  Converted buffer or NULL if failed.

 ** ----------------------------------------------------------------------- **/
NW_Byte*
NW_String_UCS2ToISO88591 (NW_String_UCS2Buff_t* s, NW_Uint32 length);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_UCS2BuffCmp

    @synopsis:    Ordered comparison of ucs2 strings.

    @scope:       public

    @parameters:
       [in] NW_String_UCS2Buff_t* s1
                  The character buffer.

       [in] NW_String_UCS2Buff_t* s2
                  The other character buffer.

       [in] NW_Bool matchCase
                  If NW_TRUE then case sensitive comparison.

    @description: Ordered comparison of ucs2 strings.

    @returns:     NW_Int32
                  Return 0 if equal, -1 if s1 comes before s2 otherwise 1.

 ** ----------------------------------------------------------------------- **/
NW_Int32
NW_String_UCS2BuffCmp (NW_String_UCS2Buff_t* s1, 
                       NW_String_UCS2Buff_t* s2,
                       NW_Bool matchCase);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_CmpToNativeAlignedUCS2

    @synopsis:    Compare to native aligned UCS2.

    @scope:       public

    @parameters:
       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint32 charCount
                  The charcount.

       [in] NW_Uint8* s1
                  The character buffer.

       [in] NW_Uint16* s2
                  The other character buffer.

       [out] NW_Int32* r
                  Result of comparison.

    @description: Compare to native aligned UCS2. Assumes s2 is null 
                  terminated, native byte order and aligned for 
                  16-bit access.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Valid string.

       [NW_STAT_FAILURE]
                  Invalid string.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_String_CmpToNativeAlignedUCS2 (NW_Uint32 encoding, NW_Uint32 charCount,
                                  NW_Uint8* s1, NW_Uint16* s2,
                                  NW_Int32* r);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_STRING_CHARACTER_H */
