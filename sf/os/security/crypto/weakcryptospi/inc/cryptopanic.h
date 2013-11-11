/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CRYPTOPANIC_H__
#define __CRYPTOPANIC_H__

/**
 * @file
 * Cryptography Panic Codes
 *
 * @publishedAll
 * @released
 */

/** 
 * @publishedAll
 * @released
 *
 * Panic codes for cryptography related libraries
 */
_LIT(KCryptoPanic, "CRYPTO-LIB");

/** 
 * @publishedAll
 * @released
 *
 * Panic codes for "CRYPTO-LIB" */
enum TCryptoPanicCode
	{
	/**
	 * The supplied descriptor is not large enough to store the output
	 * of the cryptographic operation
	 */	
	ECryptoPanicOutputDescriptorOverflow= 1,
	/**
	 * The supplied input descriptor is too large to pad
	 */	
	ECryptoPanicPadInputTooLarge= 2,
	/**
	 * The input data is too long
	 */
	ECryptoPanicInputTooLarge= 3
	};

#endif
