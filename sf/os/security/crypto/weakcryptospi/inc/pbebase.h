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

#ifndef __PBEBASE_H__
#define __PBEBASE_H__

#include <e32base.h>

class CPBEncryptParms;
class CPBEncryptionData;
class CPBEncryptSet;
class TPBPassword;

/** 
 * Abstract class defining the interface required to allow the actual
 * transformation of plaintext to ciphertext.
 *
 * Generally this class' descendants are constructed using the
 * functions CPBEncryptElement::NewEncryptLC() or CPBEncryptSet::NewEncryptLC().
 *
 * @see CPBEncryptorElement and CPBEncryptorSet
 */
class CPBEncryptor : public CBase
	{
public:
	/** 
	 * Transforms aInput into its encrypted form, aOutput.
	 *
	 *	See the Cryptography api-guide documentation for an explanation of
	 *	how buffering of data supplied to this function is handled.
	 *
	 * @param aInput	The plaintext.
	 * @param aOutput	On return, the ciphertext.
	 */
	virtual void Process(const TDesC8& aInput, TDes8& aOutput) = 0;

	/** 
	 * Transforms aInput into its encrypted form, aOutput, and applies a
	 * padding scheme to ensure a block aligned result.
	 *
	 *	See the Cryptography api-guide documentation for an explanation of
	 *	how buffering of data supplied to this function is handled.
	 * 
	 * @param aInput	The plaintext.
	 * @param aOutput	On return, the ciphertext.
	 */
	virtual void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput) = 0;

	/** 
	 * Gets the maximum length of the output resulting from calling Process() with a
	 * given input length.
	 *
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	virtual TInt MaxOutputLength(TUint aMaxInputLength) const = 0;
	
	/** 
	 * Gets the maximum length of the output resulting from calling ProcessFinalL()
	 * with a given input length.
	 *
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	virtual TInt MaxFinalOutputLength(TUint aMaxInputLength) const = 0;
};

/** 
 * Abstract class defining the interface required to allow the actual
 * transformation of ciphertext to plaintext.
 *  
 * Generally this class' descendants are constructed using the
 * functions CPBEncryptElement::NewDecryptLC() or CPBEncryptSet::NewDecryptLC().
 */
class CPBDecryptor : public CBase
	{
public:
	/** 
	 * Transforms aInput into its decrypted form, aOutput, and unpads.
	 *
	 *	See the Cryptography api-guide documentation for an explanation of
	 *	how buffering of data supplied to this function is handled.
	 * 
	 * @param aInput	The ciphertext.
	 * @param aOutput	On return, the plaintext.
	 */
	virtual void Process(const TDesC8& aInput, TDes8& aOutput) = 0;

	/** 
	 * Transforms aInput into its decrypted form, aOutput, and unpads.
	 * 
	 * @param aInput	The ciphertext.
	 * @param aOutput	On return, the plaintext.
	 */
	virtual void ProcessFinalL(const TDesC8& aInput, TDes8& aOutput) = 0;

	/** 
	 * Gets the maximum length of the output given a certain input length.
	 * 
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	virtual TInt MaxOutputLength(TUint aMaxInputLength) const = 0;
	
	/** 
	 * Gets the maximum length of the output given a certain input length.
	 * 
	 * @param aMaxInputLength	The maximum input length in bytes.
	 * @return					The maximum output length in bytes.
	 */
	virtual TInt MaxFinalOutputLength(TUint aMaxInputLength) const = 0;
	};

/** 
 * Abstract base class defining the interface required to allow the password
 * based encryption and decryption of single or multiple items or elements.
 * 
 * @see CPBEncryptElement and CPBEncryptSet
 * @since v8.0
 */
class CPBEncryptionBase : public CBase
	{
public:
	/** 
	 * Gets the parameters allowing one to re-create the object with the
	 * same state at another point in the future.  
	 * 
	 * In order to decrypt any information previously encrypted with this object, 
	 * you <B><I>must</I></B> store this encryption data along with it. Failure 
	 * to do this will result in the permanent loss of the encrypted information.
	 * 
	 * @return	The data allowing one to re-create this object at a later time.
	 */
	virtual const CPBEncryptionData& EncryptionData(void) const = 0;

	/** 
	 * Constructs a CPBEncryptor object allowing the encryption of data.
	 *
	 * @return	A pointer to a CPBEncryptor object.
	 *			The caller assumes ownership of the returned object. 
	 */
	virtual CPBEncryptor* NewEncryptL(void) const = 0;

	/** 
	 * Constructs a CPBEncryptor object allowing the encryption of data.
	 * 
	 * @return	A pointer to a CPBEncryptor object.
	 *			The caller assumes ownership of the returned object.
	 *			The returned pointer is left on the cleanup stack.
	 */
	virtual CPBEncryptor* NewEncryptLC(void) const = 0;

	/** 
	 * Constructs a CPBDecryptor object allowing the decryption of data.
	 * 
	 * @return	A pointer to a CPBDecryptor object.
	 *			The caller assumes ownership of the returned object.
	 */
	virtual CPBDecryptor* NewDecryptL(void) const = 0;

	/** 
	 * Constructs a CPBDecryptor object allowing the decryption of data.
	 * 
	 * @return	A pointer to a CPBDecryptor object.
	 *			The caller assumes ownership of the returned object.
	 *			The returned pointer is left on the cleanup stack.
	 */
	virtual CPBDecryptor* NewDecryptLC(void) const = 0;

	/** 
	 * Gets the maximum output ciphertext length given a specified input plaintext length.  
	 * 
	 * @param aPlaintextLength	The plaintext length 
	 * @return					The maximum ciphertext length given a plaintext length.
	 */
	virtual TInt MaxCiphertextLength(TInt aPlaintextLength) const = 0;

	/** 
	 * Gets the maximum output plaintext length given a specified input ciphertext length.
	 *
	 * @param aCiphertextLength	The ciphertext length
	 * @return					The maximum plaintext length given a ciphertext length.
	 */
	virtual TInt MaxPlaintextLength(TInt aCiphertextLength) const = 0;

	};

#endif
