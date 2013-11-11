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
* csymmetriccipher.h
* ** IMPORTANT ** PublishedPartner API's in this file are published to 3rd party developers via the 
* Symbian website. Changes to these API's should be treated as PublishedAll API changes and the Security TA should be consulted.
* CSymmetricCipher class implementation
*
*/


/**
 @file 
 @publishedPartner
 @released
*/
 
#ifndef __CSYMMETRICCIPHER_H__
#define __CSYMMETRICCIPHER_H__

#include <e32base.h>

/**
 * Top-level interface designed to collate the behaviour of all symmetric
 * ciphers under one interface.
 *
 * See the Cryptography api-guide documentation.
 *
 * @publishedPartner
 * @released
 */
class CSymmetricCipher : public CBase
	{
public:
	/**
	 * Runs the underlying transformation on aInput and appends the result to
	 * aOutput.
	 *
	 * For incremental buffering rules see the Cryptography api-guide documentation.
	 *
	 * @param aInput	The input data to be processed.
	 * @param aOutput	The resulting processed data appended to aOutput.  aOutput must
	 *					have MaxOutputLength() empty bytes remaining in its length.
	 */
	virtual void Process(const TDesC8& aInput, TDes8& aOutput) = 0;

	/**
	 * Pads aInput to be block aligned using the underlying padding system, if any,
	 * and then runs the underlying transformation on aInput, and appends the result
	 * to aOutput.  
	 *
	 * For incremental buffering rules see the Cryptography api-guide documentation.
	 *
	 * @param aInput	The input data to be processed.
	 * @param aOutput	The resulting, possibly padded, processed data appended to
	 *					aOutput.  aOutput must have MaxFinalOutputLength() empty bytes
	 *					remaining in its length.
	 */
	virtual void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput) = 0;

	/**	
	 * Gets a tight upper bound on the number of bytes that would be returned by a
	 * call to Process() with aInputLength bytes of data.
	 *
	 * @param aInputLength	The length of data to be supplied to Process() in bytes.
	 * @return				The length of data which would result from a call to 
	 *						Process() with an aInputLength number of bytes.
	 */
	virtual TInt MaxOutputLength(TInt aInputLength) const = 0;

	/**	
	 * Gets as tight an upper bound as possible on the number of bytes that would
	 * be returned by a call to ProcessFinalL() with aInputLength bytes of data.
	 *
	 * @param aInputLength	The length of data to be supplied to Process() in bytes.
	 * @return				An upper bound on the length of data which would result from 
	 *						a call to ProcessFinalL() with an aInputLength number of bytes.
	 */
	virtual TInt MaxFinalOutputLength(TInt aInputLength) const = 0;

	/**
	 * Resets the cipher back to its original state. Clears all its buffers.
	 */
	virtual void Reset() = 0;

	/**
	 * Gets the block size in bytes (1 for stream ciphers).
	 *
	 * @return	Block size of underlying cipher in bytes.
	 */
	virtual TInt BlockSize() const = 0;

	/**
	 * Gets the key size in bits.
	 *	
	 * @return	Key size in bits.
	 */
	virtual TInt KeySize() const = 0;
};


#endif	//	__CSYMMETRICCIPHER_H__
