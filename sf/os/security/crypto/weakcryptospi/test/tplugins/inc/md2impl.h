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

#ifndef __CRYPTOAPI_SOFTWAREMD2IMPL_H__
#define __CRYPTOAPI_SOFTWAREMD2IMPL_H__

#include "softwarehashbase.h"

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	const TInt KMD2BlockSize = 16;
	const TInt KMD2HashSize = 16;
		
	NONSHARABLE_CLASS(CMD2Impl) : public CBase, public MSoftwareHash
		{		
	public:
		//NewL & NewLC	
		static CMD2Impl* NewL(TUid aImplementationUid);
		static CMD2Impl* NewLC(TUid aImplementationUid);
		
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
		CMD2Impl(TUid aImplementationUid);
		CMD2Impl(const CMD2Impl& aCMD2Impl);
		
		//Destructor
		~CMD2Impl();
		
		TUid ImplementationUid();
		
		void DoUpdate(const TUint8* aData,TUint aLength);
		void DoFinal(void);
		void Block(const TUint8* aData);

		
	private:
	
		TUid iImplementationUid;
	
		TBuf8<KMD2HashSize> iHash;		
		TInt iNum;
		TUint8 iData[KMD2BlockSize];
		TUint iChecksum[KMD2BlockSize];
		TUint iState[KMD2BlockSize];
		TUint8 iDataTemp[KMD2BlockSize];
		TUint iChecksumTemp[KMD2BlockSize];
		TUint iStateTemp[KMD2BlockSize];
		};
	}

#endif //__CRYPTOAPI_SOFTWAREMD2IMPL_H__
