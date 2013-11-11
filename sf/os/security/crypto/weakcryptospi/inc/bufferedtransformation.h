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
 
#ifndef __BUFFEREDTRANSFORMATION_H__
#define __BUFFEREDTRANSFORMATION_H__

#include <e32std.h>
#include <msymmetriccipher.h>
#include <blocktransformation.h>

class CPadding;

/**
 * Abstract class, deriving from CSymmetricCipher, encapsulating the buffering
 * logic for block ciphers.
 *
 * It is responsible for feeding complete blocks of plaintext or ciphertext to
 * the underlying encryptor or decryptor.  Since the only difference between
 * block cipher encryption and decryption is the ProcessFinalL() call,
 * CBufferedTransformation implements all functions (by buffering and/or
 * forwarding to the encryptor/decryptor) except ProcessFinalL() and
 * MaxFinalOutputLength().
 *
 * See the Cryptography api-guide documentation for the rules that this class
 * and derived classes must follow.
 *
 */
class CBufferedTransformation : public CSymmetricCipher
{
public:
	/** The destructor frees all resources owned by the object, prior to its destruction. */
	IMPORT_C virtual ~CBufferedTransformation();
public:
	/**
	 * Encrypts or decrypts the input using the underlying block cipher, buffering
	 * the input as necessary. 
	 *
	 * See the Cryptography api-guide documentation.
	 *
	 * @param aInput	The input is appended to the internal buffer (initially empty),
	 *					then all whole blocks are encrypted using the underlying block
	 *					transformation and written into aOutput. Any leftover bytes will
	 *					be buffered.
	 * @param aOutput	The resulting processed data appended to aOutput.  aOutput must
	 *					have at least MaxOutputLength() empty bytes remaining in its length.
	 */
	virtual void Process(const TDesC8& aInput, TDes8& aOutput);
	virtual TInt MaxOutputLength(TInt aInputLength) const;
	virtual void Reset();
	virtual TInt BlockSize() const;
	virtual TInt KeySize() const;
public:
	/** 
	 * Gets the underlying block transform.
	 *
	 * @return	A pointer to the CBlockTransformation object
	 */
	 IMPORT_C CBlockTransformation* BlockTransformer() const;
protected:
	/** @internalAll */
	CBufferedTransformation();
	/** @internalAll */
	void ConstructL(CBlockTransformation* aBT, CPadding* aPadding);
protected:
	/** A block transformation object */
	CBlockTransformation* iBT;
	/** A descriptor which provides a buffer the length of the block size of iBT */
	HBufC8* iInputStoreBuf;
	/** A pointer to iInputStoreBuf */
	TPtr8 iInputStore;
	/** The padding */
	CPadding* iPadding;
};

/**
 * Subclass of CBufferedTransformation for buffered encryption.
 *
 * Objects of this class are intialised with, and subsequently own, an encryptor
 * derived from CBlockTransformation and a subclass of CPadding.
 *
 */
class CBufferedEncryptor : public CBufferedTransformation
{
public:
	/**
	 * Creates a CBufferedEncryptor object taking ownership of aBT and aPadding.
	 *
	 * @param aBT		Block transformation object (encryptor)
	 * @param aPadding	Padding object (deriving from CPadding)
	 * @return			A pointer to the new CBufferedEncryptor object
	 */
	IMPORT_C static CBufferedEncryptor* NewL(CBlockTransformation* aBT, 
		CPadding* aPadding);

	/**
	 * Creates a CBufferedEncryptor object taking ownership of aBT and aPadding.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aBT		Block transformation object (encryptor)
	 * @param aPadding	Padding object (deriving from CPadding)
	 * @return			A pointer to the new CBufferedEncryptor object
	 */
	IMPORT_C static CBufferedEncryptor* NewLC(CBlockTransformation* aBT, 
		CPadding* aPadding);
public:
	/**
	 * Completes an encryption operation using the underlying block transformation, but
	 * first ensuring that input data is block aligned using the previously supplied
	 * CPadding object.  
	 *
	 * See the Cryptography api-guide documentation.
	 *
	 * @param aInput	The final input data to be processed.
	 * @param aOutput	The resulting processed data appended to aOutput.  aOutput must
	 *					have at least MaxFinalOutputLength() empty bytes remaining in its
	 *					length.
	 */
	virtual void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);
	virtual TInt MaxFinalOutputLength(TInt aInputLength) const;
protected:
	/** @internalComponent */
	void ConstructL(CBlockTransformation* aBT, CPadding* aPadding);

	/** @internalAll */
	CBufferedEncryptor();
};

/**
 * Subclass of CBufferedTransformation for buffered decryption.
 *
 * Objects of this class are intialised with, and subsequently own, a decryptor
 * derived from CBlockTransformation and a subclass of CPadding.
 *
 */
class CBufferedDecryptor : public CBufferedTransformation
{
public:
	/**
	 * Creates a CBufferedDecryptor object taking ownership of aBT and aPadding.
	 *
	 * @param aBT		Block transformation object (decryptor)
	 * @param aPadding	Padding object (deriving from CPadding)
	 * @return			A pointer to the CBufferedDecryptor object.
	 */
	IMPORT_C static CBufferedDecryptor* NewL(CBlockTransformation* aBT, 
		CPadding* aPadding);

	/**
	 * Creates a CBufferedDecryptor object taking ownership of aBT and aPadding.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aBT		Block transformation object (decryptor)
	 * @param aPadding	Padding object (deriving from CPadding)
	 * @return			A pointer to the new CBufferedDecryptor object
	 */
	IMPORT_C static CBufferedDecryptor* NewLC(CBlockTransformation* aBT, 
		CPadding* aPadding);
public:
	/**
	 * Completes a decryption operation using the underlying block transformation and
	 * unpads the decrypted data.
	 *
	 * @param aInput	The data to be processed and unpadded.  
	 *					aInput must be a whole number of blocks.
	 * @param aOutput	The resulting processed and unpadded data appened to aOutput.
	 *					aOutput must have at least MaxFinalOutputLength() empty bytes
	 *					remaining in its length.
	 */
	virtual void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);
	virtual TInt MaxFinalOutputLength(TInt aInputLength) const;
protected:
	/** @internalComponent */
	void ConstructL(CBlockTransformation* aBT, CPadding* aPadding);

	/** @internalAll */
	CBufferedDecryptor();
};

#endif	//	__CBUFFEREDTRANSFORMATION_H__
