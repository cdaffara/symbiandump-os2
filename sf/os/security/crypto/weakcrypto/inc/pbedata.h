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

#ifndef __PBEDATA_H__
#define __PBEDATA_H__

#include <s32strm.h>
#include <pbe.h>

/**
 * Contains the Password Based Encryption parameters.
 * An object of this class can be input for CPBEncryptElement or CPBEncryptSet objects.
 *
 * @since v7.0s
 */
class CPBEncryptParms : public CBase
	{
public:
	enum TKdf
	/**
		Key derivation function.
		This does not affect the authentication key,
		which always uses PKCS#5.
	 */
		{
		EKdfPkcs5,
		EKdfPkcs12
		};
	
	/**
	 * Creates a new CPBEncryptParms object.
	 *
	 * @param aCipher		The cipher to use
	 * @param aSalt			The salt
	 * @param aIV			The Initialization Vector
	 * @param aIterations	The number of iterations of the PBE algorithm
	 * @return				A pointer to the new CPBEncryptParms object
	 */
	IMPORT_C static CPBEncryptParms* NewL(TPBECipher aCipher,
		const TDesC8& aSalt, const TDesC8& aIV, TUint aIterations);

	/**
	 * Creates a new CPBEncryptParms object and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aCipher		The cipher to use
	 * @param aSalt			The salt
	 * @param aIV			The Initialization Vector
	 * @param aIterations	The number of iterations of the PBE algorithm
	 * @return				A pointer to the new CPBEncryptParms object
	 */
	IMPORT_C static CPBEncryptParms* NewLC(TPBECipher aCipher,
		const TDesC8& aSalt, const TDesC8& aIV, TUint aIterations);

	/**
	 * Creates a new CPBEncryptParms object from an existing object.
	 *
	 * @param aParms		The existing encryption parameters object
	 * @return				A pointer to the new CPBEncryptParms object
	 */
	IMPORT_C static CPBEncryptParms* NewL(const CPBEncryptParms& aParms);

	/**
	 * Creates a new CPBEncryptParms object from an existing object
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aParms		The existing encryption parameters object
	 * @return				A pointer to the new CPBEncryptParms object
	 */
	IMPORT_C static CPBEncryptParms* NewLC(const CPBEncryptParms& aParms);

	/**
	 * Internalizes encryption parameter data from a read stream.
	 *
	 * @param aStream	The read stream to be internalized
	 * @return			A pointer to the new CPBEncryptParms object
	 * 
	 */
	IMPORT_C static CPBEncryptParms* NewL(RReadStream& aStream);

	/**
	 * Internalizes encryption parameter data from a read stream, and 
	 * puts a pointer to the new object onto the cleanup stack.
	 *
	 * @param aStream	The read stream to be internalized
	 * @return			A pointer to the new CPBEncryptParms object
	 */
	IMPORT_C static CPBEncryptParms* NewLC(RReadStream& aStream);

	/**
	 * Gets the PBE cipher
	 *
	 * @return	The cipher to use
	 */
	IMPORT_C TPBECipher Cipher() const;
	/**
		Gets the key derivation function (KDF.)
		
		@return The key derivation function (KDF) which
			transforms the password into an encryption key.
	 */
	IMPORT_C TKdf Kdf() const;

	/**
	 * Gets the PBE salt
	 *
	 * @return	The salt
	 */
	IMPORT_C TPtrC8 Salt() const;

	/**
	 * Gets the number of iterations for the PKCS#5 algorithm.
	 *
	 * @return	The number of iterations
	 */
	IMPORT_C TInt Iterations() const;

	/**
	 * Gets the PBE Initialization Vector
	 *
	 * @return	The IV
	 */
	IMPORT_C TPtrC8 IV() const;

	IMPORT_C void SetIV(const TDesC8& aNewIv);

	/**
	 * Externalizes the encryption parameters into a write stream.
	 *
	 * @param aStream	The stream to write to
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C static CPBEncryptParms* NewL();
	IMPORT_C static CPBEncryptParms* NewLC();
	
	IMPORT_C void SetCipher(TPBECipher aCipher);
	IMPORT_C void SetKdf(TKdf aKdf);
	IMPORT_C void ResizeSaltL(TInt aNewLen);
	IMPORT_C void SetIterations(TInt aIterCount);

	/** Destructor */
	virtual ~CPBEncryptParms(void);
	/** @internalAll */
	void DeriveKeyL(const TDesC8& aPassword, TDes8& aKeyBuf) const;

	
protected:	
#ifdef PBEDATA_NO_EXPORTED_CONSTRUCTORS
	/** @internalAll */
	CPBEncryptParms(void);
#else
	/** Constructor */
	IMPORT_C CPBEncryptParms(void);
#endif

	/** @internalAll */
	void ConstructL();

	/** @internalAll*/
	void ConstructL(TPBECipher aCipher, const TDesC8& aSalt,
		const TDesC8& aIV, TUint aIterations);
	/** @internalAll*/
	void ConstructL(const CPBEncryptParms& aParms);
	/** @internalAll*/
	void ConstructL(RReadStream& aStream);
private:
	CPBEncryptParms(const CPBEncryptParms&);
	CPBEncryptParms& operator= (const CPBEncryptParms&);
private:
	class TParamsData
	/**
		This class contains multiple objects.  Its
		pointer is stored in CPBEncryptParms to preserve
		BC, because CPBEncryptParms has an exported,
		protected constructor.
	 */
		{
	public:
		TPBECipher iCipher;
		/**
			The password is processed with this function
			to generate the encryption key.
		 */
		TKdf iKdf;
		};
	TParamsData* iData;

	HBufC8* iSalt;
	HBufC8* iIV;
	TUint iIterations;
	};


/**
 * Contains the password based authentication data.
 * Used to check the passphrase when decrypting.
 *
 * @since v7.0s
 */
class CPBAuthData : public CBase
	{
public:
	/**
	 * Derives an authentication key.
	 *
	 * @param aPassword		The user's initial password
	 * @param aSalt			The salt
	 * @param aKeySize		The key size
	 * @param aIterations	The number of iterations of the PBE algorithm
	 * @return				A pointer to the new CPBAuthData object
	 */
	IMPORT_C static CPBAuthData* NewL(const TDesC8& aPassword, 
		const TDesC8& aSalt, TUint aKeySize, TUint aIterations);

	/**
	 * Derives an authentication key.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aPassword		The user's initial password
	 * @param aSalt			The salt
	 * @param aKeySize		The key size
	 * @param aIterations	The number of iterations of the PBE algorithm
	 * @return				A pointer to the new CPBAuthData object
	 */
	IMPORT_C static CPBAuthData* NewLC(const TDesC8& aPassword, 
		const TDesC8& aSalt, TUint aKeySize, TUint aIterations);

	/**
	 * Creates a new CPBAuthData object from an existing authentication key.
	 *
	 * @param aData	The existing CPBAuthData object
	 * @return		A pointer to the new CPBAuthData object
	 */
	IMPORT_C static CPBAuthData* NewL(const CPBAuthData& aData);

	/**
	 * Creates a new CPBAuthData object from an existing authentication key.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aData	The existing CPBAuthData object
	 * @return		A pointer to the new CPBAuthData object
	 */
	IMPORT_C static CPBAuthData* NewLC(const CPBAuthData& aData);

	/**
	 * Creates a new CPBAuthData object from an existing authentication key
	 * by internalizing the authentication data from a read stream.
	 *
	 * @param aStream	The stream to read from
	 * @return			A pointer to the new CPBAuthData object
	 */
	IMPORT_C static CPBAuthData* NewL(RReadStream& aStream);

	/**
	 * Creates a new CPBAuthData object from an existing authentication key
	 * by internalizing the authentication data from a read stream.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aStream	The stream to read from
	 * @return			A pointer to the new CPBAuthData object
	 */
	IMPORT_C static CPBAuthData* NewLC(RReadStream& aStream);

	/**
	 * Gets the authentication key
	 *
	 * @return	The key
	 */
	IMPORT_C TPtrC8 Key() const;

	/**
	 * Gets the salt used for the authentication 
	 *
	 * @return	The salt
	 */
	IMPORT_C TPtrC8 Salt() const;

	/**
	 * Gets the number of iterations of the hashing algorithm.
	 *
	 * @return	The number of iterations
	 */
	IMPORT_C TInt Iterations() const;

	/**
	 * Tests whether two authentication keys are identical 
	 *
	 * @param aAuth	The authentication data object which holds the key to be tested
	 * @return		ETrue, if they are identical; EFalse, otherwise
	 */
	IMPORT_C TBool operator==(const CPBAuthData& aAuth) const;

	/**
	 * Externalizes the encryption parameters into a write stream.
	 *
	 * @param aStream	The stream to write to
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	/** Destructor */
	virtual ~CPBAuthData(void);
protected:
#ifdef PBEDATA_NO_EXPORTED_CONSTRUCTORS
	/** @internalAll */
	CPBAuthData(void);
#else
	/** Constructor */
	IMPORT_C CPBAuthData(void);
#endif
	/** @internalAll */
	void ConstructL(const TDesC8& aPassword, const TDesC8& aSalt,
		TUint aKeySize, TUint aIterations);
	/** @internalAll */
	void ConstructL(const CPBAuthData& aData);
	/** @internalAll */
	void ConstructL(RReadStream& aStream);
private:
	CPBAuthData(const CPBAuthData&);
	CPBAuthData& operator= (const CPBAuthData&);
private:
	HBufC8* iAuthKey;
	HBufC8* iSalt;
	TUint iIterations;
	};

/** 
 * Represents the information needed to decrypt encrypted data given the correct password.  
 * Contains the authentication key, and the parameters used to derive the encryption key.
 * A CPBEncryptionData object needs to be stored to recover any data for later use.
 *
 * @see CPBEncryptParms
 * @see CPBAuthData
 *
 * @since v7.0s
 */
class CPBEncryptionData : public CBase
	{
public:
	/**
	 * Creates a new CPBEncryptionData object 
	 *
	 * @param aPassword		The user's initial password
	 * @param aCipher		The cipher to use
	 * @param aAuthSalt		The salt used for the authentication
	 * @param aEncryptSalt	The salt used for the encryption
	 * @param aIV			The Initialization Vector
	 * @param aIterations	The number of iterations of the PBE algorithm
	 * @return				A pointer to the new CPBEncryptionData object
	 */
	IMPORT_C static CPBEncryptionData* NewL(const TDesC8& aPassword,
		TPBECipher aCipher, const TDesC8& aAuthSalt, 
		const TDesC8& aEncryptSalt, const TDesC8& aIV, TUint aIterations);

	/**
	 * Creates a new CPBEncryptionData object 
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aPassword		The user's initial password
	 * @param aCipher		The cipher to use
	 * @param aAuthSalt		The salt used for the authentication
	 * @param aEncryptSalt	The salt used for the encryption
	 * @param aIV			The Initialization Vector
	 * @param aIterations	The number of iterations of the PBE algorithm
	 * @return				A pointer to the new CPBEncryptionData object
	 */
	IMPORT_C static CPBEncryptionData* NewLC(const TDesC8& aPassword,
		TPBECipher aCipher, const TDesC8& aAuthSalt, 
		const TDesC8& aEncryptSalt, const TDesC8& aIV, TUint aIterations);

	/**
	 * Creates a new CPBEncryptionData from an existing one.
	 *
	 * @param aData	The existing CPBEncryptionData object
	 * @return		A pointer to the new CPBEncryptionData object
	 */
	IMPORT_C static CPBEncryptionData* NewL(const CPBEncryptionData& aData);

	/**
	 * Creates a new CPBEncryptionData from an existing one,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aData	The existing CPBEncryptionData object
	 * @return		A pointer to the new CPBEncryptionData object
	 */
	IMPORT_C static CPBEncryptionData* NewLC(const CPBEncryptionData& aData);

	/**
	 * Internalizes the encryption data from a read stream.
	 *
	 * @param aStream	The stream to read from
	 * @return			A pointer to the new CPBEncryptionData object
	 */
	IMPORT_C static CPBEncryptionData* NewL(RReadStream& aStream);

	/**
	 * Internalizes the encryption data from a read stream,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aStream	The stream to read from
	 * @return			A pointer to the new CPBEncryptionData object
	 */
	IMPORT_C static CPBEncryptionData* NewLC(RReadStream& aStream);
	IMPORT_C static CPBEncryptionData* NewL(
		const TDesC8& aPassword, const TDesC8& aAuthSalt,
		const CPBEncryptParms& aParms);


	/**
	 * Returns the encryption parameter object.
	 *
	 * @return	The CPBEncryptParms object
	 */
	IMPORT_C const CPBEncryptParms& EncryptParms() const;

	/**
	 * Returns the authentication data object.
	 *
	 * @return	The CPBAuthData object
	 */
	IMPORT_C const CPBAuthData& AuthData() const;

	/**
	 * Externalizes the encryption data into a write stream.
	 *
	 * @param aStream	The stream to write to
	 */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	/** Destructor */
	virtual ~CPBEncryptionData(void);
protected:
#ifdef PBEDATA_NO_EXPORTED_CONSTRUCTORS
	/** @internalAll */
	CPBEncryptionData(void);
#else
	/** Constructor */
	IMPORT_C CPBEncryptionData(void);
#endif
	/** @internalAll */
	void ConstructL(const TDesC8& aPassword, TPBECipher aCipher,
		const TDesC8& aAuthSalt, const TDesC8& aEncryptSalt, const TDesC8& aIV,
		TUint aIterations);
	/** @internalAll */
	void ConstructL(const CPBEncryptionData& aData);
	/** @internalAll */
	void ConstructL(
		const TDesC8& aPassword, const TDesC8& aAuthSalt,
		const CPBEncryptParms& aParms);
	/** @internalAll */
	void ConstructL(RReadStream& aStream);
private:
	CPBEncryptionData(const CPBEncryptionData&);
	CPBEncryptionData& operator= (const CPBEncryptionData&);
private:
	CPBEncryptParms* iParms;
	CPBAuthData* iAuth;
	};

#endif
