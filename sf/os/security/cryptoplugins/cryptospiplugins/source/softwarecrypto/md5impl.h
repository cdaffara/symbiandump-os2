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
* Software md5 implementation
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOAPI_SOFTWAREMD5IMPL_H__
#define __CRYPTOAPI_SOFTWAREMD5IMPL_H__

#include "softwarehashbase.h"

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	const TInt KMD5BlockSize = 16;
	const TInt KMD5HashSize = 16;	
	
	NONSHARABLE_CLASS(CMD5Impl) : public CBase, public MSoftwareHash
		{		
	public:
		//NewL & NewLC	
		static CMD5Impl* NewL();
		static CMD5Impl* NewLC();
		
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
		CMD5Impl();
		CMD5Impl(const CMD5Impl& aCMD5Impl);
		
		//Destructor
		~CMD5Impl();
		
		TUid ImplementationUid();
		
		void DoUpdate(const TUint8* aData,TUint aLength);
		void DoFinal(void);
		void Block();
	
	private:
		TBuf8<KMD5HashSize> iHash;

		TUint iA;
		TUint iB;
		TUint iC;
		TUint iD;
		TUint iNl;
		TUint iNh;
		TUint iData[KMD5BlockSize];

		TUint iACopy;
		TUint iBCopy;
		TUint iCCopy;
		TUint iDCopy;
		TUint iNlCopy;
		TUint iNhCopy;
		TUint iDataCopy[KMD5BlockSize];
		};
	}

#endif //  __CRYPTOAPI_SOFTWAREMD5IMPL_H__

