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
* RC4 implementation
*
*/


/**
 @file 
 @publishedPartner
 @released 
*/
 
#ifndef __ARC4_H__
#define __ARC4_H__

#include "streamcipher.h"

/** The size of the substitution box (i.e. lookup table) in bytes. */
const TInt KSBoxSize = 256;

/** Maximum ARC4 key size in bytes. */
const TInt KMaxARC4KeyBytes = 256; //2048 bits

/** Number of bytes to discard by default from an ARC4 key stream. */
const TUint KDefaultDiscardBytes = 768;

/**
* Implements an RC4-compatible stream cipher that outputs a pseudorandom stream
* of bits, having been initialised with a key. 
*
*/
class CARC4 : public CStreamCipher
{
public:
	/**
	* Constructs an instance of a CARC4 object, and initialises it with a key and
	* (optionally) the number of initial bytes to discard. Defaults to 256. 
	*
	* The number of dropped bytes <b>must</b> be agreed with the other
	* party, with which information is to be exchanged, prior to encipherment.
	*
	* @note	Several papers have been published indicating that there are weaknesses 
	*		in the first bytes of an ARC4 byte stream.  A search for "ARC4
	*		discard" should find these papers.  Recommended practice is to drop the first
	*		KDefaultDiscardBytes bytes of the key stream.  
	*
	* @param aKey			The key to use.  aKey must be less than or equal to
	*						KRC4MaxKeySizeBytes.  
	* @param aDiscardBytes	The number of bytes to drop from the beginning of the key
	*						stream.
	* @return				A pointer to the new CARC4 object.
	*  
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CARC4* NewL(const TDesC8& aKey, 
		TUint aDiscardBytes = KDefaultDiscardBytes);

	/**
	* Constructs an instance of a CARC4 object, and initialises it with a key and
	* (optionally) the number of initial bytes to discard. Defaults to 256. 
	*
	* The number of dropped bytes <b>must</b> be agreed with the other
	* party, with which information is to be exchanged, prior to encipherment.
	*
	* @see CARC4::NewL()
	*
	* @param aKey			The key to use.  aKey must be less than or equal to
	*						KRC4MaxKeySizeBytes.  
	* @param aDiscardBytes	The number of bytes to drop from the beginning of the key
	*						stream.
	* @return				A pointer to the new CARC4 object.
	*  
	* @leave KErrKeyNotWeakEnough	If the key size is larger than that allowed by the
	*								cipher strength restrictions of the crypto library.
	*								See TCrypto::IsSymmetricWeakEnoughL()
	*/
	IMPORT_C static CARC4* NewLC(const TDesC8& aKey, 
		TUint aDiscardBytes = KDefaultDiscardBytes);
public:	
	virtual void Reset(void);
	virtual TInt KeySize(void) const;
protected:
	/**	
	 * Performs an ARC4 encryption or decryption on supplied data.
	 * 
	 * @note ARC4 encryption and decryption are symmetrical.
	 *
	 * @param aData	On input, data to be transformed; 
	 *				on return, transformed data.
	 */
	virtual void DoProcess(TDes8& aData);
private:
	CARC4(const TDesC8& aKey, TUint aDiscardBytes);
	void GenerateSBox();
	inline TUint8 GenerateByte();
	void DiscardBytes(TInt aDiscardBytes);
private:
	TUint8 ix;
	TUint8 iy;
	TInt iDiscardBytes;
	TUint8 iState[KSBoxSize];
	TBuf8<KMaxARC4KeyBytes> iKey;
};

#endif	//	__ARC4_H__
