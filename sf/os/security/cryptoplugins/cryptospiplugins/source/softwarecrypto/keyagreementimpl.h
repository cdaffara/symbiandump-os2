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


#ifndef __KEYAGREEMENTIMPL_H__
#define __KEYAGREEMENTIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include <e32base.h>
#include <cryptospi/cryptospidef.h>
#include "keys.h"
#include "keyagreementplugin.h"

/**
 * Abstract base class for key agreement plug-ins.
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CKeyAgreementImpl) : public CBase, public MKeyAgreement
		{
	public:
		
		// Override MPlugin virtual functions
		void Close();
		void Reset(); // Always call reset in super-class if you override this
		TAny* GetExtension(TUid aExtensionId);
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		// End of MPlugin

		// Override MKeyAgreement virtual functions
		void SetKeyL(const CKey& aSelfPrivateKey, const CCryptoParams* aParams);
		// End of MKeyAgreement

		/// Destructor
		~CKeyAgreementImpl();
		
	protected:
		/**
		Constructor
		*/
		CKeyAgreementImpl();
			
		/**
		Second phase of construction. Always call ConstructL in the super-class
		if your override this method.
		@param aPrivateKey The private key of one of the parties
		@param aParams	The parameters shared between both parties
		*/
		virtual void ConstructL(const CKey& aPrivateKey, const CCryptoParams* aParams);

		/**
		Helper function implemented by concrete cipher sub-class that allows
		GetCharacteristicsL to return the correct characteristics object.
		@return The implemention uid
		*/
		virtual TUid ImplementationUid() const = 0;

	private:

	protected:
		CKey* iPrivateKey;
		CCryptoParams* iSharedParams;	// common parameters between our private key and their public key
		};
	}

#endif	//	__KEYAGREEMENTIMPL_H__
