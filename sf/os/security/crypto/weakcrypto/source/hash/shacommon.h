#ifndef __CRYPTO_SHACOMMON_H_
#define __CRYPTO_SHACOMMON_H_

#include <e32base.h>/*
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
* Same as used in SHA1
* SHA_CH		> CSHA1_F
* SHA_Maj		> CSHA1_H
* SHA_Parity	> CSHA1_G
* The following definitions are equivalent and potentially faster.
* #define SHA_Ch(x, y, z)      (((x) & ((y) ^ (z))) ^ (z))
* #define SHA_Maj(x, y, z)     (((x) & ((y) | (z))) | ((y) & (z)))
* These functions are defined in FIPS 180-2 Section 4.1
* Equation 4.1, 4.2, 4.3, 4.8, 4.9
*
*/




/**
 @file
 @internalComponent
 @released
*/
template<typename T>
inline T SHA_Ch(T aX, T aY, T aZ)
	{
	return ((aX & aY) ^ ((~aX) & aZ));
	}

template<typename T>
inline T SHA_Maj(T aX, T aY, T aZ)
	{
	return ((aX & aY) ^ (aX & aZ) ^ (aY & aZ));
	}

template<typename T>
inline T SHA_Parity(T aX, T aY, T aZ)
	{
	return (aX ^ aY ^ aZ);
	}

/**
 * Define the SHA shift, and rotate right macro 
 * Defined in FIPS 180-2 Section 3.2
 */
/** 
 * SHA Right Shift operation: The right shift operation SHR^n(x), 
 * where x is a w-bit word and n is an integer with 0 <= n < w, 
 * is defined by  SHR^n(x) = x >> n.
 */
template<typename T>
inline T SHA_SHR(T aBits, T aWord)
	{
	return (aWord >> aBits);
	}

/**
 * SHA Rotate Right Operation: The rotate right (circular right shift) operation
 * ROTR^n(x), where x is a w-bit word and n is an integer with 0 <= n < w, 
 * is defined by ROTR n(x)=(x >> n) || (x << w - n).
 */
template<typename T>
inline T SHA_ROTR(T aBits, T aWord)
	{
	TInt totalBits = sizeof(T) << 3;
	return ((aWord >> aBits) | (aWord << (totalBits-aBits)));
	}

NONSHARABLE_CLASS(MSHA2Impl)
	{
public:
	/**
	 * This function will reset the state of hash.
	 */
	virtual void Reset(const TAny*) = 0;
	/**
	 * This function will finalize the hash and return
	 * the calculated hash.
	 * @return Final hash
	 */ 
	virtual const TDesC8& Final() = 0;
	/**
	 * This function will add the message to the internal
	 * buffer and if the block size is reached then calcualte
	 * the hash till that point.
	 * @param aMessage Message to be updated.
	 * @param aLength Length of the message to be updated.
	 */ 
	virtual void Update(const TUint8* aMessage, TUint aLength) = 0;
	/**
	 * This function will save the internal state of the hash.
	 */ 
	virtual void StoreState() = 0;
	/**
	 * This function will retrieve the saved the internal state 
	 * of the hash.
	 */ 
	virtual void RestoreState() = 0;
	/**
	 * virtual distructor.
	 */
	virtual ~MSHA2Impl(){}
	};

#endif //__CRYPTO_SHACOMMON_H_
