// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the test key association object for the streaming test agent.
// 
//

#include "testkeyassociation.h"

using namespace StreamAccess;

CTestKeyAssociation::CTestKeyAssociation()
/**
 	Constructor for CTestKeyAssociation object.
 */
	{
	//empty
	}

EXPORT_C CTestKeyAssociation::~CTestKeyAssociation()
/**
 	Destructor for CTestKeyAssociation object.
 */
	{
	delete iKey;
	}

EXPORT_C CTestKeyAssociation* CTestKeyAssociation::NewL(RReadStream& aStream)
/**
 	Creates an empty test key association object.					
	
	@return A pointer to the empty test key association object.
 */
	{
	CTestKeyAssociation* self = new(ELeave)CTestKeyAssociation();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CTestKeyAssociation* CTestKeyAssociation::NewL(const TDesC8& aKey)
/**
 	Creates a new test key association object.				
	
	@param aKey The encryption key.
	@return A pointer to the new test key association object.
 */
	{
	CTestKeyAssociation* self = new (ELeave)CTestKeyAssociation();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	CleanupStack::Pop(self);
	return self;
	}

void CTestKeyAssociation::ConstructL(const TDesC8& aKey)
/**
 	Second-phase constructor for CTestKeyAssociation object.
 	
 	@param aKey The encryption key.
 */
	{
	iKey = aKey.AllocL();
	}
	

EXPORT_C const HBufC8* CTestKeyAssociation::GetTestKey() const
/**
 	@return The encryption key.
 */
	{
	return iKey;
	}


EXPORT_C void CTestKeyAssociation::ExternalizeL(RWriteStream& aStream) const
/**
 	Externalizes CTestKeyAssociation object.
 */
	{
	aStream<<*iKey;
	}

void CTestKeyAssociation::InternalizeL(RReadStream& aStream)
/**
 	Internalizes CTestKeyAssociation object.
 	@param aStream Stream to read from.
 */
	{
	const TInt KMaxKeyLen = 4096;
	iKey = HBufC8::NewL(aStream,KMaxKeyLen);
	}
	
