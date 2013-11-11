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
// This file contains the declaration of the CMDXMLDocumentElement class.
// 
//

/**
 @file
*/

#ifndef __GMXMLDOCUMENTELEMENT_H__
#define __GMXMLDOCUMENTELEMENT_H__

#include <e32base.h>
#include <gmxmlelement.h>

class CMDXMLDocumentElement : public CMDXMLElement
/** A dummy XML element for use as the top level element by the document.

This is required to handle parsing badly-formed XML documents. 
@publishedPartner
@released
*/
{
public:
	/** Allocates and constructs a new CMDXMLDocumentElement, leaving the object on 
	the cleanup stack.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@leave KErrNoMemory Out of memory
	@return The new CMDXMLDocumentElement */
	IMPORT_C static CMDXMLDocumentElement* NewLC( CMDXMLDocument* aOwnerDocument);

	/** Allocates and constructs a new CMDXMLDocumentElement.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@leave KErrNoMemory Out of memory
	@return The new CMDXMLDocumentElement */
	IMPORT_C static CMDXMLDocumentElement* NewL( CMDXMLDocument* aOwnerDocument);

	/** Destructor. */
	IMPORT_C virtual ~CMDXMLDocumentElement();


protected: //construction
	/*
	 * Constructor
	 * @param aOwnerDocument Pointer to the document at the root of the DOM tree - if NULL then assume this is the root
	 */
	CMDXMLDocumentElement( CMDXMLDocument* aOwnerDocument );
};


#endif
