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
    @package:     NW_Encoder

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_WBXMLWRITER_H
#define NW_WBXMLWRITER_H

#include <xml/cxml/nw_wbxml_dictionary.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


struct NW_WBXML_Writer_s;

/* ----------------------------------------------------------------------- **
   If the string is in the table, set *pFound = NW_TRUE, else set
   *pFound = NW_FALSE. Return a failure code only if something else goes wrong
   (e.g., out of memory)
** ----------------------------------------------------------------------- **/
typedef
NW_Status_t (*NW_WBXML_Writer_GetStringTableOffset_t)(void* pStringTableObject,
                                                      NW_Uint32 byteCount,
                                                      const NW_Uint8* pString,
                                                      NW_Bool* pFound,
                                                      NW_Uint32* pTableOffset);

/* ----------------------------------------------------------------------- **
   If for some reason the string can't be added to the table,
   return a failure status code.
** ----------------------------------------------------------------------- **/
typedef
NW_Status_t (*NW_WBXML_Writer_AddToStringTable_t)(void* pStringTableObject,
                                                  NW_Uint32 byteCount,
                                                  const NW_Uint8* pString,
                                                  NW_Uint32* pTableOffset);

/* at end of string table return byteCount = 0 */
typedef
NW_Status_t (*NW_WBXML_Writer_StringTableIterateInit_t)(void* pStringTableObject,
                                                        void* pStringTableIterator);

/* at end of string table return byteCount = 0 */
typedef
NW_Status_t (*NW_WBXML_Writer_StringTableIterateNext_t)(void* pStringTableIterator,
                                                        NW_Uint32* pByteCount,
                                                        NW_Uint8** ppBuf);

/* the callback is responsible for updating the writer structure
   byteCount and pBuf */
typedef
NW_Status_t (*NW_WBXML_Writer_GrowBuf_t)(struct NW_WBXML_Writer_s*,
                                         NW_Uint32 writeLength);


/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_Writer

    @synopsis:    The WBXML writer structure.

    @scope:       public
    @variables:
       NW_Uint32 index
                  Read/write position.

       NW_Uint32 byteCount
                  Allocated length

       NW_Uint8* pBuf
                  The buffer.

       NW_WBXML_Writer_GrowBuf_t growBufCallback
                  Callback.

       NW_WBXML_Dictionary_t* pTagDictionary
                  The tag dictionary.

       NW_WBXML_Dictionary_t* pAttributeDictionary
                  The attribute dictionary.

       NW_WBXML_Writer_GetStringTableOffset_t getStringTableOffset
                  Callback.

       NW_WBXML_Writer_AddToStringTable_t addToStringTable
                  Callback

       void* pStringTableObject
                  The string table.

       NW_WBXML_Writer_StringTableIterateInit_t stringTableIterateInit
                  llback.

       NW_WBXML_Writer_StringTableIterateNext_t stringTableIterateNext
                  Callback.

       NW_Uint8 tagCodePage
                  The tag code page.

       NW_Uint8 attributeCodePage
                  The attribute code page.

       NW_Bool sizing
                  If sizing != 0 then don't write to buffer. Increment
                  index as though writing so that final value of index
                  indicates the required byteCount for allocating buffer.

    @description: The WBXML writer structure. Contains all the callbacks
                  as well as cursor and dictionary data.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_Writer_s {
    NW_Uint32 index;     /* a read/write position */
    NW_Uint32 byteCount; /* allocated length */
    NW_Uint8* pBuf;
    NW_WBXML_Writer_GrowBuf_t growBufCallback;

    NW_WBXML_Dictionary_t* pTagDictionary;
    NW_WBXML_Dictionary_t* pAttributeDictionary;

    NW_WBXML_Writer_GetStringTableOffset_t getStringTableOffset;
    NW_WBXML_Writer_AddToStringTable_t addToStringTable;
    void* pStringTableObject;

    NW_WBXML_Writer_StringTableIterateInit_t stringTableIterateInit;
    NW_WBXML_Writer_StringTableIterateNext_t stringTableIterateNext;

    NW_Uint8 tagCodePage;
    NW_Uint8 attributeCodePage;

    /* if sizing != 0 then don't write to buf, increment index as
    though writing so that final value of index indicates the
    required byteCount for allocating buf */
    NW_Bool sizing;
    // WLIU_DEBUG: NW_Int32 cp_count;
} NW_WBXML_Writer_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_Initialize

    @synopsis:    Initializes the writer for use.

    @scope:       public

    @parameters:
       [in-out] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 byteCount
                  Allocated length.

       [in] NW_Uint8* pBuf
                  The buffer.

       [in] NW_WBXML_Writer_GrowBuf_t growBufCallback
                  Callback.

       [in] NW_WBXML_Dictionary_t* pTagDictionary
                  Tag dictionary.
       [in] NW_WBXML_Dictionary_t* pAttributeDictionary
                  Attribute dictionary.

       [in] NW_WBXML_Writer_GetStringTableOffset_t getStringTableOffset
                  Callback.

       [in] NW_WBXML_Writer_AddToStringTable_t addToStringTable
                  Callback.

       [in] void* pStringTableObject
                  The string table.

       [in] NW_WBXML_Writer_StringTableIterateInit_t stringTableIterateStart
                  Callback.

       [in] NW_WBXML_Writer_StringTableIterateNext_t stringTableIterateNext
                  Callback.

       [in] NW_Bool sizing
                  If sizing != 0 then don't write to buffer. Increment
                  index as though writing so that final value of index
                  indicates the required byteCount for allocating buffer.

    @description: It is OK to use byteCount==0 and pBuf==NULL if you
                  have a growBufCallback or if no allocation is needed
                  because sizing==1.  If you do a sizing pass and then
                  allocate and set pBuf you can also set
                  growBufCallback==NULL as no additional memory should
                  be required. No malloc's or free's are in the WBXML
                  writer code.  If you have no string table then null
                  the various string table related pointers.
                  Client should free the writer object when finished.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
void
NW_WBXML_Writer_Initialize(NW_WBXML_Writer_t* pW,
                           NW_Uint32 byteCount, NW_Uint8* pBuf,
                           NW_WBXML_Writer_GrowBuf_t growBufCallback,
                           NW_WBXML_Dictionary_t* pTagDictionary,
                           NW_WBXML_Dictionary_t* pAttributeDictionary,
                           NW_WBXML_Writer_GetStringTableOffset_t getStringTableOffset,
                           NW_WBXML_Writer_AddToStringTable_t addToStringTable,
                           void* pStringTableObject,
                           NW_WBXML_Writer_StringTableIterateInit_t stringTableIterateStart,
                           NW_WBXML_Writer_StringTableIterateNext_t stringTableIterateNext,
                           NW_Bool sizing);

#define NW_WBXML_Writer_GetSize(pW) ((pW)->index)

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_SetToSizing

    @synopsis:    Initialize writer for sizing (no write).

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

    @description: Initialize writer for sizing (no write).

 ** ----------------------------------------------------------------------- **/
void
NW_WBXML_Writer_SetToSizing(NW_WBXML_Writer_t* pW);

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_SetToWrite

    @synopsis:    Initialize writer for writing.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 byteCount
                  Allocated buffer length

       [in] NW_Uint8* pBuf
                  Allocated buffer.

    @description: Initialize writer for writing.

 ** ----------------------------------------------------------------------- **/
void
NW_WBXML_Writer_SetToWrite(NW_WBXML_Writer_t* pW,
                           NW_Uint32 byteCount, NW_Uint8* pBuf);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_Header

    @synopsis:    Write the document header.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint8 WBxmlVersion
                  The version.

       [in] NW_Uint32 publicIdentifier
                  The public identifier.

       [in] NW_Uint32 charsetMIBEnum
                  The character set.

       [in] NW_Uint32 stringTableByteCount
                  Size of string table.

    @description: Write the document header. The string table byte count
                  will be 0 if there is no string table.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Header written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Can't allocate memory to write header.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_Header(NW_WBXML_Writer_t* pW, NW_Uint8 WBxmlVersion,
                       NW_Uint32 publicIdentifier, NW_Uint32 charsetMIBEnum,
                       NW_Uint32 stringTableByteCount);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_TagSetContentFlag

    @synopsis:    Set content flag.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 index
                  Offset into buffer to find tag flag.

    @description: If "index" points to a tag token, then this will
                  set content flags.  Capture a tag index through the
                  pTagIndex argument to NW_WBXML_Writer_Tag.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Flag set.

       [NW_STAT_FAILURE]
                  General error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_TagSetContentFlag(NW_WBXML_Writer_t* pW, NW_Uint32 index);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_TagClearContentFlag

    @synopsis:    Clear content flag.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 index
                  Offset into buffer to find tag flag.

    @description: If "index" points to a tag token, then this will
                  clear content flags.  Capture a tag index through
                  the pTagIndex argument to NW_WBXML_Writer_Tag.

    @description: Clear content flag.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Flag set.

       [NW_STAT_FAILURE]
                  General error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_TagClearContentFlag(NW_WBXML_Writer_t* pW, NW_Uint32 index);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_TagSetAttributesFlag

    @synopsis:    Set attribute flag.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 index
                  Index into buffer to find flag.

    @description: If "index" points to a tag token, then this will
                  set attribute flags.  Capture a tag index through
                  the pTagIndex argument to NW_WBXML_Writer_Tag.

    @description: Set attribute flag.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Flag set.

       [NW_STAT_FAILURE]
                  General error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_TagSetAttributesFlag(NW_WBXML_Writer_t* pW, NW_Uint32 index);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_TagClearAttributesFlag

    @synopsis:    Clear attributes flag.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  Thw writer.

       [in] NW_Uint32 index
                  Offset into buffer to find flag.

    @description: If "index" points to a tag token, then this will
                  clear attribute flags.  Capture a tag index through
                  the pTagIndex argument to NW_WBXML_Writer_Tag.

    @description: Clear attributes flag.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Flag cleared.

       [NW_STAT_FAILURE]
                  General error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Writer_TagClearAttributesFlag(NW_WBXML_Writer_t* pW, NW_Uint32 index);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_TagToken

    @synopsis:    Extracts token and writes it to buffer.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint16 fqToken
                  The fully qualified token.

       [in] NW_Uint32* pTagIndex
                  Index of tag into buffer.

    @description: Extracts token and writes it to buffer.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Flag cleared.

       [NW_STAT_FAILURE]
                  General error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Writer_TagToken(NW_WBXML_Writer_t* pW, NW_Uint16 fqToken,
                         NW_Uint32* pTagIndex);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_TagString

    @synopsis:    Writes tag string.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint32 charCount
                  Character count. It should not include null termination
                  character if any exists.

       [in] NW_Uint32 byteCount
                  Byte count.

       [in] NW_Uint8* pBuf
                  Buffer containing characters.

       [out] NW_Uint32* pTagIndex
                   Contains the index to the tag token in the WBXML

    @description: This function tries the following in sequence as required:
                  A. look in the dictionary for the string's token
                  B. look in the string table
                  C. look for reserved name "zzzunknown" token in the tag dictionary
                  If all fail, then the function returns NW_STAT_FAILURE.
                  On return, *pTagIndex contains the index to the tag token in the WBXML so
                  you can subsequently toggle the content and attribute flags.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Token found.

       [NW_STAT_FAILURE]
                  Could not find token.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_TagString(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                          NW_Uint32 charCount, NW_Uint32 byteCount,
                          NW_Uint8* pBuf, NW_Uint32* pTagIndex);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_AttributeAndValue

    @synopsis:    Write attrbute and value.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint32 nameCharCount
                  Name character count.

       [in] NW_Uint8* pName
                  Attribute name.

       [in] NW_Uint32 valueCharCount
                  Value character count.

       [in] NW_Uint32 valueByteCount
                  Value byte count.

       [in] NW_Uint8* pValue
                  Value name.

    @description: A convenience function for when attribute name and
                  value are given as text. The character count should not
                  include NULL termination character if any exists  If there
                  is an attribute name but no value use valueCharCount==0,
                  valueByteCount==0 and pValue==NULL

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute and value written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_AttributeAndValue(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                                  NW_Uint32 nameCharCount, NW_Uint8* pName,
                                  NW_Uint32 valueCharCount,
                                  NW_Uint32 valueByteCount,
                                  NW_Uint8* pValue);

IMPORT_C
NW_Status_t
NW_WBXML_Writer_AttributeAndValue2(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                                  NW_Uint32 nameCharCount, NW_Uint8* pName,
                                  NW_Uint32 valueCharCount,
                                  NW_Uint32 valueByteCount,
                                  NW_Uint8* pValue,
                                  NW_Uint32* cp_count);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_AttributeToken

    @synopsis:    Write attribute token.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint16 fqToken
                  The fully qualified token.

    @description: Write attribute token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute and value written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Writer_AttributeToken(NW_WBXML_Writer_t* pW, NW_Uint16 fqToken);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_AttributeNameString

    @synopsis:    Write attribute name token using string.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint32 nameCharCount
                  Character count of name. Should not include null termination
                  character if any exists.

       [in] NW_Uint32 nameByteCount
                  Byte count of name.

       [in] NW_Uint8* pName
                  The name.

    @description: Write attribute name token using string.
                  This function tries the following in sequence as required:
                  A. look in the dictionary for the string's token
                  B. look in the string table
                  C. look for reserved name "zzzunknown" token in the attribute dictionary
                  If all fail, then the function returns NW_STAT_FAILURE.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute and value written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Writer_AttributeNameString(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                                    NW_Uint32 nameCharCount,
                                    NW_Uint32 nameByteCount, NW_Uint8* pName);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_Entity

    @synopsis:    Writes entity and its token.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 entity
                  The entity.

    @description: Writes entity and its token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute and value written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Writer_Entity(NW_WBXML_Writer_t* pW,
                       NW_Uint32 entity);

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_ExtensionUseStringTable

    @synopsis:    Writes EXT_T_[0,1,2] extension forms only and uses
                  the string table to hold the string.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint16 fqToken
                  The fully qualified token.

       [in] NW_Uint32 byteCount
                  byte count for string buffer including null termination

       [in] NW_Uint8* pBuf
                  pointer to the null terminated string data

    @description: This is a helper function for the basic extension writing
                  function for the particular case of EXT_T_[0,1,2] tokens
                  where the string must be put in the string table.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Entity written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Writer_ExtensionUseStringTable(NW_WBXML_Writer_t* pW,
                                        NW_Uint16 fqToken,
                                        NW_Uint32 byteCount,
                                        NW_Uint8* pBuf);

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_Extension

    @synopsis:    Writes extension.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint16 fqToken
                  The fully qualified token.

       [in] NW_Uint32 anonymousValue
                  Optional extension value.

       [in] NW_Uint32 byteCount
                  Optional extension byte count.

       [in] NW_Uint8* pBuf
                  Optional extension text.

    @description: There are three type of extensions:
                  1. one of three possible single byte tokens
                     NW_WBXML_EXT_[0,1,2]
                  2. an extension token followed by a multibyte encoded NW_Uint32 value
                     NW_WBXML_EXT_T_[0,1,2] multibyte(anonymousValue)
                  3. an extension token followed by an in-line string
                     NW_WBXML_EXT_I_[0,1,2] null-terminated-string

                  This function handles all three cases so you have to pass the
                  appropriate arguments for each case. For each fqToken should be
                  one of NW_WBXML_EXT_[0,1,2] (with any page value):

                  1. anonymousValue, byteCount and pBuf should be 0 or NULL

                  2. anonymousValue should be the value to multibyte encode
                     and byteCount and pBuf should be 0 and NULL

                  3. byteCount should be the byte length of the null terminated string
                     pointed to by pBuf and anonymousValue is ignored

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Entity written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_WBXML_Writer_Extension(NW_WBXML_Writer_t* pW,
                          NW_Uint16 fqToken,
                          NW_Uint32 anonymousValue,
                          NW_Uint32 byteCount,
                          NW_Uint8* pBuf);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_Opaque

    @synopsis:    Writes opaque data.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 byteCount
                  Length of opaque.

       [in] NW_Uint8* pBuf
                  Opaque data.

    @description: Writes opaque data.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Opaque written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_Opaque(NW_WBXML_Writer_t* pW, NW_Uint32 byteCount,
                       NW_Uint8* pBuf);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_Text

    @synopsis:    Write text or reference.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint32 encoding
                  The encoding.

       [in] NW_Uint32 byteCount
                  The text byte count.

       [in] const NW_Uint8* pText
                  The text.

    @description: If string is already in the string table, this writes
                  a reference otherwise it writes an in-line string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Text written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_Text(NW_WBXML_Writer_t* pW, NW_Uint32 encoding,
                     NW_Uint32 byteCount, const NW_Uint8* pText);

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_PI

    @synopsis:    Writes the beginning PI token.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

    @description: Just writes the beginning PI token.  The remainder of
                  the PI must be written just like an attribute.
                  The grammar is:
                       PI attributeName [attributeValue...] END

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                   Process instruction written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_PI(NW_WBXML_Writer_t* pW);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Writer_End

    @synopsis:    Writes end of element, end of attribute list or end of PI

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  default

    @description: Writes end of element, end of attribute list or end of PI.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  End written.

       [NW_STAT_FAILURE]
                  General error.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory for write.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Status_t
NW_WBXML_Writer_End(NW_WBXML_Writer_t* pW);


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_WBXMLWRITER_H */
