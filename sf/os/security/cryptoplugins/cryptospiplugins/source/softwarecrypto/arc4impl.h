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


#ifndef	ARC4IMPL_H
#define	ARC4IMPL_H

/**
@file 
@internalComponent
@released
*/

#include "symmetriccipherimpl.h"
#include <e32base.h>
#include <e32cmn.h>
#include "keys.h"


/**
Plug-in class for ARC4 stream cipher
*/
namespace SoftwareCrypto
	{
	using namespace CryptoSpi;

	NONSHARABLE_CLASS(CArc4Impl) : public CSymmetricStreamCipherImpl
		{
	public:
		
		/** 
		Number of bytes to discard by default from an ARC4 key stream. 
		*/
		static const TUint KDefaultDiscardBytes = 768;

		/** 
		The size of the substitution box (i.e. lookup table) in bytes. 
		*/
		static const TInt KSBoxSize = 256;

		/** 
		Maximum ARC4 key size in bytes.2048 bits 
		*/
		static const TInt KMaxARC4KeyBytes = 256; 

		/**
		Creates an instance of an ARC4 symmetric cipher plug-in.
		@param aKey The key
		@param aDiscardBytes The number of bytes to drop from 
							the beginning of the key stream.
		@return A pointer to a CArc4Impl instance
		*/
		static CArc4Impl* NewL(const CKey& aKey, TInt aDiscardBytes);

		/**
		Creates an instance of an ARC4 symmetric cipher plug-in,
		and leave it on the cleanup stack
		@param aKey The key
		@param aDiscardBytes The number of bytes to drop from 
							the beginning of the key stream.
		@return A pointer to a CArc4Impl instance
		*/
		static CArc4Impl* NewLC(const CKey& aKey, TInt aDiscardBytes);

		//Destructor
		~CArc4Impl();

		//Override MPlugin virtual function
		void Reset();
		
		//Override CSymmetricCipherImpl virtual functions
		TUid ImplementationUid() const;
		TBool IsValidKeyLength(TInt aKeyBytes) const;
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();
		
	private:
		//Constructor
		CArc4Impl(TInt aDiscardBytes);
			
		//second phase of construction	
		void ConstructL(const CKey& aKey);
				
		//Override CSymmetricStreamCipherImpl virtual functions
		void DoProcess(TDes8& aData);	

		void GenerateSBox();
		TUint8 GenerateByte();
		void DiscardBytes(TInt aDiscardBytes);

	private:
		TUint8 ix;
		TUint8 iy;
		TInt iDiscardBytes;
		TUint8 iState[KSBoxSize];
		};
	}

#endif //ARC4IMPL_H



