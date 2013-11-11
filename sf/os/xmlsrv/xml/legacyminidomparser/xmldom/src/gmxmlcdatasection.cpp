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
// MDXMLCDATASection.cpp
// @file
// This class represents a CDATA section of an XML file -
// basically it is a chunk of text which has a place in the
// object tree but has no children.
// 
//

#include <gmxmldomconstants.h>
#include <gmxmlnode.h>
#include <gmxmlcharacterdata.h>
#include <gmxmlcdatasection.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif

EXPORT_C CMDXMLCDATASection* CMDXMLCDATASection::NewLC( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMCDATASection
// @leave can Leave due to OOM
//
	{
	CMDXMLCDATASection* secPtr = new(ELeave) CMDXMLCDATASection( aOwnerDocument );
	CleanupStack::PushL( secPtr );
	const TDesC& nodeName = KXMLCDATASectionNodeName;
	secPtr->SetNodeNameL( nodeName );
	return secPtr;
	}

EXPORT_C CMDXMLCDATASection* CMDXMLCDATASection::NewL( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMCDATASection
// @leave can Leave due to OOM
//
	{
	CMDXMLCDATASection* secPtr = CMDXMLCDATASection::NewLC( aOwnerDocument );
	CleanupStack::Pop(1); // secPtr
	return secPtr;
	}

CMDXMLCDATASection::CMDXMLCDATASection( CMDXMLDocument* aOwnerDocument ):
CMDXMLCharacterData( ECDATASectionNode, aOwnerDocument)
//
// Constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree 
//
	{
	}

EXPORT_C CMDXMLCDATASection::~CMDXMLCDATASection()
	{
	}



// End Of File
