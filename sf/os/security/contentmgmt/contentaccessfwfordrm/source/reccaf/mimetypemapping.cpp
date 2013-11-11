/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "mimetypemapping.h"

CMimeTypeMapping* CMimeTypeMapping::NewL(const TDesC8& aContentMimeType)
	{
	CMimeTypeMapping* self;
	self = new (ELeave) CMimeTypeMapping();
	CleanupStack::PushL(self);
	self->ConstructL(aContentMimeType);
	CleanupStack::Pop(self);
	return self;
	}

CMimeTypeMapping* CMimeTypeMapping::NewL(const TDesC8& aContentMimeType, const TDesC8& aCafMimeType)
	{
	CMimeTypeMapping* self;
	self = new (ELeave) CMimeTypeMapping();
	CleanupStack::PushL(self);
	self->ConstructL(aContentMimeType, aCafMimeType);
	CleanupStack::Pop(self);
	return self;
	}

void CMimeTypeMapping::ConstructL(const TDesC8& aContentMimeType)
	{
	_LIT8(KApplicationXCafStr, "application/x-caf");
	
	// store the content mime type
	iContentMimeType = aContentMimeType.AllocL();
	
	// create the caf mime type 
	if (aContentMimeType == KApplicationXCafStr)
		{
		// the caf mime type is same as the content mime type
		iCafMimeType = aContentMimeType.AllocL();
		}
	else
		{
		// prepend x-caf- to the content mime type to create the CAF mime type
		_LIT8(KCAFPrefix,"x-caf-");
		iCafMimeType = HBufC8::NewL(KCAFPrefix().Length() + aContentMimeType.Length());
		TPtr8 ptr = iCafMimeType->Des();
		ptr.Copy(KCAFPrefix());
		ptr.Append(aContentMimeType);
		}
	}

void CMimeTypeMapping::ConstructL(const TDesC8& aContentMimeType, const TDesC8& aCafMimeType)
	{
	iContentMimeType = aContentMimeType.AllocL();
	iCafMimeType = aCafMimeType.AllocL();
	}

CMimeTypeMapping::~CMimeTypeMapping()
	{
	delete iContentMimeType;
	delete iCafMimeType;
	}

CMimeTypeMapping::CMimeTypeMapping() 
	{
	}

const TDesC8& CMimeTypeMapping::ContentMimeType() const
	{
	return *iContentMimeType;
	}

const TDesC8& CMimeTypeMapping::CafMimeType() const
	{
	return *iCafMimeType;
	}


TBool CMimeTypeMapping::ContentMimeTypeMatch(const CMimeTypeMapping& aFirst, const CMimeTypeMapping& aSecond)
	{	
	return *aFirst.iContentMimeType == *aSecond.iContentMimeType;
	}

