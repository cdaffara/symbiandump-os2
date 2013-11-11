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
    @package:     NW_WBXML

    @synopsis:    default

    @description: Definitions for WBXML document header

 ** ----------------------------------------------------------------------- **/

#ifndef NW_PARSER_WBXML_DOCUMENT_H
#define NW_PARSER_WBXML_DOCUMENT_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_string_string.h>
#include <xml/cxml/nw_encoder_stringtable.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @struct:      NW_WBXML_StringTable

    @synopsis:    WBXML document string table.

    @scope:       public
    @variables:
       NW_Uint32 length
                  Length of table.

       NW_Byte* data
                  The table.

    @description: WBXML document string table.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_StringTable_s {
  NW_Uint32 length;
  NW_Byte* data;
}NW_WBXML_StringTable_t;


/** ----------------------------------------------------------------------- **
    @struct:       NW_WBXML_Document

    @synopsis:    The document structure that describes the header of the 
                  current document.

    @scope:       public
    @variables:
       NW_Uint8 version
                  The version.

       NW_Uint32 publicid
                  The public ID.

       NW_String_t* doc_type
                  The document type.

       NW_Uint32 default_public_id
                  The public ID.

       NW_Uint32 charset
                  The charset (i.e. encoding)

       struct strtbl
                  WBXML document string table.

       NW_Uint32 body_len
                  Length of document.

       NW_Encoder_StringTable_t* strtbl_extension
                  Extension string table.

    @description: The document structure.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_WBXML_Document_s {
  NW_Uint8 version;
  NW_Uint32 publicid;
  NW_String_t* doc_type;
  NW_Uint32 default_public_id;
  NW_Uint32 charset;
  struct NW_WBXML_StringTable_s strtbl;
  NW_Uint32 body_len;
  /* When a document is modified, the extended table is created. */
  /* check what its being used for */
  NW_Encoder_StringTable_t* strtbl_extension;
} NW_WBXML_Document_t;



/* ----------------------------------------------------------------------- **
   Document methods 
** ----------------------------------------------------------------------- **/


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Document_construct

    @synopsis:    Constructor.

    @scope:       public

    @parameters:
       [out] NW_WBXML_Document_t* doc
                  The document

       [in] NW_Uint32 default_public_id
                  The public ID.

    @description: Constructor.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Document constructed.

       [NW_STAT_FAILURE]
                  Document not constructed.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_WBXML_Document_construct(NW_WBXML_Document_t* doc,
                            NW_Uint32 default_public_id);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Document_destruct

    @synopsis:    Destructor.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Document_t* doc
                  The document.

    @description: Destructor.
 ** ----------------------------------------------------------------------- **/
IMPORT_C void 
NW_WBXML_Document_destruct (NW_WBXML_Document_t* doc);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Document_getTableString

    @synopsis:    Get table string using index.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Document_t* doc
                  default

       [in] NW_Uint32 index
                  default

       [out] NW_String_t* s
                  default

    @description: Get table string using index.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Table returned.

       [NW_STAT_WBXML_ERROR_BYTECODE]
                  Can't find table.

       [NW_STAT_WBXML_ERROR_CHARSET_UNSUPPORTED]
                  Encoding not supported.

       [NW_STAT_FAILURE]
                  General error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_WBXML_Document_getTableString(NW_WBXML_Document_t* doc, 
                                 NW_Uint32 index, NW_String_t* s);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Document_putTableString

    @synopsis:    Store table string returning index.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Document_t* doc
                  The document.

       [in] NW_String_t* string
                  Addition to table.

       [out] NW_Uint32* index
                  Index of last entry in table after addition.

    @description: String table write method.  Writing a dom tree may
                  require adding strings to the string table, if one
                  exists, or creating a string table if none exists.
                  We create an extended table using the encoder
                  string table write methods if this is
                  necessary. Unfortunately, there can be some
                  duplication of strings between the two tables, because
                  there is no efficient way to look up a string by name
                  in the real string table. Therefore, any string that
                  gets added to the table gets added to the extension
                  table.  The encoder takes care of rationalizing the
                  two tables if the document gets rewritten as wbxml.
                  If the string is not already in the table, this method
                  adds it. The out parameter index returns the index of
                  the string in the extension table, offset by the size
                  of the real string table so that extension table
                  strings always have indexes bigger than real table
                  strings.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Table added.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter is NULL.

       [NW_STAT_OUT_OF_MEMORY]
                  Out of memory.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_WBXML_Document_putTableString(NW_WBXML_Document_t* doc, 
                                 NW_String_t* string, 
                                 NW_Uint32* index);

/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Document_getVersion

    @synopsis:    Get document version.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Document_t* document
                  The document.

    @description: Gets the WBXML version number.

    @returns:     NW_Uint8
                  The WBXML version number.

 ** ----------------------------------------------------------------------- **/
NW_Uint8
NW_WBXML_Document_getVersion(NW_WBXML_Document_t* document);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Document_getPublicID

    @synopsis:    Get document publicID.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Document_t* document
                  The document.

    @description: Gets the WBXML public ID. 

    @returns:     NW_Uint32
                  The WBXML public ID.

 ** ----------------------------------------------------------------------- **/
NW_Uint32
NW_WBXML_Document_getPublicID(NW_WBXML_Document_t* document);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Document_getDocType

    @synopsis:    Get document type.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Document_t* document
                  The document.

    @description: Get document type.

    @returns:     NW_String_t*
                  The address of the document type string stored in the 
                  document header. The caller must not free this string. 

 ** ----------------------------------------------------------------------- **/
NW_String_t*
NW_WBXML_Document_getDocType(NW_WBXML_Document_t* document);


/** ----------------------------------------------------------------------- **
    @function:    NW_WBXML_Document_getEncoding

    @synopsis:    Get document encoding.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Document_t* document
                  The document.

    @description: Gets the string encoding used in a document.

    @returns:     NW_Uint32
                  One of the string encoding constants defined in the 
                  WBXML specification. The XML engine defines the following
                  constants.

 ** ----------------------------------------------------------------------- **/
NW_Uint32
NW_WBXML_Document_getEncoding(NW_WBXML_Document_t* document);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_PARSER_WBXML_DOCUMENT_H */
