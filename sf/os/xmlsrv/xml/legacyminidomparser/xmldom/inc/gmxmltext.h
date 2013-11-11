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
//

/**
 @file
*/

#ifndef __GMXMLTEXT_H__
#define __GMXMLTEXT_H__

#include <e32base.h>
#include <gmxmlcharacterdata.h>

class CMDXMLText : public CMDXMLCharacterData
/** A text section of an XML file.

Objects of this type hold text. They cannot have child nodes. 
@publishedPartner
@released
*/
{
public:
	/** Allocates and constructs a new CMDXMLText, leaving the object on the cleanup 
	stack.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@return The new CMDXMLText
	@leave KErrNoMemory Out of memory */
	IMPORT_C static CMDXMLText* NewLC( CMDXMLDocument* aOwnerDocument );

	/** Allocates and constructs a new CMDXMLText.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@return The new CMDXMLText 
	@leave KErrNoMemory Out of memory */
	IMPORT_C static CMDXMLText* NewL( CMDXMLDocument* aOwnerDocument );

	/** Destructor. */
	IMPORT_C virtual ~CMDXMLText();


protected:
	/*
	 * Constructor
	 * @param aOwnerDocument The document at the root of the DOM tree
	 */
	CMDXMLText( CMDXMLDocument* aOwnerDocument );
};


#endif
