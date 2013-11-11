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

#ifndef __GMXMLCOMMENT_H__
#define __GMXMLCOMMENT_H__

#include <e32base.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <gmxmldocument.h>
#endif

class CMDXMLComment : public CMDXMLCharacterData
/** A comment section of an XML file.

Objects with this interface contain text, but cannot have child nodes. 
@publishedPartner
@released
*/
{
public:
	/** Allocates and constructs a new CMDXMLComment, leaving the object on the cleanup 
	stack.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@return New CMDXMComment 
	@leave KErrNoMemory Out of memory */
	IMPORT_C static CMDXMLComment* NewLC( CMDXMLDocument* aOwnerDocument );

	/** Allocates and constructs a new CMDXMLComment.
	
	@param aOwnerDocument Pointer to the document at the root of the DOM tree
	@return New CMDXMComment 
	@leave KErrNoMemory Out of memory */
	IMPORT_C static CMDXMLComment* NewL( CMDXMLDocument* aOwnerDocument );

	/** Destructor. */
	IMPORT_C virtual ~CMDXMLComment();


protected:
	/*
	 * Constructor
	 * @param aOwnerDocument The document at the root of the DOM tree
	 */
	CMDXMLComment( CMDXMLDocument* aOwnerDocument );
};


#endif
