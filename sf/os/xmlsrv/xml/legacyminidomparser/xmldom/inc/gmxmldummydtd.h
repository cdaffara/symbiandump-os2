// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the declaration of the CMDXMLDocument when no 
// DTD is mentioned. 
// 
//
/**
 * @file 
 * @internalComponent
 * @released
 */

#ifndef __GMXMLDUMMYDTD_H__
#define __GMXMLDUMMYDTD_H__

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "GMXMLDocument.h"
#endif

_LIT(KXMLCDATASectionNodeName, "#cdata-section");
_LIT(KXMLCommentNodeName, "comment");
_LIT(KXMLProcessingInstructionNodeName, "processing_instruction");
_LIT(KXMLTextNodeName, "#text");
_LIT(KXMLDefaultVersionTag, "<?xml version=\"1.0\" ?>");
_LIT(KXMLDefaultDocTypeTag, "<!DOCTYPE smil SYSTEM \"mms_smil.dtd\">");
_LIT(KXMLVersion, "<?xml");
_LIT(KXMLEndTag, ">");
_LIT(KXMLDocumentTypes, "<!DOCTYPE");

/**
* ELement whose enumerated type has not been set.
* @internalTechnology
* @released
*/
const TInt KXMLUndefinedElement = -1; 

// Used as panic category and number when a removed export is called
_LIT(KLDRIMPORT, "LDR-IMPORT");

/**
* @internalTechnology
* @released
*/
const TInt KLdrImportedOrdinalDoesNotExist = 2;

class CMDXMLDummyDtd : public MXMLDtd
/*
This class is used internally when no DTD is supplied to CMDXMLDocument.
*/
/**
@internalComponent
@released
*/
	{
	TBool IsValidElementL(const TDesC& aElement) const;
	TInt IsValidAttributeForElementL(const TDesC& aElement, const TDesC& aAttribute, const TDesC& aAttributeValue) const;
	TBool AreValidChildElementsL(const TDesC& aParentElement, const CDesCArray& aChildElements) const;
	TBool CanElementHaveChildren(const TDesC& aElement) const;
	};

#endif //__GMXMLDUMMYDTD_H__
