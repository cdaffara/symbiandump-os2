/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef _NW_ENCODER_WRITER_HEADER_GUARD_
#define _NW_ENCODER_WRITER_HEADER_GUARD_

#include <xml/cxml/nw_encoder_stringtable.h>
#include <xml/cxml/nw_encoder_wbxmlwriter.h>
#include <xml/cxml/nw_dom_attribute.h>
#include <xml/cxml/nw_dom_element.h>
#include <xml/cxml/nw_dom_text.h>
#include <xml/cxml/nw_dom_document.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#define MEMORY_INCREMENT 100

/** ----------------------------------------------------------------------- **
    @struct:      NW_Encoder_Encoder

    @synopsis:    Encoder structure.

    @scope:       public
    @variables:
       NW_WBXML_Writer_t writer
                  The writer.

       NW_Uint32 encoding
                  Current encoding.

       NW_Encoder_StringTable_t* table
                  The string table.

    @description: Encoder structure.
 ** ----------------------------------------------------------------------- **/
typedef struct NW_Encoder_Encoder_s{
  NW_WBXML_Writer_t writer;
  NW_Uint32 encoding;
  NW_Encoder_StringTable_t* stringTable;
  /* Some servers (for example, Wireless Villiage Oz as of January 2003)
     don't parse the string table references so this switch controls use of
     the string table during WBXML generation.  Set TRUE for normal WBXML.
     Set FALSE to prevent building a string table. */
  NW_Bool enableStringTable;
} NW_Encoder_Encoder_t;

/* Deprecated - here for backward compatibility */
typedef struct NW_Encoder_Encoder_s NW_Encoder_t;


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_encodeWBXML

    @synopsis:    Encodes the document represented by the given
                  NW_DOM_DocumentNode_t.

    @scope:       public

    @parameters:
       [in] NW_Encoder_Encoder_t* encoder
                  The encoder.

       [in] NW_DOM_DocumentNode_t* docNode
                  Document node for the document to be encoded.

       [in] NW_Bool enableStringTableUse
                  Set to TRUE for normal WBXML.
                  Set to FALSE to prevent use of the string table.

       [out] NW_Uint32* length
                  Length of the encoded buffer.

       [out] NW_Byte** buffer
                  The encoded buffer.

    @description: Encodes the document represented by the given
                  NW_DOM_DocumentNode_t.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Document encoded.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter is NULL.

       [NW_STAT_OUT_OF_MEMORY]
                  Memory could not be allocated for buffer.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Internal error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_Encoder_encodeWBXML(NW_Encoder_Encoder_t* encoder,
                       NW_DOM_DocumentNode_t* docNode,
                       NW_Bool enableStringTableUse,
                       NW_Uint32* length,
                       NW_Byte** buffer);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif
