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


#ifndef __KEYCONVERTER_H__
#define __KEYCONVERTER_H__

#include <asymmetrickeys.h>

/** 
* @file
* @internalComponent
* @released 
*/

namespace CryptoSpi
	{
	class CKeyPair;
	class CKey;
	}


class KeyConverter
	{
	public:
		static CryptoSpi::CKeyPair* CreateKeyPairL(const CDHKeyPair& aKeyPair);
		static CryptoSpi::CKey* CreateKeyL(const CDHPrivateKey& aPrivateKey);
		static CryptoSpi::CKey* CreateKeyL(const CDHPublicKey& aPublicKey);

		static CryptoSpi::CKeyPair* CreateKeyPairL(const CDSAKeyPair& aKeyPair);
		static CryptoSpi::CKey* CreateKeyL(const CDSAPrivateKey& aPrivateKey);
		static CryptoSpi::CKey* CreateKeyL(const CDSAPublicKey& aPublicKey);

		static CryptoSpi::CKeyPair* CreateKeyPairL(const CRSAKeyPair& aKeyPair);
		static CryptoSpi::CKey* CreateKeyL(const CRSAPrivateKeyStandard& aPrivateKey);
		static CryptoSpi::CKey* CreateKeyL(const CRSAPrivateKeyCRT& aPrivateKey);
		static CryptoSpi::CKey* CreateKeyL(const CRSAPrivateKey& aPrivateKey);
		static CryptoSpi::CKey* CreateKeyL(const CRSAPublicKey& aPublicKey);
	};

#endif	//	__KEYCONVERTER_H__
