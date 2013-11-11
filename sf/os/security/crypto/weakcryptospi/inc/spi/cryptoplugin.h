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
* cryptoplugin abstract interface
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOAPI_CRYPTOPLUGIN_H__
#define __CRYPTOAPI_CRYPTOPLUGIN_H__

#include <cryptospi/cryptobaseapi.h>
#include <cryptospi/plugincharacteristics.h>
#include <cryptospi/extendedcharacteristics.h>

namespace CryptoSpi
	{
	
	/**
	The crypto plugin base class definition
	*/
	class MPlugin
		{
	public:
		virtual void Close() = 0;
		
		/**
		Resets the crypto plugin.
		*/
		virtual void Reset() = 0;

		/**
		Retrieves the plugin's common characteristics
		@param aPluginCharacteristics	The plugin characteristics value retrieved
		@leave ...		Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		*/
		virtual void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics) = 0;
		
		/**
		Retrieves the plugin's extended characteristics
		@return The extended plugin characteristics, on the heap.
		This should be deleted by the caller when finished with.
		@leave ...		Any of the crypto error codes defined in 
  						cryptospi_errs.h or any of the system-wide error codes.
		*/	
		virtual const CExtendedCharacteristics* GetExtendedCharacteristicsL() = 0;

		/**
		Interface that enable plugin to extend functionality
		@param aExtensionId The UID for the extended functionality
		@return The extended handler pointer
		*/
		virtual TAny* GetExtension(TUid aExtensionId) = 0;
		};
	}

#endif //__CRYPTOAPI_CRYPTOPLUGIN_H__
