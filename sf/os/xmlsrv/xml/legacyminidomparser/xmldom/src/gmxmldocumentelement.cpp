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
// MDXMLDocumentElement.CPP
// @file
// This file contains the implementation of the CMDXMLDocumentElement class.
// This class represents a generic XML element with attributes 
// stored as name-value pairs.  DTD-specific element classes
// will store their attributes as member variables.
// 
//

#include <gmxmldomconstants.h>
#include <gmxmldocument.h>
#include <gmxmldocumentelement.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif

EXPORT_C CMDXMLDocumentElement* CMDXMLDocumentElement::NewLC( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMLDocumentElement
// @leave can Leave due to OOM
//
	{
	CMDXMLDocumentElement* elPtr = new(ELeave) CMDXMLDocumentElement( aOwnerDocument );
	CleanupStack::PushL( elPtr );
	elPtr->ConstructL();
	elPtr->SetNodeNameL( KXMLDocumentElementNodeName );
	return elPtr;
	}

EXPORT_C CMDXMLDocumentElement* CMDXMLDocumentElement::NewL( CMDXMLDocument* aOwnerDocument)
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMLDocumentElement
// @leave can Leave due to OOM
//
	{
	CMDXMLDocumentElement* elPtr = CMDXMLDocumentElement::NewLC( aOwnerDocument);
	CleanupStack::Pop(1); // elPtr
	return elPtr;
	}

CMDXMLDocumentElement::CMDXMLDocumentElement( CMDXMLDocument* aOwnerDocument ):
CMDXMLElement( ETrue, aOwnerDocument)
//
// Constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
//
	{
	}

EXPORT_C CMDXMLDocumentElement::~CMDXMLDocumentElement()
	{
	}





// End Of File
