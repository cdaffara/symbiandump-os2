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
* plugin characteristic definition
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_PLUGINCHARACTERISTICS_H__
#define __CRYPTOAPI_PLUGINCHARACTERISTICS_H__

#include <e32base.h>
#include <cryptospi/cryptoparams.h>
#include <s32mem.h>

// Forward declarations
struct TRomLitC16;


namespace CryptoSpi
	{
	const TInt32 KInterfaceType = 0x10283527;
	const TInt32 KAlgorithmType = 0x10283528;
	const TInt32 KImplementationType = 0x10283529;
	const TInt32 KCreatorNameType = 0x1028352A;
	const TInt32 KIsFIPSApprovedType = 0x1028352B;
	const TInt32 KIsHardwareSupportedType = 0x1028352C;
	const TInt32 KMaxConcurrencySupportedType = 0x1028352D;
	const TInt32 KLatencyType = 0x1028352E;
	const TInt32 KThroughputType = 0x1028352F;
	const TInt32 KBlockSizeType = 0x10283530;
	const TInt32 KOutputSizeType = 0x10283531;
	const TInt32 KSupportedOperationModesType = 0x10283532;
	const TInt32 KBlockingModeType = 0x10283533;
	const TInt32 KMaximumKeyLengthType = 0x10283534;
	const TInt32 KSupportedPaddingModesType = 0x10283535;
	const TInt32 KKeySupportModeType = 0x10283536;
	const TInt32 KConcurrencyType = 0x10283537;
	const TInt32 KExclusiveUseType = 0x10283538;
	
	/**
	The following are the characteristic UIDs which should be used for Rules-based Selector
	*/
	const TUid KNoneUid = {0x00000000};
	const TUid KInterfaceTypeUid = {KInterfaceType};
	const TUid KAlgorithmTypeUid = {KAlgorithmType};
	const TUid KImplementationTypeUid = {KImplementationType};
	const TUid KCreatorNameTypeUid = {KCreatorNameType};
	const TUid KIsFIPSApprovedTypeUid = {KIsFIPSApprovedType};
	const TUid KIsHardwareSupportedTypeUid = {KIsHardwareSupportedType};
	const TUid KMaxConcurrencySupportedTypeUid = {KMaxConcurrencySupportedType};
	const TUid KLatencyTypeUid = {KLatencyType};
	const TUid KThroughputTypeUid = {KThroughputType};
	const TUid KBlockSizeTypeUid = {KBlockSizeType};
	const TUid KOutputSizeTypeUid = {KOutputSizeType};
	const TUid KSupportedOperationModesTypeUid = {KSupportedOperationModesType};
	const TUid KBlockingModeTypeUid = {KBlockingModeType};
	const TUid KMaximumKeyLengthTypeUid = {KMaximumKeyLengthType};
	const TUid KSupportedPaddingModesTypeUid = {KSupportedPaddingModesType};
	const TUid KKeySupportModeTypeUid = {KKeySupportModeType};
	const TUid KConcurrencyTypeUid = {KConcurrencyType};
	const TUid KExclusiveUseTypeUid = {KExclusiveUseType};

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT	
	const TInt32 KMacModeType = 0x2001ED9F;
	const TUid KMacModeTypeUid = {KMacModeType};
#endif
	
	
	typedef TAny TCharacteristics;

	/**
	Common characteristics for all plug-in interface types.

	This MUST be the first element of each characteristics "sub-class" to
	allow sub-classes to be cast directly to a TCommonCharacteristics. This
	is used instead of inheritance because it enables arrays of characteristics
	objects to be defined as static constant data.
	*/ 
	class TCommonCharacteristics
		{
	public:
	
		/**
		 * @internalComponent
		 * 
		 * Externalises common characteristics to a write stream.
		 *
		 * @param aStream	Stream to which the object should be externalised.
		 */
		void ExternalizeL(RWriteStream& aStream) const;
	
	public:
		/**
		Interface UID, e.g hash, symmetric cipher
		*/
		TInt32 iInterfaceUID;

		/**
		Algorithm UID, e.g MD2, 3DES 
		*/
		TInt32 iAlgorithmUID;

		/**
		Implementaion UID
		*/
		TInt32 iImplementationUID;

		/**
		Creator name
		*/	
		const TRomLitC16* iCreatorName;

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
		const TRomLitC16* iAlgorithmName;
		
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
	Hash Characteristics
	*/	
	class THashCharacteristics
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Externalises hash characteristics to a write stream.
		 *
		 * @param aStream	Stream to which the object should be externalised.
		 **/	
		void ExternalizeL(RWriteStream& aStream) const;
		
		/**
		 * @deprecated
		 * 
		 * Determines whether an operation mode (e.g. HMAC) is supported by the plug-in 
		 * by searching through iSupportedOperationModes for the UID value corresponding
		 * to the aOperationMode parameter.
		 *
		 * @param aOperationMode	The UID of the operation mode to test
		 * @return 					ETrue if the operation mode is supported; otherwise, EFalse is returned.
		 */
		IMPORT_C TBool IsOperationModeSupported(TUid aOperationMode) const;		
		
	public:
		/**
		Common characteristics
		*/								
		TCommonCharacteristics cmn;

		/**
		The internal block size of the hash in bits
		*/
		TUint iBlockSize;

		/**
		the output size of the hash in bits
		*/
		TUint iOutputSize;
		
		/**
		The list of supported Operation modes
		*/
		const TInt32* iSupportedOperationModes;

		/**
		Number of the supported Operation mode
		*/
		TUint iOperationModeNum;
		};

	/**
	Random Characteristics
	*/	
	class TRandomCharacteristics
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Externalises random characteristics to a write stream.
		 *
		 * @param aStream	Stream to which the object should be externalised.
		 **/
		void ExternalizeL(RWriteStream& aStream) const;
	public:
		/**
		Common characteristics
		*/	
		TCommonCharacteristics cmn;
		
		/**
		The bit map of blocking non-blocking mode
		*/
		TUint iBlockingMode;
		};


	/**
	Symmetric Cipher Chararcteristics
	*/	
	class TSymmetricCipherCharacteristics
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Externalises symmetric cipher characteristics to a write stream.
		 *
		 * @param aStream	Stream to which the object should be externalised.
		 **/	
		void ExternalizeL(RWriteStream& aStream) const;
		
		/**
		Determines whether an operation mode (e.g. CBC) is supported by the plug-in 
		by searching through iSupportedOperationModes for the UID value corresponding
		to the aOperationMode parameter.
		@param aOperationMode	The UID of the operation mode to test
		@return ETrue if the operation mode is supported; otherwise, EFalse is returned.
		*/
		IMPORT_C TBool IsOperationModeSupported(TUid aOperationMode) const;
		
		/**
		Determines whether a padding mode (e.g. PKCS7) is supported by the plug-in 
		by searching through iSupportedPaddingModes for the UID value corresponding
		to the aPaddingMode parameter.
		@param aPaddingMode	The UID of the padding mode to test.
		@return ETrue if the padding mode is supported; otherwise, EFalse is returned.
		*/
		IMPORT_C TBool IsPaddingModeSupported(TUid aPaddingMode) const;
	
	public:
		/**
		Common characteristics
		*/	
		TCommonCharacteristics cmn;
	
		/**
		The maximum key length in bits
		*/
		TUint iMaximumKeyLength;

		/**
		The block size of the cipher in bits
		*/
		TUint iBlockSize;

		/**
		The list of supported padding modes
		*/
		const TInt32* iSupportedPaddingModes;
		
		/**
		Number of the supported padding mode
		*/
		TUint iPaddingModeNum;		

		/**
		The list of supported Operation modes
		*/
		const TInt32* iSupportedOperationModes;

		/**
		Number of the supported Operation mode
		*/
		TUint iOperationModeNum;
		
		/**
		The supported key mode bit map. e.g. extern key, internal key or both
		*/
		TInt iKeySupportMode;				
		};

	/**
	Asymmetric Cipher Characteristics
	*/	
	class TAsymmetricCipherCharacteristics
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Externalises asymmetric cipher characteristics to a write stream.
		 *
		 * @param aStream	Stream to which the object should be externalised.
		 **/
		void ExternalizeL(RWriteStream& aStream) const;
		
		/**
		Determines whether a padding mode (e.g. PKCS7) is supported by the plug-in 
		by searching through iSupportedPaddingModes for the UID value corresponding
		to the aPaddingMode parameter.
		@param aPaddingMode	The UID of the padding mode to test.
		@return ETrue if the padding mode is supported; otherwise, EFalse is returned.
		*/
		IMPORT_C TBool IsPaddingModeSupported(TUid aPaddingMode) const;
		
	public:
		/**
		Common characteristics
		*/	
		TCommonCharacteristics cmn;
		/**
		The maximum key length in bits
		*/
		TUint iMaximumKeyLength;
	
		/**
		The list of supported padding modes
		*/
		const TInt32* iSupportedPaddingModes;
		
		/**
		Number of the supported padding mode
		*/
		TInt iPaddingModeNum;
		
		/**
		The supported key mode bit map. e.g. extern key, internal key or both
		*/
		TInt iKeySupportMode;		
		};

	/**
	Signature Characteristics
	*/	
	class TAsymmetricSignatureCharacteristics
		{
	public:
		/**
		 * @internalComponent
		 *
		 * Externalises signature characteristics to a write stream.
		 *
		 * @param aStream	Stream to which the object should be externalised.
		 **/
		void ExternalizeL(RWriteStream& aStream) const;
		
		/**
		Determines whether a padding mode (e.g. PKCS7) is supported by the plug-in 
		by searching through iSupportedPaddingModes for the UID value corresponding
		to the aPaddingMode parameter.
		@param aPaddingMode	The UID of the padding mode to test.
		@return ETrue if the padding mode is supported; otherwise, EFalse is returned.
		*/
		IMPORT_C TBool IsPaddingModeSupported(TUid aPaddingMode) const;
		
	public:
		/**
		Common characteristics
		*/	
		TCommonCharacteristics cmn;

		/**
		The maximum key length in bits
		*/
		TUint iMaximumKeyLength;
	
		/**
		The list of supported padding modes
		*/
		const TInt32* iSupportedPaddingModes;
		
		/**
		Number of the supported padding mode
		*/
		TInt iPaddingModeNum;
	
		/**
		The supported key mode bit map. e.g. extern key, internal key or both
		*/
		TInt iKeySupportMode;		
		};


	/**
	Key Agreement Characteristics
	*/	
	class TKeyAgreementCharacteristics
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Externalises key agreement characteristics to a write stream.
		 *
		 * @param aStream	Stream to which the object should be externalised.
		 **/	
		void ExternalizeL(RWriteStream& aStream) const;
		
	public:
		/**
		Common characteristics
		*/	
		TCommonCharacteristics cmn;
		};
		
	/**
	Asymmetric Keypair Generator Characteristics
	*/	
	class TAsymmetricKeypairGeneratorCharacteristics 
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Externalises key pair generator characteristics to a write stream.
		 *
		 * @param aStream	Stream to which the object should be externalised.
		 **/
		void ExternalizeL(RWriteStream& aStream) const;
		
	public:
		/**
		Common characteristics
		*/	
		TCommonCharacteristics cmn;

		/**
		The maximum key length that may be generarted in bits
		*/
		TUint iMaximumKeyLength;		
		};

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT		
	/**
	 * Mac Characteristics
	 */	
	class TMacCharacteristics
		{
	public:
		/**
		 * @internalComponent
		 * 
		 * Externalises hash characteristics to a write stream.
		 *
		 * @param aStream Stream to which the object should be externalised.
		 */	
	    void ExternalizeL(RWriteStream& aStream) const;
			
	public:
	
		/**
		 * Common Characteristics
		 */
		TCommonCharacteristics iMacChar;  
			
		/**
		 * Mac Mode Type UID will be used to select the plug-in implementations. 
		 * The mode can be either hash or symmetric cipher based. 
		 */
		TInt32 iMacMode;

		/**
		 * The following pointers are mutually exclusive in their usage and anytime 
		 * only one will be initialised depending upon the Mac mode type the plug-in
         * supports, those not supported will be NULL.               
         */    

		/**
		 * Initialised if 'iMacMode' is Hash based otherwise will be NULL.
		 */
		const THashCharacteristics* iHashAlgorithmChar;
		/**
		 * Initialised if 'iMacMode' is Symmetric Cipher based otherwise will be NULL.
		 */
		const TSymmetricCipherCharacteristics* iCipherAlgorithmChar;
		};
#endif	
	}
#endif //__CRYPTOAPI_PLUGINCHARACTERISTICS_H__
