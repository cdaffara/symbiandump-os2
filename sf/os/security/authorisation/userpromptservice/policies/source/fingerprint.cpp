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


#include <ups/fingerprint.h>
#include <ups/upsconst.h>
#include <ups/upserr.h>

using namespace UserPromptService;


EXPORT_C CFingerprint* CFingerprint::NewL(const TDesC8& aFingerprint, const TDesC& aDescription)
/**
Creates a new fingerprint object.
@param	aFingerprint	  An 8-bit descriptor containing the raw fingerprint data.
@param	aDescription	  A human readable description of the fingerprint.
@return	A pointer to the new fingerprint object.
@leave	KErrUpsBadFingerprintLength if the fingerprint is empty or exceeds 
		KUpsMaxFingerprintLength bytes in length.
*/
	{
	CFingerprint* self = CFingerprint::NewLC(aFingerprint, aDescription);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CFingerprint* CFingerprint::NewLC(const TDesC8& aFingerprint, const TDesC& aDescription)
/**
Creates a new fingerprint object.
@param	aFingerprint	  An 8-bit descriptor containing the raw fingerprint data.
@param	aDescription	  A human readable description of the fingerprint.
@return A pointer to the new fingerprint object.
@leave	KErrUpsBadFingerprintLength if the fingerprint is empty or exceeds 
		KUpsMaxFingerprintLength bytes in length.
*/
	{
	CFingerprint* self = new(ELeave) CFingerprint();
	CleanupStack::PushL(self);
	self->ConstructL(aFingerprint, aDescription);
	return self;
	}

void CFingerprint::ConstructL(const TDesC8& aFingerprint, const TDesC& aDescription)
/**
Second phase constructor
@param	aFingerprint The raw fingerprint data.
@param	aDescription A description of the fingerprint data.
@leave	KErrUpsBadFingerprintLength if the fingerprint exceeds 
		KUpsMaxFingerprintLength bytes in length.
*/
	{
	if (aFingerprint.Length() > KUpsMaxFingerprintLength)
		{
		User::Leave(KErrUpsBadFingerprintLength);
		}
	iFingerprint = aFingerprint.AllocL();
	iDescription = aDescription.AllocL();
	}
	
EXPORT_C const TDesC8& CFingerprint::Fingerprint() const
/**
Gets the raw fingerprint data.
@return An 8-bit descriptor containing the raw fingerprint data.
*/
	{
	return *iFingerprint;
	}
	
EXPORT_C const TDesC& CFingerprint::Description() const
/**
Gets the description of the fingerprint.
@return A 16-bit description containing the description of the fingerprint.
*/
	{
	return *iDescription;
	}

CFingerprint::CFingerprint() 
/**
Constructor
*/
	{	
	}

CFingerprint::~CFingerprint() 
/**
Destructor
*/
	{
	delete iFingerprint;
	delete iDescription;
	}	
