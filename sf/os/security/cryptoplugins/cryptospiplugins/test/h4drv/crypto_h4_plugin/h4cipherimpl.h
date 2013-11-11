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
#ifndef	__SYMMETRICCIPHERIMPL_H__
#define	__SYMMETRICCIPHERIMPL_H__

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>
#include <padding.h>
#include "symmetriccipherplugin.h"


#ifdef __MARM__
#define __NOTSHARED __declspec(notshared)
#else
#define __NOTSHARED
#endif

/** The maximum block size supported (in bytes) */
const TUint KMaxBlockSizeSupported = 32;

/**
Abstract base class for symmetric cipher plug-ins.
*/
namespace HwCrypto
	{
	using namespace CryptoSpi;
		
	class __NOTSHARED CH4CipherImpl : public CBase, public MSymmetricCipher
		{
	public:
		/**
		Implemented by each cipher subclass to determine whether the
		specified key length is valid for that cipher.
		This is called by ConstructL and SetKeyL
		@param aKeyLength The key length in bytes to verify.
		*/
		virtual TBool IsValidKeyLength(TInt aKeyBytes) const = 0;
		
		/**
		Helper function implemented by concrete cipher sub-class that 
		allows GetCharacteristicsL to return the correct characteristics object.
		@return The implemention uid
		*/
		virtual TUid ImplementationUid() const = 0;
		
		/**
		Gets the strength of the current key, needed to check whether the cipher
		may operate if strong cryptography is not enabled.
		@return The strength of the current key
		*/
		virtual TInt GetKeyStrength() const;
		
				
		// Override MPlugin virtual functions
		void Close();
		TAny* GetExtension(TUid aExtensionId);

		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);	


		void Reset(); // Always call reset in super-class if you override this
		// End of MPlugin virtual functions
		
		// Override MSymmetricCipherBase virtual functions 
		TInt KeySize() const;
		virtual void SetKeyL(const CKey& aKey);

		TInt BlockSize() const;
		void SetCryptoModeL(TUid aCryptoMode);
		void SetOperationModeL(TUid aOperationMode);
		void SetPaddingModeL(TUid aPaddingMode);
		void SetIvL(const TDesC8& aIv);
		
		TInt MaxOutputLength(TInt aInputLength) const;
		TInt MaxFinalOutputLength(TInt aInputLength) const;

		void ProcessL(const TDesC8& aInput, TDes8& aOutput);
		void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);						
		// End of MSymmetricCipherBase
						
		/// Destructor
		~CH4CipherImpl();

	protected:
		
		/**
		Constructor
		@param aBlockBytes The block size in bytes
		@param aOperationMode The mode of operation e.g. CBC
		@param aCryptoMode Whether to encrypt or decrypt
		*/
		CH4CipherImpl(TUint8 aBlockBytes,
					  TUid aCryptoMode,
					  TUid aOperationMode,
					  TUid aPaddingMode);
		
		/**
		Second phase of construction. Always call ConstructL in the super-class
		if your override this method.
		
		@param aKey The key to initialise the cipher with.
		*/
		virtual void ConstructL(const CKey& aKey);		
		
		/**
		Creates the extended characteristics object. Concrete plug-in classes
		may override this to customise the object returned by GetCharacteristics();
		
		The default configuration is that  plug-ins have unlimited concurrency, 
		cannot be reserved for exclusive use and are not CERTIFIED to be standards compliant.		
		@return The extended characteristics object
		*/
		virtual CExtendedCharacteristics* CreateExtendedCharacteristicsL();		
		
		/**
		Extracts the raw symmetric key from a generic key object. The buffer
		is placed on the cleanup stack.
		
		@param aKey The key object
		@return A buffer containing the raw key value
		*/
		HBufC8* ExtractKeyDataLC(const CKey& aKey) const;	
	

		/**
		   DoSetDetails

		   Reconfigure h/w based on iCryptoMode/iOperationMode/iKey/iIv

		   Implemented by each cipher subclass.
		*/
		virtual void DoSetupL() = 0;

		/**
		   DoWrite

		   Blocking write of data to be transformed. Need not be a
		   multiple of the block size.

		   Implemented by each cipher subclass.
		*/
		virtual void DoWriteL(const TUint8* aBuffer, TUint aNumBytes) = 0;
		
		/**
		   DoRead		   

		   Blocking read of transformed data.

		   Need not be a multiple of the block size, but caller should
		   ensure enough blocks of data have been written to satisfy
		   the request.

		   The data is appended to the supplied descriptor.

		   Implemented by each cipher subclass.
		*/
		virtual void DoReadL(TDes8 &aBuffer, TUint32 aLength) = 0;
			
			
	protected:

		/// the key, extracted from a CKey object
		HBufC8* iKey;
		
		/// key size in bytes
		TUint iKeyBytes;
		
		/// Standards conformance information.
		RArray<TUid> iStandardsConformance;

		/// block size in bytes, current largest block size is 16 bytes (AES)
		TUint8 iBlockBytes;	
		/// encryption or decryption
		TUid iCryptoMode;		
		/// The block cipher mode e.g. ECB, CBC
		TUid iOperationMode;
		/// the current padding scheme
		TUid iPaddingMode;
		
		/// the initialisation vector
		RBuf8 iIv;
		
		/// current padding scheme implementation
		CPadding* iPadding;
		/// buffer to store blocks
		RBuf8 iPartialBlock;
		/// buffer to store input / output of padding
		RBuf8 iPaddingBlock;
		
	private:
		TBool iNeedToSetupHw;
		};

			
	}

#endif	//	__SYMMETRICCIPHERIMPL_H__
