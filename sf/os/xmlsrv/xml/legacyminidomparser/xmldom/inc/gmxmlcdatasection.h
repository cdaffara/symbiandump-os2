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

#ifndef __GMXMLCDATA_H__
#define __GMXMLCDATA_H__

#include <e32base.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <gmxmldocument.h>
#endif

class CMDXMLCDATASection : public CMDXMLCharacterData
/** A CDATA section of an XML file.

Objects of this type hold text, including characters that would otherwise 
be regarded as markup. They cannot have child nodes. 
@publishedPartner
@released
*/
{
public:
	/** Allocates and constructs a new CMDXMLCDATASection, leaving the object on the 
	cleanup stack.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@leave KErrNoMemory Memory allocation failed
	@return The new CMDXMCDATASection */
	IMPORT_C static CMDXMLCDATASection* NewLC( CMDXMLDocument* aOwnerDocument );

	/** Allocates and constructs a new CMDXMLCDATASection.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@leave KErrNoMemory Memory allocation failed
	@return The new CMDXMCDATASection */
	IMPORT_C static CMDXMLCDATASection* NewL( CMDXMLDocument* aOwnerDocument );

	/** Destructor. */
	IMPORT_C virtual ~CMDXMLCDATASection();


protected:
	/*
	 * Constructor
	 * @param aOwnerDocument Pointer to the document at the root of the DOM tree
	 */
	CMDXMLCDATASection( CMDXMLDocument* aOwnerDocument );
};


#endif
