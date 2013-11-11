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
* legacy plugin selector definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOAPI_LEGACYSELECTOR_H__
#define __CRYPTOAPI_LEGACYSELECTOR_H__

#include <cryptospi/pluginselectorbase.h>
#include <cryptospi/cryptospidef.h>
#include <cryptospi/cryptoplugin.h>
#include <cryptospi/cryptoparams.h>

namespace CryptoSpi
	{
	class CHash;
	class CRandom;
	class CSymmetricCipher;
	class CAsymmetricCipher;
	class CSigner;
	class CVerifier;
	class CKeyAgreement;
	class CSymmetricKeyGenerator;
	class CKeyPairGenerator;
	class CKey;
	class CCryptoParams;
	class CCharacteristicsAndPluginName;

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	class CMac;
#endif
	
	NONSHARABLE_CLASS(CLegacySelector): public CBase, public MPluginSelector
		{
	public:

		/**
		NewL create the legacy selector.
		@return A pointer to a CLegacySelector instance
		*/		
		static CLegacySelector* NewL();
		
		/**
		NewLC create the legacy selector.
		@return A pointer to a CLegacySelector instance
		*/				
		static CLegacySelector* NewLC();
		
		/**
		Destructor
		*/
		~CLegacySelector();
		
		//virtual function from MPluginSelector
		virtual void CreateHashL(CHash*& aHash,
											TUid aAlgorithmUid,
											TUid aOperationMode,
											const CKey* aKey,
											const CCryptoParams* aAlgorithmParams);
											
		//virtual function from MPluginSelector						
		virtual void CreateRandomL(CRandom*& aRandom,
											TUid aAlgorithmUid,
											const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateSymmetricCipherL(CSymmetricCipher*& aCipher,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aCryptoMode,
											TUid aOperationMode,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateAsymmetricCipherL(CAsymmetricCipher*& aCipher,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aCryptoMode,
											TUid aPaddingMode,									
											const CCryptoParams* aAlgorithmParams);
											
		//virtual function from MPluginSelector
		virtual void CreateSignerL(CSigner*& aSigner,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateVerifierL(CVerifier*& aVerifier,
											TUid aAlgorithmUid,
											const CKey& aKey,
											TUid aPaddingMode,
											const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateKeyPairGeneratorL(CKeyPairGenerator*& aKeyPairGenerator,
											TUid aKeyAlgorithmUid,
											const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateKeyAgreementL(CKeyAgreement*& aKeyAgreement,
											TUid aAlgorithmUid,
											const CKey& aPrivateKey,
											const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateAsyncHashL(CAsyncHash*& aHash,
										TUid aAlgorithmUid,
										TUid aOperationMode,
										const CKey* aKey,
										const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateAsyncRandomL(CAsyncRandom*& aRandom,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateAsyncSymmetricCipherL(CAsyncSymmetricCipher*& aCipher,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aCryptoMode,
										TUid aOperationMode,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams);
										
		//virtual function from MPluginSelector
		virtual void CreateAsyncAsymmetricCipherL(CAsyncAsymmetricCipher*& aCipher,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aCryptoMode,
										TUid aPaddingMode,																						
										const CCryptoParams* aAlgorithmParams);
										
		//virtual function from MPluginSelector
		virtual void CreateAsyncSignerL(CAsyncSigner*& aSigner,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams);
										
		//virtual function from MPluginSelector		
		virtual void CreateAsyncVerifierL(CAsyncVerifier*& aVerifier,
										TUid aAlgorithmUid,
										const CKey& aKey,
										TUid aPaddingMode,
										const CCryptoParams* aAlgorithmParams);
										
		//virtual function from MPluginSelector
		virtual void CreateAsyncKeyPairGeneratorL(CAsyncKeyPairGenerator*& aKeyPairGenerator,
										TUid aAlgorithmUid,
										const CCryptoParams* aAlgorithmParams);
										
		//virtual function from MPluginSelector
		virtual void CreateAsyncKeyAgreementL(CAsyncKeyAgreement*& aKeyAgreement,
										TUid aAlgorithmUid,
										const CKey& aPrivateKey,
										const CCryptoParams* aAlgorithmParams);		
				
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT			
		//virtual function from MPluginSelector	
		virtual void CreateMacL(CMac*& aMac,
								const TUid aAlgorithmUid,
								const CKey& aKey,
								const CCryptoParams* aAlgorithmParams);
				
		//virtual function from MPluginSelector
		virtual void CreateAsyncMacL(CAsyncMac*& aMac,
								const TUid aAlgorithmUid,
								const CKey& aKey,
								const CCryptoParams* aAlgorithmParams);
		
		//virtual function from MPluginSelector
		virtual void CreateHashL(CHash*& aHash,
								TUid aAlgorithmUid,
								const CCryptoParams* aAlgorithmParams);

		//virtual function from MPluginSelector
		virtual void CreateAsyncHashL(CAsyncHash*& aAsyncHash, 
								TUid aAlgorithmUid,
								const CCryptoParams* aAlgorithmParams);
#endif				
	private:
		//Constructor
		CLegacySelector();
				
		/**
		Find an implemenation of an algorithmUid
		@param aAlgorithmUid the alogorithm Uid
		@param aImplementationId the implementation Uid
		@param aDllIndex the index of the plugin dll
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		TInt FindPlugin(TUid aAlgorithmUid, TUid& aImplementationId, TInt& aDllIndex);
		TInt FindPlugin(TUid aAlgorithmUid, TUid& aImplementationId, TFileName& aDllName);
	private:
		/**
		Array of the characteristics of an interface
		*/
		RPointerArray<CCharacteristicsAndPluginName> iCharacteristicsAndDllIndex;
		
		/**
		The next characteristic index to try
		*/
		TInt iNextCharacteristic;
		};
	}

#endif //__CRYPTOAPI_LEGACYSELECTOR_H__






