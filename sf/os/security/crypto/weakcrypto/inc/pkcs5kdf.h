/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __PKCS5KDF_H__
#define __PKCS5KDF_H__

#include <hash.h>

/** The number of times the hashing algorithm is run. */
const TUint KDefaultIterations = 1000;

/**
 * A PKCS#5 compliant Key Derivation Function (KDF).
 *
 * This class allows the derivation of deterministic arbitrary length byte 
 * streams from an input string. The output byte stream is generated using 
 * multiple iterations of a CSHA1 message digest and is suitable for use 
 * as a cryptographic symmetric key.
 *
 * @since v7.0s
 */
class TPKCS5KDF
	{
public:
	/** 
	 * Derives deterministic arbitrary length byte streams (aKey) from an input
	 * string (aPasswd) and a randomly chosen salt (aSalt) for use as a
	 * symmetric key.
	 *
	 * Attention -- Improperly chosen values for these parameters will seriously
	 * impact the security of the derived key and as a result the security of 
	 * your application. 
	 *
	 * See the Cryptography api-guide documentation for more information and 
	 * recommended usage patterns.
	 * 
	 * @param aKey			Output Value. The key resulting from the operation.
	 * 						The length of the key will be equal to the length of
	 * 						the input descriptor. All data, from the first byte 
	 * 						to the set length, will be overwritten with the resulting
	 *						byte stream.
	 * @param aPasswd		Input Value. The password you wish to derive a key from.
	 * @param aSalt			Input Value. A <B><I>randomly</I></B> selected second
	 * 						input to the key derivation function to discourage certain
	 * 						attacks. PKCS5 recommends a minimum of 8 randomly chosen bytes.
	 * @param aIterations	Input Value. The number of times the internal hashing
	 * 						function should be run over the password and salt.
	 *						Minimum recommendation is KDefaultIterations.
	 */
	IMPORT_C static void DeriveKeyL(TDes8& aKey, const TDesC8& aPasswd, 
		const TDesC8& aSalt, TUint aIterations = KDefaultIterations);
private:
	/** 
	 * Internal iterative function that performs the actual hashing. 
	 */
	static void F(CMessageDigest& aDigest, TUint32* aAccumulator, TUint32* S,
	TUint32* Ui, TUint aHashBytes, const TUint32* aSalt, TUint aSaltBytes, 
	TUint c, TUint i);
	
	/** 
	 * XOR's the values of two equal length descriptors.  Internally, it
	 * operates on a word by word basis.  Data stored beyond the end of the
	 * descriptor, but before the end of the final word, will be xored as well.
	 */
	static inline void XORString(const TUint32* aOp1, TUint32* aOp2,
		TUint aLength);
	};

#endif
