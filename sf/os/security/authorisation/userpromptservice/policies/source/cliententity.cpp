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
*
*/


#include <ups/cliententity.h>
#include <ups/upserr.h>

using namespace UserPromptService;


EXPORT_C CClientEntity* CClientEntity::NewL(const TDesC8& aName)
/**
Creates a new client entity object.
@param aName	A name that uniquely identifies the entity within the context
				of the client process.
@return			A pointer to the new client entity object.						 
*/
	{
	CClientEntity* self = CClientEntity::NewLC(aName);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CClientEntity* CClientEntity::NewLC(const TDesC8& aName)
/**
Creates a new client entity object and places the pointer on the cleanup stack.
@param aName	A name that uniquely identifies the entity within the context
				of the client process.
@return			A pointer to the new client entity object.						 
*/
	{
	CClientEntity* self = new(ELeave) CClientEntity();
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}

void CClientEntity::ConstructL(const TDesC8& aName)
/**
Second phase constructor
@param aClientEntityName A descriptor containing the name of the client entity.
*/
	{	
	if (aName.Length() > KUpsMaxClientEntityLength)
		{
		User::Leave(KErrUpsBadClientEntityLength);
		}
	iName = aName.AllocL();
	}
	
EXPORT_C const TDesC8& CClientEntity::Name() const
/**
Gets the unique name for the client entity.
@return A descriptor containing the client entity name.
*/
	{
	return *iName;
	}

CClientEntity::CClientEntity()
/**
Constructor
*/
	{	
	}
	
CClientEntity::~CClientEntity()
/**
Destructor
*/
	{
	delete iName;
	}
