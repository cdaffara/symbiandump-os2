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
    @package:     NW_XML

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/

#ifndef NW_XMLP2WBXML_H
#define NW_XMLP2WBXML_H

#include <xml/cxml/nw_xmlp_xmlreader.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

/** ----------------------------------------------------------------------- **
    @function:    NW_Xml_Text2WbxmlEncoder_New

    @synopsis:    Constructor.

    @scope:       public

    @parameters:
       [in] NW_Uint32 publicID
                  The public ID.

       [in] NW_Uint32 encoding
                  The encoding.

       [out] void** ppV
                  The returned structure.

    @description: Constructor.

    @returns:     NW_Status_t
                  Status of operation.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_XML_Text2WbxmlEncoder_New(NW_Uint32 publicID, NW_Uint32 encoding, void** ppV);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_XmlpWbxmlEncoder_Delete

    @synopsis:    Destructor.

    @scope:       public

    @parameters:
       [in] void* pV
                  The encoder.

    @description: Destructor.
 ** ----------------------------------------------------------------------- **/
void
NW_XML_XmlpWbxmlEncoder_Delete(void* pV);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_ComputeEncoding

    @synopsis:    Compute encoding.

    @scope:       public

    @parameters:
       [in] NW_Uint32 length
                  Length of buffer.

       [in] const unsigned char* pBuf
                  Buffer.

       [out] NW_Uint32* pEncoding
                  The calculated encoding.

       [out] NW_XML_Endianness_t* pEndianness
                  The calculated endianness.

    @description: Compute encoding.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Endianness and encoding determined.

       [NW_STAT_FAILURE]
                  Invalid length. Must be >= 4 to determine encoding.

 ** ----------------------------------------------------------------------- **/
IMPORT_C 
NW_Status_t
NW_XML_ComputeEncoding(NW_Uint32 length, const unsigned char* pBuf,
                             NW_Uint32* pEncoding, NW_XML_Endianness_t* pEndianness);


/** ----------------------------------------------------------------------- **
    @function:    NW_XML_XmlToWbxml

    @synopsis:    Convert buffer from Xml to Wbxml.

    @scope:       public

    @parameters:
       [in] NW_Buffer_t* pInBuf
                  The input buffer.

       [in] NW_Uint32 encoding
                  The encoding.

       [out] NW_Buffer_t** ppOutBuf
                  Converted data.

       [out] NW_Uint32* line
                  Line count.

       [in] NW_Uint32 publicID
                  The public ID.
       [out] void** WBXMLEncStrTbl:
                Stores the string table of WBXML encoder.

    @description: Convert buffer from Xml to Wbxml.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Buffer converted to WBXML.

       [NW_STAT_FAILURE]
                  Required parameter is NULL, doc has odd number of bytes
                  or no data to process.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_XML_XmlToWbxml(NW_Buffer_t* pInBuf, NW_Uint32 encoding, 
                  NW_Buffer_t** ppOutBuf, NW_Uint32* line, 
                  NW_Uint32 publicID, void** WBXMLEncStrTbl);


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_XMLP2WBXML_H */
