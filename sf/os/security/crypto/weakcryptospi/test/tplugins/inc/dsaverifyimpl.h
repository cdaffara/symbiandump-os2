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


#ifndef __DSAVERIFYIMPL_H__
#define __DSAVERIFYIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>

#include "verifierimpl.h"
#include "../../../source/common/inlines.h"

/**
 * Implementation of DSA verification
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CDSAVerifierImpl) : public CVerifierImpl
		{
	public:
		/**
		Creates an instance of a DSA verifier plug-in.
		@param aKey The key
		@return A pointer to a CDSAVerifierImpl instance
		*/
		static CDSAVerifierImpl* NewL(const CKey& aKey);

		/**
		Creates an instance of a DSA verifier plug-in.
		@param aKey The key
		@return A pointer to a CDSAVerifierImpl instance
		*/
		static CDSAVerifierImpl* NewLC(const CKey& aKey);
		
		// Override CVerifierImpl virtual functions
		TUid ImplementationUid() const;
		// End of CVerifierImpl
		
		// Override MSignatureBase virtual functions
		void SetKeyL(const CKey& aPublicKey);
		virtual TInt GetMaximumInputLengthL() const;

		// End of MSignatureBase
		
		// Override MSigner virtual functions
		void VerifyL(const TDesC8& aInput, const CCryptoParams& aSignature, TBool& aVerificationResult);
		void InverseSignL(HBufC8*& aOutput, const CCryptoParams& aSignature);
		// End of MSigner
		
		/// Destructor
		~CDSAVerifierImpl();
		
	protected:
		/// Constructor
		CDSAVerifierImpl();
		
		/// second phase of construction
		virtual void ConstructL(const CKey& aKey);
			
	private:
		static const TUint KSha1HashLength = 20;	
		};
	}

#endif // __DSAVERIFYIMPL_H__
