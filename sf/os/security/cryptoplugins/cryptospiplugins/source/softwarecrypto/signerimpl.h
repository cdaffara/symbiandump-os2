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


#ifndef __SIGNERIMPL_H__
#define __SIGNERIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <e32cmn.h>
#include <cryptospi/cryptospidef.h>
#include "signerplugin.h"
#include <padding.h>

#include "common/inlines.h"

/**
 * Abstract base class for signer plug-ins.
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CSignerImpl) : public CBase, public MSigner
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

		/// Destructor
		~CSignerImpl();
		
	protected:
		/// Constructor
		CSignerImpl();

		/**
		Second phase of construction
		@param aKey The signing key
		*/
		virtual void ConstructL(const CKey& aKey);
		
		/**
		Helper function implemented by concrete cipher sub-class that allows
		GetCharacteristicsL to return the correct characteristics object.
		@return The implemention uid
		*/
		virtual TUid ImplementationUid() const = 0;
		
		/**
		Store the signing key. The key length is also checked to meet export restrictions and
		to ensure that it is appropriate for the cipher.
		@param aKey The key
		*/
		virtual void DoSetKeyL(const CKey& aKey);

	protected:
		/// the key, extracted from a CKey object
		CKey* iKey;

	private:
		};
	}

#endif // __SIGNERIMPL_H__
