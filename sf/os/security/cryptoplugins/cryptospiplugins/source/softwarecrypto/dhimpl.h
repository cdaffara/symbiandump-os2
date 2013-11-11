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
*
*/


#ifndef __DHIMPL_H__
#define __DHIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include "keyagreementimpl.h"
#include "dhimpl.h"

/**
 * Implementation of DH Key Agreement as described in PKCS#3.
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CDHImpl) : public CKeyAgreementImpl
		{
	public:
		
		static CDHImpl* NewL(const CKey& aPrivateKey, const CCryptoParams* aParams);
		static CDHImpl* NewLC(const CKey& aPrivateKey, const CCryptoParams* aParams);

		// Destructor
		~CDHImpl();

		// from MKeyAgreement
		CKey* AgreeL(const CKey& aOtherPublicKey, const CCryptoParams* aParams);

		// Override CKeyAgreementImpl virtual functions
		TUid ImplementationUid() const;
		
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();

	private:
		/**
		Constructor
		*/
		CDHImpl();
		};
	}

#endif // __DHIMPL_H__
