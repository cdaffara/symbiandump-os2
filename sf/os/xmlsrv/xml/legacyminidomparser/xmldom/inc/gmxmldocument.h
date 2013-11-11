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
// This file contains the declaration of the CMDXMLDocument class which
// is the root class for a Mini-DOM tree.
// 
//
/**
 * @file 
 * @publishedPartner
 * @released
 */

#ifndef __GMXMLDOCUMENT_H__
#define __GMXMLDOCUMENT_H__

#include <e32base.h>

#include <e32std.h>
#include <badesca.h>

class CMDXMLElement;
class MXMLDtd;

/**
MXMLDtd is a mixin class. If User wants  to  derive from MXMLDtd class ,
then  one  should not derive from any other class at the same time. 
i.e A class can not derive from CBase  at all  using MXMLDtd.

Interface for DTD defintion classes. 
A class derived from this interface can be used by CMDXMLParser or CMDXMLComposer 
to check that the data conforms to a specific DTD. 
@publishedPartner
@released
*/
class MXMLDtd

	{

public:
	/** Tests if an element name is valid in the DTD.
	
	@param aElement The element name to be tested
	@return ETrue the if element name valid, else EFalse
	@leave KErrNoMemory Out of memory */
	virtual TBool IsValidElementL(const TDesC& aElement) const = 0;

	/** Tests if an attribute name and value is valid in DTD.
	
	@param aElement The element to which the attributes belong
	@param aAttribute The attribute to be tested
	@param aAttributeValue The attribute value to be tested
	@return KErrNone if the name and value are valid, KXMLBadAttributeName if the 
	attribute name is invalid, or KXMLBadAttributeValue if the attribute value 
	is invalid 
	@leave KErrNoMemory Out of memory */
	virtual TInt IsValidAttributeForElementL(const TDesC& aElement, const TDesC& aAttribute, const TDesC& aAttributeValue) const = 0;

	/** Tests if the parent/child relationship is valid in DTD.
	
	@param aParentElement The name of the parent element to be tested
	@param aChildElements An array of child element names to be tested
	@return ETrue if parent/child relationship is valid
	@leave KErrNoMemory Out of memory */
	virtual TBool AreValidChildElementsL(const TDesC& aParentElement, const CDesCArray& aChildElements) const = 0;

	/** Tests it is valid for a particular element to have children.
	
	@param aElement The name of the element to be tested
	@return ETrue if it is valid for the element to have children */
	virtual TBool CanElementHaveChildren(const TDesC& aElement) const = 0;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "gmxmldummydtd.h"
#endif

class CMDXMLDocument : public CBase
/** An XML document.

An object of this type owns the root element of a DOM tree and is not itself 
an XML node.
@publishedPartner
@released
 */
{
public:

	/**
	 Allocates and constructs a new CMDXMLDocument.
	 @param iDtdRepresentation DTD for the document
	 @return The new CMDXMLDocument object
	 @leave KErrNoMemory Out of memory 
	 */
  	IMPORT_C static CMDXMLDocument* NewL(MXMLDtd& iDtdRepresentation);


	/**
	 Allocates and constructs a new CMDXMLDocument, leaving the object on the cleanup stack.
	 @param iDtdRepresentation DTD for the document
	 @return The new CMDXMLDocument object
	 @leave KErrNoMemory Out of memory 
	 */
  	IMPORT_C static CMDXMLDocument* NewLC(MXMLDtd& iDtdRepresentation);


	/** Allocates and constructs a new CMDXMLDocument, without specifying a DTD.

	Documents created using this function will not be able to validate the XML.
	
	@return The new CMDXMLDocument
	@leave KErrNoMemory Out of memory  */
  	IMPORT_C static CMDXMLDocument* NewL();


	/** Allocates and constructs a new CMDXMLDocument, without specifying a DTD, 
	leaving the object on the cleanup stack.

	Documents created using this function will not be able to validate the XML.
	
	@return The new CMDXMLDocument
	@leave KErrNoMemory Out of memory  */
  	IMPORT_C static CMDXMLDocument* NewLC();


	/** Destructor. */
  	IMPORT_C virtual ~CMDXMLDocument();
   
	/** Gets the root element of the DOM tree.
	
	@return The root element of the DOM tree */
  	inline CMDXMLElement* DocumentElement() {return iRootElement;}


	/** Sets the document's XML version processing instruction.
	
	@param aVersionTag The XML version processing instruction. This must begin 
	with <?xml and end with >, for example <?xml version="1.0"?>.
	@return ETrue if successful, EFalse if the parameter format is not correct
	@leave KErrNoMemory Out of memory */
  	IMPORT_C TBool SetVersionTagL(const TDesC& aVersionTag);

	/** Sets the document's DOCTYPE declaration.
	
	@param aDocTypeTag The XML DOCTYPE declaration. This must begin<!DOCTYPE and 
	end >.
	@return ETrue if operation succeeds, EFalse if the parameter format is not 
	correct
	@leave KErrNoMemory Out of memory */
  	IMPORT_C TBool SetDocTypeTagL(const TDesC& aDocTypeTag);

	/** Tests whether an element name is valid for the document.
	
	@param aElement The element name to be tested
	@return True the if element name valid, else false
	@leave KErrNoMemory Out of memory */
	IMPORT_C TBool ValidElementNameL(const TDesC& aElement) const;

	/**
	 Tests whether it is valid for a particular element to have children.
	 @param aElement the name of the element to be tested
	 @return ETrue if it is valid for element to have children
	 */

	IMPORT_C TBool CanElementHaveChildren(const TDesC& aElement) const;
	/** Gets the document's XML version processing instruction.
	
	@return The document's XML version processing instruction. */
  	inline const TDesC& VersionTag() const {return *iVersionTag;}

	/** Gets the document's DOCTYPE declaration.
	
	@return The document's DOCTYPE declaration. */
  	inline const TDesC& DocTypeTag() const  {return *iDocTypeTag;}

	/** Gets a reference to the MXMLDtd object to use for DTD validation checking.
	
	@return DTD */
	const MXMLDtd& CMDXMLDocument::DtdRepresentation() const;


private:  
  	CMDXMLDocument(MXMLDtd& aDtdRepresentation);
  	CMDXMLDocument();

	/*
  	 * sets the root elememnt of the DOM tree.
  	 */
  	inline void SetDocumentElement(CMDXMLElement* aRootElement) {iRootElement = aRootElement;}
  	
  	void ConstructL();
  	
  	IMPORT_C void PlaceholderForRemovedExport1(MXMLDtd& aDtdRepresentation);
  	IMPORT_C void PlaceholderForRemovedExport2();
  
private:
   	CMDXMLElement* iRootElement;	// Root element node of the DOM tree

protected:
	/** XML Version text */
	HBufC* iVersionTag;				// XML Version text
	/** Document type text */
  	HBufC* iDocTypeTag;				// Document type text

private:
	MXMLDtd* iDtdRepresentation;	
	TBool iOwnsDtd;
};

#endif
