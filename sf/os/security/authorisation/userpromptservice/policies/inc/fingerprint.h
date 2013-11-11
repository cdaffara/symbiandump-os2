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


/**
 @file
 @publishedPartner
 @released
*/

#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <e32base.h>
#include <e32cmn.h>

namespace UserPromptService
	{
	/**
	Class that represents the fingerprint data for a stored decision.
	
	The raw fingerprint data must be between one and  KMaxFingerprintLength bytes in 
	length and are typically created from selected parts of the destination and opaque fields.
	\n	
	E.g. Hash("GPRS") or Hash("+442071541000") or "ALL"\n

	Since hash functions are irreversible the fingerprint may also include a human readable
	string of up to KMaxDescriptionLength characters in length that describes the data used to 
	generate the hash. This may be used by a management user interface that allows a device user 
	to rescind individual decisions.

	This field is stored in the decision record but is otherwise unused by the User Prompt Service. 
	*/
	NONSHARABLE_CLASS(CFingerprint) : public CBase
		{
	public:		
		IMPORT_C static CFingerprint* NewL(const TDesC8& aFingerprint, const TDesC& aDescription);
		IMPORT_C static CFingerprint* NewLC(const TDesC8& aFingerprint, const TDesC& aDescription);

		IMPORT_C const TDesC8& Fingerprint() const;
		IMPORT_C const TDesC& Description() const;
		~CFingerprint();

	private:
		CFingerprint();
		void ConstructL(const TDesC8& aFingerprint, const TDesC& aDescription);
		
		HBufC8* iFingerprint;	///< The raw fingerprint data		
		HBufC* iDescription;	///< Describes the data used to generate the hash
		};
	}	

#endif // FINGERPRINT_H
