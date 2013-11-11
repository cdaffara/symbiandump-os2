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


#ifndef __DHKEYPAIRGENIMPL_H__
#define __DHKEYPAIRGENIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include "keypairgenimpl.h"

/**
 * Implementation of DH Key Pair Generation as described in PKCS#3.
 */
namespace SoftwareCrypto
	{
using namespace CryptoSpi;

	NONSHARABLE_CLASS(CDHKeyPairGenImpl) : public CKeyPairGenImpl
		{
	public:

		static CDHKeyPairGenImpl* NewL(void);
		static CDHKeyPairGenImpl* NewLC(void);

		// Destructor
		~CDHKeyPairGenImpl();

		// from MPlugin
		void Reset();

		// from MKeyPairGenerator
		void GenerateKeyPairL(TInt aKeySize, const CCryptoParams& aKeyParameters, CKeyPair*& aKeyPair);

		// Override CKeyPairGenImpl virtual functions
		TUid ImplementationUid() const;

		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();

	private:
		/**
		Constructor
		*/
		CDHKeyPairGenImpl();
		void ConstructL(void);

	private:
		};
	}

#endif // __DHKEYPAIRGENIMPL_H__
