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


#ifndef __DSASIGNERIMPL_H__
#define __DSASIGNERIMPL_H__

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
 * Implementation of DSA signing
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CDSASignerImpl) : public CSignerImpl
		{
	public:
		/**
		Creates an instance of a DSA signer plug-in.
		@param aKey The private key used to sign.
		@return A pointer to a CDSASignerImpl instance
		*/
		static CDSASignerImpl* NewL(const CKey& aKey);
		
		/**
		Creates an instance of a DSA signer plug-in.
		@param aKey The private key used to sign.
		@return A pointer to a CDSASignerImpl instance
		*/
		static CDSASignerImpl* NewLC(const CKey& aKey);
		
		// Override CSignerImpl virtual functions
		TUid ImplementationUid() const;
		// End of CSignerImpl
		
		// Override MSignatureBase virtual functions
		void SetKeyL(const CKey& aPrivateKey);
		TInt GetMaximumInputLengthL() const;
		// End of MSignatureBase
		
		// Override MSigner virtual functions
		void SignL(const TDesC8& aInput, CCryptoParams& aSignature);
		// End of MSigner
		
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
		
		/// Destructor
		~CDSASignerImpl();
		
	protected:
		/// Constructor
		CDSASignerImpl();
			
	protected:
		/// second phase of construction
		virtual void ConstructL(const CKey& aKey);
		
	private:
		static const TUint KSha1HashLength = 20;
		};
	}

#endif // __DSASIGNERIMPL_H__
