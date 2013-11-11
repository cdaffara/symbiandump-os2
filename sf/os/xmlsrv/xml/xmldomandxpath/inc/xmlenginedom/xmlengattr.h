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
// Attribute node functions
//



/**
 @file
 @publishedAll
 @released
*/
#ifndef XMLENGATTR_H
#define XMLENGATTR_H

#include <xml/dom/xmlengnode.h>

/**
This class represents a XML attribute in the DOM tree.  
*/
class TXmlEngAttr : public TXmlEngNode
{
  public:
    /** Default constructor */
    inline TXmlEngAttr();

    /**
    Clones attribute node.

	Note: Resets the namespace of the attribute. To avoid this, use 
    TXmlEngNode::CopyToL(), which finds an appropriate or creates
    a new namespace declaration on the new parent node (argument should be
    an TXmlEngElement handle)

    @see TXmlEngNode::CopyToL()

    @return A copy of this attribute
	@leave - One of the system-wide error codes
    */
    IMPORT_C TXmlEngAttr CopyL() const;

    /**
	Get the owner element.

    Same as TXmlEngNode::ParentNode() but returns TXmlEngElement 
    instead of TXmlEngNode.

	Note: Copies of attributes [TXmlEngAttr::CopyL()] and newly created 
    attribute nodes [RXmlEngDocument::CreateAttributeL()] do not have 
    parent element until they are attached to some element.

	If there is no owning element, a NULL element is returned.

    @return TXmlEngElement that contains the attribute
    */
    IMPORT_C const TXmlEngElement OwnerElement() const;

    /**
    Get the attribute name.  Equal to TXmlEngNode::Name(), but works faster.
    
    @return Local name of the attribute
    @pre Node must not be NULL
    @see TXmlEngNode::Name()
    */
    IMPORT_C TPtrC8 Name() const;

    /**
    Get the attribute's value.  If the value consists of more than one TXmlEngTextNode,
	as children of the attribute, only the beginning of the value is returned.  This 
	happens when the value is represented by list of TXmlEngTextNode and 
	TXmlEngEntityReference nodes.
    
    @return The attribute's value
    @see IsSimpleContents(), WholeValueCopyL()
    */
    IMPORT_C TPtrC8 Value() const; 

    /**
    Get a copy of attribute content.  Since the value may be composed from a set of 
	TXmlEngTextNode and TXmlEngEntityReference nodes, the returned result is a newly allocated 
    RBuf, which should be closed by the caller.
    
    Example usage of the API:
    @code
       RBuf8 value;
	   attr.WholeValueCopyL(value);
       ...
       value.Close();
    @endcode

    In most cases using Value() is enough (and it needs no memory allocation).
    Use IsSimpleTextContents() if there are doubts whether Value() can be used safely.

    @return Complex value of the attribute, probably consisting of text nodes and entity references
	@leave KXmlEngErrNullNode Node is NULL
	@leave - One of the system-wide error codes
    @see TXmlEngAttr::Value()
	@see TXmlEngNode::Value()
    @see TXmlEngNode::IsSimpleTextContents()
    @see TXmlEngNode::WholeTextContentsCopyL()
    */
    IMPORT_C void WholeValueCopyL(RBuf8& aBuffer) const;

    /**
	Sets the value of the attribute.  The new value should not contain entity
	references. Entity references are not expanded, but used as text, thus
	the string "abc &amp; def" is copied directly as "abc &amp; def" without
	expansion.

    @param aNewValue A string value for the attribute
	@leave KXmlEngErrNullNode Node is NULL
	@leave - One of the system-wide error codes
    @see SetEscapedValueL(const TDesC8&)
    */
    IMPORT_C void SetValueL(const TDesC8& aNewValue);

    /**
    Sets the value of the attribute from escaped XML character data that may contain 
    entity references.
    
    If the value contains entity references, then the resulting
    content of the attribute is a list of TXmlEngTextNode 
    and TXmlEngEntityReference nodes.
    Predefined entities are converted into characters they represent.
    
    @param aNewValue is a new attribute value
	@leave KXmlEngErrNullNode Node is NULL
	@leave - One of the system-wide error codes
    @see TXmlEngAttr::SetValueL(const TDesC8&)
    */
    IMPORT_C void SetEscapedValueL(const TDesC8& aNewValue);

	/**
	Sets new attribute value exactly as presented in the string.
	Predefined entities are not converted into characters they represent.
	
    @param aNewValue is the new attribute value 
	@leave KXmlEngErrNullNode Node is NULL
	@leave - One of the system-wide error codes
    @see TXmlEngAttr::SetValueL(const TDesC8&)
    */
	IMPORT_C void SetValueNoEncL(const TDesC8& aNewValue );

protected:
    /**
    Constructor
    @param aInternal attribute pointer
    */
    inline TXmlEngAttr(void* aInternal);
};

#include <xml/dom/xmlengattr.inl>

#endif /* XMLENGATTR_H */

