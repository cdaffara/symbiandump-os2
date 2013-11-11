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
    @package:     NW_DOM

    @synopsis:    default

    @description: default

 ** ----------------------------------------------------------------------- **/
#ifndef NW_DOM_WBXML_TYPES_H
#define NW_DOM_WBXML_TYPES_H

#include <xml/cxml/cxml_proj.h>
#include <xml/cxml/nw_dom_node.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @typedef:     NW_DOM_Extension

    @synopsis:    Tiny Dom Extension.

    @scope:       public
     
    @type:        NW_TinyDom_Extension_t

    @description: 
                  Tiny Dom Extension.

 ** ----------------------------------------------------------------------- **/
typedef NW_TinyDom_Extension_t NW_DOM_Extension_t;


/** ----------------------------------------------------------------------- **
    @typedef:     NW_DOM_Opaque

    @synopsis:    Opaque DOM type.

    @scope:       public
     
    @type:        NW_WBXML_Opaque_t

    @description: 
                  Opaque DOM type.

 ** ----------------------------------------------------------------------- **/
typedef NW_WBXML_Opaque_t NW_DOM_Opaque_t;


/* * Extension **/

/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Extension_new

    @synopsis:    Creates and initializes a new extension with a given 
                  token and string.

    @scope:       public

    @parameters:
       [in] NW_Uint32 token
                  Initialize with this token.

       [in] NW_String_t* str
                  Extension label.

    @description: Creates and initializes a new extension with a given 
                  token and string.

    @returns:     NW_DOM_Extension_t*
                  New extension.

 ** ----------------------------------------------------------------------- **/
NW_DOM_Extension_t*
NW_DOM_Extension_new(NW_Uint32 token, NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Extension_initialize

    @synopsis:    Initializes an extension with a token and string.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_Extension_t* ext
                  The extension.

       [in] NW_Uint32 token
                  Initialize with this token.

       [in] NW_String_t* str
                  Extension label.

    @description: Initializes an extension with a token and string.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_Extension_initialize(NW_DOM_Extension_t* ext, 
                            NW_Uint32 token, 
                            NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Extension_getString

    @synopsis:    Get string associated with this extension.

    @scope:       public

    @parameters:
       [in] NW_DOM_Extension_t* e
                  This extension.

       [out] NW_String_t* str
                  String for extension.

    @description: Get string associated with this extension.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_Extension_getString(NW_DOM_Extension_t* e, 
                           NW_String_t* str);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Extension_getToken

    @synopsis:    Returns token of extension.

    @scope:       public

    @parameters:
       [in] NW_DOM_Extension_t* e
                  This extension.

    @description: Returns token of extension.

    @returns:     NW_Uint32
                  Token or null if error.

 ** ----------------------------------------------------------------------- **/
NW_Uint32
NW_DOM_Extension_getToken(NW_DOM_Extension_t* e);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Extension_delete

    @synopsis:    Deletes this extension.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_Extension_t* e
                  This extension.

    @description: Deletes this extension.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_Extension_delete(NW_DOM_Extension_t* e);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Opaque_new

    @synopsis:    Creates a new Opaque type with a given data and length.

    @scope:       public

    @parameters:
       [in] NW_Uint32 length
                  Length of data block.

       [in] NW_Byte* data
                  Data block.

    @description: Creates a new Opaque type with a given data and length.

    @returns:     NW_DOM_Opaque_t*
                  New opaque object.

 ** ----------------------------------------------------------------------- **/
NW_DOM_Opaque_t*
NW_DOM_Opaque_new(NW_Uint32 length, NW_Byte* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Opaque_initialize

    @synopsis:    Initializes an Opaque type with a given data and length.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_Opaque_t* o
                  The opaque object.

       [in] NW_Uint32 length
                  Length of data block.

       [in] NW_Byte* data
                  Data block.

    @description: Initializes an Opaque type with a given data and length.

    @returns:     NW_Status_t
                  default

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_DOM_Opaque_initialize(NW_DOM_Opaque_t* o, 
                         NW_Uint32 length, 
                         NW_Byte* data);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Opaque_getLength

    @synopsis:    Returns length of opaque data.

    @scope:       public

    @parameters:
       [in] NW_DOM_Opaque_t* opaque
                  This opaque object.

    @description: Returns length of opaque data.

    @returns:     NW_Uint32
                  Length of data.

 ** ----------------------------------------------------------------------- **/
NW_Uint32
NW_DOM_Opaque_getLength(NW_DOM_Opaque_t* opaque);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Opaque_getData

    @synopsis:    Returns data from opaque.

    @scope:       public

    @parameters:
       [in] NW_DOM_Opaque_t* opaque
                  This opaque object.

    @description: Returns data from opaque.

    @returns:     NW_Byte*
                  Opaque data.

 ** ----------------------------------------------------------------------- **/
NW_Byte*
NW_DOM_Opaque_getData(NW_DOM_Opaque_t* opaque);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Opaque_delete

    @synopsis:    Delete this opaque object.

    @scope:       public

    @parameters:
       [in] NW_DOM_Opaque_t* opaque
                  This opaque object.

    @description: Delete this opaque object. Frees memory for data.

    @returns:     NW_Status_t
                  Result of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t 
NW_DOM_Opaque_delete(NW_DOM_Opaque_t* opaque);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Opaque_getUserOwnsData

    @synopsis:    Determines if user owns data storage and can deallocate it.

    @scope:       public

    @parameters:
       [in] NW_DOM_Opaque_t* opaque
                  This opaque object.

    @description: Determines if user owns data storage and can deallocate it.

    @returns:     NW_Bool
                  NW_TRUE if user owns data storage and can deallocate it, 
                  otherwise NW_FALSE.

 ** ----------------------------------------------------------------------- **/
NW_Bool
NW_DOM_Opaque_getUserOwnsData(NW_DOM_Opaque_t* opaque);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_Opaque_setUserOwnsData

    @synopsis:    Set user ownership flag on opaque data.

    @scope:       public

    @parameters:
       [in-out] NW_DOM_Opaque_t* opaque
                  This opaque object.

    @description: Set user ownership flag on opaque data.

    @returns:     NW_Status_t
                   Result of operation.

       [NW_STAT_SUCCESS]
                  Always returns success.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_Opaque_setUserOwnsData(NW_DOM_Opaque_t* opaque);

/*
combined WBXML attribute value and text components
*/

typedef enum NW_WbxmlComponent_e {
  NW_WBXML_ATTRIBUTE_VALUE_COMPONENT,
  NW_WBXML_TEXT_COMPONENT
} NW_WbxmlComponent_t;

NW_Status_t
NW_DOM_WbxmlComponent_setType(NW_WbxmlComponent_t AttributeValueOrText,
                              NW_TinyDom_AttrVal_t* pV,
                              NW_Uint32 type);

NW_Status_t
NW_DOM_WbxmlComponent_initFromString(NW_TinyDom_AttrVal_t* pV,
                                     NW_String_t * pString);

NW_Status_t
NW_DOM_WbxmlComponent_initFromEntity(NW_TinyDom_AttrVal_t* pV,
                                     NW_Uint32 entity);

NW_Status_t
NW_DOM_WbxmlComponent_initFromExtension(NW_TinyDom_AttrVal_t* val,
                                        NW_Uint16 token,
                                        NW_String_t* str);
NW_Status_t
NW_DOM_WbxmlComponent_initFromExtensionInt(NW_TinyDom_AttrVal_t* val,
                                           NW_Uint16 token,
                                           NW_Uint32 x);
NW_Status_t
NW_DOM_WbxmlComponent_initFromOpaque(NW_TinyDom_AttrVal_t* pV,
                                     NW_Uint32 byteCount,
                                     NW_Uint8* pData);
NW_Status_t
NW_DOM_WbxmlComponent_initFromToken(NW_TinyDom_AttrVal_t* pV,
                                    NW_Uint32 token);

NW_Uint32
NW_DOM_WbxmlComponent_getEntity(NW_TinyDom_AttrVal_t* pV);

NW_Status_t
NW_DOM_WbxmlComponent_getString(NW_TinyDom_AttrVal_t* pV,
                                NW_String_t* pString);

NW_Status_t
NW_DOM_WbxmlComponent_getExtensionToken(NW_TinyDom_AttrVal_t* val,
                                        NW_Uint16* pToken);

NW_Uint16
NW_DOM_WbxmlComponent_getExtension(NW_TinyDom_AttrVal_t* val,
                                   NW_String_t* str);

NW_Status_t
NW_DOM_WbxmlComponent_getExtensionInt(NW_TinyDom_AttrVal_t* val,
                                      NW_Uint32* x);
NW_Uint8*
NW_DOM_WbxmlComponent_getOpaque(NW_TinyDom_AttrVal_t* pV,
                                NW_Uint32 *pOpaqueByteCount);

NW_Uint16
NW_DOM_WbxmlComponent_getToken(NW_TinyDom_AttrVal_t* pV);

NW_Status_t
NW_DOM_WbxmlComponent_toString(NW_TinyDom_AttrVal_t* pV,
                               NW_String_t *string,
                               NW_Uint32 encoding);

/* converts an entire component sequence (via iterator on text or attrval) to string

returns either NW_STAT_SUCCESS or NW_STAT_DOM_NO_STRING_RETURNED if something fails */
NW_Status_t
NW_DOM_WbxmlComponent_sequenceToString(NW_WbxmlComponent_t AttributeValueOrText,
                                       NW_TinyDom_AttributeHandle_t* pIterator,
                                       NW_Uint32 encoding,
                                       NW_String_t* pOutString);

/* NW_Status_t NW_DOM_WbxmlComponent_[text|attribute]NextValue(pIterator, pV)
returns status NW_STAT_WBXML_ITERATE_MORE or NW_STAT_WBXML_ITERATE_DONE and fills in
*pV if returns NW_STAT_WBXML_ITERATE_MORE */
#define NW_DOM_WbxmlComponent_textNextValue(pIterator, pV) \
((NW_TinyDom_TextHandle_iterate((pIterator), (pV)) == 0)? NW_STAT_WBXML_ITERATE_DONE : NW_STAT_WBXML_ITERATE_MORE)

#define NW_DOM_WbxmlComponent_attributeNextValue(pIterator, pV) \
((NW_TinyDom_AttributeHandle_iterateValues((pIterator), (pV)) == 0)? NW_STAT_WBXML_ITERATE_DONE : NW_STAT_WBXML_ITERATE_MORE)


#ifdef __cplusplus
} // extern "C" {
#endif /* __cplusplus */

#endif  /* NW_DOM_WBXML_TYPES_H */
