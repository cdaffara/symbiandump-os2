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
//

/**
 @file tetestcontenthandler.cpp
 @internalTechnology
*/

#include "tetestcontenthandler.h"
#include <xml/documentparameters.h>
#include <xml/parserfeature.h>
#include <xml/matchdata.h>

using namespace Xml;
/**
 Allocates and constructs a CTestContentHandler object.
 Initialises all member data to their default values.
 */
CTestContentHandler* CTestContentHandler::NewL()
	{
	CTestContentHandler* self = new (ELeave) CTestContentHandler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
/**
 * Empty routine
 */
CTestContentHandler::~CTestContentHandler()
	{
	}
/**
 * Empty routine
 */
CTestContentHandler::CTestContentHandler()
	{
	}
/**
 * Empty routine
 */
void CTestContentHandler::ConstructL()
	{
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnStartDocumentL(const RDocumentParameters& /*aDocParam*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnEndDocumentL(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnStartElementL(const RTagInfo& /*aElement*/,
		const Xml::RAttributeArray& /*aAttributes*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnEndElementL(const RTagInfo& /*aElement*/,
		TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnContentL(const TDesC8& /*aBytes*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnStartPrefixMappingL(const RString& /*aPrefix*/,
		const RString& /*aUri*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnEndPrefixMappingL(const RString& /*aPrefix*/,
		TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/,
		TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnSkippedEntityL(const RString& /*aName*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnProcessingInstructionL(const TDesC8& /*aTarget*/,
		const TDesC8& /*aData*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
/**
 * Empty routine
 */
void CTestContentHandler::OnError(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

/** 
 Obtain the interface matching the specified uid.
 */
TAny* CTestContentHandler::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	// do nothing.
	return NULL;
	}

