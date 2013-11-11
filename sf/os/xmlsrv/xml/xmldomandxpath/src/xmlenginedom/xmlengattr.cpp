// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Methods for attribute node
//


#include <xml/dom/xmlengelement.h>
#include <xml/dom/xmlengdocument.h>
#include <stdapis/libxml2/libxml2_tree.h>
#include "xmlengdomdefs.h"
#include <stdapis/libxml2/libxml2_globals.h>
#include <stdapis/libxml2/libxml2_parserinternals.h>
#include <xml/utils/xmlengmem.h>
#include <xml/utils/xmlengxestrings.h>

#define LIBXML_ATTRIBUTE (static_cast<xmlAttrPtr>(iInternal))

// ---------------------------------------------------------------------------------------------
// Clones attribute node
//
// @return A copy of the attribute with its value
// @note
//    Namespace of the attribute is reset; use TXmlEngNode::CopyToL(TXmlEngNode), which
//    finds appropriate or creates new namespace declaration on the new
//    parent node (argument should be an TXmlEngElement handle)
// @see CopyToL(TXmlEngNode)
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TXmlEngAttr TXmlEngAttr::CopyL() const
    {
    if (iInternal)
        {
        xmlAttrPtr attr = xmlCopyProp(NULL, LIBXML_ATTRIBUTE);
        OOM_IF_NULL(attr);
        TXmlEngAttr ncopy(attr);
        attr->doc = NULL;
        OwnerDocument().TakeOwnership(ncopy);
        return attr;
        }
    return NULL;    
    }

// ---------------------------------------------------------------------------------------------
// @return Attribute's contents
//
// @note For values consisting of more then one TXmlEngTextNode node (as attribute's child)
//       returns only the begining of the value; this happens when the value is
//       represented by list of TXmlEngTextNode and TXmlEngEntityReference nodes.
// @see IsSimpleContents(), WholeValueCopyL()
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngAttr::Value() const
    {
    if(LIBXML_ATTRIBUTE && LIBXML_ATTRIBUTE->children &&
         LIBXML_ATTRIBUTE->children->type == XML_TEXT_NODE)
        {
        return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_ATTRIBUTE->children->content)).PtrC8();
        }
    return KNullDesC8(); // the value starts with entity reference or is NULL
    }

// ---------------------------------------------------------------------------------------------
// @return Local name of the attribute
//
// @note Equal to TXmlEngNode::NodeName(), but works faster.
//
// <b>Never call this on NULL object!</b>
// ---------------------------------------------------------------------------------------------
//
EXPORT_C TPtrC8 TXmlEngAttr::Name() const
    {
    if(LIBXML_ATTRIBUTE)
        {
        return ((TXmlEngConstString)CAST_XMLCHAR_TO_DOMSTRING(LIBXML_ATTRIBUTE->name)).PtrC8();
        }
    return KNullDesC8();
    }

// ---------------------------------------------------------------------------------------------
// @return Compex value of the attribute,
//         probably consisting of text nodes and entity references
//
// Since the value may be composed from a set of TXmlEngTextNode and EntityRefernce nodes,
// the returned result is newly allocated string, which should be freed by caller.
// 
// <B style="color: red">BE SURE TO FREE THE RESULT STRING!!!</B>
// 
// On Symbian:
// @code
//    // Note the use of AllocAndFreeLC(), which converts value to
//    // UTF-16 descriptor, pushes it to the cleanup stack and
//    // DEALLOCATES string in the returned TDOMString object
//    HBufC* value = attr.WholeValueCopyL().AllocAndFreeLC();
//    ...
//    CleanupStack::PopAndDestroy(); // value;
// @endcode
//
// @see TXmlEngAttr::Value(), TXmlEngNode::Value(),
//      TXmlEngNode::IsSimpleTextContents(), TXmlEngNode::WholeTextContentsCopyL()
//
// @note In most cases using Value() is enough (and it needs no memory allocation).
//       Use IsSimpleTextContents() if there doubts can Value() be used or not safely.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngAttr::WholeValueCopyL(RBuf8& aBuffer) const
    {
    if (!LIBXML_ATTRIBUTE) 
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    xmlChar* value = xmlNodeListGetString(
        LIBXML_ATTRIBUTE->doc,
        LIBXML_ATTRIBUTE->children,
        1);
    TEST_OOM_FLAG;
    xmlCharAssignToRbuf8L(aBuffer,value);    
    }

// -------------------------------------------------------------------------------------
// Sets new value of the attribute. Provided new value will be escaped as needed.
// 
// @param  aNewValue   A string value for the attribute
//
// The new value should not contain entity references. Entity references are not expanded,
// but used as text, because the ampersand (&) character of reference is escaped.
// 
// @see SetRawValueL(TDOMString)
// -------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngAttr::SetValueL(
    const TDesC8& aNewValue)
    {
    if (!LIBXML_ATTRIBUTE) 
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    xmlAttrPtr prop = LIBXML_ATTRIBUTE;
    xmlNodePtr new_ch;
    if (!aNewValue.Length())
        {
        new_ch = NULL;
        }
    else
        {
        xmlChar* value = xmlCharFromDesC8L(aNewValue);
        new_ch = xmlNewText(NULL);
        if(!new_ch)
            {
            delete value;
            OOM_HAPPENED;
            };
        new_ch->content = value;
        new_ch->parent = reinterpret_cast<xmlNodePtr>(prop);
        new_ch->doc = prop->doc;
        }   
    xmlFreeNodeList(prop->children);
    prop->children = new_ch;
    prop->last = new_ch;
    }

// -------------------------------------------------------------------------------------
// Sets new value from escaped XML character data that may contain entity references.
//
// The value as if it is an escaped contents from XML file.
// If the value contains entity references, then the resulting
// content of the attribute is a list of TXmlEngTextNode and EntityRefeerence nodes.
// Predefined entities are converted into characters they represent.
//
// @see    TXmlEngAttr::SetValueL(TStringArg)
// -------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngAttr::SetEscapedValueL(
    const TDesC8& aNewValue)
    {
    if (!LIBXML_ATTRIBUTE) 
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    xmlAttrPtr prop = LIBXML_ATTRIBUTE;
    xmlNodePtr new_ch;
    if (!aNewValue.Length())
        {
        prop->last = NULL;
        new_ch = NULL;
        }
    else
        {
        xmlChar* value = xmlCharFromDesC8L(aNewValue);
        new_ch = xmlStringGetNodeList(LIBXML_ATTRIBUTE->doc, value);
        delete value;
        TEST_OOM_FLAG;

        // Set parent property on all child nodes and find pointer to the last node
        xmlNodePtr tmp = new_ch;
        while (tmp)
            {
            tmp->parent = (xmlNodePtr) prop;
            if (!(tmp->next))
                prop->last = tmp;
            tmp = tmp->next;
            }
        }   
    xmlFreeNodeList(prop->children);
    prop->children = new_ch;
    }

// -------------------------------------------------------------------------------------
// Sets new attribute value exactly as presented in the string.
//
// Predefined entities are not converted into characters they represent.
//
// @see    TXmlEngAttr::SetValueL()
// -------------------------------------------------------------------------------------
//
EXPORT_C void TXmlEngAttr::SetValueNoEncL( const TDesC8& aNewValue )
    {
    if (!LIBXML_ATTRIBUTE) 
    	{
    	User::Leave(KXmlEngErrNullNode);
    	}
    //
    xmlAttrPtr prop = LIBXML_ATTRIBUTE;
    xmlNodePtr new_ch;
	if (!aNewValue.Length())
        {
        prop->last = NULL;
        new_ch = NULL;
        }
    else
        {
        xmlChar* value = xmlCharFromDesC8L(aNewValue);
        new_ch = xmlNewText(NULL);
        if(!new_ch)
            {
            delete value;
            OOM_HAPPENED;
            };

        new_ch->name = xmlStringTextNoenc;
        new_ch->content = value;
        new_ch->parent = (xmlNodePtr) prop;
       	prop->last = new_ch;
        }   
    xmlFreeNodeList(prop->children);
    prop->children = new_ch;
    }

// ---------------------------------------------------------------------------------------------
// @return TXmlEngElement that contains the attribute
//
// Same as TXmlEngNode::ParentNode() but returns TXmlEngElement instead of TXmlEngNode
//
// @note  Copies of attributes [TXmlEngAttr::CopyL()] and newly created attribute nodes
//        [RXmlEngDocument::CreateAttributeL(..)] do not have parent element until they are attached
//        to some element.
// ---------------------------------------------------------------------------------------------
//
EXPORT_C const TXmlEngElement TXmlEngAttr::OwnerElement() const
    {
    XE_ASSERT_DEBUG(LIBXML_ATTRIBUTE);
    return TXmlEngElement(LIBXML_ATTRIBUTE->parent);
    }

