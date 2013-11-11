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

#ifndef __GMXMLCHARACTERDATA_H__
#define __GMXMLCHARACTERDATA_H__

#include <e32base.h>

#include <gmxmlnode.h>

class CMDXMLCharacterData : public CMDXMLNode
/** Base class for text sections, such as CDATA sections, comments and processing 
instructions, of an XML file.

Objects with this interface contain text, but cannot have child nodes. 
@publishedPartner
@released
*/
{
public:
	/** Destructor. */
	IMPORT_C virtual ~CMDXMLCharacterData();

	/** Gets the text of the section.
	
	@return The text of the section */
	IMPORT_C virtual TDesC& Data();

	/** Sets the whole of the section text.
	
	@param aData Text to set
	@leave KErrNoMemory Memory allocation failed */
	IMPORT_C virtual void SetDataL( TDesC& aData );

	/** Gets the length of the section text.
	
	@return Returns the length of the text */
	IMPORT_C virtual TInt Length();

	/** Appends text to the section.
	
	@param aData The text to append.
	@leave KErrNoMemory Memory allocation failed */
	IMPORT_C virtual void AppendL( TDesC& aData );

	/** Inserts text at a specified point in the section.
	
	@param aInsertPos The position at which to insert the text. 0 means insert 
	at the start.
	@param aData Text to insert
	@leave KErrNoMemory Memory allocation failed
	@return Returns KErrNone if successful or KErrNotFound if the insert position 
	is out of range */
	IMPORT_C virtual TInt InsertL( TInt aInsertPos, TDesC& aData );

	/** Replaces a block of text in the section. 
	
	@param aInsertPos The position at which to insert the text. 0 means insert 
	at the start.
	@param aLength The number of characters to replace
	@param aData The text to insert
	@leave KErrNoMemory Memory allocation failed
	@return Returns KErrNone if successful or KErrNotFoundif the replace block 
	is out of range */
	IMPORT_C virtual TInt ReplaceL( TInt aInsertPos, TInt aLength, TDesC& aData );


	/** Check the children of this node for validity.
	
	For a character data section, there can be no children so this is always true.
	
	@return Always true. */
	IMPORT_C virtual TBool CheckChildren();

protected:
	/*
	 * Constructor
	 * @param aNodeType The node type of the derived object
	 * @param aOwnerDocument The Document at the root of the DOM tree.
	 */
	CMDXMLCharacterData( TDOMNodeType aNodeType, CMDXMLDocument* aOwnerDocument );

private:
	HBufC* iData;	// Buffer to hold the actual text of the CDATA section
};


#endif
