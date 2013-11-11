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
// MDXMLText.cpp
// @file
// This class represents a text section of an XML file -
// basically it is a chunk of text which has a place in the
// object tree but has no children.
// 
//

#include <f32file.h>

#include <gmxmldomconstants.h>
#include <gmxmlnode.h>
#include <gmxmlcharacterdata.h>
#include <gmxmltext.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif

EXPORT_C CMDXMLText* CMDXMLText::NewLC( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMLText
// @leave can Leave due to OOM
//
	{
	CMDXMLText* secPtr = new(ELeave) CMDXMLText( aOwnerDocument );
	CleanupStack::PushL( secPtr );
	const TDesC& nodeName = KXMLTextNodeName;
	secPtr->SetNodeNameL( nodeName );
	return secPtr;
	}

EXPORT_C CMDXMLText* CMDXMLText::NewL( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMLText
// @leave can Leave due to OOM
//
	{
	CMDXMLText* secPtr = CMDXMLText::NewLC( aOwnerDocument );
	CleanupStack::Pop(1); // secPtr
	return secPtr;
	}

CMDXMLText::CMDXMLText( CMDXMLDocument* aOwnerDocument ):
CMDXMLCharacterData( ETextNode, aOwnerDocument)
//
// Constructor
// @param aOwnerDocument The document at the root of the DOM tree
//
	{
	}

EXPORT_C CMDXMLText::~CMDXMLText()
	{
	}



// End Of File
