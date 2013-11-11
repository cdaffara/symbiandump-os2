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

    @description: Maps TinyDom types onto WBXMLWriter functions.

 ** ----------------------------------------------------------------------- **/

#ifndef NW_TINYDOM2WBXML_H
#define NW_TINYDOM2WBXML_H

#include <xml/cxml/nw_encoder_wbxmlwriter.h>
#include <xml/cxml/nw_dom_attribute.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_writeAttrVal

    @synopsis:    Write attribute.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_DOM_AttrVal_t* val
                  The attirbute value.

       [in] NW_Uint32 encoding
                  Charset as IANA MIBenum.

    @description: Write attribute.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute written.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required value is null or invalid token.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory.

       [NW_STAT_FAILURE]
                  General error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_Encoder_writeAttrVal(NW_WBXML_Writer_t* pW,
                        NW_DOM_AttrVal_t *val,
                        NW_Uint32 encoding);

/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_writeAttributeByToken

    @synopsis:    Write attribute specified token.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint16 attrFqToken
                  The fully qualified token.

       [in] NW_TinyDom_AttrVal_t* val
                  Value to write.

       [in] NW_Uint32 encoding
                  Encoding to use.

    @description: Write attribute specified token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute written.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required value is null or invalid token.

       [NW_STAT_OUT_OF_MEMORY]
                  Couldn't allocate memory.

       [NW_STAT_FAILURE]
                  General error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_Encoder_writeAttributeByToken(NW_WBXML_Writer_t* pW,
                                 NW_Uint16 attrFqToken,
                                 NW_TinyDom_AttrVal_t* val,
                                 NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_writeAttributeByName

    @synopsis:    Writes an Attribute by name plus AttrVal.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_String_t* attrName
                  The attribute name to use.

       [in] NW_TinyDom_AttrVal_t* val
                  The value to write.

       [in] NW_Uint32 encoding
                  The encoding to use.

    @description: Writes an Attribute by name plus AttrVal.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute written.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required value is NULL or invalid.

       [NW_STAT_FAILURE]
                  Not found.

       [NW_STAT_OUT_OF_MEMORY]
                  Memory couldn't be allocated.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_Encoder_writeAttributeByName(NW_WBXML_Writer_t* pW,
                                NW_String_t* attrName,
                                NW_TinyDom_AttrVal_t* val,
                                NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_writeElementByToken

    @synopsis:    Write element by token.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_Uint16 elementToken
                  The token to write.

    @description: Write element by token.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute written.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required value is NULL or invalid.

       [NW_STAT_FAILURE]
                  Not found.

       [NW_STAT_OUT_OF_MEMORY]
                  Memory couldn't be allocated.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_Encoder_writeElementByToken(NW_WBXML_Writer_t* pW,
                               NW_Uint16 elementToken);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_writeElementByName

    @synopsis:    Write element by name.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_String_t* elementName
                  Name of element to write.

       [in] NW_Uint32 encoding
                  Encoding to use.

    @description: Write element by name.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute written.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required value is NULL or invalid.

       [NW_STAT_FAILURE]
                  Not found.

       [NW_STAT_OUT_OF_MEMORY]
                  Memory couldn't be allocated.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_Encoder_writeElementByName(NW_WBXML_Writer_t* pW,
                              NW_String_t* elementName,
                              NW_Uint32 encoding);


/** ----------------------------------------------------------------------- **
    @function:    NW_Encoder_writeText

    @synopsis:    Write text.

    @scope:       public

    @parameters:
       [in] NW_WBXML_Writer_t* pW
                  The writer.

       [in] NW_TinyDom_Text_t* text
                  default

       [in] NW_Uint32 encoding
                  default

    @description: Write text.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute written.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required value is NULL or invalid.

       [NW_STAT_FAILURE]
                  Not found.

       [NW_STAT_OUT_OF_MEMORY]
                  Memory couldn't be allocated.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_Encoder_writeText(NW_WBXML_Writer_t* pW,
                     NW_TinyDom_Text_t* text,
                     NW_Uint32 encoding);

#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

/* NW_TINYDOM2WBXML_H */
#endif
