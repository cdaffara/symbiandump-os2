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
* ** IMPORTANT ** API's in this file are published to 3rd party developers via the 
* Symbian website. Changes to these API's should be treated as PublishedAll API changes and the Security TA should be consulted.
* Asymmetric keys implementation
*
*/


/**
 @file 
 @publishedAll
 @released 
*/
 
#ifndef __ASYMMETRICKEYS_H__
#define __ASYMMETRICKEYS_H__

#include <e32base.h>
#include <random.h>
#include <bigint.h>

/** 
* Defines the various ways of representing supported RSA private keys.
* 
*/
enum TRSAPrivateKeyType 
	{
	/** 
	 * Standard type of RSA private key
	 * 
	 * This consists of the modulus (n) and decryption exponent (d).
	 */
	EStandard,
	/** 
	 * CRT (Chinese Remainder Theorem) type of RSA private key
	 *
	 * This consists of the the first factor (p), the second factor (q), 
	 * the first factor's CRT exponent (dP), the second factor's CRT exponent (dQ),
	 * and the (first) CRT coefficient (qInv). The two factors, p and q, are the
	 * first two prime factors of the RSA modulus, n.
	 */
	EStandardCRT
	//We may support types like this in the future (currently these are a patent
	//minefield):
	//EMulti, //multi prime version of EStandard
	//EMultiCRT //multi prime version of EStandardCRT
	};

/** 
* Concrete class representing the parameters common to both an RSA public and
* private key.
* 
* See ANSI X9.31 and RSA PKCS#1
*
*/
class CRSAParameters : public CBase
	{
public:
	/** 
	 * Gets the RSA parameter, n (the modulus)
	 *
	 * @return	The RSA parameter, n
	 */
	IMPORT_C const TInteger& N(void) const;
	
	/** Destructor */
	IMPORT_C virtual ~CRSAParameters(void);
protected:
	/** 
	 * Constructor 
	 *
	 * @param aN	The RSA parameter, n (the modulus)
	 */
	IMPORT_C CRSAParameters(RInteger& aN);
	
	/** Default constructor */
	IMPORT_C CRSAParameters(void);
protected:
	/** The RSA modulus, n, a positive integer */
	RInteger iN;
private:
	CRSAParameters(const CRSAParameters&);
	CRSAParameters& operator=(const CRSAParameters&);
	};

/** 
* Representation of an RSA public key.  
* 
* An RSA public key is identified by its modulus (n) and its encryption exponent
* (e).
* 
*/
class CRSAPublicKey : public CRSAParameters
	{
public:
	/**
	 * Creates a new CRSAPublicKey object from a specified 
	 * modulus and encryption exponent.
	 * 
	 * @param aN	The RSA parameter, n (the modulus)
	 * @param aE	The RSA parameter, e (the encryption exponent)
	 * @return		A pointer to a new CRSAPublicKey object
	 *
	 * @leave KErrArgument	If either aN or aE are not positive integers,
	 *						and releases ownership. 
	 */
	IMPORT_C static CRSAPublicKey* NewL(RInteger& aN, RInteger& aE);

	/**
	 * Creates a new CRSAPublicKey object from a specified 
	 * modulus and encryption exponent.
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aN	The RSA parameter, n (the modulus)
	 * @param aE	The RSA parameter, e (the encryption exponent)
	 * @return		A pointer to a new CRSAPublicKey object
	 * 
	 * @leave KErrArgument	If either aN or aE are not positive integers,
	 *	 					and releases ownership. 
	 */
	IMPORT_C static CRSAPublicKey* NewLC(RInteger& aN, RInteger& aE);

	/** 
	 * Gets the RSA parameter, e (the encryption exponent)
	 *
	 * @return	The RSA parameter, e
	 */
	IMPORT_C const TInteger& E(void) const;
	
	/** Destructor */
	IMPORT_C virtual ~CRSAPublicKey(void);
protected:
	/**
	 * Constructor 
	 *
	 * @param aN	The RSA parameter, n (the modulus)
	 * @param aE	The RSA parameter, e (the encryption exponent)
	 */	
	IMPORT_C CRSAPublicKey(RInteger& aN, RInteger& aE);
	
	/** Default constructor */
	IMPORT_C CRSAPublicKey(void);
protected:
	/** The RSA encryption exponent, e */
	RInteger iE;
private:
	CRSAPublicKey(const CRSAPublicKey&);
	CRSAPublicKey& operator=(const CRSAPublicKey&);
	void ConstructL();
	};

/** 
* Non-exported container class for the various ways of representing an RSA
* private key.
*
* To instantiate a representation of an RSA private key, find a
* subclass of this appropriate to your key type.  
*
*/
class CRSAPrivateKey : public CRSAParameters
	{
public:
	/**
	 * Constructor
	 * 
	 * @param aKeyType	The type of the RSA private key
	 * @param aN		The RSA parameter, n (the modulus)
	 * @internalAll 
	 */
	CRSAPrivateKey(const TRSAPrivateKeyType aKeyType, RInteger& aN);
public:
	/**
	 * Gets the type of RSA private key
	 *
	 * @return	The RSA private key type
	 */
	inline const TRSAPrivateKeyType PrivateKeyType() const {return (iKeyType);};
protected:
	/** The type of the RSA private key */
	const TRSAPrivateKeyType iKeyType;
private:
	CRSAPrivateKey(const CRSAPrivateKey&);
	CRSAPrivateKey& operator=(const CRSAPrivateKey&);
	};

/** 
* The 'classical' representation of a RSA private key.
* 
* Such a private key is composed of a modulus (n) and a decryption exponent (d).
*   
*/
class CRSAPrivateKeyStandard : public CRSAPrivateKey
	{
public:
	/**
	 * Creates a new CRSAPrivateKeyStandard object from a specified 
	 * modulus and decryption exponent.
	 * 
	 * @param aN	The RSA parameter, n (the modulus)
	 * @param aD	The RSA parameter, d (the decryption exponent)
	 * @return		A pointer to a new CRSAPrivateKeyStandard object
	 * 
	 * @leave KErrArgument	If either aN or aD are not positive integers,
	 *	 					and releases ownership. 
	 */
	IMPORT_C static CRSAPrivateKeyStandard* NewL(RInteger& aN, RInteger& aD);

	/**
	 * Creates a new CRSAPrivateKeyStandard object from a specified 
	 * modulus and decryption exponent.
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aN	The RSA parameter, n (the modulus)
	 * @param aD	The RSA parameter, d (the decryption exponent)
	 * @return		A pointer to a new CRSAPrivateKeyStandard object
	 * 
	 * @leave KErrArgument	If either aN or aD are not positive integers,
	 *	 					and releases ownership. 
	 */
	IMPORT_C static CRSAPrivateKeyStandard* NewLC(RInteger& aN, RInteger& aD);

	/** 
	 * Gets the RSA parameter, d (the decryption exponent)
	 *
	 * @return	The RSA parameter, d
	 */
	IMPORT_C const TInteger& D(void) const;

	/** Destructor */
	IMPORT_C virtual ~CRSAPrivateKeyStandard(void);
protected:
	/** 
	 * Constructor
	 * 
	 * @param aN	The RSA parameter, n (the modulus)
	 * @param aD	The RSA parameter, d (the decryption exponent)
	 */	 
	IMPORT_C CRSAPrivateKeyStandard(RInteger& aN, RInteger& aD);
protected:
	/** The RSA decryption exponent, d */
	RInteger iD;
private:
	CRSAPrivateKeyStandard(const CRSAPrivateKeyStandard&);
	CRSAPrivateKeyStandard& operator=(const CRSAPrivateKeyStandard&);
	void ConstructL();
	};

/** 
* An alternate representation of an RSA private key providing significant
* speed enhancements through its use of the Chinese Remainder Theorem (CRT).
*
* Here, a private key is represented by a modulus (n), the two prime factors of
* the modulus (p, q), p's CRT exponent (dP), q's CRT exponent (dQ), and the CRT
* coefficient (qInv).  See PKCS#1 at http://www.rsasecurity.com/rsalabs/pkcs/
* for more information.
*
*/
class CRSAPrivateKeyCRT : public CRSAPrivateKey
	{
public:
	/**
	 * Creates a new CRSAPrivateKeyCRT object from a specified 
	 * modulus and decryption exponent.
	 * 
	 * @param iN	The RSA parameter, n (the modulus)
	 * @param aP	The RSA parameter, p (the first factor)
	 * @param aQ	The RSA parameter, q (the second factor)
	 * @param aDP	The RSA parameter, dP (the first factor's CRT exponent)
	 * @param aDQ	The RSA parameter, dQ (the second factor's CRT exponent)
	 * @param aQInv	The RSA parameter, qInv (the CRT coefficient)
	 * @return		A pointer to a new CRSAPrivateKeyCRT object
	 * 
	 * @leave KErrArgument	If any of the parameters are not positive integers,
	 *	 					and releases ownership. 
	 */
	IMPORT_C static CRSAPrivateKeyCRT* NewL(RInteger& iN, RInteger& aP, 
		RInteger& aQ, RInteger& aDP, RInteger& aDQ, RInteger& aQInv);

	/**
	 * Creates a new CRSAPrivateKeyCRT object from a specified 
	 * modulus and decryption exponent.
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param iN	The RSA parameter, n (the modulus)
	 * @param aP	The RSA parameter, p (the first factor)
	 * @param aQ	The RSA parameter, q (the second factor)
	 * @param aDP	The RSA parameter, dP (the first factor's CRT exponent)
	 * @param aDQ	The RSA parameter, dQ (the second factor's CRT exponent)
	 * @param aQInv	The RSA parameter, qInv (the CRT coefficient)
	 * @return		A pointer to a new CRSAPrivateKeyCRT object
	 * 
	 * @leave KErrArgument	If any of the parameters are not positive integers,
	 *	 					and releases ownership. 
	 */
	IMPORT_C static CRSAPrivateKeyCRT* NewLC(RInteger& iN, RInteger& aP, 
		RInteger& aQ, RInteger& aDP, RInteger& aDQ, RInteger& aQInv);

	/** Destructor */
	IMPORT_C virtual ~CRSAPrivateKeyCRT(void);
	
	/**
	 * Gets the RSA parameter, p (the first factor) 
	 *
	 * @return	The first factor
	 */
	IMPORT_C const TInteger& P(void) const;
	
	/**
	 * Gets the RSA parameter, q (the second factor) 
	 *
	 * @return	The second factor
	 */
	IMPORT_C const TInteger& Q(void) const;
	
	/**
	 * Gets the RSA parameter, dP (the first factor's CRT exponent) 
	 *
	 * @return	The first factor's CRT exponent
	 */
	IMPORT_C const TInteger& DP(void) const;
	
	/**
	 * Gets the RSA parameter, dQ (the second factor's CRT exponent) 
	 *
	 * @return	The second factor's CRT exponent
	 */
	IMPORT_C const TInteger& DQ(void) const;
	
	/**
	 * Gets the RSA parameter, qInv (the CRT coefficient) 
	 *
	 * @return	The CRT coefficient
	 */
	IMPORT_C const TInteger& QInv(void) const;
protected:
	/**
	 * Constructor
	 * 
	 * @param aN	The RSA parameter, n (the modulus)
	 * @param aP	The RSA parameter, p (the first factor)
	 * @param aQ	The RSA parameter, q (the second factor)
	 * @param aDP	The RSA parameter, dP (the first factor's CRT exponent)
	 * @param aDQ	The RSA parameter, dQ (the second factor's CRT exponent)
	 * @param aQInv	The RSA parameter, qInv (the CRT coefficient)
	 */
	IMPORT_C CRSAPrivateKeyCRT(RInteger& aN, RInteger& aP, RInteger& aQ, 
		RInteger& aDP, RInteger& aDQ, RInteger& aQInv);
protected:
	/** The RSA parameter, p, which is the first factor */
	RInteger iP;
	/** The RSA parameter, q, which is the second factor */
	RInteger iQ;
	/** The RSA parameter, dP, which is the first factor's CRT exponent */
	RInteger iDP;
	/** The RSA parameter, dQ, which is the second factor's CRT exponent */
	RInteger iDQ;
	/** The RSA parameter, qInv, which is the CRT coefficient */
	RInteger iQInv;
private:
	CRSAPrivateKeyCRT(const CRSAPrivateKeyCRT&);
	CRSAPrivateKeyCRT& operator=(const CRSAPrivateKeyCRT&);
	void ConstructL();
	};

/** 
* This class is capable of generating an RSA public/private key pair.
*
* By default, it generates 2 prime (standard) CRT private keys.
*
*/
class CRSAKeyPair : public CBase
	{
public:
	/**
	 * Creates a new RSA key pair
	 * 
	 * @param aModulusBits	The length of the modulus, n (in bits)
	 * @param aKeyType		The type of the RSA key
	 * @return				A pointer to a new CRSAKeyPair object
	 * 
	 * @leave KErrNotSupported	If the type of RSA key is not supported
	 */
	IMPORT_C static CRSAKeyPair* NewL(TUint aModulusBits, 
		TRSAPrivateKeyType aKeyType = EStandardCRT);

	/**
	 * Creates a new RSA key pair
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aModulusBits	The length of the modulus, n (in bits)
	 * @param aKeyType		The type of the RSA key
	 * @return				A pointer to a new CRSAKeyPair object
	 * 
	 * @leave KErrNotSupported	If the type of RSA key is not supported
	 */
	IMPORT_C static CRSAKeyPair* NewLC(TUint aModulusBits, 
		TRSAPrivateKeyType aKeyType = EStandardCRT);
	
	/** 
	 * Gets the RSA public key
	 *
	 * @return	A CRSAPublicKey object
	 */
	IMPORT_C const CRSAPublicKey& PublicKey(void) const;
	
	/** 
	 * Gets the RSA private key
	 *
	 * @return	A CRSAPrivateKey object
	 */
	IMPORT_C const CRSAPrivateKey& PrivateKey(void) const;
	
	/** The destructor frees all resources owned by the object, prior to its destruction. */
	IMPORT_C virtual ~CRSAKeyPair(void);
protected:
	/** Default destructor */
	IMPORT_C CRSAKeyPair(void);
protected:
	/** The RSA public key */
	CRSAPublicKey* iPublic;
	/** The RSA private key */
	CRSAPrivateKey* iPrivate;
private:
	void ConstructL(TUint aModulusBits, TRSAPrivateKeyType aKeyType, 
		TUint aPublicExponent);
	CRSAKeyPair(const CRSAKeyPair&);
	CRSAKeyPair& operator=(const CRSAKeyPair&);
	};

/** 
* Representation of the parameters used to generate the primes in a
* CDSAParameters object.
* 
* Given such a certificate, one can ensure that the DSA
* primes contained in CDSAParameters were generated correctly.
* 
* @see CDSAParameters::ValidatePrimesL() 
* 
*/
class CDSAPrimeCertificate : public CBase
	{
public:
	/** 
	 * Creates a new DSA prime certificate from a specified 
	 * seed and counter value.
	 * 
	 * @param aSeed		The seed from a DSA key generation process
	 * @param aCounter	The counter value from a DSA key generation process
	 * @return			A pointer to a new CDSAPrimeCertificate object
	 */
	IMPORT_C static CDSAPrimeCertificate* NewL(const TDesC8& aSeed, 
		TUint aCounter);

	/** 
	 * Creates a new DSA prime certificate from a specified 
	 * seed and counter value.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aSeed		The seed from a DSA key generation process
	 * @param aCounter	The counter value from a DSA key generation process
	 * @return			A pointer to a new CDSAPrimeCertificate object
	 */
	IMPORT_C static CDSAPrimeCertificate* NewLC(const TDesC8& aSeed,
		TUint aCounter);

	/**
	 * Gets the seed of the DSA prime certificate
	 *
	 * @return	The seed
	 */ 
	IMPORT_C const TDesC8& Seed(void) const;
	
	/**
	 * Gets the counter value of the DSA prime certificate
	 *
	 * @return	The counter's value
	 */
	IMPORT_C TUint Counter(void) const;
	
	/** Destructor */
	IMPORT_C virtual ~CDSAPrimeCertificate(void);
protected:
	/** 
	 * Constructor 
	 *
	 * @param aCounter	The DSA key generation counter
	 */
	IMPORT_C CDSAPrimeCertificate(TUint aCounter);

	/** Default constructor */
	IMPORT_C CDSAPrimeCertificate(void);
	/** @internalAll */
	void ConstructL(const TDesC8& aSeed);
protected:
	/** The DSA key generation seed */
	const HBufC8* iSeed;
	/** The DSA key generation counter */
	TUint iCounter;
private:
	CDSAPrimeCertificate(const CDSAPrimeCertificate&);
	CDSAPrimeCertificate& operator=(const CDSAPrimeCertificate&);
	};

/** 
* Concrete class representing the parameters common to both a DSA public and
* private key. 
*
* See FIPS 186-2, Digital Signature Standard
* 
*/
class CDSAParameters : public CBase
	{
public:
	/**
	 * Gets the DSA parameter, p (the prime)
	 * 
	 * @return	The DSA parameter, p
	 */
	IMPORT_C const TInteger& P(void) const;

	/**
	 * Gets the DSA parameter, q (the subprime)
	 * 
	 * @return	The DSA parameter, q
	 */
	IMPORT_C const TInteger& Q(void) const;

	/**
	 * Gets the DSA parameter, g (the base)
	 * 
	 * @return	The DSA parameter, g
	 */
	IMPORT_C const TInteger& G(void) const;

	/**
	 * Validates the primes regenerated from a DSA prime certificate 
	 *
	 * @param aCert	The DSA prime certificate that contains the seed and 
	 *				counter value from a DSA key generation process
	 * @return		Whether or not the primes are valid	
	 */
	IMPORT_C TBool ValidatePrimesL(const CDSAPrimeCertificate& aCert) const;

	/** 
	 * Whether or not the prime is of a valid length 
	 * 
	 * It is valid if the length of the prime modulus is between KMinPrimeLength
	 * and KMaxPrimeLength bits, and the prime is a multiple of KPrimeLengthMultiple. 
	 *
	 * @param aPrimeBits	The prime modulus
	 * @return				ETrue, if within the constraints; EFalse, otherwise.
	 */
	IMPORT_C static TBool ValidPrimeLength(TUint aPrimeBits);
	
	/** Destructor */
	IMPORT_C virtual ~CDSAParameters(void);

	/** 
	 * Creates a new DSA parameters object from a specified 
	 * prime, subprime, and base.
	 * 
	 * @param aP	The DSA parameter, p (the prime)
	 * @param aQ	The DSA parameter, g (the subprime)
	 * @param aG	The DSA parameter, g (the base)
	 * @return		A pointer to a new CDSAParameters object
	 */
	IMPORT_C static CDSAParameters* NewL(RInteger& aP, RInteger& aQ, 
		RInteger& aG);
public:
	/** @internalAll */
	static TBool GeneratePrimesL(const TDesC8& aSeed, TUint& aCounter, 
		RInteger& aP, TUint aL, RInteger& aQ, TBool aUseInputCounter=EFalse);
protected:
	/** 
	 * Constructor
	 * 
	 * @param aP	The DSA parameter, p (the prime)
	 * @param aQ	The DSA parameter, g (the subprime)
	 * @param aG	The DSA parameter, g (the base)
	 */
	IMPORT_C CDSAParameters(RInteger& aP, RInteger& aQ, RInteger& aG);
	
	/** Default constructor */
	IMPORT_C CDSAParameters(void);
protected:
	/** 
	 * The DSA parameter, p (the prime).
	 * 
	 * A prime modulus whose length is between KMinPrimeLength and KMaxPrimeLength bits,
	 * and is a multiple of KPrimeLengthMultiple. 
	 */
	RInteger iP;
	
	/** 
	 * The DSA parameter, q (the subprime)
	 * 
	 * This is a 160-bit prime divisor of <code>p-1</code>. 
	 */
	RInteger iQ;
	
	/** 
	 * The DSA parameter, g (the base)
	 * 
	 * <code>g = h^((p-1)/q) mod p</code>,
	 * 
	 * where h is any integer less than <code>p-1</code> such that <code>g &gt; 1</code> 
	 */
	RInteger iG;
private:
	CDSAParameters(const CDSAParameters&);
	CDSAParameters& operator=(const CDSAParameters&);
	};

/**
* Representation of a DSA public key.  
*
*/
class CDSAPublicKey : public CDSAParameters
	{
public:
	/** 
	 * Creates a new DSA public key object from a specified
	 * primes, base, and public key. 
	 * 
	 * @param aP	The DSA parameter, p (the prime)
	 * @param aQ	The DSA parameter, q (the subprime)
	 * @param aG	The DSA parameter, g (the base)
	 * @param aY	The DSA parameter, y (the public key)
	 * @return		A pointer to a new CDSAPublicKey object
	 */
	IMPORT_C static CDSAPublicKey* NewL(RInteger& aP, RInteger& aQ, 
		RInteger& aG, RInteger& aY);

	/** 
	 * Creates a new DSA public key object from a specified
	 * primes, base, and public key. 
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aP	The DSA parameter, p (the prime)
	 * @param aQ	The DSA parameter, q (the subprime)
	 * @param aG	The DSA parameter, g (the base)
	 * @param aY	The DSA parameter, y (the public key)
	 * @return		A pointer to a new CDSAPublicKey object
	 */
	IMPORT_C static CDSAPublicKey* NewLC(RInteger& aP, RInteger& aQ, 
		RInteger& aG, RInteger& aY);

	/**
	 * Gets the DSA parameter, y (the public key)
	 *
	 * @return	The DSA parameter, y
	 */
	IMPORT_C const TInteger& Y(void) const;

	/** Destructor */
	IMPORT_C virtual ~CDSAPublicKey(void);
protected:
	/** 
	 * Constructor
	 * 
	 * @param aP	The DSA parameter, p (the prime)
	 * @param aQ	The DSA parameter, q (the subprime)
	 * @param aG	The DSA parameter, g (the base)
	 * @param aY	The DSA parameter, y (the public key)
	 */
	IMPORT_C CDSAPublicKey(RInteger& aP, RInteger& aQ, RInteger& aG, 
		RInteger& aY);
	
	/** Default constructor */
	IMPORT_C CDSAPublicKey(void);
protected:
	/** 
	 * The DSA parameter, y, which is the public key 
	 *
	 * <code>y = g^x mod p</code>
	 */
	RInteger iY;
private:
	CDSAPublicKey(const CDSAPublicKey&);
	CDSAPublicKey& operator=(const CDSAPublicKey&);
	};

/** 
* Representation of a DSA private key.  
* 
*/
class CDSAPrivateKey : public CDSAParameters
	{
public:
	/** 
	 * Creates a new DSA private key object from a specified
	 * primes, base, and private key. 
	 * 
	 * @param aP	The DSA parameter, p (the prime)
	 * @param aQ	The DSA parameter, q (the subprime)
	 * @param aG	The DSA parameter, g (the base)
	 * @param aX	The DSA parameter, x (the private key)
	 * @return		A pointer to a new CDSAPrivateKey object
	 */
	IMPORT_C static CDSAPrivateKey* NewL(RInteger& aP, RInteger& aQ, 
		RInteger& aG, RInteger& aX);

	/** 
	 * Creates a new DSA private key object from a specified
	 * primes, base, and private key. 
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aP	The DSA parameter, p (the prime)
	 * @param aQ	The DSA parameter, q (the subprime)
	 * @param aG	The DSA parameter, g (the base)
	 * @param aX	The DSA parameter, x (the private key)
	 * @return		A pointer to a new CDSAPrivateKey object
	 */
	IMPORT_C static CDSAPrivateKey* NewLC(RInteger& aP, RInteger& aQ, 
		RInteger& aG, RInteger& aX);

	/**
	 * Gets the DSA parameter, x (the private key)
	 *
	 * @return	The DSA parameter, x
	 */
	IMPORT_C const TInteger& X(void) const;

	/** Destructor */
	IMPORT_C virtual ~CDSAPrivateKey(void);
protected:
	/** 
	 * Constructor
	 * 
	 * @param aP	The DSA parameter, p (the prime)
	 * @param aQ	The DSA parameter, q (the subprime)
	 * @param aG	The DSA parameter, g (the base)
	 * @param aX	The DSA parameter, x (the private key)
	 */
	IMPORT_C CDSAPrivateKey(RInteger& aP, RInteger& aQ, RInteger& aG, 
		RInteger& aX);
		
	/** Default constructor */
	IMPORT_C CDSAPrivateKey(void);
protected:
	/** 
	 * The DSA parameter, x, which is the private key 
	 *
	 * A pseudorandomly generated integer whose value is between 0 and q.
	*/
	RInteger iX;
private:
	CDSAPrivateKey(const CDSAPrivateKey&);
	CDSAPrivateKey& operator=(const CDSAPrivateKey&);
	};

/** 
* This class is capable of generating a DSA public/private key pair.
* 
*/
class CDSAKeyPair : public CBase
	{
public:
	/** 
	 * Creates a new DSA key pair and also a DSA prime certificate
	 * 
	 * @param aSize	The length (in bits) of the DSA parameter, p (the prime)
	 * @return		A pointer to a new CDSAKeyPair object
	 */
	IMPORT_C static CDSAKeyPair* NewL(TUint aSize);

	/** 
	 * Creates a new DSA key pair and also a DSA prime certificate
	 * 
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aSize	The length (in bits) of the DSA parameter, p (the prime)
	 * @return		A pointer to a new CDSAKeyPair object
	 */
	IMPORT_C static CDSAKeyPair* NewLC(TUint aSize);
	
	/** 
	 * Gets the DSA public key
	 *
	 * @return	The DSA public key object
	 */
	IMPORT_C const CDSAPublicKey& PublicKey(void) const;
	
	/** 
	 * Gets the DSA private key
	 *
	 * @return	The DSA private key object
	 */
	IMPORT_C const CDSAPrivateKey& PrivateKey(void) const;
	
	/** 
	 * Gets the DSA prime certificate (i.e., the seed and counter)
	 *
	 * @return	The DSA prime certificate object
	 */
	IMPORT_C const CDSAPrimeCertificate& PrimeCertificate(void) const;
	
	/** The destructor frees all resources owned by the object, prior to its destruction. */
	IMPORT_C virtual ~CDSAKeyPair(void);
protected:
	/** Default constructor */
	IMPORT_C CDSAKeyPair(void);
protected:
	/** The DSA public key */
	CDSAPublicKey* iPublic;
	/** The DSA private key */
	CDSAPrivateKey* iPrivate;
	/** The DSA prime certificate */
	CDSAPrimeCertificate* iPrimeCertificate;
private:
	void ConstructL(TUint aSize);
	CDSAKeyPair(const CDSAKeyPair&);
	CDSAKeyPair& operator=(const CDSAKeyPair&);
	};

/** 
* Concrete class representing the parameters common to both 
* a Diffie-Hellman (DH) public and private key.  
* 
*/
class CDHParameters : public CBase
	{
public:
	/**
	 * Gets the DH parameter, n
	 *
	 * @return	An integer representing the DH parameter, n
	 */
	IMPORT_C const TInteger& N(void) const;

	/**
	 * Gets the DH parameter, g
	 *
	 * @return	An integer representing the DH parameter, g
	 */
	IMPORT_C const TInteger& G(void) const;
	
	/** Destructor */
	IMPORT_C virtual ~CDHParameters(void);
protected:
	/** 
	 * Constructor
	 * 
	 * @param aN	The DH parameter, n
	 * @param aG	The DH parameter, g
	 */
	IMPORT_C CDHParameters(RInteger& aN, RInteger& aG);
	
	/** Default constructor */
	IMPORT_C CDHParameters(void);
protected:
	/**
	 * The DH parameter, n (a prime number)
	 * 
	 * <code>X = g^x mod n</code> (note the case sensitivity)
	 */
	RInteger iN;
	/** 
	 * The DH parameter, g (the generator) 
	 *
	 * <code>X = g^x mod n</code> (note the case sensitivity)
	 */
	RInteger iG;
private:
	CDHParameters(const CDHParameters&);
	CDHParameters& operator=(const CDHParameters&);
	};

/** 
* Representation of a Diffie-Hellman (DH) public key.  
* 
*/
class CDHPublicKey : public CDHParameters
	{
public:
	/** 
	 * Creates a new DH public key from a specified 
	 * large prime, generator, and random large integer.
	 * 
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param aX	The DH value, X
	 * @return		A pointer to a new CDHPublicKey object
	 */
	IMPORT_C static CDHPublicKey* NewL(RInteger& aN, RInteger& aG, 
		RInteger& aX);

	/** 
	 * Creates a new DH public key from a specified 
	 * large prime, generator, and random large integer.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param aX	The DH value, X
	 * @return		A pointer to a new CDHPublicKey object
	 */
	IMPORT_C static CDHPublicKey* NewLC(RInteger& aN, RInteger& aG, 
		RInteger& aX);
	
	/** 
	 * Gets the DH value, X
	 * 
	 * @return	The DH value, X
	 */	
	IMPORT_C const TInteger& X(void) const;

	/** Destructor */
	IMPORT_C virtual ~CDHPublicKey(void);
protected:
	/** 
	 * Constructor
	 * 
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param aX	The DH value, X
	 */
	IMPORT_C CDHPublicKey(RInteger& aN, RInteger& aG, RInteger& aX);

	/** Constructor */
	IMPORT_C CDHPublicKey(void);
protected:
	/** 
	 * The DH value, X
	 *
	 * <code>X = g^x mod n</code> (note the case sensitivity)
	 */
	RInteger iX;
private:
	CDHPublicKey(const CDHPublicKey&);
	CDHPublicKey& operator=(const CDHPublicKey&);
	};

/** 
* Representation of a Diffie-Hellman (DH) private key.  
* 
*/
class CDHPrivateKey : public CDHParameters
	{
public:
	/** 
	 * Creates a new DH private key from a specified 
	 * large prime, generator, and random large integer.
	 * 
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param ax	The DH value, x (a random large integer)
	 * @return		A pointer to a new CDHPrivateKey object
	 */
	IMPORT_C static CDHPrivateKey* NewL(RInteger& aN, RInteger& aG, 
		RInteger& ax);

	/** 
	 * Creates a new DH private key from a specified 
	 * large prime, generator, and random large integer.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 * 
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param ax	The DH value, x (a random large integer)
	 * @return		A pointer to a new CDHPrivateKey object
	 */
	IMPORT_C static CDHPrivateKey* NewLC(RInteger& aN, RInteger& aG, 
		RInteger& ax);
	
	/** 
	 * Gets the DH value, x, which is a random large integer.
	 * 
	 * @return	The DH value, x
	 */	
	IMPORT_C const TInteger& x(void) const;
	
	/** Destructor */
	IMPORT_C virtual ~CDHPrivateKey(void);
protected:
	/** 
	 * Constructor
	 * 
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param ax	The DH value, x (a random large integer)
	 */
	IMPORT_C CDHPrivateKey(RInteger& aN, RInteger& aG, RInteger& ax);
	
	/** Constructor */
	IMPORT_C CDHPrivateKey(void);
protected:
	/** 
	 * The DH value, x, which is a random large integer.
	 *
	 * <code>X = g^x mod n</code> (note the case sensitivity)
	 */
	RInteger ix;
private:
	CDHPrivateKey(const CDHPrivateKey&);
	CDHPrivateKey& operator=(const CDHPrivateKey&);
	};

/** 
* This class is capable of generating a Diffie-Hellman (DH) public/private key pair.
* 
*/
class CDHKeyPair : public CBase
	{
public:
	/**
	 * Creates a new DH key pair from a random large integer,
	 * and a specified large prime and generator.
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @return		A pointer to a new CDHKeyPair object
	 * 
	 * @leave KErrArgument	If aG is out of bounds 
	 */
	IMPORT_C static CDHKeyPair* NewL(RInteger& aN, RInteger& aG);

	/**
	 * Creates a new DH key pair from a random large integer,
	 * and a specified large prime and generator.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @return		A pointer to a new CDHKeyPair object
	 * 
	 * @leave KErrArgument	If aG is out of bounds 
	 */
	IMPORT_C static CDHKeyPair* NewLC(RInteger& aN, RInteger& aG);

	/**
	 * Creates a new DH key pair from a specified 
	 * large prime, generator, and random large integer.
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param ax	The DH value, x (a random large integer)
	 * @return		A pointer to a new CDHKeyPair object
	 * 
	 * @leave KErrArgument	If either aG or ax are out of bounds 
	 */
	IMPORT_C static CDHKeyPair* NewL(RInteger& aN, RInteger& aG, RInteger& ax);

	/**
	 * Creates a new DH key pair from a specified 
	 * large prime, generator, and random large integer.
	 *
	 * The returned pointer is put onto the cleanup stack.
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param ax	The DH value, x (a random large integer)
	 * @return		A pointer to a new CDHKeyPair object
	 * 
	 * @leave KErrArgument	If either aG or ax are out of bounds 
	 */
	IMPORT_C static CDHKeyPair* NewLC(RInteger& aN, RInteger& aG, RInteger& ax);

	/**
	 * Gets the DH public key
	 *
	 * @return	The DH public key
	 */
	IMPORT_C const CDHPublicKey& PublicKey(void) const;

	/**
	 * Gets the DH private key
	 *
	 * @return	The DH private key
	 */
	IMPORT_C const CDHPrivateKey& PrivateKey(void) const;
	
	/** The destructor frees all resources owned by the object, prior to its destruction. */
	IMPORT_C virtual ~CDHKeyPair(void);
protected:
	/** Default constructor */
	IMPORT_C CDHKeyPair(void);
	
	/** 
	 * Constructor
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 */
	IMPORT_C void ConstructL(RInteger& aN, RInteger& aG);

	/** 
	 * Constructor
	 *
	 * @param aN	The DH parameter, n (a large prime)
	 * @param aG	The DH parameter, g (the generator)
	 * @param ax	The DH value, x (a random large integer)
	 */
	IMPORT_C void ConstructL(RInteger& aN, RInteger& aG, RInteger& ax);

protected:	
	/** The DH public key */
	CDHPublicKey* iPublic;
	/** The DH private key */
	CDHPrivateKey* iPrivate;
private:
	CDHKeyPair(const CDHKeyPair&);
	CDHKeyPair& operator=(const CDHKeyPair&);
	};
#endif	//	__ASYMMETRICKEYS_H__
