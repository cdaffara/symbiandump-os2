// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the declaration of the CMDXMLElement class.
// 
//

/**
 @file
*/

#ifndef __GMXMLELEMENT_H__
#define __GMXMLELEMENT_H__

#include <e32base.h>
#include <badesca.h>
#include <gmxmlnode.h>


// forward references
class CMDXMLEntityConverter;
class CMDXMLComposer;


// Constants for the initial and terminal states
const TInt KXMLMinJump = 5; // Don't do binary chop below this size.  Not a critical value.

class CMDXMLElement : public CMDXMLNode
/** Generic XML element, and base class for particular XML element types.

It provides access to the element tag name and its attribute list. Attributes are
stored as name-value pairs.  
@publishedPartner
@released
*/
{
public:
	/** Allocates and constructs a new CMDXMLElement, leaving the object on the cleanup 
	stack.
	
	@param aCanHaveChildren Set to true if the element can have children
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@param aTagName Name of the tag for the element
	@leave KErrNoMemory Out of memory
	@return The new CMDXMLElement */
	IMPORT_C static CMDXMLElement* NewLC( TBool aCanHaveChildren, CMDXMLDocument* aOwnerDocument, TPtrC aTagName );

	/** Allocates and constructs a new CMDXMLElement.
	
	@param aCanHaveChildren Set to true if the element can have children
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@param aTagName Name of the tag for the element
	@leave KErrNoMemory Out of memory
	@return The new CMDXMLElement */
	IMPORT_C static CMDXMLElement* NewL( TBool aCanHaveChildren, CMDXMLDocument* aOwnerDocument, TPtrC aTagName );

	/** Destructor. */
	IMPORT_C virtual ~CMDXMLElement();

	// Attribute handling routines.

	/** Gets a specified attribute value.
	
	@param aAttributeName Name of the attribute to get
	@param aAttributeValue On return, the value of the attribute
	@return KErrNone if successful, KErrNotFound if the named attribute is not set. */
	IMPORT_C virtual TInt GetAttribute(const TDesC& aAttributeName, TPtrC& aAttributeValue ) const;

	/** Checks the validity of an attribute and adds it to the DOM if it's valid.
	
	@param aAttributeName Name of the attribute to set
	@param aAttributeValue Value of the attribute
	@leave KErrNoMemory Out of memory
	@return KErrNone if successful, KErrNotSupported if an invalid attribute name 
	or attribute value. */
	IMPORT_C virtual TInt SetAttributeL(const TDesC& aAttributeName, const TDesC& aAttributeValue);
	
	/** Checks the validity of an attribute and adds it to the DOM.  The aStoreInvalid 
	parameter is used to control whether invalid attributes are added to the DOM.
	@param aAttributeName Name of the attribute to set
	@param aAttributeValue Value of the attribute
	@param aStoreInvalid If set to EFalse only attributes that are found to be valid will be added to
	the DOM.  Set to ETrue to store all attributes in the DOM, even those that are invalid.
	@leave KErrNoMemory Out of memory
	@return KErrNone if successful, KErrNotSupported if an invalid attribute name 
	or attribute value. */
	IMPORT_C TInt SetAttributeL(const TDesC& aAttributeName, const TDesC& aAttributeValue, TBool aStoreInvalid);

	/** Removes a specified attribute.
	
	@param aAttributeName Name of the attribute to remove
	@return KErrNone if successful, KErrNotFound if the named attribute is not 
	present. */
	IMPORT_C virtual TInt RemoveAttribute(const TDesC& aAttributeName);

	/** Tests if a specified attribute is set.
	
	@param aAttributeName Name of the attribute to test
	@return True if the named attribute has a value set, false if not */
	IMPORT_C virtual TBool IsAttributeSpecified(const TDesC& aAttributeName) const;


	/** Checks the children of this node for validity.

	It checks that the list of child elements conforms to those allowed by the DTD. 
	
	In a generic CMDXMLElement object, this is always true as it has no basis 
	for a meaningful check. As well as checking the list of direct children, it 
	calls CheckChildren() for each child element. It does not check child nodes 
	that are not elements, as they cannot have children.
	
	@return True if the node has valid children */
	IMPORT_C virtual TBool CheckChildren();

// maybe these should be node functions ?
	/** Gets a pointer to the first child of a given type if any, otherwise returns 
	NULL.
	
	@param aElementType Name of element type to return
	@return First child element */
	IMPORT_C CMDXMLElement* FirstChildOfType(const TDesC& aElementType);

	/** Gets a pointer to the last child of a given type if any, otherwise returns 
	NULL.
	
	@param aElementType Name of element type to return
	@return Last child element */
	IMPORT_C CMDXMLElement* LastChildOfType(const TDesC& aElementType);



protected: //construction
	/**
	 * Constructor
	 * @param aCanHaveChildren Flag to indicate if the node represents a node which is allowed children
	 * @param aOwnerDocument Pointer to the document at the root of the DOM tree - if NULL then assume this is the root
	 */
	IMPORT_C CMDXMLElement( TBool aCanHaveChildren, CMDXMLDocument* aOwnerDocument );

	void ConstructL();
protected:
	/**
	 * Check the immediate children of this element - i.e. the first level of children only
	 * The default implementation returns true - derived classes can override with
	 * DTD-specific checks.
	 * @return True if immediate children are valid
	 */
	IMPORT_C virtual TBool CheckImmediateChildren();

private:
	 TBool DoCheckImmediateChildrenL();


public: 
	/** Finds an attribute and returns the index of it.
	
	@param aAttName The attribute to search for
	@return The index of the attribute if found, or KErrNotFound */
	IMPORT_C TInt FindIndex(const TDesC &aAttName);


	/** Gets the name and value of an attribute at a given index.
	
	@param Index The array index of the element for which details are required
	@param aAttributeName The attribute name returned
	@param aAttributeValue The attribute value returned
	@return KErrNone if index is valid, else KErrNotFound */
	IMPORT_C TInt AttributeDetails(TInt Index, TPtrC& aAttributeName, TPtrC& aAttributeValue);

	/** Gets the number of attributes that this element has.
	
	@return The number of attributes held by the element */
	IMPORT_C TInt NumAttributes();

protected:
	/** Store attribute names */
	CDesCArraySeg* iDescAttName;		// Store Attribute Names
	/** Store attribute values */
	CDesCArraySeg* iDescAttValue;		// Store Attribute Values

};


#endif
