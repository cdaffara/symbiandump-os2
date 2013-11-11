/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* plugin extended characteristic definition
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_EXTENDRDCHARACTERISTICS_H__
#define __CRYPTOAPI_EXTENDRDCHARACTERISTICS_H__

#include <e32base.h>
#include <cryptospi/cryptoparams.h>
#include <s32mem.h>

namespace CryptoSpi
	{
	/**
	The characteristics associated with the plugin implementation
	*/

	NONSHARABLE_CLASS(CExtendedCharacteristics) : public CBase
		{
	public:
	
		IMPORT_C static CExtendedCharacteristics* NewL(TInt aAvailableConcurrency,
													 TBool aExclusiveUse);
								
		IMPORT_C void AddCharacteristicL(const TInt value, TUid aUid);
		IMPORT_C void AddCharacteristicL(const TDesC8& value, TUid aUid);
		
		IMPORT_C ~CExtendedCharacteristics();
		
		/**
		Get the extended characteristic for this UID;
		@param aUid	The UID of the extended characteristic
		@param aCharacteristics	The value of the extended characteristic
		*/
		IMPORT_C TInt GetTIntCharacteristicL(TUid aUid) const;
		IMPORT_C const TDesC8& GetTDesC8CharacteristicL(TUid aUid) const;

		/**
		Retrieve all the extended characteristics
		*/
		IMPORT_C const CCryptoParams* ListExtendedCharacteristics() const;

	private:
		CExtendedCharacteristics();
		void ConstructL(TInt aAvailableConcurrency, TBool aExclusiveUse);
	
		/**
		The extended characteristics list
		*/
		CCryptoParams* iExtCharacteristics;
		};
	}
#endif //__CRYPTOAPI_EXTENDRDCHARACTERISTICS_H__
