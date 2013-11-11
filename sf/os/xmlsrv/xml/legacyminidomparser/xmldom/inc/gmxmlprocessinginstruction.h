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
// GMXMLProcessing Instruction.h
// 
//

/**
 @file
*/

#ifndef __GMXMLPROCINSTR_H__
#define __GMXMLPROCINSTR_H__

#include <e32base.h>
#include <gmxmlcharacterdata.h>

class CMDXMLProcessingInstruction : public CMDXMLCharacterData
/** A processing instruction section of an XML file.

It has text and a place in the object tree, but has no children. 
@publishedPartner
@released
*/
{
public:
	/** Allocates and constructs a new CMDXMLProcessingInstruction, leaving the object 
	on the cleanup stack.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@return The new CMDXMLProcessingInstruction
	@leave KErrNoMemory Out of memory */
	IMPORT_C static CMDXMLProcessingInstruction* NewLC( CMDXMLDocument* aOwnerDocument );

	/** Allocates and constructs a new CMDXMLProcessingInstruction.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@return The new CMDXMLProcessingInstruction
	@leave KErrNoMemory Out of memory */
	IMPORT_C static CMDXMLProcessingInstruction* NewL( CMDXMLDocument* aOwnerDocument );

	/** Destructor. */
	IMPORT_C virtual ~CMDXMLProcessingInstruction();


protected:
	/*
	 * Constructor
	 * @param aOwnerDocument The document at the root of the DOM tree
	 */
	CMDXMLProcessingInstruction( CMDXMLDocument* aOwnerDocument );
};


#endif
