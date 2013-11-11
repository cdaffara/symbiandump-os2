/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include <e32base.h>
#include "ct/ccttokentype.h"
#include "ct/CCTTokenTypeInfo.h"
#include "ct/InterfaceUID.hrh"
#include "ct/logger.h"
#include "cttokentypedeletehandler.h"

EXPORT_C CCTTokenTypeDeleteHandler::~CCTTokenTypeDeleteHandler()
	{
	}

EXPORT_C void CCTTokenType::ConstructL(TUid aUID, const TDesC &aLabel, RFs aFs)
	{
	iUID = aUID;
	iLabel = aLabel.AllocL();
	iFs = aFs;
	LOG2(_L("CCTTokenType::ConstructL: token type created, ref count == %d: %-32S"), iCount, iLabel);
	}

// Constructor. Register as high priority, so that deletion happens
// as soon as possible
EXPORT_C CCTTokenType::CCTTokenType()
	: iCount(1)
	{
	}

// Destructor. This should only ever be called from RunL.
EXPORT_C CCTTokenType::~CCTTokenType()
	{
	LOG1(_L("CCTTokenType::~CCTTokenType: token type destroyed: %-32S"), &Label());
	
	delete iLabel;

	// Call the delete handler by deleting it.
	// Note that the DLL may be unloaded after this, so we must return at once.
	delete iDeleteHandler;
	}

EXPORT_C void CCTTokenType::Release()
	{
	--iCount;
	
	LOG2(_L("CCTTokenType::Release: token type released, ref count == %d: %-32S"), iCount, &Label());
	LOG_INC_INDENT();
	
	if (!iCount)
		delete this;

	LOG_DEC_INDENT();
	}

EXPORT_C TUid CCTTokenType::Type() const
	{
	return iUID;
	}

EXPORT_C const TDesC& CCTTokenType::Label() const
	{
	// It's possible that this can be called before ConstructL() has been called
	// to set the label - for example by the logging code when the object gets
	// deleted before being fully constructed
	return iLabel ? static_cast<TDesC&>(*iLabel) : KNullDesC();
	}

EXPORT_C void CCTTokenType::IncReferenceCount()
	{
	++iCount;
	
	LOG2(_L("CCTTokenType::IncReferenceCount: token type referenced, ref count == %d: %-32S"), iCount, &Label());
	}
