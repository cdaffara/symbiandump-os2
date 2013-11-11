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



#include <caf/virtualpathptr.h>

#include <caf/caftypes.h>

using namespace ContentAccess;

EXPORT_C TVirtualPathPtr::TVirtualPathPtr(const TDesC& aCombinedUriUniqueId) : iUri(KNullDesC16()), iUniqueId(KNullDesC16())
	{
	TInt pos = KErrNotFound;

	//Search the URI looking for rightmost the separator character in case it is URI concatenated with UniqueId
	pos = aCombinedUriUniqueId.LocateReverse(KCafVirtualPathSeparator()[0]);
	
	// if the length and position are valid seperate the URI and UniqueId
	if(pos != KErrNotFound && aCombinedUriUniqueId.Length() - pos < KMaxCafUniqueId)
		{
		// Found the separator character
		TInt uniqueIdLength = aCombinedUriUniqueId.Length() - pos - 1;

		// Set the URI and UniqueId to point the corresponding parts of the supplied descriptor
		Set(aCombinedUriUniqueId.Left(pos), aCombinedUriUniqueId.Right(uniqueIdLength));
		}
	else
		{
		// If its just a URI, make sure we point to the default content object
		Set(aCombinedUriUniqueId, KDefaultContentObject());
		}
	}	

EXPORT_C TVirtualPathPtr::TVirtualPathPtr(const TDesC& aUri, const TDesC& aUniqueId) : iUri(aUri), iUniqueId(aUniqueId)
	{
	}	

EXPORT_C TVirtualPathPtr::TVirtualPathPtr(const TVirtualPathPtr& aPtr) : iUri(aPtr.URI()), iUniqueId(aPtr.UniqueId())
	{
	}	


EXPORT_C const TDesC& TVirtualPathPtr::URI() const
	{
	return iUri;
	}

EXPORT_C const TDesC& TVirtualPathPtr::UniqueId() const
	{
	return iUniqueId;
	}

void TVirtualPathPtr::Set(const TDesC& aUri, const TDesC& aUniqueId)
	{
	iUri.Set(aUri);
	iUniqueId.Set(aUniqueId);
	}

EXPORT_C TVirtualPathPtr& TVirtualPathPtr::operator = (const TVirtualPathPtr& aVirtualPath)
	{
	Set(aVirtualPath.URI(), aVirtualPath.UniqueId());
	return *this;
	}

EXPORT_C TVirtualPathPtr& TVirtualPathPtr::operator = (const TDesC& aCombinedUriUniqueId)
	{
	TVirtualPathPtr temp(aCombinedUriUniqueId);
	Set(temp.URI(), temp.UniqueId());
	return *this;
	}
