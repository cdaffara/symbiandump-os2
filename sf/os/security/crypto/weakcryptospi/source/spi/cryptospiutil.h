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
* CryptoSpi Utility Functionality definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOSPIUTIL_H__
#define __CRYPTOSPIUTIL_H__

#include <cryptospi/cryptocharacteristics.h>
#include <cryptospi/ruleselector.h>

namespace CryptoSpi
	{
	class CryptoSpiUtil
		{
	public:
		
		/**
		Run the cryptospi setup executable to publish the characteristics
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		static TInt RunCryptoSpiPropertySetupExe();
		
		/**
		Retrieve characteristics and Dll list for an interface
		@param aInterface the interface Uid
		*/
		static void RetrieveCharacteristicsL(TInt32 aInterface, RPointerArray<CCharacteristicsAndPluginName>& aList);

		/**
		Retrieve characteristics and Dll list for an interface
		@param aInterface the interface Uid
		*/
		static void RetrieveCharacteristicsL(TInt32 aInterface, RPointerArray<CRulesCharacteristicsAndPluginName>& aList);

		/**
		Get the plugin dll name from the given Dll index
		@param aDllIndex dll index
		@param aName the plugin file name
		@return KErrNone if successful; otherwise, a system wide error code.
		*/
		static TInt DllIndexToName(TInt aDllIndex, TFileName& aName);
		
		/**
		Load the given name plugin Dll, and leaves the plugin handle on the cleanup stack
		@param aLib the library handle of the plugin dll, which is put on the cleanup stack.
		@param aFileName the plugin dll file name
		*/
		static void LoadPluginDllLC(RLibrary& aLib, TFileName& aFileName);
		
		/**
		Create the characteristics for a crypto interface
		@param aInterface the crypto interface
		@return the pointer to characteristic
		*/
		static CCharacteristics* CreateCharacteristicsL(TInt32 aInterface);
		
	private:
		/**
		Retrieve the characteristics to the given buffer
		@param aInterface the crypto interface
		@param aStream the steam to read from
		@param aBuf the buffer which holds the characteristic
		@param aCount the count of the characteristics.
		*/	
		static TInt RetrieveCharacteristicsL(TInt32 aInterface, RDesReadStream& aStream, RBuf8& aBuf, TInt& aCount);
		};
	}

#endif //__CRYPTOSPIUTIL_H__


