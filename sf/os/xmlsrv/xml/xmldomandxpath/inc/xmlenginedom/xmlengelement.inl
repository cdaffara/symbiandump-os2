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
// Inline functions specyfic for element node
// Default constructor
//

inline TXmlEngElement::TXmlEngElement():TXmlEngNode(NULL) {}

// -------------------------------------------------------------------------------------
// Constructor
// -------------------------------------------------------------------------------------
//
inline TXmlEngElement::TXmlEngElement(void* aInternal): TXmlEngNode(aInternal) {}


// -------------------------------------------------------------------------------------
// Creates new attribute node using namespace of its parent element (this element),
// sets attribute's value and links it as the last attribute of the element
//
// @param aName	- Local name of attribute 
// @param aValue - Value to set for new attribute or NULL (sets value to "")
// @return A handler to the newly created attribute node;
//
// For more hints how to use it refer to AddNewAttributeL(const TDesC8&,const TDesC8&)
//		
// @note	
// 	- No checks are made that attribute with such name exists
// 	- if namespace of the parent element is default (i.e. bound prefix is NULL),
// 	  then temporary prefix will be used and bound to the same namespace URI as elements
// 	  (It is due to the fact that default namespaces do not spread on unprefixed attributes,
// 	   see http://w3.org/TR/REC-xml-names/#defaulting)
// -------------------------------------------------------------------------------------
//
inline TXmlEngAttr TXmlEngElement::AddNewAttributeSameNsL(const TDesC8& aName, const TDesC8& aValue)
    {
    return AddNewAttributeL(aName, aValue, NamespaceDeclaration());
    }

// -------------------------------------------------------------------------------------
// @return Default namespace in the scope of the element
//
// NULL TXmlEngNamespace means that element with no prefix have no namespace associated
// because no default namespace was declared or default namespace was undeclared with <b>xmlns=""</b>
//
// Equivalent to LookupNamespaceByPrefixL() with NULL (or "") prefix provided
// -------------------------------------------------------------------------------------
//
inline TXmlEngNamespace TXmlEngElement::DefaultNamespaceL() const
    {
    return LookupNamespaceByPrefixL(KNullDesC8);
    }

// -------------------------------------------------------------------------------------
// Specialized version of TXmlEngNode::CopyL()
//
// @return Deep copy of the element.
// -------------------------------------------------------------------------------------
//
inline TXmlEngElement TXmlEngElement::CopyL() const
    {return TXmlEngNode::CopyL().AsElement();}

// -------------------------------------------------------------------------------------
// Removes all element contents: child nodes, attributes and namespace declarations
//
// @see RemoveChildren(), RemoveAttributes(), RemoveNamespaceDeclarations();
// -------------------------------------------------------------------------------------
//
inline void TXmlEngElement::ClearElement()
    {
    RemoveChildren();
    RemoveAttributes();
    RemoveNamespaceDeclarations();
    }

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
//


// -------------------------------------------------------------------------------------
// Returns TRUE if the element holds an attribute with such namespace URI and name.
//
// Same result gives AttributeNodeL(uri,name).NotNull()
// -------------------------------------------------------------------------------------
//
inline TBool TXmlEngElement::HasAttributeL(const TDesC8& aLocalName, const TDesC8& aNamespaceUri) const
	{return AttributeNodeL(aLocalName, aNamespaceUri).NotNull();}




