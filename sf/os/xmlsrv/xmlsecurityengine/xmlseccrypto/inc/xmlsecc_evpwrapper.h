/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Methods that allows to sign and verify data.
*
*/


/* A wrapper for evp.h in openssl */

#ifndef __SYMBIANCRYPTO_EVP_WRAPPER_H__
#define __SYMBIANCRYPTO_EVP_WRAPPER_H__    

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 
#include "xmlsecc_config.h"
#include "xmlsecc_bio.h"


/************************************
 *                                  
 *            RSA Sign functions    
 *                                  
 ************************************/

/* data type compatibility for OpenSSL*/

#define	HASKEY	1
#define NOKEY	0

/** 
 * Key algorithms. 
 * EVP_PKEY_UNKNOWN  unknown algorithm
 * EVP_PKEY_RSA RSA algorithm
 * EVP_PKEY_DSA DSA algorithm
 */ 
enum sc_key_algos
{
	EVP_PKEY_UNKNOWN=0,
	EVP_PKEY_RSA,
	EVP_PKEY_DSA
};

struct ScKeyStore;
typedef struct ScKeyStore		ScKeyStore, *ScKeyStorePtr;
typedef unsigned char*			RSA;

typedef struct ScPkey
{
	int				type;
	int				load; 				// HASKEY - 1, NOKEY - 0
	unsigned int	bitsize;			// Bit size of the key
	int				duplicate;			// Indicate whether this is a duplicate copy
	char 			*name;
	ScKeyStorePtr	keyStore;
} EVP_PKEY;


/**
 * Create a new key store structure
 *
 * @param keytype type of the key
 * @param keyname name of the key
 * @return EVP_PKEY structure
 */
EVP_PKEY *sc_pkey_new(int keytype, char *keyname);

/** 
 * Symbian key store Initialization
 *
 * @return 0 if correct initialization
 * @return error code in the other hand  
 */
int sc_pkey_init();

/** 
 * Load an RSA key
 * 
 * @param pkey EVP_PKEY structure
 * @return 0 if key is loaded
 * @return -1 if key is not loaded
 * @return error code in the other hand
 */
int sc_pkey_load(EVP_PKEY *pkey);

/** 
 * Generate an RSA key
 * 
 * @param pkey EVP_KEY structure
 * @param sizeBits size of the key
 * @return 0 if key is loaded
 * @return -1 if key is not loaded
 * @return error code in the other hand
 */
int sc_pkey_generate(EVP_PKEY *pkey, unsigned int sizeBits);

/** 
 * Free the EVP_PKEY structure 
 */
void sc_pkey_free(EVP_PKEY *pkey);

/** 
 * Duplicate an EVP key
 * 
 * @param pkey EVP_KEY structure
 * @return EVP_PKEY duplicated structure
 */
EVP_PKEY *sc_pkey_duplicate(EVP_PKEY *aPKey);

/** 
 * Symbian key store shutdown process 
 */
void sc_pkey_shutdown();

/** 
 * Get the pkey size
 *
 * @param pkey EVP_KEY structure
 * @return size of the key
 */
unsigned int sc_pkey_size(EVP_PKEY *aPKey);

/** 
 * Read the private key from ASN.1 DER encoded PKCS#8 format 
 *
 * @param aBio BIO structure
 * @param aPwdCallback callback
 * @param aPwdCallbackCtx callback context
 * @return EVP_PKEY structure
 */
EVP_PKEY* d2i_PKCS8PrivateKey_bio(BIO *aBio, void *aPwdCallback, void *aPwdCallbackCtx);

/** 
 * Read the private key from Unified Key Store
 *
 * @param keyname name of the key
 * @return EVP_PKEY structure
 */
EVP_PKEY* d2i_PKCS8PrivateKey(char *keyname);

/** 
 * Read the public key from ASN.1 DER encoded format
 *
 * @param aBio BIO structure
 * @return EVP_PKEY structure
 */
EVP_PKEY* d2i_PUBKEY_bio(BIO *aBio);

#ifndef XMLSEC_NO_X509

/************************************
 *                                  
 *       X.509 related functions    
 *                                  
 ************************************/

typedef struct ScX509St
{
	char*			der;		// Certificate in ASN.1 DER format 
	unsigned int	derlen;	

} X509;

/** 
 * Set the public key info 
 *
 * @param pkey EVP_KEY structure
 * @param aCert X509 structure
 * @return 0 if operation correct
 * @return error code if operation fail
 */
int sc_pkey_setPublic(EVP_PKEY* aPKey, X509 *aCert);

#endif // XMLSEC_NO_X509

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SYMBIANCRYPTO_EVP_WRAPPER_H__ */

#define __SYMBIANCRYPTO_EVP_WRAPPER_H__    
