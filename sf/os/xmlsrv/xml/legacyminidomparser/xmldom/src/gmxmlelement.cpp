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
// MDXMLElement.CPP
// @file
// This file contains the implementation of the CMDXMLElement class.
// This class represents a generic XML element with attributes
// stored as name-value pairs.  DTD-specific element classes
// will store their attributes as member variables.
// 
//

#include <f32file.h>

#include <gmxmldomconstants.h>
#include <gmxmlnode.h>
#include <gmxmldocument.h>
#include <gmxmlelement.h>


EXPORT_C CMDXMLElement* CMDXMLElement::NewLC( TBool aCanHaveChildren, CMDXMLDocument* aOwnerDocument, TPtrC aTagName )
//
// Two phase constructor
// @param aCanHaveChildren Flag to indicate if the node represents a node
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @param aTagName Name of the tag used to create the element
// @return Created CMDXMLElement
// @leave can Leave due to OOM
//
	{
	CMDXMLElement* elPtr = new(ELeave) CMDXMLElement( aCanHaveChildren, aOwnerDocument );
	CleanupStack::PushL( elPtr );
	elPtr->SetNodeNameL( aTagName );
	elPtr->ConstructL();
	return elPtr;
	}

EXPORT_C CMDXMLElement* CMDXMLElement::NewL( TBool aCanHaveChildren, CMDXMLDocument* aOwnerDocument, TPtrC aTagName )
//
// Two phase constructor
// @param aCanHaveChildren Flag to indicate if the node represents a node
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @param aTagName Name of the tag used to create the element
// @return Created CMDXMLElement
// @leave can Leave due to OOM
//
	{
	CMDXMLElement* elPtr = CMDXMLElement::NewLC( aCanHaveChildren, aOwnerDocument, aTagName );
	CleanupStack::Pop(elPtr);
	return elPtr;
	}

EXPORT_C CMDXMLElement::CMDXMLElement( TBool aCanHaveChildren, CMDXMLDocument* aOwnerDocument ):
CMDXMLNode( EElementNode, aCanHaveChildren, aOwnerDocument)
//
// Constructor
// @param aCanHaveChildren Flag to indicate if the node represents a node which is allowed children
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
//
	{
	// No other work to do.
	}

void CMDXMLElement::ConstructL()
/**
 * 2nd-phase constructor. Initialises this object's member data.
 *
 * @leave KErrXxx Standard EPOC error codes if allocation or construction of the object's members fails
 */
	{
	iDescAttName = new (ELeave) CDesCArraySeg(1);
	iDescAttValue = new (ELeave) CDesCArraySeg(1);

	}

EXPORT_C CMDXMLElement::~CMDXMLElement()
/**
 * C++ destructor
 */
	{
	// Delete the array of attribute name-value pairs.
	if(iDescAttName)
		delete iDescAttName;

	if(iDescAttValue)
		delete iDescAttValue;

	}


EXPORT_C TInt CMDXMLElement::GetAttribute(const TDesC&  aAttributeName, TPtrC& aAttributeValue) const
//
// Returns the attribute value if it is set.
// @param aAttributeName Name of attribute to return
// @param aAttributeValue Value of attribute returned
// @return Returns KErrNone if successful, KErrNotFound if the named attribute
// is not set or KErrNotSupported if the named attribute doesn't exist.
//
	{
	TInt returnValue = KErrNone;

	TInt index;
	TInt attributeFound;
	attributeFound = iDescAttName->Find(aAttributeName,index);

	if(attributeFound == 0)
		{
		aAttributeValue.Set(iDescAttValue->MdcaPoint(index));
		}
	else
		returnValue = KErrNotSupported;

	return returnValue;

	}

EXPORT_C TInt CMDXMLElement::SetAttributeL(const TDesC& aAttributeName, const TDesC& aAttributeValue)
//
// Sets the attribute value.if it is valid
// @param aAttributeName Name of attribute to set
// @param aAttributeValue Value of attribute
// @return Returns KErrNone if successful, KErrNotSupported if a DTD-specific class.
// @leave Can Leave due to OOM
//
	{
	return SetAttributeL(aAttributeName, aAttributeValue, EFalse);
	}

EXPORT_C TInt CMDXMLElement::SetAttributeL(const TDesC& aAttributeName, const TDesC& aAttributeValue, TBool aStoreInvalid)
//
// Checks an attribute for validity and adds it to the DOM.  The aStoreInvalid parameter is used to control whether invalid
// attributes are added to the DOM.
// @param aAttributeName Name of attribute to set
// @param aAttributeValue Value of attribute
// @param aStoreInvalid If set to ETrue all attributes will be stored in the DOM.  Otherwise only those that are valid will be.
// @return Returns KErrNone if successful, KErrNotSupported if a DTD-specific class.
// @leave Can Leave due to OOM
//
	{
	TInt returnValue = (iOwnerDocument->DtdRepresentation()).IsValidAttributeForElementL(NodeName(), aAttributeName, aAttributeValue);
		
	if(returnValue == KErrNone || aStoreInvalid)
		{
		iDescAttName->AppendL(aAttributeName);
		iDescAttValue->AppendL(aAttributeValue);
		}
		
	return returnValue;
	}

EXPORT_C TInt CMDXMLElement::RemoveAttribute(const TDesC& aAttributeName)
//
// Removes the named attribute if present.
// @param aAttributeName Name of attribute to set
// @return Returns KErrNone if successful, KErrNotFound if the named attribute is not set, KErrNotSupported if a DTD-specific class.
//
	{
	TInt returnValue = KErrNone;

	TInt index;
	TInt attributeFound;
	attributeFound = iDescAttName->Find(aAttributeName,index);

	if(attributeFound == 0)
		{
		iDescAttName->Delete(index);
		iDescAttValue->Delete(index);
		}
	else
		returnValue = KErrNotFound;

	return returnValue;
	}


EXPORT_C TBool CMDXMLElement::IsAttributeSpecified(const TDesC& aAttributeName) const
//
// Finds out whether or not the named attribute is set
// @param aAttributeName Name of attribute that is subject of the enquiry.
// @return true of the named attribute has a value set, false if not
//
	{
	TBool returnValue = EFalse;

	if(iDescAttName)
		{
		TInt index = 0;
		TInt found = 0;
		found = iDescAttName->Find(aAttributeName,index);
		if(found == 0)
			returnValue = ETrue;
		}

	return returnValue;
	}





EXPORT_C TBool CMDXMLElement::CheckImmediateChildren()
// Check the immediate children of this element - i.e. the first level of children only
// If the document has not been constructed with a pointer to a MXMLDtd object then a default ETrue will be returned
// Otherwise the MXMLDtd object is used to validate immediate children - returns ETrue if valid 
// @return True if immediate children are valid
	{
	TBool returnValue = ETrue;
	TRAPD(error,returnValue = DoCheckImmediateChildrenL());
	if(error != KErrNone)
		{
		return EFalse;
		}
	return returnValue;
	}

TBool CMDXMLElement::DoCheckImmediateChildrenL()
	{
	TBool returnValue = ETrue;

	// Create an array of this elements immediate children
	CDesCArray* children = new (ELeave) CDesCArrayFlat(3);
	CleanupStack::PushL(children);

	if (HasChildNodes())
		{
		CMDXMLNode* childPtr;
		childPtr = FirstChild();
		// Cycle through the siblings
		while (childPtr != NULL)
			{
			  if( childPtr->NodeType() == EElementNode )
				  children->AppendL(childPtr->NodeName());
			  childPtr = childPtr->NextSibling();
			}
		returnValue = OwnerDocument()->DtdRepresentation().AreValidChildElementsL(this->NodeName(),*children); 
		}

	// Do a DTD specific check to see if the children are valid
	CleanupStack::PopAndDestroy();	// children	

	return returnValue;
	}

EXPORT_C TBool CMDXMLElement::CheckChildren()
//
// Check the children of this node for legality - must be defined based on DTD.
// This function checks that the list of child elements
// conforms to those allowed by the DTD.  
// As well as checking the list of direct children, it
// calls CheckChildren for each child element which is an
// element (it does not call this for child nodes which are
// not elements as they cannot have children).
// @return True if the node has legitimate children
//
	{
	TBool retVal= CheckImmediateChildren();
	if( retVal && HasChildNodes() )
		{
		CMDXMLNode* childPtr;
		childPtr = FirstChild();
		while((childPtr != NULL) && (retVal != false))
			{
			  if( childPtr->NodeType() == EElementNode )
				  {
				  retVal = childPtr->CheckChildren();
				  }
			  childPtr = childPtr->NextSibling();
			}
		}

	return retVal;
	}


EXPORT_C TInt CMDXMLElement::FindIndex(const TDesC &aAttName)
//
// Find an attribute and return the index of it
// @param aAttName the string to search for
// @return returns the index of the string if found or KErrNotFound
//
	{
	TInt index = KErrNotFound;

	if(iDescAttName)
		{
		TInt found = 0;
		found = iDescAttName->Find(aAttName,index);
		if(found != 0)
			index = KErrNotFound;
		}

	return index;


	}


EXPORT_C TInt CMDXMLElement::AttributeDetails(TInt aIndex, TPtrC& aAttributeName, TPtrC& aAttributeValue)
//
// Retrieves the Name and Value of an attribute at a given index
// @param aIndex the array index of the element for which details are required 
// @param aAttributeName the attribute name returned
// @param aAttributeValue the attribute value returned
// @return returns KErrNone if index is valid else KErrNotFound
//

	{
	TInt error = KErrNone;
	if (iDescAttName->Count() < aIndex)
		error = KErrNotFound;
	else
		{
		aAttributeValue.Set(iDescAttValue->MdcaPoint(aIndex));
		aAttributeName.Set(iDescAttName->MdcaPoint(aIndex));
		}
	return error;
	}



EXPORT_C TInt CMDXMLElement::NumAttributes()
//
// Retrieves the Number od Attributes that this element has
// @return returns the number of attributes held by the element
//
	{
	return iDescAttName->Count(); 
	}


EXPORT_C CMDXMLElement* CMDXMLElement::FirstChildOfType(const TDesC& aElementType)
// @return Returns a pointer to the first child of a given type if any, otherwise returns NULL
// @param aElementType Name of element type to return 
	{
	CMDXMLElement* retVal = NULL;
	CMDXMLNode* nodePtr = FirstChild();
	while((nodePtr != NULL) && (nodePtr->NodeName()).Compare(aElementType) != 0)
		{
		nodePtr = nodePtr->NextSibling();
		}
	if( nodePtr != NULL )
		{
		retVal = (CMDXMLElement*)nodePtr;
		}
	return retVal;

	}

EXPORT_C CMDXMLElement* CMDXMLElement::LastChildOfType(const TDesC& aElementType)
// @return Returns a pointer to the last child of a given type if any, otherwise returns NULL
// @param aElementType Name of element type to return 

	{
	CMDXMLElement* retVal = NULL;
	CMDXMLNode* nodePtr = LastChild();
	while((nodePtr != NULL) && (nodePtr->NodeName()).Compare(aElementType) != 0)
		{
		nodePtr = nodePtr->PreviousSibling();
		}
	if( nodePtr != NULL )
		{
		retVal = (CMDXMLElement*)nodePtr;
		}
	return retVal;
	}


// End Of File
