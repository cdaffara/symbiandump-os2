/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32strm.h>
#include <caf/virtualpath.h>
#include "cafutils.h"

using namespace ContentAccess;

EXPORT_C CVirtualPath* CVirtualPath::NewL(const TDesC& aCombinedUriUniqueId)
	{	
	// TVirtualPathPtr performs the separation of URI and UniqueId if required
	return CVirtualPath::NewL(TVirtualPathPtr(aCombinedUriUniqueId));
	}

EXPORT_C CVirtualPath* CVirtualPath::NewL(const TDesC& aURI, const TDesC& aUniqueId)
	{	
	return CVirtualPath::NewL(TVirtualPathPtr(aURI, aUniqueId));
	}

EXPORT_C CVirtualPath* CVirtualPath::NewL(const TVirtualPathPtr& aPtr)
	{	
	CVirtualPath* self = new (ELeave) CVirtualPath(aPtr);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CVirtualPath* CVirtualPath::NewL(RReadStream& aStream)
	{
	CVirtualPath* self = new (ELeave) CVirtualPath(TVirtualPathPtr(KNullDesC()));
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}	


CVirtualPath::CVirtualPath(const TVirtualPathPtr& aVirtualPath) : iCombinedUriUniqueIdPtr(KNullDesC16()), iVirtualPathPtr(aVirtualPath) 
	{
	}

void CVirtualPath::ConstructL()
	{
	iURI = iVirtualPathPtr.URI().AllocL();
	iUniqueId = iVirtualPathPtr.UniqueId().AllocL();
	
	// Make sure the TVirtualPathPtr points to the member
	// variables not the arguments which could be deleted or modified at any time
	iVirtualPathPtr.Set(*iURI, *iUniqueId);

	// Create a combined URI descriptor that stores the URI and UniqueId of the content
	CreateCombinedUriUniqueIdL(*iURI, *iUniqueId);
	iCombinedUriUniqueIdPtr.Set(iCombinedUriUniqueId->Des());
	}

void CVirtualPath::CreateCombinedUriUniqueIdL(const TDesC& aUri, const TDesC& aUniqueId)
	{
	
	// build a concatenated version of the URI and UniqueId
	// The KCafVirtualPathSeparator is used to separate the URI from the UniqueId
	HBufC* temp = HBufC::NewL(aUri.Length() + 1 + aUniqueId.Length());
	CleanupStack::PushL(temp);

	TPtr combinedUriAndUniqueId = temp->Des();
	combinedUriAndUniqueId.Append(aUri);
	combinedUriAndUniqueId.Append(KCafVirtualPathSeparator());
	combinedUriAndUniqueId.Append(aUniqueId);

	// Set iCombinedUriUniqueId since everything worked up to this point
	delete iCombinedUriUniqueId;
	iCombinedUriUniqueId = NULL;
	iCombinedUriUniqueId = temp;
	iCombinedUriUniqueIdPtr.Set(iCombinedUriUniqueId->Des());

	// dont delete temp since it's now pointed to by iCombinedUriUniqueId
	CleanupStack::Pop(temp);
	}


CVirtualPath::~CVirtualPath()
	{
	delete iURI;
	delete iUniqueId;
	delete iCombinedUriUniqueId;
	}

EXPORT_C const TDesC& CVirtualPath::URI() const
	{
	return *iURI;
	}

EXPORT_C const TDesC& CVirtualPath::UniqueId() const
	{
	return *iUniqueId;
	}

EXPORT_C const TDesC& CVirtualPath::GetCombinedUriUniqueId()
	{
	return iCombinedUriUniqueIdPtr;
	}

EXPORT_C void CVirtualPath::ExternalizeL(RWriteStream& aStream) const
	{
	TCafUtils::WriteDescriptor16L(aStream, *iURI);
	TCafUtils::WriteDescriptor16L(aStream, *iUniqueId);
	}	

void CVirtualPath::InternalizeL(RReadStream& aStream)
	{
	// Allocate space for the URI, UniqueId and CombinedURI
	iURI = TCafUtils::ReadDescriptor16L(aStream);
	iUniqueId = TCafUtils::ReadDescriptor16L(aStream);

	// Create a combined URI and UniqueId for these two
	CreateCombinedUriUniqueIdL(*iURI, *iUniqueId);

	// Set the TVirutalPathPtr member to be the URI and UniqueId
	iVirtualPathPtr.Set(*iURI, *iUniqueId);
	}
