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
// MDXMLDocument.CPP
// @file
// This file contains the implementation of the CMDXMLDocument class which
// is the root class for a Mini-DOM tree.
// 
//

#include <gmxmldomconstants.h>
#include <gmxmlnode.h>
#include <gmxmlelement.h>
#include <gmxmldocument.h>
#include <gmxmldocumentelement.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
#include "gmxmldummydtd.h"
#endif
//
// Global functions					//
//


CMDXMLDocument::CMDXMLDocument(MXMLDtd& aDtdRepresentation) : iDtdRepresentation(&aDtdRepresentation)
//
// Constructor
//
	{
	}

CMDXMLDocument::CMDXMLDocument()
//
// Constructor
//
	{
	}


EXPORT_C CMDXMLDocument::~CMDXMLDocument()
 	{
 	delete iRootElement;
 	delete iVersionTag;
 	delete iDocTypeTag;

	if (iOwnsDtd)
		// If the DTD is a dummy DTD created by this document then it should be deleted.
		{
		delete iDtdRepresentation;
		}
 	}

EXPORT_C CMDXMLDocument* CMDXMLDocument::NewL(MXMLDtd& aDtdRepresentation)
//
// Factory function
// @return Returns the created CMDXMLDocument
// @leave Can Leave due to OOM
//
 	{
 	CMDXMLDocument* self = CMDXMLDocument::NewLC(aDtdRepresentation);
 	CleanupStack::Pop(self); 
 	return self;
 	}
 

EXPORT_C CMDXMLDocument* CMDXMLDocument::NewLC(MXMLDtd& aDtdRepresentation)
//
// Factory function
// @return Returns the created CMDXMLDocument
// @leave Can Leave due to OOM
//
 	{
 	CMDXMLDocument* self = new(ELeave) CMDXMLDocument(aDtdRepresentation);
 	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * Factory function - Without taking a DTD
 * Documents created using this function will not be able to validate the XML.
 * @return Returns the created CMDXMLDocument
 * @leave Can Leave due to OOM
 */
EXPORT_C CMDXMLDocument* CMDXMLDocument::NewL()
	{
	CMDXMLDocument* self = CMDXMLDocument::NewLC();
 	CleanupStack::Pop(self); 
 	return self;
	}


/**
 * Factory function - Without taking a DTD
 * Documents created using this function will not be able to validate the XML.
 * @return Returns the created CMDXMLDocument
 * @leave Can Leave due to OOM
 */
EXPORT_C CMDXMLDocument* CMDXMLDocument::NewLC()
	{
	CMDXMLDocument* self = new(ELeave) CMDXMLDocument();
 	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


void CMDXMLDocument::ConstructL()
	{
// Default XML Version & Doc tags being set 
// Can be reset externally
	iVersionTag = TPtrC(KXMLDefaultVersionTag).AllocL();
	iDocTypeTag = TPtrC(KXMLDefaultDocTypeTag).AllocL();

	iRootElement = CMDXMLDocumentElement::NewL(this);

	if (iDtdRepresentation == NULL)
		// If no DTD has been provided then use the dummy DTD
		{
		iDtdRepresentation = new (ELeave) CMDXMLDummyDtd;
		iOwnsDtd = ETrue;
		}
	}


EXPORT_C TBool CMDXMLDocument::SetVersionTagL(const TDesC& aVersionTag)
//
// @param replacement tag, this must be complete, starting <?xml and finishing >
// @return ETrue if operation succeeds, EFalse if formal parameter does not 
// begin with <?xml or does not end with an >
// @leave can Leave due to OOM
//
	{
	TBool returnValue = EFalse;

	// if correct tag frame
	if(aVersionTag.Left(TPtrC(KXMLVersion).Length()) == KXMLVersion
			&& aVersionTag.Right(TPtrC(KXMLEndTag).Length()) == KXMLEndTag)
		{
		delete iVersionTag;
		iVersionTag = NULL;
		iVersionTag = aVersionTag.AllocL();
		returnValue = ETrue;
		}

	return returnValue;
 	}
 
EXPORT_C TBool CMDXMLDocument::SetDocTypeTagL(const TDesC& aDocTypeTag)
//
// @param replacement tag, this must be complete, starting <!DOCTYPE and finishing >
// @return ETrue if operation succeeds, EFalse if formal parameter does not 
// begin with <!DOCTYPE or does not end with an angle bracket
// @leave can Leave due to OOM
//
	{	
	TBool returnValue = EFalse;

	// if correct tag frame
	if(aDocTypeTag.Left(TPtrC(KXMLDocumentTypes).Length()) == KXMLDocumentTypes
			&& aDocTypeTag.Right(TPtrC(KXMLEndTag).Length()) == KXMLEndTag)
		{
		delete iDocTypeTag;
		iDocTypeTag = NULL;
		iDocTypeTag = aDocTypeTag.AllocL();
		returnValue = ETrue;
		}

	return returnValue;
	}



EXPORT_C TBool CMDXMLDocument::ValidElementNameL(const TDesC& aElement) const
//
// Function to determine whether an Element name is valid for Document
// @return ETrue the if element name valid - else EFalse
// @param aElement the element name to be tested
// @leave can leave due to OOM
//
	{
	return iDtdRepresentation->IsValidElementL(aElement);
	}


EXPORT_C TBool CMDXMLDocument::CanElementHaveChildren(const TDesC& aElement) const
	{
	return iDtdRepresentation->CanElementHaveChildren(aElement);
	}

EXPORT_C void CMDXMLDocument::PlaceholderForRemovedExport1(MXMLDtd& /*aDtdRepresentation*/)
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}

EXPORT_C void CMDXMLDocument::PlaceholderForRemovedExport2()
	{
	User::Panic(KLDRIMPORT, KLdrImportedOrdinalDoesNotExist);
	}

const MXMLDtd& CMDXMLDocument::DtdRepresentation() const
// Returns a reference to the dtd class responsible for validation 
// @return Returns a reference to MXMLDtd class that should be used for Dtd validation checking
	{
	return *iDtdRepresentation;
	}


TBool CMDXMLDummyDtd::IsValidElementL(const TDesC& /*aElement*/) const
	{
	return ETrue;
	}

TInt CMDXMLDummyDtd::IsValidAttributeForElementL(const TDesC& /*aElement*/, const TDesC& /*aAttribute*/, const TDesC& /*aAttributeValue*/) const
	{
	return KErrNone;
	}

TBool CMDXMLDummyDtd::AreValidChildElementsL(const TDesC& /*aParentElement*/, const CDesCArray& /*aChildElements*/) const
	{
	return ETrue;
	}

TBool CMDXMLDummyDtd::CanElementHaveChildren(const TDesC& /*aElement*/) const
	{
	return ETrue;
	}

// End Of File
