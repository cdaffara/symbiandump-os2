/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef	CDUMMYECCIMPL_H
#define	CDUMMYECCIMPL_H

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>
#include "keys.h"
#include "asymmetriccipherplugin.h"

namespace DummyEccHwCrypto
	{
	using namespace CryptoSpi;
	
	/**
	 * Implements the MAsymmetricCipher interface.
	 * 
	 * This should be used to create the cipher object to perform 
	 * encryption / decryption using ECC.
	 */
	NONSHARABLE_CLASS(CDummyECCCipherImpl) : public CBase, public MAsymmetricCipher
		{
	public:
	    static CDummyECCCipherImpl* NewL(const CKey& aKey, TUid aCryptoMode, TUid aPaddingMode);
	    static CDummyECCCipherImpl* NewLC(const CKey& aKey, TUid aCryptoMode, TUid aPaddingMode);
	    
	    // MPlugin Interface
	    void Close();
	    void Reset();
	    void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
	    const CExtendedCharacteristics* GetExtendedCharacteristicsL();
	    TAny* GetExtension(TUid aExtensionId); 
	    // End of MPlugin Interface
	    
	    // MAsymmetricCipherBase Interface
	    void SetKeyL(const CKey& aKey);
	    void SetCryptoModeL(TUid aCryptoMode);
	    void SetPaddingModeL(TUid aPaddingMode);
	    TInt GetMaximumInputLengthL() const;
	    TInt GetMaximumOutputLengthL() const;
	    // End of MAsymmetricCipherBase Interface
	    
		// MAsymmetricCipher Interface
		void ProcessL(const TDesC8& aInput, TDes8& aOutput);
		
		TUid ImplementationUid() const;
		// Destructor
		~CDummyECCCipherImpl();

	private:
		CDummyECCCipherImpl(TUid aCryptoMode, TUid aPaddingMode);
		void ConstructL(const CKey& aKey);
		void DecryptL(const TDesC8& aInput, TDes8& aOutput);
		void EncryptL(const TDesC8& aInput, TDes8& aOutput);
		
	private:
		/* The key extracted from a CKey object. This would just have 
		 * a handle to the key stored in hardware. Hence the actual key 
		 * would not be available to the user of this cryptoSPI plugin.
		 */
	    CKey* iKey;
		
		/// encryption or decryption
		TUid iCryptoMode;		

		/* The current padding scheme. This is not being used in the 
		 * reference implementation.
		 */
		TUid iPaddingMode;
		};

			
	}

#endif	//	CDUMMYECCIMPL_H
