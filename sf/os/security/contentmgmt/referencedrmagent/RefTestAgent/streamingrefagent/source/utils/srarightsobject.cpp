/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements the rights object for the streaming test agent.
*
*/


#include "srarightsobject.h"


CSraRightsObject::CSraRightsObject()
/**
 	Default constructor.
 */
	{
	//empty
	}

EXPORT_C CSraRightsObject::~CSraRightsObject()
/**
 	Destructor.
 */
	{
	delete iCid;
	delete iRiUri;
	delete iKey;
	}

EXPORT_C CSraRightsObject* CSraRightsObject::NewL(RReadStream& aStream)
/**
 	Creates a new test rights object from a stream.
 	@param The read stream interface.
	@return A pointer to the new test rights object.
 */
	{
	CSraRightsObject* self = new (ELeave) CSraRightsObject();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

void CSraRightsObject::ConstructL(const TDesC8& aCid, const TDesC8& aRiUri, const TDesC8& aKey)
/**
 	Second phase constructor.
 */
	{
	iCid 	= aCid.AllocL();
	iRiUri 	= aRiUri.AllocL();
	iKey 	= aKey.AllocL();
	}

#ifdef _DEBUG
CSraRightsObject::CSraRightsObject(const TBool& aExpired, TUint8 aAccessRights)
/**
 	Constructor for the utility functions.
 */
	:	iExpired(aExpired), 
		iAccessRights(aAccessRights)
	{
	//empty
	}

CSraRightsObject* CSraRightsObject::NewL(TUint8 aAccessRights, const TDesC8& aCid, const TDesC8& aRiUri, const TDesC8& aKey, const TBool& aExpired)
/**
 	Creates a new test rights object. This is not used in regular flow of the component.
	@return A pointer to the new test rights object.
 */
	{
	CSraRightsObject* self = new (ELeave) CSraRightsObject(aExpired, aAccessRights);
	CleanupStack::PushL(self);
	self->ConstructL(aCid, aRiUri, aKey);
	CleanupStack::Pop(self);
	return self;
	}

void CSraRightsObject::ExternalizeL(RWriteStream& aStream) const
/**
 	Writes the member data of this object into the stream buffer.
 	This is not used in regular flow of the component.
 	@param aStream Stream to write to.
 */
	{
	aStream.WriteUint8L(iAccessRights);
	aStream<<*iCid;
	aStream<<*iRiUri;
	aStream<<*iKey;
	aStream.WriteInt32L(iExpired);
	}
#endif

void CSraRightsObject::InternalizeL(RReadStream& aStream)
/**
 	Internalises the test rights object.
 	@param aStream Stream to read from.
 */
	{
	iAccessRights = aStream.ReadUint8L();
	iCid 	= HBufC8::NewL(aStream,KMaxFileName);
	iRiUri 	= HBufC8::NewL(aStream,KMaxFileName);
	iKey 	= HBufC8::NewL(aStream,KMaxFileName);
	iExpired = aStream.ReadInt32L();
	}


EXPORT_C HBufC8* CSraRightsObject::ContentId() const
/**
 	@return Content ID. Just pointer is passed. Ownership is not transfered.
 */
	{
	return iCid;
	}


EXPORT_C HBufC8* CSraRightsObject::RightsIssuer() const
/**
 	@return The URI of the Rights Issuer.
 */
	{
	return iRiUri;
	}


EXPORT_C HBufC8* CSraRightsObject::Key() const
/**
 	@return The decryption key.
 */
	{
	return iKey;
	}


EXPORT_C TUint8 CSraRightsObject::AccessRights() const
/**
 	@return The access rights of the RO.
 */
	{
	return iAccessRights;
	}

EXPORT_C TBool CSraRightsObject::IsExpired() const
/**
 	@return Whether the RO is expired.
 */
	{
	return iExpired;
	}
