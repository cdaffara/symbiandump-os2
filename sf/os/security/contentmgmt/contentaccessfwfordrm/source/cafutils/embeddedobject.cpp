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
#include <caf/embeddedobject.h>
#include "cafutils.h"

using namespace ContentAccess;

EXPORT_C CEmbeddedObject* CEmbeddedObject::NewL(const TDesC& aUniqueId, const TDesC& aName, const TDesC8& aMimeType, TEmbeddedType aType)
	{
	CEmbeddedObject* self = new (ELeave) CEmbeddedObject(aType);
	CleanupStack::PushL(self);
	self->ConstructL(aUniqueId, aName, aMimeType);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CEmbeddedObject* CEmbeddedObject::NewL(const TDesC& aUniqueId, const TDesC8& aMimeType, TEmbeddedType aType)
	{
	return CEmbeddedObject::NewL(aUniqueId, KNullDesC(), aMimeType, aType);
	}

EXPORT_C CEmbeddedObject* CEmbeddedObject::NewL(RReadStream& aStream)
	{
	CEmbeddedObject* self = new (ELeave) CEmbeddedObject(EContentObject);
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}


CEmbeddedObject::CEmbeddedObject(TEmbeddedType aType) : iType(aType)
	{	
	}

CEmbeddedObject::~CEmbeddedObject()
	{
	delete iUniqueId;
	delete iName;
	}

void CEmbeddedObject::ConstructL(const TDesC& aUniqueId, const TDesC& aName, const TDesC8& aMimeType)
	{
	// Store the unique Id
	iUniqueId = aUniqueId.AllocL();
	
	// Only store the name if it is non zero, otherwise we presume the name
	// is the same as the UniqueId
	if(aName.Length() != 0)
		{
		iName = aName.AllocL();			
		}
	iMimeType.Copy(aMimeType.Left(KMaxDataTypeLength));
	}

EXPORT_C const TDesC& CEmbeddedObject::UniqueId() const
	{
	// return the name of the embedded object
	return *iUniqueId;
	}

EXPORT_C const TDesC& CEmbeddedObject::Name() const
	{
	if(iName)
		{
		return *iName;	
		}
	else
		{
		// Name is NULL so must be the same as UniqueId
		return *iUniqueId;	
		}
	}
		
EXPORT_C const TDesC8& CEmbeddedObject::MimeType() const
	{
	// return the mime type of the embedded object
	return iMimeType;
	}

EXPORT_C TEmbeddedType CEmbeddedObject::Type() const
	{
	// return the type of object (container, content, agent specific object)
	return iType;
	}


EXPORT_C void CEmbeddedObject::ExternalizeL(RWriteStream& aStream) const
	{
	// write the name, mime type and embedded object type to the stream
	TCafUtils::WriteDescriptor16L(aStream, *iUniqueId);
	TCafUtils::WriteDescriptor8L(aStream, iMimeType);
	aStream.WriteInt32L(static_cast<TInt>(iType));
	if(iName)
		{
		TCafUtils::WriteDescriptor16L(aStream, *iName);
		}
	else
		{
		TCafUtils::WriteDescriptor16L(aStream, KNullDesC());	
		}
	}	

void CEmbeddedObject::InternalizeL(RReadStream& aStream)
	{
	// Read name, mime type and embedded object type from the stream
	iUniqueId = TCafUtils::ReadDescriptor16L(aStream);
	TCafUtils::ReadDescriptor8L(aStream, iMimeType);
	iType = static_cast<TEmbeddedType> (aStream.ReadInt32L());
	iName = TCafUtils::ReadDescriptor16L(aStream);
	// If the name is zero length it is supposed to be iName = NULL
	if(iName->Des().Length() == 0)
		{
		delete iName;
		iName=NULL;
		}
	}

