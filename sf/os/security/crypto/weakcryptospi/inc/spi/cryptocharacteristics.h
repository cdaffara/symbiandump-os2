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
* crypto characteristics
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOCHARACTERISTICS_H__
#define __CRYPTOCHARACTERISTICS_H__

#include <e32base.h>
#include <e32hashtab.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/cryptobaseapi.h>


namespace CryptoSpi
	{
	
	/**
	The class holds the common characteristics of an interface.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CCharacteristics) : public CBase
		{
	public:	
		/**
		Destructor
		*/
		virtual ~CCharacteristics();
		
		/**
		Internalize this object from the supplied stream.

		@param	aStream Stream to read CCharacteristics from.
		*/
		virtual void InternalizeL(RReadStream& aStream);
		
	protected:
		/**
		 * @internalComponent
		 *
		 * Constructor
		 */
		CCharacteristics();
	
	public:
		/**
		Interface UID, e.g hash, symmetric cipher
		*/
		TInt32 iInterfaceUid;

		/**
		Algorithm UID, e.g MD2, 3DES 
		*/
		TInt32 iAlgorithmUid;

		/**
		Implementaion UID
		*/
		TInt32 iImplementationUid;

		/**
		Creator name
		*/	
		RBuf iCreatorName;

		/**
		whether the implementation is FIPS compliant
		*/
		TBool iIsFIPSApproved;

		/**
		whether the implementation is a hardware one
		*/
		TBool iIsHardwareSupported;

		/**
		the Maximum concurrent operation supported
		*/
		TUint iMaxConcurrencySupported;
		
		/**
		the Algorithm Name
		*/
		RBuf iAlgorithmName;
		
		/**
		The Latency
		*/
		TInt iLatency;
		
		/**
		the Throughput
		*/
		TInt iThroughput;
		};

	/**
	The class holds the hash characteristics.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CHashCharacteristics) : public CCharacteristics
		{
	public:
		/**
		@internalComponent
		
		Create a new instance of CHashCharacteristics.
		
		@return the pointer of the hash characteristic
		*/
		static CHashCharacteristics* NewL();

		/**
		@internalComponent

		Create a new instance of CHashCharacteristics, 
		Leave it on the cleanup stack.
		
		@return the pointer of the hash characteristic
		*/			
		static CHashCharacteristics* NewLC();
		
		/**
		Destructor
		*/
		virtual ~CHashCharacteristics();
		
		/**
		Internalize this object from the supplied stream.

		@param	aStream Stream to read CHashCharacteristics from.
		*/
		virtual void InternalizeL(RReadStream& aStream);

		/**
		 * @internalComponent
		 *
		 * Determines whether an operation mode (e.g. HMAC) is supported by the plug-in 
		 * by searching through iSupportedOperationModes for the UID value corresponding
		 * to the aOperationMode parameter.
		 * @param aOperationMode	The UID of the operation mode to test
		 * @return ETrue if the operation mode is supported; otherwise, EFalse is returned.
		 */
		TBool IsOperationModeSupported(TUid aOperationMode) const;
		
	private:
		/**
		Constructor
		*/	
		CHashCharacteristics();
	public:
		/**
		The internal block size of the hash in bits
		*/
		TUint iBlockSize;

		/**
		the output size of the hash in bytes
		*/
		TUint iOutputSize;
		
		/**
		The list of supported Operation modes
		*/
		RArray<TInt32> iSupportedOperationModes;
		};

		
#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
		
	/**
	 * The class holds MAC characteristics which is used for internalization
	 * in CryptoSpi framework.
	 */
	NONSHARABLE_CLASS(CMacCharacteristics) : public CCharacteristics
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Create a new instance of CMacCharacteristics.
		 *
		 * @return the pointer of the Mac characteristic  
		 */
		static CMacCharacteristics* NewL();

		/**
		 * @internalComponent
		 * 
		 * Create a new instance of CMacCharacteristics, Leave it on the cleanup stack.
		 * 
		 * @return the pointer of the Mac characteristic  
	     */			
		static CMacCharacteristics* NewLC();
			
		/**
		 * Destructor
		 */
		virtual ~CMacCharacteristics();
			
		/**
		 * Internalize this object from the supplied stream.
		 * 
		 * @param	aStream Stream to read CMacCharacteristics from. 
         */
		virtual void InternalizeL(RReadStream& aStream);
			
	private:
		/**
		 * Constructor
		 */	
		CMacCharacteristics();

	public:
		/**
		 * The Mac mode : Hash based, Cipher based etc.
		 */
		TInt32 iMacMode;
	
		/**
		 * Characteristics of the underlying MAC algorithm
		 */ 
		CCharacteristics* iAlgorithmChar;	
		};
		
#endif		
		
	/**
	The class holds the random characteristics.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CRandomCharacteristics) : public CCharacteristics
		{
	public:
		/**
		@internalComponent
		
		Create a new instance of CRandomCharacteristics.
		
		@return the pointer of the random characteristic
		*/	
		static CRandomCharacteristics* NewL();
		
		/**
		@internalComponent

		Create a new instance of CRandomCharacteristics,
		and leave it on the cleanup stack.
		
		@return the pointer of the random characteristic
		*/			
		static CRandomCharacteristics* NewLC();
		
		/**
		Destructor
		*/
		virtual ~CRandomCharacteristics();
		
		/**
		Internalize this object from the supplied stream.

		@param	aStream Stream to read CRandomCharacteristics from.
		*/		
		virtual void InternalizeL(RReadStream& aStream);
		
	private:
		/**
		Constructor
		*/
		CRandomCharacteristics();
		
	public:
		/**
		The bit map of blocking non-blocking mode
		*/
		TUint iBlockingMode;
		};

	/**
	The class holds the symmetric cipher characteristics.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CSymmetricCipherCharacteristics) : public CCharacteristics
		{
	public:
		/**
		@internalComponent
		
		Create a new instance of CSymmetricCipherCharacteristics.
		
		@return the pointer of the symmetric cipher characteristic
		*/
		static CSymmetricCipherCharacteristics* NewL();
		
		/**
		@internalComponent
		
		Create a new instance of CSymmetricCipherCharacteristics.
		Leave it on the cleanup stack.
		
		@return the pointer of the symmetric cipher characteristic
		*/
		static CSymmetricCipherCharacteristics* NewLC();
		
		/**
		Destructor
		*/
		virtual ~CSymmetricCipherCharacteristics();
		
		/**
		Internalize this object from the supplied stream.

		@param	aStream Stream to read CSymmetricCipherCharacteristics from.
		*/				
		virtual void InternalizeL(RReadStream& aStream);

		/**
		@internalComponent
		
		Determines whether a padding mode (e.g. PKCS7) is supported by the plug-in 
		by searching through iSupportedPaddingModes for the UID value corresponding
		to the aPaddingMode parameter.
		@param aPaddingMode	The UID of the padding mode to test.
		@return ETrue if the padding mode is supported; otherwise, EFalse is returned.
		*/
		TBool IsPaddingModeSupported(TUid aPaddingMode) const;
		
		/**
		@internalComponent

		Determines whether an operation mode (e.g. HMAC) is supported by the plug-in 
		by searching through iSupportedOperationModes for the UID value corresponding
		to the aOperationMode parameter.
		@param aOperationMode	The UID of the operation mode to test
		@return ETrue if the operation mode is supported; otherwise, EFalse is returned.
		*/
		TBool IsOperationModeSupported(TUid aOperationMode) const;
		
	private:
		/**
		Constructor
		*/
		CSymmetricCipherCharacteristics();
		
	public:
		/**
		The maximum key length in bits
		*/
		TUint iMaximumKeyLength;

		/**
		The block size of the cipher in bytes
		*/
		TUint iBlockSize;

		/**
		The list of supported padding modes
		*/
		RArray<TInt32> iSupportedPaddingModes;

		/**
		The list of supported Operation modes
		*/
		RArray<TInt32> iSupportedOperationModes;
		
		/**
		The supported key mode bit map. e.g. extern key, internal key or both
		*/
		TInt iKeySupportMode;		
		};

	/**
	The class holds the asymmetric cipher characteristics.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CAsymmetricCipherCharacteristics) : public CCharacteristics
		{
	public:
		/**
		@internalComponent

		Create a new instance of CAsymmetricCipherCharacteristics.
		
		@return the pointer of the asymmetric cipher characteristic
		*/	
		static CAsymmetricCipherCharacteristics* NewL();
		
		/**
		@internalComponent
		
		Create a new instance of CAsymmetricCipherCharacteristics.
		Leave it on the cleanup stack.
		
		@return the pointer of the asymmetric cipher characteristic
		*/
		static CAsymmetricCipherCharacteristics* NewLC();
		
		/**
		Destructor
		*/
		virtual ~CAsymmetricCipherCharacteristics();
		
		/**
		Internalize this object from the supplied stream.

		@param	aStream Stream to read CAsymmetricCipherCharacteristics from.
		*/		
		virtual void InternalizeL(RReadStream& aStream);
		
		/**
		@internalComponent
		
		Determines whether a padding mode (e.g. PKCS7) is supported by the plug-in 
		by searching through iSupportedPaddingModes for the UID value corresponding
		to the aPaddingMode parameter.
		@param aPaddingMode	The UID of the padding mode to test.
		@return ETrue if the padding mode is supported; otherwise, EFalse is returned.
		*/
		TBool IsPaddingModeSupported(TUid aPaddingMode) const;

	private:
		/**
		Constructor
		*/
		CAsymmetricCipherCharacteristics();
		
	public:
		/**
		The maximum key length in bits
		*/
		TUint iMaximumKeyLength;
	
		/**
		The list of supported padding modes
		*/
		RArray<TInt32> iSupportedPaddingModes;
		
		/**
		The supported key mode bit map. e.g. extern key, internal key or both
		*/
		TInt iKeySupportMode;						
		};


	/**
	The class holds the signature characteristics.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CAsymmetricSignatureCharacteristics) : public CCharacteristics
		{
	public:
		/**
		@internalComponent
		
		Create a new instance of CAsymmetricSignatureCharacteristics.
		
		@return the pointer of the asymmetric signature characteristic
		*/		
		static CAsymmetricSignatureCharacteristics* NewL();
		
		/**
		@internalComponent
		
		Create a new instance of CAsymmetricSignatureCharacteristics.
		Leave it on the cleanup stack.
		
		@return the pointer of the asymmetric signature characteristic
		*/				
		static CAsymmetricSignatureCharacteristics* NewLC();
		
		/**
		Destructor
		*/
		virtual ~CAsymmetricSignatureCharacteristics();
		
		/**
		Internalize this object from the supplied stream.

		@param	aStream Stream to read CAsymmetricSignatureCharacteristics from.
		*/				
		virtual void InternalizeL(RReadStream& aStream);
		
		/**
		@internalComponent
		
		Determines whether a padding mode (e.g. PKCS7) is supported by the plug-in 
		by searching through iSupportedPaddingModes for the UID value corresponding
		to the aPaddingMode parameter.
		@param aPaddingMode	The UID of the padding mode to test.
		@return ETrue if the padding mode is supported; otherwise, EFalse is returned.
		*/
		TBool IsPaddingModeSupported(TUid aPaddingMode) const;

	private:
		/**
		Constructor
		*/
		CAsymmetricSignatureCharacteristics();
		
	public:
		/**
		The maximum key length in bits
		*/
		TUint iMaximumKeyLength;
	
		/**
		The list of supported padding modes
		*/
		RArray<TInt32> iSupportedPaddingModes;
		
		/**
		The supported key mode bit map. e.g. extern key, internal key or both
		*/
		TInt iKeySupportMode;		
		};

	/**
	The class holds the key agreement characteristics.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CKeyAgreementCharacteristics) : public CCharacteristics
		{
	public:
		/**
		@internalComponent
		
		Create a new instance of CKeyAgreementCharacteristics.
		
		@return the pointer of the key agreement characteristic
		*/			
		static CKeyAgreementCharacteristics* NewL();
		
		/**
		@internalComponent

		Create a new instance of CKeyAgreementCharacteristics.
		Leave it on the cleanup stack.
		
		@return the pointer of the key agreement characteristic
		*/
		static CKeyAgreementCharacteristics* NewLC();

		/**
		Destructor
		*/
		virtual ~CKeyAgreementCharacteristics();
		
		/**
		Internalize this object from the supplied stream.

		@param	aStream Stream to read CKeyAgreementCharacteristics from.
		*/
		virtual void InternalizeL(RReadStream& aStream);
		
	private:
		/**
		Constructor
		*/	
		CKeyAgreementCharacteristics();
		};

	/**
	The class holds the key pair generator characteristics.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CKeypairGeneratorCharacteristics) : public CCharacteristics
		{
	public:
		/**
		@internalComponent
		
		Create a new instance of CKeypairGeneratorCharacteristics.
		
		@return the pointer of the keypair generator characteristic
		*/	
		static CKeypairGeneratorCharacteristics* NewL();
		
		/**
		@internalComponent
		
		Create a new instance of CKeypairGeneratorCharacteristics.
		Leave it on the cleanup stack.
		
		@return the pointer of the keypair generator characteristic
		*/
		static CKeypairGeneratorCharacteristics* NewLC();

		/**
		Destructor
		*/
		virtual ~CKeypairGeneratorCharacteristics();
		
		/**
		Internalize this object from the supplied stream.

		@param	aStream Stream to read CKeyAgreementCharacteristics from.
		*/		
		virtual void InternalizeL(RReadStream& aStream);
		
	private:
		/**
		Constructor
		*/		
		CKeypairGeneratorCharacteristics();
	public:
		/**
		The maximum key length in bits
		*/
		TUint iMaximumKeyLength;
		};

	/**
	The class holds characteristics and dll index.
	which is used for internalization.
	*/
	NONSHARABLE_CLASS(CCharacteristicsAndPluginName) : public CBase
		{
	public:
		/**
		@internalComponent
		
		Create a new instance of CKeypairGeneratorCharacteristics.
		
		@param aInterface the interface uid
		@return the pointer of the keypair generator characteristic
		*/		
		static CCharacteristicsAndPluginName* NewL(TInt32 aInterface);
		
		/**
		@internalComponent

		Create a new instance of CKeypairGeneratorCharacteristics.
		Leave it on the cleanup stack.
		
		@param aInterface the interface uid
		@return the pointer of the keypair generator characteristic
		*/				
		static CCharacteristicsAndPluginName* NewLC(TInt32 aInterface);
		
		/**
		Destructor
		*/		
		virtual ~CCharacteristicsAndPluginName();
		
		/**
		@internalComponent

		Internalize this object from the supplied stream.

		@param	aStream Stream to read CCharacteristicsAndPluginName from.
		*/		
		void InternalizeL(RReadStream& aStream);
		
	protected:
		/**
		 * @internalComponent
		 *
		 * Constructors
		 */
		CCharacteristicsAndPluginName();
		/** @internalComponent */
		void ConstructL(TInt32 aInterface);
		
	public:
		/**
		The characteristic of the crypto implementation
		*/
		CCharacteristics* iCharacteristic;
		
		/**
		The extended characteristic of the crypto implementation
		*/
		CExtendedCharacteristics* iExtendedCharacteristic;
		
		/**
		The name of the plugin dll which contains this implementation
		*/
		TFileName iDllName;		
		};
	}
#endif //__CRYPTOCHARACTERISTICS_H__



