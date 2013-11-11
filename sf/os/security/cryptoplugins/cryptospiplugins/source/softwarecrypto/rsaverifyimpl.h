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
*
*/


#ifndef __RSAVERIFYIMPL_H__
#define __RSAVERIFYIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>

#include "verifierimpl.h"
#include "common/inlines.h"

/**
 * Implementation of RSA verification
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CRSAVerifierImpl) : public CVerifierImpl
		{
	public:
		/**
		Creates an instance of an RSA verifier plug-in.
		@param aKey The key
		@param aPaddingMode The padding mode
		@return A pointer to a CRSAVerifierImpl instance
		*/
		static CRSAVerifierImpl* NewL(const CKey& aKey, TUid aPaddingMode);

		/**
		Creates an instance of an RSA verifier plug-in.
		@param aKey The key
		@param aPaddingMode The padding mode
		@return A pointer to a CRSAVerifierImpl instance
		*/
		static CRSAVerifierImpl* NewLC(const CKey& aKey, TUid aPaddingMode);
		
		// Override CVerifierImpl virtual functions
		TUid ImplementationUid() const;
		// End of CVerifierImpl
		
		// Override MSignatureBase virtual functions
		void SetPaddingModeL(TUid aPaddingMode);
		void SetKeyL(const CKey& aPublicKey);
		TInt GetMaximumInputLengthL() const;
		TInt GetMaximumOutputLengthL() const;
		// End of MSignatureBase

		// Override MSigner virtual functions
		void VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult);
		void InverseSignL(HBufC8*& aOutput, const CCryptoParams& aSignature);
		// End of MSigner
		
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
		
		/// Destructor
		~CRSAVerifierImpl();
		
	protected:
		/// Constructor
		CRSAVerifierImpl(TUid aPaddingMode);
		
		/// second phase of construction
		virtual void ConstructL(const CKey& aKey);
		
	protected:
		/// the current padding scheme
		TUid iPaddingMode;
		CPadding* iPadding;
		};
	}

#endif // __RSAVERIFYIMPL_H__
