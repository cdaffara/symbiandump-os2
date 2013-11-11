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
#ifndef __CRYPTOAPI_RANDOMIMPL_H__
#define __CRYPTOAPI_RANDOMIMPL_H__

#include <cryptospi/cryptoplugin.h>
#include <cryptospi/randomplugin.h>
#include "cryptodriver.h"

#ifdef __MARM__
#define __NOTSHARED __declspec(notshared)
#else
#define __NOTSHARED
#endif

namespace HwCrypto
	{
	using namespace CryptoSpi;
	
	//	NONSHARABLE_CLASS(CRandomImpl): public CBase, public MRandom
	class __NOTSHARED CRandomImpl : public CBase, public MRandom
		{
	public:
		// NewL & NewLC
		static CRandomImpl* NewL();
		static CRandomImpl* NewLC();
		
		// From MPlugin
		void Reset();
		void Close();
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		static CExtendedCharacteristics* StaticGetExtendedCharacteristicsL();


		TAny* GetExtension(TUid aExtensionId);
		TUid ImplementationUid() const;
		
		// From MRandom
		void GenerateRandomBytesL(TDes8& aDest);

	private:
		//Constructor
		CRandomImpl();
		
		//Destructor
		~CRandomImpl();
		
	private:
		void ConstructL();

		TUid iImplementationUid;
		RCryptoDriver iCryptoDriver;
		};
	}

#endif // __CRYPTOAPI_RANDOMIMPL_H__
