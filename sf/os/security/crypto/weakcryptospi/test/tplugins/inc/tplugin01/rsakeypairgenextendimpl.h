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
* rsakeypairgenimpl.h
*
*/


#ifndef __RSAKEYPAIRGENEXTENDIMPL_H__
#define __RSAKEYPAIRGENEXTENDIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include "keypairgenimpl.h"

/**
 * Implementation of RSA key pair generation as described in PKCS#1 v1.5.
 */
namespace SoftwareCrypto
	{
	NONSHARABLE_CLASS(CRSAKeyPairGenExtendImpl) : public CKeyPairGenImpl
		{
	public:
	
		static CRSAKeyPairGenExtendImpl* NewL(TUid aImplementationUid);
		static CRSAKeyPairGenExtendImpl* NewLC(TUid aImplementationUid);

		// from MPlugin
		void Reset();

		// from MKeyPairGenerator
		void GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair);
		
		// Override CKeyPairGenImpl virtual functions
		TUid ImplementationUid() const;
		
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();

		
		// Destructor
		~CRSAKeyPairGenExtendImpl();

	private:
		/**
		Constructor
		*/
		CRSAKeyPairGenExtendImpl(TUid aImplementationUid);
		
		//Extended Characteristics
		CExtendedCharacteristics* iExtendChars;
			
		/// second phase of construction
		void ConstructL();
		};
	}

#endif // __RSAKEYPAIRGENEXTENDIMPL_H__
