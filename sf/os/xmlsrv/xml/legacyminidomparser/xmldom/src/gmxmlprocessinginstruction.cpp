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
// MDXMLProcessingInstruction.cpp
// @file
// This class represents a CDATA section of an XML file -
// basically it is a chunk of text which has a place in the
// object tree but has no children.
// 
//

#include <gmxmldomconstants.h>
#include <gmxmlnode.h>
#include <gmxmlcharacterdata.h>
#include <gmxmlprocessinginstruction.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif

EXPORT_C CMDXMLProcessingInstruction* CMDXMLProcessingInstruction::NewLC( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMProcessingInstruction
// @leave can Leave due to OOM
//
	{
	CMDXMLProcessingInstruction* secPtr = new(ELeave) CMDXMLProcessingInstruction( aOwnerDocument );
	CleanupStack::PushL( secPtr );
	const TDesC& nodeName = KXMLProcessingInstructionNodeName;
	secPtr->SetNodeNameL( nodeName );
	return secPtr;
	}

EXPORT_C CMDXMLProcessingInstruction* CMDXMLProcessingInstruction::NewL( CMDXMLDocument* aOwnerDocument )
//
// Two phase constructor
// @param aOwnerDocument Pointer to the document at the root of the DOM tree
// @return Created CMDXMLElement
// @leave can Leave due to OOM
//
	{
	CMDXMLProcessingInstruction* secPtr = CMDXMLProcessingInstruction::NewLC( aOwnerDocument );
	CleanupStack::Pop(1); // secPtr
	return secPtr;
	}

CMDXMLProcessingInstruction::CMDXMLProcessingInstruction( CMDXMLDocument* aOwnerDocument ):
CMDXMLCharacterData( EProcessingInstructionNode, aOwnerDocument)
//
// Constructor
// @param aOwnerDocument The document at the root of the DOM tree
//
	{
	}

EXPORT_C CMDXMLProcessingInstruction::~CMDXMLProcessingInstruction()
	{
	}



// End Of File
