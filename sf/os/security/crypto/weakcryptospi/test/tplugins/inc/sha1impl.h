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
* Software md2 implementation
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOAPI_SOFTWARESHA1IMPL_H__
#define __CRYPTOAPI_SOFTWARESHA1IMPL_H__

#include "softwarehashbase.h"

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	const TInt KSHA1BlockSize = 16;
	const TInt KSHA1HashSize = 20;
			
	NONSHARABLE_CLASS(CSHA1Impl) : public CBase, public MSoftwareHash
		{		
	public:
		//NewL & NewLC	
		static CSHA1Impl* NewL(TUid aImplmentationUid);
		static CSHA1Impl* NewLC(TUid aImplmentationUid);
		
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
		CSHA1Impl(TUid aImplmentationUid);
		CSHA1Impl(const CSHA1Impl& aSHA1Impl);
		
		//Destructor
		~CSHA1Impl();
		
		TUid ImplementationUid();
		
		void DoUpdate(const TUint8* aData,TUint aLength);
		void DoFinal(void);
		void Block();

		
	private:
		TUid iImplementationUid;
	
		TBuf8<KSHA1HashSize> iHash;
		TUint iA;
		TUint iB;
		TUint iC;
		TUint iD;
		TUint iE;
		TUint iNl;
		TUint iNh;
		TUint iData[KSHA1BlockSize*5];

		TUint iACopy;
		TUint iBCopy;
		TUint iCCopy;
		TUint iDCopy;
		TUint iECopy;
		TUint iNlCopy;
		TUint iNhCopy;	
		TUint iDataCopy[KSHA1BlockSize*5];
		CExtendedCharacteristics* iExtendedCharacteristics;
		};
	}
#endif // __CRYPTOAPI_SOFTWARESHA1IMPL_H__
