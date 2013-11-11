/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TVECTORUTILS_H__
#define __TVECTORUTILS_H__

#include <e32std.h>
#include "asymmetrickeys.h"
#include "asymmetric.h"
#include "bigint.h"

/**
 * Defines static utility methods used by the vector tests.
 */

class VectorUtils
    {
public:

    /** Parse a hex string and return a new RInteger. */
    static RInteger ParseIntegerL(const TDesC8& aDes);

    /** Parse a hex string and return a new descriptor containing the binary data. */
	static HBufC8* ParseBinaryL(const TDesC8& aDes);

    /** Return a new descriptor containing the hex representation of an Integer. */
	static TDesC* PrintIntegerL(const TInteger& aInt);

    /** Return a new descriptor containing the hex representation of a binary string. */
	static TDesC* PrintBinaryL(const TDesC8& aData);

    /** Parse a string containg either "ETrue" or "EFalse" and return a TBool. */
    static TBool ParseBoolL(const TDesC8& aDes);

    /** Create an RSA public key from from action data. */
    static CRSAPublicKey* ReadRSAPublicKeyL(const TDesC8& aData);

    /** Create an RSA private key from from action data. */
    static CRSAPrivateKeyStandard* ReadRSAPrivateKeyL(const TDesC8& aData);

	/** Create an RSA private key from by CRT */
	static CRSAPrivateKeyCRT* ReadRSAPrivateKeyCRTL(const TDesC8& aData);

    /** Create a DSA public key from from action data. */
    static CDSAPublicKey* ReadDSAPublicKeyL(const TDesC8& aData);

    /** Create a DSA private key from from action data. */
    static CDSAPrivateKey* ReadDSAPrivateKeyL(const TDesC8& aData);    

    /** Create a DSA signature from from action data. */
	static CDSASignature* ReadDSASignatureL(const TDesC8& aData);
    };

#endif
