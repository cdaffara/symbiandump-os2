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
* plugin entry interface & ordinal definition
*
*/


/**
 @file
 @publishedPartner
 @released
*/
 
#ifndef __CRYPTOAPI_PLUGINENTRYDEF_H__
#define __CRYPTOAPI_PLUGINENTRYDEF_H__

#include <cryptospi/plugincharacteristics.h>

namespace CryptoSpi
	{
	class CCryptoParam;
	class CCryptoParams;
	class MRandom;
	class MHash;
	class MSymmetricCipher;
	class MAsymmetricCipher;
	class MSigner;
	class MVerifier;
	class MKeyAgreement;
	class MKeyPairGenerator;
	class MAsyncRandom;
	class MAsyncHash;
	class MAsyncSymmetricCipher;
	class MAsyncAsymmetricCipher;
	class MAsyncSigner;
	class MAsyncVerifier;
	class MAsyncKeyAgreement;
	class MAsyncKeyPairGenerator;
	class CKey;
	class CExtendedCharacteristics;
	
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
	class MMac;
	class MAsyncMac;
#endif	
	
	/**
	Crypto plugin entry points ordinals in the plugin DLLs
	*/
	enum TPluginEntryOrdinal
		{
		/**
		Enumeration function ordinal
		*/
		EEnumerateCharacteristicsOrdinal=1,
		/**
		Get Extended Characteristic function ordinal
		*/		
		EGetExtendedCharacteristicOrdinal=2,
		/**
		Asymmetric Cipher creation function ordinal
		*/		
		ECreateAsymmetricCipherOrdinal=3,
		/**
		Signer creation function ordinal
		*/				
		ECreateSignerOrdinal=4,
		/**
		Verifier creation function ordinal
		*/						
		ECreateVerifierOrdinal=5,
		/**
		Hash creation function ordinal
		*/								
		ECreateHashOrdinal=6,
		/**
		Key agreement creation function ordinal
		*/								
		ECreateKeyAgreementOrdinal=7,
		/**
		Key Pair Generator creation function ordinal
		*/								
		ECreateKeyPairGeneratorOrdinal=8,
		/**
		Random creation function ordinal
		*/										
		ECreateRandomOrdinal=9,
		/**
		Symmetric Cipher creation function ordinal
		*/		
		ECreateSymmetricCipherOrdinal=10,
        /**
		Symmetric key generator creation function ordinal
		*/				
		ECreateSymmetricKeyGeneratorOrdinal=11,
		/**
		Asynchronous Asymmetric Cipher creation function ordinal
		*/					
		ECreateAsyncAsymmetricCipherOrdinal=12,
		/**
		Asynchronous Signer creation function ordinal
		*/						
		ECreateAsyncSignerOrdinal=13,
		/**
		Asynchronous Verifier creation function ordinal
		*/								
		ECreateAsyncVerifierOrdinal=14,
		/**
		Asynchronous Hash creation function ordinal
		*/										
		ECreateAsyncHashOrdinal=15,
		/**
		Asynchronous Key agreement creation function ordinal
		*/
		ECreateAsyncKeyAgreementOrdinal=16,
		/**
		Asynchronous Key Pair Generator creation function ordinal
		*/								
		ECreateAsyncKeyPairGeneratorOrdinal=17,
		/**
		Asynchronous Random creation function ordinal
		*/										
		ECreateAsyncRandomOrdinal=18,
		/**
		Asynchronous Symmetric Cipher creation function ordinal
		*/		
		ECreateAsyncSymmetricCipherOrdinal=19,
		/**
		Asynchronous Symmetric key generator creation function ordinal
		*/		
		ECreateAsyncSymmetricKeyGeneratorOrdinal=20,
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT			
		/**
		Mac creation function ordinal
		*/								
		ECreateMacOrdinal=21,
		/**
		Asynchronous Mac creation function ordinal
		*/										
		ECreateAsyncMacOrdinal=22,
		/**
		Hash creation function ordinal
		*/								
		ECreateHashOrdinalv2=23,
		/**
		Asynchronous Mac creation function ordinal
		*/										
		ECreateAsyncMacOrdinalv2=24,
#endif		
		};
	
	/**
	Crypto Plugin entry function definition
	*/
	/**
	The enumeration function definition 
	@param TUid the interface Uid
	@param TInt& the number of the characteristics
	@return the pointer to characteristics
	*/
	typedef const TCharacteristics** (*EnumerateCharacteristicsFunc)(TUid, TInt&);
	
	/**
	The Extended characteristic function definition 
	@param TUid the implementation Uid
	@param CCryptoParams* the extended characteristics
	*/	
	typedef void (*GetExtendedCharacteristicsFuncL)(TUid, CExtendedCharacteristics*&);
	
	/**
	Random object Creation function
	@param MRandom*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/
	typedef void (*CreateRandomFuncL)(MRandom*&, TUid, const CCryptoParams*);
	
	/**
	 * @deprecated
	 *  
	 * Hash object Creation function
	 * @param	MHash*& the new created plugin instance, should not be set if leaves with error
	 * @param 	CCryptoParams* the extra paramters for creation.
	 * @leave 	System wide error code.
	 */	
	typedef void (*CreateHashFuncL)(MHash*&, TUid, TUid, const CKey*, const CCryptoParams*);
	
	/**
	Symmetric Cipher object Creation function
	@param MSymmetricCipher*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/	
	typedef void (*CreateSymmetricCipherFuncL)(MSymmetricCipher*&, TUid, const CKey&, TUid, TUid, TUid, const CCryptoParams*);
	
	/**
	Asymmetric Cipher object Creation function
	@param MAsymmetricCipher*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/	
	typedef void (*CreateAsymmetricCipherFuncL)(MAsymmetricCipher*&, TUid, const CKey&, TUid, TUid, const CCryptoParams*);

	/**
	Signer object Creation function
	@param MSigner*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/		
	typedef void (*CreateSignerFuncL)(MSigner*&, TUid, const CKey&, TUid, const CCryptoParams*);
	
	/**
	Verifier object Creation function
	@param MVerifier*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/		
	typedef void (*CreateVerifierFuncL)(MVerifier*&, TUid, const CKey&, TUid, const CCryptoParams*);

	/**
	KeyAgreement object Creation function
	@param MKeyAgreement*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/			
	typedef void (*CreateKeyAgreementFuncL)(MKeyAgreement*&, TUid, const CKey&, const CCryptoParams*);
	
	/**
	Key Pair Generator object Creation function
	@param MKeyPairGenerator*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/	
	typedef void (*CreateKeyPairGeneratorFuncL)(MKeyPairGenerator*&, TUid, const CCryptoParams*);
	
	/**
	Asynchronous Random object Creation function
	@param MRandom*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/
	//typedef void (*CreateAsyncRandomFuncL)(MAsyncRandom*&, TUid, const CCryptoParams*);

	/**
	 * @deprecated
	 * 
	 * Asynchronous Hash object Creation function
	 * @param MHash*& the new created plugin instance, should not be set if leaves with error
	 * @param CCryptoParams* the extra paramters for creation.
	 * @leave System wide error code.
	 */		
	//typedef void (*CreateAsyncHashFuncL)(MAsyncHash*&, TUid, TUid, const CKey*, const CCryptoParams*);

	/**
	Asynchronous Symmetric Cipher object Creation function
	@param MSymmetricCipher*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/	
	//typedef void (*CreateAsyncSymmetricCipherFuncL)(MAsyncSymmetricCipher*&, TUid, const CKey&, TUid, TUid, TUid, const CCryptoParams*);
	
	/**
	Asynchronous Asymmetric Cipher object Creation function
	@param MAsymmetricCipher*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/	
	//typedef void (*CreateAsyncAsymmetricCipherFuncL)(MAsyncAsymmetricCipher*&, TUid, const CKey&, TUid, TUid, const CCryptoParams*);
	
	/**
	Asynchronous Signer object Creation function
	@param MSigner*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/
	//typedef void (*CreateAsyncSignerFuncL)(MAsyncSigner*&, TUid, const CKey&, TUid, const CCryptoParams*);
	
	/**
	Asynchronous Verifier object Creation function
	@param MVerifier*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/			
	//typedef void (*CreateAsyncVerifierFuncL)(MAsyncVerifier*&, TUid, const CKey&, TUid, const CCryptoParams*);
	
	/**
	Asynchronous KeyAgreement object Creation function
	@param MKeyAgreement*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/	
	//typedef void (*CreateAsyncKeyAgreementFuncL)(MAsyncKeyAgreement*&, TUid, const CKey&, const CCryptoParams*);

	/**
	Asynchronous Key Pair Generator object Creation function
	@param MKeyPairGenerator*& the new created plugin instance, should not be set if leaves with error
	@param CCryptoParams* the extra paramters for creation.
	@leave System wide error code.
	*/
	//typedef void (*CreateAsyncKeyPairGeneratorFuncL)(MAsyncKeyPairGenerator*&, TUid, const CCryptoParams*);

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	
	/**
	 * Mac object Creation function
	 *
	 * @param MMac*&   		 the new created plugin instance, should not be set if leaves with error
	 * @param TUid   		 the implementation Uid
	 * @param aKey   		 Symmetric key for calculating message authentication code value. 
	 * @param CCryptoParams* the extra parameters for creation.
	 * @leave System wide error code.
	 */	
	typedef void (*CreateMacFuncL)(MMac*&,const TUid, const CKey&, const CCryptoParams*);
	
	/**
	 * Mac object Creation function
	 *
	 * @param MAsyncMac*&    the new created plugin instance, should not be set if leaves with error
	 * @param TUid   		 the implementation Uid
	 * @param aKey   		 Symmetric key for calculating message authentication code value. 
	 * @param CCryptoParams* the extra parameters for creation.
	 * @leave System wide error code.
	 */	
	//typedef void (*CreateAsyncMacFuncL)(MAsyncMac*&,const TUid, const CKey&, const CCryptoParams*);
	

	/**
	 * Hash object Creation function
	 * @param MHash*& the new created plugin instance, should not be set if leaves with error
	 * @param CCryptoParams* the extra paramters for creation.
	 * @leave System wide error code.
	 */	
	typedef void (*CreateHashFuncLv2)(MHash*&, TUid, const CCryptoParams*);

	/**
	 * Hash object Creation function
	 * @param MAsyncHash*& the new created plugin instance, should not be set if leaves with error
	 * @param CCryptoParams* the extra paramters for creation.
	 * @leave System wide error code.
	 */	
	//typedef void (*CreateAsyncHashFuncLv2)(MAsyncHash*&, TUid, const CCryptoParams*);
#endif	
	}

#endif //__CRYPTOAPI_PLUGINENTRYDEF_H__
