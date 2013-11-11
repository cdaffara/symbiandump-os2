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
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGNAMESPACE_H
#define XMLENGNAMESPACE_H

#include <xml/dom/xmlengnode.h>

/**
This class represents an existing namespace declaration in the DOM tree or a
namespace node returned in RXmlEngNodeSet as a result of some XPath expression. 
 
All elements and attributes may have a namespace instance associated with them.
Once added to an element, a namespace declaration (instance of
TXmlEngNamespace) may be referred to by all attributes and descendants of the
element (unless the prefix that this namespace-to-prefix binding uses is
overriden somewhere below in the document tree).
 
General rules for namespace handling:
 
- A NULL prefix in the namespace node means NO PREFIX. An empty string "" can
be used to specify the absence of a prefix when setting it, however, NULL will
be stored and returned.

- A NULL value for a namespace URI is legal only with a NULL prefix and is only
used in the undeclaration of a namespace (@c xmlns="" ).  An empty string ""
can be used too, however, it will be returned as NULL.
 
- A NULL or empty string namespace URI with a non-NULL prefix is illegal
acording to the XML specification.

- The namespace of an element is a default namespace if it has a NULL prefix.
 
- Elements with namespace undeclarations (@c xmlns="" ), have no namespace and
are treated as local names.
 
- Attributes do not have a default namespace. Even if a namespace is applied at
element scope, the default namespace declaration is not applied to the
element's attributes (according to the XML specification).
 
- The namespace of a node (element or attribute) is undefined if no namespace
declaration is associated with it.
 
- A node with an undefined namespace is serialized as having no prefix. In
general, the DOM API handles the complexity of namespace declaration handling,
creating neccessary namespace declarations and undeclaring default namespaces.
However, some node-creation methods (e.g. TXmlEngElement::AddNewElementL()) do
not ensure that a node created with an undefined namespace (i.e. without a
prefix) will be treated after serialization as having the default namespace.
 
Note:
Namespace nodes that are the result of XPath expressions have following restrictions:

- They cannot be used as namespace definitions, because they are not part of
the DOM tree, but rather copies of existing namespace definitions.

- The namespace prefix is not available because it is not mandated by the DOM
Level 3 XPath API.  So, in returned copies of the DOM tree, namespace prefix
strings are not preserved and Prefix() returns NULL.
*/
class TXmlEngNamespace: public TXmlEngNode
{
    friend class TXmlEngElement;
    friend class TXmlEngNode;

  public:
    /** Default constructor */
	inline TXmlEngNamespace();

    /**
    Gets the namespace URI
	@return Namespace URI string or an empty string if the namespace is either
	NULL or undeclared (@c xmlns="" ) 
    */
    IMPORT_C TPtrC8 Uri() const;

    /**
    Gets the namespace prefix. 
	@return The prefix bound in the namespace declaration or an empty string
	when there is a default namespace or if no binding exists
    */
    IMPORT_C TPtrC8 Prefix() const;

    /**
    Check if the namespace is default.
	@return ETrue if the namespace is default (no prefix), EFalse if the
	namespace is not default (bound to prefix) or the namespace is NULL
    */
    IMPORT_C TBool IsDefault() const;

	/**
	Check if the namespace is undefined.  A node's namespace is undefined if no
	namespace declaration is associated with it.  This is the same as
	TXmlEngNode::IsNull().
    @return ETrue if the namespace is undefined, EFalse otherwise
    */
    IMPORT_C TBool IsUndefined() const; 

protected:
    /**
	This method is inherited from TXmlEngNode.  To prevent its use, no function body
	has been supplied.  The method Uri() should be used instead.  
	
	@see Uri().
    */
	inline TPtrC8 NamespaceUri();

protected:
    /**
    Constructor
    @param aPtr Namespace pointer
    */
	inline TXmlEngNamespace(void* aPtr);
};

#include <xml/dom/xmlengnamespace.inl>

#endif /* XMLENGNAMESPACE_H */

