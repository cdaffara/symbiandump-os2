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


#ifndef __VERIFIERIMPL_H__
#define __VERIFIERIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>
#include <padding.h>
#include "verifierplugin.h"

#include "common/inlines.h"

/**
 * Abstract base class for verifier plug-ins.
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CVerifierImpl) : public CBase, public MVerifier
		{
	public:
		// Override MPlugin virtual functions
		void Close();
		void Reset(); // Always call reset in super-class if you override this
		TAny* GetExtension(TUid aExtensionId);
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		// End of MPlugin
		
		// Override MSignatureBase virtual functions
		virtual void SetPaddingModeL(TUid aPaddingMode);
		virtual void SetKeyL(const CKey& aPrivateKey);
		virtual TInt GetMaximumInputLengthL() const;
		virtual TInt GetMaximumOutputLengthL() const;
		// End of MSignatureBase
		
		// Override MVerifier virtual functions
		virtual void VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult) = 0;
		virtual void InverseSignL(HBufC8*& aOutput, const CCryptoParams& aSignature) = 0;
		// End of MVerifier
		
		/// Destructor
		~CVerifierImpl();
		
	protected:
		/// Constructor
		CVerifierImpl();
		
		virtual void ConstructL(const CKey& aKey);
			
		/**
		Helper function implemented by concrete cipher sub-class that allows
		GetCharacteristicsL to return the correct characteristics object.
		@return The implemention uid
		*/
		virtual TUid ImplementationUid() const = 0;
		
		/**
		Store the key to use for verifying.
		The key length is also checked to meet export restrictions and
		to ensure that it is appropriate for the cipher.
		@param aKey The key
		*/
		virtual void DoSetKeyL(const CKey& aKey);
		
	protected:
		/// the key
		CKey* iKey;
		};
	}

#endif // __VERIFIERIMPL_H__
