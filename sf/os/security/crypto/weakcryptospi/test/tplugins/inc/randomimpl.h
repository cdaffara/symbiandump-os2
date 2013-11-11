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
* Software random implementation
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

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CRandomImpl): public CBase, public MRandom
		{
	public:
		// NewL & NewLC
		static CRandomImpl* NewL(TUid aImplementationUid);
		static CRandomImpl* NewLC(TUid aImplementationUid);
		
		// From MPlugin
		void Reset();
		void Close();
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		TAny* GetExtension(TUid aExtensionId);
		TUid ImplementationUid() const;
		
		// From MRandom
		void GenerateRandomBytesL(TDes8& aDest);
		
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();

	private:
		//Constructor
		CRandomImpl(TUid aImplementationUid);
		
		//Destructor
		~CRandomImpl();
		
	private:
		TUid iImplementationUid;	
		};
	}

#endif // __CRYPTOAPI_RANDOMIMPL_H__
