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
// MDXMLComment.cpp
// @file
// This class represents a Comment section of an XML file -
// basically it is a chunk of text which has a place in the
// object tree but has no children.
// 
//

#include <f32file.h>
#include <gmxmldomconstants.h>
#include <gmxmlnode.h>
#include <gmxmlcharacterdata.h>
#include <gmxmlcomment.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif

EXPORT_C CMDXMLComment* CMDXMLComment::NewLC( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMComment
// @leave can Leave due to OOM
//
	{
	CMDXMLComment* secPtr = new(ELeave) CMDXMLComment( aOwnerDocument );
	CleanupStack::PushL( secPtr );
	const TDesC& nodeName = KXMLCommentNodeName;
	secPtr->SetNodeNameL( nodeName );
	return secPtr;
	}

EXPORT_C CMDXMLComment* CMDXMLComment::NewL( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMLElement
// @leave can Leave due to OOM
//
	{
	CMDXMLComment* secPtr = CMDXMLComment::NewLC( aOwnerDocument );
	CleanupStack::Pop(1); // secPtr
	return secPtr;
	}

CMDXMLComment::CMDXMLComment( CMDXMLDocument* aOwnerDocument ):
CMDXMLCharacterData( ECommentNode, aOwnerDocument)
//
// Constructor
// @param aOwnerDocument The document at the root of the DOM tree
//
	{
	}

EXPORT_C CMDXMLComment::~CMDXMLComment()
	{
	}



// End Of File
