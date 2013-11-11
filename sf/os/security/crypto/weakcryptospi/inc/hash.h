/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This header contains the definition of the message digest classes
*
*/


/**
 @file 
 @publishedAll
 @released
*/

#ifndef __HASH_H__
#define __HASH_H__

#include <e32base.h>

/**
 * Base class for message digests.
 */
class CMessageDigest:public CBase
	{
	public:
	/**
	 *	Enumeration to identify hash functions (aka message-digest algorithms).
	 */
	enum THashId
	{
		/** 
		 * Message Digest algorithm developed by Rivest for digital signature
		 * applications (and optimized for 8-bit machines). 
		 * 
		 * Takes a message of arbitrary length and produces a 128-bit message digest. 
		 *
		 * See RFC 1319
		 */
		EMD2,
		/** 
		 * Message Digest algorithm developed by Rivest for digital signature
		 * applications (and optimized for 32-bit machines). 
		 * 
		 * Takes a message of arbitrary length and produces a 128-bit message digest. 
		 * 
		 * See RFC 1321
		 */
		EMD5,
		/** 
		 * Secure Hash Algorithm (version 1) is a message digest algorithm developed by 
		 * NIST, along with the NSA, for use with the Digital Signature Standard (DSS).
		 * 
		 * It takes a message of less than 2^64 bits in length and produces
		 * a 160-bit message digest.
		 *
		 * See FIPS 180-1 and RFC 3174
		 */
		ESHA1,
		/** 
		 * HMAC - Hash function based Message Authentication Code is a mechanism
		 * for message authentication using cryptographic hash functions. (A checksum.)
		 * 
		 * It can be used with any iterative cryptographic hash function,
   		 * e.g., MD5, SHA-1, in combination with a secret shared key
   		 * to produce a checksum that is appended to the message.
   		 * The cryptographic strength of HMAC depends on the properties
   		 * of the underlying hash function.
		 *
		 * See RFC 2104 
		 */
		HMAC,
		/** 
		 * Message Digest algorithm developed by Rivest for digital signature
		 * applications (and optimized for 32-bit machines). 
		 * 
		 * Takes a message of arbitrary length and produces a 128-bit message digest. 
		 * 
		 * See RFC 1320
		 */
		EMD4,
		/** 
		 * Secure Hash Algorithm - 224 (version 2) is a message digest algorithm developed by 
		 * NIST, along with the NSA, for use with the Digital Signature Standard (DSS).
		 * 
		 * It takes a message of less than 2^64 bits in length and produces
		 * a 224-bit message digest.
		 *
		 * See FIPS 180-2 (with change notice), RFC3874 and FIPS 180-3
		 */
		ESHA224,
		/** 
		 * Secure Hash Algorithm - 256 (version 2) is a message digest algorithm developed by 
		 * NIST, along with the NSA, for use with the Digital Signature Standard (DSS).
		 * 
		 * It takes a message of less than 2^64 bits in length and produces
		 * a 256-bit message digest.
		 *
		 * See FIPS 180-2 and RFC 4634
		 */
		ESHA256,
		/** 
		 * Secure Hash Algorithm - 384 (version 2) is a message digest algorithm developed by 
		 * NIST, along with the NSA, for use with the Digital Signature Standard (DSS).
		 * 
		 * It takes a message of less than 2^128 bits in length and produces
		 * a 384-bit message digest.
		 *
		 * See FIPS 180-2 and RFC 4634
		 */
		ESHA384,
		/** 
		 * Secure Hash Algorithm - 512 (version 2) is a message digest algorithm developed by 
		 * NIST, along with the NSA, for use with the Digital Signature Standard (DSS).
		 * 
		 * It takes a message of less than 2^128 bits in length and produces
		 * a 512-bit message digest.
		 *
		 * See FIPS 180-2 and RFC 4634
		 */
		ESHA512
	};
	
	public:
		/** 
		 * Creates a brand new reset CMessageDigest object containing no state
		 * information from the current object.  
		 * 
		 * To make a copy of a message digest with its internal state intact,
		 * see CopyL().
		 *
		 * @return A pointer to the new reset CMessageDigest object
		 */
		IMPORT_C virtual CMessageDigest* ReplicateL(void)=0;		

		/** 
		 * Adds aMessage to the internal representation of data to be hashed,
		 * then returns a TPtrC8 of the finalised hash of all the previously
		 * appended messages.
		 * 
		 * @param aMessage	Data to be included in the hash.
		 * @return			A descriptor pointer to the buffer containing the
		 *					resulting hash.
		 */
		IMPORT_C virtual TPtrC8 Hash(const TDesC8& aMessage)=0;

		/** 
		 * Creates a new CMessageDigest object with the exact same state as
		 * the current object.  
		 *
		 * This function copies all internal state of the message digest.
		 * To create a new CMessageDigest object without the state of
		 * the current object, see ReplicateL().
		 *
		 * @return A pointer to the new CMessageDigest object
		 */
		IMPORT_C virtual CMessageDigest* CopyL(void)=0;
		
		/** 
		 * Gets the internal block size of the message digest.
		 * 
		 * @return	Internal block size of message digest in bytes.
		 */
		IMPORT_C virtual TInt BlockSize(void)=0;
		
		/** 
		 * Gets the size of the message digest output.
		 *
		 * @return	Output size of the message digest in bytes.
		 */
		IMPORT_C virtual TInt HashSize(void)=0;
		
		/** 
		 * Resets the internal state of the message digest.  
		 *
		 * A reset hash object loses all internal state representing the hashed
		 * data. A reset message digest is suitable to begin a new, distinct hash
		 * of different data.  Any previously returned TPtrC8 from a call to
		 * Final() remains valid until any subsequent call to Update() or
		 * Final().
		 */
		IMPORT_C virtual void Reset(void)=0;
		
		/**
		 * Destructor.
		 */		
		IMPORT_C ~CMessageDigest(void);
	public:
		/** 
		 * Adds data to the internal representation of messages to be hashed.
		 *
		 * @param aMessage	Data to be included in the hash.
		 * @since v8.0
		 */
		IMPORT_C virtual void Update(const TDesC8& aMessage)=0;
		
		/** 
		 * Adds aMessage to the internal representation of data to be hashed,
		 * returns a TPtrC8 of the finalised hash of all the previously
		 * appended messages, and calls Reset().
		 * 
		 * @param aMessage	Data to be included in the hash 
		 * @return			A descriptor pointer to the buffer containing the
		 *					resulting hash.
		 * @since v8.0
		 */
		IMPORT_C virtual TPtrC8 Final(const TDesC8& aMessage)=0;
		
		/** 
		 * Gets a TPtrC8 of the finalised hash of all the previously
		 * appended messages and then calls Reset().
		 * 
		 * @return	A descriptor pointer to the buffer containing the
		 * 			resulting hash.
		 * @since v8.0
		 */
		IMPORT_C virtual TPtrC8 Final(void)=0;
	public:
		/**
		 * Restores the internal state of the message digest
		 * to a previously stored state.
		 *
		 * @see StoreState()
		 */
		virtual void RestoreState() = 0;

		/**
		 * Stores the internal state of the message digest. 
		 */
		virtual void StoreState() = 0;

		/**
		@internalComponent
		Used to retrieve the extended interfaces extension
		*/		
		TInt GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1);		

	protected:
		/**
		 * Constructor
		 */
		IMPORT_C CMessageDigest(void);

		/**
		 * Copy constructor
		 *
		 * @param aMD	A CMessageDigest object
		 */
		IMPORT_C CMessageDigest(const CMessageDigest& aMD);
	};

/** 
The MD2 block size (in bytes) 
*/
const TInt MD2_BLOCK=16;

/** 
The size (in bytes) of the MD2 message digest 
*/
const TInt MD2_HASH=16;

/**
 * An MD2 message digest
 */
class CMD2:public CMessageDigest

	{
	public:
		/**
		 * Creates a new MD2 object.
		 *
		 * @return	A pointer to the new CMD2 object
		 */
		IMPORT_C static CMD2* NewL(void);
		IMPORT_C CMessageDigest* ReplicateL(void);
		IMPORT_C TPtrC8 Hash(const TDesC8& aMessage);
		/** Destructor */
		IMPORT_C ~CMD2(void);
		IMPORT_C CMessageDigest* CopyL(void);
		IMPORT_C TInt BlockSize(void);
		IMPORT_C TInt HashSize(void);
		IMPORT_C void Reset(void);
		IMPORT_C void Update(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final();
	public:
		void RestoreState();
		void StoreState();
	protected:	
		/** @internalComponent */
		CMD2(void);
	};

/** 
The MD5 block size (in bytes)
*/
const TUint MD5_LBLOCK=16;

/** 
The size (in bytes) of the MD5 message digest 
*/
const TUint MD5_HASH=16;

/**
 * An MD5 message digest
 *
 * Takes a message of arbitrary length as input and produces a 128-bit message digest. 
 * 
 * The total input length of data should not be longer than 2^32 in bits(2^31 in bytes)
 * which is roughly half a gig.
 *
 */
class CMD5:public CMessageDigest
	{
	public:
		/**
		 * Creates a new MD5 object.
		 *
		 * @return	A pointer to the new CMD5 object
		 */
		IMPORT_C static CMD5* NewL(void);
		IMPORT_C CMessageDigest* ReplicateL(void);
		IMPORT_C TPtrC8 Hash(const TDesC8& aMessage);
		/** Destructor */
		IMPORT_C ~CMD5(void);
		IMPORT_C CMessageDigest* CopyL(void);
		IMPORT_C TInt BlockSize(void);
		IMPORT_C TInt HashSize(void);
		IMPORT_C void Reset(void);
		IMPORT_C void Update(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final();
	public:
		void RestoreState();
		void StoreState();
	protected:
		/** @internalComponent */
		CMD5(void);
	};


/** 
The SHA-1 block size (in bytes) 
*/
const TUint SHA1_LBLOCK=16;

/** 
The size (in bytes) of the SHA-1 message digest 
*/
const TUint SHA1_HASH=20;

/** 
The size (in bytes) of the SHA message digest 
*/
const TUint SHA_HASH=SHA1_HASH;

/**
 * A SHA-1 message digest
 */
class CSHA1:public CMessageDigest
	{
	public:
		/**
		 * Creates a new SHA-1 object.
		 *
		 * @return	A pointer to the new SHA-1 object
		 */
		IMPORT_C static CSHA1* NewL(void);
		IMPORT_C CMessageDigest* ReplicateL(void);
		IMPORT_C TPtrC8 Hash(const TDesC8& aMessage);
		/** Destructor */
		IMPORT_C ~CSHA1(void);
		IMPORT_C CMessageDigest* CopyL(void);
		IMPORT_C TInt BlockSize(void);
		IMPORT_C TInt HashSize(void);
		IMPORT_C void Reset(void);
		IMPORT_C void Update(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final();
	public:
		void RestoreState();
		void StoreState();
	protected:
		/** @internalComponent */
		CSHA1(void);
	};

enum TSH2Algo
	{
	E224Bit,
	E256Bit,
	E384Bit,
	E512Bit
	};
	
/**
 * A SHA-2 message digest
 * 
 * SHA-2 is comprised of SHA-224, SHA256, SHA384 and SHA512
 */
class CSHA2 : public CMessageDigest
	{
	public:
		/**
		 * Creates a new SHA-1 object.
		 *
		 * @return	A pointer to the new SHA-1 object
		 */
		IMPORT_C static CSHA2* NewL(TSH2Algo aAlgorithmId);
		IMPORT_C static CSHA2* NewLC(TSH2Algo aAlgorithmId);
		/** Destructor */
		IMPORT_C ~CSHA2(void);
	public:
		void RestoreState();
		void StoreState();		
	protected:
		/** @internalComponent */
		CSHA2(void);
	};	


/**
 * A SHA message digest
 *
 * @deprecated Replaced by CSHA1
 */
class CSHA:public CMessageDigest
	{
	public:
		/**
		 * Creates a new SHA object.
		 *
		 * @return	A pointer to the new SHA object
		 */
		IMPORT_C static CSHA* NewL(void);
		IMPORT_C CMessageDigest* ReplicateL(void);
		IMPORT_C TPtrC8 Hash(const TDesC8& aMessage);
		/** Destructor */
		IMPORT_C ~CSHA(void);
		IMPORT_C CMessageDigest* CopyL(void);
		IMPORT_C TInt BlockSize(void);
		IMPORT_C TInt HashSize(void);
		IMPORT_C void Reset(void);
		IMPORT_C void Update(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final();
	public:
		void RestoreState();
		void StoreState();
	};

/**
 * This is the maximum block size currently supported by HMAC implementation.
 */ 
const TUint KMaxBlockSize=128;

/**
 * An HMAC (Hashed Message Authentication Code)
 */
class CHMAC:public CMessageDigest

	{
	public:
		/**
		 * Creates a new HMAC object from a specified type of message digest 
		 * and authentication key.
		 * 
		 * @param aKey		Authentication key.
		 * @param aDigest	A message digest to construct the HMAC from.
		 * @return			A pointer to the new CHMAC object. 
		 *					The resulting HMAC object takes ownership of aDigest
		 *					and is responsible for its deletion.
		 */
		IMPORT_C static CHMAC* NewL(const TDesC8& aKey,CMessageDigest* aDigest);
		IMPORT_C CMessageDigest* ReplicateL(void);
		IMPORT_C TPtrC8 Hash(const TDesC8& aMessage);
		/** Destructor */
		IMPORT_C ~CHMAC(void);
		IMPORT_C CMessageDigest* CopyL(void);
		IMPORT_C TInt BlockSize(void);
		IMPORT_C TInt HashSize(void);
		IMPORT_C void Reset(void);
		IMPORT_C void Update(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final();
	public:
		void RestoreState();
		void StoreState();
	protected:
		/** @internalComponent */
		CHMAC(void);
		/** @internalComponent */
		CHMAC(CMessageDigest* aDigest);
		/** @internalComponent */
		CHMAC(const CHMAC& aHMAC);
		/** @internalComponent */
		void InitialiseL(const TDesC8& aKey);
		
	private:
		void InitBlockSizeL();

	private:
		CMessageDigest* iDigest;
		TBuf8<KMaxBlockSize> iInnerPad;
		TBuf8<KMaxBlockSize> iOuterPad;
		TBuf8<KMaxBlockSize> iInnerPadCopy;
		TBuf8<KMaxBlockSize> iOuterPadCopy;		
		TInt iBlockSize;

	};

/** 
The MD4 block size (in bytes)
*/
const TUint MD4_LBLOCK=16;

/** 
The size (in bytes) of the MD4 message digest 
*/
const TUint MD4_HASH=16;

/**
 * An MD4 message digest Algorithm.
 * Takes a message of arbitrary length as input and produces a 128-bit message digest. 
 * 
 * The total input length of data should not be longer than 2^32 in bits(2^31 in bytes)
 * which is roughly half a gig.
 *
 */
class CMD4:public CMessageDigest
	{
	public:
		/**
		 * Creates a new MD4 object.
		 *
		 * @return	A pointer to the new CMD4 object
		 */
		IMPORT_C static CMD4* NewL(void);
		IMPORT_C CMessageDigest* ReplicateL(void);
		IMPORT_C TPtrC8 Hash(const TDesC8& aMessage);
		/** Destructor */
		IMPORT_C ~CMD4(void);
		IMPORT_C CMessageDigest* CopyL(void);
		IMPORT_C TInt BlockSize(void);
		IMPORT_C TInt HashSize(void);
		IMPORT_C void Reset(void);
		IMPORT_C void Update(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final(const TDesC8& aMessage);
		IMPORT_C TPtrC8 Final();
	public:
		virtual void RestoreState();
		virtual void StoreState();
	protected:
		/** @internalComponent */
		CMD4(void);
	};


/**
 *	Factory to create a CMessageDigest derived object according to the identity of the hash algorithm.
 */
class CMessageDigestFactory : public CBase
{
public:
	/**
	 *Creates a CMessageDigest derived object according to the specified type of hash algorithm.
	 *
	 * @param aHashId	The identity of the hash algorithm
	 * @return			A pointer to a CMessageDigest object
	 */
	IMPORT_C static CMessageDigest* NewDigestL(CMessageDigest::THashId aHashId);

	/**
	 * Creates a CMessageDigest derived object according to the specified type of hash algorithm.
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aHashId	The identity of the hash algorithm
	 * @return			A pointer to a CMessageDigest object
	 */
	IMPORT_C static CMessageDigest* NewDigestLC(CMessageDigest::THashId aHashId);

	/**
	 * Creates a CMessageDigest derived object according to the specified type of hash algorithm
	 * and authentication key.
	 *
	 * @param aHashId	The identity of the hash algorithm
	 * @param aKey		The authentication key 
	 * @return			A pointer to a CMessageDigest object
	 */
	IMPORT_C static CMessageDigest* NewHMACL(CMessageDigest::THashId aHashId, const TDesC8& aKey);

	/**
	 * Creates a CMessageDigest derived object according to the specified type of hash algorithm
	 * and authentication key.
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aHashId	The identity of the hash algorithm
	 * @param aKey		The authentication key 
	 * @return			A pointer to a CMessageDigest object
	 */
	IMPORT_C static CMessageDigest* NewHMACLC(CMessageDigest::THashId aHashId, const TDesC8& aKey);
};

#endif // __HASH_H__
