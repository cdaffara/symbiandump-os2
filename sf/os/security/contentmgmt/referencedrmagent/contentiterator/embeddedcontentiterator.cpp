/*
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <f32file.h>
#include <apmstd.h>

#include "contentIterator.h"
#include "virtualpath.h"
#include "content.h"
#include "manager.h"
#include "EmbeddedObject.h"
#include "EmbeddedcontentIterator.h"
#include "EmbeddedcontentIteratorBase.h"

using namespace ContentAccess;

CEmbeddedContentIterator* CEmbeddedContentIterator::NewL(const TVirtualPathPtr& aVirtualPath, TBool aRecursive, const TDesC8& aMimeType)
	{
	CEmbeddedContentIterator* self = new (ELeave) CEmbeddedContentIterator;
	CleanupStack::PushL(self);
	self->ConstructL(aVirtualPath, aRecursive, aMimeType);
	CleanupStack::Pop(self);
	return self;
	}

CEmbeddedContentIterator::CEmbeddedContentIterator() 
	{
	}

CEmbeddedContentIterator::~CEmbeddedContentIterator()
	{	
	delete iSubIterator;
	delete iContent;
	delete iUri;
	delete iMimeType;
	}

void CEmbeddedContentIterator::ConstructL(const TVirtualPathPtr& aVirtualPath, TBool aRecursive, const TDesC8& aMimeType)
	{
	iUri = aVirtualPath.URI().AllocL();
	iContent = CContent::NewL(*iUri);
	iRecursive = aRecursive;
	iMimeType = aMimeType.AllocL();
	
	if(aVirtualPath.UniqueId().Length() > 0)
		{
		iContent->OpenContainer(aVirtualPath.UniqueId());
		}
	
	// Find the first content object
	iSubIterator = CEmbeddedContentIteratorBase::NewL(*iContent, iRecursive, *iMimeType);
	}

const TDesC& CEmbeddedContentIterator::FileName() const
	{
	return *iUri;
	}

const TDesC& CEmbeddedContentIterator::UniqueId() const
	{
	return iSubIterator->EmbeddedObject().UniqueId();
	}

const TDesC& CEmbeddedContentIterator::Name() const
	{
	return iSubIterator->EmbeddedObject().Name();
	}

const TDesC8& CEmbeddedContentIterator::MimeType() const
	{
	return iSubIterator->EmbeddedObject().MimeType();
	}

TInt CEmbeddedContentIterator::Next()
	{
	return iSubIterator->Next();
	}

