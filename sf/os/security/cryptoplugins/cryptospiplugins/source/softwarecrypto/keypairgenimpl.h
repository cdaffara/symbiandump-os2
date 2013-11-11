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


#ifndef __KEYPAIRGENIMPL_H__
#define __KEYPAIRGENIMPL_H__

/**
@file 
@internalComponent
@released
*/

#include "cryptokeypairgeneratorapi.h"
#include "keypairgeneratorplugin.h"

/**
 * Key pair generator plugin base class
 */
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CKeyPairGenImpl) : public CBase, public MKeyPairGenerator
		{
	public:

		// Override MPlugin virtual functions
		void Close();
		TAny* GetExtension(TUid aExtensionId);
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		// End of MPlugin

		// Destructor
		~CKeyPairGenImpl();

	protected:
		/**
		Constructor
		*/
		CKeyPairGenImpl();
			
		/**
		Second phase of construction. Always call ConstructL in the super-class
		if you override this method.
		*/
		virtual void ConstructL();
		
		/**
		Helper function implemented by concrete cipher sub-class that allows
		GetCharacteristicsL to return the correct characteristics object.
		@return The implemention uid
		*/
		virtual TUid ImplementationUid() const = 0;
		};
	}

#endif // __KEYPAIRGENIMPL_H__
