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
 @internalComponent
 @released
*/
#ifndef __CRYPTOAPI_H4PLUGINCONFIG_H__
#define __CRYPTOAPI_H4PLUGINCONFIG_H__

#include <cryptospi/cryptoparams.h>
#include <e32cmn.h>
#include <cryptospi/romlit.h>
#include "cryptospi/cryptospidef.h"

namespace HwCrypto
	{
	using namespace CryptoSpi;

	/**
	Creator Name
	*/
	_ROMLIT16(KSymbian, "Symbian");
	
	/**
	implementation Name
	*/
	_ROMLIT16(KRandomDescription, "Random");
	_ROMLIT16(KAesDescription, "Aes");


	/**
	For each crypto algorithm implemented in this plugin define
		a plugin-wide unique id to identify it
		a UID that contains the above id
		a table of characteristics
	*/	
	
	
	/**
	implementation 7 Random
	*/
	static const TInt32 KCryptoPluginRandom = 0x20001CF3;
	static const TUid KCryptoPluginRandomUid={KCryptoPluginRandom};
	static const TRandomCharacteristics KRandom_1 = {KRandomInterface, KAlgorithmRandom, KCryptoPluginRandom, &KSymbian, EFalse, ETrue, 6, &KRandomDescription, 100, 90, 3};	


	/**
	implementation 10 AES
	*/
	static const TInt32 KCryptoPluginAes = 0x20001CF4;
	static const TUid KCryptoPluginAesUid={KCryptoPluginAes};
	static const TInt32 KAesPaddingModes[]={KPaddingModeNone, KPaddingModeSSLv3, KPaddingModePKCS7};
	static const TInt32 KAesCryptoModes[]={KOperationModeNone, KOperationModeECB, KOperationModeCBC};
	
	static const TSymmetricCipherCharacteristics KAes_1 = {KSymmetricCipherInterface, KAlgorithmCipherAes, KCryptoPluginAes, &KSymbian, EFalse, ETrue, 6, &KAesDescription, 100, 90, 
	256, 128, KAesPaddingModes, sizeof(KAesPaddingModes)/sizeof(KAesPaddingModes[0]), KAesCryptoModes, sizeof(KAesCryptoModes)/sizeof(KAesCryptoModes[0]), 3};



	/**
	For each crypto algorithm type implemented in this plugin list the characteristics
	*/
	

	/**
	Random implementation characteristic table
	*/
	
	static const TRandomCharacteristics* const KRandomCharacteristics[]=
		{
		&KRandom_1	
		};
		
	/**
	Symmetric Cipher implementation characteristic table
	*/
	static const TSymmetricCipherCharacteristics* const KSymmetricCipherCharacteristics[]=
		{
		&KAes_1
		};

	}

#endif //__CRYPTOAPI_H4PLUGINCONFIG_H__
