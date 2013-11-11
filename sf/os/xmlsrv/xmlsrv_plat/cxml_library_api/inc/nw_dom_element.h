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
#ifndef NW_DOM_ELEMENT_H
#define NW_DOM_ELEMENT_H

#include <xml/cxml/nw_dom_node.h>
#include <xml/cxml/nw_dom_attribute.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getTagName

    @synopsis:    Gets the tag name of the element.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in-out] NW_String_t* name
                  The name of the element.

    @description: Gets the tag name of the element.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Got tag name.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  It is not a NW_DOM_ELEMENT_NODE

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  Error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_DOM_ElementNode_getTagName(NW_DOM_ElementNode_t* elem, 
                              NW_String_t* name);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getTagToken

    @synopsis:    Gets the fully qualified token for the node.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

    @description: Gets the fully qualified token for the node. The most 
                  significant 8 bits are the codepage; the next 8 bits 
                  are the token.

    @returns:     NW_Uint16
                  The token, including the codepage. Returns 0 if not 
                  NW_DOM_ELEMENT_NODE.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Uint16 
NW_DOM_ElementNode_getTagToken(NW_DOM_ElementNode_t* elem);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getAttribute

    @synopsis:    Returns attribute value as string.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_String_t* attrName
                  Name of the attribute whose value is to be returned.

       [out] NW_String_t* attrValue
                  The value of the attribute.

    @description: Returns attribute value as string. In the case of WBXML, 
                  the value may consist of one or more of the five attribute 
                  types concatenated together to form a simple string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Returned attribute.

       [NW_STAT_NOT_FOUND]
                  Attribute is not found.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not a NW_DOM_ELEMENT_NODE.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                  General error.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_ElementNode_getAttribute(NW_DOM_ElementNode_t* elem, 
                                NW_String_t* attrName, 
                                NW_String_t* attrValue);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_setAttribute

    @synopsis:    Sets a named attribute to a value.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_String_t* attrName
                  The attribute whose value is to be set.

       [in] NW_String_t* attrValue
                  The new value of the attribute.

    @description: If the attribute with the given name exists then a its
                  value will be changed to input value, otherwise a new
                  attribute will be created. If the given attribute was
                  already present when the document was parsed the first 
                  time, it can not be removed. In this case a new attribute 
                  is added, and you will have two attributes with same name.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute set.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an element node.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter was NULL.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_ElementNode_setAttribute(NW_DOM_ElementNode_t* elem, 
                                NW_String_t* attrName, 
                                NW_String_t* attrValue);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_removeAttribute

    @synopsis:    Removes an attribute.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_String_t* attributeName
                  Name of the attribute to be removed.

    @description: Removes given attribute. Limited Use: You can only remove 
                  attributes that you explicitly created and added to the 
                  element. None of the attributes in the originally parsed 
                  document can be removed.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Removed attribute.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an NW_DOM_ELEMENT_NODE.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter was NULL.
 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_ElementNode_removeAttribute(NW_DOM_ElementNode_t* elem, 
                                   NW_String_t* attributeName);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_hasAttributes

    @synopsis:    Checks whether an element has attributes.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

    @description: Returns NW_TRUE if Attributes are present otherwise returns 
                  false; If value is true handle can be used to iterate over 
                  the Attributes list.

    @returns:     NW_Bool
                  Returns NW_TRUE if element has attributes, otherwise NW_FALSE.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Bool
NW_DOM_ElementNode_hasAttributes(NW_DOM_ElementNode_t* elem);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getAttributeListIterator

    @synopsis:    Gets an iterator that enables iteration over the 
                  list of attributes.

    @scope:       public

    @parameters:
       [in] NW_DOM_Node_t* node
                  The element node.

       [out] NW_DOM_AttributeListIterator_t* iterator
                  Handle to iterate over the list of attributes of an element.

    @description: Gets an iterator that enables iteration over the list 
                  of attributes. This is a WAP-adapted DOM extension.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Iterator returned.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an NW_DOM_ELEMENT_NODE.

       [NW_STAT_NOT_FOUND]
                  Iterator could not be found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t 
NW_DOM_ElementNode_getAttributeListIterator(NW_DOM_Node_t* node, 
                                            NW_DOM_AttributeListIterator_t* iterator);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getAttributeHandleByName

    @synopsis:    Gets an attribute handle by name.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_String_t* attributeName
                  Name of the attribute.

       [out] NW_DOM_AttributeHandle_t* handle
                  Handle to iterate over the different values of an attribute.

    @description: This method returns an AttributeHandle which enables user 
                  to evaluate different AttrVals (attribute values) that the 
                  attribute may have. This is a WAP-adapted DOM extension.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Handle returned.

       [NW_STAT_NOT_FOUND]
                  Attribute is not found.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not a NW_DOM_ELEMENT_NODE.

       [NW_STAT_FAILURE]
                  Empty name.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_ElementNode_getAttributeHandleByName(NW_DOM_ElementNode_t* elem, 
                                            NW_String_t* attributeName, 
                                            NW_DOM_AttributeHandle_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getAttributeHandleByToken

    @synopsis:    Gets an attribute handle by token.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_Uint16 token
                  Token of the attribute.

       [in-out] NW_DOM_AttributeHandle_t* handle
                  Gets different params of an attribute, its name, token, and values.

    @description: This method returns an AttributeHandle which enables user
                  to evaluate different AttrVals (attribute values) that the 
                  attribute may have.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Handle returned.

       [NW_STAT_NOT_FOUND]
                  Attribute is not found.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                   Not an NW_DOM_ELEMENT_NODE.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_ElementNode_getAttributeHandleByToken(NW_DOM_ElementNode_t* elem, 
                                             NW_Uint16 token, 
                                             NW_DOM_AttributeHandle_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getAttributeValueStringByToken

    @synopsis:    Gets attribute value as string.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_Uint16 token
                  Token of the attribute whose value is to be returned.

       [out] NW_String_t* attributeValue
                  The value of the attribute.

    @description: Gets attribute value as string.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute value returned.

       [NW_STAT_NOT_FOUND]
                   Attribute is not found.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                   Not an NW_DOM_ELEMENT_NODE.

       [NW_STAT_DOM_NO_STRING_RETURNED]
                   General error.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_ElementNode_getAttributeValueStringByToken(NW_DOM_ElementNode_t* elem, 
                                                  NW_Uint16 token, 
                                                  NW_String_t* attributeValue);

 
/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_setAttributeByToken

    @synopsis:    Set attribute by token.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_Uint16 token
                  The attribute to be changed. 

       [in-out] NW_DOM_AttrVal_t* attrValue
                  The new value of the attribute.

    @description: If the attribute with the given name exists then a its
                  value will be changed to input value, otherwise a new
                  attribute will be created. If the given attribute was
                  already present when the document was parsed the first
                  time, it can not be removed. In this case a new attribute
                  is added, and you will have two attributes with same name.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute set.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an NW_DOM_ELEMENT_NODE.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter is null.

       [NW_STAT_NOT_FOUND]
                  Attribute is not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_ElementNode_setAttributeByToken(NW_DOM_ElementNode_t* elem, 
                                       NW_Uint16 token, 
                                       NW_DOM_AttrVal_t* attrValue);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_setAttributeByAttrVal

    @synopsis:    Sets atrribute by attribute value.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_String_t* attrName
                  The attribute to be changed.

       [in] NW_DOM_AttrVal_t* attrValue
                  The new value of the attribute.

    @description: If the attribute with the given name exists then a its
                  value will be changed to input value, otherwise a new
                  attribute will be created LIMITATION: If the given
                  attribute was already present when the document was
                  parsed the first time, it can not be removed. In this
                  case a new attribute is added, and you will have two
                  attributes with same name

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute set.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an NW_DOM_ELEMENT_NODE.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter is null.

       [NW_STAT_NOT_FOUND]
                  Attribute is not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_ElementNode_setAttributeByAttrVal(NW_DOM_ElementNode_t* elem, 
                                         NW_String_t* attrName, 
                                         NW_DOM_AttrVal_t* attrValue);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_setAttributeByAttributeHandle

    @synopsis:    Sets attribute by attribute handle

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_DOM_AttributeHandle_t* handle
                   The attribute handle.

    @description: A new attribute will be created on element.
                  LIMITATION: If the given attribute was already
                  present when the document was parsed the first time,
                  it can not be removed. In this case a new attribute
                  is added, and you will have two attributes with same
                  name

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute set.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an NW_DOM_ELEMENT_NODE.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter is null.

       [NW_STAT_NOT_FOUND]
                  Attribute is not found.

 ** ----------------------------------------------------------------------- **/
IMPORT_C NW_Status_t
NW_DOM_ElementNode_setAttributeByAttributeHandle(NW_DOM_ElementNode_t* elem,
                                                 NW_DOM_AttributeHandle_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_removeAttributeByToken

    @synopsis:    Remove attribute by token.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_Uint16 attributeToken
                  Token of the attribute which is to be removed.

    @description: Removes given attribute. You can only remove attributes
                  that you explicitly created and added to the element.
                  None of the attributes in the originally parsed document
                  can not be removed.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Attribute removed.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an NW_DOM_ELEMENT_NODE.

       [NW_STAT_BAD_INPUT_PARAM]
                  Required parameter is null.

       [NW_STAT_NOT_FOUND]
                  Attribute is not found.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_ElementNode_removeAttributeByToken(NW_DOM_ElementNode_t* elem, 
                                          NW_Uint16 attributeToken);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getElementIteratorByTagName

    @synopsis:    Gets the handle to iterate over the elements of a given token.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_String_t* name
                  Name of the element to be found.

       [out] NW_DOM_NodeIterator_t* handle
                  Iterator to iterate over elements of a given token.

    @description: Returns a pointer to the next Element with the given name
                  using this node as the root of the subtree.

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Got iterator.

       [NW_STAT_FAILURE]
                  Required parameter is null.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an NW_DOM_ELEMENT_NODE.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_ElementNode_getElementIteratorByTagName(NW_DOM_ElementNode_t* elem, 
                                               NW_String_t* name,
                                               NW_DOM_NodeIterator_t* handle);


/** ----------------------------------------------------------------------- **
    @function:    NW_DOM_ElementNode_getElementIteratorByTagToken

    @synopsis:    Get element iterator by tag token.

    @scope:       public

    @parameters:
       [in] NW_DOM_ElementNode_t* elem
                  The element node.

       [in] NW_Uint16 token
                  Token of the element to be found.

       [out] NW_DOM_NodeIterator_t* handle
                  Iterator to iterate over elements of a given token.

    @description: Gets the handle to iterate over elements of a given token. 

    @returns:     NW_Status_t
                  Status of operation.

       [NW_STAT_SUCCESS]
                  Got iterator.

       [NW_STAT_FAILURE]
                  Required parameter is null.

       [NW_STAT_DOM_NODE_TYPE_ERR]
                  Not an NW_DOM_ELEMENT_NODE.

 ** ----------------------------------------------------------------------- **/
NW_Status_t
NW_DOM_ElementNode_getElementIteratorByTagToken(NW_DOM_ElementNode_t* elem, 
                                                NW_Uint16 token,
                                                NW_DOM_NodeIterator_t* handle);

#ifdef __cplusplus
} /* extern "C" { */
#endif /* __cplusplus */

#endif  /* NW_DOM_ELEMENT_H */
