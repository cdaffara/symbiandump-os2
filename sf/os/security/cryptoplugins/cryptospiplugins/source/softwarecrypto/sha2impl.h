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


#ifndef __CRYPTOAPI_SOFTWARESHA2IMPL_H_
#define __CRYPTOAPI_SOFTWARESHA2IMPL_H_


#include "softwarehashbase.h"

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	class MSHA2Impl;
	
	NONSHARABLE_CLASS(CSHA2Impl) : public CBase, public MSoftwareHash
		{
	public:
		//NewL & NewLC	
		static CSHA2Impl* NewL(TInt32 aAlgorithmId);
		static CSHA2Impl* NewLC(TInt32 aAlgorithmId);
		
		//From MPlugin
		void Reset();
		void Close();		
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);		
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		TAny* GetExtension(TUid aExtensionId);
		
		//From MHash
		TPtrC8 Hash(const TDesC8& aMessage);
		void Update(const TDesC8& aMessage);
		TPtrC8 Final(const TDesC8& aMessage);
		MHash* ReplicateL();		
		MHash* CopyL();
		void SetKeyL(const CKey& aKey);
		void SetOperationModeL(TUid aOperationMode);		
		
		//From MSoftwareHash
		void RestoreState();
		void StoreState();
		
	private:
		//Constructors
		void ConstructL(TInt32 aAlgorithmId);
		void ConstructL(const CSHA2Impl& aSHA512Impl);
		
		//Destructor
		~CSHA2Impl();
		
		TUid ImplementationUid();

	private:
		MSHA2Impl*	iImplementation;
		const TAny*	iInitValues;
		TUid		iImplementationUid;
		TUint		iHashSize;
		};
	}

#endif //__CRYPTOAPI_SOFTWARESHA2IMPL_H_
