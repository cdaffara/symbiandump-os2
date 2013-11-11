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
*
*/


/**
 @file
 @publishedPartner
 @released
*/


#ifndef __PKCS12KDF_H__
#define __PKCS12KDF_H__

#include <e32std.h>

class PKCS12KDF
/**
	Namespace class contains static functions which
	are used to generate a key for PKCS#12 operations.
	
	See Section B from PKCS 12 v1.0.
 */
	{
public:
	enum TIDByteType
	/**
		ID byte value used to generate (en|de)cryption key,
		IV, or MAC key.  See SB.3 of spec.
	 */
		{
		/** Generates a key for (en|de)cryption. */
		EIDByteEncryptKey = 1,
		/** Generates an initialization vector. */
		EIDByteIV = 2,
		/** Generates a key for MAC-ing. */
		EIDByteMACKey = 3
		};
public:
	IMPORT_C static void DeriveKeyL(
		TDes8& aKey, TIDByteType aIDType,
		const TDesC8& aPasswd, const TDesC8& aSalt, const TUint aIterations);
	IMPORT_C static HBufC8* GeneratePasswordLC(const TDesC& aDes);

private:
	static void Process6cL(TDes8& Ij, const TDesC8& B, TInt v);

#ifdef _DEBUG
	enum TPanic
		{
		EDKEmptyPswd = 0x10, EDKOddPswdByteCount, EDKBadNullTerminator
		};
	static void Panic(TPanic aPanic);
#endif

	// not implemented - prevents instantiation
	PKCS12KDF();
	};

#endif	// #ifndef __PKCS12KDF_H__

