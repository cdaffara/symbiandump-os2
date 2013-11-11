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
* This utility class provides functions to generate key identifer data from
* supplied key data
*
*/


/**
 @file
 @publishedPartner
 @released
*/

#ifndef _KEYIDENTIFIERUTIL_H_
#define _KEYIDENTIFIERUTIL_H_

#include <e32base.h>

class CRSAPublicKey;
class CDSAPublicKey;
class RInteger;

/** The length of a SHA-1 hash 
  * @publishedPartner
  * @released
  */
 const TInt KSHA1HashLengthInBytes = 20;

/**
  * A SHA-1 hash.
  * @publishedPartner
  * @released
  */
 typedef  TBuf8<KSHA1HashLengthInBytes> TSHA1Hash;
 
  
 /**
  * A SHA-1 hash is also used as a key identifier.
  * @publishedPartner
  * @released
  */
 typedef TSHA1Hash TKeyIdentifier;


/**
 * Utilities for generation of the key identifier
 * @publishedPartner
 * @released
 */
class KeyIdentifierUtil
	{
public:
	/**
	 * Creates a RSA key identifier using the RSAPublicKey.
	 *
	 * @param aKey	The RSA encryption key
	 * @param aIdentifier	key identifier
	  */
	IMPORT_C static void RSAKeyIdentifierL(const CRSAPublicKey& aKey, TKeyIdentifier& aIdentifier);
	/**
	 * Creates a DSA key identifier using the DSAPublicKey.
	 *
	 * @param aKey	The DSA encryption key
	 * @param aIdentifier	key identifier
	  */
	IMPORT_C static void DSAKeyIdentifierL(const CDSAPublicKey& aKey, TKeyIdentifier& aIdentifier);
	/**
	 * Creates a DHKeyIdentifier using the RInteger.
	 *
	 * @param aKey			The DH parameter 
	 * @param aIdentifier	key identifier
	  */
	IMPORT_C static void DHKeyIdentifierL(const RInteger& aKey, TKeyIdentifier& aIdentifier);
	};

#endif
