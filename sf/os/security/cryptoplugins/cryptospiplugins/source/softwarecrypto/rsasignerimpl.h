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


#ifndef __RSASIGNERIMPL_H__
#define __RSASIGNERIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>

#include "signerimpl.h"
#include "common/inlines.h"

/**
 * Implementation of RSA signing
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CRSASignerImpl) : public CSignerImpl
		{
	public:
		/**
		Creates an instance of an RSA signer plug-in.
		@param aKey The private key used to sign.
		@param aPaddingMode The padding mode
		@return A pointer to a CRSASignerImpl instance
		*/
		static CRSASignerImpl* NewL(const CKey& aKey, TUid aPaddingMode);
		
		/**
		Creates an instance of an RSA signer plug-in.
		@param aKey The private key used to sign.
		@param aPaddingMode The padding mode
		@return A pointer to a CRSASignerImpl instance
		*/
		static CRSASignerImpl* NewLC(const CKey& aKey, TUid aPaddingMode);
		
		// Override CSignerImpl virtual functions
		TUid ImplementationUid() const;
		// End of CSignerImpl
		
		// Override MSignatureBase virtual functions
		void SetKeyL(const CKey& aPrivateKey);
		void SetPaddingModeL(TUid aPaddingMode);
		TInt GetMaximumInputLengthL() const;
		TInt GetMaximumOutputLengthL() const;
		// End of MSignatureBase
		
		// Override MSigner virtual functions
		void SignL(const TDesC8& aInput, CCryptoParams& aSignature);
		// End of MSigner
		
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
		
		/// Destructor
		~CRSASignerImpl();

	protected:
		/// Constructor
		CRSASignerImpl(TUid aPaddingMode);

		/// second phase of construction
		virtual void ConstructL(const CKey& aKey);

	protected:
		/// the current padding scheme
		TUid iPaddingMode;
		CPadding* iPadding;
		};
	}

#endif // __RSASIGNERIMPL_H__
