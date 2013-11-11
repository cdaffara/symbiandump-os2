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
* crypto base api interface
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_BASEAPI_H__
#define __CRYPTOAPI_BASEAPI_H__

#include <e32base.h>
#include <cryptospi/plugincharacteristics.h>

namespace CryptoSpi
	{
	class MPlugin;
	class MHash;
	class MAsyncHash;
	class CCryptoParams;
	class CExtendedCharacteristics;
		
	NONSHARABLE_CLASS(CCryptoBase) : public CBase
		{
	public:
		/**
		Retrieves the plugin's common characteristics
		@param aPluginCharacteristics The plugin characteristics value retrieved
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/
		IMPORT_C void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		
		/**
		Retrieves the plugin's extended characteristics
		@return The extended plugin characteristics, on the heap.
		This should be deleted by the caller when finished with.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.
		*/	
		IMPORT_C const CExtendedCharacteristics* GetExtendedCharacteristicsL();

        /**
        Resets the state of the crypto object to uninitialised. All prior
        state is lost.
        */
        IMPORT_C void Reset();
	
	protected:
		/**
		 * @internalComponent
		 * 
		 * Constructor
		 **/		
		CCryptoBase(MPlugin* aPlugin, TInt aLibHandle);
		/**
		Destructor
		*/
		virtual ~CCryptoBase();
	
	protected:
		/**
		The plugin implementation
		*/
		MPlugin* iPlugin;	
		
		/**
		The current plugin DLL loaded
		*/
		TInt iLibHandle;
		};
	}

#endif //__CRYPTOAPI_BASEAPI_H__

