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
* crypto spi application interface
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __CRYPTOAPI_CRYPTOSPISTATEAPI_H__
#define __CRYPTOAPI_CRYPTOSPISTATEAPI_H__

#include <e32base.h>

namespace CryptoSpi
	{
	class MPluginSelector;
	class CCharacteristicsAndPluginName;

	/**
	The class provides the APIs to set and unset the rule-based selector.
	*/
	NONSHARABLE_CLASS(CCryptoSpiStateApi)
		{
	public:

		/**
		Set the rule-based selector for the current thread. Once the rule-based 
		selector is set, it will be used by the crypto factory to select appropriate 
		plugin implementation. The owner of the selector is the user, and the ownership 
		is not transferd.
		
		@param aSelector The plugin selector to use
		*/
		IMPORT_C static void SetSelector(MPluginSelector* aSelector);
		
		/**
		The function unsets the selector for the current thread, so that
		the legacy selector will be used to select plugins. 
		*/
		IMPORT_C static void UnsetSelector();
		
		/**
		Enumerate the all characteristics in the system
		@param aCharacteristics the array of characteristic dll list.
		@param aInterface the uid of the interface, 0 means that enumerating characteristics for all the interfaces
		@param aExtended if extended characteristic needs to be retrieved.
		@leave ...	Any of the crypto error codes defined in 
  					cryptospi_errs.h or any of the system-wide error codes.	
		*/
		IMPORT_C static void EnumerateCharacteristicsL(RPointerArray<CCharacteristicsAndPluginName>& aCharacteristics, TInt32 aInterface, TBool aExtended);
		};
	}
	
#endif //__CRYPTOAPI_CRYPTOSPISTATEAPI_H__
