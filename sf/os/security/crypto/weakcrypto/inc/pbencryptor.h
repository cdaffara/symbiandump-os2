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

#ifndef __PBENCRYPTOR_H__
#define __PBENCRYPTOR_H__

#include "pbe.h"
#include "padding.h"
#include "msymmetriccipher.h"

/**
 * Implements the password based encryption of elements.
 *
 * @see CPBEncryptElement
 * @since v7.0s
 */
class CPBEncryptorElement : public CPBEncryptor
	{
public:
	/**
	 * Creates a new CPBEncryptorElement object from the specified cipher, 
	 * key, and Initialization Vector (IV).
	 *
	 * @param aCipher	The encryption cipher
	 * @param aKey		The encryption key
	 * @param aIV		The Initialization Vector
	 * @return			A pointer to the new CPBEncryptorElement object
	 */
	IMPORT_C static CPBEncryptorElement* NewL(TPBECipher aCipher, 
		const TDesC8& aKey, const TDesC8& aIV);

	/**
	 * Creates a new CPBEncryptorElement object from the specified cipher, 
	 * key, and IV.
	 * 
	 * Puts a pointer to the returned object onto the cleanup stack.
	 *
	 * @param aCipher	The encryption cipher
	 * @param aKey		The encryption key
	 * @param aIV		The Initialization Vector
	 * @return			A pointer to the new CPBEncryptorElement object
	 */
	IMPORT_C static CPBEncryptorElement* NewLC(TPBECipher aCipher, 
		const TDesC8& aKey, const TDesC8& aIV);

	/** 
	 * Transforms aInput into its encrypted form, aOutput.
	 *
	 * aOutput must have CPBEncryptorElement::MaxOutputLength() empty bytes remaining in its length. 
	 *
	 *	See the Cryptography api-guide documentation for an explanation of
	 *	how buffering of data supplied to this function is handled.
	 *
	 * @param aInput	The plaintext.
	 * @param aOutput	The ciphertext.
	 */
	void Process(const TDesC8& aInput, TDes8& aOutput);

	/** 
	 * Transforms aInput into its encrypted form, aOutput, and applies a
	 * padding scheme to ensure a block aligned result.
	 *
	 * aOutput must have CPBEncryptorElement::MaxFinalOutputLength() 
	 * empty bytes remaining in its length. 
	 *
	 *	See the Cryptography api-guide documentation for an explanation of
	 *	how buffering of data supplied to this function is handled.
	 * 
	 * @param aInput	The plaintext.
	 * @param aOutput	The ciphertext.
	 */
	void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);

	/** 
	 * Gets the maximum size of the output resulting from calling Process() with a
	 * given input length.
	 *
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	TInt MaxOutputLength(TUint aMaxInputLength) const;

	/** 
	 * Gets the maximum size of the output resulting from calling ProcessFinalL()
	 * with a given input length.
	 *
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					TInt The maximum output length in bytes.
	 */
	TInt MaxFinalOutputLength(TUint aMaxInputLength) const;

	/** Destructor */
	virtual ~CPBEncryptorElement();
protected:
	/* @internalComponent */
	CPBEncryptorElement();
	/* @internalComponent */
	void ConstructL(TPBECipher aCipher, const TDesC8& aKey, const TDesC8& aIV);
private:
	CSymmetricCipher* iCipher;
	};

/**
 * Implements the password based decryption of elements.
 *
 * @since v7.0s
 */
class CPBDecryptorElement : public CPBDecryptor
	{
public:
	/**
	 * Creates a new CPBDecryptorElement object from the specified cipher, 
	 * key, and IV.
	 *
	 * @param aCipher	The decryption cipher
	 * @param aKey		The decryption key
	 * @param aIV		The Initialization Vector
	 * @return			A pointer to the new CPBDecryptorElement object
	 */
	IMPORT_C static CPBDecryptorElement* NewL(const TPBECipher aCipher, 
		const TDesC8& aKey, const TDesC8& aIV);
	
	/**
	 * Creates a new CPBDecryptorElement object from the specified cipher, 
	 * key, and IV.
	 * 
	 * Puts a pointer to the returned object onto the cleanup stack.
	 *
	 * @param aCipher	The decryption cipher
	 * @param aKey		The decryption key
	 * @param aIV		The Initialization Vector
	 * @return			A pointer to the new CPBDecryptorElement object
	 */
	IMPORT_C static CPBDecryptorElement* NewLC(const TPBECipher aCipher, 
		const TDesC8& aKey, const TDesC8& aIV);

	/** 
	 * Transforms aInput into its decrypted form, aOutput.
	 *
	 * aOutput must have CPBDecryptorElement::MaxOutputLength() empty bytes
	 * remaining in its length. 
	 *
	 *	See the Cryptography api-guide documentation for an explanation of
	 *	how buffering of data supplied to this function is handled.
	 * 
	 * @param aInput	The ciphertext.
	 * @param aOutput	The plaintext.
	 */
	void Process(const TDesC8& aInput, TDes8& aOutput);

	/** 
	 * Transforms aInput into its decrypted form, aOutput.
	 *
	 * aOutput must have CPBDecryptorElement::MaxFinalOutputLength() 
	 * empty bytes remaining in its length. 
	 * 
	 * @param aInput	The ciphertext.
	 * @param aOutput	The plaintext.
	 */
	void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);

	/** 
	 * Gets the maximum size of the output given a certain input length.
	 * 
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	TInt MaxOutputLength(TUint aMaxInputLength) const;

	/** 
	 * Gets the maximum size of the output given a certain input length.
	 * 
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	TInt MaxFinalOutputLength(TUint aMaxInputLength) const;

	/** Destructor */
	virtual ~CPBDecryptorElement();
protected:
	/* @internalComponent */
	CPBDecryptorElement();
	/* @internalComponent */
	void ConstructL(const TPBECipher aCipher, const TDesC8& aKey, const TDesC8& aIV);
private:
	CSymmetricCipher* iCipher;
	};

/**
 * Implements the password based encryption of multiple elements.
 *
 * @see CPBEncryptSet
 * @since v7.0s
 */
class CPBEncryptorSet : public CPBEncryptor
	{
public:
	/**
	 * Creates a new CPBEncryptorSet object from the specified cipher and key,
	 * and a random Initialization Vector (IV).
	 *
	 * @param aCipher	The encryption cipher
	 * @param aKey		The encryption key
	 * @return			A pointer to the new CPBEncryptorSet object
	 */
	IMPORT_C static CPBEncryptorSet* NewL(const TPBECipher aCipher, 
		const TDesC8& aKey);

	/**
	 * Creates a new CPBEncryptorSet object from the specified cipher and key,
	 * and a random IV.
	 * 
	 * Puts a pointer to the returned object onto the cleanup stack.
	 *
	 * @param aCipher	The encryption cipher
	 * @param aKey		The encryption key
	 * @return			A pointer to the new CPBEncryptorSet object
	 */
	IMPORT_C static CPBEncryptorSet* NewLC(const TPBECipher aCipher, 
		const TDesC8& aKey);

	/**
	 * Resets the CPBEncryptorSet object back to its original state
	 * and clears all its buffers.
	 */
	IMPORT_C void Reset(void);

	/** 
	 * Transforms aInput into its encrypted form, aOutput.
	 *
	 * aOutput must have CPBEncryptorSet::MaxOutputLength() empty bytes
	 * remaining in its length. 
	 *
	 * @param aInput	The plaintext.
	 * @param aOutput	The ciphertext.
	 */
	void Process(const TDesC8& aInput, TDes8& aOutput);

	/** 
	 * Transforms aInput into its encrypted form, aOutput, and applies a
	 * padding scheme to ensure a block aligned result.
	 *
	 * aOutput must have CPBEncryptorSet::MaxFinalOutputLength() 
	 * empty bytes remaining in its length. 
	 * 
	 * @param aInput	The plaintext.
	 * @param aOutput	The ciphertext.
	 */
	void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);

	/** 
	 * Gets the maximum size of the output given a certain input length.
	 * 
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	TInt MaxOutputLength(TUint aMaxInputLength) const;

	/** 
	 * Gets the maximum size of the output given a certain input length.
	 * 
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	TInt MaxFinalOutputLength(TUint aMaxInputLength) const;

	/** Destructor */
	virtual ~CPBEncryptorSet();
protected:
	/* @internalComponent */
	CPBEncryptorSet();
	/* @internalComponent */
	void ConstructL(TPBECipher aCipher, const TDesC8& aKey);
private:
	CSymmetricCipher* iCipher;
	HBufC8* iIV;
	TBool iIVSent;
	};


/**
 * Implements the password based decryption of multiple elements.
 *
 * @since v7.0s
 */
class CPBDecryptorSet : public CPBDecryptor
	{
public:
	/**
	 * Creates a new CPBDecryptorSet object from the specified cipher and key,
	 * and a random IV.
	 *
	 * @param aCipher	The decryption cipher
	 * @param aKey		The decryption key
	 * @return			A pointer to the new CPBDecryptorSet object
	 */
	IMPORT_C static CPBDecryptorSet* NewL(const TPBECipher aCipher, 
		const TDesC8& aKey);

	/**
	 * Creates a new CPBDecryptorSet object from the specified cipher and key,
	 * and a random IV.
	 * 
	 * Puts a pointer to the returned object onto the cleanup stack.
	 *
	 * @param aCipher	The decryption cipher
	 * @param aKey		The decryption key
	 * @return			A pointer to the new CPBDecryptorSet object
	 */
	IMPORT_C static CPBDecryptorSet* NewLC(const TPBECipher aCipher, 
		const TDesC8& aKey);

	/**
	 * Resets the CPBDecryptorSet object back to its original state
	 * and clears all its buffers.
	 */
	IMPORT_C void Reset(void);

	/** 
	 * Transforms aInput into its decrypted form, aOutput.
	 *
	 * aOutput must have CPBDecryptorSet::MaxOutputLength() empty bytes 
	 * remaining in its length. 
	 *
	 * @param aInput	The ciphertext.
	 * @param aOutput	The plaintext.
	 */
	void Process(const TDesC8& aInput, TDes8& aOutput);

	/** 
	 * Transforms aInput into its decrypted form, aOutput, and applies a
	 * padding scheme to ensure a block aligned result.
	 *
	 * aOutput must have CPBDecryptorSet::MaxFinalOutputLength() 
	 * empty bytes remaining in its length. 
	 * 
	 * @param aInput	The ciphertext.
	 * @param aOutput	The plaintext.
	 */
	void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput);

	/** 
	 * Gets the maximum size of the output given a certain input length.
	 * 
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	TInt MaxOutputLength(TUint aMaxInputLength) const;

	/** 
	 * Gets the maximum size of the output given a certain input length.
	 * 
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	TInt MaxFinalOutputLength(TUint aMaxInputLength) const;

	/** Destructor */
	virtual ~CPBDecryptorSet();
protected:
	/* @internalComponent */
	CPBDecryptorSet();
	/* @internalComponent */
	void ConstructL(TPBECipher aCipher, const TDesC8& aKey, const TDesC8& aIV);
private:
	TPtrC8 ProcessIV(const TDesC8& aInput);
private:
	CSymmetricCipher* iCipher;
	HBufC8* iIVBuf;
	TBool iIVSent;
	};

#endif
