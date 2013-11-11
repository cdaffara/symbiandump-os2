/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* hmac implementation
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOAPI_SOFTWAREHMACIMPL_H__
#define __CRYPTOAPI_SOFTWAREHMACIMPL_H__

#include "softwarehashbase.h"
#include <cryptospi/keys.h>

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	const TInt KHMacPad = 64;		
	
	NONSHARABLE_CLASS(CHMacImpl) : public CBase, public MSoftwareHash
		{		
	public:
		//NewL & NewLC
		static CHMacImpl* NewL(MSoftwareHash* aHash);
		static CHMacImpl* NewLC(MSoftwareHash* aHash);
		static CHMacImpl* NewL(const CKey& aKey, MSoftwareHash* aHash);
		static CHMacImpl* NewLC(const CKey& aKey, MSoftwareHash* aHash);

		//From MPlugin
		void Reset();
		void Close();		
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);		
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		TAny* GetExtension(TUid aExtensionId);
		
		//From MHmac
	    TPtrC8 Hash(const TDesC8& aMessage);
	    void Update(const TDesC8& aMessage);
		TPtrC8 Final(const TDesC8& aMessage);
		void SetKeyL(const CKey& aKey);
		void SetOperationModeL(TUid aOperationMode);
		MHash* ReplicateL();		
		MHash* CopyL();
				
	private:
		//Constructors
		CHMacImpl();
		CHMacImpl(const CHMacImpl& aHMacImpl);
		
		//Destructor
		~CHMacImpl();
		void RestoreState();
		void StoreState();
		void ConstructL(const CKey& aKey, MSoftwareHash* aHash);
		void ConstructL(MSoftwareHash* aHash);
		void Initialise(const TDesC8& aKey);
		
	private:
		TUid iImplementationUid;
		MSoftwareHash* iDigest;
		TBuf8<KHMacPad> iInnerPad;
		TBuf8<KHMacPad> iOuterPad;

		TBuf8<KHMacPad> iInnerPadCopy;
		TBuf8<KHMacPad> iOuterPadCopy;
		};
	}

#endif // __CRYPTOAPI_SOFTWAREHMACIMPL_H__
