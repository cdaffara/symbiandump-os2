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

#ifndef NW_STRING_STRING_H
#define NW_STRING_STRING_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_string_char.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @struct:      NW_String_String

    @synopsis:    The basic wbxml string type.

    @scope:       public
    @variables:
       NW_Uint32 length
                  In bytes.

       NW_Byte* storage
                  default

    @description: The basic wbxml string type.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_String_String_s{
  NW_Uint32 length;
  NW_Byte* storage;
}NW_String_String_t;

/* Deprecated - Left in for backward compatibility */
typedef struct NW_String_String_s NW_String_t;

typedef struct NW_String_String_s NW_String_UCS2String_t;

/** ----------------------------------------------------------------------- **
    @function:    NW_String_new

    @synopsis:    Consructor.

    @scope:       public

    @description: Constructor.

    @returns:     NW_String_t*
                  New string or NULL if out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_String_t*
NW_String_new (void);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_initialize

    @synopsis:    Initialize.

    @scope:       public

    @parameters:
       [in] NW_String_t* string
                  The string.

       [in] void* storage
                  The storage.

       [in] NW_Uint32 encoding
                  The encoding.

    @description: Initialize the string by assigning length, encoding 
                  and storage. Note that the length is calculated.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String initialized.

       [NW_STAT_FAILURE]
                  String not initialized.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Unsupported charset.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_String_initialize (NW_String_t* string, void *storage, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_delete

    @synopsis:    Delete.

    @scope:       public

    @parameters:
       [in-out] NW_String_t* string
                  The string.

    @description: Freeing a string may not free the storage. String storage 
                  is complicated by the fact that it can be allocated outside 
                  the bounds of the parser. If so, it is not freed here.

 ** ----------------------------------------------------------------------- **/
IMPORT_C void 
NW_String_delete(NW_String_t* string);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_getByteCount

    @synopsis:    Returns number of bytes in the string including NULL 
                  terminator.

    @scope:       public

    @parameters:
       [in] NW_String_t* string
                  The string.

    @description: Returns number of bytes in the string including NULL 
                  terminator.

    @returns:     NW_Uint16
                  Length in bytes.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint32 
NW_String_getByteCount (NW_String_t* string);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_getCharCount

    @synopsis:    Returns number of characters in the string excluding NULL 
                  terminator.

    @scope:       public

    @parameters:
       [in] NW_String_t* string
                  The string.

       [in] NW_Uint32 encoding
                  The encoding.

    @description: Returns number of characters in the string excluding NULL 
                  terminator.

    @returns:     NW_Uint16
                  Length of string in characters.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint16 
NW_String_getCharCount(NW_String_t* string, NW_Uint32 encoding);

/* 
 * The following function is a duplication of NW_String_getByteCount to
 * handle big files.  For not affecting the components other than
 * browser, this function is only called inside the browser.).
 *
 * INSTEAD OF CALLING NW_String_getByteCount, ALL THE BROWSER CODES SHOULD
 * CALL THIS DUPLICATE FUNCTION TO GET CHARACTER COUNTS.
 * 
 */
IMPORT_C NW_Uint32 
NW_String_getCharCount32(NW_String_t* string, NW_Uint32 encoding);

/** ----------------------------------------------------------------------- **
    @function:    NW_String_getStorage

    @synopsis:    Returns NULL or valid storage.

    @scope:       public

    @parameters:
       [in] NW_String_t* str
                  The string.

    @description: Returns the storage occupied by the string (in bytes).

    @returns:     NW_Byte*
                  String storage. 

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Byte*
NW_String_getStorage(NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_equals

    @synopsis:    Determines if strings are equal.

    @scope:       public

    @parameters:
       [in] const NW_String_t* string1
                  The string.

       [in] const NW_String_t* string2
                  The other string.

    @description: This function assumes the encoding of each string is 
                  the same. It does not attempt to convert strings of 
                  different encodings because encoding is regarded as 
                  implicit for XML strings. NOte that if both string 
                  pointers are NULL thay are considered unequal. 

    @returns:     NW_Int32
                  1 if the strings are equal; otherwise 0.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Int32 
NW_String_equals(const NW_String_t* string1, const NW_String_t* string2);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_getUserOwnsStorage

    @synopsis:    Get user owns storage flag.

    @scope:       public

    @parameters:
       [in] NW_String_t* s
                  The string.

    @description: Get user owns storage flag. If the user owns the storage
                  it means that this string is the sole user of the storage
                  and can delete it when done.

    @returns:     NW_Bool
                  NW_TRUE if user owns storage, otherwise NW_FALSE.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Bool
NW_String_getUserOwnsStorage(NW_String_t* s);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_setUserOwnsStorage

    @synopsis:    Set user owns storage flag.

    @scope:       public

    @parameters:
       [in-out] NW_String_t* s
                  The string.

    @description: Set user owns storage flag. (see NW_String_getUserOwnsStorage)

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_String_setUserOwnsStorage(NW_String_t* s);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_clearUserOwnsStorage

    @synopsis:    Clear user owns storage flag.

    @scope:       public

    @parameters:
       [in-out] NW_String_t* s
                  The string.

    @description: Clear user owns storage flag. This means that someone
                  else may own the storage.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_String_clearUserOwnsStorage(NW_String_t* s);

/* ----------------------------------------------------------------------- **
   Conversion routines   
** ----------------------------------------------------------------------- **/

/** ----------------------------------------------------------------------- **
    @function:    NW_String_stringToUCS2

    @synopsis:    Convert string to UCS2.

    @scope:       public

    @parameters:
       [out] NW_String_UCS2String_t** u
                  The converted string.

       [in] NW_String_t* string
                  The string to convert.

       [in] NW_Uint32 encoding
                  The encoding of the string to be converted

    @description: Converts a string of a given encoding to ucs2_char.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String converted.

       [NW_STAT_FAILURE]
                  String not converted.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Unsupported charset.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_String_stringToUCS2(NW_String_UCS2String_t** u, NW_String_t* string, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_UCS2BuffToString

    @synopsis:    Convert UCS2 buffer to string.

    @scope:       public

    @parameters:
       [in] NW_String_UCS2Buff_t* u
                  Source of data to convert.

       [out] NW_String_t* string
                  The converted string.

       [in] NW_Uint32 encoding
                  default

    @description: Convert UCS2 buffer to string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String converted.

       [NW_STAT_OUT_OFMEMORY]
                  Out of memory.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Unsupported charset.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_String_UCS2BuffToString(NW_String_UCS2Buff_t* u, NW_String_t* string, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_stringToUCS2Buff

    @synopsis:    Convert string to UCS2 buffer.

    @scope:       public

    @parameters:
       [in] NW_String_t* string
                  The string.

       [in] NW_Uint32 encoding
                  Initial encoding.

    @description: Convert a NW_String_t (in any of the supported character
                  set encodings) to a UCS2Buff string. Caller is responsible
                  for freeing the pointer returned by this function. Returned
                  array is null-terminated.

    @returns:     NW_String_UCS2Buff_t* 
                  USC2 buffer or NULL if it fails.

 ** ----------------------------------------------------------------------- **/
NW_String_UCS2Buff_t* 
NW_String_stringToUCS2Buff(NW_String_t* string, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_ucs2CharToString

    @synopsis:    Convert UCS2 characters to string.
    @scope:       public

    @parameters:
       [in-out] NW_String_t* string
                  The string.

       [in] NW_Ucs2* u
                  The characters to convert.

       [in] NW_Uint32 encoding
                  Target encoding.

    @description: Convert UCS2 characters to string.

    @returns:     NW_Status_t
                  The status of the operation.

       [NW_STAT_SUCCESS]
                  String filled with chars.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Unsupported encoding.

       [NW_STAT_OUT_OF_MEMORY]
                  String storage could not be allocated.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_String_ucs2CharToString (NW_String_t* string, NW_Ucs2* u, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_entityToString

    @synopsis:    Convert entity to string.
    @scope:       public

    @parameters:
       [in] NW_Uint32 entity
                  default

       [in-out] NW_String_t* str
                  The string.

       [in] NW_Uint32 encoding
                  Target encoding.

    @description: Convert entity to string.

    @returns:     NW_Status_t
                  The status of the operation.

       [NW_STAT_SUCCESS]
                  String initialized from entity.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Unsupported encoding.

       [NW_STAT_OUT_OF_MEMORY]
                  String storage could not be allocated.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_String_entityToString(NW_Uint32 entity, NW_String_t* str, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_byteToString

    @synopsis:    Convert byte array to string.

    @scope:       public

    @parameters:
       [in] NW_Byte* s
                  Bytes to initialize string with.

       [in-out] NW_String_t* str
                  The string.

       [in] NW_Uint32 encoding
                  The encoding.

    @description: Convert byte to string.

    @returns:     NW_Status_t
                  The status of the operation.

       [NW_STAT_SUCCESS]
                  String initialized from bytes.

       [NW_STAT_FAILURE]
                  String not initialized.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_String_byteToString(NW_Byte* s, NW_String_t* str, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_tokenToString

    @synopsis:    Token to string.

    @scope:       public

    @parameters:
       [in] NW_Uint32 token
                  default

       [in-out] NW_String_t* str
                  The string.

       [in] NW_Uint32 encoding
                  default

    @description: Token to string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String converted.

       [NW_STAT_FAILURE]
                  Token not found in current dictionary.

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Unsupported charset.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_String_tokenToString(NW_Uint32 token, NW_String_t* str, NW_Uint32 encoding);

/* ----------------------------------------------------------------------- **
   String manipulation functions
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_String_copy

    @synopsis:    Copy string duplicating storage.

    @scope:       public

    @parameters:
       [out] NW_String_t* dest
                  The destination string.

       [in] NW_String_t* source
                  The source string.

    @description: If the source owns the storage (i.e. it can modify and/or 
                  delete it) than performs a deep copy. This duplicates the 
                  storage. Otherwise it performs a shallow copy.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String copied.
  
       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_String_copy(NW_String_t* dest, NW_String_t* source);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_shallowCopy

    @synopsis:    Copies without duplicating storage.

    @scope:       public

    @parameters:
       [out] NW_String_t* dest
                  The destination string.

       [in] NW_String_t* source
                  The source string.

    @description: Performs a shallow copy. The storage is shared with 
                  the copy, not duplicated.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_String_shallowCopy(NW_String_t* dest, NW_String_t* source);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_deepCopy

    @synopsis:    Copy string duplicating storage.

    @scope:       public

    @parameters:
       [out] NW_String_t* dest
                  The destination string.

       [in] NW_String_t* source
                  The source string.

    @description: Performs a deep copy. The storage is duplicated so it 
                  is not shared with the original.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String copied.
  
       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_String_deepCopy(NW_String_t* dest, NW_String_t* source);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_concatenate

    @synopsis:    Concatenates two strings together.

    @scope:       public

    @parameters:
       [in-out] NW_String_t* dest
                  The destination string.

       [in] NW_String_t* source
                  The source string.

       [in] NW_Uint32 encoding
                  Encoding both strings are assumed to have.

    @description: Concatenated two strings together.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String copied.
  
       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_String_concatenate(NW_String_t* dest, NW_String_t* source, NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_findChar

    @synopsis:    Finds char in string.

    @scope:       public

    @parameters:
       [in] NW_String_t* string
                  The string.

       [in] NW_Int32 character
                  Character to find.

    @description: Find char in string.

    @returns:     NW_Byte*
                  Pointer to character if found, otherwise NULL.

 ** ----------------------------------------------------------------------- **/
NW_Byte*
NW_String_findChar(NW_String_t* string, NW_Int32 character, NW_Uint32 encoding);


/* ----------------------------------------------------------------------- **
   Functions
** ----------------------------------------------------------------------- **/

/** ----------------------------------------------------------------------- **
    @function:    NW_String_ucs2CharInit

    @synopsis:    Initialize UCS2 characters.

    @scope:       public

    @parameters:
       [out] NW_Ucs2** data
                  Allocated storage.

       [in] NW_Uint32 length
                  The length.

    @description: Allocates enough storage to hold the specified number 
                  of characters.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String copied.
  
       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_String_ucs2CharInit(NW_Ucs2** data, NW_Uint32 length);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_ucs2CharLength

    @synopsis:    Get character length.

    @scope:       public

    @parameters:
       [in] NW_Ucs2* data
                  Buffer to count.

    @description: Get character length.

    @returns:     NW_Int32
                  Length of buffer in characters.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Int32 
NW_String_ucs2CharLength (NW_Ucs2* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_byteToUCS2Char

    @synopsis:    Convert byte to UCS2 characters.

    @scope:       public

    @parameters:
       [in] NW_Byte* b
                  default

       [in] NW_Uint32 length
                  default

       [out] NW_Ucs2** data
                  default

    @description: Given an arbitrary NW_Byte array with no known character 
                  encoding, convert it to a NW_Ucs2 string. Actually it 
                  copies the bytes and assumes the source is UCS2 encoding. 

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Bytes copied.
  
       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_String_byteToUCS2Char (NW_Byte* b, NW_Uint32 length, NW_Ucs2** data);


/** ----------------------------------------------------------------------- **
    @function:    NW_String_entityToUCS2Char

    @synopsis:    Convert entity to UCS2 character.

    @scope:       public

    @parameters:
       [in] NW_Uint32 entity
                  default

       [out] NW_Ucs2** data
                  default

    @description: Convert entity to UCS2 character.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Bytes copied.
  
       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_String_entityToUCS2Char(NW_Uint32 entity, NW_Ucs2** data);

/*
 * Given an arbitrary NW_Byte array with no known character encoding, 
 * convert it to a NW_Ucs2 string.
 *
 */
/* Not used. 8/14/01 */
/*
NW_Status_t 
NW_String_tokenToUCS2Char(NW_Uint32 token, NW_Ucs2** data);
*/

/** ----------------------------------------------------------------------- **
    @function:    NW_String_stringToUCS2Char

    @synopsis:    default

    @scope:       public

    @parameters:
       [in] const string
                  default

       [in] NW_Uint32 encoding
                  default

       [out] NW_Ucs2 **data
                  default

    @description: default

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Bytes copied.
  
       [NW_STAT_FAILURE]
                  No bytes to copy.
  
       [NW_STAT_OUT_OF_MEMORY]
                  Ran out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_String_stringToUCS2Char(const NW_String_t* string, NW_Uint32 encoding, NW_Ucs2 **data);

/** ----------------------------------------------------------------------- **
    @function:    NW_String_deleteStorage

    @synopsis:    Delete storage.

    @scope:       public

    @parameters:
       [in-out] NW_String_t* s
                  The string.

    @description: Delete storage if user owns it otherwise does nothing.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_String_deleteStorage(NW_String_t* s);

#ifdef __cplusplus
} /*extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_STRING_STRING_H */
