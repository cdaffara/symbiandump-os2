/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* ** IMPORTANT ** PublishedPartner API's in this file are published to 3rd party developers via the 
* Symbian website. Changes to these API's should be treated as PublishedAll API changes and the Security TA should be consulted.
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __CRYPTOSTRENGTH_H__
#define __CRYPTOSTRENGTH_H__

/**
 * Some builds of the crypto library have restrictions that only allow weak
 * ciphers to be used.  This class provides static helper functions for
 * determining these restrictions.
 */
class TCrypto
	{
public:
	/**
	 * Defines the strength of the cipher. 
	 */
	enum TStrength 
		{ 
		EWeak, EStrong
		};
public:
	/**
	 * Gets the allowed cipher strength.
	 *
	 * @return The allowed cipher strength.	
	 */	
	static IMPORT_C TCrypto::TStrength Strength();

	/**
	 * Indicates whether a symmetric key is small enough to be allowed. Note
	 * that this function leaves if the key is too large - in other words it can
	 * never return EFalse.
	 * 
	 * @param aSymmetricKeyBits	    The size (in bits) of the symmetric key
	 * @return						Whether the key is small enough to be allowed
	 * 
	 * @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	 *								cipher strength restrictions of the crypto library.
	 */
	static IMPORT_C TBool IsSymmetricWeakEnoughL(TInt aSymmetricKeyBits);

	/**
	 * Indicates whether an asymmetric key is small enough to be allowed.  Note
	 * that this function leaves if the key is too large - in other words it can
	 * never return EFalse.
	 *
	 * @param aAsymmetricKeyBits	The size (in bits) of the asymmetric key
	 * @return						Whether the key is small enough to be allowed
	 * 
	 * @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	 *								cipher strength restrictions of the crypto library.
	 */
	static IMPORT_C TBool IsAsymmetricWeakEnoughL(TInt aAsymmetricKeyBits);
	};

#endif //__CRYPTOSTRENGTH_H__
