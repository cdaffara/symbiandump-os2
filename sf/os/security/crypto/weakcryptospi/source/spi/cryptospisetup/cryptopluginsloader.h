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
* crypto plugins loader
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOPlUGINSLOADER_H__
#define __CRYPTOPlUGINSLOADER_H__

#include <e32hashtab.h>
#include <cryptospi/pluginentrydef.h>

namespace CryptoSpi
	{		
	/**
	The class holds the characteristics of an interface and DLL index,
	which is used for externalization.
	*/
	class TCharacteristicsDll
		{
	public:
		/**
		Constructor that constructs a TCharacteristicsDll object
		
		@param aCharacteristics a pointer to the Characteristics.
		@param aIndex the index of the plugindll that contains the interface implemetation
		*/
		TCharacteristicsDll(const TCharacteristics* aCharacteristics, TInt aIndex);
		
		/**
		Externalize this object to the supplied stream.

		@param	aStream Stream to write TCharacteristicsDll to.
		*/
		void ExternalizeL(RWriteStream& aStream);
		
	public:
		/**
		The pointer the characteristic of an interface
		*/
		const TCharacteristics* iCharacteristics;
		
		/**
		The index of the plugin dll
		*/
		TInt iDllIndex;
		}; 
	
	/**
	The class contains a list of characteristics-DllIndex pair.
	which is used for externalization.
	*/
	NONSHARABLE_CLASS(CCharacteristicDllIndexList) : public CBase
		{
	public:
		/**
		Creates a new CCharacteristicDllIndexList.
		@return A pointer to the CCharacteristicDllIndexList instance		
		*/
		static CCharacteristicDllIndexList* NewL();
		
		/**
		Creates a new CCharacteristicDllIndexList.
		Leave the CCharacteristicDllIndexList pointer on the cleanup stack.
		@return A pointer to the CCharacteristicDllIndexList instance		
		*/		
		static CCharacteristicDllIndexList* NewLC();
		
		/**
		Destructor
		*/
		~CCharacteristicDllIndexList();

		/**
		Externalize this object to the supplied stream.

		@param	aStream Stream to write CCharacteristicDllIndexList to.
		*/		
		void ExternalizeL(RWriteStream& aStream);
		
	private:
		/**
		Constructor
		*/
		CCharacteristicDllIndexList();
		
	public:
		/**
		The list of of characteristics-DllIndex pair.
		*/
		RArray<TCharacteristicsDll> iCharacteristicList;
		};
		
	
	/**
	This subclass of MStreamBuf is used with
	RWriteStream to count how many bytes are
	required to externalize an object.
	*/
	class TSizeStream : public MStreamBuf
		{
	public:
		inline TSizeStream();
		inline TInt Size() const;

		// override MStreamBuf
		virtual void DoWriteL(const TAny* /* aPtr */, TInt aLength);

	private:
		/** 
		Accumulated stream length in bytes.
		*/
		TInt iSize;
		};

	inline TSizeStream::TSizeStream()
	:	iSize(0)
		{
		}

	inline TInt TSizeStream::Size() const
		{
		return iSize;
		}
	
	/**
	This class loads all the plugins, build all the characteristics,
	and publish the properties for crypto spi.
	*/
	NONSHARABLE_CLASS(CCryptoPluginsLoader) : public CBase
		{
	public:
		/**
		Create a CCryptoPluginsLoader instance
		*/
		static CCryptoPluginsLoader* NewL();
		
		/**
		Create a CCryptoPluginsLoader instance.
		Leave the pointer on the cleanup stack
		*/		
		static CCryptoPluginsLoader* NewLC();
		
		/**
		Destructor
		*/
		~CCryptoPluginsLoader();
		
		/**
		Create the properties of the plugin configurations.
		*/
		void CreatePluginConfigPropertyL();
		
		/**
		Create the properties for the interfaces
		*/
		void CreateInterfacePropertyL(TInt32 aInterface);
		
	private:
		/**
		Construtors
		*/
		CCryptoPluginsLoader();
		void ConstructL();
		
		/**
		Build the plugins' characteristics lists
		*/
		void BuildPluginCharacteristicsL();
		
		/**
		Build the plugin' characteristics list
		@param aInterfaceUid the interface UID
		@param aEnumerateFunc the entry pointer of enumeration function
		@param aDllIndex the index of the loaded plugin
		*/		
		void BuildInterfaceCharacteristicsL(TUid aInterfaceUid,
											EnumerateCharacteristicsFunc aEnumerateFunc,
											TInt aDllIndex);											
		/**
		Loads the crypto plugins
		@param aOption the option to load all the plugins or one more plugins
		*/
		void LoadPluginsL();
		
		/**
		Create the interfaces' properties and write to the stream
		@param aStream the stream to be written to
		@param aInterface the interface Uid
		*/
		void DoCreateInterfacePropertyL(RWriteStream& aStream, TInt32 aInterface);
				
	private:
		/**
		The plugin DLL list, which holds all the plugin DLLs
		*/
		RArray<RLibrary> iPluginDllList;
		
		/**
		The file name corresponding to the plugin DLLs.
		*/
		RArray<TFileName> iPluginNames;
				
		/**
		The characteristic list, the length of the array is the number of the interfaces
		*/
		RHashMap<TInt32, CCharacteristicDllIndexList*> iInterfaceCharacteristicsMap;				
		};
	}
#endif //__CRYPTOPlUGINSLOADER_H__

