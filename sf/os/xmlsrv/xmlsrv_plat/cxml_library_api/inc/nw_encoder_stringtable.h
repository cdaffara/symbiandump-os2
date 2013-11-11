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

#ifndef _NW_ENCODER_STRING_TABLE_HEADER_GUARD_
#define _NW_ENCODER_STRING_TABLE_HEADER_GUARD_

#include <xml/cxml/nw_string_string.h>
#include <xml/cxml/nw_tinytree.h>
#include <xml/cxml/nw_wbxml_dictionary.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define CXML_MAX_STRTBL_ITEMS 150 

/** ----------------------------------------------------------------------- **
    @typedef:     NW_DOM_DocumentNode

    @synopsis:    default

    @scope:       public
    @type:        NW_TinyTree_Node_t

    @description: default
 ** ----------------------------------------------------------------------- **/
typedef NW_TinyTree_Node_t NW_DOM_DocumentNode_t;


/*
 * StringItem methods
 */

/** ----------------------------------------------------------------------- **
    @struct:      NW_Encoder_StringTable

    @synopsis:    Container for the strings in a document.

    @scope:       public
    @variables:
      CXML_Vector_t* vector
                  Array of strings.

    @description: Container for the strings in a document.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_Encoder_StringTable_s{
  CXML_Vector_t* vector;
}NW_Encoder_StringTable_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_new

    @synopsis:    Creates a new String Table.

    @scope:       public

    @description: Creates a new String Table.

    @returns:     NW_Encoder_StringTable_t*
                  POinter to string table or NULL if not successful.

  ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Encoder_StringTable_t*
NW_Encoder_StringTable_new(void);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_delete

    @synopsis:    Deletes a String Table.

    @scope:       public

    @parameters:
       [in-out] NW_Encoder_StringTable_t* strTable
                  Table to delete.

    @description: Deletes a String Table

 ** ----------------------------------------------------------------------- **/
IMPORT_C void
NW_Encoder_StringTable_delete(NW_Encoder_StringTable_t* strTable);


/** ----------------------------------------------------------------------- **
    @struct:      NW_Encoder_StringItem

    @synopsis:    String item structure.

    @scope:       public
    @variables:
       NW_String_t data
                  The string data.

       NW_Uint16 frequency
                  Reference count.

    @description: This structures is used to count the frequency of
                  string occurance.

 ** ----------------------------------------------------------------------- **/
typedef struct NW_Encoder_StringItem_s{
  NW_String_t data;
  NW_Uint16 frequency;
} NW_Encoder_StringItem_t;


/** ----------------------------------------------------------------------- **
    @struct:       NW_Encoder_StringTableIterator

    @synopsis:    Iterator for walking through the string table.

    @scope:       public
    @variables:
       NW_Encoder_StringTable_t* strTable
                  The string table.

       NW_Uint16 currentIndex
                  Current iterator cursor.

    @description: Iterator for walking through the string table.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_Encoder_StringTableIterator_s{
  NW_Encoder_StringTable_t* strTable;
  NW_Uint16 currentIndex;
} NW_Encoder_StringTableIterator_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_new

    @synopsis:    Creates a new String Item.

    @scope:       public

    @description: Creates a new String Item.

    @returns:     NW_Encoder_StringTable_t*
                  The string table or NULL if not success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C
NW_Encoder_StringTable_t*
NW_Encoder_StringTable_new(void);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_GetSize

    @synopsis:    Returns number of strings in the string table.

    @scope:       public

    @parameters:
       [in] NW_Encoder_StringTable_t* table
                  The string table.

    @description: Returns number of strings in the string table.

    @returns:     CXML_Vector_Metric_t
                  Number of strings in the table.

 ** ----------------------------------------------------------------------- **/
IMPORT_C CXML_Vector_Metric_t
NW_Encoder_StringTable_GetSize(NW_Encoder_StringTable_t* table);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_append

    @synopsis:    Appends string to list.

    @scope:       public

    @parameters:
       [in] NW_Encoder_StringTable_t* table
                  The string table.

       [in] NW_String_t* string
                  String to add.

       [in] NW_Bool literal
                  String literal.

       [out] NW_Uint32* extensionByteOffset
                  byte offset position of inserted or found string
                  in string table

    @description: Appends new found string in the document to the list.
                  If the string was already present in the list, then it
                  increases its frequency (e.g. reference count), otherwise
                  it creates a new StringItem in the list.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Appended or found.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter is NULL.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_Encoder_StringTable_append(NW_Encoder_StringTable_t* table,
                              NW_String_t* string,
                              NW_Bool literal,
                              NW_Uint32* extensionByteOffset);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_find

    @synopsis:    Find a string in the table.

    @scope:       public

    @parameters:
       [in] NW_Encoder_StringTable_t* strTable
                  The string table.

       [in] NW_String_t* string
                  default

       [out] NW_Uint32* strIndex
                  Index of entry.

       [out] NW_Uint32* byteOffset
                  Offset of text into the table.

    @description: Finds a string in the list and returns the corresponding
                  StringItem, otherwise returns NULL.

    @returns:     NW_Encoder_StringItem_t*
                  Poiter to string item or NULL if not in the list.

 ** ----------------------------------------------------------------------- **/
NW_Encoder_StringItem_t*
NW_Encoder_StringTable_find(NW_Encoder_StringTable_t* strTable,
                            NW_String_t* string,
                            NW_Uint32* strIndex,
                            NW_Uint32* byteOffset);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_get

    @synopsis:    Finds the string at a given index.

    @scope:       public

    @parameters:
       [in] NW_Encoder_StringTable_t* strTable
                  The string table.

       [in] NW_Uint32 index
                  Index to get from.

       [out] NW_String_t* string
                  Returned string. Check status for validity.

    @description: Finds the string at a given index.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  String found.

       [NW_STAT_FAILURE]
                  String not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_Encoder_StringTable_get(NW_Encoder_StringTable_t* strTable,
                           NW_Uint32 index,
                           NW_String_t* string);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_getItemAt

    @synopsis:    Finds the string at a given index.

    @scope:       public

    @parameters:
       [in] NW_Encoder_StringTable_t* strTable
                  The string table.

       [in] NW_Uint32 index
                  Index to get from.

    @description: Finds the string at a given index.

    @returns:     NW_Encoder_StringItem_t*
                  String item at index or NULL if not found.

 ** ----------------------------------------------------------------------- **/
NW_Encoder_StringItem_t*
NW_Encoder_StringTable_getItemAt(NW_Encoder_StringTable_t* strTable,
                                 NW_Uint32 index);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_removeAt

    @synopsis:    Removes a string item.

    @scope:       public

    @parameters:
       [in] NW_Encoder_StringTable_t* strTable
                  The string table.

       [in] CXML_Vector_Metric_t index
                  Index to remove from.

    @description: Removes a string item.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Item removed.

       [NW_STAT_FAILURE]
                  Invalid index.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_Encoder_StringTable_removeAt(NW_Encoder_StringTable_t* strTable,
                                CXML_Vector_Metric_t index);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_getTotalBytes

    @synopsis:    Returns the total number of bytes used for storing all
                  the strings in the list.

    @scope:       public

    @parameters:
       [in] NW_Encoder_StringTable_t* strTable
                  The string table.

    @description: Returns the total number of bytes used for storing all
                  the strings in the list.

    @returns:     NW_Uint32
                  Total byte count.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint32
NW_Encoder_StringTable_getTotalBytes(NW_Encoder_StringTable_t* strTable);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_createFromDOM

    @synopsis:    Creates table for a given document.

    @scope:       public

    @parameters:
       [in] NW_Encoder_StringTable_t* strTable
                  The string table.

       [in] NW_DOM_DocumentNode_t* doc
                  Root node of DOM to encode.

       [in] NW_WBXML_Dictionary_t* dictionary
                  The dictionary.

    @description: Creates table for a given document.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Created.

       [NW_STAT_FAILURE]
                  No bytes found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_Encoder_StringTable_createFromDOM(NW_Encoder_StringTable_t* strTable,
                                     NW_DOM_DocumentNode_t* doc,
                                     NW_WBXML_Dictionary_t* dictionary);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_getStringTableOffset

    @synopsis:    Looks for occurrence of a string with the specified content.

    @scope:       public

    @parameters:
       [in] void* pStringTableObject
                  The string table.

       [in] NW_Uint32 byteCount
                  Length of text.

       [in] const NW_Uint8* pString
                  Text.

       [out] NW_Bool* pFound
                  True if found in table.

       [out] NW_Uint32* pTableOffset
                  Offset if found.

    @description: Looks for occurrence of a string with the specified content.
                  Creates a string with the specified storage and byte length.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_Encoder_StringTable_getStringTableOffset(void* pStringTableObject,
                                            NW_Uint32 byteCount,
                                            const NW_Uint8* pString,
                                            NW_Bool* pFound,
                                            NW_Uint32* pTableOffset);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_addToStringTable

    @synopsis:    Add string to string table.

    @scope:       public

    @parameters:
       [in-out] void* pStringTableObject
                  The string table.

       [in] NW_Uint32 byteCount
                  Length of text.

       [in] const NW_Uint8* pString
                  Text to add to table.

       [in] NW_Uint32* pTableOffset
                  Offset installed at.

    @description: Adds the string to the table if it is not already there.
                  Otherwise it increases the reference count.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Added to table.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter is NULL or invalid.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_Encoder_StringTable_addToStringTable(void* pStringTableObject,
                                        NW_Uint32 byteCount,
                                        const NW_Uint8* pString,
                                        NW_Uint32* pTableOffset);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_StringTableIterateInit

    @synopsis:    Initializes a string table iterator.

    @scope:       public

    @parameters:
       [in] void* pStringTableObject
                  The string table.

       [in-out] void* pStringTableIterator
                  Initialized iterator.

    @description: Initializes a string table iterator.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_Encoder_StringTable_StringTableIterateInit(void* pStringTableObject,
                                              void* pStringTableIterator);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_StringTable_StringTableIterateNext

    @synopsis:    Iterates through the table.

    @scope:       public

    @parameters:
       [in-out] void* pStringTableIterator
                  The iterator.

       [out] NW_Uint32* pByteCount
                  Byte count for returned value or 0.

       [out] NW_Uint8** ppBuf
                  The returned text.

    @description: Iterates through the table. At end of string table
                  return byteCount = 0.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_WBXML_ITERATE_MORE]
                  More to iterate. This value valid.

       [NW_STAT_WBXML_ITERATE_DONE]
                  Nothing more to iterate. This value not valid.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_Encoder_StringTable_StringTableIterateNext(void* pStringTableIterator,
                                              NW_Uint32* pByteCount,
                                              NW_Uint8** ppBuf);


#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif
