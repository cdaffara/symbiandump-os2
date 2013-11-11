/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Software hash base definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOAPI_SOFTWAREHASHBASE_H__
#define __CRYPTOAPI_SOFTWAREHASHBASE_H__

#include <cryptospi/hashplugin.h>
#include <cryptospi/keys.h>

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	/**
	Interfaces for Software Hash
	*/
	class MSoftwareHash : public MHash
		{
	public:
		/**
		Restores the internal state of the message digest
		to a previously stored state.
		*/
		virtual void RestoreState() = 0;

		/**
		Stores the internal state of the message digest.
		*/
		virtual void StoreState() = 0;
		};
			
	/**
	Proxy class Software Hash and Hmac implementation
	*/
	class CSoftwareHash : public MHash, public CBase
		{
	public:
		//
		//NewL & NewLC
		//
		static CSoftwareHash* NewL(TUid aAlgorithm, TUid aOperationMode, const CKey* aKey);
		static CSoftwareHash* NewLC(TUid aAlgorithm, TUid aOperationMode, const CKey* aKey);

#ifdef SYMBIAN_SDP_IPSEC_VOIP_SUPPORT
		static CSoftwareHash* NewL(TUid aAlgorithm);
#endif

		//
		// Destructor
		//
		~CSoftwareHash();
		//
		//From MPlugin
		//
		void Reset();
		void Close();		
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);		
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		TAny* GetExtension(TUid aExtensionId);
		//
		//From MHash
		//
	    TPtrC8 Hash(const TDesC8& aMessage);
	    void Update(const TDesC8& aMessage);
		TPtrC8 Final(const TDesC8& aMessage);
		MHash* ReplicateL();		
		MHash* CopyL();
		void SetKeyL(const CKey& aKey);			
		void SetOperationModeL(TUid aOperationMode);
	
	private:
		//
		//Constructor
		// 
		CSoftwareHash();
		void ConstructL(TUid aAlgorithm, TUid aOperationMode, const CKey* aKey);
		
		//
		//get the active implementation
		//
		MSoftwareHash* Impl();
		
	private:
		/**
		The Key for HMAC mode operation
		*/
		CKey* iKey;
		
		/**
		The Uid for the operation mode
		*/
		TUid iOperationMode;
		
		/**
		The hash implementation
		*/
		MSoftwareHash* iHashImpl;
		
		/**
		The Hmac implementation
		*/
		MSoftwareHash* iHmacImpl;
		};
		
	GLDEF_C inline TUint CMD_R(TUint a,TUint s)
		{
		return (a<<s) | (a>>(32-s));
		}
	}
	

#endif // __CRYPTOAPI_SOFTWAREHASHBASE_H__
